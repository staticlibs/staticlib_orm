/* 
 * File:   orm_test.cpp
 * Author: alex
 *
 * Created on May 28, 2016, 7:10 PM
 */

#include "staticlib/config/assert.hpp"

#include <iostream>

void test_simple() {
    
}

void test_transaction() {
    
}

void test_batch() {
    
}


int main() {
    try {
        test_simple();
        test_transaction();
        test_batch();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

