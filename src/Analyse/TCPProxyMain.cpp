#include <stdio.h>
#include <iostream>
#include <asio.hpp>

#include <SRCIServer/TCPProxy.h>

int main(int argc, char** argv)
{
	try {

		std::cout << argc << std::endl;

		if (argc != 5) {
			std::cerr << "usage: SRCIServer";
			std::cerr << " <listen addr> <listen port>";
			std::cerr << " <target_addr> <target_port>" << std::endl;
			exit(1);
		}

		asio::io_context ctx;

		auto listenEndpoint = *asio::ip::tcp::resolver(ctx).resolve(
			argv[1], argv[2]
		);

		auto targetEndpoint = *asio::ip::tcp::resolver(ctx).resolve(
			argv[3], argv[4]
		);

		tcp_proxy::TCPServer tcpServer(ctx, listenEndpoint, targetEndpoint);

		co_spawn(ctx, tcpServer.listen(), asio::detached);
		ctx.run();

		exit(0);

	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}


	return 0;
}
