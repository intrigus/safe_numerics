#ifndef BOOST_NUMERIC_SAFE_BASE_HPP
#define BOOST_NUMERIC_SAFE_BASE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <limits>
#include <type_traits> // is_integral, enable_if
#include <iosfwd>
#include <cassert>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/and.hpp>

#include "concept/integer.hpp"
#include "concept/exception_policy.hpp"
#include "concept/promotion_policy.hpp"

#include "safe_common.hpp"
#include "exception_policies.hpp"

#include "boost/concept/assert.hpp"

namespace boost {
namespace numeric {

/////////////////////////////////////////////////////////////////
// forward declarations to support friend function declarations
// in safe_base

template<
    class Stored,
    Stored Min,
    Stored Max,
    class P, // promotion polic
    class E  // exception policy
>
class safe_base;

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct is_safe<safe_base<T, Min, Max, P, E> > : public std::true_type
{};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct get_promotion_policy<safe_base<T, Min, Max, P, E> > {
    using type = P;
};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct get_exception_policy<safe_base<T, Min, Max, P, E> > {
    using type = E;
};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct base_type<safe_base<T, Min, Max, P, E> > {
    using type = T;
};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
constexpr T base_value(
    const safe_base<T, Min, Max, P, E>  & st
) {
    return static_cast<T>(st);
}

template<
    typename T,
    T N,
    class P, // promotion policy
    class E  // exception policy
>
class safe_literal_impl;

/////////////////////////////////////////////////////////////////
// Main implementation

template<
    class Stored,
    Stored Min,
    Stored Max,
    class P, // promotion polic
    class E  // exception policy
>
class safe_base {
private:
    BOOST_CONCEPT_ASSERT((Integer<Stored>));
    BOOST_CONCEPT_ASSERT((PromotionPolicy<P>));
    BOOST_CONCEPT_ASSERT((ExceptionPolicy<E>));
    Stored m_t;

    template<
        class StoredX,
        StoredX MinX,
        StoredX MaxX,
        class PX, // promotion polic
        class EX  // exception policy
    >
    friend class safe_base;

    friend class std::numeric_limits<safe_base>;

    template<class T>
    constexpr Stored validated_cast(const T & t) const;

    template<typename T, T N, class P1, class E1>
    constexpr Stored validated_cast(
        const safe_literal_impl<T, N, P1, E1> & t
    ) const;

    // stream support

    template<class CharT, class Traits>
    void output(std::basic_ostream<CharT, Traits> & os) const;

    // note usage of friend declaration to mark function as
    // a global function rather than a member function.  If
    // this is not done, the compiler will confuse this with
    // a member operator overload on the << operator.  Weird
    // I know.  But it's documented here
    // http://en.cppreference.com/w/cpp/language/friend
    // under the heading "Template friend operators"
    template<class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits> &
    operator<<(
        std::basic_ostream<CharT, Traits> & os,
        const safe_base & t
    ){
        t.output(os);
        return os;
    }

    template<class CharT, class Traits>
    void input(std::basic_istream<CharT, Traits> & is);

    // see above
    template<class CharT, class Traits>
    friend inline std::basic_istream<CharT, Traits> &
    operator>>(
        std::basic_istream<CharT, Traits> & is,
        safe_base & t
    ){
        t.input(is);
        return is;
    }

public:

    ////////////////////////////////////////////////////////////
    // constructors

    constexpr explicit safe_base(const Stored & rhs, std::false_type);

    // default constructor
    /*
    constexpr explicit safe_base() {
        // this permits creating of invalid instances.  This is inline
        // with C++ built-in but violates the premises of the whole library
        // choice are:
        // do nothing - violates premise of he library that all safe objects
        // are valid
        // initialize to valid value - violates C++ behavior of types.
        // add "initialized" flag.  Preserves fixes the above, but doubles
        // "overhead"
        // still pending on this.
    }
    */
    constexpr safe_base() = default;

    template<class T>
    constexpr /*explicit*/ safe_base(const T & t);

    // note: Rule of Five.  Don't specify
    // custom constructor, custom destructor, custom assignment
    // custom move, custom move assignment
    // Let the compiler build the defaults.
public:
    /////////////////////////////////////////////////////////////////
    // casting operators for intrinsic integers
    // convert to any type which is not safe.  safe types need to be
    // excluded to prevent ambiguous function selection which
    // would otherwise occur
    template<
        class R,
        typename std::enable_if<
            !boost::numeric::is_safe<R>::value,
            int
        >::type = 0
    >
    constexpr operator R () const;
    constexpr operator Stored () const;

    /////////////////////////////////////////////////////////////////
    // modification binary operators
    template<class T>
    constexpr safe_base & operator=(const T & rhs){
        m_t = validated_cast(rhs);
        return *this;
    }
    
    template<class T, T MinT, T MaxT, class PT, class ET>
    constexpr safe_base &
    operator=(const safe_base<T, MinT, MaxT, PT, ET> & rhs);

    // mutating unary operators
    safe_base & operator++(){      // pre increment
        return *this = *this + 1;
    }
    safe_base & operator--(){      // pre decrement
        return *this = *this - 1;
    }
    safe_base operator++(int){   // post increment
        safe_base old_t = *this;
        ++(*this);
        return old_t;
    }
    safe_base operator--(int){ // post decrement
        safe_base old_t = *this;
        --(*this);
        return old_t;
    }
    // non mutating unary operators
    constexpr safe_base & operator+() const { // unary plus
        return *this;
    }
    // after much consideration, I've permited the resulting value of a unary
    // - to change the type.  The C++ standard does invoke integral promotions
    // so it's changing the type as well.

    /*  section 5.3.1 &8 of the C++ standard
    The operand of the unary - operator shall have arithmetic or unscoped
    enumeration type and the result is the negation of its operand. Integral
    promotion is performed on integral or enumeration operands. The negative
    of an unsigned quantity is computed by subtracting its value from 2n,
    where n is the number of bits in the promoted operand. The type of the
    result is the type of the promoted operand.
    */
    constexpr auto operator-() const { // unary minus
        // if this is a unsigned type and the promotion policy is native
        // the result will be unsigned. But then the operation will fail
        // according to the requirements of arithmetic correctness.
        // if this is an unsigned type and the promotion policy is automatic.
        // the result will be signed.
        return 0 - *this;
    }
    /*   section 5.3.1 &10 of the C++ standard
    The operand of ~ shall have integral or unscoped enumeration type; 
    the result is the ones’ complement of its operand. Integral promotions 
    are performed. The type of the result is the type of the promoted operand.
    */
    constexpr auto operator~() const { // complement
        return ~Stored(0u) ^ *this;
    }
};

} // numeric
} // boost

/////////////////////////////////////////////////////////////////
// numeric limits for safe<int> etc.

#include <limits>

namespace std {

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
class numeric_limits<boost::numeric::safe_base<T, Min, Max, P, E> >
    : public std::numeric_limits<T>
{
    using SB = boost::numeric::safe_base<T, Min, Max, P, E>;
public:
    constexpr static SB lowest() noexcept {
        return SB(Min, std::false_type());
    }
    constexpr static SB min() noexcept {
        return SB(Min, std::false_type());
    }
    constexpr static SB max() noexcept {
        return SB(Max, std::false_type());
    }
};

} // std

#endif // BOOST_NUMERIC_SAFE_BASE_HPP
