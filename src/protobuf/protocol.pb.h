// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: protocol.proto

#ifndef PROTOBUF_protocol_2eproto__INCLUDED
#define PROTOBUF_protocol_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3005001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace protobuf_protocol_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
void InitDefaultsProtocolWrapperImpl();
void InitDefaultsProtocolWrapper();
inline void InitDefaults() {
  InitDefaultsProtocolWrapper();
}
}  // namespace protobuf_protocol_2eproto
namespace gamemessages {
class ProtocolWrapper;
class ProtocolWrapperDefaultTypeInternal;
extern ProtocolWrapperDefaultTypeInternal _ProtocolWrapper_default_instance_;
}  // namespace gamemessages
namespace gamemessages {

enum ProtocolWrapper_messageTypes {
  ProtocolWrapper_messageTypes_connect = 0,
  ProtocolWrapper_messageTypes_playerUpdate = 1
};
bool ProtocolWrapper_messageTypes_IsValid(int value);
const ProtocolWrapper_messageTypes ProtocolWrapper_messageTypes_messageTypes_MIN = ProtocolWrapper_messageTypes_connect;
const ProtocolWrapper_messageTypes ProtocolWrapper_messageTypes_messageTypes_MAX = ProtocolWrapper_messageTypes_playerUpdate;
const int ProtocolWrapper_messageTypes_messageTypes_ARRAYSIZE = ProtocolWrapper_messageTypes_messageTypes_MAX + 1;

const ::google::protobuf::EnumDescriptor* ProtocolWrapper_messageTypes_descriptor();
inline const ::std::string& ProtocolWrapper_messageTypes_Name(ProtocolWrapper_messageTypes value) {
  return ::google::protobuf::internal::NameOfEnum(
    ProtocolWrapper_messageTypes_descriptor(), value);
}
inline bool ProtocolWrapper_messageTypes_Parse(
    const ::std::string& name, ProtocolWrapper_messageTypes* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ProtocolWrapper_messageTypes>(
    ProtocolWrapper_messageTypes_descriptor(), name, value);
}
// ===================================================================

class ProtocolWrapper : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:gamemessages.ProtocolWrapper) */ {
 public:
  ProtocolWrapper();
  virtual ~ProtocolWrapper();

  ProtocolWrapper(const ProtocolWrapper& from);

  inline ProtocolWrapper& operator=(const ProtocolWrapper& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ProtocolWrapper(ProtocolWrapper&& from) noexcept
    : ProtocolWrapper() {
    *this = ::std::move(from);
  }

  inline ProtocolWrapper& operator=(ProtocolWrapper&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ProtocolWrapper& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ProtocolWrapper* internal_default_instance() {
    return reinterpret_cast<const ProtocolWrapper*>(
               &_ProtocolWrapper_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(ProtocolWrapper* other);
  friend void swap(ProtocolWrapper& a, ProtocolWrapper& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ProtocolWrapper* New() const PROTOBUF_FINAL { return New(NULL); }

  ProtocolWrapper* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const ProtocolWrapper& from);
  void MergeFrom(const ProtocolWrapper& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(ProtocolWrapper* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  typedef ProtocolWrapper_messageTypes messageTypes;
  static const messageTypes connect =
    ProtocolWrapper_messageTypes_connect;
  static const messageTypes playerUpdate =
    ProtocolWrapper_messageTypes_playerUpdate;
  static inline bool messageTypes_IsValid(int value) {
    return ProtocolWrapper_messageTypes_IsValid(value);
  }
  static const messageTypes messageTypes_MIN =
    ProtocolWrapper_messageTypes_messageTypes_MIN;
  static const messageTypes messageTypes_MAX =
    ProtocolWrapper_messageTypes_messageTypes_MAX;
  static const int messageTypes_ARRAYSIZE =
    ProtocolWrapper_messageTypes_messageTypes_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  messageTypes_descriptor() {
    return ProtocolWrapper_messageTypes_descriptor();
  }
  static inline const ::std::string& messageTypes_Name(messageTypes value) {
    return ProtocolWrapper_messageTypes_Name(value);
  }
  static inline bool messageTypes_Parse(const ::std::string& name,
      messageTypes* value) {
    return ProtocolWrapper_messageTypes_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // required bytes data = 3;
  bool has_data() const;
  void clear_data();
  static const int kDataFieldNumber = 3;
  const ::std::string& data() const;
  void set_data(const ::std::string& value);
  #if LANG_CXX11
  void set_data(::std::string&& value);
  #endif
  void set_data(const char* value);
  void set_data(const void* value, size_t size);
  ::std::string* mutable_data();
  ::std::string* release_data();
  void set_allocated_data(::std::string* data);

  // required int32 messageType = 2;
  bool has_messagetype() const;
  void clear_messagetype();
  static const int kMessageTypeFieldNumber = 2;
  ::google::protobuf::int32 messagetype() const;
  void set_messagetype(::google::protobuf::int32 value);

  // required int32 protocolVersion = 1 [default = 1];
  bool has_protocolversion() const;
  void clear_protocolversion();
  static const int kProtocolVersionFieldNumber = 1;
  ::google::protobuf::int32 protocolversion() const;
  void set_protocolversion(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:gamemessages.ProtocolWrapper)
 private:
  void set_has_protocolversion();
  void clear_has_protocolversion();
  void set_has_messagetype();
  void clear_has_messagetype();
  void set_has_data();
  void clear_has_data();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr data_;
  ::google::protobuf::int32 messagetype_;
  ::google::protobuf::int32 protocolversion_;
  friend struct ::protobuf_protocol_2eproto::TableStruct;
  friend void ::protobuf_protocol_2eproto::InitDefaultsProtocolWrapperImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// ProtocolWrapper

// required int32 protocolVersion = 1 [default = 1];
inline bool ProtocolWrapper::has_protocolversion() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ProtocolWrapper::set_has_protocolversion() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ProtocolWrapper::clear_has_protocolversion() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ProtocolWrapper::clear_protocolversion() {
  protocolversion_ = 1;
  clear_has_protocolversion();
}
inline ::google::protobuf::int32 ProtocolWrapper::protocolversion() const {
  // @@protoc_insertion_point(field_get:gamemessages.ProtocolWrapper.protocolVersion)
  return protocolversion_;
}
inline void ProtocolWrapper::set_protocolversion(::google::protobuf::int32 value) {
  set_has_protocolversion();
  protocolversion_ = value;
  // @@protoc_insertion_point(field_set:gamemessages.ProtocolWrapper.protocolVersion)
}

// required int32 messageType = 2;
inline bool ProtocolWrapper::has_messagetype() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ProtocolWrapper::set_has_messagetype() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ProtocolWrapper::clear_has_messagetype() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ProtocolWrapper::clear_messagetype() {
  messagetype_ = 0;
  clear_has_messagetype();
}
inline ::google::protobuf::int32 ProtocolWrapper::messagetype() const {
  // @@protoc_insertion_point(field_get:gamemessages.ProtocolWrapper.messageType)
  return messagetype_;
}
inline void ProtocolWrapper::set_messagetype(::google::protobuf::int32 value) {
  set_has_messagetype();
  messagetype_ = value;
  // @@protoc_insertion_point(field_set:gamemessages.ProtocolWrapper.messageType)
}

// required bytes data = 3;
inline bool ProtocolWrapper::has_data() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ProtocolWrapper::set_has_data() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ProtocolWrapper::clear_has_data() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ProtocolWrapper::clear_data() {
  data_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_data();
}
inline const ::std::string& ProtocolWrapper::data() const {
  // @@protoc_insertion_point(field_get:gamemessages.ProtocolWrapper.data)
  return data_.GetNoArena();
}
inline void ProtocolWrapper::set_data(const ::std::string& value) {
  set_has_data();
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:gamemessages.ProtocolWrapper.data)
}
#if LANG_CXX11
inline void ProtocolWrapper::set_data(::std::string&& value) {
  set_has_data();
  data_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:gamemessages.ProtocolWrapper.data)
}
#endif
inline void ProtocolWrapper::set_data(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_data();
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:gamemessages.ProtocolWrapper.data)
}
inline void ProtocolWrapper::set_data(const void* value, size_t size) {
  set_has_data();
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:gamemessages.ProtocolWrapper.data)
}
inline ::std::string* ProtocolWrapper::mutable_data() {
  set_has_data();
  // @@protoc_insertion_point(field_mutable:gamemessages.ProtocolWrapper.data)
  return data_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* ProtocolWrapper::release_data() {
  // @@protoc_insertion_point(field_release:gamemessages.ProtocolWrapper.data)
  clear_has_data();
  return data_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void ProtocolWrapper::set_allocated_data(::std::string* data) {
  if (data != NULL) {
    set_has_data();
  } else {
    clear_has_data();
  }
  data_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), data);
  // @@protoc_insertion_point(field_set_allocated:gamemessages.ProtocolWrapper.data)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace gamemessages

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::gamemessages::ProtocolWrapper_messageTypes> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::gamemessages::ProtocolWrapper_messageTypes>() {
  return ::gamemessages::ProtocolWrapper_messageTypes_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_protocol_2eproto__INCLUDED