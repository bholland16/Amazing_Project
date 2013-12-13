/*
 ============================================================================
 Name        : avatar_client.c
 Authors     : Benjamin Page, Benjamin Holland, Xinran Xiao
 Date 	     : May 27th, 2013
 Description : Avatar Client for Amazing Project for cs50

 Input	     : serverIP, avatarID, nAvatars, difficulty, mazePort, logName
 Output	     : Error messages, logfile, various messages to user, graphics
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include "amazing.h"
#include "avatar_client.h"


/*=============================================================================*/
/*				Global Variables  			       */
/*=============================================================================*/
FILE *fp;
unsigned int turnID;


/*=============================================================================*/
/*				   MAIN  			               */
/*=============================================================================*/

int main(int argc, char **argv)
{
	unsigned int avatarID, nAvatars, difficulty, mazePort, width, height;
	char *serverIP, *logName;


	// Command line processing on arguments-------------------------------------
	// No checks since all checks are done in AMStartup. Everything passed into this should be good.
	serverIP = argv[4];
	avatarID = strtol(argv[1], NULL, 10);
	nAvatars = strtol(argv[2], NULL, 10);
	difficulty = strtol(argv[3], NULL, 10);
	mazePort = strtol(argv[5], NULL, 10);
	logName = argv[6];
	width = strtol(argv[7], NULL, 10);
	height = strtol(argv[8], NULL, 10);
	printf("avatar_client %d %d %d %s %d %s\n", avatarID, nAvatars, difficulty, serverIP, mazePort, logName);
	//---------------------------------------------------------------------------

	// Connect to the server-----------------------------------------------------
	int sockfd;
	struct sockaddr_in servaddr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// <0 means there was an error in creation of the socket
		fprintf(stderr, "avatar_client: Error creating socket to connect to server.\n");
		return EXIT_FAILURE;
	}
	BZERO(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(serverIP);
	servaddr.sin_port = htons(mazePort);
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		fprintf(stderr, "avatar_client: Problem connecting to the server.\n");
		return EXIT_FAILURE;
	}
	//---------------------------------------------------------------------------


	// Create ready signal and send it-------------------------------------------
	AM_MESSAGE readyMessage;
	readyMessage.message_type = htonl(AM_AVATAR_READY);
	readyMessage.msg.avatar_ready.AvatarId = htonl(avatarID);
	if (send(sockfd, &readyMessage, sizeof(readyMessage), 0) < 0)	// very important to send sizeof the pointer
	{
		fprintf(stderr, "avatar_client: Error sending ready message.\n");
		return EXIT_FAILURE;
	}
	//---------------------------------------------------------------------------


	// Main processing loop
	unsigned int direction;
	AM_MESSAGE receivedMessage;
	AM_MESSAGE moveMessage;
	ssize_t recvrc;
	XYPOS *positions;

	// Some initialization stuff-------------------------------------------------


	int walls[MAX_DIM][MAX_DIM][4];
	int clues[MAX_DIM][MAX_DIM][2];
	LAST_MOVE recent[AM_MAX_AVATAR];
	int i, j, k, X, Y, last_Avatar, firstTime, follow;
	
	int linked, dest_X, dest_Y;
	int initialized[AM_MAX_AVATAR];
	int link[AM_MAX_AVATAR];
	
	firstTime=1;	
	linked=-1;

	for(i=0; i<MAX_DIM; i++)
	  for(j=0; j<MAX_DIM; j++)
	    {
	      for(k=0; k<4; k++)
		walls[i][j][k]=0;
	      
	      clues[i][j][0]=-1;
	      clues[i][j][1]=-1;
	    }
	
	for(i=0; i<AM_MAX_AVATAR; i++)
	  {
	    recent[i].last_move=M_NORTH;
	    
	    initialized[i]=0;
	  }

	for(i=0; i<AM_MAX_AVATAR; i++)
	 {
	    link[i]=i;
	  }


	// --------------------------------------------------------------------------
	

	int done = 0;
	while (!done)
	{

		if ((recvrc = recv(sockfd, &receivedMessage, sizeof(receivedMessage), 0)) == 0)
		{
			fprintf(stderr, "The server terminated prematurely.\n");
			break;
		}

		switch(ntohl(receivedMessage.message_type))
		{
		case AM_AVATAR_TURN:
			// Main meat, do stuff here

			// Get fields from turn-------------------------------
			turnID = ntohl(receivedMessage.msg.avatar_turn.TurnId);
			positions = receivedMessage.msg.avatar_turn.Pos;
			// ---------------------------------------------------


			//If it is not our fist time and linked is still initialized to -1
			if( !firstTime && linked<0) 
			 {
				//Get positions from XYPOS structure and increment both coordinates due to the way walls are handled
			   	Y=ntohl(positions[last_Avatar].yPos);
			   	X=ntohl(positions[last_Avatar].xPos);
			   	X++;
			   	Y++;
				follow=0;
				if( clues[X][Y][0]>=0 && !following_me(last_Avatar, link, clues[X][Y][1], nAvatars) ) follow=1; 
				if(follow) follow=clues[X][Y][0]; else follow=-1;
				direction=calculate( walls[X][Y], recent[last_Avatar], X, Y, follow); //calculate the direction
			     	update_info(X, Y, &recent[last_Avatar], walls, clues, last_Avatar, direction);
 			 }

			//Get positions and increment
			Y=ntohl(positions[turnID].yPos);
			X=ntohl(positions[turnID].xPos);
			X++;
			Y++;

			follow=0;

			//If there is a clue in the coordinates the person who left the clue is not already followinf you, then set follow to one.  (meaning you should, infact follow the clue).
			if( clues[X][Y][0]>=0 && !following_me(turnID, link, clues[X][Y][1], nAvatars) ) follow=1;

			//Set up the links
			if(linked<0)
			 {
				link[turnID]=turnID;

				if(clues[X][Y][0]>=0)
				{	
					if(!following_me(turnID, link, clues[X][Y][1], nAvatars) )
					link[turnID]=clues[X][Y][1];
				 }
			
				linked=link_check(link, nAvatars);

				if(linked>=0) 
				 {
					dest_X=ntohl(positions[linked].xPos)+1;
					dest_Y=ntohl(positions[linked].yPos)+1;			
				  }
			  }



			last_Avatar=turnID;//
			firstTime=0;

			close_walls(walls); //Close off any dead-ends in the maze

			//If this turn ID has not been initialized, set its recent moves
			if(!initialized[turnID])
			 {
				recent[turnID].xPos=X;
				recent[turnID].yPos=Y;
				initialized[turnID]=1;
			  }

			// Execute move
			if (turnID == avatarID)
			{
				// It's your turn, do stuff

if(turnID==0) {print_stuff(positions, walls, width, height);}

				//Set follow depending on clues
				if(follow) follow=clues[X][Y][0]; else follow=-1;
				
				//Calculate a move
			  	direction=calculate( walls[X][Y], recent[turnID], X, Y, follow);

			 	//printf("%d:\n\tPosition: (%d,%d)\n\tWalls:\n\t\tNorth- %d\n\t\tSouth- %d\n\t\tEast-  %d\n\t\tWest-  %d\n\tMOVE: %d\n\tCLUE: %d,%d,%d\n\tLAST: %d\n\tLinked: %d\n\tDest:(%d,%d)\n", turnID, X-1, Y-1, walls[X][Y][M_NORTH],walls[X][Y][M_SOUTH],walls[X][Y][M_EAST],walls[X][Y][M_WEST], direction, clues[X][Y][1], clues[X][Y][0], follow, recent[turnID].last_move, linked, dest_X-1, dest_Y-1);

				//printf("(%d,%d)\n", X-1, Y-1);

				//If we are linked up
				if(linked>=0)
				 {
					direction=clues[X][Y][0]; //direction is just the breadcrumb left there, no need to calculate move
					
					if(X==dest_X && Y==dest_Y)
					 direction=M_NULL_MOVE; //If we are at the destination then we stay put
				  }

				//Make the message
				moveMessage.message_type = htonl(AM_AVATAR_MOVE);				
				moveMessage.msg.avatar_move.AvatarId = htonl(avatarID);
				moveMessage.msg.avatar_move.Direction = htonl(direction);				

				// Send move
				if (send(sockfd, &moveMessage, sizeof(moveMessage), 0) < 0)	// very important to send sizeof the pointer
				{
					fprintf(stderr, "avatar_client: Error sending move.\n");
					break;	// this may not be right.. does it break out of switch/case or while loop..?
				}

				// Log move in log
				fp=fopen(logName, "ab");
				if(direction==0)fprintf(fp, "Avatar %d is in position (%d,%d) and moved West\n", avatarID,X-1,Y-1);
				if(direction==1)fprintf(fp, "Avatar %d is in position (%d,%d) and moved North\n", avatarID,X-1,Y-1);
				if(direction==2)fprintf(fp, "Avatar %d is in position (%d,%d) and moved South\n", avatarID,X-1,Y-1);
				if(direction==3)fprintf(fp, "Avatar %d is in position (%d,%d) and moved East\n", avatarID,X-1,Y-1);
				if(direction==8)fprintf(fp, "Avatar %d chose to move nowhere.\n", avatarID);
				fclose(fp);
			}
			else
			{
				// Not your turn, move on.
				continue;
			}

			break;

		case AM_MAZE_SOLVED:
			if(turnID==avatarID)
			  {
			fp=fopen(logName, "ab");
			fprintf(fp,"Maze solved! It took %u moves", ntohl(receivedMessage.msg.maze_solved.nMoves));
			fclose(fp);
			printf("\nMUY BUENO!\n");
			   }
			done = 1;// Maze solved, do stuff here
			break;
			 

		case AM_NO_SUCH_AVATARID:
			fprintf(stderr, "AM_NO_SUCH_AVATARID message received. Exiting...\n");
			done = 1;
			break;

		case AM_TOO_MANY_MOVES:
			fprintf(stderr, "AM_NO_SUCH_AVATARID message received. Exiting...\n");
			done = 1;
			break;

		case AM_UNKNOWN_MSG_TYPE:
			fprintf(stderr, "AM_NO_SUCH_AVATARID message received. Exiting...\n");
			done = 1;
			break;

		case AM_SERVER_TIMEOUT:
			fprintf(stderr, "AM_NO_SUCH_AVATARID message received. Exiting...\n");
			done = 1;
			break;

		case AM_UNEXPECTED_MSG_TYPE:
			fprintf(stderr, "AM_NO_SUCH_AVATARID message received. Exiting...\n");
			done = 1;
			break;
		default:
			fprintf(stderr, "Unexpected message.\n");
			done = 1;
			break;
		}
	}

	
	return EXIT_SUCCESS;
}


/*Function to calculate a direction to move. 
if there is a bread crumb, then it will attempt to follow it unless it is already blocked by a wall.  if there are no bread crumbs, then it will attempt to move left.  if there is already a wall detected, then it will attempt forward, etc.*/
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


/*Function to determine where left is based on the NSEW direction arows
*/
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


/*
Heavy-lifting function to update the walls and breadcrumbs, or clues.
The goal of this function is to generate informain that will be usefull for making the next move.  in order to acomplish this, we examine the avatar which has just previously attempted a move.  if the new poistion is different from the old position, then we know where it moved, and we know in which direction. (which is essencial if we want to turn left).  we also add a bread crumb in that square with the appropriate identification.  if the move was unsuccessful, then we examine the move which it would have attempted, and it adds a wall in that direction. 
*/
void update_info(int X_new, int Y_new, LAST_MOVE *old, int walls[MAX_DIM][MAX_DIM][4], int clues[MAX_DIM][MAX_DIM][2], unsigned int turnID, unsigned int direction)
{
	int add_walls, new_clue;


	 if( (X_new!=old->xPos) || (Y_new!=old->yPos) ) //new position
	   {

	     if(  X_new < old->xPos ) //we moved west
	       old->last_move=M_WEST;		
	     else if( X_new > old->xPos ) //we moved east
	       old->last_move=M_EAST;
	     else if( Y_new > old->yPos ) //we moved south
	       old->last_move=M_SOUTH;
	     else if( Y_new < old->yPos ) //we moved north
	       old->last_move=M_NORTH;

		new_clue=old->last_move;

		if(new_clue!=clues[old->xPos][old->yPos][0])
		 clues[old->xPos][old->yPos][1]=turnID;

	       clues[old->xPos][old->yPos][0]=old->last_move;

	     old->xPos=X_new;
	     old->yPos=Y_new;

	   }


	 else if( old->last_move!=M_NULL_MOVE ) //same position
		 {
			add_walls=direction;

		if( walls[X_new][Y_new][add_walls]==0 )
		{
		   walls[X_new][Y_new][add_walls]=1;	
		   
		   if(add_walls==M_NORTH) Y_new--;
		   if(add_walls==M_SOUTH) Y_new++;
		   if(add_walls==M_EAST) X_new++;
		   if(add_walls==M_WEST) X_new--;
		   
		   add_walls=left_turn(add_walls);
		   add_walls=left_turn(add_walls);
		   
		   walls[X_new][Y_new][add_walls]=1;
		 }
		   
		 }
}


/*Function to close any dead-ends in the maze
If a unit in the maze has three walls that have been observed, then we know that we are at a dead end.  we then close off the fourth wall.  what's important here is that we close off the walls such that an avatar can get out of that square.  so it prevents an avatar from going into that closed box, but it allows them to leave.  this is accomplished by closeing the opposing wall in the adjacent box, but leaves one wall open in the box in question.
*/
int close_walls(int walls[MAX_DIM][MAX_DIM][4])
{
	int x, y, i, total, which, test;

	test=0;

	for(x=0; x<MAX_DIM; x++)
	for(y=0; y<MAX_DIM; y++)
	 {
		total=0;	

		for(i=0; i<4; i++)
		total+=walls[x][y][i];

		if(total==3)
		 {
			for(i=0; walls[x][y][i]!=0; i++);

			which=i;

			which=left_turn(which);
			which=left_turn(which);

			if(which==M_NORTH) walls[x][y+1][which]=1;
			if(which==M_SOUTH) walls[x][y-1][which]=1;
			if(which==M_EAST) walls[x-1][y][which]=1;
			if(which==M_WEST) walls[x+1][y][which]=1;

			test=1;

			//printf("\t(%d,%d)", x, y);
		  }
	  }


	return(test);

}


/*Function to check to see if avatars are linked to the right path
we use the concept of linking avatars to chose a finish line.  they all go until they are following the same person.  the following is determined by who's bread crumb path you are on.  however, we don't all have to folling the same person's bread cumbs.  rather, we are all linked up if everyone is following an avatar who is following an avatar who is being led by the same person.  once this case is reached, we return the avatar ID of the leader.
*/
int link_check(int *link, unsigned int nAvatars)
{
	int i, j, test, linked;
	int link_test[AM_MAX_AVATAR];


	for(i=0; i<nAvatars; i++)
	 {
		test=i;

		for(j=0; j<nAvatars; j++)
		 {	
			if(test==link[test]) break;
			 test=link[test];
		  }
		link_test[i]=test;
	  }

	linked=1; test=link_test[0];

	for(i=1; i<nAvatars; i++)
	 {
		if(test!=link_test[i])
		 linked=0;
	  }

	if(linked) return(link_test[0]);

	else return(-1);

}


/*Function to determine if another avatar is following your path
We don't want to follow bread crumbs of people who are following is (such as if you arrive at a dead end and turn around to find your friend).  this function determines if the avatar who left the clue is follwing avatar whos turn it is to move, it then recognizes that it should not follow the bread crumb and makes a move indepent of the clue path.
*/
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


/*Function for some home-made graphics
This is pretty straight forward.  It used printf to print off the walls and avatars who exist in each box.  the > represents a wall to the west.  < is a wall to the east.  _ is a wall to the north, and ^ is a wall to the south.  
*/
void print_stuff(XYPOS *positions, int walls[MAX_DIM][MAX_DIM][4], unsigned int collums, unsigned int rows)
{
int i, j, k, X, Y, dude;

printf("\n\n__________________________________________________\n\n");
for(i=0; i<rows; i++)
{
	printf(" [");	
	for(j=0; j<collums; j++)
	 {	printf(" ");
		if( walls[j+1][i+1][M_NORTH] ) printf("_");
		else printf(" ");
		printf(" ");
	  } printf("]\n%2d", i);


	for(j=0; j<collums; j++)
	{
		if(walls[j+1][i+1][M_WEST]) printf(">");
		else printf(" ");		

		dude=-1;
		for(k=0; k<4; k++)
		 {
			Y=ntohl(positions[k].yPos);
			X=ntohl(positions[k].xPos);
			if(X==j && Y==i) dude=k;
		  }

		if(dude>=0) printf("%d", dude);
		else if(dude>1) printf("$");
		else printf(" ");

		if(walls[j+1][i+1][M_EAST]) printf("<");
		else printf(" ");

	  } printf("%2d\n [", i);

	for(j=0; j<collums; j++)
	 {	printf(" ");
		if( walls[j+1][i+1][M_SOUTH] ) printf("^");
		else printf(" ");
		printf(" ");
	  }
	printf("]\n");
 }
fflush(stdout);
}
			



























