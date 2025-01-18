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

#ifndef __ModbusTCP_TCPBase_h__
#define __ModbusTCP_TCPBase_h__

#include <memory>
#include <asio.hpp>

#include "Base/LogHandler.h"
#include "Base/LogDefault.h"
#include "ModbusTCP/ModbusTCP.h"

namespace ModbusTCP
{

	class TCPBase
	{
	  public:
		TCPBase(
			asio::io_context& ctx
		);
		TCPBase(
			void
		);
		virtual ~TCPBase(
			void
		);

		void logHandler(Base::LogHandler::SPtr& logHandler);
		Base::LogHandler::SPtr logHandler(void);

		asio::io_context& ctx(void);

		bool getEndpoint(
			const std::string& ipAddress,
			const std::string& port,
			asio::ip::tcp::endpoint& endpoint
		);

	  protected:
		Base::LogHandler::SPtr logHandler_ = std::make_shared<Base::LogDefault>();

	  private:
		asio::io_context ctx_;			// Internal context
		asio::io_context& ctxRef_;		// Reference to internal or external context
		asio::io_context::work *work_ = nullptr;
		bool useOwnThread_ = false;
		std::thread thread_;

		void startThread(void);
		void stopThread(void);
	};

}

#endif
