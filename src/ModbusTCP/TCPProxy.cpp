#include "SRCIServer/TCPProxy.h"

using namespace std::literals::chrono_literals;
using namespace asio::experimental::awaitable_operators;

namespace tcp_proxy
{

	constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class TCPProxy
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	TCPProxy::TCPProxy(asio::ip::tcp::socket client, asio::ip::tcp::endpoint targetEndpoint)
	: targetEndpoint_(targetEndpoint)
	, client_(std::move(client))
	{
	}

	TCPProxy::~TCPProxy(void)
	{
	}

	void
	TCPProxy::close(void)
	{
		client_.close();
		server_.close();
	}

	asio::awaitable<void>
	TCPProxy::connectToServer(void)
	{
		co_await server_.async_connect(targetEndpoint_, use_nothrow_awaitable);
		co_return;
	}

	asio::awaitable<void>
	TCPProxy::clientToServer(void)
	{
		std::array<char, 512> data;

		for (;;) {
			deadline_ = std::max(deadline_, std::chrono::steady_clock::now() + 5s);

			auto [e1, n1] = co_await client_.async_read_some(asio::buffer(data), use_nothrow_awaitable);
			if (e1) {
				break;
			}

			auto [e2, n2] = co_await async_write(server_, asio::buffer(data, n1), use_nothrow_awaitable);
			if (e2) {
				break;
			}
		}

		co_return;
	}

	asio::awaitable<void>
	TCPProxy::serverToClient(void)
	{
		std::array<char, 512> data;

		for (;;) {
			deadline_ = std::max(deadline_, std::chrono::steady_clock::now() + 5s);

			auto [e1, n1] = co_await server_.async_read_some(asio::buffer(data), use_nothrow_awaitable);
			if (e1) {
				break;
			}

			auto [e2, n2] = co_await async_write(client_, asio::buffer(data, n1), use_nothrow_awaitable);
			if (e2) {
				break;
			}
		}

		co_return;
	}

	asio::awaitable<void>
	TCPProxy::watchdog(void)
	{
		asio::steady_timer timer(client_.get_executor());

		auto now = std::chrono::steady_clock::now();
		while (deadline_ > now) {
			timer.expires_at(deadline_);
			co_await timer.async_wait(use_nothrow_awaitable);
			now = std::chrono::steady_clock::now();
		}

		co_return;
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class TCPServer
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	TCPServer::TCPServer(asio::io_context& ctx, asio::ip::tcp::endpoint listenEndpoint, asio::ip::tcp::endpoint targetEndpoint)
	: targetEndpoint_(targetEndpoint)
	, acceptor_(ctx, listenEndpoint)
	{
	}

	TCPServer::~TCPServer(void)
	{
	}

	asio::awaitable<void>
	TCPServer::listen(void)
	{
		for (;;) {
			auto client = co_await acceptor_.async_accept(asio::use_awaitable);

			auto ex = client.get_executor();
			co_spawn(ex, proxy(std::move(client), targetEndpoint_), asio::detached);
		}

		co_return;
	}

	asio::awaitable<void>
	TCPServer::proxy(asio::ip::tcp::socket client, asio::ip::tcp::endpoint targetEndpoint)
	{
		auto proxy = std::make_shared<TCPProxy>(std::move(client), targetEndpoint);

		co_await proxy->connectToServer();

		co_await(
			proxy->clientToServer() ||
			proxy->serverToClient() ||
			proxy->watchdog()
		);

		proxy->close();

		co_return;
	}

}
