// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_CUSTOM_NG_CUSTOM_LAYOUT_ALGORITHM_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_CUSTOM_NG_CUSTOM_LAYOUT_ALGORITHM_H_

#include "third_party/blink/renderer/core/layout/ng/ng_layout_algorithm.h"

#include "third_party/blink/renderer/core/layout/ng/ng_box_fragment_builder.h"

namespace blink {

class NGBlockBreakToken;

class CORE_EXPORT NGCustomLayoutAlgorithm
    : public NGLayoutAlgorithm<NGBlockNode,
                               NGBoxFragmentBuilder,
                               NGBlockBreakToken> {
 public:
  NGCustomLayoutAlgorithm(const NGLayoutAlgorithmParams& params);

  MinMaxSizesResult ComputeMinMaxSizes(const MinMaxSizesFloatInput&) override;
  scoped_refptr<const NGLayoutResult> Layout() override;

 private:
  void AddAnyOutOfFlowPositionedChildren(NGLayoutInputNode* child);
  MinMaxSizesResult FallbackMinMaxSizes(const MinMaxSizesFloatInput&) const;
  scoped_refptr<const NGLayoutResult> FallbackLayout();

  const NGLayoutAlgorithmParams& params_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_CUSTOM_NG_CUSTOM_LAYOUT_ALGORITHM_H_
