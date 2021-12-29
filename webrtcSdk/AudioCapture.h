#pragma once
#include <functional> 
#include <string>
#include <map>
typedef  std::function<void(uint8_t* pcm, int datalen, int nSampleRate, int nChannel, int64_t nTimeStamp)> PcmCallBack;
typedef  std::function<void(uint8_t* aac_raw, int file_size, int64_t nTimeStamp)> AacCallBack;
class   AudioCapture
{
public:
	AudioCapture() {};

	virtual ~AudioCapture() {};

	static AudioCapture* CreateAudioCapture(std::string audiourlconst, std::map<std::string, std::string> opts = {});

	virtual int Init(int nSampleRate, int nChannel, int nBitsPerSample) = 0;

	virtual int Init(const std::map<std::string, std::string>& opts) =0;

	virtual int Start( ) = 0;

	virtual void Stop() = 0;

	virtual void Destroy() =0;

	virtual void RegisterPcmCallback(std::function<void(uint8_t* pcm, int datalen,int nSampleRate,int nChannel,int64_t nTimeStamp)> PcmCallback) = 0;		

	virtual void RegisterAacCallback(std::function<void(uint8_t* aac_raw, int file_size, int64_t nTimeStamp)> aacCallBack) = 0;


};