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

#ifndef __ModbusTCP_TCPClient_h__
#define __ModbusTCP_TCPClient_h__

#include <functional>
#include <memory>
#include <asio/experimental/channel.hpp>

#include "ModbusProt/ModbusPDU.h"
#include "ModbusProt/ModbusCallback.h"
#include "ModbusTCP/TCPBase.h"
#include "ModbusTCP/ModbusTCPQueueElement.h"

namespace ModbusTCP
{

	enum class TCPClientState
	{
		Init,				// The connection class was created
		Connecting,			// The connection is currently being opened
		Connected,			// The connection is open
		WaitForReconnect,	// Wait for reconnecting
		Close,				// The connection was closed and a reconnect is performed
		Error,				// The connection could not be open or an error has occurred
		Down				// The connection is down. A connection can no longer be established
	};

	class TCPClient
	: public TCPBase
	{
	  public:
		using ClientChannel = asio::experimental::channel<void(asio::error_code, ModbusTCPQueueElement::SPtr)>;
		using StateCallback = std::function<void (TCPClientState)>;

		enum class Result
		{
			Ok,
			SocketError,
			ChannelError
		};

		TCPClient(
			asio::io_context& ctx
		);
		TCPClient(
			void
		);
		virtual ~TCPClient(
			void
		);

		void connectTimeout(uint32_t connectTimeout);
		void reconnectTimeout(uint32_t reconnectTimeout);
		void sendTimeout(uint32_t sendTimeout);
		void recvTimeout(uint32_t recvTimeout);
		std::string tcpClientStateToString(TCPClientState tcpClientState);

		void connect(
			asio::ip::tcp::endpoint target,
			StateCallback stateCallback
		);
		void disconnect(void);
		void send(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ResponseCallback responseCallback
		);

	  private:
		uint32_t connectTimeout_ = 1000; 		/* 1000 milliseconds */
		uint32_t reconnectTimeout_ = 0;			/* 0 milliseconds */
		uint32_t sendTimeout_ = 1000;			/* 1000 milliseconds */
		uint32_t recvTimeout_ = 3000;			/* 3000 milliseconds */

		std::mutex mutex_;
		bool clientLoopReady_ = false;
		bool shutdown_ = false;

		TCPClientState tcpClientState_ = TCPClientState::Init;
		StateCallback stateCallback_ = nullptr;

		std::shared_ptr<asio::ip::tcp::socket> socket_ = nullptr;
		std::shared_ptr<asio::steady_timer> timer_ = nullptr;
		ClientChannel channel_;

		void setState(TCPClientState tcpClientState);

		asio::awaitable<bool> timeout(
			std::chrono::steady_clock::duration duration
		);
		asio::awaitable<bool> connectToServer(
			asio::ip::tcp::endpoint targetEndpoint
		);
		asio::awaitable<bool> sendToServer(
			std::array<char, 512>& sendBuffer,
			uint32_t sendBufferLen
		);
		asio::awaitable<bool> recvFromServer(
			std::array<char, 512>& recvBuffer,
			uint32_t* recvBufferLen
		);
		asio::awaitable<TCPClient::Result> recvFromChannel(
			ModbusTCPQueueElement::SPtr& qe
		);
		asio::awaitable<void> clientLoop(
			asio::ip::tcp::endpoint targetEndpoint
		);
		asio::awaitable<void> addToChannel(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ResponseCallback responseCallback
		);
		asio::awaitable<void> close(void);

		bool encode(
			ModbusTCPQueueElement::SPtr& queueElement,
			uint16_t transactionIdentifier,
			std::array<char, 512>& sendBuffer,
			uint32_t* sendBufferLen
		);
		bool decode(
			ModbusTCPQueueElement::SPtr& queueElement,
			std::array<char, 512>& recvBuffer,
			uint32_t recvBufferLen
		);
	};

}

#endif
