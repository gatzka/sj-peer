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
#include <memory>

#include "scramjet/error_code.hpp"
#include "scramjet/jet_connection.hpp"

namespace scramjet {

class jet_peer final {
public:
	jet_peer(std::unique_ptr<jet_connection> c) noexcept;
	virtual ~jet_peer() noexcept;

	void connect(const connected_callback_t& connect_callback, std::chrono::milliseconds timeout) noexcept;
	void disconnect(void) noexcept;

private:
	std::unique_ptr<jet_connection> m_connection;
	connected_callback_t m_connected_callback;

	void connected(scramjet::error_code ec);
	void version_received(enum error_code ec, const uint8_t* message, size_t message_length);
	void message_received(enum error_code ec, const uint8_t* message, size_t message_length);
};
} // namespace scramjet

#endif
