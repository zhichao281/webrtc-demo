// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SHAPING_FONT_FEATURES_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SHAPING_FONT_FEATURES_H_

#include <hb.h>

#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class Font;

// Represents a list of OpenType font feature settings.
class FontFeatures {
  STACK_ALLOCATED();

 public:
  // Initialize the list from |Font|.
  void Initialize(const Font&);

  wtf_size_t size() const { return features_.size(); }
  bool IsEmpty() const { return features_.IsEmpty(); }

  const hb_feature_t* data() const { return features_.data(); }

  void Append(const hb_feature_t& feature) { features_.push_back(feature); }
  void Insert(const hb_feature_t& feature) { features_.push_front(feature); }

  void EraseAt(wtf_size_t position, wtf_size_t length) {
    features_.EraseAt(position, length);
  }

 private:
  Vector<hb_feature_t, 6> features_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SHAPING_FONT_FEATURES_H_
