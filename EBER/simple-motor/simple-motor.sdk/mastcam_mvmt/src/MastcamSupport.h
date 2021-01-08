/* MastcamSupport.h
 *
 *  Header file for Mastcam movement functions
 *
 ***********************************************/

#include "xuartlite.h"
#include "xparameters.h"
#include "xscugic.h"

#define STOP 0
#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8



typedef struct{
	int azimuth_pos;
	int zenith_pos;
	int move_state;
	XUartLite uart_dev;
} Mastcam;

typedef enum {RGB, RED, GREEN, BLUE, EDGE} cam_channel;

typedef struct{
	signed short azimuth_pos;
	signed short zenith_pos;
	cam_channel channel;
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

//Get position of mastcam head
int mastcam_getpos(Mastcam *mastcam);
