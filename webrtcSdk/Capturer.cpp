#include "Capturer.h"
#include "rtc_base/thread.h"
#include "rtc_base/ref_counted_object.h"
#include <modules/desktop_capture/desktop_capture_options.h>
#include <third_party/libyuv/include/libyuv.h>
#include "modules\desktop_capture\cropping_window_capturer.h"




 void AudioTrackSourceInput::AddSink(webrtc::AudioTrackSinkInterface* sink)
{
	RTC_LOG(INFO) << "AudioTrackSourceInput::AddSink ";
	std::lock_guard<std::mutex> lock(m_sink_lock);
	m_sinks.push_back(sink);
}

 void AudioTrackSourceInput::RemoveSink(webrtc::AudioTrackSinkInterface* sink)
{
	RTC_LOG(INFO) << "AudioTrackSourceInput::RemoveSink ";
	std::lock_guard<std::mutex> lock(m_sink_lock);
	m_sinks.remove(sink);
}

 //∑¢ÀÕ“Ù∆µ¬„¡˜
 int32_t AudioTrackSourceInput::SendRecordedBuffer(int8_t* audio_data,
	 uint32_t data_len,
	 int bits_per_sample,
	 int sample_rate,
	 size_t number_of_channels
	 , int64_t sourcets) {
 

	 int64_t ts = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000 / 1000;
	 if ((m_wait) && (m_prevts != 0))
	 {
		 int64_t periodSource = sourcets - m_previmagets;
		 int64_t periodDecode = ts - m_prevts;

		 RTC_LOG(LS_VERBOSE) << "FFmpegAudioSource::onData interframe decode:" << periodDecode << " source:" << periodSource;
		 int64_t delayms = periodSource - periodDecode;
		 if ((delayms > 0) && (delayms < 1000))
		 {
			 //std::this_thread::sleep_for(std::chrono::milliseconds(delayms));
		 }
	 }
	 int16_t* newdecoded = (int16_t*)audio_data;
	 if (data_len > 0)
	 {
		 for (int i = 0; i < data_len / 2; )
		 {
			 m_vecbuffer.push_back((int16_t)(newdecoded)[i]);
			 i++;
		 }
	 }

	 //delete audio_data;
	 //audio_data = nullptr;
	 int segmentLength = sample_rate / 100;
	 while (m_vecbuffer.size() > segmentLength * number_of_channels)
	 {
		 std::lock_guard<std::mutex> lock(m_sink_lock);
		 for (auto* sink : m_sinks)
		 {
			 sink->OnData(&m_vecbuffer[0], 16, sample_rate, number_of_channels, segmentLength);
		 }
		 m_vecbuffer.erase(m_vecbuffer.begin(), m_vecbuffer.begin() + (segmentLength * number_of_channels));

	 }

	 m_previmagets = sourcets;
	 m_prevts = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000 / 1000;
	 return 0;

 
 };

#include "rtc_base/thread.h"
 AudioTrackSourceInput::AudioTrackSourceInput(const std::string& uri, const std::map<std::string, std::string>& opts, bool wait)
	 :m_wait(wait)
	 , m_previmagets(0)
	 , m_prevts(0)
 {
	 m_pAudioCapture = AudioCapture::CreateAudioCapture(uri, opts);
	 if (m_pAudioCapture)
	 {
		 m_pAudioCapture->Init(0, 0, 0);
		 m_pAudioCapture->RegisterPcmCallback([=](uint8_t* pcm, int datalen, int nSampleRate, int nChannel, int64_t nTimeStamp)
			 {
				 SendRecordedBuffer((int8_t*)pcm, (uint32_t)datalen, 16, nSampleRate, (size_t)nChannel, nTimeStamp);


			 });
		 m_pAudioCapture->Start();
	 }


 }

 AudioTrackSourceInput::~AudioTrackSourceInput()
 {
	 if (m_pAudioCapture)
	 {
		 m_pAudioCapture->Stop();

	 }

 }