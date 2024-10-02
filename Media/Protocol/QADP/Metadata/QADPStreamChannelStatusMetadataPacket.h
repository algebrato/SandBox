// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPSTREAMCHANNELSTATUSMETADATAPACKET__H__
#define __MN_MEDIA_QADPSTREAMCHANNELSTATUSMETADATAPACKET__H__

#include "QADPMetadataPacket.h"
#include "QADPMetadataPacketHeader.h"
#include "QADPStreamChannelBusyStatus.h"
#include "QADPStreamChannelImageSlicingType.h"

#include <memory>
#include <unordered_map>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				class  QADPStreamChannelStatusMetadataPacket : public QADPMetadataPacket
				{
				private:
					QADPStreamChannelStatusMetadataPacket(int8_t isUserUnauthorized, int8_t isStreamChannelBeingRecorded, int8_t isStreamChannelDisconnected, int8_t isEncryptedStreamChannel, int8_t unused, int8_t streamChannelBusyStatus, int8_t imageSlicingType, int8_t isAudioOnlyStreamChannel);

				public:
					virtual ~QADPStreamChannelStatusMetadataPacket();

					static std::unique_ptr<QADPStreamChannelStatusMetadataPacket> Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size);

				private:
					int8_t m_isUserUnauthorized;
					int8_t m_isStreamChannelBeingRecorded;
					int8_t m_isStreamChannelDisconnected;
					int8_t m_isEncryptedStreamChannel;
					int8_t m_unused;
					int8_t m_streamChannelBusyStatus;
					int8_t m_imageSlicingType;
					int8_t m_isAudioOnlyStreamChannel;

					static std::unordered_map<decltype(m_streamChannelBusyStatus), QADPStreamChannelBusyStatus> s_streamChannelBusyStates;
					static std::unordered_map<decltype(m_imageSlicingType), QADPStreamChannelImageSlicingType> s_imageSlicingTypes;

				public:
					QADPStreamChannelBusyStatus GetStreamChannelBusyStatus() const;
					QADPStreamChannelImageSlicingType GetStreamChannelImageSlicingType() const;
					bool IsAudioOnlyStreamChannel() const;
					bool IsEncryptedStreamChannel() const;
					bool IsUserUnauthorized() const;
					bool IsStreamChannelBeingRecorded() const;
					bool IsStreamChannelDisconnected() const;
				};
			}
		}
	}
}

#endif
