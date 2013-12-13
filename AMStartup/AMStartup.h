/*
 ============================================================================
 Name        : AMStartup.h
 Authors     : Benjamin Page, Benjamin Holland, Xinran Xiao
 Date 	     : May 27th, 2013
 Description : Header for AMStartup.c
 ============================================================================
 */

#ifndef AMSTARTUP_H_
#define AMSTARTUP_H_

#define BZERO(n,m)  memset(n, 0, m)

// Sets a hard limit for the size of characters (used to convert ints to char *)
#define MAX_CHAR_LIMIT 2056

//! Check whether arg is NULL or not on a memory allocation. Quit this program if it is NULL.
#define MALLOC_CHECK(s)  if ((s) == NULL)   {                     \
    printf("No enough memory at %s:line%d ", __FILE__, __LINE__); \
    perror(":");                                                  \
    exit(-1); \
  }

/*=============================================================================*/
/*				 Functions 			               */
/*=============================================================================*/

/*
 * This function simply tests if the input string can be successfully converted into a long. If so, a 1 is returned,
 * if not, 0 is returned.
 */
int isValidLong(char *x);

#endif /* AMSTARTUP_H_ */
