#ifndef UTILITY_DIAGNOSTICS_H
#define UTILITY_DIAGNOSTICS_H

#include <iostream>
#include <string>
#include <intrin.h>

#include "../std_types.h"


#define CLOCK(name) diagnostics::clock clock##name(#name)

namespace diagnostics
{
    /* NOTE(Joey):

      Clock is a general purpose CPU clock timer that automatically
      destructs itself based on variable scope. This way there is no
      need to keep track of beginning and ending the cpu timer; simply
      create an instance of clock and it will automatically diagnose
      the respective block.

      It's also possible to diagnose parts of a larger block by
      encapsulating the clock within a manual block like so:

      somefunction()
      {
          function_we_do_not_care_about();

          {
            CLOCK(NAME_OF_TIMING);
            function_we_care_about();
            function_we_care_about();
          }
          function_we_do_not_care_about();
      }

    */
    class clock
    {
    private:
        std::string m_Name;
        u64         m_StartClock;
    public:
        clock(std::string name)
        {
            m_Name = name;
            m_StartClock = (u64)__rdtsc();
        }

        ~clock()
        {
            u64 endClock = (u64)__rdtsc();
            u64 delta = endClock - m_StartClock;
            // NOTE(Joey): clock is mostly used for internal time diagnosis, so for ease of use I simply 
            // directly set my CPU's clock speed (requesting this requires OS specific calls). If you want
            // to diagnose your own syste, simply replace the value below with your own CPU's clock speed
            // or use something like windows' QueryPerformanceFrequency to retrieve the clock speed.
            const u32 clockSpeed = 3400000000;
            float clockTime = (float)delta / (float)clockSpeed * 1000.0f;
            std::cout << m_Name << ":  " << delta << " cycles | " << clockTime << " ms" << std::endl;
            // TODO(Joey): store in global performance table for later analysis
        }
    };
}

#endif