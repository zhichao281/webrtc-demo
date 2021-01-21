// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MOJO_URL_LOADER_CLIENT_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MOJO_URL_LOADER_CLIENT_H_

#include <stdint.h>
#include <vector>

#include "base/callback_forward.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "services/network/public/mojom/url_loader.mojom.h"
#include "third_party/blink/public/mojom/frame/back_forward_cache_controller.mojom-forward.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_url_loader.h"

namespace base {
class SingleThreadTaskRunner;
}  // namespace base

namespace net {
struct RedirectInfo;
}  // namespace net

namespace network {
struct URLLoaderCompletionStatus;
}  // namespace network

namespace blink {
class WebMojoURLLoaderClientObserver;

// WebMojoURLLoaderClient is an implementation of
// network::mojom::URLLoaderClient to receive messages from a single URLLoader.
// TODO(https://crbug.com/860403): Move this class from blink/public/platform/
// to blink/renderer/platform/loader/fetch/url_loader/ finally.
class BLINK_PLATFORM_EXPORT WebMojoURLLoaderClient final
    : public network::mojom::URLLoaderClient {
 public:
  WebMojoURLLoaderClient(
      int request_id,
      WebMojoURLLoaderClientObserver* url_loader_client_observer,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner,
      bool bypass_redirect_checks,
      const GURL& request_url);
  ~WebMojoURLLoaderClient() override;

  // Set the defer status. If loading is deferred, received messages are not
  // dispatched to clients until it is set not deferred.
  void SetDefersLoading(WebURLLoader::DeferType value);

  // network::mojom::URLLoaderClient implementation
  void OnReceiveResponse(
      network::mojom::URLResponseHeadPtr response_head) override;
  void OnReceiveRedirect(
      const net::RedirectInfo& redirect_info,
      network::mojom::URLResponseHeadPtr response_head) override;
  void OnUploadProgress(int64_t current_position,
                        int64_t total_size,
                        OnUploadProgressCallback ack_callback) override;
  void OnReceiveCachedMetadata(mojo_base::BigBuffer data) override;
  void OnTransferSizeUpdated(int32_t transfer_size_diff) override;
  void OnStartLoadingResponseBody(
      mojo::ScopedDataPipeConsumerHandle body) override;
  void OnComplete(const network::URLLoaderCompletionStatus& status) override;

  void EvictFromBackForwardCache(blink::mojom::RendererEvictionReason reason);
  bool IsDeferredWithBackForwardCache() {
    return deferred_state_ ==
           blink::WebURLLoader::DeferType::kDeferredWithBackForwardCache;
  }

 private:
  class BodyBuffer;
  class DeferredMessage;
  class DeferredOnReceiveResponse;
  class DeferredOnReceiveRedirect;
  class DeferredOnUploadProgress;
  class DeferredOnReceiveCachedMetadata;
  class DeferredOnStartLoadingResponseBody;
  class DeferredOnComplete;

  bool NeedsStoringMessage() const;
  void StoreAndDispatch(std::unique_ptr<DeferredMessage> message);
  void OnConnectionClosed();
  const GURL& last_loaded_url() const { return last_loaded_url_; }

  // Dispatches the messages received after SetDefersLoading is called.
  void FlushDeferredMessages();

  void EvictFromBackForwardCacheDueToTimeout();
  void StopBackForwardCacheEvictionTimer();

  std::vector<std::unique_ptr<DeferredMessage>> deferred_messages_;
  std::unique_ptr<BodyBuffer> body_buffer_;
  base::OneShotTimer back_forward_cache_eviction_timer_;
  base::TimeDelta back_forward_cache_timeout_;
  const int request_id_;
  bool has_received_response_head_ = false;
  bool has_received_response_body_ = false;
  bool has_received_complete_ = false;
  WebURLLoader::DeferType deferred_state_ =
      WebURLLoader::DeferType::kNotDeferred;
  int32_t accumulated_transfer_size_diff_during_deferred_ = 0;
  WebMojoURLLoaderClientObserver* const url_loader_client_observer_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  bool bypass_redirect_checks_ = false;
  GURL last_loaded_url_;

  // For UMA.
  base::TimeTicks on_receive_response_time_;

  mojo::Remote<network::mojom::URLLoader> url_loader_;
  mojo::Receiver<network::mojom::URLLoaderClient> url_loader_client_receiver_{
      this};

  base::WeakPtrFactory<WebMojoURLLoaderClient> weak_factory_{this};
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MOJO_URL_LOADER_CLIENT_H_
