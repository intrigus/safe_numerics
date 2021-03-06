#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "../include/safe_range.hpp"

// NOT using safe numerics - enforce program contract explicitly
// return total number of minutes
unsigned int contract_convert(
    const unsigned int & hours,
    const unsigned int & minutes
) {
    // check that parameters are within required limits
    // invokes a runtime cost EVERYTIME the function is called
    // and the overhead of supporting an interrupt.
    // note high runtime cost!
    if(minutes > 59)
        throw std::domain_error("minutes exceeded 59");
    if(hours > 23)
        throw std::domain_error("hours exceeded 23");
    return hours * 60 + minutes;
}

// Use safe numeric to enforce program contract automatically
// define convenient typenames for hours and minutes hh:mm
using hours_t = boost::numeric::safe_unsigned_range<0, 23>;
using minutes_t = boost::numeric::safe_unsigned_range<0, 59>;
using minutes_total_t = boost::numeric::safe_unsigned_range<0, 59>;

// return total number of minutes
// type returned is safe_unsigned_range<0, 24*60 - 1>
auto convert(const hours_t & hours, const minutes_t & minutes) {
    // no need to test pre-conditions
    // input parameters are guaranteed to hold legitimate values
    // no need to test post-conditions
    // return value guaranteed to hold result
    return hours * 60 + minutes;
}

unsigned int test(
    unsigned int hours,
    unsigned int minutes
){
    // problem: checking of externally produced value can be expensive
    // invalid parameters - detected - but at a heavy cost
    return contract_convert(hours, minutes);

    // solution: use safe numerics
    // safe types can be implicitly constructed base types
    // construction guarentees corectness
    // return value is known to fit in unsigned int
    return convert(hours, minutes);

    // actually we don't even need the convert function any more
    return hours_t(hours) * 60 + minutes_t(minutes);
}

int main(int argc, const char * argv[]){
    std::cout << "example 8: ";
    std::cout << "enforce contracts with zero runtime cost" << std::endl;

    unsigned int total_minutes;

    try {
        total_minutes = test(17, 83);
    }
    catch(std::exception e){
        std::cout << "parameter error detected" << std::endl;
    }

    try {
        total_minutes = test(17, 10);
    }
    catch(std::exception e){
        // should never arrive here
        std::cout << "parameter error erroneously detected" << std::endl;
        return 1;
    }
    return 0;
}
