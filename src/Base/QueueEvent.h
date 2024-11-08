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

#ifndef __Base_QueueEvent_h__
#define __Base_QueueEvent_h__

#include <iostream>

#include <atomic>
#include <coroutine>

namespace Base
{

	struct QueueEvent {

		struct promise_type {

			QueueEvent get_return_object(void)
			{
				// std::cout << "QueueEvent::get_return_object"  << std::endl;
				return {};
			}
			std::suspend_never initial_suspend(void)
			{
				// std::cout << "QueueEvent::initial_suspend"  << std::endl;
				return {};
			}
			std::suspend_never final_suspend(void) noexcept
			{
				// std::cout << "QueueEvent::final_suspend"  << std::endl;
				return {};
			}
			void return_void(void)
			{
				// std::cout << "QueueEvent::return_void"  << std::endl;
			}
			void unhandled_exception(void)
			{
				// std::cout << "QueueEvent::unhandled_exception"  << std::endl;
			}
		};

	};

}

#endif
