// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_HSL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_HSL_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/cssom/css_color_value.h"
#include "third_party/blink/renderer/core/css/cssom/css_numeric_value.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {

// Defines a javascript HSL color interface.
// https://drafts.css-houdini.org/css-typed-om-1/#csshsl
class CORE_EXPORT CSSHSL final : public CSSColorValue {
  DEFINE_WRAPPERTYPEINFO();

 public:
  // Constructor defined in the IDL.
  static CSSHSL* Create(CSSNumericValue* hue,
                        const CSSNumberish& saturation,
                        const CSSNumberish& lightness,
                        const CSSNumberish& alpha,
                        ExceptionState& exception_state);

  // Internal constructor used by blink.
  explicit CSSHSL(const Color&);
  CSSHSL(CSSNumericValue*,
         CSSNumericValue*,
         CSSNumericValue*,
         CSSNumericValue*);

  // Getters and setters from the IDL
  Member<CSSNumericValue> h() { return h_; }
  void s(CSSNumberish& g) { g.SetCSSNumericValue(s_); }
  void l(CSSNumberish& b) { b.SetCSSNumericValue(l_); }
  void alpha(CSSNumberish& alpha) { alpha.SetCSSNumericValue(alpha_); }
  void setH(CSSNumericValue*, ExceptionState&);
  void setS(const CSSNumberish&, ExceptionState&);
  void setL(const CSSNumberish&, ExceptionState&);
  void setAlpha(const CSSNumberish&, ExceptionState&);

  void Trace(Visitor* visitor) const override {
    visitor->Trace(h_);
    visitor->Trace(s_);
    visitor->Trace(l_);
    visitor->Trace(alpha_);
    CSSColorValue::Trace(visitor);
  }

  Color ToColor() const final;

 private:
  Member<CSSNumericValue> h_;
  Member<CSSNumericValue> s_;
  Member<CSSNumericValue> l_;
  Member<CSSNumericValue> alpha_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_HSL_H_
