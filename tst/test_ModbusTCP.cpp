
#include <cpunit>

#include "ModbusTCP/ModbusTCPClient.h"
#include "ModbusTCP/ModbusTCPServer.h"

namespace TestModbusTCP
{

	using namespace cpunit;
	using namespace ModbusTCP;

    CPUNIT_TEST(TestModbusTCP, client_not_connect)
	{
    	// Create client object and start loop
    	asio::io_context ctx;
    	ModbusTCPClient client(ctx);


    }

}
