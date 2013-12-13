// Filename: Test cases for avatar_client.c
// Description: A unit test for all functions in avatar_client.c
// 
//
// Test Harness Spec:
// ------------------
//
//  It tests the following functions:
//
//  int close_walls(int walls[MAX_DIM][MAX_DIM][4]);
//  void update_info(int X_new, int Y_new, LAST_MOVE *old, int walls[MAX_DIM][MAX_DIM][4], int clues[MAX_DIM][MAX_DIM][2], unsigned int
//  turnID, unsigned int direction);
//  int left_turn(int last_move);
//  int calculate(int *wall, LAST_MOVE recent_info, int X, int Y, int clue);
//  int link_check(int *link, unsigned int nAvatars);
//  void print_stuff(XYPOS *positions, int walls[MAX_DIM][MAX_DIM][4], unsigned int collums, unsigned int rows);
//  int following_me(unsigned int turnID, int *link, int clueID, unsigned int nAvatars);
// 
//  If any of the tests fail it prints status 
//  If all tests pass it prints status.
//



#include <stdlib.h>                          // exit, strtod, EXIT_FAILURE
#include <stdio.h>                           // fprintf, printf
#include "amazing.h"
#include "avatar_client.h"
#include "avatarFunctionsToTest.c"

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
    

//test case for left_turn(int last_move)
int LEFT1 (){
  START_TEST_CASE;
  int i= M_NORTH;
  int result= left_turn(i);
  SHOULD_BE(result==M_WEST);
  END_TEST_CASE;
}

//test case for left_turn(int last_move)
int LEFT2 (){
  START_TEST_CASE;
  int i= M_SOUTH;
  int result= left_turn(i);
  SHOULD_BE(result==M_EAST);
  END_TEST_CASE;
}

//test case for calculating with no walls or clues
int CALC1 (){
  START_TEST_CASE;
  int walls[4];
  walls[0]=0;
  walls[1]=0;
  walls[2]=0;
  walls[3]=0;
  LAST_MOVE recent;
  recent.last_move=1;
  int result=calculate(walls,recent,1,1,-1);
  SHOULD_BE(result==M_WEST);
  END_TEST_CASE;
}


//test case for calculating with no walls or clues
int CALC2 (){
  START_TEST_CASE;
  int walls[4];
  walls[0]=1;
  walls[1]=1;
  walls[2]=0;
  walls[3]=0;
  LAST_MOVE recent;
  recent.last_move=1;
  int result=calculate(walls,recent,1,1,-1);
  SHOULD_BE(result==M_EAST);
  END_TEST_CASE;
}

//test case for calculating with no walls or clues
int FOLLOW1 (){
  START_TEST_CASE;
  unsigned int turnID= 1;
  int link[4];
  int clueID=1;
  unsigned int nAvatars=4;
  int result= following_me(turnID,link,clueID,nAvatars);
  SHOULD_BE(result==1);
  END_TEST_CASE;
}


//MAIN
int main(int argc, char **argv) {
  int cnt = 0;
  RUN_TEST(LEFT1, "Case 1");
  RUN_TEST(LEFT1, "Case 1");
  RUN_TEST(CALC1, "Case 3");
  RUN_TEST(CALC2, "Case 4");
  RUN_TEST(FOLLOW1,"Case 5");
  if (!cnt) {
    printf("All passed!\n"); return 0;
  } else {
    printf("Some fails!\n"); return 1;
  }
}

