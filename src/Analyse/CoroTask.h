#pragma once

#include <exception>
#include <assert.h>

class CoroTask
{
  public:
	struct promise_type {

		int value_ = 0;
		int result_ = 0;

		auto yield_value(int value)
		{
			value_ = value;
			return std::suspend_always{};
		}

		void return_value(const int& value)
		{
			result_ = value;
		}

	    auto get_return_object(void)
	    {
	    	return CoroTask{CoroHdl::from_promise(*this)};
	    }

	    auto initial_suspend(void)
	    {
	    	return std::suspend_always{};
	    }

#if 0
	    void return_void(void)
	    {
	    }
#endif

	    void unhandled_exception(void)
	    {
	    	std::terminate();
	    }

	    auto final_suspend(void) noexcept
	    {
	    	return std::suspend_always{};
	    }
	};

	struct iterator
	{
		std::coroutine_handle<promise_type> hdl_;

		iterator(auto hdl)
		: hdl_(hdl)
		{
		}

		void getNext(void)
		{
			if (hdl_) {
				hdl_.resume();
				if (hdl_.done()) {
					hdl_ = nullptr;
				}
			}
		}

		int operator*(void) const
		{
			assert(hdl_ != nullptr);
			return hdl_.promise().value_;
		}

		iterator operator++(void)
		{
			getNext();
			return *this;
		}

		bool operator==(const iterator& i) const = default;
	};

	using CoroHdl = std::coroutine_handle<promise_type>;
  private:
	CoroHdl hdl_;

  public:
    CoroTask(auto hdl)
    : hdl_(hdl)
    {
    }

    ~CoroTask(void)
    {
    	if (hdl_) {
    		hdl_.destroy();
    	}
    }

    bool resume(void) const
    {
    	if (!hdl_ || hdl_.done()) {
    		return false;
    	}
    	hdl_.resume();
    	return !hdl_.done();
    }

    int getValue(void) const
    {
    	return hdl_.promise().value_;
    }

    int getResult(void) const
    {
    	return hdl_.promise().result_;
    }

    iterator begin(void) const
    {
    	if (!hdl_ || hdl_.done()) {
    		return iterator{nullptr};
    	}
    	iterator itor{hdl_};
    	itor.getNext();
    	return itor;
    }

    iterator end(void) const
    {
    	return iterator{nullptr};
    }
};


