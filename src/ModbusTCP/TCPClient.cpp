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
		socket_ = std::make_shared<asio::ip::tcp::socket>(TCPBase::ctx());
	}

	TCPClient::TCPClient(
		void
	)
	: TCPBase()
	, channel_(ctx(), 10)
	{
		socket_ = std::make_shared<asio::ip::tcp::socket>(ctx());
	}

	TCPClient::~TCPClient(
		void
	)
	{
		// Delete socket
		socket_ = nullptr;
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

	asio::awaitable<void>
	TCPClient::timeout(std::chrono::steady_clock::duration duration)
	{
		asio::steady_timer timer(co_await asio::this_coro::executor);
		timer.expires_after(duration);
		co_await timer.async_wait(use_nothrow_awaitable);
	}

	void
	TCPClient::stopSendQueue(void)
	{
		;
	}

	asio::awaitable<bool>
	TCPClient::connectToServer(
		asio::ip::tcp::endpoint targetEndpoint
	)
	{
		std::cout << "TCPClient::connectToServer" << std::endl;

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
				error = true;
			}
			else {
				auto [e] = std::get<0>(result);
				if (e) {
					// Connect error
					error = true;
				}
			}

			if (error) {
				if (reconnectTimeout_ == 0) {
					// Set close state
					setState(TCPClientState::Close);
					co_return false;
				}

				// Start reconnect timer
				setState(TCPClientState::WaitForReconnect);
				co_await timeout(std::chrono::milliseconds(reconnectTimeout_));
				continue;
			}
			break;
		}

		// Set connected state
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
			error = true;
		}
		else {
			auto [e, n] = std::get<0>(result);
			if (e) {
				// Send error
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
			error = true;
		}
		else {
			auto [e, n] = std::get<0>(result);
			*recvBufferLen = n;
			if (e) {
				// Send error
				error = true;
			}
		}

		if (error) {
			setState(TCPClientState::Close);
			co_return false;
		}

		co_return true;
	}

	void
	TCPClient::shutdown(StateCallback stateCallback)
	{
#if 0
		mutex_.lock();
		clientLoopReady_ = false;
		mutex_.unlock();

		// Close socket
		if (tcpClientState_== TCPClientState::Connected) {
			socket_->close();
			tcpClientState_= TCPClientState::Close;
			stateCallback(tcpClientState_);
		}

		// Cleanup send queue
		Base::QueueElement::SPtr queueElement = nullptr;
		while ((queueElement = sendQueue_.getAndRemoveFirst()) != nullptr) {
			auto qe = std::dynamic_pointer_cast<ModbusTCPQueueElement>(queueElement);
			qe->responseCallback_(ModbusProt::ModbusError::ConnectionError, qe->req_, qe->res_);
		}
#endif

		// Set down state
		setState(TCPClientState::Down);
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
		auto address = queueElement->address_;
		auto req = queueElement->req_;

		// Encode data packet
		ModbusTCP modbusTCPReq;
		modbusTCPReq.modbusPDU(req);
		modbusTCPReq.transactionIdentifier(transactionIdentifier);
		modbusTCPReq.unitIdentifier(address);

		ss.rdbuf()->pubsetbuf(sendBuffer.data(), sendBuffer.size());
		bool rc = modbusTCPReq.encode(ss);
		if (!rc) {
			return false;
		}
		*sendBufferLen = ss.rdbuf()->in_avail();
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

		ss.rdbuf()->pubsetbuf(recvBuffer.data(), recvBuffer.size());
		bool rc = modbusTCPRes.decode(ss);
		if (!rc) {
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

		while(true) {
			// Connect to server
			auto rc = co_await connectToServer(targetEndpoint);
			if (!rc) break;

			uint16_t tid = 1;
			while (true) {

				// Receive element from client channel
				auto [e, qe] = co_await channel_.async_receive(use_nothrow_awaitable);
				if (e) {
					// Set close state
					setState(TCPClientState::Close);
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
				if (rc) {
					qe->responseCallback_(ModbusProt::ModbusError::ConnectionError, qe->req_, qe->res_);
					if (reconnectTimeout_ == 0) co_return;
					continue;
				}

				// Receive modbus pdu from server
				std::array<char, 512> recvBuffer;
				uint32_t recvBufferLen = 512;
				rc = co_await recvFromServer(recvBuffer, &recvBufferLen);
				if (rc) {
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

	void
	TCPClient::disconnect(void)
	{
		std::cout << "TCPClient::disconnect" << std::endl;

		socket_->close();
		channel_.cancel();
	}

	void
	TCPClient::send(
		uint8_t address,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ResponseCallback responseCallback
	)
	{
		mutex_.lock();
		if (!clientLoopReady_) {
			// TCP client not ready
			mutex_.unlock();

			ModbusProt::ModbusPDU::SPtr res = nullptr;
			responseCallback(ModbusProt::ModbusError::ConnectionError, req, res);
			return;
		}

		// Add new packet to queue
		auto qe = std::make_shared<ModbusTCPQueueElement>();
		qe->address_ = address;
		qe->req_ = req;
		qe->res_ = nullptr;
		qe->responseCallback_ = responseCallback;
		if (!channel_.try_send(std::error_code{}, qe)) {
			// TCP client not ready
			mutex_.unlock();

			ModbusProt::ModbusPDU::SPtr res = nullptr;
			responseCallback(ModbusProt::ModbusError::ConnectionError, req, res);
			return;
		}

		mutex_.unlock();
	}

}
