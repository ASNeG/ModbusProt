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

#include "ModbusTCP/ModbusTCPServer.h"

namespace ModbusTCP
{

	ModbusTCPServer::ModbusTCPServer(
		asio::io_context& ctx
	)
	: ModbusTCPBase(ctx)
	{
	}

	ModbusTCPServer::ModbusTCPServer(
		void
	)
	: ModbusTCPBase()
	{
	}

	ModbusTCPServer::~ModbusTCPServer(
		void
	)
	{
	}

	bool
	ModbusTCPServer::open(
		asio::ip::tcp::endpoint listenEndpoint
	)
	{
		std::cout << "ModbusTCPServer::open" << std::endl;

		acceptor_ = std::make_shared<asio::ip::tcp::acceptor>(ctx_, listenEndpoint);
		if (acceptor_ == nullptr) {
			return false;
		}
		return true;
	}

	void
	ModbusTCPServer::close(void)
	{
		std::cout << "ModbusTCPServer::close" << std::endl;
		acceptor_->close();
	}

	asio::awaitable<void>
	ModbusTCPServer::listen(void)
	{
		std::cout << "ModbusTCPServer::listen" << std::endl;

		for (;;) {
			auto client = co_await acceptor_->async_accept(asio::use_awaitable);
		}

		co_return;
	}

}
