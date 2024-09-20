
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
		ready_ = true;
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
			while (!ready_) {
				if (cv_.wait_for(lk, std::chrono::milliseconds(timeoutMs)) == std::cv_status::timeout) {
					return false;
				}
			}
		}

        return true;
	}

}
