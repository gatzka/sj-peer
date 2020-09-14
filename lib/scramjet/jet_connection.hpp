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

#ifndef SCRAMJET__JET_CONNECTION_HPP
#define SCRAMJET__JET_CONNECTION_HPP

#include <chrono>
#include <cstdbool>
#include <cstdint>
#include <cstdlib>
#include <functional>

#include "scramjet/error_code.hpp"

namespace scramjet {

typedef std::function<void(enum error_code ec)> connected_callback_t;
typedef std::function<void(enum error_code ec)> disconnected_callback_t;
typedef std::function<void(enum error_code ec, const uint8_t *message, size_t message_length)> message_received_callback_t;

class jet_connection {
public:
	virtual ~jet_connection() noexcept = 0;
	virtual void connect(const connected_callback_t& connect_callback, std::chrono::milliseconds timeout) noexcept = 0;
	virtual void disconnect(void) noexcept = 0;
	virtual void receive_message(const message_received_callback_t callback) noexcept = 0;

protected:
	connected_callback_t m_connected_callback = nullptr;
	std::chrono::milliseconds m_connect_timeout = std::chrono::milliseconds(0);

	message_received_callback_t m_message_received_callback = nullptr;
};
} // namespace scramjet

#endif
