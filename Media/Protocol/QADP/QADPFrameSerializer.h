// Copyright © 2024 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAMESERIALIZER__H__
#define __MN_MEDIA_QADPFRAMESERIALIZER__H__

#include "QADPFrame.h"
#include "QADPFrameCommonHeader.h"
#include "Services/QADPFrameParser.h"

#include <functional>

namespace marchnetworks
{
	namespace media
	{
		namespace io
		{
			class IInputStream;
		}

		namespace protocol
		{
			namespace qadp
			{
				struct QADPFrameCommonHeader;

				template <QADPFrameType _qadpFrameType>
				struct QADPFrames;

				class QADPFrameSerializer
				{
				public:
					QADPFrameSerializer() = delete;

					static std::unique_ptr<QADPFrame> Deserialize(media::io::IInputStream& stream);

				private:
					template <QADPFrameType _qadpFrameType>
					static std::unique_ptr<QADPFrame> Deserialize(media::io::IInputStream& stream, std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader);

					template <typename T>
					static std::unique_ptr<T> ReadBytes(media::io::IInputStream& stream);
					static std::unique_ptr<uint8_t[]> ReadBytes(media::io::IInputStream& stream, std::size_t numberOfBytesToRead);

					static std::unordered_map<QADPFrameType, std::function<std::unique_ptr<QADPFrame>(media::io::IInputStream&, std::unique_ptr<QADPFrameCommonHeader>&&, std::unique_ptr<QADPFrameCommonHeader>&&)>> s_deserializationFunctions;
				};

				template <QADPFrameType _qadpFrameType>
				std::unique_ptr<QADPFrame> QADPFrameSerializer::Deserialize(media::io::IInputStream& stream, std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader)
				{
					std::unique_ptr<QADPFrame> qadpFrame;

					std::unique_ptr<typename QADPFrames<_qadpFrameType>::headerType> networkByteOrderDataHeader = ReadBytes<typename QADPFrames<_qadpFrameType>::headerType>(stream);
					if (networkByteOrderDataHeader)
					{
						auto hostByteOrderDataHeader = std::move(QADPFrameParser::ParseDataHeader(*networkByteOrderDataHeader));

						std::unique_ptr<uint8_t[]> metadata;
						if (hostByteOrderCommonHeader->metadataSize <= 0 || (metadata = ReadBytes(stream, hostByteOrderCommonHeader->metadataSize)))
						{
							std::unique_ptr<uint8_t[]> data;
							if (hostByteOrderCommonHeader->dataSize <= 0 || (data = ReadBytes(stream, hostByteOrderCommonHeader->dataSize)))
								qadpFrame = std::move(QADPFrames<_qadpFrameType>::frameType::Make(std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader),
									std::move(networkByteOrderDataHeader), std::move(hostByteOrderDataHeader),
									std::move(metadata), std::move(data)));
						}
					}

					return qadpFrame;
				}

				template <>
				std::unique_ptr<QADPFrame> QADPFrameSerializer::Deserialize<QADPFrameType::EndOfStream>(media::io::IInputStream& stream, std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader);

				template <>
				std::unique_ptr<QADPFrame> QADPFrameSerializer::Deserialize<QADPFrameType::Text>(media::io::IInputStream& stream, std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader);

				template <>
				std::unique_ptr<QADPFrame> QADPFrameSerializer::Deserialize<QADPFrameType::Video>(media::io::IInputStream& stream, std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader);

				template <typename T>
				std::unique_ptr<T> QADPFrameSerializer::ReadBytes(media::io::IInputStream& stream)
				{
					return std::unique_ptr<T>(reinterpret_cast<T*>(ReadBytes(stream, sizeof(T)).release()));
				}
			}
		}
	}
}

#endif
