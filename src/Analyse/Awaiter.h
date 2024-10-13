#pragma once

#include <iostream>

class Awaiter
{ 
  public:
	bool await_ready(void) const noexcept
	{
		std::cout << "await_ready" << std::endl;
		return false;			// do suspend
	}

	void await_suspend(auto hdl) const noexcept
	{
		std::cout << "await_suspend" << std::endl;
	}

	void await_resume(void) const noexcept
	{
		std::cout << "await_resume" << std::endl;
	}
};

