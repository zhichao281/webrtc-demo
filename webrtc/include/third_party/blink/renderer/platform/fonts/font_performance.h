// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_FONT_PERFORMANCE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_FONT_PERFORMANCE_H_

#include "base/time/time.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/wtf.h"

namespace blink {

// This class collects performance data for font-related operations.
class PLATFORM_EXPORT FontPerformance {
 public:
  // The aggregated time spent in |DeterminePrimarySimpleFontData|.
  static base::TimeDelta PrimaryFontTime() {
    return primary_font_ + primary_font_in_style_;
  }
  static const base::TimeDelta& PrimaryFontTimeInStyle() {
    return primary_font_in_style_;
  }
  static void AddPrimaryFontTime(base::TimeDelta time) {
    if (UNLIKELY(!IsMainThread()))
      return;
    if (in_style_)
      primary_font_in_style_ += time;
    else
      primary_font_ += time;
  }

  class StyleScope {
   public:
    StyleScope() { ++in_style_; }
    ~StyleScope() {
      DCHECK(in_style_);
      --in_style_;
    }
  };

 private:
  static base::TimeDelta primary_font_;
  static base::TimeDelta primary_font_in_style_;
  static unsigned in_style_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_FONT_PERFORMANCE_H_
