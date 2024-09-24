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

#ifndef __ModbusProt_ModbusTCPServer_h__
#define __ModbusProt_ModbusTCPServer_h__

#include <functional>

#include "ModbusTCP/ModbusTCPBase.h"
#include "ModbusTCP/ModbusTCP.h"

namespace ModbusTCP
{

	enum class ModbusTCPServerState
	{
		Init
	};

	class ModbusTCPServer
	: public ModbusTCPBase
	{
	  public:
		ModbusTCPServer(
			asio::io_context& ctx
		);
		ModbusTCPServer(
			void
		);
		virtual ~ModbusTCPServer(
			void
		);

		bool open(
			asio::ip::tcp::endpoint listenEndpoint
		);
		void close(void);

	  private:
	  	ModbusTCPServerState state_ = ModbusTCPServerState::Init;
	  	std::shared_ptr<asio::ip::tcp::acceptor> acceptor_ = nullptr;

	  	asio::awaitable<void> listen(void);
	};

}

#endif
