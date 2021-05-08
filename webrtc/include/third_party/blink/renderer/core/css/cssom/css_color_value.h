// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_COLOR_VALUE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_COLOR_VALUE_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/cssom/css_numeric_value.h"
#include "third_party/blink/renderer/core/css/cssom/css_style_value.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {

class Color;
class CSSNumericValue;
class CSSRGB;
class CSSHSL;

class CORE_EXPORT CSSColorValue : public CSSStyleValue {
  DEFINE_WRAPPERTYPEINFO();

 public:
  CSSRGB* toRGB() const;
  CSSHSL* toHSL() const;

  const CSSValue* ToCSSValue() const override;

  StyleValueType GetType() const override { return kColorType; }

  virtual Color ToColor() const = 0;

 protected:
  static CSSNumericValue* ToNumberOrPercentage(const CSSNumberish&);
  static CSSNumericValue* ToPercentage(const CSSNumberish&);
  static float ComponentToColorInput(CSSNumericValue*);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_COLOR_VALUE_H_
