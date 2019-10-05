#include <stdio.h>
#include "lx-16a_packet_build.h"
#include "movements.h"

int main(){

	Parameters params;
	Packet pack;

	set_parameters(&params, 4, 0x01, 0x00, 0xe8, 0x03);
	build_command_packet(&pack, params, SERVO_OR_MOTOR_MODE_WRITE, 1);

	for(int i=0; i<pack.byteNum; i++){
		printf("%02x\n",pack.data[i]);
	}

	return 0;
}

