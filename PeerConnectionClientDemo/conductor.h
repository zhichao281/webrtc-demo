/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef EXAMPLES_PEERCONNECTION_CLIENT_CONDUCTOR_H_
#define EXAMPLES_PEERCONNECTION_CLIENT_CONDUCTOR_H_

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <regex>
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "main_wnd.h"
#include "peer_connection_client.h"
#include "rtc_base/thread.h"



#pragma comment(lib,"version.lib")
#pragma comment(lib,"Imm32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"comdlg32.lib")
#pragma comment(lib,"dbghelp.lib")
#pragma comment(lib,"dnsapi.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"odbc32.lib")
#pragma comment(lib,"odbccp32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"usp10.lib")
#pragma comment(lib,"uuid.lib")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"winspool.lib")
#pragma comment(lib,"delayimp.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"secur32.lib")
#pragma comment(lib,"dmoguids.lib")
#pragma comment(lib,"wmcodecdspuuid.lib")
#pragma comment(lib,"amstrmid.lib")
#pragma comment(lib,"msdmo.lib")
#pragma comment(lib,"strmiids.lib")
#pragma comment(lib,"oleaut32.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

#if defined(GNUC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored ˇ°-Wdeprecated-declarationsˇ±
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

namespace webrtc {
    class VideoCaptureModule;
}  // namespace webrtc

namespace cricket {
    class VideoRenderer;
}  // namespace cricket

class Conductor : public webrtc::PeerConnectionObserver,
    public webrtc::CreateSessionDescriptionObserver,
    public PeerConnectionClientObserver,
    public MainWndCallback {
public:
    enum CallbackID {
        MEDIA_CHANNELS_INITIALIZED = 1,
        PEER_CONNECTION_CLOSED,
        SEND_MESSAGE_TO_PEER,
        NEW_TRACK_ADDED,
        TRACK_REMOVED,
    };

    Conductor(PeerConnectionClient* client, MainWindow* main_wnd);

    bool connection_active() const;

    void Close() override;

protected:
    ~Conductor();
    bool InitializePeerConnection();
    bool ReinitializePeerConnectionForLoopback();
    bool CreatePeerConnection(bool dtls);
    void DeletePeerConnection();
    void EnsureStreamingUI();
    void AddTracks();
	rtc::scoped_refptr<webrtc::AudioSourceInterface> CreateAudioSource(const std::string& audiourl, const std::map<std::string, std::string>& opts);

    //
    // PeerConnectionObserver implementation.
    //

    void OnSignalingChange(
        webrtc::PeerConnectionInterface::SignalingState new_state) override {}
    void OnAddTrack(
        rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
        const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
        streams) override;
    void OnRemoveTrack(
        rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
    void OnDataChannel(
        rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
    void OnRenegotiationNeeded() override {}
    void OnIceConnectionChange(
        webrtc::PeerConnectionInterface::IceConnectionState new_state) override {}
    void OnIceGatheringChange(
        webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
    void OnIceConnectionReceivingChange(bool receiving) override {}

    //
    // PeerConnectionClientObserver implementation.
    //

    void OnSignedIn() override;

    void OnDisconnected() override;

    void OnPeerConnected(int id, const std::string& name) override;

    void OnPeerDisconnected(int id) override;

    void OnMessageFromPeer(int peer_id, const std::string& message) override;

    void OnMessageSent(int err) override;

    void OnServerConnectionFailure() override;

    //
    // MainWndCallback implementation.
    //

    void StartLogin(const std::string& server, int port) override;

    void DisconnectFromServer() override;

    void ConnectToPeer(int peer_id) override;

    void DisconnectFromCurrentPeer() override;

    void UIThreadCallback(int msg_id, void* data) override;

    // CreateSessionDescriptionObserver implementation.
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;

protected:
    // Send a message to the remote peer.
    void SendMessage(const std::string& json_object);

    int peer_id_;
    bool loopback_;
    std::unique_ptr<rtc::Thread> signaling_thread_;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
        peer_connection_factory_;
    PeerConnectionClient* client_;
    MainWindow* main_wnd_;
    std::deque<std::string*> pending_messages_;
    std::string server_;

	std::unique_ptr<webrtc::TaskQueueFactory>                                 m_task_queue_factory;
	rtc::scoped_refptr<webrtc::AudioDeviceModule>                             m_audioDeviceModule;
};

#endif  // EXAMPLES_PEERCONNECTION_CLIENT_CONDUCTOR_H_
