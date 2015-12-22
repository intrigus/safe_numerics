#ifndef BOOST_NUMERIC_UTILITY_HPP
#define BOOST_NUMERIC_UTILITY_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint> // intmax_t, uintmax_t
#include <algorithm>
#include <boost/integer.hpp> // (u)int_t<>::least, exact

namespace boost {
namespace numeric {

    constexpr int ulog(boost::uintmax_t x){
        unsigned i = 0;
        for(; x > 0; ++i)
            x >>= 1;
        return i;
    }
    constexpr int log(std::intmax_t x){
        if(x < 0)
            x = ~x;
        return ulog(x) + 1;
    }
    template<
        std::intmax_t Min,
        std::intmax_t Max
    >
    using signed_stored_type = typename boost::int_t<
        std::max(log(Min), log(Max))
    >::least ;

    template<
        std::uintmax_t Min,
        std::uintmax_t Max
    >
    using unsigned_stored_type = typename boost::uint_t<
        std::max(ulog(Min), ulog(Max))
    >::least ;

} // numeric
} // boost

#endif  // BOOST_NUMERIC_UTILITY_HPP