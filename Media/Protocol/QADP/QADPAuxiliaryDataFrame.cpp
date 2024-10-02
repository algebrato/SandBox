// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPAuxiliaryDataFrame.h"

#include "QADPFrameCommonHeader.h"

using marchnetworks::media::protocol::qadp::QADPAuxiliaryDataFrame;
using marchnetworks::media::protocol::qadp::QADPFrame;
using marchnetworks::media::protocol::qadp::QADPFrameAuxiliaryDataHeader;
using marchnetworks::media::protocol::qadp::QADPFrameCommonHeader;

QADPAuxiliaryDataFrame::QADPAuxiliaryDataFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
	std::unique_ptr<QADPFrameAuxiliaryDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameAuxiliaryDataHeader>&& hostByteOrderDataHeader,
	std::unique_ptr<uint8_t[]> metadata, std::unique_ptr<uint8_t[]> data) :
	QADPFrame(std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader), std::move(metadata), std::move(data)),
	m_networkByteOrderDataHeader(std::move(networkByteOrderDataHeader)),
	m_hostByteOrderDataHeader(std::move(hostByteOrderDataHeader))
{
}

QADPAuxiliaryDataFrame::~QADPAuxiliaryDataFrame()
{
}

std::unique_ptr<QADPAuxiliaryDataFrame> QADPAuxiliaryDataFrame::Make(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
	std::unique_ptr<QADPFrameAuxiliaryDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameAuxiliaryDataHeader>&& hostByteOrderDataHeader,
	std::unique_ptr<uint8_t[]> metadata, std::unique_ptr<uint8_t[]> data)
{
	if (!networkByteOrderCommonHeader || !hostByteOrderCommonHeader)
		throw std::runtime_error("media packet framing header cannot be null");

	if (!networkByteOrderDataHeader || !hostByteOrderDataHeader)
		throw std::runtime_error("auxiliary data header cannot be null");

	if (metadata && hostByteOrderCommonHeader->metadataSize <= 0)
		throw std::runtime_error("unexpected metadata with no size specified");

	if (!metadata && hostByteOrderCommonHeader->metadataSize > 0)
		throw std::runtime_error("unexpected null metadata with size greater than zero");

	if (data && hostByteOrderCommonHeader->dataSize <= 0)
		throw std::runtime_error("unexpected data with no size specified");

	if (!data && hostByteOrderCommonHeader->dataSize > 0)
		throw std::runtime_error("unexpected null data with size greater than zero");

	return std::unique_ptr<QADPAuxiliaryDataFrame>(new QADPAuxiliaryDataFrame(std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader),
		std::move(networkByteOrderDataHeader), std::move(hostByteOrderDataHeader),
		std::move(metadata), std::move(data)));
}

std::string QADPAuxiliaryDataFrame::GetCodecName() const
{
	return m_hostByteOrderDataHeader->codec;
}
