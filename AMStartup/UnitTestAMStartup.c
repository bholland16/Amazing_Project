// Filename: Test cases for AMStartup.c
// Description: A unit test for all functions in AMStartup.c
// 
//
// Test Harness Spec:
// ------------------
//
//  It tests the following functions:
//
//  isValidLong(char *);
// 
//  If any of the tests fail it prints status 
//  If all tests pass it prints status.
//



#include <stdlib.h>                          // exit, strtod, EXIT_FAILURE
#include <stdio.h>                           // fprintf, printf
#include "UnitTestFunctions.c"

// each test should start by setting the result count to zero

#define START_TEST_CASE  int rs=0

// check a condition and if false print the test condition failed
// e.g., SHOULD_BE(dict->start == NULL)
// note: the construct "#x" below is the sstringification preprocessor 
// operator that converts the argument x into a character string constant

#define SHOULD_BE(x) if (!(x))  {rs=rs+1; \
    printf("Line %d test [%s] Failed\n", __LINE__,#x); \
  }


// return the result count at the end of a test
#define END_TEST_CASE return rs


#define RUN_TEST(x, y) if (!x()) {					\
  printf("Test %s passed\n", y);		\
 } else {						\
   printf("Test %s failed\n", y);		\
   cnt = cnt + 1;				\
 }
    

//test case for isValidLong()
int VALID1 (){
  START_TEST_CASE;
  char *test= "anything";
  int result= isValidLong(test);
  SHOULD_BE(result==0);
  END_TEST_CASE;
}

//test case for isValidLong()
int VALID2 (){
  START_TEST_CASE;
  char *test= "123456";
  int result= isValidLong(test);
  SHOULD_BE(result==1);
  END_TEST_CASE;
}


//MAIN
int main(int argc, char **argv) {
  int cnt = 0;
  RUN_TEST(VALID1, "Case 1");
  RUN_TEST(VALID2, "Case 2");
  if (!cnt) {
    printf("All passed!\n"); return 0;
  } else {
    printf("Some fails!\n"); return 1;
  }
}


