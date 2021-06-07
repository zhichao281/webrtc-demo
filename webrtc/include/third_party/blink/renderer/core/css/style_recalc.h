// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_RECALC_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_RECALC_H_

#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"

namespace blink {

class Element;
class Node;
class PseudoElement;

// Class for keeping track of the need for traversing down flat tree children,
// recompute their computed styles, and marking nodes for layout tree re-
// attachment during the style recalc phase.
class StyleRecalcChange {
 private:
  enum ContainerQueryDependentFlag {
    kNoContainerRecalc,
    // Recalc container query dependent elements within this container,
    // but not in nested containers.
    kRecalcContainer,
    // Recalc container query dependent elements within this container,
    // and also in nested containers.
    kRecalcDescendantContainers,
  };

 public:
  enum Propagate {
    // No need to update style of any children.
    kNo,
    // Need to traverse children in display:none or non-slotted/distributed
    // children of shadow hosts to clear ensured computed styles.
    kClearEnsured,
    // Need to traverse descendants to invalidate style for container queries.
    // This value is passed in for the container itself, it will translate into
    // recalc_container_query_dependent_=true for descendants. We should not
    // recalc style for the container itself.
    kRecalcContainerQueryDependent,
    // Like kRecalcContainerQueryDependent, but we do not stop traversal on
    // descendant containers.
    kRecalcDescendantContainerQueryDependent,
    // Need to update existence and style for pseudo elements.
    kUpdatePseudoElements,
    // Need to recalculate style for children for inheritance. All changed
    // inherited properties can be propagated (PropagateInheritedProperties)
    // instead of a full rule matching.
    kIndependentInherit,
    // Need to recalculate style for children, typically for inheritance.
    kRecalcChildren,
    // Need to recalculate style for all descendants.
    kRecalcDescendants,
  };

  StyleRecalcChange() = default;
  StyleRecalcChange(const StyleRecalcChange&) = default;
  StyleRecalcChange(Propagate propagate) : propagate_(propagate) {}

  StyleRecalcChange ForChildren(const Element& element) const {
    return {RecalcDescendants() ? kRecalcDescendants : kNo, reattach_,
            ContainerQueryDependentFlagForChildren(element)};
  }
  StyleRecalcChange ForPseudoElement() const {
    if (propagate_ == kUpdatePseudoElements)
      return {kRecalcChildren, reattach_, container_query_dependent_flag_};
    return *this;
  }
  StyleRecalcChange EnsureAtLeast(Propagate propagate) const {
    if (propagate > propagate_)
      return {propagate, reattach_, container_query_dependent_flag_};
    return {propagate_, reattach_, container_query_dependent_flag_};
  }
  StyleRecalcChange ForceRecalcDescendants() const {
    return {kRecalcDescendants, reattach_, container_query_dependent_flag_};
  }
  StyleRecalcChange ForceReattachLayoutTree() const {
    return {propagate_, true, container_query_dependent_flag_};
  }

  bool ReattachLayoutTree() const { return reattach_; }
  bool RecalcChildren() const { return propagate_ > kUpdatePseudoElements; }
  bool RecalcDescendants() const { return propagate_ == kRecalcDescendants; }
  bool UpdatePseudoElements() const { return propagate_ != kNo; }
  bool IndependentInherit() const { return propagate_ == kIndependentInherit; }
  bool TraverseChildren(const Element&) const;
  bool TraverseChild(const Node&) const;
  bool TraversePseudoElements(const Element&) const;
  bool ShouldRecalcStyleFor(const Node&) const;
  bool ShouldUpdatePseudoElement(const PseudoElement&) const;

 private:
  StyleRecalcChange(Propagate propagate,
                    bool reattach,
                    ContainerQueryDependentFlag container_query_dependent_flag)
      : propagate_(propagate),
        reattach_(reattach),
        container_query_dependent_flag_(container_query_dependent_flag) {}

  bool RecalcContainerQueryDependent() const {
    return container_query_dependent_flag_ != kNoContainerRecalc;
  }
  ContainerQueryDependentFlag ContainerQueryDependentFlagForChildren(
      const Element&) const;

  // To what extent do we need to update style for children.
  Propagate propagate_ = kNo;
  // Need to reattach layout tree if true.
  bool reattach_ = false;
  // Force recalc of elements depending on container queries.
  ContainerQueryDependentFlag container_query_dependent_flag_ =
      kNoContainerRecalc;
};

// StyleRecalcContext is an object that is passed on the stack during
// the style recalc process.
//
// Its purpose is to hold context related to the style recalc process as
// a whole, i.e. information not directly associated to the specific element
// style is being calculated for.
class StyleRecalcContext {
  STACK_ALLOCATED();

 public:
  // Using the ancestor chain, build a StyleRecalcContext suitable for
  // resolving the style of the given Element.
  static StyleRecalcContext FromAncestors(Element&);

  // Set to the nearest container (for container queries), if any.
  // This is used to evaluate container queries in ElementRuleCollector.
  Element* container = nullptr;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_RECALC_H_
