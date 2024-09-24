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

	ModbusTCPClient::ModbusTCPClient(
		asio::io_context& ctx
	)
	: ModbusTCPBase(ctx)
	{
		createTimer();
	}

	ModbusTCPClient::ModbusTCPClient(
		void
	)
	: ModbusTCPBase()
	{
		createTimer();
	}

	ModbusTCPClient::~ModbusTCPClient(
		void
	)
	{
		// Stop Timer
		stopTimer();
		destroyTimer();
	}

	void
	ModbusTCPClient::createTimer(void)
	{
		timer_ = std::make_shared<asio::steady_timer>(socket_.get_executor());
	}

	void
	ModbusTCPClient::destroyTimer(void)
	{
		timer_ = nullptr;
	}

	asio::awaitable<bool>
	ModbusTCPClient::startTimer(uint32_t timeoutMs)
	{
		timer_->expires_after(std::chrono::milliseconds(timeoutMs));
		auto [e] = co_await timer_->async_wait(use_nothrow_awaitable);
		if (e) {
			co_return false;
		}
		co_return true;
	}

	void
	ModbusTCPClient::stopTimer(void)
	{
		if (timer_ != nullptr) {
			timer_->cancel();
		}
	}

	asio::awaitable<bool>
	ModbusTCPClient::connectToServer(
		asio::ip::tcp::endpoint targetEndpoint,
		StateCallback stateCallback,
		uint32_t reconnectTimeout
	)
	{
		std::cout << "ModbusTCPClient::connectToServer" << std::endl;

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
		uint32_t reconnectTimeout
	)
	{
		std::cout << "ModbusTCPClient::clientLoop" << std::endl;
		loopReady_ = true;
		while(loopReady_) {
			// Connect to server
			auto rc = co_await connectToServer(
				targetEndpoint,
				stateCallback,
				reconnectTimeout

			);
			if (!rc && reconnectTimeout == 0) {
				// Set error state
				state_ = ModbusTCPClientState::Error;
				stateCallback(state_);
				co_return;
			}
			if (!rc && loopReady_) {
				auto rc = co_await startTimer(reconnectTimeout);
				if (!rc) {
					co_return;
				}
				continue;
			}
		}

		co_return;
	}

	void
	ModbusTCPClient::connect(
		asio::ip::tcp::endpoint target,
		StateCallback stateCallback,
		uint32_t reconnectTimeout
	)
	{
		std::cout << "ModbusTCPClient::connect" << std::endl;

		// Connect to server
		co_spawn(
			socket_.get_executor(),
			clientLoop(target, stateCallback, reconnectTimeout),
			asio::detached
		);
	}

	void
	ModbusTCPClient::disconnect(void)
	{
		std::cout << "ModbusTCPClient::disconnect" << std::endl;
		loopReady_ = false;
		socket_.close();
		stopTimer();
	}

}
