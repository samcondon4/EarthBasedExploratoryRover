/*
 *movements.h
 *
 *Header defining basic movement functions for the rover
 *
 *Sam Condon
 *08/14/2019
 */

#ifndef MOVEMENTS
#define MOVEMENTS

#include "lx-16a_packet_build.h"

typedef struct Movement{
	unsigned char movementID;
	unsigned char packetNum;
	Packet* data;
}Movement;

/*
 *forward: drive forward
 *custom: create custom movement, useful for testing individual motors
 *
 */

void forward(Movement* movement);
void custom(Movement* movement);

#endif
