/*
Functions to unit test for AMStartup
*/

#include <stdio.h>
#include <stdlib.h>

/*Function to determine if something is a valid long integer*/
int isValidLong(char *x)
{
  char* endp;
  
  strtol(x, &endp, 10);
  
  return !*endp;
}
