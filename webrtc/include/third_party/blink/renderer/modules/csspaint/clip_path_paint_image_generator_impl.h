// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_CLIP_PATH_PAINT_IMAGE_GENERATOR_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_CLIP_PATH_PAINT_IMAGE_GENERATOR_IMPL_H_

#include "third_party/blink/renderer/core/css/clip_path_paint_image_generator.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/geometry/float_size.h"

namespace blink {

class Image;
class ClipPathPaintWorklet;

class MODULES_EXPORT ClipPathPaintImageGeneratorImpl final
    : public ClipPathPaintImageGenerator {
 public:
  static ClipPathPaintImageGenerator* Create(LocalFrame&);

  explicit ClipPathPaintImageGeneratorImpl(ClipPathPaintWorklet*);
  ~ClipPathPaintImageGeneratorImpl() override = default;

  scoped_refptr<Image> Paint(float zoom,
                             const FloatRect& reference_box,
                             const Node&) final;

  void Shutdown() final;

  void Trace(Visitor*) const override;

 private:
  Member<ClipPathPaintWorklet> clip_path_paint_worklet_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_CLIP_PATH_PAINT_IMAGE_GENERATOR_IMPL_H_
