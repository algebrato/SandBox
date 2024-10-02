// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "Timestamp.h"

using marchnetworks::media::chrono::Timestamp;

Timestamp::Timestamp() :
	m_timestamp()
{
}

template <>
Timestamp::Timestamp(std::chrono::nanoseconds value) :
	m_timestamp(value)
{
}

Timestamp::~Timestamp()
{
}

Timestamp Timestamp::Max() noexcept
{
	return Timestamp((std::chrono::nanoseconds::max)());
}

Timestamp Timestamp::Min() noexcept
{
	return Timestamp((std::chrono::nanoseconds::min)());
}

bool Timestamp::operator == (const Timestamp& other) const
{
	return m_timestamp == other.m_timestamp;
}

bool Timestamp::operator < (const Timestamp& other) const
{
	return m_timestamp < other.m_timestamp;
}

bool Timestamp::operator <= (const Timestamp & other) const
{
	return m_timestamp <= other.m_timestamp;
}

bool Timestamp::operator > (const Timestamp& other) const
{
	return m_timestamp > other.m_timestamp;
}

bool Timestamp::operator>=(const Timestamp& other) const
{
	return m_timestamp >= other.m_timestamp;
}

Timestamp Timestamp::operator+(const Timestamp& other) const
{
	return m_timestamp + other.m_timestamp;
}

Timestamp Timestamp::operator-(const Timestamp& other) const
{
	return m_timestamp - other.m_timestamp;
}

template <>
std::chrono::nanoseconds Timestamp::Value() const
{
	return m_timestamp;
}
