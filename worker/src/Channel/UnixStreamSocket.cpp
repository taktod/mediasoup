#define MS_CLASS "Channel::UnixStreamSocket"
// #define MS_LOG_DEV

#include "UnixStreamSocket.hpp"
#include "../Logger.hpp"
#include "../MediaSoupError.hpp"
#include <cmath>   // std::ceil()
#include <cstdio>  // sprintf()
#include <cstring> // std::memmove()
#include <sstream> // std::ostringstream
extern "C" {
//#include <netstring.h>
#include "../netstring.h"
}
static bool isCallbackSet_ = false;
static Nan::Persistent<v8::Object> callback_;
static Channel::UnixStreamSocket *channel_ = nullptr;

// 今回はこいつ自身を改良してしまうか・・・
// その方がはやそう。
// サイズとかどうでもいい。
// まずinitでcallbackを登録可能にしておかなければならない。
// このcallbackをresponseがきたら、返すのに利用するという具合で
// あとはrequestという形でnode側からこちら側に要求がある。
// というわけでこの２つだけ実装しておかなければいけないか・・・

namespace Channel
{
	void UnixStreamSocket::setChannel(Channel::UnixStreamSocket *channel) {
		channel_ = channel;
	}
	// その他追記したもの。
	void UnixStreamSocket::NodeInit(v8::Local<v8::Object> target) {
		// ここでここがcallされる。
		// targetに2つの関数を登録しておく
		Nan::Set(target, Nan::New("setCallback").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(SetCallback)).ToLocalChecked());
		Nan::Set(target, Nan::New("request").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(Request)).ToLocalChecked());
	}
	NAN_METHOD(UnixStreamSocket::SetCallback) {
		if(info.Length() == 1) {
			// とりあえずセットしとく
			isCallbackSet_ = true;
			callback_.Reset(info[0]->ToObject());
		}
	}
	// これ・・・ここにいれない方がしあわせかね。
	NAN_METHOD(UnixStreamSocket::Request) {
		puts("requestが呼ばれた");
		// この読み込まれたデータを送らなければならない。
		if(channel_ != nullptr) {
			// ここに対してメッセージを送り込む必要がある模様
	    v8::Local<v8::Value> binary   = info[0];
      void   *data      = (void *)node::Buffer::Data(binary->ToObject());
      size_t  data_size = node::Buffer::Length(binary->ToObject());
			channel_->NodeRequestRead((const char *)data, data_size);
		}
	}

	/* Static. */

	// netstring length for a 65536 bytes payload.
	static constexpr size_t MaxSize{ 65543 };
	static constexpr size_t MessageMaxSize{ 65536 };
	static uint8_t WriteBuffer[MaxSize];

	/* Instance methods. */

	UnixStreamSocket::UnixStreamSocket(int fd) : ::UnixStreamSocket::UnixStreamSocket(fd, MaxSize)
	{
		MS_TRACE_STD();

		// Create the JSON reader.
		{
			Json::CharReaderBuilder builder;
			Json::Value settings = Json::nullValue;
			Json::Value invalidSettings;

			builder.strictMode(&settings);

			MS_ASSERT(builder.validate(&invalidSettings), "invalid Json::CharReaderBuilder");

			this->jsonReader = builder.newCharReader();
		}

		// Create the JSON writer.
		{
			Json::StreamWriterBuilder builder;
			Json::Value invalidSettings;

			builder["commentStyle"]            = "None";
			builder["indentation"]             = "";
			builder["enableYAMLCompatibility"] = false;
			builder["dropNullPlaceholders"]    = false;

			MS_ASSERT(builder.validate(&invalidSettings), "invalid Json::StreamWriterBuilder");

			this->jsonWriter = builder.newStreamWriter();
		}
	}

	UnixStreamSocket::~UnixStreamSocket()
	{
		MS_TRACE_STD();

		delete this->jsonReader;
		delete this->jsonWriter;
	}

	void UnixStreamSocket::SetListener(Listener* listener)
	{
		MS_TRACE_STD();

		this->listener = listener;
	}

	void UnixStreamSocket::Send(Json::Value& msg)
	{
		if (this->closed)
			return;

		// MS_TRACE_STD();

		std::ostringstream stream;
		std::string nsPayload;
		size_t nsPayloadLen;
		size_t nsNumLen;
		size_t nsLen;

		this->jsonWriter->write(msg, &stream);
		nsPayload    = stream.str();
		nsPayloadLen = nsPayload.length();

		if (nsPayloadLen > MessageMaxSize)
		{
			MS_ERROR_STD("mesage too big");

			return;
		}

		if (nsPayloadLen == 0)
		{
			nsNumLen       = 1;
			WriteBuffer[0] = '0';
			WriteBuffer[1] = ':';
			WriteBuffer[2] = ',';
		}
		else
		{
			nsNumLen = static_cast<size_t>(std::ceil(std::log10(static_cast<double>(nsPayloadLen) + 1)));
			std::sprintf(reinterpret_cast<char*>(WriteBuffer), "%zu:", nsPayloadLen);
			std::memcpy(WriteBuffer + nsNumLen + 1, nsPayload.c_str(), nsPayloadLen);
			WriteBuffer[nsNumLen + nsPayloadLen + 1] = ',';
		}

		nsLen = nsNumLen + nsPayloadLen + 2;

// このデータをcallbackで応答しなければならない。
		if(isCallbackSet_) {
			// callbackをつかって応答しなければならない。
			Nan::HandleScope scope;
			Nan::Callback callback(Nan::New(callback_).As<v8::Function>());
		  v8::Local<v8::Object> binary   = Nan::CopyBuffer((char *)WriteBuffer, nsLen).ToLocalChecked();
  		v8::Local<v8::Value>  args[]   = {
		    binary
  		};
  		callback.Call(1, args);
		}
//		Write(WriteBuffer, nsLen);
	}

	void UnixStreamSocket::SendLog(char* nsPayload, size_t nsPayloadLen)
	{
		if (this->closed)
			return;

		// MS_TRACE_STD();

		size_t nsNumLen;
		size_t nsLen;

		if (nsPayloadLen > MessageMaxSize)
		{
			MS_ERROR_STD("mesage too big");

			return;
		}

		if (nsPayloadLen == 0)
		{
			nsNumLen       = 1;
			WriteBuffer[0] = '0';
			WriteBuffer[1] = ':';
			WriteBuffer[2] = ',';
		}
		else
		{
			nsNumLen = static_cast<size_t>(std::ceil(std::log10(static_cast<double>(nsPayloadLen) + 1)));
			std::sprintf(reinterpret_cast<char*>(WriteBuffer), "%zu:", nsPayloadLen);
			std::memcpy(WriteBuffer + nsNumLen + 1, nsPayload, nsPayloadLen);
			WriteBuffer[nsNumLen + nsPayloadLen + 1] = ',';
		}

		nsLen = nsNumLen + nsPayloadLen + 2;
		if(isCallbackSet_) {
			// callbackをつかって応答しなければならない。
			Nan::HandleScope scope;
			Nan::Callback callback(Nan::New(callback_).As<v8::Function>());
		  v8::Local<v8::Object> binary   = Nan::CopyBuffer((char *)WriteBuffer, nsLen).ToLocalChecked();
  		v8::Local<v8::Value>  args[]   = {
		    binary
  		};
  		callback.Call(1, args);
		}
//		Write(WriteBuffer, nsLen);
	}

	void UnixStreamSocket::SendBinary(const uint8_t* nsPayload, size_t nsPayloadLen)
	{
		if (this->closed)
			return;

		size_t nsNumLen;
		size_t nsLen;

		if (nsPayloadLen > MessageMaxSize)
		{
			MS_ERROR_STD("mesage too big");

			return;
		}

		if (nsPayloadLen == 0)
		{
			nsNumLen       = 1;
			WriteBuffer[0] = '0';
			WriteBuffer[1] = ':';
			WriteBuffer[2] = ',';
		}
		else
		{
			nsNumLen = static_cast<size_t>(std::ceil(std::log10(static_cast<double>(nsPayloadLen) + 1)));
			std::sprintf(reinterpret_cast<char*>(WriteBuffer), "%zu:", nsPayloadLen);
			std::memcpy(WriteBuffer + nsNumLen + 1, nsPayload, nsPayloadLen);
			WriteBuffer[nsNumLen + nsPayloadLen + 1] = ',';
		}

		nsLen = nsNumLen + nsPayloadLen + 2;
		if(isCallbackSet_) {
			// callbackをつかって応答しなければならない。
			Nan::HandleScope scope;
			Nan::Callback callback(Nan::New(callback_).As<v8::Function>());
		  v8::Local<v8::Object> binary   = Nan::CopyBuffer((char *)WriteBuffer, nsLen).ToLocalChecked();
  		v8::Local<v8::Value>  args[]   = {
		    binary
  		};
  		callback.Call(1, args);
		}
//		Write(WriteBuffer, nsLen);
	}

	void UnixStreamSocket::NodeRequestRead(const char *data, size_t data_size) {
		// node側からデータがきた場合の動作
		MS_TRACE_STD();

		char* jsonStart = nullptr;
		size_t jsonLen;
		int nsRet = netstring_read(
			(char *)data, data_size, &jsonStart, &jsonLen);

		if (nsRet != 0)
		{
			switch (nsRet)
			{
				case NETSTRING_ERROR_TOO_SHORT:
					// Check if the buffer is full.
					MS_ERROR_STD("NETSTRING_ERROR_TOO_SHORT");
					// Otherwise the buffer is not full, just wait.

					// Exit the parsing loop.
					return;

				case NETSTRING_ERROR_TOO_LONG:
					MS_ERROR_STD("NETSTRING_ERROR_TOO_LONG");
					break;

				case NETSTRING_ERROR_NO_COLON:
					MS_ERROR_STD("NETSTRING_ERROR_NO_COLON");
					break;

				case NETSTRING_ERROR_NO_COMMA:
					MS_ERROR_STD("NETSTRING_ERROR_NO_COMMA");
					break;

				case NETSTRING_ERROR_LEADING_ZERO:
					MS_ERROR_STD("NETSTRING_ERROR_LEADING_ZERO");
					break;

				case NETSTRING_ERROR_NO_LENGTH:
					MS_ERROR_STD("NETSTRING_ERROR_NO_LENGTH");
					break;
			}

			// Error, so reset and exit the parsing loop.
			this->msgStart      = 0;
			this->bufferDataLen = 0;

			return;
		}

		// If here it means that jsonStart points to the beginning of a JSON string
		// with jsonLen bytes length, so recalculate readLen.
//		readLen = reinterpret_cast<const uint8_t*>(jsonStart) - (this->buffer + this->msgStart) +
//							jsonLen + 1;

		Json::Value json;
		std::string jsonParseError;

		if (this->jsonReader->parse(
						(const char*)jsonStart, (const char*)jsonStart + jsonLen, &json, &jsonParseError))
		{
			Channel::Request* request = nullptr;

			try
			{
				request = new Channel::Request(this, json);
			}
			catch (const MediaSoupError& error)
			{
				MS_ERROR_STD("discarding wrong Channel request");
			}

			if (request != nullptr)
			{
				// Notify the listener.
				this->listener->OnChannelRequest(this, request);

				// Delete the Request.
				delete request;
			}
		}
		else
		{
			MS_ERROR_STD("JSON parsing error: %s", jsonParseError.c_str());
		}
	}

	void UnixStreamSocket::UserOnUnixStreamRead()
	{
		MS_TRACE_STD();

		// Be ready to parse more than a single message in a single TCP chunk.
		while (true)
		{
			if (IsClosing())
				return;

			size_t readLen  = this->bufferDataLen - this->msgStart;
			char* jsonStart = nullptr;
			size_t jsonLen;
			int nsRet = netstring_read(
			    reinterpret_cast<char*>(this->buffer + this->msgStart), readLen, &jsonStart, &jsonLen);

			if (nsRet != 0)
			{
				switch (nsRet)
				{
					case NETSTRING_ERROR_TOO_SHORT:
						// Check if the buffer is full.
						if (this->bufferDataLen == this->bufferSize)
						{
							// First case: the incomplete message does not begin at position 0 of
							// the buffer, so move the incomplete message to the position 0.
							if (this->msgStart != 0)
							{
								std::memmove(this->buffer, this->buffer + this->msgStart, readLen);
								this->msgStart      = 0;
								this->bufferDataLen = readLen;
							}
							// Second case: the incomplete message begins at position 0 of the buffer.
							// The message is too big, so discard it.
							else
							{
								MS_ERROR_STD(
								    "no more space in the buffer for the unfinished message being parsed, "
								    "discarding it");

								this->msgStart      = 0;
								this->bufferDataLen = 0;
							}
						}
						// Otherwise the buffer is not full, just wait.

						// Exit the parsing loop.
						return;

					case NETSTRING_ERROR_TOO_LONG:
						MS_ERROR_STD("NETSTRING_ERROR_TOO_LONG");
						break;

					case NETSTRING_ERROR_NO_COLON:
						MS_ERROR_STD("NETSTRING_ERROR_NO_COLON");
						break;

					case NETSTRING_ERROR_NO_COMMA:
						MS_ERROR_STD("NETSTRING_ERROR_NO_COMMA");
						break;

					case NETSTRING_ERROR_LEADING_ZERO:
						MS_ERROR_STD("NETSTRING_ERROR_LEADING_ZERO");
						break;

					case NETSTRING_ERROR_NO_LENGTH:
						MS_ERROR_STD("NETSTRING_ERROR_NO_LENGTH");
						break;
				}

				// Error, so reset and exit the parsing loop.
				this->msgStart      = 0;
				this->bufferDataLen = 0;

				return;
			}

			// If here it means that jsonStart points to the beginning of a JSON string
			// with jsonLen bytes length, so recalculate readLen.
			readLen = reinterpret_cast<const uint8_t*>(jsonStart) - (this->buffer + this->msgStart) +
			          jsonLen + 1;

			Json::Value json;
			std::string jsonParseError;

			if (this->jsonReader->parse(
			        (const char*)jsonStart, (const char*)jsonStart + jsonLen, &json, &jsonParseError))
			{
				Channel::Request* request = nullptr;

				try
				{
					request = new Channel::Request(this, json);
				}
				catch (const MediaSoupError& error)
				{
					MS_ERROR_STD("discarding wrong Channel request");
				}

				if (request != nullptr)
				{
					// Notify the listener.
					this->listener->OnChannelRequest(this, request);

					// Delete the Request.
					delete request;
				}
			}
			else
			{
				MS_ERROR_STD("JSON parsing error: %s", jsonParseError.c_str());
			}

			// If there is no more space available in the buffer and that is because
			// the latest parsed message filled it, then empty the full buffer.
			if ((this->msgStart + readLen) == this->bufferSize)
			{
				this->msgStart      = 0;
				this->bufferDataLen = 0;
			}
			// If there is still space in the buffer, set the beginning of the next
			// parsing to the next position after the parsed message.
			else
			{
				this->msgStart += readLen;
			}

			// If there is more data in the buffer after the parsed message
			// then parse again. Otherwise break here and wait for more data.
			if (this->bufferDataLen > this->msgStart)
			{
				continue;
			}

			break;
		}
	}

	void UnixStreamSocket::UserOnUnixStreamSocketClosed(bool isClosedByPeer)
	{
		MS_TRACE_STD();

		this->closed = true;

		if (isClosedByPeer)
		{
			// Notify the listener.
			this->listener->OnChannelUnixStreamSocketRemotelyClosed(this);
		}
	}
} // namespace Channel
