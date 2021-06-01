// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_NG_NG_HIGHLIGHT_PAINTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_NG_NG_HIGHLIGHT_PAINTER_H_

#include "third_party/abseil-cpp/absl/types/optional.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/editing/frame_selection.h"
#include "third_party/blink/renderer/core/editing/markers/document_marker.h"
#include "third_party/blink/renderer/core/layout/api/selection_state.h"
#include "third_party/blink/renderer/core/layout/geometry/physical_rect.h"
#include "third_party/blink/renderer/core/paint/text_paint_style.h"
#include "third_party/blink/renderer/platform/graphics/dom_node_id.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"

namespace blink {

class AffineTransform;
class ComputedStyle;
class FrameSelection;
class LayoutObject;
class NGFragmentItem;
class NGTextPainter;
class NGInlineCursor;
class Node;
struct LayoutSelectionStatus;
struct PaintInfo;
struct PhysicalOffset;

// Highlight overlay painter for LayoutNG. Operates on NGFragmentItem that
// IsText(). Delegates to NGTextPainter to paint the text itself.
class CORE_EXPORT NGHighlightPainter {
  STACK_ALLOCATED();

 public:
  class SelectionPaintState {
    STACK_ALLOCATED();

   public:
    explicit SelectionPaintState(const NGInlineCursor& containing_block);
    explicit SelectionPaintState(const NGInlineCursor& containing_block,
                                 const FrameSelection&);

    const LayoutSelectionStatus& Status() const { return selection_status_; }

    const TextPaintStyle& GetSelectionStyle() const { return selection_style_; }

    SelectionState State() const { return state_; }

    bool ShouldPaintSelectedTextOnly() const {
      return paint_selected_text_only_;
    }

    void ComputeSelectionStyle(const Document& document,
                               const ComputedStyle& style,
                               Node* node,
                               const PaintInfo& paint_info,
                               const TextPaintStyle& text_style);

    PhysicalRect ComputeSelectionRect(const PhysicalOffset& box_offset);

    void PaintSelectionBackground(
        GraphicsContext& context,
        Node* node,
        const Document& document,
        const ComputedStyle& style,
        const absl::optional<AffineTransform>& rotation);

    void MapSelectionRectIntoRotatedSpace(const AffineTransform& rotation);

    void PaintSelectedText(NGTextPainter& text_painter,
                           unsigned length,
                           const TextPaintStyle& text_style,
                           DOMNodeId node_id);

    void PaintSuppressingTextProperWhereSelected(
        NGTextPainter& text_painter,
        unsigned start_offset,
        unsigned end_offset,
        unsigned length,
        const TextPaintStyle& text_style,
        DOMNodeId node_id);

   private:
    const LayoutSelectionStatus selection_status_;
    TextPaintStyle selection_style_;
    const SelectionState state_;
    absl::optional<PhysicalRect> selection_rect_;
    absl::optional<PhysicalRect> selection_rect_before_rotation_;
    const NGInlineCursor& containing_block_;
    bool paint_selected_text_only_;
  };

  explicit NGHighlightPainter(NGTextPainter& text_painter,
                              const PaintInfo& paint_info,
                              const NGInlineCursor& cursor,
                              const NGFragmentItem& fragment_item,
                              const PhysicalOffset& box_origin,
                              const ComputedStyle& style,
                              absl::optional<SelectionPaintState>,
                              bool is_printing);

  enum Phase { kBackground, kForeground };
  void Paint(Phase phase);

  absl::optional<SelectionPaintState>& Selection() { return selection_; }
  absl::optional<AppliedTextDecoration> SelectionDecoration() {
    return selection_
               ? selection_->GetSelectionStyle().selection_text_decoration
               : absl::nullopt;
  }

 private:
  NGTextPainter& text_painter_;
  const PaintInfo& paint_info_;
  const NGInlineCursor& cursor_;
  const NGFragmentItem& fragment_item_;
  const PhysicalOffset& box_origin_;
  const ComputedStyle& style_;
  absl::optional<SelectionPaintState> selection_;
  const LayoutObject* layout_object_;
  Node* node_;
  const DocumentMarkerVector markers_;
  const bool skip_backgrounds_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_NG_NG_HIGHLIGHT_PAINTER_H_
