// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_RGB_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_RGB_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/cssom/css_color_value.h"
#include "third_party/blink/renderer/core/css/cssom/css_numeric_value.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {

// Defines a javascript RGB color interface.
// https://drafts.css-houdini.org/css-typed-om-1/#dom-cssrgb-r
class CORE_EXPORT CSSRGB final : public CSSColorValue {
  DEFINE_WRAPPERTYPEINFO();

 public:
  // Constructor defined in the IDL.
  static CSSRGB* Create(const CSSNumberish&,
                        const CSSNumberish&,
                        const CSSNumberish&,
                        const CSSNumberish&,
                        ExceptionState&);

  // Internal constructor used by blink.
  explicit CSSRGB(const Color&);
  CSSRGB(CSSNumericValue*,
         CSSNumericValue*,
         CSSNumericValue*,
         CSSNumericValue*);

  // Getters and setters from the IDL
  void r(CSSNumberish& r) { r.SetCSSNumericValue(r_); }
  void g(CSSNumberish& g) { g.SetCSSNumericValue(g_); }
  void b(CSSNumberish& b) { b.SetCSSNumericValue(b_); }
  void alpha(CSSNumberish& alpha) { alpha.SetCSSNumericValue(alpha_); }
  void setR(const CSSNumberish&, ExceptionState&);
  void setG(const CSSNumberish&, ExceptionState&);
  void setB(const CSSNumberish&, ExceptionState&);
  void setAlpha(const CSSNumberish&, ExceptionState&);

  void Trace(Visitor* visitor) const override {
    visitor->Trace(r_);
    visitor->Trace(g_);
    visitor->Trace(b_);
    visitor->Trace(alpha_);
    CSSColorValue::Trace(visitor);
  }

  Color ToColor() const final;

 private:
  Member<CSSNumericValue> r_;
  Member<CSSNumericValue> g_;
  Member<CSSNumericValue> b_;
  Member<CSSNumericValue> alpha_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_RGB_H_
