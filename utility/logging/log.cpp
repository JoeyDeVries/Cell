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


void Log::Message(std::string message, LOG_TYPE type)
{
    // NOTE(Joey): push message into proper log queue for later display
    m_LogEntries[type].push_back(message);
}

void Log::Clear()
{
    for(unsigned int i = 0; i < m_LogEntries.size(); ++i)
        m_LogEntries[i].clear();
}

void Log::Display(LOG_TYPE type)
{
    const std::string divider = "=========================================================";
    bool filter = type != LOG_DEFAULT;

    for (unsigned int i = 0; i < m_LogEntries.size(); ++i)
    {
        if (m_LogEntries[i].size() > 0)
        {
            if (!filter || (filter && i == type))
            {
                std::cout << divider << std::endl;
                std::cout << LOG_TYPE_NAME[i] << std::endl;
                std::cout << divider << std::endl;
                for (unsigned int j = 0; j < m_LogEntries[i].size(); ++j)
                {
                    //std::cout << std::setw(7) << LOG_TYPE_NAME[i] << ": " << m_LogEntries[i][j] << std::endl;
                    std::cout << " - "  << m_LogEntries[i][j] << std::endl;
                }
                std::cout << divider << std::endl << std::endl;
            }
        }
    }

}

