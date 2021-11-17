/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** main.cpp
**
** -------------------------------------------------------------------------*/


#include <signal.h>

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <regex>
#include <thread>
#include <future>
#include <map>
#include <memory>
#include "rtc_base/strings/json.h"
#include "modules/audio_device/include/audio_device.h"
#include "rtc_base/thread.h"
#include "p2p/base/stun_server.h"
#include "p2p/base/turn_server.h"
#include "p2p/base/basic_packet_socket_factory.h"
#include "rtc_base/ssl_adapter.h"
#include "head.h"

#define VERSION	 "1.3.0.1112"

#if WIN32
#include "getopt.h"
#endif

void sighandler(int n)
{
	printf("SIGINT\n");
	rtc::Thread::Current()->Quit();
}

void  printfVersion()
{

	RTC_LOG(LS_ERROR) << "******************************************";
	RTC_LOG(LS_ERROR) << "*                                         *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*           webrtc-streamer-linux        *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*                " << VERSION << "              *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "*                                        *";
	RTC_LOG(LS_ERROR) << "******************************************";
}

#include <iostream>
#include <typeinfo>
#include <cxxabi.h>
#include <cxxabi.h>
/* ---------------------------------------------------------------------------
**  main
** -------------------------------------------------------------------------*/
#include "p2p/base/stun_server.h"
#include "rtc_base/async_udp_socket.h"
#include "rtc_base/socket_address.h"
#include "rtc_base/socket_server.h"
#include "rtc_base/thread.h"
using cricket::StunServer;
int main(int argc, char* argv[]) {

	std::vector<std::string>vecLineParam;
	std::vector<std::string> vecBatchParamData;
	std::vector<std::string>::iterator itresult_vecLineParam;
	std::vector<std::string>::iterator itresult_BatchParamData;
	for (itresult_vecLineParam = vecLineParam.begin(), itresult_BatchParamData = vecBatchParamData.begin(); 
		(itresult_vecLineParam !=vecLineParam.end()) && (itresult_BatchParamData != vecBatchParamData.end()); 
		++itresult_vecLineParam, ++itresult_BatchParamData)

	if (argc != 2) {
		std::cerr << "usage: stunserver address" << std::endl;
		return 1;
	}

	rtc::SocketAddress server_addr;
	if (!server_addr.FromString(argv[1])) {
		std::cerr << "Unable to parse IP address: " << argv[1];
		return 1;
	}

	rtc::Thread* pthMain = rtc::Thread::Current();

	rtc::AsyncUDPSocket* server_socket =
		rtc::AsyncUDPSocket::Create(pthMain->socketserver(), server_addr);
	if (!server_socket) {
		std::cerr << "Failed to create a UDP socket" << std::endl;
		return 1;
	}

	StunServer* server = new StunServer(server_socket);

	std::cout << "Listening at " << server_addr.ToString() << std::endl;

	pthMain->Run();

	delete server;
	return 0;
}

