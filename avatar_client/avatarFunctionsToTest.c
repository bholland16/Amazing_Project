/*
Functions to unit test for the avatar client
*/

#include <stdio.h>
#include <stdlib.h>
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


/*Function to calculate a direction to move. Basically an adapted wall follower that fills in dead-ends and follows breadcrumbs*/
int calculate(int *wall, LAST_MOVE recent_info, int X, int Y, int clue)
{
	int left, right, forward, back;

	//Each relative direction is just a left turn from another relative direction
	left=left_turn(recent_info.last_move);
	back=left_turn(left);
	right=left_turn(back);
	forward=left_turn(right);


	//If there is a breadrumb, follow it
	if( clue>=0 && !wall[clue] )
	 return( clue );

	if(!wall[left])
	  {
	    // printf("moving left\n");
	    return(left);
	  }

	if(!wall[forward])
	  {
	    //printf("moving forward\n");
	    return(forward);
	  }
	 
	 if(!wall[right])
	   {
	     // printf("moving right\n");
	     return(right);
	   }

	 return(back);
 
}


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


/*Function to determine if another avatar is following your path*/
int following_me(unsigned int turnID, int *link, int clueID, unsigned int nAvatars)
{
	int i, test;

	test=clueID;

	for(i=0; i<nAvatars; i++)
	 {
		if(test==turnID)
		 return(1);

		test=link[test];
	  }

	return(0);
}


