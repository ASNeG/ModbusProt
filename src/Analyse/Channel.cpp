#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <syncstream>
#include <asio.hpp>
#include <asio/experimental/concurrent_channel.hpp>
#include <asio/experimental/as_tuple.hpp>
#include <asio/experimental/awaitable_operators.hpp>

using Channel = asio::experimental::concurrent_channel<void(asio::error_code i, std::string)>;
using namespace asio::experimental::awaitable_operators;
constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);


asio::awaitable<void>
timeout(std::chrono::steady_clock::duration duration)
{
	asio::steady_timer timer(co_await asio::this_coro::executor);
	timer.expires_after(duration);
	co_await timer.async_wait(use_nothrow_awaitable);
}

asio::awaitable<void>
sendMessage(Channel& channel, asio::steady_timer& timer)
{
	uint32_t count = 0;
	std::string message = "Message";

	while (count < 5) {
		count++;
		std::stringstream ss;
		ss << message << count;

		auto [e1] = co_await channel.async_send(
			asio::error_code{},
			ss.str(),
			use_nothrow_awaitable
		);
		if (e1) {
			std::cout << "async_send error " << e1 << std::endl;
			co_return;
		}

		std::cout << "send message " << ss.str() << std::endl;

		timer.expires_after(std::chrono::milliseconds(1000));
		auto [e2] = co_await timer.async_wait(use_nothrow_awaitable);
		if (e2) {
			std::cout << "async_wait error " << e2 << std::endl;
			co_return;
		}

	}

	std::cout << "send message end..." << std::endl;
	co_return;
}

asio::awaitable<void>
receiveMessage(Channel& channel, asio::steady_timer& timer)
{
	uint32_t count = 0;
	while (true) {
		count++;

		auto result = co_await (
			channel.async_receive(use_nothrow_awaitable) ||
			timeout(std::chrono::milliseconds(5000))
		);

		if (result.index() == 1) { // timed out
			std::cout << "async receive timeout" << std::endl;
			co_return;
		}

		auto [e, r] = std::get<0>(result);
		if (e) {
			std::cout << "async_receive error " << e << std::endl;
			co_return;
		}

		std::cout << "receive message " << r << std::endl;

		if (count == 2) {
			std::cout << "receive message end 1..." << std::endl;
			co_return;
		}
	}

	std::cout << "receive message end 2..." << std::endl;
	co_return;
}

asio::awaitable<void>
run(asio::io_context& ctx)
{
	Channel channel(ctx);
	asio::steady_timer timer(ctx);

	co_await(sendMessage(channel, timer) || receiveMessage(channel, timer));
	std::cout << "await end..." << std::endl;
}

int main()
{
	asio::io_context ctx{1};

	asio::co_spawn(ctx, run(ctx), asio::detached);

    ctx.run();
    std::cout << "end" << std::endl;
}
