// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPPRIVACYPATCHMETADATAPACKET__H__
#define __MN_MEDIA_QADPPRIVACYPATCHMETADATAPACKET__H__

#include "QADPMetadataPacket.h"
#include "QADPMetadataPacketHeader.h"
#include "QADPPrivacyPatchLocation.h"
#include "QADPPrivacyPatchType.h"

#include <memory>
#include <vector>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				template <QADPPrivacyPatchType _privacyPatchType>
				class QADPPrivacyPatchMetadataPacket : public QADPMetadataPacket
				{
				private:
					QADPPrivacyPatchMetadataPacket();

				public:
					virtual ~QADPPrivacyPatchMetadataPacket();

					static std::unique_ptr<QADPPrivacyPatchMetadataPacket> Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size);

					static constexpr QADPPrivacyPatchType PatchType = _privacyPatchType;

					const std::vector<QADPPrivacyPatchLocation>& GetPatches() const;

				protected:
					void Add(QADPPrivacyPatchLocation privacyPatchLocation);

				private:
					std::vector<QADPPrivacyPatchLocation> m_patches;

				public:
				};

				template <QADPPrivacyPatchType _privacyPatchType>
				QADPPrivacyPatchMetadataPacket<_privacyPatchType>::QADPPrivacyPatchMetadataPacket() :
					QADPMetadataPacket(static_cast<QADPMetadataPacketType>(_privacyPatchType))
				{
				}

				template <QADPPrivacyPatchType _privacyPatchType>
				QADPPrivacyPatchMetadataPacket<_privacyPatchType>::~QADPPrivacyPatchMetadataPacket()
				{
				}

				template <QADPPrivacyPatchType _privacyPatchType>
				std::unique_ptr<QADPPrivacyPatchMetadataPacket<_privacyPatchType>> QADPPrivacyPatchMetadataPacket<_privacyPatchType>::Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size)
				{
					std::unique_ptr<QADPPrivacyPatchMetadataPacket<_privacyPatchType>> metadataPacket(new QADPPrivacyPatchMetadataPacket<_privacyPatchType>());

					for (auto i = 0; i < size / sizeof(QADPPrivacyPatchLocation); ++i)
					{
						QADPPrivacyPatchLocation privacyPatchLocation;

						privacyPatchLocation.left = (std::max)(Read<decltype(QADPPrivacyPatchLocation::left)>(bytes), static_cast<decltype(QADPPrivacyPatchLocation::left)>(0));
						privacyPatchLocation.top = (std::max)(Read<decltype(QADPPrivacyPatchLocation::top)>(bytes), static_cast<decltype(QADPPrivacyPatchLocation::top)>(0));
						privacyPatchLocation.width = (std::min)(Read<decltype(QADPPrivacyPatchLocation::width)>(bytes), static_cast<decltype(QADPPrivacyPatchLocation::width)>(10000));
						privacyPatchLocation.height = (std::min)(Read<decltype(QADPPrivacyPatchLocation::height)>(bytes), static_cast<decltype(QADPPrivacyPatchLocation::height)>(10000));

						metadataPacket->Add(privacyPatchLocation);
					}

					return metadataPacket;
				}

				template <QADPPrivacyPatchType _privacyPatchType>
				const std::vector<QADPPrivacyPatchLocation>& QADPPrivacyPatchMetadataPacket<_privacyPatchType>::GetPatches() const
				{
					return m_patches;
				}

				template <QADPPrivacyPatchType _privacyPatchType>
				void QADPPrivacyPatchMetadataPacket<_privacyPatchType>::Add(QADPPrivacyPatchLocation privacyPatchLocation)
				{
					m_patches.push_back(privacyPatchLocation);
				}
			}
		}
	}
}

#endif
