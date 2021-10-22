// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_OUTLINE_PAINTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_OUTLINE_PAINTER_H_

#include "base/callback.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#include "third_party/skia/include/core/SkPoint.h"

class SkPath;

namespace blink {

class ComputedStyle;
class DisplayItemClient;
class GraphicsContext;
class Path;
struct PaintInfo;
struct PhysicalRect;

class CORE_EXPORT OutlinePainter {
  STATIC_ONLY(OutlinePainter);

 public:
  static void PaintOutlineRects(const PaintInfo&,
                                const DisplayItemClient&,
                                const Vector<PhysicalRect>&,
                                const ComputedStyle&);

  static void PaintFocusRingPath(GraphicsContext&,
                                 const Path&,
                                 const ComputedStyle&);

  static int OutlineOutsetExtent(const ComputedStyle&);

  // For pre-CompositeAfterPaint only.
  static int FocusRingWidthInsideBorderBox(const ComputedStyle&);

  struct Line {
    SkPoint start;
    SkPoint end;
  };
  static void IterateRightAnglePathForTesting(
      const SkPath&,
      const base::RepeatingCallback<void(const Vector<Line>&)>& contour_action);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_OUTLINE_PAINTER_H_
