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

#include "ModbusTCP/Queue.h"

namespace ModbusTCP
{

	QueueElement::QueueElement(
		void
	)
	{
	}

	QueueElement::~QueueElement(
		void
	)
	{
	}

	Queue::Queue(
		void
	)
	{
	}

	Queue::~Queue(
		void
	)
	{
	}

	bool
	Queue::send(QueueElement::SPtr& queueElement)
	{
		std::lock_guard<std::mutex> guard(mutex_);
		auto empty = queueElementList_.empty();
		queueElementList_.push_back(queueElement);
		if (empty) {
			event_.notify();
		}

		return true;
	}

	EventTask
	Queue::recv(void)
	{
		// Check if element in queue list exist
		if (!queueElementList_.empty()) {
			// Get element from queue list
			mutex_.lock();
			auto queueElement = queueElementList_.front();
			queueElementList_.pop_front();
			mutex_.unlock();

			co_return;
		}

		// No element in queue list - wait for new element
		co_await event_;
		if (queueElementList_.empty()) {
			co_return;
		}

		// Get element from queue list
		mutex_.lock();
		auto queueElement = queueElementList_.front();
		queueElementList_.pop_front();
		mutex_.unlock();

		co_return;
	}

}
