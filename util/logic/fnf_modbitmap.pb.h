// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: fnf_modbitmap.proto

#ifndef PROTOBUF_fnf_5fmodbitmap_2eproto__INCLUDED
#define PROTOBUF_fnf_5fmodbitmap_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace FNF {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_fnf_5fmodbitmap_2eproto();
void protobuf_AssignDesc_fnf_5fmodbitmap_2eproto();
void protobuf_ShutdownFile_fnf_5fmodbitmap_2eproto();

class mod_bitmap;

enum EM_PLAT {
  PLAT_AND = 1,
  PLAT_IOS = 2
};
bool EM_PLAT_IsValid(int value);
const EM_PLAT EM_PLAT_MIN = PLAT_AND;
const EM_PLAT EM_PLAT_MAX = PLAT_IOS;
const int EM_PLAT_ARRAYSIZE = EM_PLAT_MAX + 1;

const ::google::protobuf::EnumDescriptor* EM_PLAT_descriptor();
inline const ::std::string& EM_PLAT_Name(EM_PLAT value) {
  return ::google::protobuf::internal::NameOfEnum(
    EM_PLAT_descriptor(), value);
}
inline bool EM_PLAT_Parse(
    const ::std::string& name, EM_PLAT* value) {
  return ::google::protobuf::internal::ParseNamedEnum<EM_PLAT>(
    EM_PLAT_descriptor(), name, value);
}
// ===================================================================

class mod_bitmap : public ::google::protobuf::Message {
 public:
  mod_bitmap();
  virtual ~mod_bitmap();

  mod_bitmap(const mod_bitmap& from);

  inline mod_bitmap& operator=(const mod_bitmap& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const mod_bitmap& default_instance();

  void Swap(mod_bitmap* other);

  // implements Message ----------------------------------------------

  mod_bitmap* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const mod_bitmap& from);
  void MergeFrom(const mod_bitmap& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int64 uid = 1;
  inline bool has_uid() const;
  inline void clear_uid();
  static const int kUidFieldNumber = 1;
  inline ::google::protobuf::int64 uid() const;
  inline void set_uid(::google::protobuf::int64 value);

  // required .FNF.EM_PLAT plat = 2;
  inline bool has_plat() const;
  inline void clear_plat();
  static const int kPlatFieldNumber = 2;
  inline ::FNF::EM_PLAT plat() const;
  inline void set_plat(::FNF::EM_PLAT value);

  // @@protoc_insertion_point(class_scope:FNF.mod_bitmap)
 private:
  inline void set_has_uid();
  inline void clear_has_uid();
  inline void set_has_plat();
  inline void clear_has_plat();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int64 uid_;
  int plat_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_fnf_5fmodbitmap_2eproto();
  friend void protobuf_AssignDesc_fnf_5fmodbitmap_2eproto();
  friend void protobuf_ShutdownFile_fnf_5fmodbitmap_2eproto();

  void InitAsDefaultInstance();
  static mod_bitmap* default_instance_;
};
// ===================================================================


// ===================================================================

// mod_bitmap

// required int64 uid = 1;
inline bool mod_bitmap::has_uid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void mod_bitmap::set_has_uid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void mod_bitmap::clear_has_uid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void mod_bitmap::clear_uid() {
  uid_ = GOOGLE_LONGLONG(0);
  clear_has_uid();
}
inline ::google::protobuf::int64 mod_bitmap::uid() const {
  return uid_;
}
inline void mod_bitmap::set_uid(::google::protobuf::int64 value) {
  set_has_uid();
  uid_ = value;
}

// required .FNF.EM_PLAT plat = 2;
inline bool mod_bitmap::has_plat() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void mod_bitmap::set_has_plat() {
  _has_bits_[0] |= 0x00000002u;
}
inline void mod_bitmap::clear_has_plat() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void mod_bitmap::clear_plat() {
  plat_ = 1;
  clear_has_plat();
}
inline ::FNF::EM_PLAT mod_bitmap::plat() const {
  return static_cast< ::FNF::EM_PLAT >(plat_);
}
inline void mod_bitmap::set_plat(::FNF::EM_PLAT value) {
  assert(::FNF::EM_PLAT_IsValid(value));
  set_has_plat();
  plat_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace FNF

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::FNF::EM_PLAT>() {
  return ::FNF::EM_PLAT_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_fnf_5fmodbitmap_2eproto__INCLUDED
