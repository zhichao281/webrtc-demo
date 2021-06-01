// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_IMAGE_DECODERS_AVIF_AVIF_IMAGE_DECODER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_IMAGE_DECODERS_AVIF_AVIF_IMAGE_DECODER_H_

#include <memory>

#include "third_party/blink/renderer/platform/image-decoders/image_decoder.h"
#include "third_party/libavif/src/include/avif/avif.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "ui/gfx/color_space.h"
#include "ui/gfx/color_transform.h"

namespace blink {

class FastSharedBufferReader;

class PLATFORM_EXPORT AVIFImageDecoder final : public ImageDecoder {
 public:
  AVIFImageDecoder(AlphaOption,
                   HighBitDepthDecodingOption,
                   const ColorBehavior&,
                   size_t max_decoded_bytes,
                   AnimationOption);
  AVIFImageDecoder(const AVIFImageDecoder&) = delete;
  AVIFImageDecoder& operator=(const AVIFImageDecoder&) = delete;
  ~AVIFImageDecoder() override;

  // ImageDecoder:
  String FilenameExtension() const override { return "avif"; }
  bool ImageIsHighBitDepth() override;
  void OnSetData(SegmentReader* data) override;
  cc::YUVSubsampling GetYUVSubsampling() const override;
  IntSize DecodedYUVSize(cc::YUVIndex) const override;
  size_t DecodedYUVWidthBytes(cc::YUVIndex) const override;
  SkYUVColorSpace GetYUVColorSpace() const override;
  uint8_t GetYUVBitDepth() const override;
  void DecodeToYUV() override;
  int RepetitionCount() const override;
  bool FrameIsReceivedAtIndex(size_t) const override;
  base::TimeDelta FrameDurationAtIndex(size_t) const override;
  bool ImageHasBothStillAndAnimatedSubImages() const override;

  // Returns true if the data in fast_reader begins with a valid FileTypeBox
  // (ftyp) that supports the brand 'avif' or 'avis'.
  static bool MatchesAVIFSignature(const FastSharedBufferReader& fast_reader);

  gfx::ColorTransform* GetColorTransformForTesting();

  // Exposes the internal function ValidateClapProperty() for testing. Uses only
  // the following members of |image|: image->clap, image->width, image->height,
  // image->yuvFormat.
  static bool ValidateClapPropertyForTesting(const avifImage* image,
                                             int& clap_width,
                                             int& clap_height,
                                             int& clap_leftmost,
                                             int& clap_topmost);

 private:
  struct AvifIOData {
    blink::SegmentReader* reader = nullptr;
    std::vector<uint8_t> buffer;
    bool all_data_received = false;
  };

  void ParseMetadata();

  // ImageDecoder:
  void DecodeSize() override;
  size_t DecodeFrameCount() override;
  void InitializeNewFrame(size_t) override;
  void Decode(size_t) override;
  bool CanReusePreviousFrameBuffer(size_t) const override;

  // Implements avifIOReadFunc, the |read| function in the avifIO struct.
  static avifResult ReadFromSegmentReader(avifIO* io,
                                          uint32_t read_flags,
                                          uint64_t offset,
                                          size_t size,
                                          avifROData* out);

  // Creates |decoder_| if not yet created and decodes the size and frame count.
  bool UpdateDemuxer();

  // Decodes the frame at index |index| and checks if the frame's size, bit
  // depth, and YUV format matches those reported by the container. The decoded
  // frame is available in decoded_image_.
  avifResult DecodeImage(size_t index);

  // Updates or creates |color_transform_| for YUV-to-RGB conversion.
  void UpdateColorTransform(const gfx::ColorSpace& frame_cs, int bit_depth);

  // Crops |decoded_image_|.
  void CropDecodedImage();

  // Renders |image| in |buffer|. Returns whether |image| was rendered
  // successfully.
  bool RenderImage(const avifImage* image, ImageFrame* buffer);

  // Applies color profile correction to the pixel data for |buffer|, if
  // desired.
  void ColorCorrectImage(ImageFrame* buffer);

  bool have_parsed_current_data_ = false;
  // The image width and height (before cropping, if any) from the container.
  //
  // Note: container_width_, container_height_, decoder_->image->width, and
  // decoder_->image->height are the width and height of the full image. Size()
  // returns the size of the cropped image (the clean aperture).
  uint32_t container_width_ = 0;
  uint32_t container_height_ = 0;
  // The bit depth from the container.
  uint8_t bit_depth_ = 0;
  bool decode_to_half_float_ = false;
  uint8_t chroma_shift_x_ = 0;
  uint8_t chroma_shift_y_ = 0;
  // The YUV format from the container.
  avifPixelFormat avif_yuv_format_ = AVIF_PIXEL_FORMAT_NONE;
  size_t decoded_frame_count_ = 0;
  SkYUVColorSpace yuv_color_space_ = SkYUVColorSpace::kIdentity_SkYUVColorSpace;
  // Whether the 'clap' (clean aperture) property should be ignored, e.g.
  // because the 'clap' property is invalid or unsupported.
  bool ignore_clap_ = false;
  // The leftmost pixel and topmost line of the clean aperture. Used only when
  // the image has a 'clap' (clean aperture) property.
  int clap_leftmost_ = 0;
  int clap_topmost_ = 0;
  // A copy of decoder_->image with the width, height, and plane buffers
  // adjusted to those of the clean aperture. Used only when the image has a
  // 'clap' (clean aperture) property.
  avifImage cropped_image_;
  // Set by a successful DecodeImage() call to either decoder_->image or
  // &cropped_image_ depending on whether the image has a 'clap' (clean
  // aperture) property.
  const avifImage* decoded_image_ = nullptr;
  std::unique_ptr<avifDecoder, void (*)(avifDecoder*)> decoder_{nullptr,
                                                                nullptr};
  avifIO avif_io_ = {};
  AvifIOData avif_io_data_;

  std::unique_ptr<gfx::ColorTransform> color_transform_;

  const AnimationOption animation_option_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_IMAGE_DECODERS_AVIF_AVIF_IMAGE_DECODER_H_
