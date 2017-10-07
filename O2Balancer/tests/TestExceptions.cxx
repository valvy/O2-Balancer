#define BOOST_TEST_MODULE "Testing Exceptions"
//#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <O2/Balancer/Exceptions/InitException.h>
#include <O2/Balancer/Exceptions/UnimplementedException.h>

using namespace O2::Balancer::Exceptions;

template<typename T>
void testException(){
    T exc("exception");
    BOOST_CHECK(exc.getMessage() == "exception");
}

BOOST_AUTO_TEST_CASE(testInitException){
    testException<InitException>(); 
}

BOOST_AUTO_TEST_CASE(testUnimplementedException){
    testException<UnimplementedException>(); 
}


