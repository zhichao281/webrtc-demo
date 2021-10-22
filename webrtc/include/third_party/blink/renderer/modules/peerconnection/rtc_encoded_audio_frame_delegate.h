// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_ENCODED_AUDIO_FRAME_DELEGATE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_ENCODED_AUDIO_FRAME_DELEGATE_H_

#include <stdint.h>

#include <memory>

#include "third_party/blink/renderer/bindings/core/v8/serialization/serialized_script_value.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/wtf/thread_safe_ref_counted.h"
#include "third_party/blink/renderer/platform/wtf/threading_primitives.h"
#include "third_party/webrtc/api/frame_transformer_interface.h"

namespace blink {

class DOMArrayBuffer;

// This class wraps a WebRTC audio frame and allows making shallow
// copies. Its purpose is to support making RTCEncodedVideoFrames
// serializable in the same process.
class RTCEncodedAudioFrameDelegate
    : public WTF::ThreadSafeRefCounted<RTCEncodedAudioFrameDelegate> {
 public:
  explicit RTCEncodedAudioFrameDelegate(
      std::unique_ptr<webrtc::TransformableFrameInterface> webrtc_frame,
      Vector<uint32_t> contributing_sources);

  uint32_t Timestamp() const;
  DOMArrayBuffer* CreateDataBuffer() const;
  void SetData(const DOMArrayBuffer* data);
  uint32_t Ssrc() const;
  Vector<uint32_t> ContributingSources() const;
  std::unique_ptr<webrtc::TransformableFrameInterface> PassWebRtcFrame();

 private:
  mutable Mutex mutex_;
  std::unique_ptr<webrtc::TransformableFrameInterface> webrtc_frame_
      GUARDED_BY(mutex_);
  Vector<uint32_t> contributing_sources_;
};

class MODULES_EXPORT RTCEncodedAudioFramesAttachment
    : public SerializedScriptValue::Attachment {
 public:
  static const void* kAttachmentKey;
  RTCEncodedAudioFramesAttachment() = default;
  ~RTCEncodedAudioFramesAttachment() override = default;

  bool IsLockedToAgentCluster() const override {
    return !encoded_audio_frames_.IsEmpty();
  }

  Vector<scoped_refptr<RTCEncodedAudioFrameDelegate>>& EncodedAudioFrames() {
    return encoded_audio_frames_;
  }

  const Vector<scoped_refptr<RTCEncodedAudioFrameDelegate>>&
  EncodedAudioFrames() const {
    return encoded_audio_frames_;
  }

 private:
  Vector<scoped_refptr<RTCEncodedAudioFrameDelegate>> encoded_audio_frames_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_ENCODED_AUDIO_FRAME_DELEGATE_H_
