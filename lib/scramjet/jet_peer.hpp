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

#ifndef SCRAMJET__JET_PEER_HPP
#define SCRAMJET__JET_PEER_HPP

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "scramjet/error_code.hpp"
#include "scramjet/function_type.h"
#include "scramjet/jet_connection.hpp"

namespace scramjet {

class jet_peer final {
public:
	/// Opaque data block
	using data = std::vector <uint8_t >;

	/// @param responseData response data from other peer
	using response_callback_code = std::function < void (int resultCode) >;

	jet_peer(std::unique_ptr<jet_connection> c) noexcept;
	virtual ~jet_peer() noexcept;

	void connect(const connected_callback_t& connect_callback, std::chrono::milliseconds timeout) noexcept;
	void disconnect(void) noexcept;

	/// @param key Key of the state to be created.
	/// @param value The initial state value
	void addState(scramjet::jet_peer& peer, const std::string& key, data value, response_callback_code callback);

private:
	/// @todo We need this id to differentiate all requests that are in flight. Are 16 bit enough or too much?
	static uint16_t s_nextRequestId;
	std::unique_ptr<jet_connection> m_connection;
	connected_callback_t m_connected_callback;

	void connected(scramjet::error_code ec);
	void version_received(enum error_code ec, const uint8_t* message, size_t message_length);
	void message_received(enum error_code ec, const uint8_t* message, size_t message_length);

	/// private because it is not to be called directly
	void sendRequest(function_type function, const std::string& key, data value, response_callback_code callback);

};
} // namespace scramjet

#endif
