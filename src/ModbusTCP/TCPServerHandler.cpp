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

#include <string>

#include <asio/experimental/as_tuple.hpp>
#include <asio/experimental/awaitable_operators.hpp>

#include "ModbusProt/ErrorPDU.h"
#include "ModbusTCP/TCPServerHandler.h"

namespace ModbusTCP
{

	using namespace asio::experimental::awaitable_operators;

	constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);

	TCPServerHandler::TCPServerHandler(
		asio::io_context& ctx
	)
	: TCPBase(ctx)
	{
	}

	TCPServerHandler::TCPServerHandler(
		void
	)
	: TCPBase()
	{
	}

	TCPServerHandler::~TCPServerHandler(
		void
	)
	{
	}

	std::string
	TCPServerHandler::tcpServerStateToString(TCPServerState tcpServerState)
	{
		switch (tcpServerState)
		{
			case TCPServerState::Init: return "Init";
			case TCPServerState::Connected: return "Connected";
			case TCPServerState::Down: return "Down";
			default: return "Unknown";
		}
	}

	void
	TCPServerHandler::recvTimeout(uint32_t recvTimeout)
	{
		recvTimeout_ = recvTimeout;
	}

	void
	TCPServerHandler::sendTimeout(uint32_t sendTimeout)
	{
		sendTimeout_ = sendTimeout;
	}

	asio::awaitable<void>
	TCPServerHandler::close(void)
	{
		if (socket_ != nullptr) {
			socket_->cancel();
		}
		co_return;
	}

	asio::awaitable<bool>
	TCPServerHandler::timeout(
		std::chrono::steady_clock::duration duration
	)
	{
		timer_->expires_after(duration);
		auto [e] = co_await timer_->async_wait(use_nothrow_awaitable);
		if (e) {
			co_return false;
		}

		co_return true;
	}

	asio::awaitable<bool>
	TCPServerHandler::recvFromClient(
		std::array<char, 512>& recvBuffer,
		uint32_t* recvBufferLen
	)
	{
		// Receive modbus pdu from server
		auto result = co_await(
			socket_->async_read_some(asio::buffer(recvBuffer), use_nothrow_awaitable) ||
			timeout(std::chrono::milliseconds(recvTimeout_))
		);

		bool error = false;
		if (result.index() == 1) {
			// timed out
			logHandler_->logList(Base::LogLevel::Error, {"receive from client timeout"});
			error = true;
		}
		else {
			auto [e, n] = std::get<0>(result);
			*recvBufferLen = n;
			if (e) {
				// Send error
				logHandler_->logList(Base::LogLevel::Error, {"receive from client error:", e.message()});
				error = true;
			}
		}

		if (error) {
			co_return false;
		}

		co_return true;
	}

	asio::awaitable<bool>
	TCPServerHandler::sendToClient(
		std::array<char, 512>& sendBuffer,
		uint32_t sendBufferLen
	)
	{
		// Send modbus pdu to server
		auto result = co_await(
			async_write(*socket_, asio::buffer(sendBuffer, sendBufferLen), use_nothrow_awaitable) ||
			timeout(std::chrono::milliseconds(sendTimeout_))
		);

		bool error = false;
		if (result.index() == 1) {
			// timed out
			logHandler_->logList(Base::LogLevel::Error, {"send to client timeout"});
			error = true;
		}
		else {
			auto [e, n] = std::get<0>(result);
			if (e) {
				// Send error
				logHandler_->logList(Base::LogLevel::Error, {"send to client error:", e.message()});
				error = true;
			}
		}

		if (error) {
			co_return false;
		}

		co_return true;
	}

	bool
	TCPServerHandler::decode(
		std::array<char, 512>& recvBuffer,
		uint32_t recvBufferLen,
		ModbusTCP& modbusTCPReq
	)
	{
		// Decode data packet
		std::stringstream ss;

		modbusTCPReq.pduType(ModbusProt::PDUType::Request);
		ss.rdbuf()->pubsetbuf(recvBuffer.data(), recvBufferLen);
		bool rc = modbusTCPReq.decode(ss);
		if (!rc) {
			std::string pduType = "PDUType null";
			if (modbusTCPReq.modbusPDU() != nullptr) modbusTCPReq.modbusPDU()->pduToString();
			logHandler_->logList(Base::LogLevel::Error, {
				"server decode error:", pduType
			});
			return false;
		}

		return true;
	}

	bool
	TCPServerHandler::encode(
		ModbusTCP& modbusTCPRes,
		std::array<char, 512>& sendBuffer,
		uint32_t* sendBufferLen
	)
	{
		std::stringstream ss;
		*sendBufferLen = 0;

		// Encode data packet
		ss.rdbuf()->pubsetbuf(sendBuffer.data(), sendBuffer.size());
		bool rc = modbusTCPRes.encode(ss);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"server encode error"});
			return false;
		}
		*sendBufferLen = ss.tellp();
		return true;
	}

	ModbusProt::ErrorPDU::SPtr
	TCPServerHandler::createErrorPDU(
		ModbusProt::PDUFunction pduFunction,
		ModbusProt::ErrorPDU::ExceptionCode ec
	)
	{
		logHandler_->logList(Base::LogLevel::Error, {
			"create error pdu for function",
			pduFunctionToString(pduFunction)
		});
		auto errorPDU = std::make_shared<ModbusProt::ErrorPDU>(pduFunction);
		errorPDU->exceptionCode(ec);
		return errorPDU;
	}

	bool
	TCPServerHandler::handleModbusReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
		return true;
	}

	void
	TCPServerHandler::stateCallback(StateCallback stateCallback)
	{
		stateCallback_ = stateCallback;
		stateCallback_(state_);
	}

	asio::awaitable<void>
	TCPServerHandler::open(asio::ip::tcp::socket socket)
	{
		bool rc = true;

		// Create socket and timer
		socket_ = std::make_shared<asio::ip::tcp::socket>(std::move(socket));
		timer_ = std::make_shared<asio::steady_timer>(co_await asio::this_coro::executor);

		state_ = TCPServerState::Connected;
		if (stateCallback_) stateCallback_(state_);

		for (;;) {

			// Receive pdu packet from client
			std::array<char, 512> recvBuffer;
			uint32_t recvBufferLen = 512;
			rc = co_await recvFromClient(recvBuffer, &recvBufferLen);
			if (!rc) {
				// Close connection
				break;
			}

			// Decode received pdu packet
			ModbusTCP modbusTCP;
			rc = decode(recvBuffer, recvBufferLen, modbusTCP);
			if (!rc) {
				// Close connection
				break;
			}

			// Call application handler
			ModbusProt::ModbusPDU::SPtr res;
			rc = handleModbusReq(modbusTCP.unitIdentifier(), modbusTCP.modbusPDU(), res);
			if (!rc) {
				// Close connection
				break;
			}
			modbusTCP.modbusPDU(res);

			// Encode send pdu packet
			std::array<char, 512> sendBuffer;
			uint32_t sendBufferLen = 512;
			rc = encode(modbusTCP, sendBuffer, &sendBufferLen);
			if (!rc) {
				// Close connection
				break;
			}

			// Send pdu packet to client
			rc = co_await sendToClient(sendBuffer, sendBufferLen);
			if (!rc) {
				// Close connection
				break;
			}
		}

		// Cleanup timer and socket
		timer_ = nullptr;
		socket_ = nullptr;

		state_ = TCPServerState::Down;
		if (stateCallback_) stateCallback_(state_);
	}

	void
	TCPServerHandler::disconnect(void)
	{
		std::cout << "TCPServerHandler::disconnect" << std::endl;
		co_spawn(ctx(), close(), asio::detached);
	}

}
