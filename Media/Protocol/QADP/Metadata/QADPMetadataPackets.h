// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPMETADATAPACKETS__H__
#define __MN_MEDIA_QADPMETADATAPACKETS__H__

#include "QADPAudioConfigurationMetadataPacket.h"
#include "QADPDriveSerialNumberMetadataPacket.h"
#include "QADPImageAspectRatioMetadataPacket.h"
#include "QADPImageEnhancementsMetadataPacket.h"
#include "QADPImageTransformsMetadataPacket.h"
#include "QADPLensConfigurationMetadataPacket.h"
#include "QADPPrivacyPatchMetadataPacket.h"
#include "QADPSeprobanMetadataPacket.h"
#include "QADPStreamChannelStatusMetadataPacket.h"

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				template <QADPMetadataPacketType _qadpMetadataPacketType>
				struct QADPMetadataPackets;

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::AudioConfiguration>
				{
					using packetType = QADPAudioConfigurationMetadataPacket;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::DriveSerialNumber>
				{
					using packetType = QADPDriveSerialNumberMetadataPacket;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::ImageAspectRatio>
				{
					using packetType = QADPImageAspectRatioMetadataPacket;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::ImageEnhancements>
				{
					using packetType = QADPImageEnhancementsMetadataPacket;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::ImageTransforms>
				{
					using packetType = QADPImageTransformsMetadataPacket;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::StreamChannelStatus>
				{
					using packetType = QADPStreamChannelStatusMetadataPacket;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::LensConfiguration>
				{
					using packetType = QADPLensConfigurationMetadataPacket;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::Seproban>
				{
					using packetType = QADPSeprobanMetadataPacket;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::SmallMosaicPrivacyPatches>
				{
					using packetType = QADPPrivacyPatchMetadataPacket<static_cast<QADPPrivacyPatchType>(QADPMetadataPacketType::SmallMosaicPrivacyPatches)>;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::MosaicPrivacyPatches>
				{
					using packetType = QADPPrivacyPatchMetadataPacket<static_cast<QADPPrivacyPatchType>(QADPMetadataPacketType::MosaicPrivacyPatches)>;
				};

				template <>
				struct QADPMetadataPackets<QADPMetadataPacketType::BlackPrivacyPatches>
				{
					using packetType = QADPPrivacyPatchMetadataPacket<static_cast<QADPPrivacyPatchType>(QADPMetadataPacketType::BlackPrivacyPatches)>;
				};
			}
		}
	}
}

#endif
