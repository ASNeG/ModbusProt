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

#include "ModbusTCP/ModbusTCPClient.h"

namespace ModbusTCP
{

	using namespace asio::experimental::awaitable_operators;

	constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);

	ModbusTCPClient::ModbusTCPClient(asio::io_context ctx)
	: socket_(ctx)
	, state_(ModbusTCPClientState::Init)
	{
	}

	ModbusTCPClient::~ModbusTCPClient(void)
	{
	}

	asio::awaitable<bool>
	ModbusTCPClient::connectToServer(
		asio::ip::tcp::endpoint targetEndpoint,
		StateCallback stateCallback,
		uint32_t connectTimeout
	)
	{
		// Set connecting state
		state_ = ModbusTCPClientState::Connecting;
		stateCallback(state_);

		// Connect to server
		auto [e] = co_await socket_.async_connect(targetEndpoint, use_nothrow_awaitable);
		if (e) {
			// Set close state
			state_ = ModbusTCPClientState::Close;
			stateCallback(state_);
			co_return false;
		}

		// Set connected state
		state_ = ModbusTCPClientState::Connected;
		stateCallback(state_);
		co_return true;
	}

	asio::awaitable<void>
	ModbusTCPClient::clientLoop(
		asio::ip::tcp::endpoint targetEndpoint,
		StateCallback stateCallback,
		uint32_t connectTimeout
	)
	{
		for(;;) {
			// Connect to server
			auto rc = co_await connectToServer(
				targetEndpoint,
				stateCallback,
				connectTimeout

			);
			if (!rc) {
				// Start timer

				continue;
			}
		}

		co_return;
	}

	asio::awaitable<void>
	ModbusTCPClient::connect(
		asio::ip::tcp::endpoint target,
		StateCallback stateCallback,
		uint32_t connectTimeout
	)
	{
		stateCallback_ = stateCallback;

		// Connect to server
		co_spawn(
			socket_.get_executor(),
			clientLoop(target, stateCallback, connectTimeout),
			asio::detached
		);

		co_return;
	}

	void
	ModbusTCPClient::disconnect(void)
	{
		;

	}

}