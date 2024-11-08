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

#ifndef __ModbusProt_Event_h__
#define __ModbusProt_Event_h__

#include <atomic>
#include <coroutine>

namespace Base
{

	class Event
	{
	  public:
		Event(void) = default;

		class Awaiter;
		Awaiter operator co_await(void) const noexcept;

		void notify(void) noexcept;

	  private:

		friend class Awaiter;

		mutable std::atomic<void*> suspendedWaiter_ = nullptr;
		mutable std::atomic<bool> notified_ = false;
	};

	class Event::Awaiter
	{
	  public:
		Awaiter(const Event& event);

		bool await_ready(void) const;
		bool await_suspend(std::coroutine_handle<void> coroHandle) noexcept;
		void await_resume(void) noexcept;

	  private:
		friend class Event;

		const Event& event_;
		std::coroutine_handle<> coroHandle_;
	};
}

#endif
