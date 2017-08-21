{
  'target_defaults': {
    'type': 'executable',
    'dependencies':
    [
#      'deps/jsoncpp/jsoncpp.gyp:jsoncpp',
#      'deps/netstring/netstring.gyp:netstring'
#      'deps/libuv/uv.gyp:libuv',
#      'deps/openssl/openssl.gyp:openssl',
#      'deps/libsrtp/libsrtp.gyp:libsrtp'
    ],
    'sources':
    [
      # C++ source files
      'src/DepLibSRTP.cpp',
      'src/DepLibUV.cpp',
      'src/DepOpenSSL.cpp',
      'src/Logger.cpp',
      'src/Loop.cpp',
      'src/Settings.cpp',
      'src/Channel/Notifier.cpp',
      'src/Channel/Request.cpp',
      'src/Channel/UnixStreamSocket.cpp',
      'src/RTC/DtlsTransport.cpp',
      'src/RTC/IceCandidate.cpp',
      'src/RTC/IceServer.cpp',
      'src/RTC/NackGenerator.cpp',
      'src/RTC/Peer.cpp',
      'src/RTC/Room.cpp',
      'src/RTC/RtpListener.cpp',
      'src/RTC/RtpPacket.cpp',
      'src/RTC/RtpReceiver.cpp',
      'src/RTC/RtpSender.cpp',
      'src/RTC/RtpStream.cpp',
      'src/RTC/RtpStreamRecv.cpp',
      'src/RTC/RtpStreamSend.cpp',
      'src/RTC/RtpDataCounter.cpp',
      'src/RTC/SrtpSession.cpp',
      'src/RTC/StunMessage.cpp',
      'src/RTC/TcpConnection.cpp',
      'src/RTC/TcpServer.cpp',
      'src/RTC/Transport.cpp',
      'src/RTC/TransportTuple.cpp',
      'src/RTC/UdpSocket.cpp',
      'src/RTC/RtpDictionaries/Media.cpp',
      'src/RTC/RtpDictionaries/Parameters.cpp',
      'src/RTC/RtpDictionaries/RtcpFeedback.cpp',
      'src/RTC/RtpDictionaries/RtcpParameters.cpp',
      'src/RTC/RtpDictionaries/RtpCapabilities.cpp',
      'src/RTC/RtpDictionaries/RtpCodecMime.cpp',
      'src/RTC/RtpDictionaries/RtpCodecParameters.cpp',
      'src/RTC/RtpDictionaries/RtpEncodingParameters.cpp',
      'src/RTC/RtpDictionaries/RtpFecParameters.cpp',
      'src/RTC/RtpDictionaries/RtpHeaderExtension.cpp',
      'src/RTC/RtpDictionaries/RtpHeaderExtensionParameters.cpp',
      'src/RTC/RtpDictionaries/RtpHeaderExtensionUri.cpp',
      'src/RTC/RtpDictionaries/RtpParameters.cpp',
      'src/RTC/RtpDictionaries/RtpRtxParameters.cpp',
      'src/RTC/RTCP/Packet.cpp',
      'src/RTC/RTCP/CompoundPacket.cpp',
      'src/RTC/RTCP/SenderReport.cpp',
      'src/RTC/RTCP/ReceiverReport.cpp',
      'src/RTC/RTCP/Sdes.cpp',
      'src/RTC/RTCP/Bye.cpp',
      'src/RTC/RTCP/Feedback.cpp',
      'src/RTC/RTCP/FeedbackPs.cpp',
      'src/RTC/RTCP/FeedbackRtp.cpp',
      'src/RTC/RTCP/FeedbackRtpNack.cpp',
      'src/RTC/RTCP/FeedbackRtpTmmb.cpp',
      'src/RTC/RTCP/FeedbackRtpSrReq.cpp',
      'src/RTC/RTCP/FeedbackRtpTllei.cpp',
      'src/RTC/RTCP/FeedbackRtpEcn.cpp',
      'src/RTC/RTCP/FeedbackPsPli.cpp',
      'src/RTC/RTCP/FeedbackPsSli.cpp',
      'src/RTC/RTCP/FeedbackPsRpsi.cpp',
      'src/RTC/RTCP/FeedbackPsFir.cpp',
      'src/RTC/RTCP/FeedbackPsTst.cpp',
      'src/RTC/RTCP/FeedbackPsVbcm.cpp',
      'src/RTC/RTCP/FeedbackPsLei.cpp',
      'src/RTC/RTCP/FeedbackPsAfb.cpp',
      'src/RTC/RTCP/FeedbackPsRemb.cpp',
      'src/RTC/RemoteBitrateEstimator/AimdRateControl.cpp',
      'src/RTC/RemoteBitrateEstimator/InterArrival.cpp',
      'src/RTC/RemoteBitrateEstimator/OveruseDetector.cpp',
      'src/RTC/RemoteBitrateEstimator/OveruseEstimator.cpp',
      'src/RTC/RemoteBitrateEstimator/RemoteBitrateEstimatorAbsSendTime.cpp',
      'src/RTC/RemoteBitrateEstimator/RemoteBitrateEstimatorSingleStream.cpp',
      'src/Utils/Crypto.cpp',
      'src/Utils/File.cpp',
      'src/Utils/IP.cpp',
      'src/handles/SignalsHandler.cpp',
      'src/handles/TcpConnection.cpp',
      'src/handles/TcpServer.cpp',
      'src/handles/Timer.cpp',
      'src/handles/UdpSocket.cpp',
      'src/handles/UnixStreamSocket.cpp',
      'src/netstring.c'
    ],
    'include_dirs':
    [
      '/usr/local/include',
      '/usr/local/opt/openssl/include'
    ],
    "library_dirs": [
      "/usr/local/lib",
      "/usr/local/opt/openssl/lib"
    ],
    "libraries": [
      '-lssl',
      '-lcrypto',
      '-lz',
      '-ljsoncpp',
      '-luv',
      '-lpthread',
      '-ldl',
      '-lsrtp2',
      '-lpcap'
    ],
    'conditions':
    [
      # FIPS
      [ 'openssl_fips != ""', {
        'defines': [ 'BUD_FIPS_ENABLED=1' ]
      }],

      # Endianness
      [ 'node_byteorder=="big"', {
          # Define Big Endian
          'defines': [ 'MS_BIG_ENDIAN' ]
        }, {
          # Define Little Endian
          'defines': [ 'MS_LITTLE_ENDIAN' ]
      }],

      # Platform-specifics

      [ 'OS == "mac" and mediasoup_asan == "true"', {
        'xcode_settings':
        {
          'OTHER_CFLAGS': [ '-fsanitize=address' ],
          'OTHER_LDFLAGS': [ '-fsanitize=address' ]
        }
      }],

      [ 'OS == "linux"', {
        'defines':
        [
          '_POSIX_C_SOURCE=200112',
          '_GNU_SOURCE'
        ]
      }],

      [ 'OS == "linux" and mediasoup_asan == "true"', {
        'cflags': [ '-fsanitize=address' ],
        'ldflags': [ '-fsanitize=address' ]
      }],

      [ 'OS in "linux freebsd"', {
        'ldflags':
        [
          '-Wl,--whole-archive <(libopenssl) -Wl,--no-whole-archive'
        ]
      }],

      [ 'OS in "freebsd"', {
        'ldflags': [ '-Wl,--export-dynamic' ]
      }],

      [ 'OS != "win"', {
        'cflags': [ '-std=c++11', '-Wall', '-Wextra', '-Wno-unused-parameter' ]
      }],

      [ 'OS == "mac"', {
        'xcode_settings':
        {
          'WARNING_CFLAGS': [ '-Wall', '-Wextra', '-Wno-unused-parameter' ],
          'OTHER_CPLUSPLUSFLAGS' : [ '-std=c++11' ]
        }
      }]
    ]
	},
  'targets':
  [
    {
      'target_name': 'mediasoup-worker',
      'sources':
      [
        # C++ source files
        'src/main.cpp'
      ]
    },
    {
      'target_name': 'mediasoup-worker-test',
      'defines': [ 'MS_TEST', 'MS_LOG_STD' ],
      'sources':
      [
        # C++ source files
        'test/tests.cpp',
        'test/test-nack.cpp',
        'test/test-rtp.cpp',
        'test/test-rtcp.cpp',
        'test/test-bitrate.cpp',
        'test/test-rtpstreamrecv.cpp',
        # C++ include files
        'test/catch.hpp',
        'test/helpers.hpp'
      ]
    }
  ]
}
