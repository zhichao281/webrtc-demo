// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBCODECS_HARDWARE_PREFERENCE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBCODECS_HARDWARE_PREFERENCE_H_

#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

enum class HardwarePreference { kAllow, kDeny, kRequire };

HardwarePreference StringToHardwarePreference(const String& value);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBCODECS_HARDWARE_PREFERENCE_H_