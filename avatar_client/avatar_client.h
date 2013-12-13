/*
 ============================================================================
 Name        : avatar_client.h
 Authors     : Benjamin Page, Benjamin Holland, Xinran Xiao
 Date 	     : May 27th, 2013
 Description : Header for avatar_client.c
 ============================================================================
 */

#ifndef AVATAR_CLIENT_H_
#define AVATAR_CLIENT_H_
#define MAX_DIM 105

#define BZERO(n,m)  memset(n, 0, m)

//! Check whether arg is NULL or not on a memory allocation. Quit this program if it is NULL.
#define MALLOC_CHECK(s)  if ((s) == NULL)   {                     \
    printf("No enough memory at %s:line%d ", __FILE__, __LINE__); \
    perror(":");                                                  \
    exit(-1); \
  }

/*=============================================================================*/
/*				  Structures			               */
/*=============================================================================*/


/*
saves the information on where an avatar was last time it went through the loop, and keeps track of what it's last successful move was.
*/
struct _RNODE
{
	unsigned int xPos;
	unsigned int yPos;
	int last_move;
};
typedef struct _RNODE LAST_MOVE;


/*=============================================================================*/
/*				 Functions 			               */
/*=============================================================================*/



/*Function to close any dead-ends in the maze
If a unit in the maze has three walls that have been observed, then we know that we are at a dead end.  we then close off the fourth wall.  what's important here is that we close off the walls such that an avatar can get out of that square.  so it prevents an avatar from going into that closed box, but it allows them to leave.  this is accomplished by closeing the opposing wall in the adjacent box, but leaves one wall open in the box in question.
*/
int close_walls(int walls[MAX_DIM][MAX_DIM][4]);


/*
Heavy-lifting function to update the walls and breadcrumbs, or clues.
The goal of this function is to generate informain that will be usefull for making the next move.  in order to acomplish this, we examine the avatar which has just previously attempted a move.  if the new poistion is different from the old position, then we know where it moved, and we know in which direction. (which is essencial if we want to turn left).  we also add a bread crumb in that square with the appropriate identification.  if the move was unsuccessful, then we examine the move which it would have attempted, and it adds a wall in that direction. 
*/
void update_info(int X_new, int Y_new, LAST_MOVE *old, int walls[MAX_DIM][MAX_DIM][4], int clues[MAX_DIM][MAX_DIM][2], unsigned int turnID, unsigned int direction);


/*Function to determine where left is based on the NSEW direction arows
*/
int left_turn(int last_move);


/*Function to calculate a direction to move. 
if there is a bread crumb, then it will attempt to follow it unless it is already blocked by a wall.  if there are no bread crumbs, then it will attempt to move left.  if there is already a wall detected, then it will attempt forward, etc.*/
int calculate(int *wall, LAST_MOVE recent_info, int X, int Y, int clue);


/*Function to check to see if avatars are linked to the right path
we use the concept of linking avatars to chose a finish line.  they all go until they are following the same person.  the following is determined by who's bread crumb path you are on.  however, we don't all have to folling the same person's bread cumbs.  rather, we are all linked up if everyone is following an avatar who is following an avatar who is being led by the same person.  once this case is reached, we return the avatar ID of the leader.
*/
int link_check(int *link, unsigned int nAvatars);

/*Function for some home-made graphics
This is pretty straight forward.  It used printf to print off the walls and avatars who exist in each box.  the > represents a wall to the west.  < is a wall to the east.  _ is a wall to the north, and ^ is a wall to the south.  
*/
void print_stuff(XYPOS *positions, int walls[MAX_DIM][MAX_DIM][4], unsigned int collums, unsigned int rows);


/*Function to determine if another avatar is following your path
We don't want to follow bread crumbs of people who are following is (such as if you arrive at a dead end and turn around to find your friend).  this function determines if the avatar who left the clue is follwing avatar whos turn it is to move, it then recognizes that it should not follow the bread crumb and makes a move indepent of the clue path.
*/
int following_me(unsigned int turnID, int *link, int clueID, unsigned int nAvatars);


#endif /* AVATAR_CLIENT_H_ */
