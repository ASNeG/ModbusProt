/*
   Copyright 2025a Kai Huebl (kai@huebl-sgh.de)

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

		std::cout << logLevelString << " " << message << std::endl;
	}

}
