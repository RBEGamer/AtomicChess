/*
 * all_tests.cpp
 *
 *  Created on: 21 Sep 2015
 *      Author: miles
 */

#include "gtest/gtest.h"


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
