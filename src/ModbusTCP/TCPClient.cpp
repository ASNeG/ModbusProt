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

#include <asio/experimental/as_tuple.hpp>
#include <asio/experimental/awaitable_operators.hpp>

#include "ModbusTCP/ModbusTCPQueueElement.h"
#include "ModbusTCP/TCPClient.h"

namespace ModbusTCP
{

	using namespace asio::experimental::awaitable_operators;

	constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);

	TCPClient::TCPClient(
		asio::io_context& ctx
	)
	: TCPBase(ctx)
	{
		socket_ = std::make_shared<asio::ip::tcp::socket>(TCPBase::ctx());
		createTimer();
	}

	TCPClient::TCPClient(
		void
	)
	: TCPBase()
	{
		socket_ = std::make_shared<asio::ip::tcp::socket>(ctx());
		createTimer();
	}

	TCPClient::~TCPClient(
		void
	)
	{
		// Stop Timer
		stopTimer();
		destroyTimer();

		// Delete socket
		socket_ = nullptr;
	}

	void
	TCPClient::createTimer(void)
	{
		timer_ = std::make_shared<asio::steady_timer>(ctx());
	}

	void
	TCPClient::destroyTimer(void)
	{
		timer_ = nullptr;
	}

	asio::awaitable<bool>
	TCPClient::startTimer(uint32_t timeoutMs)
	{
		timer_->expires_after(std::chrono::milliseconds(timeoutMs));
		auto [e] = co_await timer_->async_wait(use_nothrow_awaitable);
		if (e) {
			state_ = TCPClientState::Error;
			stateCallback_(state_);
			co_return false;
		}
		co_return true;
	}

	void
	TCPClient::stopTimer(void)
	{
		if (timer_ != nullptr) {
			timer_->cancel();
		}
	}

	void
	TCPClient::stopSendQueue(void)
	{
		;
	}

	asio::awaitable<bool>
	TCPClient::connectToServer(
		asio::ip::tcp::endpoint targetEndpoint,
		StateCallback stateCallback,
		uint32_t reconnectTimeout
	)
	{
		std::cout << "TCPClient::connectToServer" << std::endl;

		// Set connecting state
		state_ = TCPClientState::Connecting;
		stateCallback(state_);

		// Connect to server
		auto [e] = co_await socket_->async_connect(targetEndpoint, use_nothrow_awaitable);
		if (e) {
			// Set close state
			state_ = TCPClientState::Close;
			stateCallback(state_);
			co_return false;
		}

		// Set connected state
		state_ = TCPClientState::Connected;
		stateCallback(state_);
		co_return true;
	}

	void
	TCPClient::shutdown(StateCallback stateCallback)
	{
		mutex_.lock();
		clientLoopReady_ = false;
		mutex_.unlock();

		// Close socket
		if (state_ == TCPClientState::Connected) {
			socket_->close();
			state_ = TCPClientState::Close;
			stateCallback(state_);
		}

		// Cleanup send queue
		Base::QueueElement::SPtr queueElement = nullptr;
		while ((queueElement = sendQueue_.getAndRemoveFirst()) != nullptr) {
			auto qe = std::dynamic_pointer_cast<ModbusTCPQueueElement>(queueElement);
			qe->responseCallback_(ModbusProt::ModbusError::ConnectionError, qe->req_, qe->res_);
		}

		// Set down state
		state_ = TCPClientState::Down;
		stateCallback(state_);
	}

	asio::awaitable<void>
	TCPClient::clientLoop(
		asio::ip::tcp::endpoint targetEndpoint,
		StateCallback stateCallback,
		uint32_t reconnectTimeout
	)
	{
		std::cout << "TCPClient::clientLoop" << std::endl;
		clientLoopReady_ = true;
		stateCallback_ = stateCallback;

		while(clientLoopReady_) {
			// Connect to server
			auto rc = co_await connectToServer(
				targetEndpoint,
				stateCallback,
				reconnectTimeout

			);
			if (!rc) {
				// Check end condition
				if (!clientLoopReady_ || reconnectTimeout == 0) {
					shutdown(stateCallback);
					co_return;
				}

				// Start reconnect timer
				auto rc = co_await startTimer(reconnectTimeout);
				if (!rc) {
					shutdown(stateCallback);
					co_return;
				}
				continue;
			}

			bool recvLoopReady = true;
			uint16_t transactionIdentifier = 1;
			while (recvLoopReady) {
				// Read data from queue
				auto [resultCode, queueElement] = sendQueue_.recv();
				if (!resultCode) {
					// Only a disconnect call should cause this error
					shutdown(stateCallback);
					co_return;
				}

				// Encode data packet
				auto qe = std::dynamic_pointer_cast<ModbusTCPQueueElement>(queueElement);
				auto address = qe->address_;
				auto req = qe->req_;
				auto res = qe->res_;
				auto responseCallback = qe->responseCallback_;

				std::array<char, 512> sendBuffer;
				std::stringstream ss1;
				ss1.rdbuf()->pubsetbuf(sendBuffer.data(), sendBuffer.size());

				ModbusTCP modbusTCPReq;
				modbusTCPReq.modbusPDU(req);
				modbusTCPReq.transactionIdentifier(transactionIdentifier);
				modbusTCPReq.unitIdentifier(address);

				if (!modbusTCPReq.encode(ss1)) {
					responseCallback(ModbusProt::ModbusError::EncodeDataError, req, res);
					continue;
				}

				// Send modbus pdu to tcp server
				auto [e1, n1] = co_await async_write(*socket_, asio::buffer(sendBuffer, ss1.rdbuf()->in_avail()), use_nothrow_awaitable);
				if (e1) {
					recvLoopReady = false;

					// Check end condition
					if (!clientLoopReady_ || reconnectTimeout == 0) {
						responseCallback(ModbusProt::ModbusError::ConnectionError, req, res);
						shutdown(stateCallback);
						co_return;
					}
					continue;
				}

				// Receive modbu pdu from tcp server
				// FIXME: Timeout handling is missing
				std::array<char, 512> recvBuffer;
				auto [e2, n2] = co_await socket_->async_read_some(asio::buffer(recvBuffer), use_nothrow_awaitable);
				if (e2) {
					recvLoopReady = false;

					// Check end condition
					if (!clientLoopReady_ || reconnectTimeout == 0) {
						responseCallback(ModbusProt::ModbusError::ConnectionError, req, res);
						shutdown(stateCallback);
						co_return;
					}
					continue;
				}

				// Decode data packet
				ModbusTCP modbusTCPRes;
				std::stringstream ss2;
				ss2.rdbuf()->pubsetbuf(recvBuffer.data(), recvBuffer.size());
				if (!modbusTCPRes.decode(ss2)) {
					recvLoopReady = false;

					// Check end condition
					if (!clientLoopReady_ || reconnectTimeout == 0) {
						responseCallback(ModbusProt::ModbusError::DecodeDataError, req, res);
						shutdown(stateCallback);
						co_return;
					}
					continue;
				}
				res = modbusTCPRes.modbusPDU();

				responseCallback(ModbusProt::ModbusError::Ok, req, res);
			}

			// Reconnect
			if (clientLoopReady_) {
				// Start reconnect timer
				auto rc = co_await startTimer(reconnectTimeout);
				if (!rc) {
					shutdown(stateCallback);
					co_return;
				}
			}
		}

		co_return;
	}

	void
	TCPClient::connect(
		asio::ip::tcp::endpoint target,
		StateCallback stateCallback,
		uint32_t reconnectTimeout
	)
	{
		std::cout << "TCPClient::connect" << std::endl;

		// Connect to server
		co_spawn(
			ctx(),
			clientLoop(target, stateCallback, reconnectTimeout),
			asio::detached
		);
	}

	void
	TCPClient::disconnect(void)
	{
		std::cout << "TCPClient::disconnect" << std::endl;
		mutex_.lock();
		clientLoopReady_ = false;
		mutex_.unlock();

		// Close send queue
		stopSendQueue();

		// Close socket
		socket_->close();

		// Stop timer
		stopTimer();
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
		Base::QueueElement::SPtr bqe = qe;
		sendQueue_.send(bqe);

		mutex_.unlock();
	}

}
