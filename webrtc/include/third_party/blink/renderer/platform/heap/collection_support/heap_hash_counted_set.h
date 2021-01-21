// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_COLLECTION_SUPPORT_HEAP_HASH_COUNTED_SET_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_COLLECTION_SUPPORT_HEAP_HASH_COUNTED_SET_H_

#include "third_party/blink/renderer/platform/heap/heap.h"
#include "third_party/blink/renderer/platform/heap/heap_allocator_impl.h"
#include "third_party/blink/renderer/platform/heap/visitor.h"
#include "third_party/blink/renderer/platform/wtf/hash_counted_set.h"

namespace blink {

template <typename Value,
          typename HashFunctions = typename DefaultHash<Value>::Hash,
          typename Traits = HashTraits<Value>>
class HeapHashCountedSet final
    : public GarbageCollected<HeapHashCountedSet<Value, HashFunctions, Traits>>,
      public HashCountedSet<Value, HashFunctions, Traits, HeapAllocator> {
  DISALLOW_NEW();

 public:
  HeapHashCountedSet() = default;

  void Trace(Visitor* visitor) const {
    CheckType();
    HashCountedSet<Value, HashFunctions, Traits, HeapAllocator>::Trace(visitor);
  }

 private:
  static constexpr void CheckType() {
    static_assert(WTF::IsMemberOrWeakMemberType<Value>::value,
                  "HeapHashCountedSet supports only Member and WeakMember.");
    static_assert(std::is_trivially_destructible<HeapHashCountedSet>::value,
                  "HeapHashCountedSet must be trivially destructible.");
    static_assert(WTF::IsTraceable<Value>::value,
                  "For counted sets without traceable elements, use "
                  "HashCountedSet<> instead of HeapHashCountedSet<>.");
  }
};

}  // namespace blink

namespace WTF {

template <typename Value,
          typename HashFunctions,
          typename Traits,
          typename VectorType>
inline void CopyToVector(
    const blink::HeapHashCountedSet<Value, HashFunctions, Traits>& set,
    VectorType& vector) {
  CopyToVector(static_cast<const HashCountedSet<Value, HashFunctions, Traits,
                                                blink::HeapAllocator>&>(set),
               vector);
}

}  // namespace WTF

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_COLLECTION_SUPPORT_HEAP_HASH_COUNTED_SET_H_
