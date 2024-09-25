#pragma once

#include <array>
#include <memory>
#include <iostream>
#include <asio.hpp>
#include <asio/experimental/as_tuple.hpp>
#include <asio/experimental/awaitable_operators.hpp>

namespace tcp_proxy {

	class TCPProxy
	: public std::enable_shared_from_this<TCPProxy>
	{
	  public:
		using SPtr = std::shared_ptr<TCPProxy>;

		TCPProxy(asio::ip::tcp::socket client, asio::ip::tcp::endpoint targetEndpoint);
		~TCPProxy(void);

		void close(void);
		asio::awaitable<void> connectToServer(void);
		asio::awaitable<void> clientToServer(void);
		asio::awaitable<void> serverToClient(void);
		asio::awaitable<void> watchdog(void);

	  private:
		asio::ip::tcp::endpoint targetEndpoint_;
		asio::ip::tcp::socket client_;
		asio::ip::tcp::socket server_{client_.get_executor()};
		std::chrono::steady_clock::time_point deadline_;
	};


	class TCPServer
	{
	  public:
		TCPServer(asio::io_context& ctx, asio::ip::tcp::endpoint listenEndpoint, asio::ip::tcp::endpoint targetEndpoint);
		~TCPServer(void);

		asio::awaitable<void> listen(void);
		asio::awaitable<void> proxy(asio::ip::tcp::socket client, asio::ip::tcp::endpoint targetEndpoint);

	  private:
		asio::ip::tcp::endpoint targetEndpoint_;
		asio::ip::tcp::acceptor acceptor_;
	};

}
