/*
   Copyright 2024 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Kai Huebl (kai@huebl-sgh.de)
 */

#include "ModbusTCP/Event.h"

namespace ModbusTCP
{

	Event::Awaiter::Awaiter(
		const Event& event
	)
	: event_(event)
	{
	}

	bool
	Event::Awaiter::await_ready(void) const
	{
		// Allow at most one waiter
		if (event_.suspendedWaiter_.load() != nullptr) {
			throw std::runtime_error("More than one waiter is not valid");
		}

		// event.notified == false; suspends the coroutine
		// event.notified == true; the coroutine is executed such as a usual function
		return event_.notified_;
	}

	bool
	Event::Awaiter::await_suspend(
		std::coroutine_handle<void> coroHandle
	) noexcept
	{
		coroHandle_ = coroHandle;

		if (event_.notified_ == true) {
			return false;
		}

		// store the waiter for later notification
		event_.suspendedWaiter_.store(this);

		return true;
	}

	void Event::Awaiter::await_resume(
		void
	) noexcept
	{
	}

	void
	Event::notify(void) noexcept
	{
		notified_ = true;

		// try to load the waiter
	    auto* waiter = static_cast<Awaiter*>(suspendedWaiter_.load());

	    // check if a waiter is available
	    if (waiter != nullptr) {
	        // resume the coroutine => await_resume
	        waiter->coroHandle_.resume();
	    }
	}

	Event::Awaiter Event::operator co_await(void) const noexcept
	{
		return Event::Awaiter{*this};
	}




}
