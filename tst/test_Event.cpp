
#include <cpunit>
#include <thread>
#include <iostream>

#include "ModbusTCP/Event.h"

namespace TestEvent
{
	using namespace cpunit;
	using namespace ModbusTCP;

	bool eventReceived_ = false;
	EventTask receiver(Event& event)
	{
		co_await event;
		std::cout << "Got the notification! " << std::endl;
		eventReceived_ = true;
	}

    CPUNIT_TEST(TestEvent, notify_event)
	{
    	std::cout << "notify - event" << std::endl;

    	Event event;
    	auto sendThread = std::thread([&event]{ event.notify(); });
    	auto recvThread = std::thread(receiver, std::ref(event));

    	std::cout << "SLEEP START" << std::endl;
    	sleep(1);
    	std::cout << "SLEEP END" << std::endl;

    	CPUNIT_ASSERT(eventReceived_ == true);

    	sendThread.join();
    	recvThread.join();
	}

    CPUNIT_TEST(TestEvent, event_notify)
	{
    	std::cout << "event-notify" << std::endl;

    	Event event;
    	auto recvThread = std::thread(receiver, std::ref(event));

    	std::cout << "SLEEP START" << std::endl;
    	auto sendThread = std::thread([&event]{ event.notify(); });
    	std::cout << "SLEEP END" << std::endl;
    	sleep(1);

    	CPUNIT_ASSERT(eventReceived_ == true);

    	sendThread.join();
    	recvThread.join();
	}


}
