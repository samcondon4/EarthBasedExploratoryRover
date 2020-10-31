#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include "uart.h"

#define GET_LOW_BYTE(A) (unsigned char)(A)
//Macro function  get lower 8 bits of A
#define GET_HIGH_BYTE(A) (unsigned char)(A >> 8)
//Macro function  get higher 8 bits of A

#define LOBOT_SERVO_FRAME_HEADER         0x55
#define LOBOT_SERVO_ID_WRITE             13
#define BUF_LEN 			 7

unsigned char LobotCheckSum(unsigned char buf[])
{
	unsigned char i;
	unsigned short temp = 0;
	for (i = 2; i < buf[3] + 2; i++) {
		temp += buf[i];
	}
	temp = ~temp;
	i = (unsigned char)temp;
	return i;
}


int main(int argc, char **argv)
{
	struct UartDevice dev;
	int rc;
	int in_arg;
	unsigned char oldID, newID;
	unsigned char buf[BUF_LEN];
	
	//Grab input arguments////////////////////
	if(argc != 3){
		printf("Incorrect input arguments provided. Expected motor-start <motor-id> <speed>");
		rc = -1;
	}
	else{
		in_arg = atoi(argv[1]);
		if(in_arg > 255 || in_arg < 0){
			printf("Improper ID value given.");
		}
		oldID = (unsigned char)in_arg;	
		
		in_arg = atoi(argv[2]);
		if(in_arg > 255 || in_arg < 0){
			printf("Improper ID value given.");
		}
		newID = (unsigned char)in_arg;	
	}
	dev.filename = "/dev/ttyUL1";
	dev.rate = B115200;

	rc = uart_start(&dev, false);

	if(rc == 0){
		buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
		buf[2] = oldID;
		buf[3] = 4;
		buf[4] = LOBOT_SERVO_ID_WRITE;
		buf[5] = newID;
		buf[6] = LobotCheckSum(buf);
	}
	else{
		printf("ttyUL1 Initialization Failed.");
	}

	uart_writes(&dev, buf, BUF_LEN);

	uart_stop(&dev);

	return 0;
}




