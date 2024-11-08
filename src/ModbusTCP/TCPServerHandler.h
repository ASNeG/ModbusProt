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

#ifndef __ModbusTCP_TCPServerHandler_h__
#define __ModbusTCP_TCPServerHandler_h__

#include <memory>
#include <asio.hpp>

namespace ModbusTCP
{

	class TCPServerHandler
	{
	  public:
		using SPtr = std::shared_ptr<TCPServerHandler>;

		TCPServerHandler(
			void
		);
		~TCPServerHandler(
			void
		);

		asio::awaitable<void> open(asio::ip::tcp::socket socket);
		void close(void);
	};

}

#endif
