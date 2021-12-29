

#include "../AudioCapture.h"
#include "AudioRecordWas.h"
#include <thread>
AudioCapture* AudioCapture::CreateAudioCapture(std::string audiourl, const std::map<std::string, std::string> opts)
{
	
	if ((audiourl.find("audiocap://") == 0))
	{
		std::string out_name = audiourl.substr(strlen("audiocap://")).c_str();
	
		return new AudioRecordWas(out_name, opts);
	}
	else
	{		
		return new AudioRecordWas(audiourl, opts);
	}
	return nullptr;
}