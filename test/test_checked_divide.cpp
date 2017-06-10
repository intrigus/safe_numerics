//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cstdlib>   // EXIT_SUCCESS

#include "test_checked_divide.hpp"
#include "test.hpp"
#include "test_values.hpp"

// note: same test matrix as used in test_divide.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_division_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/* 0*/ "................................x",
/* 1*/ "................................x",
/* 2*/ "........................xxxxxxxxx",
/* 3*/ "........................xxxxxxxxx",
/* 4*/ "................................x",
/* 5*/ "................................x",
/* 6*/ "........................xxxxxxxxx",
/* 7*/ "........................xxxxxxxxx",

/* 8*/ "................................x",
/* 9*/ "................................x",
/*10*/ "...x...x...x............xxxxxxxxx",
/*11*/ "........................xxxxxxxxx",
/*12*/ "................................x",
/*13*/ "................................x",
/*14*/ "...x...x...x...x............xxxxx",
/*15*/ "............................xxxxx",

//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/*16*/ "................................x",
/*17*/ "................................x",
/*18*/ "................................x",
/*19*/ "................................x",
/*20*/ "................................x",
/*21*/ "................................x",
/*22*/ "................................x",
/*23*/ "................................x",

/*24*/ "..xx..xx..xx....................x",
/*25*/ "..xx..xx..xx....................x",
/*26*/ "..xx..xx..xx....................x",
/*27*/ "..xx..xx..xx....................x",
/*28*/ "..xx..xx..xx..xx................x",
/*29*/ "..xx..xx..xx..xx................x",
/*30*/ "..xx..xx..xx..xx................x",
/*31*/ "..xx..xx..xx..xx................x",
/*32*/ "................................x"
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_checked_divide(     \
        v1,                       \
        v2,                       \
        BOOST_PP_STRINGIZE(v1),   \
        BOOST_PP_STRINGIZE(v2),   \
        result                    \
    );
/**/

#define TESTX(value_index1, value_index2)          \
    (std::cout << value_index1 << ',' << value_index2 << ','); \
    TEST_IMPL(                                     \
        BOOST_PP_ARRAY_ELEM(value_index1, VALUES), \
        BOOST_PP_ARRAY_ELEM(value_index2, VALUES), \
        test_division_result[value_index1][value_index2] \
    )
/**/

int main(int argc, char *argv[]){
    bool rval = true;

    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}

