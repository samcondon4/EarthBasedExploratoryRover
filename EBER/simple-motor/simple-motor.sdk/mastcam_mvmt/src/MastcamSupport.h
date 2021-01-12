/* MastcamSupport.h
 *
 *  Header file for Mastcam movement functions
 *
 ***********************************************/

#include "xuartlite.h"
#include "xparameters.h"
#include "xscugic.h"

//Movements for use in mastcam_move();
#define STOP 0
#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8
//////////////////////////////////////

//motor id's for use in mastcam_moveto();
#define PAN 1
#define TILT 2
/////////////////////////////////////////

//Motor modes///////////////////
#define POSITION_CONTROL 	0
#define MOTOR_CONTROL		1
/////////////////////////////////

typedef enum {RGB, RED, GREEN, BLUE, EDGE} CamChannel;

typedef struct{
	int move_state;
	int pan_motor_mode;
	int tilt_motor_mode;
	CamChannel channel;
	XUartLite uart_dev;
} Mastcam;

typedef struct{
	signed short azimuth_pos;
	signed short zenith_pos;
	CamChannel channel;
	unsigned int duration;
}Mastcam_State_Data;

typedef struct Mastcam_State{
	Mastcam_State_Data state_data;
	struct Mastcam_State *next_state;
}Mastcam_State;

//Initialize uart device that will control mastcam actuators
int mastcam_init(XScuGic *Intc, Mastcam *mastcam);

//Move given mastcam instance in specified direction at fixed speed
int mastcam_move(Mastcam *mastcam, int direction);

void mastcam_moveto(Mastcam *mastcam, char motor_id, short position);

//Get position of mastcam head
int mastcam_getpos(Mastcam *mastcam, short *azimuth_pos, short *zenith_pos);
