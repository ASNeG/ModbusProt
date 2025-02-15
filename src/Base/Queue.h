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

#ifndef __Base_Queue_h__
#define __Base_Queue_h__

#include <asio.hpp>
#include <list>

#include "Base/Event.h"
#include "Base/QueueEvent.h"
#include "Base/QueueElement.h"

namespace Base
{

	class Queue
	{
	  public:
		using QueueResult = std::tuple<bool, QueueElement::SPtr>;

		Queue(
			void
		);
		~Queue(
			void
		);

		bool send(QueueElement::SPtr& queueElement);
		QueueResult recv(void);
		QueueElement::SPtr getAndRemoveFirst(void);

	  private:
		// Queue list
		std::list<QueueElement::SPtr> queueElementList_;
		std::mutex mutex_;
		Event event_;

		QueueEvent waitForEvent(void);
	};

}

#endif
