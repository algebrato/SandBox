// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPLensConfigurationMetadataPacket.h"

#include <type_traits>

#ifndef _WIN32
#include <cstring>
#endif // !_WIN32

using marchnetworks::media::protocol::qadp::QADPLensConfigurationMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacket;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketHeader;
using marchnetworks::media::protocol::qadp::QADPMetadataPacketType;

QADPLensConfigurationMetadataPacket::QADPLensConfigurationMetadataPacket(int8_t unused[3], uint8_t moutingType, std::unique_ptr<int8_t[]>&& model, std::size_t modelLength) :
	QADPMetadataPacket(QADPMetadataPacketType::LensConfiguration),
	m_mountingType(moutingType),
	m_model(std::move(model)),
	m_modelLength(modelLength)
{
	memcpy(m_unused, unused, sizeof(m_unused));
}

QADPLensConfigurationMetadataPacket::~QADPLensConfigurationMetadataPacket()
{
}

std::unique_ptr<QADPLensConfigurationMetadataPacket> QADPLensConfigurationMetadataPacket::Parse(uint8_t* bytes, decltype(QADPMetadataPacketHeader::length) size)
{
	std::unique_ptr<QADPLensConfigurationMetadataPacket> packet;

	constexpr auto MinMetadataPacketSize = sizeof(m_unused) + sizeof(m_mountingType);

	if (bytes && size >= MinMetadataPacketSize)
	{
		auto unused = reinterpret_cast<std::remove_all_extents<decltype(m_unused)>::type*>(bytes);
		bytes += sizeof(m_unused);

		auto mountingType = Read<decltype(m_mountingType)>(bytes);
		auto modelLength = size > MinMetadataPacketSize ? strnlen(reinterpret_cast<char*>(bytes), size - MinMetadataPacketSize) : 0;

		decltype(m_model) model;
		if (modelLength > 0)
		{
			model = std::make_unique<decltype(m_model)::element_type[]>(modelLength);
			memcpy(model.get(), bytes, modelLength);
		}

		packet = std::move(std::unique_ptr<QADPLensConfigurationMetadataPacket>(new QADPLensConfigurationMetadataPacket(unused, mountingType, std::move(model), modelLength)));
	}

	return packet;
}

decltype(QADPLensConfigurationMetadataPacket::m_mountingType) QADPLensConfigurationMetadataPacket::GetMountingType() const
{
	return m_mountingType;
}

std::string QADPLensConfigurationMetadataPacket::GetModel() const
{
	return m_modelLength > 0 ? std::string(reinterpret_cast<char*>(m_model.get()), m_modelLength) : "";
}
