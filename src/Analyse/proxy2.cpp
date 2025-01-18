
//
// Using callbacks in C++ 11
//

#include <exception>
#include <iostream>

#include "asio.hpp"


class Proxy
: public std::enable_shared_from_this<Proxy>
{
  public:
	Proxy(asio::ip::tcp::socket client)
	: client_(std::move(client))
	, server_(client_.get_executor())
	{
	}

	void connectoToServer(asio::ip::tcp::endpoint target)
	{
		auto self = shared_from_this();
		server_.async_connect(
			target,
			[self](std::error_code error) {
				if (!error) {
					self->readFromClient();
					self->readFromServer();
				}
			}
		);
	}

  private:
	void stop()
	{
		client_.close();
		server_.close();
	}

	void readFromClient()
	{
		auto self = shared_from_this();
		client_.async_read_some(
			asio::buffer(dataFromClient_),
			[self](std::error_code error, std::size_t n) {
				if (!error) {
					self->writeToServer(n);
				}
				else {
					self->stop();
				}
			}
		);
	}

	void writeToServer(std::size_t n)
	{
		auto self = shared_from_this();
		async_write(
			server_,
			asio::buffer(dataFromClient_, n),
			[self](std::error_code error, std::size_t n) {
				if (!error) {
					self->readFromClient();
				}
				else {
					self->stop();
				}
			}
		);
	}

	void readFromServer()
	{
		auto self = shared_from_this();
		server_.async_read_some(
			asio::buffer(dataFromServer_),
			[self](std::error_code error, std::size_t n) {
				if (!error) {
					self->writeToClient(n);
				}
				else {
					self->stop();
				}
			}
		);
	}

	void writeToClient(std::size_t n)
	{
		auto self = shared_from_this();
		async_write(
			client_,
			asio::buffer(dataFromServer_, n),
			[self](std::error_code error, std::size_t n) {
				if (!error) {
					self->readFromServer();
				}
				else {
					self->stop();
				}
			}
		);
	}

	asio::ip::tcp::socket client_;
	asio::ip::tcp::socket server_;
	std::array<char, 1024> dataFromClient_;
	std::array<char, 1024> dataFromServer_;

};

void listen(asio::ip::tcp::acceptor& acceptor, asio::ip::tcp::endpoint targetEndpoint)
{
	acceptor.async_accept(
		[&acceptor, targetEndpoint](std::error_code error, asio::ip::tcp::socket client) {
			if (!error) {
				std::make_shared<Proxy>(std::move(client))->connectoToServer(targetEndpoint);
			}

			listen(acceptor, targetEndpoint);
		}
	);
}


int main(int argc, char* argv[])
{
	try {

		if (argc != 5) {
			std::cerr << "Usage: proxy";
			std::cerr << " <listen_address> <listen_port>";
			std::cerr << " <target_address> <target_port>\n";
			return 1;
		}

		asio::io_context ctx;

		auto listenEndpoint =
			*asio::ip::tcp::resolver(ctx).resolve(
				argv[1],
				argv[2],
				asio::ip::tcp::resolver::passive
			);

		auto targetEndpoint =
			*asio::ip::tcp::resolver(ctx).resolve(
				argv[3],
				argv[4]
			);

		asio::ip::tcp::acceptor acceptor(ctx, listenEndpoint);

		listen(acceptor, targetEndpoint);

		ctx.run();

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}
