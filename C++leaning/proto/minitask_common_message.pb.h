// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: minitask_common_message.proto

#ifndef PROTOBUF_minitask_5fcommon_5fmessage_2eproto__INCLUDED
#define PROTOBUF_minitask_5fcommon_5fmessage_2eproto__INCLUDED

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
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace minitask {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_minitask_5fcommon_5fmessage_2eproto();
void protobuf_AssignDesc_minitask_5fcommon_5fmessage_2eproto();
void protobuf_ShutdownFile_minitask_5fcommon_5fmessage_2eproto();

class KeyValType;
class KeyValTab;
class KeyAddrType;
class AddrType;
class KeyAddrTab;

// ===================================================================

class KeyValType : public ::google::protobuf::Message {
 public:
  KeyValType();
  virtual ~KeyValType();

  KeyValType(const KeyValType& from);

  inline KeyValType& operator=(const KeyValType& from) {
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
  static const KeyValType& default_instance();

  void Swap(KeyValType* other);

  // implements Message ----------------------------------------------

  KeyValType* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const KeyValType& from);
  void MergeFrom(const KeyValType& from);
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

  // required string key = 1;
  inline bool has_key() const;
  inline void clear_key();
  static const int kKeyFieldNumber = 1;
  inline const ::std::string& key() const;
  inline void set_key(const ::std::string& value);
  inline void set_key(const char* value);
  inline void set_key(const char* value, size_t size);
  inline ::std::string* mutable_key();
  inline ::std::string* release_key();
  inline void set_allocated_key(::std::string* key);

  // optional bytes val = 2;
  inline bool has_val() const;
  inline void clear_val();
  static const int kValFieldNumber = 2;
  inline const ::std::string& val() const;
  inline void set_val(const ::std::string& value);
  inline void set_val(const char* value);
  inline void set_val(const void* value, size_t size);
  inline ::std::string* mutable_val();
  inline ::std::string* release_val();
  inline void set_allocated_val(::std::string* val);

  // @@protoc_insertion_point(class_scope:minitask.KeyValType)
 private:
  inline void set_has_key();
  inline void clear_has_key();
  inline void set_has_val();
  inline void clear_has_val();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* key_;
  ::std::string* val_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_AssignDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_ShutdownFile_minitask_5fcommon_5fmessage_2eproto();

  void InitAsDefaultInstance();
  static KeyValType* default_instance_;
};
// -------------------------------------------------------------------

class KeyValTab : public ::google::protobuf::Message {
 public:
  KeyValTab();
  virtual ~KeyValTab();

  KeyValTab(const KeyValTab& from);

  inline KeyValTab& operator=(const KeyValTab& from) {
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
  static const KeyValTab& default_instance();

  void Swap(KeyValTab* other);

  // implements Message ----------------------------------------------

  KeyValTab* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const KeyValTab& from);
  void MergeFrom(const KeyValTab& from);
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

  // repeated .minitask.KeyValType keyvaltmp = 1;
  inline int keyvaltmp_size() const;
  inline void clear_keyvaltmp();
  static const int kKeyvaltmpFieldNumber = 1;
  inline const ::minitask::KeyValType& keyvaltmp(int index) const;
  inline ::minitask::KeyValType* mutable_keyvaltmp(int index);
  inline ::minitask::KeyValType* add_keyvaltmp();
  inline const ::google::protobuf::RepeatedPtrField< ::minitask::KeyValType >&
      keyvaltmp() const;
  inline ::google::protobuf::RepeatedPtrField< ::minitask::KeyValType >*
      mutable_keyvaltmp();

  // @@protoc_insertion_point(class_scope:minitask.KeyValTab)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::minitask::KeyValType > keyvaltmp_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_AssignDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_ShutdownFile_minitask_5fcommon_5fmessage_2eproto();

  void InitAsDefaultInstance();
  static KeyValTab* default_instance_;
};
// -------------------------------------------------------------------

class KeyAddrType : public ::google::protobuf::Message {
 public:
  KeyAddrType();
  virtual ~KeyAddrType();

  KeyAddrType(const KeyAddrType& from);

  inline KeyAddrType& operator=(const KeyAddrType& from) {
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
  static const KeyAddrType& default_instance();

  void Swap(KeyAddrType* other);

  // implements Message ----------------------------------------------

  KeyAddrType* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const KeyAddrType& from);
  void MergeFrom(const KeyAddrType& from);
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

  // required string key = 1;
  inline bool has_key() const;
  inline void clear_key();
  static const int kKeyFieldNumber = 1;
  inline const ::std::string& key() const;
  inline void set_key(const ::std::string& value);
  inline void set_key(const char* value);
  inline void set_key(const char* value, size_t size);
  inline ::std::string* mutable_key();
  inline ::std::string* release_key();
  inline void set_allocated_key(::std::string* key);

  // required .minitask.AddrType sockaddr = 2;
  inline bool has_sockaddr() const;
  inline void clear_sockaddr();
  static const int kSockaddrFieldNumber = 2;
  inline const ::minitask::AddrType& sockaddr() const;
  inline ::minitask::AddrType* mutable_sockaddr();
  inline ::minitask::AddrType* release_sockaddr();
  inline void set_allocated_sockaddr(::minitask::AddrType* sockaddr);

  // @@protoc_insertion_point(class_scope:minitask.KeyAddrType)
 private:
  inline void set_has_key();
  inline void clear_has_key();
  inline void set_has_sockaddr();
  inline void clear_has_sockaddr();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* key_;
  ::minitask::AddrType* sockaddr_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_AssignDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_ShutdownFile_minitask_5fcommon_5fmessage_2eproto();

  void InitAsDefaultInstance();
  static KeyAddrType* default_instance_;
};
// -------------------------------------------------------------------

class AddrType : public ::google::protobuf::Message {
 public:
  AddrType();
  virtual ~AddrType();

  AddrType(const AddrType& from);

  inline AddrType& operator=(const AddrType& from) {
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
  static const AddrType& default_instance();

  void Swap(AddrType* other);

  // implements Message ----------------------------------------------

  AddrType* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AddrType& from);
  void MergeFrom(const AddrType& from);
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

  // required string domain = 1;
  inline bool has_domain() const;
  inline void clear_domain();
  static const int kDomainFieldNumber = 1;
  inline const ::std::string& domain() const;
  inline void set_domain(const ::std::string& value);
  inline void set_domain(const char* value);
  inline void set_domain(const char* value, size_t size);
  inline ::std::string* mutable_domain();
  inline ::std::string* release_domain();
  inline void set_allocated_domain(::std::string* domain);

  // required string addr = 2;
  inline bool has_addr() const;
  inline void clear_addr();
  static const int kAddrFieldNumber = 2;
  inline const ::std::string& addr() const;
  inline void set_addr(const ::std::string& value);
  inline void set_addr(const char* value);
  inline void set_addr(const char* value, size_t size);
  inline ::std::string* mutable_addr();
  inline ::std::string* release_addr();
  inline void set_allocated_addr(::std::string* addr);

  // required string port = 3;
  inline bool has_port() const;
  inline void clear_port();
  static const int kPortFieldNumber = 3;
  inline const ::std::string& port() const;
  inline void set_port(const ::std::string& value);
  inline void set_port(const char* value);
  inline void set_port(const char* value, size_t size);
  inline ::std::string* mutable_port();
  inline ::std::string* release_port();
  inline void set_allocated_port(::std::string* port);

  // @@protoc_insertion_point(class_scope:minitask.AddrType)
 private:
  inline void set_has_domain();
  inline void clear_has_domain();
  inline void set_has_addr();
  inline void clear_has_addr();
  inline void set_has_port();
  inline void clear_has_port();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* domain_;
  ::std::string* addr_;
  ::std::string* port_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_AssignDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_ShutdownFile_minitask_5fcommon_5fmessage_2eproto();

  void InitAsDefaultInstance();
  static AddrType* default_instance_;
};
// -------------------------------------------------------------------

class KeyAddrTab : public ::google::protobuf::Message {
 public:
  KeyAddrTab();
  virtual ~KeyAddrTab();

  KeyAddrTab(const KeyAddrTab& from);

  inline KeyAddrTab& operator=(const KeyAddrTab& from) {
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
  static const KeyAddrTab& default_instance();

  void Swap(KeyAddrTab* other);

  // implements Message ----------------------------------------------

  KeyAddrTab* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const KeyAddrTab& from);
  void MergeFrom(const KeyAddrTab& from);
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

  // repeated .minitask.KeyAddrType kadrecord = 1;
  inline int kadrecord_size() const;
  inline void clear_kadrecord();
  static const int kKadrecordFieldNumber = 1;
  inline const ::minitask::KeyAddrType& kadrecord(int index) const;
  inline ::minitask::KeyAddrType* mutable_kadrecord(int index);
  inline ::minitask::KeyAddrType* add_kadrecord();
  inline const ::google::protobuf::RepeatedPtrField< ::minitask::KeyAddrType >&
      kadrecord() const;
  inline ::google::protobuf::RepeatedPtrField< ::minitask::KeyAddrType >*
      mutable_kadrecord();

  // @@protoc_insertion_point(class_scope:minitask.KeyAddrTab)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::minitask::KeyAddrType > kadrecord_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_AssignDesc_minitask_5fcommon_5fmessage_2eproto();
  friend void protobuf_ShutdownFile_minitask_5fcommon_5fmessage_2eproto();

  void InitAsDefaultInstance();
  static KeyAddrTab* default_instance_;
};
// ===================================================================


// ===================================================================

// KeyValType

// required string key = 1;
inline bool KeyValType::has_key() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void KeyValType::set_has_key() {
  _has_bits_[0] |= 0x00000001u;
}
inline void KeyValType::clear_has_key() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void KeyValType::clear_key() {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    key_->clear();
  }
  clear_has_key();
}
inline const ::std::string& KeyValType::key() const {
  return *key_;
}
inline void KeyValType::set_key(const ::std::string& value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(value);
}
inline void KeyValType::set_key(const char* value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(value);
}
inline void KeyValType::set_key(const char* value, size_t size) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* KeyValType::mutable_key() {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  return key_;
}
inline ::std::string* KeyValType::release_key() {
  clear_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = key_;
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void KeyValType::set_allocated_key(::std::string* key) {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    delete key_;
  }
  if (key) {
    set_has_key();
    key_ = key;
  } else {
    clear_has_key();
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional bytes val = 2;
inline bool KeyValType::has_val() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void KeyValType::set_has_val() {
  _has_bits_[0] |= 0x00000002u;
}
inline void KeyValType::clear_has_val() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void KeyValType::clear_val() {
  if (val_ != &::google::protobuf::internal::kEmptyString) {
    val_->clear();
  }
  clear_has_val();
}
inline const ::std::string& KeyValType::val() const {
  return *val_;
}
inline void KeyValType::set_val(const ::std::string& value) {
  set_has_val();
  if (val_ == &::google::protobuf::internal::kEmptyString) {
    val_ = new ::std::string;
  }
  val_->assign(value);
}
inline void KeyValType::set_val(const char* value) {
  set_has_val();
  if (val_ == &::google::protobuf::internal::kEmptyString) {
    val_ = new ::std::string;
  }
  val_->assign(value);
}
inline void KeyValType::set_val(const void* value, size_t size) {
  set_has_val();
  if (val_ == &::google::protobuf::internal::kEmptyString) {
    val_ = new ::std::string;
  }
  val_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* KeyValType::mutable_val() {
  set_has_val();
  if (val_ == &::google::protobuf::internal::kEmptyString) {
    val_ = new ::std::string;
  }
  return val_;
}
inline ::std::string* KeyValType::release_val() {
  clear_has_val();
  if (val_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = val_;
    val_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void KeyValType::set_allocated_val(::std::string* val) {
  if (val_ != &::google::protobuf::internal::kEmptyString) {
    delete val_;
  }
  if (val) {
    set_has_val();
    val_ = val;
  } else {
    clear_has_val();
    val_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// -------------------------------------------------------------------

// KeyValTab

// repeated .minitask.KeyValType keyvaltmp = 1;
inline int KeyValTab::keyvaltmp_size() const {
  return keyvaltmp_.size();
}
inline void KeyValTab::clear_keyvaltmp() {
  keyvaltmp_.Clear();
}
inline const ::minitask::KeyValType& KeyValTab::keyvaltmp(int index) const {
  return keyvaltmp_.Get(index);
}
inline ::minitask::KeyValType* KeyValTab::mutable_keyvaltmp(int index) {
  return keyvaltmp_.Mutable(index);
}
inline ::minitask::KeyValType* KeyValTab::add_keyvaltmp() {
  return keyvaltmp_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::minitask::KeyValType >&
KeyValTab::keyvaltmp() const {
  return keyvaltmp_;
}
inline ::google::protobuf::RepeatedPtrField< ::minitask::KeyValType >*
KeyValTab::mutable_keyvaltmp() {
  return &keyvaltmp_;
}

// -------------------------------------------------------------------

// KeyAddrType

// required string key = 1;
inline bool KeyAddrType::has_key() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void KeyAddrType::set_has_key() {
  _has_bits_[0] |= 0x00000001u;
}
inline void KeyAddrType::clear_has_key() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void KeyAddrType::clear_key() {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    key_->clear();
  }
  clear_has_key();
}
inline const ::std::string& KeyAddrType::key() const {
  return *key_;
}
inline void KeyAddrType::set_key(const ::std::string& value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(value);
}
inline void KeyAddrType::set_key(const char* value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(value);
}
inline void KeyAddrType::set_key(const char* value, size_t size) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  key_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* KeyAddrType::mutable_key() {
  set_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    key_ = new ::std::string;
  }
  return key_;
}
inline ::std::string* KeyAddrType::release_key() {
  clear_has_key();
  if (key_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = key_;
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void KeyAddrType::set_allocated_key(::std::string* key) {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    delete key_;
  }
  if (key) {
    set_has_key();
    key_ = key;
  } else {
    clear_has_key();
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required .minitask.AddrType sockaddr = 2;
inline bool KeyAddrType::has_sockaddr() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void KeyAddrType::set_has_sockaddr() {
  _has_bits_[0] |= 0x00000002u;
}
inline void KeyAddrType::clear_has_sockaddr() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void KeyAddrType::clear_sockaddr() {
  if (sockaddr_ != NULL) sockaddr_->::minitask::AddrType::Clear();
  clear_has_sockaddr();
}
inline const ::minitask::AddrType& KeyAddrType::sockaddr() const {
  return sockaddr_ != NULL ? *sockaddr_ : *default_instance_->sockaddr_;
}
inline ::minitask::AddrType* KeyAddrType::mutable_sockaddr() {
  set_has_sockaddr();
  if (sockaddr_ == NULL) sockaddr_ = new ::minitask::AddrType;
  return sockaddr_;
}
inline ::minitask::AddrType* KeyAddrType::release_sockaddr() {
  clear_has_sockaddr();
  ::minitask::AddrType* temp = sockaddr_;
  sockaddr_ = NULL;
  return temp;
}
inline void KeyAddrType::set_allocated_sockaddr(::minitask::AddrType* sockaddr) {
  delete sockaddr_;
  sockaddr_ = sockaddr;
  if (sockaddr) {
    set_has_sockaddr();
  } else {
    clear_has_sockaddr();
  }
}

// -------------------------------------------------------------------

// AddrType

// required string domain = 1;
inline bool AddrType::has_domain() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void AddrType::set_has_domain() {
  _has_bits_[0] |= 0x00000001u;
}
inline void AddrType::clear_has_domain() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void AddrType::clear_domain() {
  if (domain_ != &::google::protobuf::internal::kEmptyString) {
    domain_->clear();
  }
  clear_has_domain();
}
inline const ::std::string& AddrType::domain() const {
  return *domain_;
}
inline void AddrType::set_domain(const ::std::string& value) {
  set_has_domain();
  if (domain_ == &::google::protobuf::internal::kEmptyString) {
    domain_ = new ::std::string;
  }
  domain_->assign(value);
}
inline void AddrType::set_domain(const char* value) {
  set_has_domain();
  if (domain_ == &::google::protobuf::internal::kEmptyString) {
    domain_ = new ::std::string;
  }
  domain_->assign(value);
}
inline void AddrType::set_domain(const char* value, size_t size) {
  set_has_domain();
  if (domain_ == &::google::protobuf::internal::kEmptyString) {
    domain_ = new ::std::string;
  }
  domain_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* AddrType::mutable_domain() {
  set_has_domain();
  if (domain_ == &::google::protobuf::internal::kEmptyString) {
    domain_ = new ::std::string;
  }
  return domain_;
}
inline ::std::string* AddrType::release_domain() {
  clear_has_domain();
  if (domain_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = domain_;
    domain_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void AddrType::set_allocated_domain(::std::string* domain) {
  if (domain_ != &::google::protobuf::internal::kEmptyString) {
    delete domain_;
  }
  if (domain) {
    set_has_domain();
    domain_ = domain;
  } else {
    clear_has_domain();
    domain_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required string addr = 2;
inline bool AddrType::has_addr() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void AddrType::set_has_addr() {
  _has_bits_[0] |= 0x00000002u;
}
inline void AddrType::clear_has_addr() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void AddrType::clear_addr() {
  if (addr_ != &::google::protobuf::internal::kEmptyString) {
    addr_->clear();
  }
  clear_has_addr();
}
inline const ::std::string& AddrType::addr() const {
  return *addr_;
}
inline void AddrType::set_addr(const ::std::string& value) {
  set_has_addr();
  if (addr_ == &::google::protobuf::internal::kEmptyString) {
    addr_ = new ::std::string;
  }
  addr_->assign(value);
}
inline void AddrType::set_addr(const char* value) {
  set_has_addr();
  if (addr_ == &::google::protobuf::internal::kEmptyString) {
    addr_ = new ::std::string;
  }
  addr_->assign(value);
}
inline void AddrType::set_addr(const char* value, size_t size) {
  set_has_addr();
  if (addr_ == &::google::protobuf::internal::kEmptyString) {
    addr_ = new ::std::string;
  }
  addr_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* AddrType::mutable_addr() {
  set_has_addr();
  if (addr_ == &::google::protobuf::internal::kEmptyString) {
    addr_ = new ::std::string;
  }
  return addr_;
}
inline ::std::string* AddrType::release_addr() {
  clear_has_addr();
  if (addr_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = addr_;
    addr_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void AddrType::set_allocated_addr(::std::string* addr) {
  if (addr_ != &::google::protobuf::internal::kEmptyString) {
    delete addr_;
  }
  if (addr) {
    set_has_addr();
    addr_ = addr;
  } else {
    clear_has_addr();
    addr_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required string port = 3;
inline bool AddrType::has_port() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void AddrType::set_has_port() {
  _has_bits_[0] |= 0x00000004u;
}
inline void AddrType::clear_has_port() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void AddrType::clear_port() {
  if (port_ != &::google::protobuf::internal::kEmptyString) {
    port_->clear();
  }
  clear_has_port();
}
inline const ::std::string& AddrType::port() const {
  return *port_;
}
inline void AddrType::set_port(const ::std::string& value) {
  set_has_port();
  if (port_ == &::google::protobuf::internal::kEmptyString) {
    port_ = new ::std::string;
  }
  port_->assign(value);
}
inline void AddrType::set_port(const char* value) {
  set_has_port();
  if (port_ == &::google::protobuf::internal::kEmptyString) {
    port_ = new ::std::string;
  }
  port_->assign(value);
}
inline void AddrType::set_port(const char* value, size_t size) {
  set_has_port();
  if (port_ == &::google::protobuf::internal::kEmptyString) {
    port_ = new ::std::string;
  }
  port_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* AddrType::mutable_port() {
  set_has_port();
  if (port_ == &::google::protobuf::internal::kEmptyString) {
    port_ = new ::std::string;
  }
  return port_;
}
inline ::std::string* AddrType::release_port() {
  clear_has_port();
  if (port_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = port_;
    port_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void AddrType::set_allocated_port(::std::string* port) {
  if (port_ != &::google::protobuf::internal::kEmptyString) {
    delete port_;
  }
  if (port) {
    set_has_port();
    port_ = port;
  } else {
    clear_has_port();
    port_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// -------------------------------------------------------------------

// KeyAddrTab

// repeated .minitask.KeyAddrType kadrecord = 1;
inline int KeyAddrTab::kadrecord_size() const {
  return kadrecord_.size();
}
inline void KeyAddrTab::clear_kadrecord() {
  kadrecord_.Clear();
}
inline const ::minitask::KeyAddrType& KeyAddrTab::kadrecord(int index) const {
  return kadrecord_.Get(index);
}
inline ::minitask::KeyAddrType* KeyAddrTab::mutable_kadrecord(int index) {
  return kadrecord_.Mutable(index);
}
inline ::minitask::KeyAddrType* KeyAddrTab::add_kadrecord() {
  return kadrecord_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::minitask::KeyAddrType >&
KeyAddrTab::kadrecord() const {
  return kadrecord_;
}
inline ::google::protobuf::RepeatedPtrField< ::minitask::KeyAddrType >*
KeyAddrTab::mutable_kadrecord() {
  return &kadrecord_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace minitask

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_minitask_5fcommon_5fmessage_2eproto__INCLUDED
