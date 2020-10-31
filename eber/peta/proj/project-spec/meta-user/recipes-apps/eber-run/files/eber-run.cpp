/*EBER Run:
 *
 *	Main program that EBER Compute Element (ECE)
 *	will run to control the rover
 *
 */


#include <iostream>
#include <fstream>
#include <pthread.h>
#include "MotorComChannel.h"
using namespace std;

//Defines for movement commands////
#define MASTCAM 0
#define BODY 1

#define LEFT 0 
#define RIGHT 1 
#define FORWARD 2 //corresponds to tilt up in mastcam mode and drive forward in body
#define BACK 3
///////////////////////////////////

//Data transfer defines//
#define READ_SIZE 1

//constant buffers
const char motor_uart_file[] = "/dev/ttyUL1";


//Prototypes//////////////////////
void *MoveCheck(void * ptr);

struct moveType{
	int mode;
	int move;
};



int main(int argc, char *argv[])
{

	MotorComChannel MotorCom(motor_uart_file, B115200, false); //Channel to talk to motors
	std::ifstream CommandCom("/dev/ttyPS0");
	char input;


	while(CommandCom >> input){
		if(input != 0x00){
			printf("\n read data: %02x \n", input);
		}
		switch((int)input){
			case (int)'d':
				MotorCom.eber_move(MASTCAM, RIGHT);
			break;

			case (int)'a':
				MotorCom.eber_move(MASTCAM, LEFT);
			break;

			case (int)'w':
				MotorCom.eber_move(MASTCAM, UP);
			break;

			case (int)'s':
				MotorCom.eber_move(MASTCAM, DOWN);
			break;

			case (int)'x':
				MotorCom.eber_move(MASTCAM, STOP);
			break;
		}
		CommandCom.clear();
		CommandCom.seekg(0);
	}
	
	return 0;
}






