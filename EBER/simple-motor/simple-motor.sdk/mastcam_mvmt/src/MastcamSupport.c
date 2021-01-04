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

#define MOVE_BUF_SIZE 10


//private function prototypes
static int UartLiteSetupInterruptSystem(XScuGic *Intc, XUartLite *UartLitePtr, int IntrId);
void SendHandler(void *CallbackRef, unsigned int EventData);
void RecvHandler(void *CallbackRef, unsigned int EventData);

//globals for count of send/receive data volume
static volatile int TotalReceivedCount;
static volatile int TotalSentCount;

//Fixed buffers for Mast movement
static unsigned char left_spin_buf[MOVE_BUF_SIZE] = {0x55, 0x55, 0x01, 0x07, 0x1d, 0x01, 0x00, 0x2c, 0x01, 0xac};
static unsigned char right_spin_buf[MOVE_BUF_SIZE] = {0x55, 0x55, 0x01, 0x07, 0x1d, 0x01, 0x00, 0xd4, 0xfe, 0x07};
static unsigned char up_spin_buf[MOVE_BUF_SIZE] = {0x55, 0x55, 0x02, 0x07, 0x1d, 0x01, 0x00, 0x2c, 0x01, 0xab};
static unsigned char down_spin_buf[MOVE_BUF_SIZE] = {0x55, 0x55, 0x02, 0x07, 0x1d, 0x01, 0x00, 0xd4, 0xfe, 0x06};
static unsigned char one_stop_buf[MOVE_BUF_SIZE] = {0x55, 0x55, 0x01, 0x07, 0x1d, 0x01, 0x00, 0x00, 0x00, 0xd9};
static unsigned char two_stop_buf[MOVE_BUF_SIZE] = {0x55, 0x55, 0x02, 0x07, 0x1d, 0x01, 0x00, 0x00, 0x00, 0xd8};


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
			while(TotalSentCount != MOVE_BUF_SIZE){
				//pend
			}
			TotalSentCount = 0;
		break;

		case(DOWN):
			XUartLite_Send(&(mastcam->uart_dev), down_spin_buf, MOVE_BUF_SIZE);
			while(TotalSentCount != MOVE_BUF_SIZE){
				//pend
			}
			TotalSentCount = 0;
		break;

		case(LEFT):
			XUartLite_Send(&(mastcam->uart_dev), left_spin_buf, MOVE_BUF_SIZE);
			while(TotalSentCount != MOVE_BUF_SIZE){
				//pend
			}
			TotalSentCount = 0;
		break;

		case(RIGHT):
			XUartLite_Send(&(mastcam->uart_dev), right_spin_buf, MOVE_BUF_SIZE);
			while(TotalSentCount != MOVE_BUF_SIZE){
				//pend
			}
			TotalSentCount = 0;
		break;

		case(STOP):
			XUartLite_Send(&(mastcam->uart_dev), one_stop_buf, MOVE_BUF_SIZE);
			while(TotalSentCount != MOVE_BUF_SIZE){
				//pend
			}
			TotalSentCount = 0;

			XUartLite_Send(&(mastcam->uart_dev), two_stop_buf, MOVE_BUF_SIZE);
			while(TotalSentCount != MOVE_BUF_SIZE){
				//pend
			}
			TotalSentCount = 0;

			mastcam->move_state = STOP;

		break;

	}


	return ret;

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

