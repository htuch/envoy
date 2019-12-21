# API upgrade business logic.

import copy
import re

from tools.api_proto_plugin import traverse
from tools.api_proto_plugin import visitor
from tools.protoxform import options
from tools.protoxform import utils

from udpa.annotations import migrate_pb2
from google.api import annotations_pb2

ENVOY_COMMENT_WITH_TYPE_REGEX = re.compile('<envoy_api_(msg|enum_value|field|enum)_([\w\.]+)>')


class UpgradeVisitor(visitor.Visitor):
  """Visitor to generate an upgraded proto from a FileDescriptor proto.

  See visitor.Visitor for visitor method docs comments.
  """

  def __init__(self, typedb, envoy_internal_shadow):
    self._typedb = typedb
    self._envoy_internal_shadow = envoy_internal_shadow

  def _UpgradedComment(self, c):

    def UpgradeType(match):
      # We're upgrading a type within a RST anchor reference here. These are
      # stylized and match the output format of tools/protodoc. We need to do
      # some special handling of field/enum values, and also the normalization
      # that was performed in v2 for envoy.api.v2 types.
      ref_type, normalized_type_name = match.groups()
      if ref_type == 'field' or ref_type == 'enum_value':
        normalized_type_name, residual = normalized_type_name.rsplit('.', 1)
      else:
        residual = ''
      type_name = 'envoy.' + normalized_type_name
      api_v2_type_name = 'envoy.api.v2.' + normalized_type_name
      if type_name in self._typedb.types:
        type_desc = self._typedb.types[type_name]
      else:
        # We need to deal with envoy.api.* normalization in the v2 API. We won't
        # need this in v3+, so rather than churn docs, we just have this workaround.
        type_desc = self._typedb.types[api_v2_type_name]
      repl_type = type_desc.next_version_type_name[len(
          'envoy.'):] if type_desc.next_version_type_name else normalized_type_name
      return '<envoy_api_%s_%s%s>' % (ref_type, repl_type, '.' + residual if residual else '')

    return re.sub(ENVOY_COMMENT_WITH_TYPE_REGEX, UpgradeType, c)

  def _UpgradedPostMethod(self, m):
    return re.sub(r'^/v2/', '/v3alpha/', m)

  def _UpgradedType(self, t):
    if not t.startswith('.envoy'):
      return t
    type_desc = self._typedb.types[t[1:]]
    if type_desc.next_version_type_name:
      return '.' + type_desc.next_version_type_name
    return t

  def _Deprecate(self, proto, field_or_value):
    """Deprecate a field or value in a message/enum proto.

    Args:
      proto: DescriptorProto or EnumDescriptorProto message.
      field_or_value: field or value inside proto.
    """
    if self._envoy_internal_shadow:
      field_or_value.name = 'hidden_envoy_deprecated_' + field_or_value.name
    else:
      reserved = proto.reserved_range.add()
      reserved.start = field_or_value.number
      reserved.end = field_or_value.number + 1
      proto.reserved_name.append(field_or_value.name)
      options.AddHideOption(field_or_value.options)

  def _Rename(self, proto, migrate_annotation):
    """Rename a field/enum/service/message

    Args:
      proto: DescriptorProto or corresponding proto message
      migrate_annotation: udpa.annotations.MigrateAnnotation message
    """
    if migrate_annotation.rename:
      proto.name = migrate_annotation.rename
      migrate_annotation.rename = ""

  def VisitService(self, service_proto, type_context):
    upgraded_proto = copy.deepcopy(service_proto)
    for m in upgraded_proto.method:
      if m.options.HasExtension(annotations_pb2.http):
        http_options = m.options.Extensions[annotations_pb2.http]
        # TODO(htuch): figure out a more systematic approach using the type DB
        # to service upgrade.
        http_options.post = self._UpgradedPostMethod(http_options.post)
      m.input_type = self._UpgradedType(m.input_type)
      m.output_type = self._UpgradedType(m.output_type)
    return upgraded_proto

  def VisitMessage(self, msg_proto, type_context, nested_msgs, nested_enums):
    upgraded_proto = copy.deepcopy(msg_proto)
    if upgraded_proto.options.deprecated:
      options.AddHideOption(upgraded_proto.options)
    options.SetVersioningAnnotation(upgraded_proto.options, type_context.name)
    # Mark deprecated fields as ready for deletion by protoxform.
    for f in upgraded_proto.field:
      if f.options.deprecated:
        self._Deprecate(upgraded_proto, f)
        if self._envoy_internal_shadow:
          # When shadowing, we use the upgraded version of types (which should
          # themselves also be shadowed), to allow us to avoid unnecessary
          # references to the previous version (and complexities around
          # upgrading during API boosting).
          f.type_name = self._UpgradedType(f.type_name)
        else:
          # Make sure the type name is erased so it isn't picked up by protoxform
          # when computing deps.
          f.type_name = ""
      else:
        f.type_name = self._UpgradedType(f.type_name)
      if f.options.HasExtension(migrate_pb2.field_migrate):
        self._Rename(f, f.options.Extensions[migrate_pb2.field_migrate])
    # Upgrade nested messages.
    del upgraded_proto.nested_type[:]
    upgraded_proto.nested_type.extend(nested_msgs)
    # Upgrade enums.
    del upgraded_proto.enum_type[:]
    upgraded_proto.enum_type.extend(nested_enums)
    return upgraded_proto

  def VisitEnum(self, enum_proto, type_context):
    upgraded_proto = copy.deepcopy(enum_proto)
    for v in upgraded_proto.value:
      if v.options.deprecated:
        # We need special handling for the zero field, as proto3 needs some value
        # here.
        if v.number == 0 and not self._envoy_internal_shadow:
          v.name = 'DEPRECATED_AND_UNAVAILABLE_DO_NOT_USE'
        else:
          # Mark deprecated enum values as ready for deletion by protoxform.
          self._Deprecate(upgraded_proto, v)
    return upgraded_proto

  def VisitFile(self, file_proto, type_context, services, msgs, enums):
    upgraded_proto = copy.deepcopy(file_proto)
    # Upgrade imports.
    upgraded_proto.dependency[:] = [
        dependency for dependency in upgraded_proto.dependency
        if dependency not in ("udpa/annotations/migrate.proto")
    ]
    # Upgrade package.
    upgraded_proto.package = self._typedb.next_version_packages[upgraded_proto.package]
    upgraded_proto.name = self._typedb.next_version_proto_paths[upgraded_proto.name]
    # Upgrade comments.
    for location in upgraded_proto.source_code_info.location:
      location.leading_comments = self._UpgradedComment(location.leading_comments)
      location.trailing_comments = self._UpgradedComment(location.trailing_comments)
      for n, c in enumerate(location.leading_detached_comments):
        location.leading_detached_comments[n] = self._UpgradedComment(c)
    # Upgrade services.
    del upgraded_proto.service[:]
    upgraded_proto.service.extend(services)
    # Upgrade messages.
    del upgraded_proto.message_type[:]
    upgraded_proto.message_type.extend(msgs)
    # Upgrade enums.
    del upgraded_proto.enum_type[:]
    upgraded_proto.enum_type.extend(enums)

    return upgraded_proto


def V3MigrationXform(envoy_internal_shadow, file_proto):
  """Transform a FileDescriptorProto from v2[alpha\d] to v3alpha.

  Args:
    envoy_internal_shadow: generate a shadow for Envoy internal use containing deprecated fields.
    file_proto: v2[alpha\d] FileDescriptorProto message.

  Returns:
    v3 FileDescriptorProto message.
  """
  # Load type database.
  typedb = utils.GetTypeDb()
  # If this isn't a proto in an upgraded package, return None.
  if file_proto.package not in typedb.next_version_packages or not typedb.next_version_packages[
      file_proto.package]:
    return None
  # Otherwise, this .proto needs upgrading, do it.
  return traverse.TraverseFile(file_proto, UpgradeVisitor(typedb, envoy_internal_shadow))
