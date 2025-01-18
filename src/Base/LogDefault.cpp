/*
   Copyright 2025 Kai Huebl (kai@huebl-sgh.de)

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

#include <iostream>
#include <chrono>

#include "Base/LogDefault.h"

namespace Base
{

	LogDefault::LogDefault(void)
	: LogHandler()
	{
	}

	LogDefault::~LogDefault(void)
	{
	}

	void
	LogDefault::log(LogLevel logLevel, const std::string& message)
	{
		std::string logLevelString = "UKN";
		switch (logLevel)
		{
			case LogLevel::Error:
			{
				logLevelString = "ERR";
				break;
			}
			case LogLevel::Warning:
			{
				logLevelString = "WRN";
				break;
			}
			case LogLevel::Info:
			{
				logLevelString = "INF";
				break;
			}
			case LogLevel::Debug:
			{
				logLevelString = "DBG";
				break;
			}
		}

		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::time_t t = std::chrono::system_clock::to_time_t(now);
		std::string ts = std::ctime(&t);
		ts.resize(ts.size()-1);
		std::cout << logLevelString << " " << ts << " " << message << std::endl;
	}

}
