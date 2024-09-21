
#include <iostream>

#include "Condition.h"

namespace Test
{

	Condition::Condition(void)
	{
	}

	Condition::~Condition(void)
	{
	}

	void
	Condition::init(void)
	{
		ready_ = false;
	}

	void
	Condition::signal(void)
	{
		{
			std::lock_guard lk(m_);
			ready_ = true;
		}
		cv_.notify_one();
	}

	bool
	Condition::wait(uint32_t timeoutMs)
	{
		std::unique_lock lk(m_);
		if (timeoutMs == 0) {
			cv_.wait(lk, [this]{ return ready_; });
		}
		else {
			std::cout << "llll" << std::endl;
			while (!ready_) {
				std::cout << "yy" << std::endl;
				if (cv_.wait_for(lk, std::chrono::milliseconds(timeoutMs)) == std::cv_status::timeout) {
					std::cout << "XXXX" << std::endl;
					return false;
				}
				std::cout << "YYY" << std::endl;
			}
		}

        return true;
	}

}
