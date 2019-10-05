/*
 *movements.c
 *
 *Functions enabling basic rover movement
 *
 *Sam Condon
 *08/14/2019
 */

#include <stdio.h>
#include "movements.h"

//MOVEMENT ID'S//////
#define CUSTOM 0
#define FORWARD 1
////////////////////


void forward(Movement* movement){
	int test;
	
	movement->movementID = FORWARD;
	movement->packetNum = 7;

	Packet fwdData[7];
	for(int t=0; t<6; t++){
       		Parameters fwdParams;	
		test = set_parameters(&fwdParams, 4, 0, 100, 0, 255); 
		build_command_packet(&(fwdData[t]), fwdParams, SERVO_MOVE_TIME_WAIT_WRITE, t+1);
	}
	
	Parameters fwdParams;
	test = set_parameters(&fwdParams, 0);
	build_command_packet(&fwdData[6], fwdParams, SERVO_MOVE_START, 0xfe);

	movement->data = fwdData;
	
	//write fwdData
}	

