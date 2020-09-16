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
#include <string>

#include "scramjet/error_code.hpp"
#include "scramjet/jet_connection.hpp"

namespace scramjet {
/**
 * @brief Function prototype for a "set state" callback.
 * 
 * If a state is added, you can provide a state callback method. This callback function conform to this type.
 * @param key The key under which the state was registered. This might be useful to use a single callback method for several
 *            states and multiplex via "key".
 * @param value A pointer the the value to which the state shall be set.
 * @param value_length A pointer containing the length of the value.
 * 
 * @return A pointer to the new value of the state, if the value was not just taken over but adapted. If the value was not adapted,
 *         the return value is null_ptr. If the value was adapted, the length of the new value is stored in @p value_length.
 */
typedef std::function<const uint8_t*(const std::string& key, const uint8_t* value, size_t& value_length)> state_callback_t;

typedef std::function<void(enum error_code ec)> response_callback_t;

class jet_peer final {
public:
	jet_peer(std::unique_ptr<jet_connection> c) noexcept;
	virtual ~jet_peer() noexcept;

	void connect(const connected_callback_t& connect_callback, std::chrono::milliseconds timeout) noexcept;
	void disconnect(void) noexcept;

	void add_state(const std::string& key, const uint8_t* value, size_t value_length,
	               const state_callback_t& state_callback, std::chrono::milliseconds state_set_timeout,
	               const response_callback_t& response_callback, std::chrono::milliseconds response_timoeut);

private:
	std::unique_ptr<jet_connection> m_connection;
	connected_callback_t m_connected_callback;

	void connected(scramjet::error_code ec);
	void version_received(enum error_code ec, const uint8_t* message, size_t message_length);
	void message_received(enum error_code ec, const uint8_t* message, size_t message_length);
};
} // namespace scramjet

#endif
