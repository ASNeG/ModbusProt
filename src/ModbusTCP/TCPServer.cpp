/*
   Copyright 2024-2025 Kai Huebl (kai@huebl-sgh.de)

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

#include "ModbusTCP/TCPServer.h"

namespace ModbusTCP
{

	using namespace asio::experimental::awaitable_operators;

	constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);

	TCPServer::TCPServer(
		asio::io_context& ctx
	)
	: TCPBase(ctx)
	{
	}

	TCPServer::TCPServer(
		void
	)
	: TCPBase()
	{
	}

	TCPServer::~TCPServer(
		void
	)
	{
	}

	bool
	TCPServer::open(
		asio::ip::tcp::endpoint listenEndpoint,
		AcceptCallback acceptCallback
	)
	{
		std::cout << "TCPServer::open" << std::endl;

		// Check parameter
		if (acceptor_ != nullptr) {
			return false;
		}

		// Create acceptor socket
		logHandler_->logList(Base::LogLevel::Debug, {
			"open server with ip", listenEndpoint.address().to_string(),
			"and port", std::to_string(listenEndpoint.port())
		});
		acceptor_ = std::make_shared<asio::ip::tcp::acceptor>(ctx(), listenEndpoint);
		if (acceptor_ == nullptr) {
			logHandler_->logList(Base::LogLevel::Debug, {
				"close server with ip", listenEndpoint.address().to_string(),
				"and port", std::to_string(listenEndpoint.port())
			});
			return false;
		}

		// Call listen function
		co_spawn(ctx(), listen(acceptCallback), asio::detached);

		return true;
	}

	void
	TCPServer::close(void)
	{
		std::cout << "TCPServer::close" << std::endl;

		// Check parameter
		if (acceptor_ == nullptr) {
			return;
		}

		// Close and remove acceptor endpoint
		logHandler_->logList(Base::LogLevel::Debug, {"close server"});
		acceptor_->close();
		acceptor_ = nullptr;
	}

	asio::awaitable<void>
	TCPServer::listen(AcceptCallback acceptCallback)
	{
		std::cout << "TCPServer::listen" << std::endl;

		for (;;) {
			// Listen to a new connection from the client
			auto [e, client] = co_await acceptor_->async_accept(use_nothrow_awaitable);
			if (e) {
				co_return;
			}

			// Get handler form application
			auto handler = acceptCallback(ctx(), client);
			if (handler == nullptr) {
				continue;
			}

			// Handle client connection
			co_spawn(ctx(), handler->open(std::move(client)), asio::detached);
		}

		co_return;
	}

}
