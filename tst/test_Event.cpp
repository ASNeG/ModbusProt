
#include <cpunit>
#include <thread>
#include <iostream>

#include "Base/Event.h"
#include "ModbusTCP/QueueEvent.h"

namespace TestEvent
{
	using namespace cpunit;

	bool eventReceived_ = false;
	ModbusTCP::QueueEvent receiver(Base::Event& event)
	{
		std::cout << "receiver start" << std::endl;
		co_await event;
		std::cout << "Got the notification! " << std::endl;
		eventReceived_ = true;
		std::cout << "receiver end" << std::endl;

		// QueueEvent::return_void
		// QueueEvent::final_suspend
	}

    CPUNIT_TEST(TestEvent, notify_event)
	{
    	std::cout << "notify - event" << std::endl;

    	Base::Event event;
    	auto sendThread = std::thread([&event]{ event.notify(); });
    	sleep(1);
    	auto recvThread = std::thread(receiver, std::ref(event));
    	sleep(1);

    	CPUNIT_ASSERT(eventReceived_ == true);

    	sendThread.join();
    	recvThread.join();
	}

    CPUNIT_TEST(TestEvent, event_notify)
	{
    	std::cout << "event-notify" << std::endl;

    	Base::Event event;
    	auto recvThread = std::thread(receiver, std::ref(event));
    	sleep(1);
    	auto sendThread = std::thread([&event]{ event.notify(); });
    	sleep(1);

    	CPUNIT_ASSERT(eventReceived_ == true);

    	sendThread.join();
    	recvThread.join();
	}

}
