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

#ifndef __ModbusTCP_TCPClient_h__
#define __ModbusTCP_TCPClient_h__

#include <functional>
#include <memory>

#include "Base/Queue.h"
#include "ModbusTCP/TCPBase.h"
#include "ModbusProt/ModbusPDU.h"
#include "ModbusProt/ModbusCallback.h"

namespace ModbusTCP
{

	enum class TCPClientState
	{
		Init,			// The connection class was created
		Connecting,		// The connection is currently being opened
		Connected,		// The connection is open
		Close,			// The connection was closed and a reconnect is performed
		Error,			// The connection could not be open or an error has occurred
		Down			// The connection is down. A connection can no longer be established
	};

	class TCPClient
	: public TCPBase
	{
	  public:
		using StateCallback = std::function<void (TCPClientState)>;

		TCPClient(
			asio::io_context& ctx
		);
		TCPClient(
			void
		);
		virtual ~TCPClient(
			void
		);

		void connect(
			asio::ip::tcp::endpoint target,
			StateCallback stateCallback,
			uint32_t reconnectTimeout = 0
		);
		void disconnect(void);
		void send(ModbusProt::ModbusPDU::SPtr& req, ModbusProt::ResponseCallback responseCallback);

	  private:
		std::mutex mutex_;
		bool clientLoopReady_ = false;
		std::shared_ptr<asio::steady_timer> timer_ = nullptr;
		TCPClientState state_ = TCPClientState::Init;
		std::shared_ptr<asio::ip::tcp::socket> socket_ = nullptr;

		StateCallback stateCallback_;
		Base::Queue sendQueue_;

		void shutdown(StateCallback stateCallback);
		void stopSendQueue(void);
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
