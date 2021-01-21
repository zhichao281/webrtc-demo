// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_V8_WRAPPER_PERSISTENT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_V8_WRAPPER_PERSISTENT_H_

#include "base/bind.h"
#include "third_party/blink/renderer/platform/bindings/buildflags.h"
#include "third_party/blink/renderer/platform/wtf/type_traits.h"
#include "v8/include/cppgc/cross-thread-persistent.h"
#include "v8/include/cppgc/persistent.h"
#include "v8/include/cppgc/source-location.h"

namespace blink {

template <typename T>
using Persistent = cppgc::Persistent<T>;

template <typename T>
using WeakPersistent = cppgc::WeakPersistent<T>;

template <typename T>
using CrossThreadPersistent = cppgc::subtle::CrossThreadPersistent<T>;

template <typename T>
using CrossThreadWeakPersistent = cppgc::subtle::WeakCrossThreadPersistent<T>;

using PersistentLocation = cppgc::SourceLocation;

template <typename T>
Persistent<T> WrapPersistent(
    T* value,
    const cppgc::SourceLocation& loc = cppgc::SourceLocation::Current()) {
  return Persistent<T>(value, loc);
}

template <typename T>
WeakPersistent<T> WrapWeakPersistent(
    T* value,
    const cppgc::SourceLocation& loc = cppgc::SourceLocation::Current()) {
  return WeakPersistent<T>(value, loc);
}

template <typename T>
CrossThreadPersistent<T> WrapCrossthreadPersistent(
    T* value,
    const cppgc::SourceLocation& loc = cppgc::SourceLocation::Current()) {
  return CrossThreadPersistent<T>(value, loc);
}

template <typename T>
CrossThreadWeakPersistent<T> WrapCrossThreadWeakPersistent(
    T* value,
    const cppgc::SourceLocation& loc = cppgc::SourceLocation::Current()) {
  return CrossThreadWeakPersistent<T>(value, loc);
}

#if BUILDFLAG(RAW_HEAP_SNAPSHOTS)
#define PERSISTENT_FROM_HERE PersistentLocation::Current()
#else
#define PERSISTENT_FROM_HERE PersistentLocation()
#endif  // BUILDFLAG(RAW_HEAP_SNAPSHOTS)

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_V8_WRAPPER_PERSISTENT_H_
