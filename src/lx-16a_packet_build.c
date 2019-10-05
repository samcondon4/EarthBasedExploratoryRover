/*
 * lx-16a_packet_build.c
 *
 * Created: 07/26/2019
 * Sam Condon
 *
 * Source code to build uart command packets to send to the lx-16a
 * serial bus servo motor
 *
 */

#include <stdio.h>
#include "lx-16a_packet_build.h"

int set_parameters(Parameters* paramPtr, int paramNum_, ...){
	
	paramPtr->paramNum = paramNum_;

	if(paramNum_ > MAXPARAMS){
		printf("Please pass valid number of parameters.  Blank parameters structure returned");
		return 0;
	}
	if(paramNum_ == 0){
		unsigned char t = 0;
		paramPtr->paramNum = 0;
		paramPtr->params = &t;
	}
	else{
		va_list args;
		va_start(args, paramNum_);
	
		unsigned char params_[paramNum_];

		for(int c=0; c<paramNum_; c++)
			params_[c] = (unsigned char)(va_arg(args, int));
	
		va_end(args);

		paramPtr->params = params_;
	}


	return 1;
}

void build_command_packet(Packet* packet, Parameters params, unsigned char commandID_, unsigned char motorID_){
	/*
	printf("\n");	

	for(int i=0; i<params->paramNum; i++)
		printf("\n%02x",params->params[i]);
	printf("\n");
	*/

	int checkSumTest = 0;
	unsigned char checkSum;
	unsigned char minByte;
	unsigned char length = 3 + params.paramNum;
	unsigned char data_[length + 3]; //THIS VALUE SET TO + 4 FOR DEBUGGING/USE OF PRINTF, CHANGE THIS BACK TO THREE FOR IMPLEMENTATION	
	
	data_[0] = HEADER;
	data_[1] = HEADER;
	minByte = HEADER;

	data_[2] = motorID_;
	checkSumTest += motorID_;
	if(motorID_ < minByte)
		minByte = motorID_;

	data_[3] = length;
	checkSumTest += length;
	if(length < minByte)
		minByte = length;

	data_[4] = commandID_;
	checkSumTest += commandID_;
	if(commandID_ < minByte)
		minByte = commandID_;
	
	int index = 5;
	for(int c=0; c<params.paramNum; c++){
		if(params.params[c] < minByte)
			minByte = params.params[c];
		data_[5+c] = params.params[c];
		checkSumTest += params.params[c];
		index++;
	}
	
	checkSumTest = ~checkSumTest;
	checkSum = (unsigned char)checkSumTest;
	
	data_[index] = checkSum;
	
	packet->commandID = commandID_;
	packet->motorID = motorID_;
	packet->byteNum = length + 3;
	packet->data = data_;
		
}

void print_packet(Packet* packet){
	
	for(int c=0; c<packet->byteNum; c++){
		printf("%02x\n",packet->data[c]);
	}

}



