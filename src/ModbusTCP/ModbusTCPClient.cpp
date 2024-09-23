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

	ModbusTCPClient::ModbusTCPClient(asio::io_context& ctx)
	: socket_(ctx)
	, state_(ModbusTCPClientState::Init)
	, useOwnThread_(false)
	{
	}

	ModbusTCPClient::ModbusTCPClient(
		void
	)
	: socket_(ctx_)
	, state_(ModbusTCPClientState::Init)
	, useOwnThread_(true)
	, thread_([this](void){ startThread(); })
	{
	}

	ModbusTCPClient::~ModbusTCPClient(void)
	{
		// Stop own event loop thread
		stopThread();
	}

	void ModbusTCPClient::startThread(void)
	{
		work_ = new asio::io_context::work(ctx_);
		ctx_.run();
	}

	void ModbusTCPClient::stopThread(void)
	{
		if (useOwnThread_) {
			delete work_;
			work_ = nullptr;
			thread_.join();
		}
	}

	asio::awaitable<void>
	ModbusTCPClient::startTimer(uint32_t timeoutMs)
	{
		timer_ = std::make_shared<asio::steady_timer>(socket_.get_executor());
		timer_->expires_after(std::chrono::milliseconds(timeoutMs));
		co_await timer_->async_wait(use_nothrow_awaitable);
	}

	void
	ModbusTCPClient::stopTimer(void)
	{
		if (timer_ != nullptr) {
			timer_->cancel();
		}
	}

	bool
	ModbusTCPClient::getEndpoint(
		const std::string& ipAddress,
		const std::string& port,
		asio::ip::tcp::endpoint& endpoint
	)
	{
		try {
			endpoint = *asio::ip::tcp::resolver(socket_.get_executor()).resolve(ipAddress, port);
		}
		catch (...) {
			return false;
		}
		return true;
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
			std::cout << "ERR=" << e.message() << std::endl;
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
			if (!rc) {
				co_await startTimer(reconnectTimeout);
#if 0
				asio::steady_timer timer(socket_.get_executor());
				timer.expires_after(std::chrono::milliseconds(reconnectTimeout));
				co_await timer.async_wait(use_nothrow_awaitable);
#endif
				continue;
			}
		}

		std::cout << "LOOP END" << std::endl;
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
		stopTimer();
		socket_.close();

	}

}
