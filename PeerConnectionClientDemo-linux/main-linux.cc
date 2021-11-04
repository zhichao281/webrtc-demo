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

#define VERSION	 "1.2.0.0914"

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
int main(int argc, char* argv[]) {

	if (std::string(typeid(bool).name()).c_str() == "bool")
	{
		std::cout << abi::__cxa_demangle(typeid(bool).name(), 0, 0, 0)<<std::endl ;
	}

	std::cout << std::string(typeid(bool).name()).c_str()<<std::endl ;
	const char* turnurl = "admin:admin@110.80.40.206:3478";

	//const char* turnurl = "";
	const char* defaultlocalstunurl = "110.80.40.206:3478";
	const char* localstunurl = NULL;
	const char* defaultlocalturnurl = "admin:admin@110.80.40.206:3478";
	const char* localturnurl = NULL;
	const char* stunurl = "";
	std::string defaultWebrtcUdpPortRange = "0:65535";
	std::string localWebrtcUdpPortRange = "";
	int logLevel = rtc::LS_INFO;
	const char* webroot = "./html";
	std::string sslCertificate;
	//webrtc::AudioDeviceModule::AudioLayer audioLayer = webrtc::AudioDeviceModule::kPlatformDefaultAudio;
	webrtc::AudioDeviceModule::AudioLayer audioLayer = webrtc::AudioDeviceModule::kDummyAudio;

	std::string streamName;
	std::string nbthreads;
	std::string passwdFile;
	std::string authDomain = "mydomain.com";
	std::string publishFilter(".*");
	Json::Value config;

	std::string httpAddress("0.0.0.0:");
	std::string httpPort = "8000";
	const char* port = getenv("PORT");
	if (port)
	{
		httpPort = port;
	}
	httpAddress.append(httpPort);

	int c = 0;
	while ((c = getopt(argc, argv, "hVv::" "c:H:w:N:A:D:C:" "T::t:S::s::" "a::q:" "n:u:U:" "R:")) != -1)
	{
		switch (c)
		{
		case 'H': httpAddress = optarg; break;
		case 'c': sslCertificate = optarg; break;
		case 'w': webroot = optarg; break;
		case 'N': nbthreads = optarg; break;
		case 'A': passwdFile = optarg; break;
		case 'D': authDomain = optarg; break;

		case 'T': localturnurl = optarg ? optarg : defaultlocalturnurl; turnurl = localturnurl; break;
		case 't': turnurl = optarg; break;
		case 'S': localstunurl = optarg ? optarg : defaultlocalstunurl; stunurl = localstunurl; break;
		case 's': stunurl = optarg ? optarg : defaultlocalstunurl; break;

		case 'a': audioLayer = optarg ? (webrtc::AudioDeviceModule::AudioLayer)atoi(optarg) : webrtc::AudioDeviceModule::kDummyAudio; break;
		case 'q': publishFilter = optarg; break;

		case 'C': {
			std::ifstream stream(optarg);
			stream >> config;
			break;
		}

		case 'n': streamName = optarg; break;
		case 'u': {
			if (!streamName.empty()) {
				config["urls"][streamName]["video"] = optarg;
			}
		}
				break;
		case 'U': {
			if (!streamName.empty()) {
				config["urls"][streamName]["audio"] = optarg;
			}
		}
				break;

		case 'v':
			logLevel--;
			if (optarg) {
				logLevel -= strlen(optarg);
			}
			break;
		case 'V':
			std::cout << VERSION << std::endl;
			exit(0);
			break;
		case 'R':
			std::cout << " First log " << std::endl;
			localWebrtcUdpPortRange = optarg ? optarg : defaultWebrtcUdpPortRange;
			std::cout << localWebrtcUdpPortRange << " [-R defaultWebrtcUdpPortRange]" << std::endl;
			break;
		case 'h':
		default:
			std::cout << argv[0] << " [-H http port] [-S[embeded stun address]] [-t [username:password@]turn_address] -[v[v]]  [url1]...[urln]" << std::endl;
			std::cout << argv[0] << " [-H http port] [-s[externel stun address]] [-t [username:password@]turn_address] -[v[v]] [url1]...[urln]" << std::endl;
			std::cout << argv[0] << " -V" << std::endl;

			std::cout << "\t -v[v[v]]           : verbosity" << std::endl;
			std::cout << "\t -V                 : print version" << std::endl;

			std::cout << "\t -H hostname:port   : HTTP server binding (default " << httpAddress << ")" << std::endl;
			std::cout << "\t -w webroot         : path to get files" << std::endl;
			std::cout << "\t -c sslkeycert      : path to private key and certificate for HTTPS" << std::endl;
			std::cout << "\t -N nbthreads       : number of threads for HTTP server" << std::endl;
			std::cout << "\t -A passwd          : password file for HTTP server access" << std::endl;
			std::cout << "\t -D authDomain      : authentication domain for HTTP server access (default:mydomain.com)" << std::endl;

			std::cout << "\t -S[stun_address]                   : start embeded STUN server bind to address (default " << defaultlocalstunurl << ")" << std::endl;
			std::cout << "\t -s[stun_address]                   : use an external STUN server (default:" << stunurl << " , -:means no STUN)" << std::endl;
			std::cout << "\t -t[username:password@]turn_address : use an external TURN relay server (default:disabled)" << std::endl;
			std::cout << "\t -T[username:password@]turn_address : start embeded TURN server (default:disabled)" << std::endl;

			std::cout << "\t -a[audio layer]                    : spefify audio capture layer to use (default:" << audioLayer << ")" << std::endl;

			std::cout << "\t -n name -u videourl -U audiourl    : register a stream with name using url" << std::endl;
			std::cout << "\t [url]                              : url to register in the source list" << std::endl;
			std::cout << "\t -C config.json                     : load urls from JSON config file" << std::endl;

			std::cout << "\t -R [Udp port range min:max]        : Set the webrtc udp port range (default 0:65534)" << std::endl;

			exit(0);
		}
	}

	while (optind < argc)
	{
		std::string url(argv[optind]);
		config["urls"][url]["video"] = url;
		optind++;
	}

	std::cout << "Version:" << VERSION << std::endl;

	std::cout << config;

	//rtc::LogMessage::LogToDebug((rtc::LoggingSeverity)logLevel);
	//rtc::LogMessage::LogTimestamps();
	//rtc::LogMessage::LogThreads();
	//rtc::LogMessage::AddLogToStream(new FileLog("logs"), (rtc::LoggingSeverity)logLevel);
	//std::cout << "Logger level:" << rtc::LogMessage::GetLogToDebug() << std::endl;

	rtc::Thread* thread = rtc::Thread::Current();
	rtc::InitializeSSL();

	// webrtc server
	std::list<std::string> iceServerList;

	//iceServerList.push_back(std::string("turn:") + "admin:admin@110.80.40.206:3478?transport=udp");
	//iceServerList.push_back(std::string("turn:") + "admin:admin@110.80.40.206:3478?transport=tcp");
	if ((strlen(stunurl) != 0) && (strcmp(stunurl, "-") != 0)) {
		iceServerList.push_back(std::string("stun:") + stunurl);
	}
	if (strlen(turnurl)) {
		iceServerList.push_back(std::string("turn:") + turnurl);
	}

	//start STUN server if needed
	//	std::unique_ptr<cricket::StunServer> stunserver;
	//if (localstunurl != NULL)
	//{
	//	rtc::SocketAddress server_addr;
	//	server_addr.FromString(localstunurl);
	//	rtc::AsyncUDPSocket* server_socket = rtc::AsyncUDPSocket::Create(thread->socketserver(), server_addr);
	//	if (server_socket)
	//	{
	//		stunserver.reset(new cricket::StunServer(server_socket));
	//		std::cout << "STUN Listening at " << server_addr.ToString() << std::endl;
	//	}
	//}

	//// start TRUN server if needed
	//std::unique_ptr<cricket::TurnServer> turnserver;
	//if (localturnurl != NULL)
	//{
	//	std::istringstream is(localturnurl);
	//	std::string addr;
	//	std::getline(is, addr, '@');
	//	std::getline(is, addr, '@');
	//	rtc::SocketAddress server_addr;
	//	server_addr.FromString(addr);
	//	turnserver.reset(new cricket::TurnServer(rtc::Thread::Current()));

	//	rtc::AsyncUDPSocket* server_socket = rtc::AsyncUDPSocket::Create(thread->socketserver(), server_addr);
	//	if (server_socket)
	//	{
	//		std::cout << "TURN Listening UDP at " << server_addr.ToString() << std::endl;
	//		turnserver->AddInternalSocket(server_socket, cricket::PROTO_UDP);
	//	}
	//	rtc::AsyncSocket* tcp_server_socket = thread->socketserver()->CreateAsyncSocket(AF_INET, SOCK_STREAM);
	//	if (tcp_server_socket) {
	//		std::cout << "TURN Listening TCP at " << server_addr.ToString() << std::endl;
	//		tcp_server_socket->Bind(server_addr);
	//		tcp_server_socket->Listen(5);
	//		turnserver->AddInternalServerSocket(tcp_server_socket, cricket::PROTO_TCP);
	//	}

	//	is.str(turnurl);
	//	is.clear();
	//	std::getline(is, addr, '@');
	//	std::getline(is, addr, '@');
	//	rtc::SocketAddress external_server_addr;
	//	external_server_addr.FromString(addr);
	//	std::cout << "TURN external addr:" << external_server_addr.ToString() << std::endl;
	//	turnserver->SetExternalSocketFactory(new rtc::BasicPacketSocketFactory(), rtc::SocketAddress(external_server_addr.ipaddr(), 0));
	//}

	// mainloop
	printfVersion();
	signal(SIGINT, sighandler);
	thread->Run();
	rtc::CleanupSSL();
	std::cout << "Exit" << std::endl;
	return 0;
}

