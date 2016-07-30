#ifndef MATH_LINEAR_ALGEBRA_VECTOR_H
#define MATH_LINEAR_ALGEBRA_VECTOR_H

#include <initializer_list>
#include <assert.h>

namespace math
{
    /* NOTE(Joey):

      Generic vector template version supporting vectors of any type and any
      positive dimension. Constructors and templated vector mathematics are 
      defined outside the templated class to support shared functionality in
      combination with template specialization of vec2,vec3 and vec4.

      The underlying implementation of all the vector types are based on C++
      templates. The most common found vector classes are however defined with
      preprocessor types for better readability hiding the underlying template
      implementation. For vectors of a dimension > 4 it is required to directly
      specify the template variables.

    */
    template <u32 n, class T>
    struct vector
    {
    public:
        T e[n];

        // NOTE(Joey): constructor0: default empty constructor; default initialize all vector elements
        vector()
        {
            for (u32 i = 0; i < n; ++i)
                e[i] = {};
        }
        // NOTE(Joey): constructor1: one argument given: initialize all vectors elements w/ same value
        vector(T v)
        {
            for (u32 i = 0; i < n; ++i)
                e[i] = v;
        }
        // NOTE(Joey): constructor2: use std::initializer list for accepting any number of arguments
        vector(std::initializer_list<T> args)
        {
            assert(args.size() < n);
            int index = 0;
            for (auto begin = args.begin(); begin != args.end(); ++begin)
                e[index++] = *begin;
        }

        // NOTE(Joey): subscript operator
        T& operator[] (const u32 index)
        {
            assert(index >= 0 && index < n);
            return e[index];
        }

        // NOTE(Joey): math member operators (defined in operation.h)
        // ---------------------------------------------------
        // NOTE(Joey): negate operator
        vector<n, T> operator-();
    };

    /* NOTE(Joey): 

      Specialized vector version for 2-dimensional vectors.

    */
    template <typename T>
    struct vector<2, T>
    {
        union 
        {
            T e[2];
            // position
            struct 
            {
                T x;
                T y;
            };
            // texture coordinates
            // NOTE(Joey): only available in vec2, due to .w being reserved for position.
            struct
            {
                T u;
                T v;
            };
            // texture coordinates
            // NOTE(Joey): s,t,r is the general form of accessing texture coordinates in OpenGL.
            // Using u,v,w in 3 dimensions requires w which is reserved for the position's 
            // homogenous coordinate set.
            struct
            {
                T s;
                T t;
            };
        };

        // NOTE(Joey): constructor0: default empty constructor; default initialize all vector elements
        vector()
        {
            for (u32 i = 0; i < 2; ++i)
                e[i] = {};
        }
        // NOTE(Joey): constructor1: one argument given: initialize all vectors elements w/ same value
        vector(T v)
        {
            for (u32 i = 0; i < 2; ++i)
                e[i] = v;
        }
        // NOTE(Joey): constructor2: use std::initializer list for accepting any number of arguments
        vector(std::initializer_list<T> args)
        {
            assert(args.size() <= 2);
            int index = 0;
            for (auto begin = args.begin(); begin != args.end(); ++begin)
                e[index++] = *begin;
        }
        // NOTE(Joey): constructor3: vec2 per-element initialization
        vector(T x, T y)
        {
            e[0] = x;
            e[1] = y;
        }

        // NOTE(Joey): subscript operator
        T& operator[] (const u32 index)
        {
            assert(index >= 0 && index < 2);
            return e[index];
        }

        // NOTE(Joey): math operators (defined in operation.h)
        // ---------------------------------------------------
        // NOTE(Joey): negate operator
        vector<2, T> operator-();
    };

    /* NOTE(Joey):
        
       Specialized vector version for 3-dimensional vectors.

    */
    template<typename T>
    struct vector<3, T>
    {
        union 
        {
            T e[3];
            // position
            struct
            {
                T x;
                T y;
                T z;
            };
            // color
            struct
            {
                T r;
                T g;
                T b;
            };
            // texture coordinates
            struct
            {
                T s;
                T t;
                T r;
            };
            struct
            {
                vector<2, T> xy;
            };
            struct
            {
                vector<2, T> yz;
            };
        };

        // NOTE(Joey): static frequently used vectors
        static vector<3, T> UP;
        static vector<3, T> DOWN;
        static vector<3, T> LEFT;
        static vector<3, T> RIGHT;
        static vector<3, T> FORWARD;
        static vector<3, T> BACK;

        // NOTE(Joey): constructor0: default empty constructor; default initialize all vector elements
        vector()
        {
            for (u32 i = 0; i < 3; ++i)
                e[i] = {};
        }
        // NOTE(Joey): constructor1: one argument given: initialize all vectors elements w/ same value
        vector(T v)
        {
            for (u32 i = 0; i < 3; ++i)
                e[i] = v;
        }
        // NOTE(Joey): constructor2: use std::initializer list for accepting any number of arguments
        vector(std::initializer_list<T> args)
        {
            assert(args.size() <= 3);
            int index = 0;
            for (auto begin = args.begin(); begin != args.end(); ++begin)
                e[index++] = *begin;
        }
        // NOTE(Joey): constructor3: vec3 per-element initialization
        vector(T x, T y, T z)
        {
            e[0] = x;
            e[1] = y;
            e[2] = z;
        }
        // NOTE(Joey): constructor4: vec2 initialization
        vector(vector<2, T> vec, T z)
        {
            e[0] = vec.x;
            e[1] = vec.y;
            e[2] = z;
        }
        // NOTE(Joey): constructor5: vec2 initialization
        vector(T x, vector<2, T> vec)
        {
            e[0] = x;
            e[1] = vec.x;
            e[2] = vec.y;
        }

        // NOTE(Joey): subscript operator
        T& operator[] (const u32 index)
        {
            assert(index >= 0 && index < 3);
            return e[index];
        }

        // NOTE(Joey): math operators (defined in operation.h)
        // ---------------------------------------------------
        // NOTE(Joey): negate operator
        vector<3, T> operator-();
    };

    // NOTE(Joey): initialize static variables of vec3
    template<typename T> vector<3, T> vector<3, T>::UP      = vector<3, T>( 0.0,  1.0,  0.0);
    template<typename T> vector<3, T> vector<3, T>::DOWN    = vector<3, T>( 0.0, -1.0,  0.0);
    template<typename T> vector<3, T> vector<3, T>::LEFT    = vector<3, T>(-1.0,  0.0,  0.0);
    template<typename T> vector<3, T> vector<3, T>::RIGHT   = vector<3, T>( 1.0,  0.0,  0.0);
    template<typename T> vector<3, T> vector<3, T>::FORWARD = vector<3, T>( 0.0,  0.0, -1.0);
    template<typename T> vector<3, T> vector<3, T>::BACK    = vector<3, T>( 0.0,  0.0,  1.0);

    /* NOTE(Joey):

    Specialized vector version for 4-dimensional vectors.

    */
    template<typename T>
    struct vector<4, T>
    {
        union
        {
            T e[4];
            // position
            struct
            {
                T x;
                T y;
                T z;
                T w;
            };
            // color
            struct
            {
                T r;
                T g;
                T b;
                T a;
            };
            // texture coordinates
            struct
            {
                T s;
                T t;
                T r;
            };
            struct
            {
                vector<2, T> xy;
                T _ignored1;
                T _ignored2;
            };
            struct
            {
                T _ignored1;
                T _ignored2;
                vector<2, T> yz;
            };
            struct
            {
                vector<3, T> xyz;
                T _ignored1;
            };
            struct
            {
                vector<3, T> rgb;
                T _ignored1;
            };
            struct
            {
                vector<3, T> srt;
                T _ignored1;
            };
        };

        // NOTE(Joey): constructor0: default empty constructor; default initialize all vector elements
        vector()
        {
            for (u32 i = 0; i < 4; ++i)
                e[i] = {};
        }
        // NOTE(Joey): constructor1: one argument given: initialize all vectors elements w/ same value
        vector(T v)
        {
            for (u32 i = 0; i < 4; ++i)
                e[i] = v;
        }
        // NOTE(Joey): constructor2: use std::initializer list for accepting any number of arguments
        vector(std::initializer_list<T> args)
        {
            assert(args.size() <= 4);
            int index = 0;
            for (auto begin = args.begin(); begin != args.end(); ++begin)
                e[index++] = *begin;
        }
        // NOTE(Joey): constructor3: vec3 per-element initialization
        vector(T x, T y, T z, T w)
        {
            e[0] = x;
            e[1] = y;
            e[2] = z;
            e[3] = w;
        }
        // NOTE(Joey): constructor4: vec2 initialization
        vector(vector<2, T> xy, vector<2, T> zw)
        {
            e[0] = xy.x;
            e[1] = xy.y;
            e[2] = zw.x;
            e[3] = zw.y;
        }
        // NOTE(Joey): constructor5: vec3 initialization
        vector(vector<3, T> xyz, T w)
        {
            e[0] = xyz.x;
            e[1] = xyz.y;
            e[2] = xyz.z;
            e[3] = w;
        }

        // NOTE(Joey): subscript operator
        T& operator[] (const u32 index)
        {
            assert(index >= 0 && index < 4);
            return e[index];
        }

        // NOTE(Joey): math operators (defined in operation.h)
        // ---------------------------------------------------
        // NOTE(Joey): negate operator
        vector<4, T> operator-();
    };
}


#endif