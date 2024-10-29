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

	EventTask
	Queue::startHandler(Handler handler)
	{
		handlerRunning_ = true;
		while (handlerRunning_) {
			co_await event_;

			if (!handlerRunning_) continue;

			while (!queueElementList_.empty()) {
				mutex_.lock();
				auto queueElement = queueElementList_.front();
				queueElementList_.pop_front();
				mutex_.unlock();

				handler(queueElement);
			}
		}
	}

	void
	Queue::stopHandler(void)
	{
		handlerRunning_ = false;
		event_.notify();
	}

	void
	Queue::add(QueueElement::SPtr& queueElement)
	{
		std::lock_guard<std::mutex> guard(mutex_);
		auto empty = queueElementList_.empty();
		queueElementList_.push_back(queueElement);
		if (empty) {
			event_.notify();
		}
	}

}
