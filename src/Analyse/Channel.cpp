#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <syncstream>
#include <asio.hpp>
#include <asio/experimental/concurrent_channel.hpp>

using Channel = asio::experimental::concurrent_channel<void(asio::error_code i, std::string)>;

void sendMessage(Channel& channel, const std::string& message)
{
	bool ok = channel.try_send(asio::error_code{}, message);
}

void receiveMessage(Channel& channel)
{
	channel.async_receive(
	    [&channel](asio::error_code i, std::string message)
	    {
			std::cout << "Recv Message: " << message << std::endl;
	    }
	);
}

int main() {

}
