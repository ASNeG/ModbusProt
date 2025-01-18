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

#include <asio/experimental/as_tuple.hpp>
#include <asio/experimental/awaitable_operators.hpp>

#include "ModbusTCP/TCPClient.h"

namespace ModbusTCP
{

	using namespace asio::experimental::awaitable_operators;

	constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);

	TCPClient::TCPClient(
		asio::io_context& ctx
	)
	: TCPBase(ctx)
	, channel_(ctx, 10)
	{
	}

	TCPClient::TCPClient(
		void
	)
	: TCPBase()
	, channel_(ctx(), 10)
	{
	}

	TCPClient::~TCPClient(
		void
	)
	{
		// Delete socket and timer
		socket_ = nullptr;
		timer_ = nullptr;
	}

	void
	TCPClient::connectTimeout(uint32_t connectTimeout)
	{
		connectTimeout_ = connectTimeout;
	}

	void
	TCPClient::reconnectTimeout(uint32_t reconnectTimeout)
	{
		reconnectTimeout_ = reconnectTimeout;
	}

	void
	TCPClient::sendTimeout(uint32_t sendTimeout)
	{
		sendTimeout_ = sendTimeout;
	}

	void
	TCPClient::recvTimeout(uint32_t recvTimeout)
	{
		recvTimeout_ = recvTimeout;
	}

	void
	TCPClient::setState(TCPClientState tcpClientState)
	{



		std::lock_guard<std::mutex> guard(mutex_);
		tcpClientState_= tcpClientState;
		stateCallback_(tcpClientState_);
	}

	std::string
	TCPClient::tcpClientStateToString(TCPClientState tcpClientState)
	{
		switch (tcpClientState)
		{
			case TCPClientState::Init: return "Init";
			case TCPClientState::Connecting: return "Connecting";
			case TCPClientState::Connected: return "Connected";
			case TCPClientState::WaitForReconnect: return "WaitForReconnect";
			case TCPClientState::Close: return "Close";
			case TCPClientState::Error: return "Error";
			case TCPClientState::Down: return "Down";
			default: return "Unknown";
		}
	}

	asio::awaitable<bool>
	TCPClient::timeout(std::chrono::steady_clock::duration duration)
	{
		timer_->expires_after(duration);
		auto [e] = co_await timer_->async_wait(use_nothrow_awaitable);
		if (e) {
			co_return false;
		}

		co_return true;
	}

	asio::awaitable<bool>
	TCPClient::connectToServer(
		asio::ip::tcp::endpoint targetEndpoint
	)
	{
		std::cout << "TCPClient::connectToServer" << std::endl;

		logHandler_->logList(Base::LogLevel::Debug, {
			"connecting to ip", targetEndpoint.address().to_string(),
			"and port", std::to_string(targetEndpoint.port())
		});
		while (true) {
			// Set connecting state
			setState(TCPClientState::Connecting);

			// Connect tcp connection to server
			auto result = co_await(
				socket_->async_connect(targetEndpoint, use_nothrow_awaitable) ||
				timeout(std::chrono::milliseconds(connectTimeout_))
			);

			bool error = false;
			if (result.index() == 1) {
				// timed out
				logHandler_->logList(Base::LogLevel::Error, {
					"connect timeout to ip", targetEndpoint.address().to_string(),
					"and port", std::to_string(targetEndpoint.port())
				});
				error = true;
			}
			else {
				auto [e] = std::get<0>(result);
				if (e) {
					// Connect error
					logHandler_->logList(Base::LogLevel::Error, {
						"connect error to ip", targetEndpoint.address().to_string(),
						"and port", std::to_string(targetEndpoint.port()),
						":", e.message()
					});
					error = true;
				}
			}

			if (error) {
				if (reconnectTimeout_ == 0) {
					// Set close state
					logHandler_->logList(Base::LogLevel::Error, {
						"disconnect to ip", targetEndpoint.address().to_string(),
						"and port", std::to_string(targetEndpoint.port())
					});
					setState(TCPClientState::Close);
					co_return false;
				}

				// Start reconnect timer
				setState(TCPClientState::WaitForReconnect);
				auto rc = co_await timeout(std::chrono::milliseconds(reconnectTimeout_));
				if (!rc) {
					// Set close state
					logHandler_->logList(Base::LogLevel::Error, {
						"reconnect error to ip", targetEndpoint.address().to_string(),
						"and port", std::to_string(targetEndpoint.port())
					});
					setState(TCPClientState::Close);
					co_return false;
				}
				continue;
			}
			break;
		}

		// Set connected state
		logHandler_->logList(Base::LogLevel::Debug, {
			"connected to ip", targetEndpoint.address().to_string(),
			"and port", std::to_string(targetEndpoint.port())
		});
		setState(TCPClientState::Connected);
		co_return true;
	}

	asio::awaitable<bool>
	TCPClient::sendToServer(
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
			logHandler_->logList(Base::LogLevel::Error, {"send timeout"});
			error = true;
		}
		else {
			auto [e, n] = std::get<0>(result);
			if (e) {
				// Send error
				logHandler_->logList(Base::LogLevel::Error, {"send error:", e.message()});
				error = true;
			}
		}

		if (error) {
			setState(TCPClientState::Close);
			co_return false;
		}

		co_return true;
	}

	asio::awaitable<bool>
	TCPClient::recvFromServer(
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
			logHandler_->logList(Base::LogLevel::Error, {"receive timeout"});
			error = true;
		}
		else {
			auto [e, n] = std::get<0>(result);
			*recvBufferLen = n;
			if (e) {
				// Send error
				logHandler_->logList(Base::LogLevel::Error, {"receive error:", e.message()});
				error = true;
			}
		}

		if (error) {
			setState(TCPClientState::Close);
			co_return false;
		}

		co_return true;
	}

	asio::awaitable<bool>
	TCPClient::recvFromChannel(
		ModbusTCPQueueElement::SPtr& qe
	)
	{
		std::array<char, 512> recvBuffer;

		// Receive data from channel and handle tcp connection errors
		auto result = co_await(
			channel_.async_receive(use_nothrow_awaitable) ||
			socket_->async_read_some(asio::buffer(recvBuffer), use_nothrow_awaitable)
		);

		bool error = false;
		if (result.index() == 1) {
			// timed out
			co_return false;
		}
		else {
			auto [e, queueElement] = std::get<0>(result);
			if (e) {
				// Send error
				logHandler_->logList(Base::LogLevel::Error, {"receive channel error:", e.message()});
				co_return false;
			}
			qe = queueElement;
		}

		co_return true;
	}

	bool
	TCPClient::encode(
		ModbusTCPQueueElement::SPtr& queueElement,
		uint16_t transactionIdentifier,
		std::array<char, 512>& sendBuffer,
		uint32_t* sendBufferLen
	)
	{
		std::stringstream ss;
		*sendBufferLen = 0;

		// Get queue data
		auto unitIdentifier = queueElement->unitIdentifier_;
		auto req = queueElement->req_;

		// Encode data packet
		ModbusTCP modbusTCPReq;
		modbusTCPReq.modbusPDU(req);
		modbusTCPReq.transactionIdentifier(transactionIdentifier);
		modbusTCPReq.unitIdentifier(unitIdentifier);

		ss.rdbuf()->pubsetbuf(sendBuffer.data(), sendBuffer.size());
		bool rc = modbusTCPReq.encode(ss);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"encode error"});
			return false;
		}
		*sendBufferLen = ss.tellp();
		return true;
	}

	bool
	TCPClient::decode(
		ModbusTCPQueueElement::SPtr& queueElement,
		std::array<char, 512>& recvBuffer,
		uint32_t recvBufferLen
	)
	{
		// Decode data packet
		ModbusTCP modbusTCPRes;
		std::stringstream ss;

		modbusTCPRes.pduType(ModbusProt::PDUType::Response);
		ss.rdbuf()->pubsetbuf(recvBuffer.data(), recvBuffer.size());
		bool rc = modbusTCPRes.decode(ss);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"decode error"});
			return false;
		}

		// Call result handler
		auto res = modbusTCPRes.modbusPDU();
		queueElement->responseCallback_(ModbusProt::ModbusError::Ok, queueElement->req_, res);
		return true;
	}

	asio::awaitable<void>
	TCPClient::clientLoop(
		asio::ip::tcp::endpoint targetEndpoint
	)
	{
		std::cout << "TCPClient::clientLoop" << std::endl;
		bool rc = true;

		socket_ = std::make_shared<asio::ip::tcp::socket>(co_await asio::this_coro::executor);
		timer_ = std::make_shared<asio::steady_timer>(co_await asio::this_coro::executor);

		while(true) {
			// Connect to server
			auto rc = co_await connectToServer(targetEndpoint);
			if (!rc) break;

			uint16_t tid = 1;
			while (true) {

				// Receive element from client channel
				ModbusTCPQueueElement::SPtr qe;
				rc = co_await recvFromChannel(qe);
				if (!rc) {
					// Set close state
					timer_ = nullptr;
					socket_ = nullptr;
					setState(TCPClientState::Down);
					co_return;
				}

				// Encode modbus data to modbus pdu
				std::array<char, 512> sendBuffer;
				uint32_t sendBufferLen = 0;
				rc = encode(qe, tid, sendBuffer, &sendBufferLen);
				if (!rc) {
					qe->responseCallback_(ModbusProt::ModbusError::EncodeDataError, qe->req_, qe->res_);
					continue;
				}

				// Send modbus pdu to server
				rc = co_await sendToServer(sendBuffer, sendBufferLen);
				if (!rc) {
					qe->responseCallback_(ModbusProt::ModbusError::ConnectionError, qe->req_, qe->res_);
					if (reconnectTimeout_ == 0) co_return;
					continue;
				}

				// Receive modbus pdu from server
				std::array<char, 512> recvBuffer;
				uint32_t recvBufferLen = 512;
				rc = co_await recvFromServer(recvBuffer, &recvBufferLen);
				if (!rc) {
					qe->responseCallback_(ModbusProt::ModbusError::ConnectionError, qe->req_, qe->res_);
					if (reconnectTimeout_ == 0) co_return;
					continue;
				}

				// Decode modbus pdu to modbus data
				rc = decode(qe, recvBuffer, recvBufferLen);
				if (!rc) {
					qe->responseCallback_(ModbusProt::ModbusError::DecodeDataError, qe->req_, qe->res_);
					continue;
				}
			}
		}

		// Remove all elements from queue
		while (true) {
 			bool rc = channel_.try_receive(
				[](asio::error_code e, ModbusTCPQueueElement::SPtr qe) {
 					if (e) return;
					qe->responseCallback_(ModbusProt::ModbusError::ConnectionError, qe->req_, qe->res_);
				}
			);
			if (!rc) break;
		}

		// Cleanup and set down state
		timer_ = nullptr;
		socket_ = nullptr;
		logHandler_->logList(Base::LogLevel::Debug, {"close connection"});
		setState(TCPClientState::Down);
		co_return;
	}

	void
	TCPClient::connect(
		asio::ip::tcp::endpoint target,
		StateCallback stateCallback
	)
	{
		std::cout << "TCPClient::connect" << std::endl;
		stateCallback_ = stateCallback;

		// Connect to server
		co_spawn(ctx(), clientLoop(target), asio::detached);
	}

	asio::awaitable<void>
	TCPClient::close(void)
	{
		// Cancel timer
		if (timer_ != nullptr) timer_->cancel();

		// Close and cancel socket connection to tcp server
		if (socket_ != nullptr) {
			if (socket_->is_open()) socket_->close();
			socket_->cancel();
		}

		// Cancel channel
		channel_.cancel();

		co_return;
	}

	void
	TCPClient::disconnect(void)
	{
		std::cout << "TCPClient::disconnect" << std::endl;
		logHandler_->logList(Base::LogLevel::Debug, {"disconnect"});
		co_spawn(ctx(), close(), asio::detached);
	}

	asio::awaitable<void>
	TCPClient::addToChannel(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ResponseCallback responseCallback
	)
	{
		// Check tcp client state
		if (tcpClientState_ != TCPClientState::Connected) {
			ModbusProt::ModbusPDU::SPtr res = nullptr;
			responseCallback(ModbusProt::ModbusError::ConnectionError, req, res);
			co_return;
		}

		// Add new packet to queue
		auto qe = std::make_shared<ModbusTCPQueueElement>();
		qe->unitIdentifier_ = unitIdentifier;
		qe->req_ = req;
		qe->res_ = nullptr;
		qe->responseCallback_ = responseCallback;

		if (!channel_.try_send(std::error_code{}, qe)) {
			// Channel error
			ModbusProt::ModbusPDU::SPtr res = nullptr;
			responseCallback(ModbusProt::ModbusError::ConnectionError, req, res);
		}

		co_return;
	}

	void
	TCPClient::send(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ResponseCallback responseCallback
	)
	{
		std::cout << "TCPClient::send" << std::endl;
		co_spawn(ctx(), addToChannel(unitIdentifier, req, responseCallback), asio::detached);
	}

}
