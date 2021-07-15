// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_SANITIZER_API_ELEMENT_SANITIZER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_SANITIZER_API_ELEMENT_SANITIZER_H_

#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class Element;
class ExceptionState;
class Sanitizer;
class ScriptState;

class MODULES_EXPORT ElementSanitizer final {
 public:
  static void SetHTML(ScriptState*,
                      Element&,
                      const String&,
                      Sanitizer*,
                      ExceptionState&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_SANITIZER_API_ELEMENT_SANITIZER_H_
