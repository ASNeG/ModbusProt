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

#include "ModbusTCP/ModbusTCPBase.h"

namespace ModbusTCP
{

	ModbusTCPBase::ModbusTCPBase(
		asio::io_context& ctx
	)
	: socket_(ctx)
	, useOwnThread_(false)
	{
	}

	ModbusTCPBase::ModbusTCPBase(
		void
	)
	: socket_(ctx_)
	, useOwnThread_(true)
	, thread_([this](void){ startThread(); }) // Start own event loop thread
	{
	}

	ModbusTCPBase::~ModbusTCPBase(void)
	{
		// Stop own event loop thread
		stopThread();
	}

	bool
	ModbusTCPBase::getEndpoint(
		const std::string& ipAddress,
		const std::string& port,
		asio::ip::tcp::endpoint& endpoint
	)
	{
		try {
			endpoint = *asio::ip::tcp::resolver(socket_.get_executor()).resolve(ipAddress, port);
		}
		catch (...) {
			return false;
		}

		return true;
	}

	void ModbusTCPBase::startThread(void)
	{
		work_ = new asio::io_context::work(ctx_);
		ctx_.run();
	}

	void ModbusTCPBase::stopThread(void)
	{
		if (useOwnThread_) {
			while (!work_) ;
			delete work_;
			work_ = nullptr;
			thread_.join();
		}
	}

}