/*
 * Copyright (C) 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 David Smith <catfish.man@gmail.com>
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
 *
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_ELEMENT_RARE_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_ELEMENT_RARE_DATA_H_

#include <memory>
#include "base/token.h"
#include "third_party/blink/renderer/core/animation/element_animations.h"
#include "third_party/blink/renderer/core/aom/accessible_node.h"
#include "third_party/blink/renderer/core/css/container_query_data.h"
#include "third_party/blink/renderer/core/css/cssom/inline_style_property_map.h"
#include "third_party/blink/renderer/core/css/inline_css_style_declaration.h"
#include "third_party/blink/renderer/core/display_lock/display_lock_context.h"
#include "third_party/blink/renderer/core/dom/attr.h"
#include "third_party/blink/renderer/core/dom/dataset_dom_string_map.h"
#include "third_party/blink/renderer/core/dom/dom_token_list.h"
#include "third_party/blink/renderer/core/dom/named_node_map.h"
#include "third_party/blink/renderer/core/dom/names_map.h"
#include "third_party/blink/renderer/core/dom/node_rare_data.h"
#include "third_party/blink/renderer/core/dom/pseudo_element.h"
#include "third_party/blink/renderer/core/dom/pseudo_element_data.h"
#include "third_party/blink/renderer/core/dom/region_capture_crop_id.h"
#include "third_party/blink/renderer/core/dom/shadow_root.h"
#include "third_party/blink/renderer/core/dom/space_split_string.h"
#include "third_party/blink/renderer/core/html/custom/custom_element_definition.h"
#include "third_party/blink/renderer/core/intersection_observer/element_intersection_observer_data.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"

namespace blink {

class ContainerQueryData;
class Element;
class HTMLElement;
class ResizeObservation;
class ResizeObserver;

// Element rare data is intended to store values that are not frequently
// set on elements, but need to be associated with them still.
// IMPORTANT NOTE: do NOT use element rare data to store boolean values,
// instead favoring the |ElementFlags| class defined in element.h.
class ElementRareData final : public NodeRareData {
 public:
  explicit ElementRareData(NodeRenderingData*);
  ~ElementRareData();

  void SetPseudoElement(PseudoId, PseudoElement*);
  PseudoElement* GetPseudoElement(PseudoId) const;
  PseudoElementData::PseudoElementVector GetPseudoElements() const;

  void SetTabIndexExplicitly() {
    SetElementFlag(ElementFlags::kTabIndexWasSetExplicitly, true);
  }

  void ClearTabIndexExplicitly() {
    ClearElementFlag(ElementFlags::kTabIndexWasSetExplicitly);
  }

  CSSStyleDeclaration& EnsureInlineCSSStyleDeclaration(Element* owner_element);
  InlineStylePropertyMap& EnsureInlineStylePropertyMap(Element* owner_element);

  InlineStylePropertyMap* GetInlineStylePropertyMap() {
    return cssom_map_wrapper_.Get();
  }

  ShadowRoot* GetShadowRoot() const { return shadow_root_.Get(); }
  void SetShadowRoot(ShadowRoot& shadow_root) {
    DCHECK(!shadow_root_);
    shadow_root_ = &shadow_root;
  }

  EditContext* GetEditContext() const { return edit_context_.Get(); }
  void SetEditContext(EditContext* edit_context) {
    edit_context_ = edit_context;
  }

  NamedNodeMap* AttributeMap() const { return attribute_map_.Get(); }
  void SetAttributeMap(NamedNodeMap* attribute_map) {
    attribute_map_ = attribute_map;
  }

  DOMTokenList* GetClassList() const { return class_list_.Get(); }
  void SetClassList(DOMTokenList* class_list) {
    class_list_ = class_list;
  }

  void SetPart(DOMTokenList* part) {
    part_ = part;
  }
  DOMTokenList* GetPart() const { return part_.Get(); }

  void SetPartNamesMap(const AtomicString part_names) {
    if (!part_names_map_) {
      part_names_map_.reset(new NamesMap());
    }
    part_names_map_->Set(part_names);
  }
  const NamesMap* PartNamesMap() const { return part_names_map_.get(); }

  DatasetDOMStringMap* Dataset() const { return dataset_.Get(); }
  void SetDataset(DatasetDOMStringMap* dataset) {
    dataset_ = dataset;
  }

  ScrollOffset SavedLayerScrollOffset() const {
    return saved_layer_scroll_offset_;
  }
  void SetSavedLayerScrollOffset(ScrollOffset offset) {
    saved_layer_scroll_offset_ = offset;
  }

  ElementAnimations* GetElementAnimations() {
    return element_animations_.Get();
  }
  void SetElementAnimations(ElementAnimations* element_animations) {
    element_animations_ = element_animations;
  }

  bool HasPseudoElements() const;
  void ClearPseudoElements();

  void SetCustomElementDefinition(CustomElementDefinition* definition) {
    custom_element_definition_ = definition;
  }
  CustomElementDefinition* GetCustomElementDefinition() const {
    return custom_element_definition_.Get();
  }
  void SetIsValue(const AtomicString& is_value) { is_value_ = is_value; }
  const AtomicString& IsValue() const { return is_value_; }
  ElementInternals& EnsureElementInternals(HTMLElement& target);
  const ElementInternals* GetElementInternals() const {
    return element_internals_;
  }

  RegionCaptureCropId* GetRegionCaptureCropId() const {
    return region_capture_crop_id_.get();
  }
  void SetRegionCaptureCropId(std::unique_ptr<RegionCaptureCropId> value) {
    region_capture_crop_id_ = std::move(value);
  }

  AccessibleNode* GetAccessibleNode() const { return accessible_node_.Get(); }
  AccessibleNode* EnsureAccessibleNode(Element* owner_element) {
    if (!accessible_node_) {
      accessible_node_ = MakeGarbageCollected<AccessibleNode>(owner_element);
    }
    return accessible_node_;
  }

  AttrNodeList& EnsureAttrNodeList();
  AttrNodeList* GetAttrNodeList() { return attr_node_list_.Get(); }
  void RemoveAttrNodeList() { attr_node_list_.Clear(); }
  void AddAttr(Attr* attr) {
    EnsureAttrNodeList().push_back(attr);
  }

  ElementIntersectionObserverData* IntersectionObserverData() const {
    return intersection_observer_data_.Get();
  }
  ElementIntersectionObserverData& EnsureIntersectionObserverData() {
    if (!intersection_observer_data_) {
      intersection_observer_data_ =
          MakeGarbageCollected<ElementIntersectionObserverData>();
    }
    return *intersection_observer_data_;
  }

  using ResizeObserverDataMap =
      HeapHashMap<Member<ResizeObserver>, Member<ResizeObservation>>;

  ResizeObserverDataMap* ResizeObserverData() const {
    return resize_observer_data_;
  }
  ResizeObserverDataMap& EnsureResizeObserverData();

  DisplayLockContext* EnsureDisplayLockContext(Element* element) {
    if (!display_lock_context_) {
      display_lock_context_ = MakeGarbageCollected<DisplayLockContext>(element);
    }
    return display_lock_context_.Get();
  }
  DisplayLockContext* GetDisplayLockContext() const {
    return display_lock_context_;
  }

  ContainerQueryData& EnsureContainerQueryData() {
    if (!container_query_data_)
      container_query_data_ = MakeGarbageCollected<ContainerQueryData>();
    return *container_query_data_;
  }
  ContainerQueryData* GetContainerQueryData() const {
    return container_query_data_;
  }

  ContainerQueryEvaluator* GetContainerQueryEvaluator() const {
    if (!container_query_data_)
      return nullptr;
    return container_query_data_->GetContainerQueryEvaluator();
  }
  void SetContainerQueryEvaluator(ContainerQueryEvaluator* evaluator) {
    if (container_query_data_)
      container_query_data_->SetContainerQueryEvaluator(evaluator);
    else if (evaluator)
      EnsureContainerQueryData().SetContainerQueryEvaluator(evaluator);
  }

  const AtomicString& GetNonce() const { return nonce_; }
  void SetNonce(const AtomicString& nonce) { nonce_ = nonce; }

  void TraceAfterDispatch(blink::Visitor*) const;

 private:
  ScrollOffset saved_layer_scroll_offset_;
  AtomicString nonce_;

  Member<DatasetDOMStringMap> dataset_;
  Member<ShadowRoot> shadow_root_;
  Member<EditContext> edit_context_;
  Member<DOMTokenList> class_list_;
  Member<DOMTokenList> part_;
  std::unique_ptr<NamesMap> part_names_map_;
  Member<NamedNodeMap> attribute_map_;
  Member<AttrNodeList> attr_node_list_;
  Member<InlineCSSStyleDeclaration> cssom_wrapper_;
  Member<InlineStylePropertyMap> cssom_map_wrapper_;

  Member<ElementAnimations> element_animations_;
  Member<ElementIntersectionObserverData> intersection_observer_data_;
  Member<ResizeObserverDataMap> resize_observer_data_;

  Member<CustomElementDefinition> custom_element_definition_;
  AtomicString is_value_;
  Member<ElementInternals> element_internals_;

  Member<PseudoElementData> pseudo_element_data_;

  Member<AccessibleNode> accessible_node_;

  Member<DisplayLockContext> display_lock_context_;
  Member<ContainerQueryData> container_query_data_;
  std::unique_ptr<RegionCaptureCropId> region_capture_crop_id_;
};

inline LayoutSize DefaultMinimumSizeForResizing() {
  return LayoutSize(LayoutUnit::Max(), LayoutUnit::Max());
}

inline bool ElementRareData::HasPseudoElements() const {
  return (pseudo_element_data_ && pseudo_element_data_->HasPseudoElements());
}

inline void ElementRareData::ClearPseudoElements() {
  if (pseudo_element_data_) {
    pseudo_element_data_->ClearPseudoElements();
    pseudo_element_data_.Clear();
  }
}

inline void ElementRareData::SetPseudoElement(PseudoId pseudo_id,
                                              PseudoElement* element) {
  if (!pseudo_element_data_) {
    if (!element)
      return;
    pseudo_element_data_ = MakeGarbageCollected<PseudoElementData>();
  }
  pseudo_element_data_->SetPseudoElement(pseudo_id, element);
}

inline PseudoElement* ElementRareData::GetPseudoElement(
    PseudoId pseudo_id) const {
  if (!pseudo_element_data_)
    return nullptr;
  return pseudo_element_data_->GetPseudoElement(pseudo_id);
}

inline PseudoElementData::PseudoElementVector
ElementRareData::GetPseudoElements() const {
  if (!pseudo_element_data_)
    return {};
  return pseudo_element_data_->GetPseudoElements();
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_ELEMENT_RARE_DATA_H_
