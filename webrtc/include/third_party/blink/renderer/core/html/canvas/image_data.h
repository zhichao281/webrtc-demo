/*
 * Copyright (C) 2008, 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CANVAS_IMAGE_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CANVAS_IMAGE_DATA_H_

#include "base/numerics/checked_math.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_image_data_settings.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_typedefs.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/html/canvas/canvas_rendering_context.h"
#include "third_party/blink/renderer/core/imagebitmap/image_bitmap_source.h"
#include "third_party/blink/renderer/core/typed_arrays/array_buffer_view_helpers.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_typed_array.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/geometry/int_rect.h"
#include "third_party/blink/renderer/platform/geometry/int_size.h"
#include "third_party/blink/renderer/platform/graphics/canvas_color_params.h"
#include "third_party/blink/renderer/platform/graphics/static_bitmap_image.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/skia/include/core/SkColorSpace.h"

namespace blink {

class ExceptionState;
class ImageBitmapOptions;

constexpr const char* kUint8ClampedArrayStorageFormatName = "uint8";
constexpr const char* kUint16ArrayStorageFormatName = "uint16";
constexpr const char* kFloat32ArrayStorageFormatName = "float32";

// Convert a string to an ImageDataStorageFormat. On unrecognized strings this
// will return kUint8ClampedArrayStorageFormat.
ImageDataStorageFormat CORE_EXPORT
ImageDataStorageFormatFromName(const String& string);
String CORE_EXPORT
ImageDataStorageFormatToName(ImageDataStorageFormat storage_format);

class CORE_EXPORT ImageData final : public ScriptWrappable,
                                    public ImageBitmapSource {
  DEFINE_WRAPPERTYPEINFO();

 public:
  // Constructors that take width, height, and an optional ImageDataSettings.
  static ImageData* Create(unsigned width,
                           unsigned height,
                           ExceptionState& exception_state) {
    return ValidateAndCreate(width, height, absl::nullopt, /*settings=*/nullptr,
                             ValidateAndCreateParams(), exception_state);
  }
  static ImageData* Create(unsigned width,
                           unsigned height,
                           const ImageDataSettings* settings,
                           ExceptionState& exception_state) {
    ValidateAndCreateParams params;
    params.require_canvas_color_management = true;
    return ValidateAndCreate(width, height, absl::nullopt, settings, params,
                             exception_state);
  }

  // Constructors that take Uint8ClampedArray, width, optional height, and
  // optional ImageDataSettings.
  static ImageData* Create(NotShared<DOMUint8ClampedArray> data,
                           unsigned width,
                           ExceptionState& exception_state) {
    return ValidateAndCreate(width, absl::nullopt, data, nullptr,
                             ValidateAndCreateParams(), exception_state);
  }
  static ImageData* Create(NotShared<DOMUint8ClampedArray> data,
                           unsigned width,
                           unsigned height,
                           ExceptionState& exception_state) {
    return ValidateAndCreate(width, height, data, /*settings=*/nullptr,
                             ValidateAndCreateParams(), exception_state);
  }
  static ImageData* Create(NotShared<DOMUint8ClampedArray> data,
                           unsigned width,
                           unsigned height,
                           const ImageDataSettings* settings,
                           ExceptionState& exception_state) {
    ValidateAndCreateParams params;
    params.require_canvas_color_management = true;
    return ValidateAndCreate(width, height, data, settings, params,
                             exception_state);
  }

  // Constructor that takes DOMUint16Array, width, optional height, and optional
  // ImageDataSettings.
  static ImageData* Create(NotShared<DOMUint16Array> data,
                           unsigned width,
                           ExceptionState& exception_state) {
    ValidateAndCreateParams params;
    params.require_canvas_color_management_v2 = true;
    return ValidateAndCreate(width, absl::nullopt, data, nullptr, params,
                             exception_state);
  }
  static ImageData* Create(NotShared<DOMUint16Array> data,
                           unsigned width,
                           unsigned height,
                           const ImageDataSettings* settings,
                           ExceptionState& exception_state) {
    ValidateAndCreateParams params;
    params.require_canvas_color_management_v2 = true;
    return ValidateAndCreate(width, height, data, settings, params,
                             exception_state);
  }

  // Constructor that takes DOMFloat32Array, width, optional height, and
  // optional ImageDataSettings.
  static ImageData* Create(NotShared<DOMFloat32Array> data,
                           unsigned width,
                           ExceptionState& exception_state) {
    ValidateAndCreateParams params;
    params.require_canvas_color_management_v2 = true;
    return ValidateAndCreate(width, absl::nullopt, data, nullptr, params,
                             exception_state);
  }
  static ImageData* Create(NotShared<DOMFloat32Array> data,
                           unsigned width,
                           unsigned height,
                           const ImageDataSettings* settings,
                           ExceptionState& exception_state) {
    ValidateAndCreateParams params;
    params.require_canvas_color_management_v2 = true;
    return ValidateAndCreate(width, height, data, settings, params,
                             exception_state);
  }

  // ValidateAndCreate is the common path that all ImageData creation code
  // should call directly. The other Create functions are to be called only by
  // generated code.
  struct ValidateAndCreateParams {
    // When a too-large ImageData is created using a constructor, it has
    // historically thrown an IndexSizeError. When created through a 2D
    // canvas, it has historically thrown a RangeError. This flag will
    // trigger the RangeError path.
    bool context_2d_error_mode = false;
    // Constructors in IDL files cannot specify RuntimeEnabled restrictions.
    // This argument is passed by Create functions that should require that the
    // CanvasColorManagement feature be enabled.
    bool require_canvas_color_management = false;
    // Constructors in IDL files cannot specify RuntimeEnabled restrictions.
    // This argument is passed by Create functions that should require that the
    // CanvasColorManagementV2 feature be enabled.
    bool require_canvas_color_management_v2 = false;
    // If the caller is guaranteed to write over the result in its entirety,
    // then this flag may be used to skip initialization of the result's
    // data.
    bool zero_initialize = true;
    // If no color space is specified, then use this value for the resulting
    // ImageData.
    CanvasColorSpace default_color_space = CanvasColorSpace::kSRGB;
  };
  static ImageData* ValidateAndCreate(
      unsigned width,
      absl::optional<unsigned> height,
      absl::optional<NotShared<DOMArrayBufferView>> data,
      const ImageDataSettings* settings,
      ValidateAndCreateParams params,
      ExceptionState& exception_state);
  // TODO(https://crbug.com/1198606): Remove this.
  ImageDataSettings* getSettings() { return settings_; }

  static ImageData* CreateForTest(const IntSize&);
  static ImageData* CreateForTest(const IntSize&,
                                  NotShared<DOMArrayBufferView>,
                                  CanvasColorSpace,
                                  ImageDataStorageFormat);

  ImageData(const IntSize&,
            NotShared<DOMArrayBufferView>,
            CanvasColorSpace,
            ImageDataStorageFormat);

  static String CanvasColorSpaceName(CanvasColorSpace);
  static ImageDataStorageFormat GetImageDataStorageFormat(const String&);
  static unsigned StorageFormatBytesPerPixel(const String&);
  static unsigned StorageFormatBytesPerPixel(ImageDataStorageFormat);

  IntSize Size() const { return size_; }
  int width() const { return size_.Width(); }
  int height() const { return size_.Height(); }
  String colorSpace() const;
  String storageFormat() const;

  // TODO(https://crbug.com/1198606): Remove this.
  ImageDataSettings* getSettings() const;

  const V8ImageDataArray* data() const { return data_; }

  bool IsBufferBaseDetached() const;
  CanvasColorSpace GetCanvasColorSpace() const;
  ImageDataStorageFormat GetImageDataStorageFormat() const;

  // Return an SkPixmap that references this data directly.
  SkPixmap GetSkPixmap() const;

  // ImageBitmapSource implementation
  IntSize BitmapSourceSize() const override { return size_; }
  ScriptPromise CreateImageBitmap(ScriptState*,
                                  absl::optional<IntRect> crop_rect,
                                  const ImageBitmapOptions*,
                                  ExceptionState&) override;

  void Trace(Visitor*) const override;

  WARN_UNUSED_RESULT v8::Local<v8::Object> AssociateWithWrapper(
      v8::Isolate* isolate,
      const WrapperTypeInfo* wrapper_type_info,
      v8::Local<v8::Object> wrapper) override;

 private:
  IntSize size_;
  // TODO(https://crbug.com/1198606): Remove this.
  Member<ImageDataSettings> settings_;
  Member<V8ImageDataArray> data_;
  NotShared<DOMUint8ClampedArray> data_u8_;
  NotShared<DOMUint16Array> data_u16_;
  NotShared<DOMFloat32Array> data_f32_;
  CanvasColorSpace color_space_ = CanvasColorSpace::kSRGB;
  ImageDataStorageFormat storage_format_ = kUint8ClampedArrayStorageFormat;

  static NotShared<DOMArrayBufferView> AllocateAndValidateDataArray(
      const unsigned&,
      ImageDataStorageFormat,
      bool initialize,
      ExceptionState&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CANVAS_IMAGE_DATA_H_
