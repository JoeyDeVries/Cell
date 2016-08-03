#ifndef UTILITY_DIAGNOSTICS_H
#define UTILITY_DIAGNOSTICS_H

#include <iostream>
#include <string>
#include <intrin.h>

#include "std_types.h"

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

    #define CLOCK(name) clock clock##name(#name)

    class clock
    {
        std::string m_Name;
        u64         m_StartClock;

        clock(std::string name)
        {
            m_Name = name;
            m_StartClock = (u64)__rdtsc();
        }

        ~clock()
        {
            u64 endClock = (u64)__rdtsc();
            u64 delta = endClock - m_StartClock;
            // TODO(Joey): globally retrieve CPU estimate of cycles/p/second and calculate time in s and ms
            std::cout << m_Name << " :  " << delta << " cycles " << std::endl;
            // TODO(Joey): directly print and/or store in global performance table for later analysis
        }
    };
}

#endif