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

#ifndef __ModbusProt_ModbusTCPBase_h__
#define __ModbusProt_ModbusTCPBase_h__

//#include <functional>
#include <asio.hpp>

#include "ModbusTCP/ModbusTCP.h"

namespace ModbusTCP
{

	class ModbusTCPBase
	{
	  public:
		ModbusTCPBase(
			asio::io_context& ctx
		);
		ModbusTCPBase(
			void
		);
		virtual ~ModbusTCPBase(
			void
		);

		bool getEndpoint(
			const std::string& ipAddress,
			const std::string& port,
			asio::ip::tcp::endpoint& endpoint
		);

	  protected:
		asio::io_context ctx_;
		asio::ip::tcp::socket socket_;

	  private:
		asio::io_context::work *work_ = nullptr;
		bool useOwnThread_ = false;
		std::thread thread_;

		void startThread(void);
		void stopThread(void);
	};

}

#endif
