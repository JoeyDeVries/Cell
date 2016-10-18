#ifndef UTILITY_RANDOM_H
#define UTILITY_RANDOM_H

namespace Random
{
    /* NOTE(Joey):
    
      Represents a PRNG (pseudo random number generator) that generates a deterministic chain of
      random values given some mathematical equation and the modulo operator. The PRNG we use to 
      generate the random sequence is known as the Linear Congruential Generator (which you can 
      find in NextUint).

    */
    struct Series
    {
        unsigned int Index;
        unsigned int Modulo;
    };
    // NOTE(Joey): always define a global random series for easy number generation (if no series
    // is given to any of the random functions, the global series is used.
    extern Series GlobalSeries;


    // NOTE(Joey): generates a new random series 
    Series Seed(unsigned int value, unsigned int modulo = 65536);

    // NOTE(Joey): gets the next uint in series's random series.
    unsigned int NextUInt(Series *series = nullptr);
    // NOTE(Joey): gets the next uint constrained within the region specified by count.
    unsigned int Choice(unsigned int count, Series *series = nullptr);
    // NOTE(Joey): gets the next random value as a float within the range [ 0.0, 1.0]
    float Uniliteral(Series *series = nullptr);
    // NOTE(Joey): gets the next random value as a float within the range [-1.0, 1.0]
    float Biliteral(Series *series = nullptr);
    // NOTE(Joey): gets the next random value as an int between a specified min and max.
    int   RandomBetween(int min, int max, Series *series);
    // NOTE(Joey): gets the next random value as a float between a specified min and max.
    float RandomBetween(float min, float max, Series *series);
}

#endif