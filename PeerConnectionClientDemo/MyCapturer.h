#pragma once
/**
 *　Windows屏幕录像模块
 */

#include <api/scoped_refptr.h>
#include <api/video/i420_buffer.h>
#include <modules/desktop_capture/desktop_capturer.h>
#include <modules/desktop_capture/desktop_frame.h>
#include "rtc_base/thread.h"
#include "media/base/adapted_video_track_source.h"
#include "rtc_base/message_handler.h"

#include <mutex>
#include <memory>
#include <atomic>
class MyDesktopCapture : public rtc::AdaptedVideoTrackSource, public webrtc::DesktopCapturer::Callback, public rtc::MessageHandler
{
public:
	explicit MyDesktopCapture(std::string  desktopname = "desktop");

	~MyDesktopCapture();
	static rtc::scoped_refptr<MyDesktopCapture> Create(std::string  desktopname = "desktop");

	void CaptureFrame();

	// overide webrtc::DesktopCapturer::Callback
	void OnCaptureResult(webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> desktopframe) override;

	//需要注意的是，采集摄像头视频进行压缩发送时，
	//webrtc会根据当前网络状况进行分辨率自适应调整，
	//这是没有问题，但是在进行桌面采集时就不同了，
	//采集桌面无论何时都不会进行分辨率自适应调整，
	//其依据是根据采集器的 is_screencast() 接口来控制的，那么当我们不希望进行分辨率自适应时，
	//只需返回true即可
	bool is_screencast() const override;

	absl::optional<bool> needs_denoising() const override;

	webrtc::MediaSourceInterface::SourceState state() const override;

	bool remote() const override;

	void OnMessage(rtc::Message* msg) override;

private:
	std::unique_ptr<webrtc::DesktopCapturer> m_desktop_capturer;

	int64_t next_timestamp_us_ = rtc::kNumMicrosecsPerMillisec;
	std::atomic<bool>m_bStop = false;
	std::mutex m_mutex;                //互斥锁		
	std::string m_desktopname;
};

