
#include <iostream>
#include <coroutine>
#include "CoroTask.h"

CoroTask coro1(const int max)
{
    std::cout << "coro begin " << max << std::endl;

    for (int val = 0; val <= max; val++) {
    	std::cout << "coro " << val << "/" << max << std::endl;
    	co_await std::suspend_always{};
    }

    std::cout << "coro end" << std::endl;
}

CoroTask coro2(const int max)
{
    std::cout << "coro begin " << max << std::endl;

    for (int val = 0; val <= max; val++) {
    	std::cout << "coro " << val << "/" << max << std::endl;
    	co_yield val;
    }

    std::cout << "coro end" << std::endl;
}

CoroTask coro3(const int max)
{
    std::cout << "coro begin " << max << std::endl;

    for (int val = 0; val <= max; val++) {
    	std::cout << "coro " << val << "/" << max << std::endl;
    	co_yield val;
    }

    std::cout << "coro end" << std::endl;
    co_return 4711;
}

void test1(void)
{
	auto coroTask = coro1(3);

	while (coroTask.resume());
}

void test2(void)
{
	auto coroTask = coro2(3);

	while (coroTask.resume()) {
		auto val = coroTask.getValue();
		std::cout << "resume-val " << val << std::endl;
	}
}

void test3(void)
{
	auto coroTask = coro2(3);

	for (const auto& val : coroTask) {
		std::cout << "resume-val " << val << std::endl;
	}
}

void test4(void)
{
	auto coroTask = coro3(3);

	for (const auto& val : coroTask) {
		std::cout << "resume-val " << val << std::endl;
	}

	std::cout << "Result=" << coroTask.getResult() << std::endl;
}


int main(int argc, char** argv)
{
	std::cout << std::endl << "TEST1:" << std::endl;
	test1();

	std::cout << std::endl << "TEST2:" << std::endl;
	test2();

	std::cout << std::endl << "TEST3:" << std::endl;
	test3();

	std::cout << std::endl << "TEST4:" << std::endl;
	test4();

	return 0;
}
