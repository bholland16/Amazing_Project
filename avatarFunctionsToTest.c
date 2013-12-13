/*
Functions to unit test for the avatar client
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include "amazing.h"
#define MAX_DIM 105


/*=============================================================================*/
/*				  Structures			               */
/*=============================================================================*/

struct _RNODE
{
	unsigned int xPos;
	unsigned int yPos;
	int since;
	int last_move;
};
typedef struct _RNODE LAST_MOVE;





/*Function to determine the cardinal direction of a left turn*/
int left_turn(int last_move)
{
	if(last_move==M_NORTH)
	 return(M_WEST);

	if(last_move==M_SOUTH)
	 return(M_EAST);

	if(last_move==M_WEST)
	 return(M_SOUTH);

	if(last_move==M_EAST)
	 return(M_NORTH);
	
	else
	 return(M_NULL_MOVE);
}



