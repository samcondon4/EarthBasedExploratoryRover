/*
 * Header file for packet_build.c
 *
 * Create UART packet to be sent to the lx-16a servo
 *
 * Sam Condon
 * 07/24/2019
 */

#ifndef LX16A_PACKET_BUILD
#define LX16A_PACKET_BUILD


#include <stdarg.h>

#define HEADER 0x55

#define MAXPARAMS 4

//List of commands requiring more than 8 bytes
#define SERVO_MOVE_TIME_WRITE 1
#define SERVO_MOVE_TIME_WAIT_WRITE 7
#define SERVO_ANGLE_LIMIT_WRITE 20
#define SERVO_VIN_LIMIT_WRITE 22
#define SERVO_OR_MOTOR_MODE_WRITE 29

//List of commands requiring less than 8 bytes
#define SERVO_MOVE_START 11

/*
 * Parameters structure
 *
 * paramNum: number of parameters to send in packet
 * params: holds parameter data w/ at most 4 paramters
 */
typedef struct Parameters{

	unsigned char paramNum;
	unsigned char* params;

}Parameters;

/*
 * Packet structure
 *
 * commandID: command this packet is associated with
 * motorID: motor the command will address
 * paramNum: number of parameters to be sent
 * byteNum: total number of bytes to be sent
 * params: pointer to array of parameters
 * data: pointer to array of all the data
 */
typedef struct Packet{

	unsigned char commandID;
	unsigned char motorID;
	unsigned char byteNum;
	unsigned char* data;

}Packet;

/*
 * Function definitions
 *
 * set_parameters: set parameters of command to be written
 *
 * build_command_packet: build a packet structure
 *
 * print_packet: print the data values of a packet structure
 */
int set_parameters(Parameters* params, int paramNum_, ...);
void build_command_packet(Packet* packet, Parameters params, unsigned char commandID, unsigned char motorID);
void print_packet(Packet* packet);

#endif

