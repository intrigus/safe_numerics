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
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ "................................",
/* 1*/ "................................",
/* 2*/ "........................xxxxxxxx",
/* 3*/ "........................xxxxxxxx",
/* 4*/ ".................................",
/* 5*/ "................................",
/* 6*/ "........................xxxxxxxx",
/* 7*/ "........................xxxxxxxx",

/* 8*/ "................................",
/* 9*/ "................................",
/*10*/ "...x...x...x............xxxxxxxx",
/*11*/ "........................xxxxxxxx",
/*12*/ "................................",
/*13*/ "................................",
/*14*/ "...x...x...x...x............xxxx",
/*15*/ "............................xxxx",

//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/*16*/ "................................",
/*17*/ "................................",
/*18*/ "................................",
/*19*/ "................................",
/*20*/ "................................",
/*21*/ "................................",
/*22*/ "................................",
/*23*/ "................................",

/*24*/ "..xx..xx..xx....................",
/*25*/ "..xx..xx..xx....................",
/*26*/ "..xx..xx..xx....................",
/*27*/ "..xx..xx..xx....................",
/*28*/ "..xx..xx..xx..xx................",
/*29*/ "..xx..xx..xx..xx................",
/*30*/ "..xx..xx..xx..xx................",
/*31*/ "..xx..xx..xx..xx................"
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


