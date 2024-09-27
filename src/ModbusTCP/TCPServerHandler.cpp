/*
   Copyright 2024 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Kai Huebl (kai@huebl-sgh.de)
 */

#include <asio/experimental/as_tuple.hpp>
#include <asio/experimental/awaitable_operators.hpp>

#include "ModbusTCP/TCPServerHandler.h"

namespace ModbusTCP
{

	using namespace asio::experimental::awaitable_operators;

	constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);


	TCPServerHandler::TCPServerHandler(
		void
	)
	{
	}

	TCPServerHandler::~TCPServerHandler(
		void
	)
	{
	}

	asio::awaitable<void>
	TCPServerHandler::open(asio::ip::tcp::socket socket)
	{
		std::array<char, 512> data;

		for (;;) {
			auto [e1, n1] = co_await socket.async_read_some(asio::buffer(data), use_nothrow_awaitable);
			if (e1) {
				break;
			}

#if 0
			auto [e2, n2] = co_await async_write(client_, asio::buffer(data, n1), use_nothrow_awaitable);
			if (e2) {
				break;
			}
#endif
		}
	}

	void
	TCPServerHandler::close(void)
	{

	}

}
