#define MS_CLASS "node"

// nodeとのやりとりをするnode-gypの動作部分
#include <nan.h>
#include "common.hpp"
#include "Logger.hpp"
#include "DepLibSRTP.hpp"
#include "DepLibUV.hpp"
#include "DepOpenSSL.hpp"
#include "DepTimer.hpp"
#include "Logger.hpp"
#include "Loop.hpp"
#include "MediaSoupError.hpp"
#include "Settings.hpp"
#include "Utils.hpp"
#include "Channel/UnixStreamSocket.hpp"
#include "RTC/DtlsTransport.hpp"
#include "RTC/SrtpSession.hpp"
#include "RTC/TcpServer.hpp"
#include "RTC/UdpSocket.hpp"
#include <uv.h>
#include <cerrno>
#include <csignal>  // sigaction()
#include <cstdlib>  // std::_Exit(), std::genenv()
#include <iostream> // std::cout, std::cerr, std::endl
#include <map>
#include <string>
#include <unistd.h> // getpid(), usleep()

static void init();
static void ignoreSignals();
static void destroy();
static void exitSuccess();
static void exitWithError();

static Loop *loop = nullptr;

static NAN_METHOD(NodeInit) {
  // nodeの初期化
  // 必要な初期化を実施すればいいわけだが・・・
/*'iquhjmgy#1',
  '--logLevel=debug',
  '--logTag=info',
  '--logTag=rtp',
  '--logTag=rtcp',
  '--logTag=rtx',
  '--rtcIPv4=true',
  '--rtcIPv6=true',
  '--rtcMinPort=40000',
  '--rtcMaxPort=49999*/
	  int argc = 11;
  char* argv[11];
  std::string id= std::string("blxvigly#1");
  argv[1] = (char *)id.c_str();
  std::string level= std::string("--logLevel=debug");
  argv[2] = (char *)level.c_str();
  std::string tag1= std::string("--logTag=info");
  argv[3] = (char *)tag1.c_str();
  std::string tag2= std::string("--logTag=rtp");
  argv[4] = (char *)tag2.c_str();
  std::string tag3= std::string("--logTag=rtcp");
  argv[5] = (char *)tag3.c_str();
  std::string tag4= std::string("--logTag=rtx");
  argv[6] = (char *)tag4.c_str();
  std::string ip1= std::string("--rtcIPv4=true");
  argv[7] = (char *)ip1.c_str();
  std::string ip2= std::string("--rtcIPv6=true");
  argv[8] = (char *)ip2.c_str();
  std::string minport= std::string("--rtcMinPort=40000");
  argv[9] = (char *)minport.c_str();
  std::string maxport= std::string("--rtcMaxPort=49999");
  argv[10] = (char *)maxport.c_str();
  int channelFd = 3; // これはめっちゃ適当(これは3みたい)
	// このchannelFdは適当につくるのではなく、node側と同じものを準備しなければならないのか・・・

  // これはcallしなくていい、すでにnodeの動作でcall済みのはず
//  DepLibUV::ClassInit();
	DepTimer::ClassInit();
	// Set the Channel socket (this will be handled and deleted by the Loop).
	auto* channel = new Channel::UnixStreamSocket(channelFd);

  Channel::UnixStreamSocket::setChannel(channel);
	// Initialize the Logger.
	Logger::Init(id, channel);
	// Setup the configuration.
	try
	{
		Settings::SetConfiguration(argc, argv);
	}
	catch (const MediaSoupError& error)
	{
		MS_ERROR("configuration error: %s", error.what());

		exitWithError();
	}

	// Print the effective configuration.
	Settings::PrintConfiguration();

	MS_DEBUG_TAG(info, "starting mediasoup-worker [pid:%ld]", (long)getpid());

#if defined(MS_LITTLE_ENDIAN)
	MS_DEBUG_TAG(info, "Little-Endian CPU detected");
#elif defined(MS_BIG_ENDIAN)
	MS_DEBUG_TAG(info, "Big-Endian CPU detected");
#endif

#if defined(INTPTR_MAX) && defined(INT32_MAX) && (INTPTR_MAX == INT32_MAX)
	MS_DEBUG_TAG(info, "32 bits architecture detected");
#elif defined(INTPTR_MAX) && defined(INT64_MAX) && (INTPTR_MAX == INT64_MAX)
	MS_DEBUG_TAG(info, "64 bits architecture detected");
#else
	MS_WARN_TAG(info, "can not determine whether the architecture is 32 or 64 bits");
#endif
  try {
    init(); // destroyとexitSuccessはあとで考えるということで
//    Loop loop(channel); // これ必要みたい。 // このあとでこのまま蒸発するね・・これ
    loop = new Loop(channel);
  }
  catch(const MediaSoupError& error) {
    MS_ERROR_STD("failure:%s", error.what());
  }
}

void init()
{
	MS_TRACE();

	ignoreSignals();
	DepLibUV::PrintVersion();

	// Initialize static stuff.
	DepOpenSSL::ClassInit();
	DepLibSRTP::ClassInit();
	Utils::Crypto::ClassInit();
	RTC::UdpSocket::ClassInit();
	RTC::TcpServer::ClassInit();
	RTC::DtlsTransport::ClassInit();
	RTC::SrtpSession::ClassInit();
	RTC::Room::ClassInit();
}

void ignoreSignals()
{
	MS_TRACE();

	int err;
	// clang-format off
	struct sigaction act{};
	std::map<std::string, int> ignoredSignals =
	{
		{ "PIPE", SIGPIPE },
		{ "HUP",  SIGHUP  },
		{ "ALRM", SIGALRM },
		{ "USR1", SIGUSR2 },
		{ "USR2", SIGUSR1}
	};
	// clang-format on

	// Ignore clang-tidy cppcoreguidelines-pro-type-cstyle-cast.
	act.sa_handler = SIG_IGN; // NOLINT
	act.sa_flags   = 0;
	err            = sigfillset(&act.sa_mask);
	if (err != 0)
		MS_THROW_ERROR("sigfillset() failed: %s", std::strerror(errno));

	for (auto& ignoredSignal : ignoredSignals)
	{
		auto& sigName = ignoredSignal.first;
		int sigId     = ignoredSignal.second;

		err = sigaction(sigId, &act, nullptr);
		if (err != 0)
		{
			MS_THROW_ERROR("sigaction() failed for signal %s: %s", sigName.c_str(), std::strerror(errno));
		}
	}
}

void destroy()
{
	MS_TRACE();

	// Free static stuff.
	RTC::DtlsTransport::ClassDestroy();
	Utils::Crypto::ClassDestroy();
	DepLibUV::ClassDestroy();
	DepOpenSSL::ClassDestroy();
	DepLibSRTP::ClassDestroy();
}

void exitSuccess()
{
	// Wait a bit so peding messages to stdout/Channel arrive to the main process.
	usleep(100000);
	// And exit with success status.
	std::_Exit(EXIT_SUCCESS);
}

void exitWithError()
{
	// Wait a bit so peding messages to stderr arrive to the main process.
	usleep(100000);
	// And exit with error status.
	std::_Exit(EXIT_FAILURE);
}

// ここで処理をいろいろしてみないといけないわけか・・・
static NAN_MODULE_INIT(NodePreInit) {
  Channel::UnixStreamSocket::NodeInit(target);
  // ここで初期化してから、node側のデータをセットしてから、initをcallしなければならないというわけ・・・
  Nan::Set(target, Nan::New("init").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(NodeInit)).ToLocalChecked());
}

NODE_MODULE(mediasoupGyp, NodePreInit);
