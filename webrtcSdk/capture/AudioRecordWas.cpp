#include "AudioRecordWas.h"
#include "error_define.h"
#include "system_error.h"
#include "utils_string.h"
#ifdef _WIN32

#define NS_PER_SEC 1000000000
#define REFTIMES_PER_SEC  NS_PER_SEC/100            //100ns per buffer unit

#endif // _WIN32

using namespace am;
AudioRecordWas::AudioRecordWas(const std::string& uri, const std::map<std::string, std::string>& opts)
{
	_device_name = uri;
	if (opts.find("device_id") != opts.end()) {
		_device_id = opts.at("device_id");
	}
	_is_default = (utils_string::ascii_utf8(DEFAULT_AUDIO_INOUTPUT_ID).compare(_device_id) == 0);
	_is_input = false;
	_co_inited = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	DWORD err = GetLastError();
	if (hr != S_OK && err != S_OK)
		printf("%s,error:%s", err2str(AE_CO_INITED_FAILED), system_error::error2str(err).c_str());

	_co_inited = (hr == S_OK || hr == S_FALSE);//if already initialize will return S_FALSE

	_is_default = false;

	_wfex = NULL;
	_enumerator = nullptr;
	_device = nullptr;
	_capture_client = nullptr;
	_capture = nullptr;
	_render = nullptr;
	_render_client = nullptr;

	_capture_sample_count = 0;
	_render_sample_count = 0;

	_ready_event = NULL;
	_stop_event = NULL;
	_render_event = NULL;

	_use_device_ts = false;

	_start_time = 0;
}

AudioRecordWas::~AudioRecordWas()
{
	Stop();

	clean_wasapi();

	if (_co_inited == true)
		CoUninitialize();
}
void get_device_info(IMMDevice* device) {
	HRESULT resSample;
	IPropertyStore* store = nullptr;
	PWAVEFORMATEX deviceFormatProperties;
	PROPVARIANT prop;
	resSample = device->OpenPropertyStore(STGM_READ, &store);
	if (!FAILED(resSample)) {
		resSample =
			store->GetValue(PKEY_AudioEngine_DeviceFormat, &prop);
		if (!FAILED(resSample)) {
			deviceFormatProperties =
				(PWAVEFORMATEX)prop.blob.pBlobData;
			std::string device_sample = std::to_string(
				deviceFormatProperties->nSamplesPerSec);
		}
	}
}
#define KSAUDIO_SPEAKER_2POINT1     (KSAUDIO_SPEAKER_STEREO|SPEAKER_LOW_FREQUENCY)

#define OBS_KSAUDIO_SPEAKER_4POINT1 \
	(KSAUDIO_SPEAKER_SURROUND | SPEAKER_LOW_FREQUENCY)

// int64_t record_audio_wasapi::convert_layout(DWORD layout, WORD channels)
// {
//     switch (layout) {
//     case KSAUDIO_SPEAKER_2POINT1:
//         return AV_CH_LAYOUT_SURROUND;
//     case KSAUDIO_SPEAKER_SURROUND:
//         return AV_CH_LAYOUT_4POINT0;
//     case OBS_KSAUDIO_SPEAKER_4POINT1:
//         return AV_CH_LAYOUT_4POINT1;
//     case KSAUDIO_SPEAKER_5POINT1_SURROUND:
//         return AV_CH_LAYOUT_5POINT1_BACK;
//     case KSAUDIO_SPEAKER_7POINT1_SURROUND:
//         return AV_CH_LAYOUT_7POINT1;
//     }

//     return av_get_default_channel_layout(channels);
// }
#define AV_SAMPLE_FMT_FLT 3
void AudioRecordWas::init_format(WAVEFORMATEX* wfex)
{
	DWORD layout = 0;

	if (wfex->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		WAVEFORMATEXTENSIBLE* ext = (WAVEFORMATEXTENSIBLE*)wfex;
		layout = ext->dwChannelMask;
	}

	//_channel_layout = convert_layout(layout, wfex->nChannels);
	_sample_rate = wfex->nSamplesPerSec;
	_bit_rate = wfex->nAvgBytesPerSec;
	_bit_per_sample = wfex->wBitsPerSample;
	_channel_num = wfex->nChannels;

	//wasapi is always flt
	_fmt = AV_SAMPLE_FMT_FLT;
}

int AudioRecordWas::init_render()
{
	int error = AE_NO;
	HRESULT res = S_OK;

	do {
		res = _device->Activate(__uuidof(IAudioClient),
			CLSCTX_ALL,
			nullptr,
			(void**)&_render_client
		);

		if (FAILED(res)) {
			error = AE_CO_ACTIVE_DEVICE_FAILED;
			break;
		}

		WAVEFORMATEX* wfex;
		res = _render_client->GetMixFormat(&wfex);
		if (FAILED(res)) {
			error = AE_CO_GET_FORMAT_FAILED;
			break;
		}

		res = _render_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
			REFTIMES_PER_SEC,
			0, wfex, nullptr);

		CoTaskMemFree(wfex);
		if (FAILED(res)) {
			error = AE_CO_AUDIOCLIENT_INIT_FAILED;
			break;
		}

		/* Silent loopback fix. Prevents audio stream from stopping and */
		/* messing up timestamps and other weird glitches during silence */
		/* by playing a silent sample all over again. */

		res = _render_client->GetService(__uuidof(IAudioRenderClient),
			(void**)&_render);
		if (FAILED(res)) {
			error = AE_CO_GET_CAPTURE_FAILED;
			break;
		}

		_render_event = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!_render_event) {
			error = AE_CO_CREATE_EVENT_FAILED;
			break;
		}

		res = _render_client->SetEventHandle(_render_event);
		if (FAILED(res)) {
			error = AE_CO_SET_EVENT_FAILED;
			break;
		}

		//pre fill a single silent buffer
		res = _render_client->GetBufferSize(&_render_sample_count);
		if (FAILED(res)) {
			error = AE_CO_GET_VALUE_FAILED;
			break;
		}

		uint8_t* buffer = NULL;
		res = _render->GetBuffer(_render_sample_count, &buffer);
		if (FAILED(res)) {
			error = AE_CO_GET_VALUE_FAILED;
			break;
		}

		res = _render->ReleaseBuffer(_render_sample_count, AUDCLNT_BUFFERFLAGS_SILENT);
		if (FAILED(res)) {
			error = AE_CO_RELEASE_BUFFER_FAILED;
			break;
		}
	} while (0);

	if (error != AE_NO)
		printf("init render failed(%ld), %s,lasterror:%lu", res, err2str(error), GetLastError());

	return error;
}
int AudioRecordWas::adjustFormatTo16Bits(WAVEFORMATEX* pwfx)
{
	if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		pwfx->wFormatTag = WAVE_FORMAT_PCM;
	}
	else if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
		if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat))
		{
			pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			pEx->Samples.wValidBitsPerSample = 16;
		}
	}
	else
	{
		return -1;
	}

	pwfx->wBitsPerSample = 16;
	pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
	pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
	return 0;
}

int AudioRecordWas::Init(int nSampleRate, int nChannel, int nBitsPerSample)
{
	_sample_rate = nSampleRate;
	_channel_num = nChannel;
	_bit_per_sample = nBitsPerSample;

	int error = AE_NO;
	HRESULT hr = S_OK;

	printf("wasapi start to initialize in %s mode with: %s",
		_is_input ? "input" : "output",
		_device_name.c_str());

	if (_co_inited == false) {
		return AE_CO_INITED_FAILED;
	}

	if (_inited == true) {
		return AE_NO;
	}


	do {
		hr = CoCreateInstance(
			__uuidof(MMDeviceEnumerator),
			NULL,
			CLSCTX_ALL,
			__uuidof(IMMDeviceEnumerator),
			(void**)&_enumerator);

		if (FAILED(hr)) {
			error = AE_CO_CREATE_FAILED;
			break;
		}

		if (_is_default) {
			hr = _enumerator->GetDefaultAudioEndpoint(
				_is_input ? eCapture : eRender,
				_is_input ? eCommunications : eConsole, &_device);
		}
		else {
			hr = _enumerator->GetDevice(utils_string::utf8_unicode(_device_id).c_str(), &_device);
		}

		if (hr != S_OK) {
			error = AE_CO_GETENDPOINT_FAILED;
			break;
		}

		get_device_info(_device);

		hr = _device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&_capture_client);
		if (hr != S_OK) {
			error = AE_CO_ACTIVE_DEVICE_FAILED;
			break;
		}

		hr = _capture_client->GetMixFormat(&_wfex);
		if (hr != S_OK) {
			error = AE_CO_GET_FORMAT_FAILED;
			break;
		}
		adjustFormatTo16Bits(_wfex);
		init_format(_wfex);

		DWORD flags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
		if (_is_input == false)
			flags |= AUDCLNT_STREAMFLAGS_LOOPBACK;

		hr = _capture_client->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			flags,
			REFTIMES_PER_SEC,
			0,
			_wfex,
			NULL);

		// AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED
		// https://docs.microsoft.com/en-us/windows/win32/api/audioclient/nf-audioclient-iaudioclient-initialize
		if (hr != S_OK) {
			error = AE_CO_AUDIOCLIENT_INIT_FAILED;
			break;
		}

		//For ouotput mode,ready event will not signal when there is nothing rendering
		//We run a render thread and render silent pcm data all time
		if (!_is_input) {
			error = init_render();
			if (error != AE_NO)
				break;
		}

		hr = _capture_client->GetBufferSize(&_capture_sample_count);
		if (hr != S_OK) {
			error = AE_CO_GET_VALUE_FAILED;
			break;
		}


		hr = _capture_client->GetService(__uuidof(IAudioCaptureClient), (void**)&_capture);
		if (hr != S_OK) {
			error = AE_CO_GET_CAPTURE_FAILED;
			break;
		}

		_ready_event = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!_ready_event) {
			error = AE_CO_CREATE_EVENT_FAILED;
			break;
		}

		_stop_event = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (!_stop_event) {
			error = AE_CO_CREATE_EVENT_FAILED;
			break;
		}

		hr = _capture_client->SetEventHandle(_ready_event);
		if (hr != S_OK) {
			error = AE_CO_SET_EVENT_FAILED;
			break;
		}

		_inited = true;

	} while (0);

	if (error != AE_NO) {
		printf("wasapi initialize failed,%s,error:%lu,hr:%lld",
			err2str(error), GetLastError(), hr);
		clean_wasapi();
	}

	return error;
}

int AudioRecordWas::Init(const std::map<std::string, std::string>& opts)
{
	return 0;
}

int AudioRecordWas::Start()
{
	if (_running == true) {
		printf("audio record is already running");
		return AE_NO;
	}

	if (_inited == false)
		return AE_NEED_INIT;

	HRESULT hr = S_OK;


	if (!_is_input) {
		hr = _render_client->Start();
		if (FAILED(hr)) {
			printf("%s,error:%lu", err2str(AE_CO_START_FAILED), GetLastError());
			return AE_CO_START_FAILED;
		}
	}


	hr = _capture_client->Start();
	if (hr != S_OK) {
		printf("%s,error:%lu", err2str(AE_CO_START_FAILED), GetLastError());
		return AE_CO_START_FAILED;
	}

	_running = true;


	if (!_is_input) {
		_captrue_thread.reset(new std::thread(std::bind(&AudioRecordWas::CaptureLoop, this)));
	}


	_record_thread.reset(new std::thread(std::bind(&AudioRecordWas::RecordLoop, this)));

	return AE_NO;


}

void AudioRecordWas::Stop()
{
	_running = false;
	SetEvent(_stop_event);

	if (_captrue_thread->joinable())
		_captrue_thread->join();

	if (_record_thread->joinable())
		_record_thread->join();

	if (_capture_client)
		_capture_client->Stop();

	if (_render_client)
		_render_client->Stop();

	return;
}

void AudioRecordWas::Destroy()
{

}

void AudioRecordWas::RegisterPcmCallback(PcmCallBack pcmCallback)
{
	m_pPcmCallback = pcmCallback;
}

void AudioRecordWas::RegisterAacCallback(AacCallBack aacCallBack)
{
	m_pAacCallback = aacCallBack;
}

void AudioRecordWas::CaptureLoop()
{
	HANDLE events[2] = { _stop_event,_render_event };

	HRESULT res = S_OK;
	uint8_t* pData = NULL;
	uint32_t padding_count = 0;

	while (_running &&
		WaitForMultipleObjects(2, events, FALSE, INFINITE) != WAIT_OBJECT_0
		) {

		res = _render_client->GetCurrentPadding(&padding_count);
		if (FAILED(res)) {
			break;
		}

		if (padding_count == _render_sample_count) {
			//if (_on_error) _on_error(AE_CO_PADDING_UNEXPECTED, _cb_extra_index);
			break;
		}

		res = _render->GetBuffer(_render_sample_count - padding_count, &pData);
		if (FAILED(res)) {
			//if (_on_error) _on_error(AE_CO_GET_BUFFER_FAILED, _cb_extra_index);
			break;
		}

		res = _render->ReleaseBuffer(_render_sample_count - padding_count, AUDCLNT_BUFFERFLAGS_SILENT);
		if (FAILED(res)) {
			//if (_on_error) _on_error(AE_CO_RELEASE_BUFFER_FAILED, _cb_extra_index);
			break;
		}
	}
}

void AudioRecordWas::RecordLoop()
{
	HANDLE events[2] = { _stop_event,_ready_event };

	// while,sys will never not signal this ready_event in windows7,
	// and only signal when something is rendring,so we just wait 10ms for speaker
	DWORD dur = _is_input ? INFINITE : 10;

	int error = AE_NO;
	while (_running)
	{
		if (WaitForMultipleObjects(2, events, FALSE, dur) == WAIT_OBJECT_0)
			break;

		if ((error = do_record()) != AE_NO) {
			//if (_on_error) _on_error(error, _cb_extra_index);
			break;
		}
	}//while(_running)

}

int AudioRecordWas::do_record()
{
	HRESULT res = S_OK;
	LPBYTE buffer = NULL;
	DWORD flags = 0;
	uint32_t sample_count = 0;
	UINT64 pos, ts;
	int error = 0;

	while (_running) {
		res = _capture->GetNextPacketSize(&sample_count);

		if (FAILED(res)) {
			if (res != AUDCLNT_E_DEVICE_INVALIDATED)
				printf("GetNextPacketSize failed: %lX", res);
			error = -1;
			break;
		}

		if (!sample_count)
			break;

		buffer = NULL;
		res = _capture->GetBuffer(&buffer, &sample_count, &flags, &pos, &ts);
		if (FAILED(res)) {
			if (res != AUDCLNT_E_DEVICE_INVALIDATED)
				printf("GetBuffer failed: %lX", res);
			error = -1;
			break;
		}

		//input mode do not have silent data flag do nothing here
		if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
			//al_warn("on slient data %d", sample_count);
		}



		if (buffer) {
			int sample_size = _bit_per_sample / 8 * _channel_num;
			int pkt_size = sample_count * sample_size;
			if (m_pPcmCallback) m_pPcmCallback(buffer, pkt_size, _sample_rate, _channel_num, ts * 100);
			//process_data(frame, buffer, sample_count, ts);
		}
		else {
			printf("buffer invalid is");
		}

		_capture->ReleaseBuffer(sample_count);
	}

	return error;
}

void AudioRecordWas::clean_wasapi()
{
	if (_wfex)
		CoTaskMemFree(_wfex);
	_wfex = NULL;

	if (_enumerator)
		_enumerator->Release();
	_enumerator = nullptr;

	if (_device)
		_device->Release();
	_device = nullptr;

	if (_capture_client)
		_capture_client->Release();
	_capture_client = nullptr;

	if (_render_client)
		_render_client->Release();
	_render_client = nullptr;

	if (_capture)
		_capture->Release();
	_capture = nullptr;

	if (_render)
		_render->Release();
	_render = nullptr;

	if (_ready_event)
		CloseHandle(_ready_event);
	_ready_event = NULL;

	if (_stop_event)
		CloseHandle(_stop_event);
	_stop_event = NULL;

	if (_render_event)
		CloseHandle(_render_event);
	_render_event = NULL;

	if (_co_inited == true)
		CoUninitialize();

	_co_inited = false;
	_inited = false;
}