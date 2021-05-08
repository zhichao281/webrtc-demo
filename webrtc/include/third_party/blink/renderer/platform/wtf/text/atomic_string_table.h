// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_TEXT_ATOMIC_STRING_TABLE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_TEXT_ATOMIC_STRING_TABLE_H_

#include "base/macros.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/text/string_hash.h"
#include "third_party/blink/renderer/platform/wtf/text/string_impl.h"
#include "third_party/blink/renderer/platform/wtf/threading.h"
#include "third_party/blink/renderer/platform/wtf/wtf_export.h"

namespace WTF {

// The underlying storage that keeps the map of unique AtomicStrings. This is
// not thread safe and each Threading has one.
class WTF_EXPORT AtomicStringTable final {
  USING_FAST_MALLOC(AtomicStringTable);

 public:
  AtomicStringTable();
  ~AtomicStringTable();

  // Gets the shared table for the current thread.
  static AtomicStringTable& Instance() {
    return WtfThreading().GetAtomicStringTable();
  }

  // Used by system initialization to preallocate enough storage for all of
  // the static strings.
  void ReserveCapacity(unsigned size);

  // Inserting strings into the table. Note that the return value from adding
  // a UChar string may be an LChar string as the table will attempt to
  // convert the string to save memory if possible.
  StringImpl* Add(StringImpl*);
  scoped_refptr<StringImpl> Add(const LChar* chars, unsigned length);
  scoped_refptr<StringImpl> Add(const UChar* chars, unsigned length);

  // Adding UTF8.
  // Returns null if the characters contain invalid utf8 sequences.
  // Pass null for the charactersEnd to automatically detect the length.
  scoped_refptr<StringImpl> AddUTF8(const char* characters_start,
                                    const char* characters_end);

  // Returned as part of the WeakFind() APIs below. Represents the result of
  // the non-creating lookup within the AtomicStringTable. See the WeakFind()
  // documentation for a description of how it can be used.
  class WeakResult {
   public:
    WeakResult() = default;
    explicit WeakResult(StringImpl* str)
        : ptr_value_(reinterpret_cast<uintptr_t>(str)) {
      CHECK(!str || str->IsAtomic() || str == StringImpl::empty_);
    }

    bool IsNull() const { return ptr_value_ == 0; }

   private:
    friend bool operator==(const WeakResult& lhs, const WeakResult& rhs);
    friend bool operator==(const StringImpl* lhs, const WeakResult& rhs);

    // Contains the pointer a string in a non-deferenceable form. Do NOT cast
    // back to a StringImpl and dereference. The object may no longer be alive.
    uintptr_t ptr_value_ = 0;
  };

  // Checks for existence of a string in the AtomicStringTable without
  // unnecessarily creating an AtomicString. Useful to optimize fast-path
  // non-existence checks inside collections of AtomicStrings.
  //
  // Specifically, if WeakFind() returns an IsNull() WeakResult, then a
  // collection search can be skipped because the AtomicString cannot exist
  // in the collection. If WeakFind() returns a non-null WeakResult, then
  // assuming the target collection has no concurrent access, this lookup
  // can be reused to check for existence in the collection without
  // requiring either an AtomicString collection or another lookup within
  // the AtomicStringTable.
  WeakResult WeakFind(StringImpl* string) {
    // Mirror the empty logic in Add().
    if (UNLIKELY(!string->length()))
      return WeakResult(StringImpl::empty_);

    if (LIKELY(string->IsAtomic()))
      return WeakResult(string);

    return WeakFindSlow(string);
  }

  WeakResult WeakFind(const StringView& string) {
    // Mirror the empty logic in Add().
    if (UNLIKELY(!string.length()))
      return WeakResult(StringImpl::empty_);

    if (LIKELY(string.IsAtomic()))
      return WeakResult(string.SharedImpl());

    return WeakFindSlow(string);
  }

  WeakResult WeakFind(const LChar* chars, unsigned length);
  WeakResult WeakFind(const UChar* chars, unsigned length);

  WeakResult WeakFindLowercased(const StringView& string) {
    // Mirror the empty logic in Add().
    if (UNLIKELY(!string.length()))
      return WeakResult(StringImpl::empty_);

    if (LIKELY(string.IsAtomic() && string.IsLowerASCII()))
      return WeakResult(string.SharedImpl());

    return WeakFindLowercasedSlow(string);
  }

  // This is for ~StringImpl to unregister a string before destruction since
  // the table is holding weak pointers. It should not be used directly.
  void Remove(StringImpl*);

 private:
  template <typename T, typename HashTranslator>
  inline scoped_refptr<StringImpl> AddToStringTable(const T& value);

  WeakResult WeakFindSlow(StringImpl*);
  WeakResult WeakFindSlow(const StringView&);
  WeakResult WeakFindLowercasedSlow(const StringView& string);

  HashSet<StringImpl*> table_;

  DISALLOW_COPY_AND_ASSIGN(AtomicStringTable);
};

inline bool operator==(const AtomicStringTable::WeakResult& lhs,
                       const AtomicStringTable::WeakResult& rhs) {
  return lhs.ptr_value_ == rhs.ptr_value_;
}

inline bool operator==(const AtomicStringTable::WeakResult& lhs,
                       const StringImpl* rhs) {
  return rhs == lhs;
}

inline bool operator==(const StringImpl* lhs,
                       const AtomicStringTable::WeakResult& rhs) {
  return reinterpret_cast<uintptr_t>(lhs) == rhs.ptr_value_;
}

inline bool operator==(const AtomicStringTable::WeakResult& lhs,
                       const String& rhs) {
  return lhs == rhs.Impl();
}

inline bool operator==(const String& lhs,
                       const AtomicStringTable::WeakResult& rhs) {
  return lhs.Impl() == rhs;
}

inline bool operator==(const AtomicStringTable::WeakResult& lhs,
                       const AtomicString& rhs) {
  return lhs == rhs.Impl();
}

inline bool operator==(const AtomicString& lhs,
                       const AtomicStringTable::WeakResult& rhs) {
  return lhs.Impl() == rhs;
}

}  // namespace WTF

using WTF::AtomicStringTable;

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_TEXT_ATOMIC_STRING_TABLE_H_
