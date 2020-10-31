#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define LOOPBACK_FORMAT "loopback: %s\r\n"
#define LOOPBACK_FORMAT_LEN strlen(LOOPBACK_FORMAT)
#define MAX_READ_SIZE 235
#define MAX_LOOPBACK_SIZE MAX_READ_SIZE + LOOPBACK_FORMAT_LEN

#define GET_LOW_BYTE(A) (unsigned char)(A)
//Macro function  get lower 8 bits of A
#define GET_HIGH_BYTE(A) (unsigned char)(A >> 8)
//Macro function  get higher 8 bits of A

#define LOBOT_SERVO_FRAME_HEADER         0x55
#define LOBOT_SERVO_OR_MOTOR_MODE_WRITE  29



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



int main(int argc, char** argv){

	int rc;
	int in_arg;
	unsigned char motor_id, speed_l, speed_h;

	unsigned char buf[10];


	//Grab input arguments////////////////////
	if(argc != 3){
		printf("Incorrect input arguments provided. Expected motor-start <motor-id> <speed>");
		rc = -1;
	}
	else{
		in_arg = atoi(argv[2]);
		if(in_arg > 100 || in_arg < -100){
			printf("argument 2 too large.");
		}
		else{
			in_arg = in_arg * 10;
			speed_l = GET_LOW_BYTE(in_arg);
			speed_h = GET_HIGH_BYTE(in_arg);
		}

		in_arg = atoi(argv[1]);

		if(in_arg > 255){
			printf("argument 1 too large.");
		}
		else{
			motor_id = (unsigned char)(in_arg);
		}
	}
	///////////////////////////////////////////////

	


	if(rc == 0){
		buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
		printf("\n %02x, %02x \n",buf[0],buf[1]);
		buf[2] = motor_id;
		buf[3] = 7;
		buf[4] = LOBOT_SERVO_OR_MOTOR_MODE_WRITE;
		buf[5] = 1;
		buf[6] = 0;
		buf[7] = speed_l;
		buf[8] = speed_h;
		buf[9] = LobotCheckSum(buf);
	}


	for(int i = 0; i < 10; i++){
		printf("\nbuffer data: %02x\n",buf[i]);
	}





}

