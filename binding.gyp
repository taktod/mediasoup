{
  "targets": [
    {
      'variables': {
        "mediasoup_asan%":"false"
      },
      "defines": [
        "__ENABLE_NODE__"
      ],
      "target_name": "mediasoupGyp",
      "sources": [
        'worker/src/DepLibSRTP.cpp',
        'worker/src/DepLibUV.cpp',
        'worker/src/DepOpenSSL.cpp',
        'worker/src/Logger.cpp',
        'worker/src/Loop.cpp',
        'worker/src/Settings.cpp',
        'worker/src/Channel/Notifier.cpp',
        'worker/src/Channel/Request.cpp',
        'worker/src/Channel/UnixStreamSocket.cpp',
        'worker/src/RTC/DtlsTransport.cpp',
        'worker/src/RTC/IceCandidate.cpp',
        'worker/src/RTC/IceServer.cpp',
        'worker/src/RTC/NackGenerator.cpp',
        'worker/src/RTC/Peer.cpp',
        'worker/src/RTC/Room.cpp',
        'worker/src/RTC/RtpListener.cpp',
        'worker/src/RTC/RtpPacket.cpp',
        'worker/src/RTC/RtpReceiver.cpp',
        'worker/src/RTC/RtpSender.cpp',
        'worker/src/RTC/RtpStream.cpp',
        'worker/src/RTC/RtpStreamRecv.cpp',
        'worker/src/RTC/RtpStreamSend.cpp',
        'worker/src/RTC/RtpDataCounter.cpp',
        'worker/src/RTC/SrtpSession.cpp',
        'worker/src/RTC/StunMessage.cpp',
        'worker/src/RTC/TcpConnection.cpp',
        'worker/src/RTC/TcpServer.cpp',
        'worker/src/RTC/Transport.cpp',
        'worker/src/RTC/TransportTuple.cpp',
        'worker/src/RTC/UdpSocket.cpp',
        'worker/src/RTC/RtpDictionaries/Media.cpp',
        'worker/src/RTC/RtpDictionaries/Parameters.cpp',
        'worker/src/RTC/RtpDictionaries/RtcpFeedback.cpp',
        'worker/src/RTC/RtpDictionaries/RtcpParameters.cpp',
        'worker/src/RTC/RtpDictionaries/RtpCapabilities.cpp',
        'worker/src/RTC/RtpDictionaries/RtpCodecMime.cpp',
        'worker/src/RTC/RtpDictionaries/RtpCodecParameters.cpp',
        'worker/src/RTC/RtpDictionaries/RtpEncodingParameters.cpp',
        'worker/src/RTC/RtpDictionaries/RtpFecParameters.cpp',
        'worker/src/RTC/RtpDictionaries/RtpHeaderExtension.cpp',
        'worker/src/RTC/RtpDictionaries/RtpHeaderExtensionParameters.cpp',
        'worker/src/RTC/RtpDictionaries/RtpHeaderExtensionUri.cpp',
        'worker/src/RTC/RtpDictionaries/RtpParameters.cpp',
        'worker/src/RTC/RtpDictionaries/RtpRtxParameters.cpp',
        'worker/src/RTC/RTCP/Packet.cpp',
        'worker/src/RTC/RTCP/CompoundPacket.cpp',
        'worker/src/RTC/RTCP/SenderReport.cpp',
        'worker/src/RTC/RTCP/ReceiverReport.cpp',
        'worker/src/RTC/RTCP/Sdes.cpp',
        'worker/src/RTC/RTCP/Bye.cpp',
        'worker/src/RTC/RTCP/Feedback.cpp',
        'worker/src/RTC/RTCP/FeedbackPs.cpp',
        'worker/src/RTC/RTCP/FeedbackRtp.cpp',
        'worker/src/RTC/RTCP/FeedbackRtpNack.cpp',
        'worker/src/RTC/RTCP/FeedbackRtpTmmb.cpp',
        'worker/src/RTC/RTCP/FeedbackRtpSrReq.cpp',
        'worker/src/RTC/RTCP/FeedbackRtpTllei.cpp',
        'worker/src/RTC/RTCP/FeedbackRtpEcn.cpp',
        'worker/src/RTC/RTCP/FeedbackPsPli.cpp',
        'worker/src/RTC/RTCP/FeedbackPsSli.cpp',
        'worker/src/RTC/RTCP/FeedbackPsRpsi.cpp',
        'worker/src/RTC/RTCP/FeedbackPsFir.cpp',
        'worker/src/RTC/RTCP/FeedbackPsTst.cpp',
        'worker/src/RTC/RTCP/FeedbackPsVbcm.cpp',
        'worker/src/RTC/RTCP/FeedbackPsLei.cpp',
        'worker/src/RTC/RTCP/FeedbackPsAfb.cpp',
        'worker/src/RTC/RTCP/FeedbackPsRemb.cpp',
        'worker/src/RTC/RemoteBitrateEstimator/AimdRateControl.cpp',
        'worker/src/RTC/RemoteBitrateEstimator/InterArrival.cpp',
        'worker/src/RTC/RemoteBitrateEstimator/OveruseDetector.cpp',
        'worker/src/RTC/RemoteBitrateEstimator/OveruseEstimator.cpp',
        'worker/src/RTC/RemoteBitrateEstimator/RemoteBitrateEstimatorAbsSendTime.cpp',
        'worker/src/RTC/RemoteBitrateEstimator/RemoteBitrateEstimatorSingleStream.cpp',
        'worker/src/Utils/Crypto.cpp',
        'worker/src/Utils/File.cpp',
        'worker/src/Utils/IP.cpp',
        'worker/src/handles/SignalsHandler.cpp',
        'worker/src/handles/TcpConnection.cpp',
        'worker/src/handles/TcpServer.cpp',
        'worker/src/handles/Timer.cpp',
        'worker/src/handles/UdpSocket.cpp',
        'worker/src/handles/UnixStreamSocket.cpp',
        'worker/src/netstring.c',
        'worker/src/node.cpp' # なんかこんなのをつくらないとだめだろうね。
      ],
      "library_dirs": [
        "/usr/local/lib",
        "/usr/local/opt/openssl/lib"
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions', '-fno-rtti' ],
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
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        '/usr/local/include',
        '/usr/local/opt/openssl/include'
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
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'GCC_ENABLE_CPP_RTTI': 'YES',
            'WARNING_CFLAGS': [ '-Wall', '-Wextra', '-Wno-unused-parameter' ],
            'OTHER_CPLUSPLUSFLAGS' : [ '-std=c++11' ]
          }
        }]
      ]
    }
  ]
}