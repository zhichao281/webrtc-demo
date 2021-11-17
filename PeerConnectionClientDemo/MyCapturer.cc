#include "MyCapturer.h"
#include "rtc_base/thread.h"
#include "rtc_base/ref_counted_object.h"
#include <modules/desktop_capture/desktop_capture_options.h>
#include <third_party/libyuv/include/libyuv.h>
#include "modules\desktop_capture\cropping_window_capturer.h"

MyDesktopCapture::MyDesktopCapture(std::string  desktopname)
{
	webrtc::DesktopCaptureOptions options(webrtc::DesktopCaptureOptions::CreateDefault());
	options.set_allow_directx_capturer(true);
	if (desktopname == "desktop")
	{
		m_desktop_capturer = webrtc::DesktopCapturer::CreateScreenCapturer(options);
		webrtc::DesktopCapturer::SourceList sourceList;
		m_desktop_capturer->GetSourceList(&sourceList);
		if (sourceList.size() > 0)
		{
			m_desktop_capturer->SelectSource(sourceList[0].id);
		}
	}
	else
	{
		m_desktop_capturer = webrtc::DesktopCapturer::CreateWindowCapturer(options);
		webrtc::DesktopCapturer::SourceList sourceList;
		m_desktop_capturer->GetSourceList(&sourceList);
		bool bfind = false;
		for (int i = 0; i < sourceList.size(); i++)
		{
			if (sourceList[i].title == desktopname)
			{
				m_desktop_capturer->SelectSource(sourceList[i].id);
				bfind = true;
				break;
			}
		}
		if (!bfind)
		{
			//m_LinkMicConfig->GetLinkMicManagerImpl()->PostErrorMsg("ÕÒ²»µ½ ´°¿Ú");
			return;
		}
	}


	m_bStop.store(false);
	m_desktop_capturer->Start(this);
	CaptureFrame();




}
MyDesktopCapture::~MyDesktopCapture()
{
	m_bStop.store(true);
	rtc::Thread::SleepMs(40);

}
rtc::scoped_refptr<MyDesktopCapture> MyDesktopCapture::Create(std::string  desktopname)
{
	rtc::scoped_refptr<MyDesktopCapture> source(new rtc::RefCountedObject<MyDesktopCapture>(desktopname));
	return source;
}
void MyDesktopCapture::OnCaptureResult(webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> desktopframe)
{
	std::unique_lock<std::mutex> _lock(m_mutex);
	if (result != webrtc::DesktopCapturer::Result::SUCCESS)
		return;
	int width = desktopframe->size().width();
	int height = desktopframe->size().height();
	rtc::scoped_refptr<webrtc::I420Buffer> buffer = webrtc::I420Buffer::Create(width, height);
	int stride = width;
	uint8_t* yplane = buffer->MutableDataY();
	uint8_t* uplane = buffer->MutableDataU();
	uint8_t* vplane = buffer->MutableDataV();
	libyuv::ConvertToI420(desktopframe->data(), 0,
		yplane, stride,
		uplane, (stride + 1) / 2,
		vplane, (stride + 1) / 2,
		0, 0,
		width, height,
		width, height,
		libyuv::kRotate0, libyuv::FOURCC_ARGB);
	next_timestamp_us_ = rtc::TimeMicros();
	webrtc::VideoFrame frame = webrtc::VideoFrame(buffer, next_timestamp_us_, 0, webrtc::kVideoRotation_0);
	OnFrame(frame);

}


void MyDesktopCapture::OnMessage(rtc::Message* msg) {
	if (msg->message_id == 0)
	{
		CaptureFrame();
	
	}
	
}
void MyDesktopCapture::CaptureFrame()
{
	if (m_bStop.load())
	{
		return;
	}
	if (m_desktop_capturer)
	{
		m_desktop_capturer->CaptureFrame();
	}


	rtc::Location loc(__FUNCTION__, __FILE__, __LINE__);
	rtc::Thread::Current()->PostDelayed(loc, 33, this, 0);
}


bool MyDesktopCapture::is_screencast() const
{
	return true;
}

absl::optional<bool> MyDesktopCapture::needs_denoising() const
{
	return false;
}
webrtc::MediaSourceInterface::SourceState MyDesktopCapture::state() const
{
	return webrtc::MediaSourceInterface::kLive;
}
bool MyDesktopCapture::remote() const
{
	return false;
}


