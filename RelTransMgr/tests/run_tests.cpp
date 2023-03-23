#define STDC_HEADERS 1

#include <gtest/gtest.h>
#include <limits.h>

int main(int argc, char *argv[]) {

  testing::InitGoogleTest(&argc, argv);
  int status = RUN_ALL_TESTS();

  return status;
}
