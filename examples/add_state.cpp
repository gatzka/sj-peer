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

#include <boost/asio.hpp>
#include <chrono>
#include <cstdbool>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>

#include <scramjet/error_code.hpp>
#include <scramjet/jet_peer.hpp>
#include <scramjet/socket_jet_connection.hpp>

static void connected(enum scramjet::error_code ec)
{
	if (ec == scramjet::error_code::SCRAMJET_OK) {
		std::cout << "peer connected!" << std::endl;
	} else {
		std::cout << "peer not connected!" << std::endl;
    }
}

int main(void)
{
	boost::asio::io_context io_context;
	scramjet::jet_peer peer(std::make_unique<scramjet::socket_jet_connection>(io_context, "localhost"));

	peer.connect(std::bind(&connected, std::placeholders::_1), std::chrono::milliseconds(100));
	io_context.run();
	return EXIT_SUCCESS;
}
