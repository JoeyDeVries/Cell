#ifndef UTILITY_FLOATING_POINT_H
#define UTILITY_FLOATING_POINT_H

#include <algorithm>

/* NOTE(Joey):

  Define a set of utility functions that deal with or
  circumvent IEEE 754 floating point precision issues.

*/
namespace floating_point
{

    /* NOTE(Joey):

      When comparing two floating point values one can use
      an epsilon value to mitigate the precision issues of
      a floating point. However, this is an absolute value
      while the magnitude of error on floating point values
      is not absolute (the error of precision increases as
      the magnitude of the value increases).
      
      We thus compare two floating points within a given
      'relative' interval instead of an 'absolute' fixed
      interval epsilon.
      
      We can compute the relative interval by dividing the
      difference of a and b by b and see if the absolute
      value is less than or equal to epsilon. We can remove
      the division by multiplying the epsilon by b.
      
      For a more detailled description: see 1.4.7 in
      'Essential mathematics for Games and Interactive
      Applications'.

    */

	// NOTE(Nabil/htmlboss): The STL has an overloaded abs()
    inline bool compare(const float a, const float b, const float epsilon)
    {
        const float diff = std::abs(a - b);
        // NOTE(Joey): originally we want to take the absolute max of either
        // a or b or 1.0 (s.t. we don't scale epsilon down) to make sure we 
        // scale epsilon by the biggest value; however, max() can be 
        // expensive so we approximate by adding all 3 values together.
        const float compare = epsilon * (std::abs(a) + std::abs(b) + 1.0f);
        return diff <= compare;
    }

    // NOTE(Joey): the above comparrison is more general, but can still be too
    // slow in performance critical code. Here we use a more efficient absolute
    // epsilon check which is fine if one takes the relative magnitude of the
    // inputs into account.
    inline bool compareAbsolute(const float a, const float b, const float epsilon)
    {
        return std::abs(a - b) <= epsilon;
    }
} // namespace floating_point

#endif
