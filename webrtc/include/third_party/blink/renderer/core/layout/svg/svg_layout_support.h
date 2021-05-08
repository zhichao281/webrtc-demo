/**
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Google, Inc.  All rights reserved.
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_SVG_SVG_LAYOUT_SUPPORT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_SVG_SVG_LAYOUT_SUPPORT_H_

#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/platform/graphics/dash_array.h"
#include "third_party/blink/renderer/platform/transforms/affine_transform.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"

namespace blink {

class AffineTransform;
class FloatPoint;
class FloatRect;
class LayoutBoxModelObject;
class LayoutObject;
class ComputedStyle;
class SVGLengthContext;
class StrokeData;
class TransformState;

class CORE_EXPORT SVGLayoutSupport {
  STATIC_ONLY(SVGLayoutSupport);

 public:
  // Helper function determining whether overflow is hidden.
  static bool IsOverflowHidden(const LayoutObject&);
  static bool IsOverflowHidden(const ComputedStyle&);

  // Adjusts the visual rect with clipper and masker in local coordinates.
  static void AdjustWithClipPathAndMask(const LayoutObject& layout_object,
                                        const FloatRect& object_bounding_box,
                                        FloatRect& visual_rect);

  // Add any contribution from 'stroke' to a text content bounding rect.
  static FloatRect ExtendTextBBoxWithStroke(const LayoutObject&,
                                            const FloatRect& text_bounds);

  // Compute the visual rect for the a text content LayoutObject.
  static FloatRect ComputeVisualRectForText(const LayoutObject&,
                                            const FloatRect& text_bounds);

  // Determine whether the passed location intersects a clip path referenced by
  // the passed LayoutObject.
  // |reference_box| is used to resolve 'objectBoundingBox' units/percentages,
  // and can differ from the reference box of the passed LayoutObject.
  static bool IntersectsClipPath(const LayoutObject&,
                                 const FloatRect& reference_box,
                                 const HitTestLocation&);

  // Important functions used by nearly all SVG layoutObjects centralizing
  // coordinate transformations / visual rect calculations
  static FloatRect LocalVisualRect(const LayoutObject&);
  static PhysicalRect VisualRectInAncestorSpace(
      const LayoutObject&,
      const LayoutBoxModelObject& ancestor,
      VisualRectFlags = kDefaultVisualRectFlags);
  static bool MapToVisualRectInAncestorSpace(
      const LayoutObject&,
      const LayoutBoxModelObject* ancestor,
      const FloatRect& local_visual_rect,
      PhysicalRect& result_rect,
      VisualRectFlags = kDefaultVisualRectFlags);
  static void MapLocalToAncestor(const LayoutObject*,
                                 const LayoutBoxModelObject* ancestor,
                                 TransformState&,
                                 MapCoordinatesFlags);
  static void MapAncestorToLocal(const LayoutObject&,
                                 const LayoutBoxModelObject* ancestor,
                                 TransformState&,
                                 MapCoordinatesFlags);

  // Shared between SVG layoutObjects and resources.
  static void ApplyStrokeStyleToStrokeData(StrokeData&,
                                           const ComputedStyle&,
                                           const LayoutObject&,
                                           float dash_scale_factor);

  static DashArray ResolveSVGDashArray(const SVGDashArray&,
                                       const ComputedStyle&,
                                       const SVGLengthContext&);

  // Determines if any ancestor has adjusted the scale factor.
  static bool ScreenScaleFactorChanged(const LayoutObject*);

  // Determines if any ancestor's layout size has changed.
  static bool LayoutSizeOfNearestViewportChanged(const LayoutObject*);

  // Helper method for determining if a LayoutObject marked as text (isText()==
  // true) can/will be laid out as part of a <text>.
  static bool IsLayoutableTextNode(const LayoutObject*);

  // Determines whether a svg node should isolate or not based on ComputedStyle.
  static bool WillIsolateBlendingDescendantsForStyle(const ComputedStyle&);
  static bool WillIsolateBlendingDescendantsForObject(const LayoutObject*);
  static bool IsIsolationRequired(const LayoutObject*);

  static AffineTransform DeprecatedCalculateTransformToLayer(
      const LayoutObject*);
  static float CalculateScreenFontSizeScalingFactor(const LayoutObject*);

  static LayoutObject* FindClosestLayoutSVGText(const LayoutObject*,
                                                const FloatPoint&);

  static void NotifySVGRootOfChangedCompositingReasons(const LayoutObject*);
};

class SubtreeContentTransformScope {
  STACK_ALLOCATED();

 public:
  SubtreeContentTransformScope(const AffineTransform&);
  ~SubtreeContentTransformScope();

  static AffineTransform CurrentContentTransformation() {
    return AffineTransform(current_content_transformation_);
  }

 private:
  static AffineTransform::Transform current_content_transformation_;
  AffineTransform saved_content_transformation_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_SVG_SVG_LAYOUT_SUPPORT_H_
