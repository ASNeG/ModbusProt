
#include <cpunit>
#include <thread>
#include <iostream>

#include "ModbusTCP/Queue.h"

namespace TestQueue
{
	using namespace cpunit;
	using namespace ModbusTCP;

    CPUNIT_TEST(TestQueue, send_recv)
	{
    	Queue queue;

    	// Send
    	for (uint32_t idx = 0; idx < 10; idx++) {
    		auto queueElement = std::make_shared<QueueElement>();
    		CPUNIT_ASSERT(queue.send(queueElement) == true);
    	}

    	// Receive
    	for (uint32_t idx = 0; idx < 10; idx++) {
    		queue.recv();
    	}
	}

    CPUNIT_TEST(TestQueue, recv_send)
	{
    	Queue queue;

    	// Receive
    	auto recvThread = std::thread(
    		[&queue]() {
    			for (uint32_t idx = 0; idx < 10; idx++) {
    				queue.recv();
    			}
    		}
    	);

    	// Send
    	for (uint32_t idx = 0; idx < 10; idx++) {
    		auto queueElement = std::make_shared<QueueElement>();
    		CPUNIT_ASSERT(queue.send(queueElement) == true);
    	}

    	recvThread.join();
	}

}
