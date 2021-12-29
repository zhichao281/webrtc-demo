#include "../AudioCapture.h"
#include "headers_mmdevice.h"
#include <thread>
#include <memory>
#include <atomic>
#include <functional>

class AudioRecordWas: public AudioCapture
{
public:
    AudioRecordWas(const std::string& uri, const std::map<std::string, std::string>& opts);
    ~AudioRecordWas();
    virtual int Init(int nSampleRate, int nChannel, int nBitsPerSample) override;

	virtual int Init(const std::map<std::string, std::string>& opts) override;

	virtual int Start() override;

	virtual void Stop() override;

	virtual void Destroy() override;

	virtual void RegisterPcmCallback(PcmCallBack pcmCallback);		

	virtual void RegisterAacCallback(AacCallBack aacCallBack);

	//int64_t convert_layout(DWORD layout, WORD channels);
	int adjustFormatTo16Bits(WAVEFORMATEX *pwfx);
	void init_format(WAVEFORMATEX *wfex);

	int init_render();

	void CaptureLoop();

	void RecordLoop();

	int do_record();

	void clean_wasapi();

private:
	std::atomic_bool _running;
	std::atomic_bool _inited;
	std::atomic_bool _paused;

	int _sample_rate;
	
	int _bit_rate;

	int _channel_num;

	int64_t _channel_layout;

	int _bit_per_sample;
	
	//AVSampleFormat _fmt;
	
	std::string _device_name;
	std::string _device_id;


	WAVEFORMATEX *_wfex;

	IMMDeviceEnumerator *_enumerator;

	IMMDevice *_device;

	IAudioClient *_capture_client;

	IAudioCaptureClient *_capture;

	IAudioRenderClient *_render;

	IAudioClient *_render_client;

	std::thread _render_thread;

	uint32_t _capture_sample_count;
	uint32_t _render_sample_count;

	HANDLE _ready_event;
	HANDLE _stop_event;
	HANDLE _render_event;

	bool _co_inited;

	bool _is_default;

	bool _is_input;

	bool _use_device_ts;

	//define time stamps here
	int64_t _start_time;
	PcmCallBack m_pPcmCallback = nullptr;
	AacCallBack m_pAacCallback = nullptr;
	std::unique_ptr<std::thread> _captrue_thread;//output
	std::unique_ptr<std::thread> _record_thread;//record
	int _fmt;
};