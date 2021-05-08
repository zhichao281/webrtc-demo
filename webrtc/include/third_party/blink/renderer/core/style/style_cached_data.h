// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_STYLE_CACHED_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_STYLE_CACHED_DATA_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class ComputedStyle;

using PseudoElementStyleCache = HeapVector<Member<const ComputedStyle>, 4>;

class CORE_EXPORT StyleCachedData final
    : public GarbageCollected<StyleCachedData> {
 public:
  void Trace(Visitor*) const;

 private:
  friend class ComputedStyle;

  // This cache stores ComputedStyles for pseudo elements originating from this
  // ComputedStyle's element. Pseudo elements which are represented by
  // PseudoElement in DOM store the ComputedStyle on those elements, so this
  // cache is for:
  //
  // 1. Pseudo elements which do not generate a PseudoElement internally like
  //    ::first-line and ::selection.
  //
  // 2. Pseudo element style requested from getComputedStyle() where the element
  //    currently doesn't generate a PseudoElement. E.g.:
  //
  //    <style>
  //      #div::before { color: green /* no content property! */}
  //    </style>
  //    <div id=div></div>
  //    <script>
  //      getComputedStyle(div, "::before").color // still green.
  //    </script>
  Member<PseudoElementStyleCache> pseudo_element_styles_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_STYLE_CACHED_DATA_H_