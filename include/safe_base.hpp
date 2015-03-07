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

#include <type_traits> // is_base_of, is_convertible, remove_reference

#include "safe_compare.hpp"
#include "policies.hpp"

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/mpl/sizeof.hpp>

namespace boost {
namespace numeric {

struct safe_tag {};

template<
    class Stored,
    class Derived,
    class Policies
>
class safe_base : private safe_tag {
    Derived & 
    derived() {
        return static_cast<Derived &>(*this);
    }
    const Derived & 
    derived() const {
        return static_cast<const Derived &>(*this);
    }
    Stored m_t;
protected:
    ////////////////////////////////////////////////////////////
    // constructors
    // default constructor
    safe_base() {}

    // copy constructor 
    safe_base(const safe_base & t) :
        m_t(t.m_t)
    {}
    template<class T>
    safe_base(const T & t){
        // verify that this is convertible to the storable type
        static_assert(
            std::is_convertible<T, Stored>::value,
            "verify that constructor argument is convertible to the storable type"
        );
        if(! derived().validate(t)){
            get_exception_policy<Policies>::type::range_error(
                "Invalid value passed to constructor "
            );
        }
        m_t = t;
    }

public:
    // used to implement stream i/o operators
    Stored & get_stored_value(){
        return m_t;
    }
    const Stored & get_stored_value() const {
        return m_t;
    }

    bool validate() const {
        if(! derived().validate(m_t)){
            get_exception_policy<Policies>::type::range_error(
                "Invalid value passed on assignment"
            );
        }
    }

    template<class T>
    bool validate(const T & rhs){
        static_assert(
            std::is_convertible<T, Derived>::value,
            "Not convertible to this save type"
        );
        if(! derived().validate(rhs)){
            get_exception_policy<Policies>::type::range_error(
                "Invalid value passed on assignment"
            );
        }
    }

    /////////////////////////////////////////////////////////////////
    // modification binary operators
    template<class T>
    Derived & operator=(const T & rhs){
        if(! derived().validate(rhs)){
            get_exception_policy<Policies>::type::range_error(
                "Invalid value passed on assignment"
            );
        }
        m_t = rhs;
        return derived();
    }
    template<class T>
    Derived & operator+=(const T & rhs){
        // validate?
        m_t = derived() + rhs;
        return derived();
    }
    template<class T>
    Derived & operator-=(const T & rhs){
        *this = *this - rhs;
        return derived();
    }
    template<class T>
    Derived & operator*=(const T & rhs){
        *this = *this * rhs;
        return derived();
    }
    template<class T>
    Derived & operator/=(const T & rhs){
        *this = *this / rhs;
        return derived();
    }
    template<class T>
    Derived & operator%=(const T & rhs){
        *this = *this % rhs;
        return derived();
    }
    template<class T>
    Derived & operator|=(const T & rhs){
        *this = *this | rhs;
        return derived();
    }
    template<class T>
    Derived & operator&=(const T & rhs){
        *this = *this & rhs;
        return derived();
    }
    template<class T>
    Derived & operator^=(const T & rhs){
        *this = *this * rhs;
        return derived();
    }
    template<class T>
    Derived & operator>>=(const T & rhs){
        *this = *this >> rhs;
        return derived();
    }
    template<class T>
    Derived & operator<<=(const T & rhs){
        *this = *this << rhs;
        return derived();
    }
    // unary operators
    Derived operator++(){
        // this checks for overflow
        *this = *this + 1;
        return derived();
    }
    Derived operator--(){
        // this checks for overflow
        *this = *this - 1;
        return derived();
    }
    Derived operator++(int){ // post increment
        Stored t = m_t;
        if(! derived().validate(*this + 1)){
            get_exception_policy<Policies>::type::overflow_error(
                "Overflow on increment"
            );
        }
        ++t;
        return derived();
    }
    Derived & operator--(int){ // post decrement
        Stored t = m_t;
        if(! derived().validate(*this - 1)){
            get_exception_policy<Policies>::type::overflow_error(
                "Overflow on increment"
            );
        }
        --t;
        return derived();
    }
    Derived operator-() const { // unary minus
        static_assert(
            boost::numeric::is_signed<Stored>::value,
            "Application of unary minus to unsigned value is an error"
        );
        *this = 0 - *this; // this will check for overflow
        return derived();
    }
    Derived operator~() const {
        static_assert(
            boost::numeric::is_signed<Stored>::value,
            "Bitwise inversion of unsigned value is an error"
        );
        validate(~m_t);
        return derived();
    }

    /////////////////////////////////////////////////////////////////
    // binary comparison operators
    template<class U>
    bool operator<(const U & rhs) const {
        return boost::numeric::safe_compare::less_than(m_t, rhs);
    }
    template<class U>
    bool operator>(const U & rhs) const {
        return boost::numeric::safe_compare::greater_than(m_t, rhs);
    }
    template<class U>
    bool operator==(const U & rhs) const {
        return boost::numeric::safe_compare::equal(m_t, rhs);
    }
    template<class U>
    bool inline operator!=(const U & rhs) const {
        return ! boost::numeric::safe_compare::equal(m_t,rhs);
    }
    template<class U>
    bool inline operator>=(const U & rhs) const {
        return ! boost::numeric::safe_compare::less_than(m_t, rhs);
    }
    template<class U>
    bool inline operator<=(const U & rhs) const {
        return ! boost::numeric::safe_compare::greater_than(m_t, rhs);
    }

    /////////////////////////////////////////////////////////////////
    // subtraction
/*
    template<class T, class U>
    struct no_subtraction_overflow_possible : public
        boost::mpl::and_<
            typename boost::mpl::greater<
                typename boost::mpl::sizeof_< decltype(Stored() - U()) >,
                typename boost::mpl::max<
                    boost::mpl::sizeof_<U>,
                    boost::mpl::sizeof_<Stored>
                >::type
            >,
            boost::numeric::is_signed<decltype(Stored() - U())>
        >
    {};

    template<class T, class U>
    struct no_subtraction_overflow_possible;

    // case 1 - no overflow possible

    template<class U>
    typename boost::enable_if<
        no_subtraction_overflow_possible<Stored, U>,
        decltype(Stored() - U())
    >::type
    inline operator-(const U & rhs) const {
        return m_t - rhs;
    }

    template<class U>
    typename boost::disable_if<
        no_subtraction_overflow_possible<Stored, U>,
        decltype(Stored() - U())
    >::type
    inline operator-(const U & rhs) const {
        return detail::check_subtraction_overflow<
            boost::numeric::is_signed<Stored>::value,
            boost::numeric::is_signed<U>::value
        >::subtract(m_t, boost::numeric::safe_cast<decltype(Stored() - U())>(rhs));
    }

    /////////////////////////////////////////////////////////////////
    // multiplication

    template<class U>
    decltype(U() * Stored())
    inline operator*(const U & rhs) const {
        return detail::check_multiplication_overflow(m_t, rhs);
    }

    /////////////////////////////////////////////////////////////////
    // division
    template<class U>
    decltype(U() / Stored())
    inline operator/(const U & rhs) const {
        return detail::check_division_overflow(m_t, rhs);
    }

    /////////////////////////////////////////////////////////////////
    // modulus
    template<class U>
    decltype(Stored() % U())
    inline operator%(const U & rhs) const {
        if(0 == rhs)
            throw std::domain_error("Divide by zero");
        return detail::check_modulus_overflow(m_t, rhs);
    }

    /////////////////////////////////////////////////////////////////
    // logical operators
    template<class U>
    decltype(Stored() | U())
    inline operator|(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        return m_t | rhs;
    }
    template<class U>
    decltype(Stored() & U())
    inline operator&(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        return m_t & rhs;
    }
    template<class U>
    decltype(Stored() ^ U())
    inline operator^(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        return m_t ^ rhs;
    }
    template<class U>
    Stored inline operator>>(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        if(m_t < 0)
            boost::numeric::overflow("right shift of negative number undefined");
        typedef decltype(Stored() >> U()) result_type;
        if(rhs > boost::numeric::bits<Stored>::value)
            boost::numeric::overflow("conversion of negative value to unsigned");

        return m_t >> rhs;
    }
    template<class U>
    Stored inline operator<<(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        if(m_t < 0)
            boost::numeric::overflow("right shift of negative number undefined");
        typedef decltype(Stored() >> U()) result_type;
        if(rhs > boost::numeric::bits<Stored>::value)
            boost::numeric::overflow("conversion of negative value to unsigned");
        return m_t << rhs;
    }

*/
    /////////////////////////////////////////////////////////////////
    // casting operators for intrinsic integers
    explicit operator Stored () const {
        return m_t;
    }

    typedef Stored stored_type;
    typedef Policies policies_type;
    //typedef Stored derived_type;
};

} // numeric
} // boost

#include <type_traits>
#include <boost/mpl/print.hpp>

namespace boost {
namespace numeric {
template<class T>
struct is_safe : public
    std::is_base_of<boost::numeric::safe_tag, T>
{};

template<class T>
struct safe_base_type {
    typedef typename T::stored_type type;
};

// invoke using base_type::type
template<class T>
struct base_type : public
    boost::mpl::eval_if<
        is_safe<T>,
        safe_base_type<T>,
        boost::mpl::identity<T>
    >
{};

namespace detail {
    template<class T>
    struct safe_type {
        static_assert(
            is_safe<T>::value,
            "Should be safe type here!"
        );
        static const typename base_type<T>::type & get_stored_value(T & t){
            return t.get_stored_value();
        }
    };
    template<class T>
    struct other_type {
        static typename base_type<T>::type & get_stored_value(T & t){
            return t;
        }
    };
} // detail

template<class T>
const typename base_type<T>::type & base_value(T & t){
    typedef typename boost::mpl::if_<
        is_safe<T>,
        detail::safe_type<T>,
        detail::other_type<T>
    >::type invoke_operator;
    return invoke_operator::get_stored_value(t);
}

template<class T>
struct get_policies {
    static_assert(
        is_safe<T>::value,
        "Policies only defined for safe types"
    );
    typedef typename T::policies_type type;
};

} // numeric
} // boost




#endif // BOOST_NUMERIC_SAFE_BASE_HPP