#include "log.h"

#include <iostream>
#include <iomanip>

// NOTE(Joey): not too pretty, but directly define the log types as
// strings for proper category printing. Given the small amount of 
// log categories I deem this an acceptable solution, instead of 
// toying around with preprocessor magic.
static std::string LOG_TYPE_NAME[] =
{
    "DEFAULT",
    "DEBUG",
    "INIT",
    "WARNING",
    "ERROR",
};

// NOTE(Joey): based on the number of enum log types we've specified
// we initialize that many log queue vectors.
int nrLogTypes = LOG_ERROR - LOG_DEFAULT + 1;
std::vector<std::vector<std::string>> Log::m_LogEntries = std::vector<std::vector<std::string>>(nrLogTypes);

// initialize the log filter to print all log types
unsigned int Log::m_Filter = LOG_TYPE::LOG_DEFAULT |
                             LOG_TYPE::LOG_DEBUG   |
                             LOG_TYPE::LOG_INIT    |
                             LOG_TYPE::LOG_WARNING |
                             LOG_TYPE::LOG_ERROR;

void Log::Message(const std::string& message, const LOG_TYPE type)
{
    // push message into proper log queue for later display and/or write-to-disk
    m_LogEntries[type].push_back(message);

    if (type == LOG_TYPE::LOG_DEFAULT && m_Filter & LOG_TYPE::LOG_DEFAULT ||
        type == LOG_TYPE::LOG_DEBUG   && m_Filter & LOG_TYPE::LOG_DEBUG   ||
        type == LOG_TYPE::LOG_INIT    && m_Filter & LOG_TYPE::LOG_INIT    ||
        type == LOG_TYPE::LOG_WARNING && m_Filter & LOG_TYPE::LOG_WARNING ||
        type == LOG_TYPE::LOG_ERROR   && m_Filter & LOG_TYPE::LOG_ERROR)
    {
        Log::printMessage(message, type);
    }
}

void Log::SetFilter(unsigned int filter)
{
    m_Filter = filter;
}

void Log::Clear()
{
	for (auto& entry : m_LogEntries) {
		entry.clear();
	}
}

void Log::Display(const LOG_TYPE type)
{
    const std::string divider = "=========================================================";
    const bool filter = type != LOG_DEFAULT;

    for (std::size_t i = 0; i < m_LogEntries.size(); ++i)
    {
        if (m_LogEntries.at(i).size() > 0)
        {
            if (!filter || (filter && i == type))
            {
             /*   std::cout << divider << std::endl;
                std::cout << LOG_TYPE_NAME[i] << std::endl;
                std::cout << divider << std::endl;*/
                for (std::size_t j = 0; j < m_LogEntries.at(i).size(); ++j)
                {
                    std::cout << std::setw(7) << LOG_TYPE_NAME[i] << ": " << m_LogEntries.at(i).at(j) << std::endl;
                    //std::cout << " - "  << m_LogEntries[i][j] << std::endl;
                }
                std::cout << std::endl;
                //std::cout << divider << std::endl << std::endl;
            }
        }
    }
}



void Log::printMessage(const std::string& message, const LOG_TYPE type)
{
    std::string typeName = "DEFAULT";
    // TODO(Joey): solve this w/ preprocessor magic
    if (type == LOG_TYPE::LOG_DEBUG)  typeName = "DEBUG";
    if (type == LOG_TYPE::LOG_INIT)   typeName = "INIT";
    if (type == LOG_TYPE::LOG_ERROR)  typeName = "ERROR";
    if(type == LOG_TYPE::LOG_WARNING) typeName = "WARNING";

    std::cout << std::setw(7) << typeName << ":  " << message << std::endl;
}
