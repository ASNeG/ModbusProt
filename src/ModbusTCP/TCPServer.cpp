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

#include "ModbusTCP/TCPServer.h"

namespace ModbusTCP
{

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
		acceptor_ = std::make_shared<asio::ip::tcp::acceptor>(ctx(), listenEndpoint);
		if (acceptor_ == nullptr) {
			return false;
		}

		// Call listen function
		co_spawn(ctx(), listen(), asio::detached);

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
		acceptor_->close();
		acceptor_ = nullptr;
	}

	asio::awaitable<void>
	TCPServer::listen(void)
	{
		std::cout << "TCPServer::listen" << std::endl;

		for (;;) {
			std::cout << "AA" << std::endl;
			auto client = co_await acceptor_->async_accept(asio::use_awaitable);
			std::cout << "BB" << std::endl;
		}

		co_return;
	}

}
