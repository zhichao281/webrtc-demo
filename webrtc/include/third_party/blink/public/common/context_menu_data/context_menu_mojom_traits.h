// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_CONTEXT_MENU_DATA_CONTEXT_MENU_MOJOM_TRAITS_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_CONTEXT_MENU_DATA_CONTEXT_MENU_MOJOM_TRAITS_H_

#include "build/build_config.h"
#include "mojo/public/cpp/bindings/struct_traits.h"
#include "third_party/blink/public/common/common_export.h"
#include "third_party/blink/public/common/context_menu_data/untrustworthy_context_menu_params.h"
#include "third_party/blink/public/mojom/context_menu/context_menu.mojom.h"
#include "url/mojom/url_gurl_mojom_traits.h"

namespace mojo {

template <>
struct BLINK_COMMON_EXPORT
    StructTraits<blink::mojom::UntrustworthyContextMenuParamsDataView,
                 blink::UntrustworthyContextMenuParams> {
  static blink::mojom::ContextMenuDataMediaType media_type(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.media_type;
  }

  static int x(const blink::UntrustworthyContextMenuParams& r) { return r.x; }

  static int y(const blink::UntrustworthyContextMenuParams& r) { return r.y; }

  static const GURL link_url(const blink::UntrustworthyContextMenuParams& r) {
    return r.link_url;
  }

  static std::u16string link_text(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.link_text;
  }

  static base::Optional<blink::Impression> impression(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.impression;
  }

  static const GURL unfiltered_link_url(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.unfiltered_link_url;
  }

  static const GURL src_url(const blink::UntrustworthyContextMenuParams& r) {
    return r.src_url;
  }

  static bool has_image_contents(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.has_image_contents;
  }

  static int media_flags(const blink::UntrustworthyContextMenuParams& r) {
    return r.media_flags;
  }

  static std::u16string selection_text(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.selection_text;
  }

  static std::u16string title_text(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.title_text;
  }

  static std::u16string alt_text(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.alt_text;
  }

  static std::u16string suggested_filename(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.suggested_filename;
  }

  static std::u16string misspelled_word(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.misspelled_word;
  }

  static std::vector<std::u16string> dictionary_suggestions(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.dictionary_suggestions;
  }

  static bool spellcheck_enabled(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.spellcheck_enabled;
  }

  static bool is_editable(const blink::UntrustworthyContextMenuParams& r) {
    return r.is_editable;
  }

  static int writing_direction_default(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.writing_direction_default;
  }

  static int writing_direction_left_to_right(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.writing_direction_left_to_right;
  }

  static int writing_direction_right_to_left(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.writing_direction_right_to_left;
  }

  static int edit_flags(const blink::UntrustworthyContextMenuParams& r) {
    return r.edit_flags;
  }

  static std::string frame_charset(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.frame_charset;
  }

  static network::mojom::ReferrerPolicy referrer_policy(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.referrer_policy;
  }

  static const GURL& link_followed(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.link_followed;
  }

  static const std::vector<blink::mojom::CustomContextMenuItemPtr>&
  custom_items(const blink::UntrustworthyContextMenuParams& r) {
    return r.custom_items;
  }

  static ui::MenuSourceType source_type(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.source_type;
  }

  static blink::mojom::ContextMenuDataInputFieldType input_field_type(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.input_field_type;
  }

  static gfx::Rect selection_rect(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.selection_rect;
  }

  static int selection_start_offset(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.selection_start_offset;
  }

  static bool opened_from_highlight(
      const blink::UntrustworthyContextMenuParams& r) {
    return r.opened_from_highlight;
  }

  static bool Read(blink::mojom::UntrustworthyContextMenuParamsDataView r,
                   blink::UntrustworthyContextMenuParams* out);
};

}  // namespace mojo

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_CONTEXT_MENU_DATA_CONTEXT_MENU_MOJOM_TRAITS_H_
