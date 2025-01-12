
#include <cpunit>
#include <thread>
#include <iostream>

#include "Base/Event.h"
#include "Base/QueueEvent.h"

namespace TestEvent
{
	using namespace cpunit;

	bool eventReceived_ = false;
	Base::QueueEvent receiver(Base::Event& event)
	{
		std::cout << "wait event" << std::endl;
		co_await event;
		std::cout << "wait event ready" << std::endl;
		eventReceived_ = true;

		// QueueEvent::return_void
		// QueueEvent::final_suspend
	}

    CPUNIT_TEST(TestEvent, notify_event)
	{
    	std::cout << "notify - event" << std::endl;

    	Base::Event event;
    	auto sendThread = std::thread([&event]{ event.notify(); });
    	std::cout << "sleep" << std::endl;
    	sleep(1);
    	auto recvThread = std::thread(receiver, std::ref(event));
    	std::cout << "sleep" << std::endl;
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
    	std::cout << "sleep" << std::endl;
    	sleep(1);
    	auto sendThread = std::thread([&event]{ event.notify(); });
    	std::cout << "sleep" << std::endl;
    	sleep(1);

    	CPUNIT_ASSERT(eventReceived_ == true);

    	sendThread.join();
    	recvThread.join();
	}

}
