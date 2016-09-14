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
    template <unsigned int n, class T>
    struct vector
    {
    public:
        T e[n];

        // NOTE(Joey): constructor0: default empty constructor; default initialize all vector elements
        vector()
        {
            // NOTE(Joey): the empty constructor frequently gets called (when declaring large lists of
            // vectors for instance) so default initializing them can be expensive as in almost all 
            // cases we'll be setting them to different values anyways.
            /*for (unsigned int i = 0; i < n; ++i)
                e[i] = {};*/
        }
        // NOTE(Joey): constructor1: one argument given: initialize all vectors elements w/ same value
        vector(T v)
        {
            for (unsigned int i = 0; i < n; ++i)
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
        T& operator[] (const unsigned int index)
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
            for (unsigned int i = 0; i < 2; ++i)
                e[i] = {};
        }
        // NOTE(Joey): constructor1: one argument given: initialize all vectors elements w/ same value
        vector(T v)
        {
            for (unsigned int i = 0; i < 2; ++i)
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
        T& operator[] (const unsigned int index)
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
            for (unsigned int i = 0; i < 3; ++i)
                e[i] = {};
        }
        // NOTE(Joey): constructor1: one argument given: initialize all vectors elements w/ same value
        vector(T v)
        {
            for (unsigned int i = 0; i < 3; ++i)
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
        T& operator[] (const unsigned int index)
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
            for (unsigned int i = 0; i < 4; ++i)
                e[i] = {};
        }
        // NOTE(Joey): constructor1: one argument given: initialize all vectors elements w/ same value
        vector(T v)
        {
            for (unsigned int i = 0; i < 4; ++i)
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
        T& operator[] (const unsigned int index)
        {
            assert(index >= 0 && index < 4);
            return e[index];
        }     

        // NOTE(Joey): math operators (defined in operation.h)
        // ---------------------------------------------------
        // NOTE(Joey): negate operator
        vector<4, T> operator-();
    };

    typedef vector<2, float>  vec2;
    typedef vector<3, float>  vec3;
    typedef vector<4, float>  vec4;
    typedef vector<2, int>    ivec2;
    typedef vector<3, int>    ivec3;
    typedef vector<4, int>    ivec4;
    typedef vector<2, double> dvec2;
    typedef vector<3, double> dvec3;
    typedef vector<4, double> dvec4;

    // NOTE(Joey): per-vector operations
    // ---------------------------------
    // NOTE(Joey): negate operator
    // NOTE(Joey): because this is a member operator, we have to
    // define this for each specialization.
    template <unsigned int n, typename T>
    inline vector<n, T> vector<n, T>::operator-()
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = -e[i];
        return result;
    }
    template <typename T>
    inline vector<2, T> vector<2, T>::operator-()
    {
        return{ -x, -y };
    }
    template <typename T>
    inline vector<3, T> vector<3, T>::operator-()
    {
        return{ -x, -y, -z };
    }
    template <typename T>
    inline vector<4, T> vector<4, T>::operator-()
    {
        return{ -x, -y, -z, -w };
    }

    // NOTE(Joey): addition
    template <unsigned int n, typename T>
    inline vector<n, T> operator+(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] + scalar;
        return result;
    }
    template <unsigned int n, typename T>
    inline vector<n, T> operator+(T scalar, vector<n, T> rhs)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] + scalar;
        return result;
    }
    template <unsigned int n, typename T>
    inline vector<n, T> operator+(vector<n, T> lhs, vector<n, T> rhs)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] + rhs[i];
        return result;
    }

    // NOTE(Joey): subtraction
    template <unsigned int n, typename T>
    inline vector<n, T> operator-(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] - scalar;
        return result;
    }
    template <unsigned int n, typename T>
    inline vector<n, T> operator-(vector<n, T> lhs, vector<n, T> rhs)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] - rhs[i];
        return result;
    }

    // NOTE(Joey): multiplication
    template <unsigned int n, typename T>
    inline vector<n, T> operator*(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] * scalar;
        return result;
    }
    template <unsigned int n, typename T>
    inline vector<n, T> operator*(T scalar, vector<n, T> lhs)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] * scalar;
        return result;
    }
    template <unsigned int n, typename T>
    inline vector<n, T> operator*(vector<n, T> lhs, vector<n, T> rhs) // NOTE(Joey): hadamard product
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] * rhs[i];
        return result;
    }

    // NOTE(Joey): division
    template <unsigned int n, typename T>
    inline vector<n, T> operator/(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] / scalar;
        return result;
    }
    template <unsigned int n, typename T>
    inline vector<n, T> operator/(T scalar, vector<n, T> lhs)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] / scalar;
        return result;
    }
    template <unsigned int n, typename T>
    inline vector<n, T> operator/(vector<n, T> lhs, vector<n, T> rhs) // NOTE(Joey): hadamard product
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = lhs[i] / rhs[i];
        return result;
    }
}


#endif