#ifndef MS_RTC_RTCP_FEEDBACK_RTP_NACK_HPP
#define MS_RTC_RTCP_FEEDBACK_RTP_NACK_HPP

#include "../../common.hpp"
#include "FeedbackRtp.hpp"

/* RFC 4585
 * Generic NACK message (NACK)
 *
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |              PID              |             BPL               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

namespace RTC
{
	namespace RTCP
	{
		class FeedbackRtpNackItem : public FeedbackItem
		{
		public:
			struct Header
			{
				uint16_t packetId;
				uint16_t lostPacketBitmask;
			};

		public:
			static const FeedbackRtp::MessageType messageType{ FeedbackRtp::MessageType::NACK };

		public:
			static FeedbackRtpNackItem* Parse(const uint8_t* data, size_t len);

		public:
			explicit FeedbackRtpNackItem(Header* header);
			explicit FeedbackRtpNackItem(FeedbackRtpNackItem* item);
			FeedbackRtpNackItem(uint16_t packetId, uint16_t lostPacketBitmask);
			~FeedbackRtpNackItem() override = default;

			uint16_t GetPacketId() const;
			uint16_t GetLostPacketBitmask() const;

			/* Virtual methods inherited from FeedbackItem. */
		public:
			void Dump() const override;
			size_t Serialize(uint8_t* buffer) override;
			size_t GetSize() const override;

		private:
			Header* header{ nullptr };
		};

		// Nack packet declaration.
		using FeedbackRtpNackPacket = FeedbackRtpItemsPacket<FeedbackRtpNackItem>;

		/* Inline instance methods. */

		inline FeedbackRtpNackItem::FeedbackRtpNackItem(Header* header) : header(header)
		{
		}

		inline FeedbackRtpNackItem::FeedbackRtpNackItem(FeedbackRtpNackItem* item)
		    : header(item->header)
		{
		}

		inline size_t FeedbackRtpNackItem::GetSize() const
		{
			return sizeof(Header);
		}

		inline uint16_t FeedbackRtpNackItem::GetPacketId() const
		{
			return uint16_t{ ntohs(this->header->packetId) };
		}

		inline uint16_t FeedbackRtpNackItem::GetLostPacketBitmask() const
		{
			return uint16_t{ ntohs(this->header->lostPacketBitmask) };
		}
	} // namespace RTCP
} // namespace RTC

#endif
