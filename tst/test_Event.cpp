
#include <cpunit>
#include <thread>
#include <iostream>

#include "ModbusTCP/Event.h"

struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

namespace TestModbusTCP
{
	using namespace cpunit;
	using namespace ModbusTCP;

	bool eventReceived_ = false;
	Task receiver(Event& event)
	{
		co_await event;
		std::cout << "Got the notification! " << std::endl;
		eventReceived_ = true;
	}

    CPUNIT_TEST(TestModbusTCP, notify_event)
	{
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

    CPUNIT_TEST(TestModbusTCP, event_notify)
	{
    	Event event;
    	auto recvThread = std::thread(receiver, std::ref(event));

    	std::cout << "SLEEP START" << std::endl;
    	sleep(1);
    	auto sendThread = std::thread([&event]{ event.notify(); });
    	sleep(1);
    	std::cout << "SLEEP END" << std::endl;

    	CPUNIT_ASSERT(eventReceived_ == true);

    	sendThread.join();
    	recvThread.join();
	}


}
