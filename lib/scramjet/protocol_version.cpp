/*
 * SPDX-License-Identifier: MIT
 *
 * The MIT License (MIT)
 *
 * Copyright (c) <2020> Matthias Loy, Stephan Gatzka
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <boost/endian/conversion.hpp>

#include "protocol_version.hpp"

namespace scramjet {
protocol_version::protocol_version(const uint8_t* buffer) noexcept
{
	std::memcpy(&m_major, buffer, sizeof(m_major));
	buffer += sizeof(m_major);
	boost::endian::little_to_native_inplace(m_major);

	std::memcpy(&m_minor, buffer, sizeof(m_minor));
	buffer += sizeof(m_minor);
	boost::endian::little_to_native_inplace(m_minor);

	std::memcpy(&m_patch, buffer, sizeof(m_patch));
	boost::endian::little_to_native_inplace(m_patch);
}

protocol_version::protocol_version(uint32_t major, uint32_t minor, uint32_t patch) noexcept
{
	m_major = major;
	m_minor = minor;
	m_patch = patch;
}

void protocol_version::print() const noexcept
{
	std::cout << "Protocol version: " << std::dec << m_major << "." << m_minor << "." << m_patch << std::endl;
}

bool protocol_version::is_compatible(const protocol_version& v) const noexcept
{
	if (m_major != v.m_major) {
		return false;
	}
	
	if (m_minor > v.m_minor) {
		return false;
	}

	if (m_minor < v.m_minor) {
		return true;
	}

	if (m_patch > v.m_patch) {
		return false;
	}

	return true;
}

size_t protocol_version::get_version_size(void) noexcept
{
	return sizeof(protocol_version::m_major) + sizeof(protocol_version::m_minor) + sizeof(protocol_version::m_patch);
}
} // namespace scramjet
