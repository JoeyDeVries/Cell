#ifndef UTILITY_LOG_H
#define UTILITY_LOG_H

#include <vector>
#include <string>

/* NOTE(Joey):

  A note on global functionality.

  It is preferred to keep your objects/classes relatively closed s.t. 
  they're only being accessed/viewed from the systems that actually 
  need their service(s).

  Some services however are needed by most other service of the 
  engine/application in which case they're often required to be global. 
  One approach is to pass this functionality to each part of the engine 
  where relevant w/ a pattern like 'dependancy injection', but I 
  personally find it too much development overhead to be practical for 
  this 'global' functionality.

  We then often revert to declaring variables globally, use a 
  singleton class (which is a glorified global variable) or declare 
  variables globally encapsulated in a namespace to keep them out of 
  the global code-space. It is frowned upon to declare objects 
  globally for shared functionality throughout an engine as this makes 
  it incredibly difficult to determine where a global is accessed 
  throughout the application and cumbersome to replace this 
  functionality if required. One can slightly mitigate this by using
  the 'Service Locator' pattern.

  For a logging system that is likely to be required by every part of 
  engine I chose to use a static class because while its functionality 
  is still global, it does give me a means to keep the state of all 
  system logs private and only expose the relevant logging functions 
  to the other systems.  

*/


/* NOTE(Joey):

  The logging system supports multiple categories of logging.
  By default we display all categories, but it is possible to filter
  output on one or more logging categories; both in console output
  and in single-category output files.

*/
enum LOG_TYPE
{
    LOG_DEFAULT =  0,
    LOG_DEBUG   =  2, 
    LOG_INIT    =  4,
    LOG_WARNING =  8,
    LOG_ERROR   =  16,
};



class Log
{
private:
    /* NOTE(Joey): 
      
      we store all log entries in continuous-memory vectors instead of a map, as this ensures each 
      subsequent log entry is linear in memory. This makes looping over log entries cache 
      efficient. It does require that we manually specify a vector per category, but this is an 
      acceptable trade-off.
      
      This will be a vector of log vectors per type with the type of log entry vector indexed as 
      the enum value (which is an integer).

    */
    static std::vector<std::vector<std::string>> m_LogEntries;

    // stores the filter settings e.g. which log types the user likes to display.
    static unsigned int m_Filter;

public:
    // logs a user/engine specified message with optional type of log message. Note that log 
    // messages are queued and flushed/displyayed once by calling Flush.
    static void Message(const std::string& message, const LOG_TYPE type = LOG_DEFAULT);

    // through bitwise concatanation of LOG_TYPEs, specify which log messages should print/write 
    // to std output or file.
    static void SetFilter(unsigned int filter);

    // clear all log entries
    static void Clear();

    // displays all log entries to the standard output. By passing a log type one can filter on 
    // specific log types.
    static void Display(const LOG_TYPE type = LOG_DEFAULT);

    // TODO(Joey): implement file IO


private:
    static void printMessage(const std::string& message, const LOG_TYPE type);
};

#endif
