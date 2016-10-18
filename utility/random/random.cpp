#include "random.h"

#include <math/common.h>

namespace Random
{
    Series GlobalSeries = { 1337, 65536 };

    // ------------------------------------------------------------------------
    Series Seed(unsigned int value, unsigned int modulo)
    {
        Series series = {};

        series.Index = value;
        series.Modulo = modulo;

        return series;
    }
    // ------------------------------------------------------------------------
    unsigned int NextUInt(Series *series)
    {
        unsigned int a = 1103515245;
        unsigned int c = 12345;
        if (series != nullptr)
        {
            series->Index = (a * series->Index + c) % series->Modulo;
            return series->Index;
        }
        else
        {
            GlobalSeries.Index = (a * GlobalSeries.Index + c) % GlobalSeries.Modulo;
            return GlobalSeries.Index;
        }
    }
    // ------------------------------------------------------------------------
    unsigned int Choice(unsigned int count, Series *series)
    {
        unsigned int result = NextUInt(series) % count;
        return result;
    }
    // ------------------------------------------------------------------------
    float Uniliteral(Series *series)
    {
        if (series != nullptr)
        {
            float divisor = 1.0f / series->Modulo;
            float result = (float)NextUInt(series) * divisor;
            return result;
        }
        else
        {
            float divisor = 1.0f / GlobalSeries.Modulo;
            float result = (float)NextUInt(series) * divisor;
            return result;
        }
    }
    // ------------------------------------------------------------------------
    float Biliteral(Series *series)
    {
        float result = 2.0f * Uniliteral(series) - 1.0f;
        return result;
    }
    // ------------------------------------------------------------------------
    int RandomBetween(int min, int max, Series *series)
    {
        int result = min + NextUInt(series) % ((max + 1)- min);
        return result;
    }
    // ------------------------------------------------------------------------
    float RandomBetween(float min, float max, Series *series)
    {
        float result = math::lerp(min, max, Uniliteral(series));
        return result;
    }
}