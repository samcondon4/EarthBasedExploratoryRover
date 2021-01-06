/*
 * MastcamSupport.c
 *
 *  Created on: Jan 3, 2021
 *      Author: thoma
 */

#include "MastcamSupport.h"
#include "xil_exception.h"

#define MASTCAM_UART_DEV_ID			XPAR_UARTLITE_0_DEVICE_ID
#define MASTCAM_UART_DEV_INT_ID		XPAR_FABRIC_UARTLITE_0_VEC_ID
#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

//Defines to build Mast control buffers///////////////////////////
#define MOVE_BUF_SIZE 			10
#define READ_POS_BUF_SIZE_TX	6
#define READ_POS_BUF_SIZE_RX	8

#define PAN_MOTOR_ID				0x01
#define TILT_MOTOR_ID				0x02

#define LX16A_HEADER 				0x55
#define SERVO_POS_READ				0x1c
#define SERVO_OR_MOTOR_MODE_WRITE	0x1d
///////////////////////////////////////////////////////////////////

//private function prototypes
static int UartLiteSetupInterruptSystem(XScuGic *Intc, XUartLite *UartLitePtr, int IntrId);
void SendHandler(void *CallbackRef, unsigned int EventData);
void RecvHandler(void *CallbackRef, unsigned int EventData);
void tx_pend(int buf_size);
void rx_pend(int buf_size);

//globals for count of send/receive data volume
static volatile int TotalReceivedCount;
static volatile int TotalSentCount;

//Fixed buffers for Mast position control
static unsigned char left_spin_buf[MOVE_BUF_SIZE] = {LX16A_HEADER, LX16A_HEADER, PAN_MOTOR_ID, 0x07, SERVO_OR_MOTOR_MODE_WRITE, 0x01, 0x00, 0x2c, 0x01, 0xac};
static unsigned char right_spin_buf[MOVE_BUF_SIZE] = {LX16A_HEADER, LX16A_HEADER, PAN_MOTOR_ID, 0x07, SERVO_OR_MOTOR_MODE_WRITE, 0x01, 0x00, 0xd4, 0xfe, 0x07};
static unsigned char up_spin_buf[MOVE_BUF_SIZE] = {LX16A_HEADER, LX16A_HEADER, TILT_MOTOR_ID, 0x07, SERVO_OR_MOTOR_MODE_WRITE, 0x01, 0x00, 0x2c, 0x01, 0xab};
static unsigned char down_spin_buf[MOVE_BUF_SIZE] = {LX16A_HEADER, LX16A_HEADER, TILT_MOTOR_ID, 0x07, SERVO_OR_MOTOR_MODE_WRITE, 0x01, 0x00, 0xd4, 0xfe, 0x06};
static unsigned char pan_stop_buf[MOVE_BUF_SIZE] = {LX16A_HEADER, LX16A_HEADER, PAN_MOTOR_ID, 0x07, SERVO_OR_MOTOR_MODE_WRITE, 0x01, 0x00, 0x00, 0x00, 0xd9};
static unsigned char tilt_stop_buf[MOVE_BUF_SIZE] = {LX16A_HEADER, LX16A_HEADER, TILT_MOTOR_ID, 0x07, SERVO_OR_MOTOR_MODE_WRITE, 0x01, 0x00, 0x00, 0x00, 0xd8};

static unsigned char pan_read_pos_buf[READ_POS_BUF_SIZE_TX] = {LX16A_HEADER, LX16A_HEADER, PAN_MOTOR_ID, 0x03, SERVO_POS_READ, 0xdf};
static unsigned char tilt_read_pos_buf[READ_POS_BUF_SIZE_TX] = {LX16A_HEADER, LX16A_HEADER, TILT_MOTOR_ID, 0x03, SERVO_POS_READ, 0xde};

//Initialize uart device that will control mastcam actuators as well as interrupt controller
int mastcam_init(XScuGic *Intc, Mastcam *mastcam){

	int ret;
	int Status;

	ret = XST_SUCCESS;

	//Initialize the UartLite driver so that it is ready to use.
	Status = XUartLite_Initialize(&(mastcam->uart_dev), MASTCAM_UART_DEV_ID);
	if (Status != XST_SUCCESS) {
		ret = XST_FAILURE;
	}

	//Perform a self-test to ensure that the hardware was built correctly
	Status = XUartLite_SelfTest(&(mastcam->uart_dev));
	if (Status != XST_SUCCESS) {
		ret = XST_FAILURE;
	}

	Status = UartLiteSetupInterruptSystem(Intc, &(mastcam->uart_dev), MASTCAM_UART_DEV_INT_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	return ret;
}

//Move given mastcam instance in specified direction at fixed speed
int mastcam_move(Mastcam *mastcam, int direction){

	int ret;

	ret = XST_SUCCESS;

	mastcam->move_state |= direction;

	switch(direction){

		case(UP):
			XUartLite_Send(&(mastcam->uart_dev), up_spin_buf, MOVE_BUF_SIZE);
			tx_pend(MOVE_BUF_SIZE);
		break;

		case(DOWN):
			XUartLite_Send(&(mastcam->uart_dev), down_spin_buf, MOVE_BUF_SIZE);
			tx_pend(MOVE_BUF_SIZE);
		break;

		case(LEFT):
			XUartLite_Send(&(mastcam->uart_dev), left_spin_buf, MOVE_BUF_SIZE);
			tx_pend(MOVE_BUF_SIZE);
		break;

		case(RIGHT):
			XUartLite_Send(&(mastcam->uart_dev), right_spin_buf, MOVE_BUF_SIZE);
			tx_pend(MOVE_BUF_SIZE);
		break;

		case(STOP):
			XUartLite_Send(&(mastcam->uart_dev), pan_stop_buf, MOVE_BUF_SIZE);
			tx_pend(MOVE_BUF_SIZE);

			XUartLite_Send(&(mastcam->uart_dev), tilt_stop_buf, MOVE_BUF_SIZE);
			tx_pend(MOVE_BUF_SIZE);

			mastcam->move_state = STOP;

		break;

	}


	return ret;

}

//Update position values in the passed mastcam structure
int mastcam_getpos(Mastcam *mastcam){

	int azimuth_pos;
	int zenith_pos;

	unsigned char recv_buf[5];

	XUartLite_Send(&(mastcam->uart_dev), pan_read_pos_buf, READ_POS_BUF_SIZE_TX);
	tx_pend(READ_POS_BUF_SIZE_TX);

	XUartLite_Recv(&(mastcam->uart_dev), recv_buf, READ_POS_BUF_SIZE_RX);
	rx_pend(READ_POS_BUF_SIZE_RX);

	for(int i = 0; i < READ_POS_BUF_SIZE_RX; i++){
		xil_printf("position data at index %i: %02x \n\r", i, recv_buf[i]);
	}

	xil_printf("\n\r");

	return XST_SUCCESS;
}


static int UartLiteSetupInterruptSystem(XScuGic *Intc, XUartLite *UartLitePtr, int IntrId)
{
	int Status;

	XScuGic_SetPriorityTriggerType(Intc, IntrId, 0x90, 0x03);

	Status = XScuGic_Connect(Intc, IntrId, (Xil_ExceptionHandler)XUartLite_InterruptHandler,
							 UartLitePtr);
	if(Status != XST_SUCCESS){
		return Status;
	}

	XScuGic_Enable(Intc, IntrId);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, Intc);

	XUartLite_SetSendHandler(UartLitePtr, SendHandler, UartLitePtr);
	XUartLite_SetRecvHandler(UartLitePtr, RecvHandler, UartLitePtr);

	XUartLite_EnableInterrupt(UartLitePtr);

	return XST_SUCCESS;
}


void SendHandler(void *CallbackRef, unsigned int EventData){
	TotalSentCount = EventData;
}

void RecvHandler(void *CallbackRef, unsigned int EventData){
	TotalReceivedCount = EventData;
}

//This function blocks until UartLite transmit has completed
void tx_pend(int buf_size){
	while(TotalSentCount != buf_size){
		//pend
	}
	TotalSentCount = 0;
}

void rx_pend(int buf_size){
	while(TotalReceivedCount != buf_size){
		//pend
	}
	TotalReceivedCount = 0;
}

