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

#ifndef __ModbusProt_ModbusTCPClient_h__
#define __ModbusProt_ModbusTCPClient_h__

#include <functional>

#include "ModbusTCP/ModbusTCPBase.h"
#include "ModbusTCP/ModbusTCP.h"

namespace ModbusTCP
{

	enum class ModbusTCPClientState
	{
		Init,
		Connecting,
		Connected,
		Close,
		Error
	};

	class ModbusTCPClient
	: public ModbusTCPBase
	{
	  public:
		using StateCallback = std::function<void (ModbusTCPClientState)>;

		ModbusTCPClient(
			asio::io_context& ctx
		);
		ModbusTCPClient(
			void
		);
		virtual ~ModbusTCPClient(
			void
		);

		void connect(
			asio::ip::tcp::endpoint target,
			StateCallback stateCallback,
			uint32_t reconnectTimeout = 0
		);
		void disconnect(void);

	  private:
		bool loopReady_ = false;
		std::shared_ptr<asio::steady_timer> timer_ = nullptr;
		ModbusTCPClientState state_ = ModbusTCPClientState::Init;

		void createTimer(void);
		void destroyTimer(void);
		asio::awaitable<bool> startTimer(uint32_t timeoutMs);
		void stopTimer(void);

		asio::awaitable<bool> connectToServer(
			asio::ip::tcp::endpoint targetEndpoint,
			StateCallback stateCallback,
			uint32_t reconnectTimeout
		);
		asio::awaitable<void> clientLoop(
			asio::ip::tcp::endpoint targetEndpoint,
			StateCallback stateCallback,
			uint32_t reconnectTimeout
		);
	};

}

#endif
