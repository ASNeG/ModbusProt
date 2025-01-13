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

#ifndef __ModbusTCP_TCPServerHandler_h__
#define __ModbusTCP_TCPServerHandler_h__

#include <memory>
#include <asio.hpp>

#include "ModbusTCP/ModbusTCP.h"

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

		void recvTimeout(uint32_t recvTimeout);
		void sendTimeout(uint32_t sendTimeout);

		asio::awaitable<void> open(asio::ip::tcp::socket socket);
		void close(void);
		virtual bool handleModbusReq(
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		);

	  private:
		uint32_t recvTimeout_ = 5000;
		uint32_t sendTimeout_ = 5000;

		std::shared_ptr<asio::ip::tcp::socket> socket_ = nullptr;
		std::shared_ptr<asio::steady_timer> timer_ = nullptr;

		void closeConnection(void);
		asio::awaitable<bool> timeout(
			std::chrono::steady_clock::duration duration
		);
		asio::awaitable<bool> recvFromClient(
			std::array<char, 512>& recvBuffer,
			uint32_t* recvBufferLen
		);
		asio::awaitable<bool> sendToClient(
			std::array<char, 512>& recvBuffer,
			uint32_t recvBufferLen
		);
		bool decode(
			std::array<char, 512>& recvBuffer,
			uint32_t recvBufferLen,
			ModbusTCP& modbusTCPReq
		);
		bool encode(
			ModbusTCP& modbusTCPRes,
			std::array<char, 512>& sendBuffer,
			uint32_t* sendBufferLen
		);
	};

}

#endif
