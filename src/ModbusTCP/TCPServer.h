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

#ifndef __ModbusTCP_TCPServer_h__
#define __ModbusTCP_TCPServer_h__

#include <functional>

#include "ModbusTCP/TCPServerHandler.h"
#include "ModbusTCP/TCPBase.h"

namespace ModbusTCP
{

	class TCPServer
	: public TCPBase
	{
	  public:
		using AcceptCallback = std::function<TCPServerHandler::SPtr (asio::io_context& ctx, asio::ip::tcp::socket& client)>;

		TCPServer(
			asio::io_context& ctx
		);
		TCPServer(
			void
		);
		virtual ~TCPServer(
			void
		);

		bool open(
			asio::ip::tcp::endpoint listenEndpoint,
			AcceptCallback acceptCallback
		);
		void close(void);

	  private:
	  	std::shared_ptr<asio::ip::tcp::acceptor> acceptor_ = nullptr;

	  	asio::awaitable<void> listen(AcceptCallback acceptCallback);
	};

}

#endif
