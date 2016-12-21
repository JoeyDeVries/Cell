#ifndef UTILITY_STRING_ID_H
#define UTILITY_STRING_ID_H

#include <string>

/* NOTE(Joey):

  Transforms any string into a fixed-size 32 bit integer for
  efficient identification purposes while keeping all the 
  human-readable advantages of strings. This is inspired 
  from the similar approach as described by Jason Gregory
  in his book 'Game Engine Architecture'.

  Strings require more memory, dynamic memory allocation and
  are more expensive to compare than pure integers. Seeing
  as any game/graphics related engine has heavy requirements
  on those 3 mentioned points deciding on a proper unique
  identification scheme is quite relevant.

  We create a hash function named SID that does the string 
  to integer conversion. Note that hashing is relatively
  expensive; it's best to hash a string to get its unique
  and use the integer id for all further computations.

*/

#define SID(string) custom_simple_hash(string)

inline unsigned int custom_simple_hash(const std::string &str)
{
    unsigned int hash = 0;

	for (auto& it : str) {
		// NOTE(Joey): be sure to use prime numbers
		hash = 37 * hash + 17 * static_cast<char>(it);
	}

    // NOTE(Joey): hash value will automatically wrap around 
    return hash;
}
// NOTE(Joey): supports c string literals
inline unsigned int custom_simple_hash(const char* cStr)
{
    std::string str(cStr);
    return custom_simple_hash(str);
}

#endif