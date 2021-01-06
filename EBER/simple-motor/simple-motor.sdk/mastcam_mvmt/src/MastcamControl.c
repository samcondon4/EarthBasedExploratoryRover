/*************************************
 * Mastcam Control:
 *
 * 	Main EBER Mastcam control demo application
 *
 *
 *  Sam Condon
 **************************************/


#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "XUartPs.h"
#include "xtmrctr.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "MastcamSupport.h"

//Defines for AXI Timer parameters
#define TMRCTR_DEVICE_ID		XPAR_TMRCTR_0_DEVICE_ID
#define TMRCTR_INTERRUPT_ID		XPAR_FABRIC_TMRCTR_0_VEC_ID
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_CNTR_0				0

#define RESET_VALUE	 0xFFcf0000

//Ascii values of keys
#define a 0x61 //left
#define d 0x64 //right
#define s 0x73 //down
#define w 0x77 //up
#define r 0x72 //reprogram
#define y 0x79 //yes
#define n 0x6e

//Contains structures for all hw devices controlled by main
typedef struct{
	XUartPs ps_uart;
	XTmrCtr tmr_ctr;
	XScuGic int_cntrl;
} MainHw;

//State machine state definitions
typedef enum {PROGRAM, RUN} SysState;
typedef enum {ALLOC, PROG_STATE, DONE} ProgramState;
//typedef enum {} RunState;

//function prototypes/////////////////////////////////////////////////
int MainHwInit(MainHw* main_hw);

int TmrCtrSetupIntrSystem(XScuGic* IntcInstancePtr,
				XTmrCtr* TmrCtrInstancePtr,
				u16 DeviceId,
				u16 IntrId,
				u8 TmrCtrNumber);

void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);
void timer_pend(int pend_count);
//////////////////////////////////////////////////////////////////////

//System States///////////////////////////////////////////////////////
ProgramState Program(SysState *SystemState, MainHw *Main_Hw, Mastcam *Mast, Mastcam_State *CurState, int StateNum);
void Run(SysState *SystemState, MainHw *Main_Hw, Mastcam *Mast, Mastcam_State *CurState, int StateNum);
//////////////////////////////////////////////////////////////////////

//Global flag for timer counter
volatile int TimerExpired;


int main()
{
	//System Control variables////////////
	Mastcam Mast;
	Mastcam_State Head;
	Mastcam_State *Tail;
	Mastcam_State *CurState;
	int StateNum;
	MainHw Main_Hw;
	SysState SystemState;
	ProgramState ProgState;
	//RunState RState;
	///////////////////////////////////////

    unsigned char rec_byte;
    int rec_cnt;
    int status;

    //System Initialization/////////////////////////////////////////
    init_platform();
    //Initialize exception table
    Xil_ExceptionInit();
    //Enable non-critical exceptions
    Xil_ExceptionEnable();

    status = MainHwInit(&Main_Hw);
        if(status != XST_SUCCESS){
        	return XST_FAILURE;
        }
    mastcam_init(&(Main_Hw.int_cntrl), &Mast);

    StateNum = 0;
    CurState = &Head;
    Tail = &Head;
    /////////////////////////////////////////////////////////////////

    while(1){

    	switch(SystemState){
			case(PROGRAM):
				ProgState = Program(&SystemState,&Main_Hw, &Mast,CurState,StateNum);
				switch(ProgState){
					case(ALLOC):
						//Allocate new Mastcam state and append to list
					break;

					case(PROG_STATE):
						//do nothing here, allow Program to finish populating state data
					break;

					case(DONE):
						SystemState = RUN;
					break;
				}
			break;

			case(RUN):

			break;
    	}

    	//Program/////////////////////////////////////////////////////////////////////
    	/*
    	timer_pend(1);

    	rec_cnt = XUartPs_Recv(&(Main_Hw.ps_uart), &rec_byte, 16);

    	if(rec_cnt != 0){

    		if(rec_byte != '\r'){
        		switch(rec_byte){
    				case(a):
    					if(!(Mast.move_state & LEFT))
    						mastcam_move(&Mast,LEFT);
    				break;

    				case(d):
    					if(!(Mast.move_state & RIGHT))
    						mastcam_move(&Mast,RIGHT);
    				break;

    				case(s):
    					if(!(Mast.move_state & DOWN))
    						mastcam_move(&Mast,DOWN);
    				break;

    				case(w):
    					if(!(Mast.move_state & UP))
    						mastcam_move(&Mast,UP);
    				break;
        		}
    		}
    		else{
    			mastcam_getpos(&Mast);
    		}

    	}
    	else if(Mast.move_state != STOP){
    		mastcam_move(&Mast,STOP);
    	}
		*/
    	////////////////////////////////////////////////////////////////////////////

    	//Run////////////////////////////////////////////////////////////////////////

    	////////////////////////////////////////////////////////////////////////////

    }

	 cleanup_platform();
	 return 0;

}


//Program movement state machine
ProgramState Program(SysState *SystemState, MainHw *Main_Hw, Mastcam *Mast, Mastcam_State *CurState, int StateNum){

	ProgramState prog_state;
	//do the stuff

	return prog_state;
}


int MainHwInit(MainHw *main_hw){
	XUartPs_Config *ps_uart_cfg;
	XScuGic_Config *int_cntrl_cfg;

	int status;

	//Initialize gic for hardware interrupt control///////////////////////////////
	int_cntrl_cfg = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if(NULL == int_cntrl_cfg){
		return XST_FAILURE;
	}
	status = XScuGic_CfgInitialize(&(main_hw->int_cntrl), int_cntrl_cfg,
			int_cntrl_cfg->CpuBaseAddress);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/////////////////////////////////////////////////////////////////////////////


	//Initialize ps uart for interaction w/ user////////////////////////////////
	ps_uart_cfg = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
	status = XUartPs_CfgInitialize(&(main_hw->ps_uart), ps_uart_cfg, XPAR_PS7_UART_1_BASEADDR);
	if(status != XST_SUCCESS){
		xil_printf("PS Uart initialization failed! \n");
		return XST_FAILURE;
	}
	////////////////////////////////////////////////////////////////////////////


	//Initialize Timer counter for timing uart sampling//////////////////////////////////////////////////////
	status = XTmrCtr_Initialize(&(main_hw->tmr_ctr), TMRCTR_DEVICE_ID);
	if(status != XST_SUCCESS){
		return XST_FAILURE;
	}

	status = XTmrCtr_SelfTest(&(main_hw->tmr_ctr), TIMER_CNTR_0);
	if(status != XST_SUCCESS){
		return XST_FAILURE;
	}

	status = TmrCtrSetupIntrSystem(&(main_hw->int_cntrl), &(main_hw->tmr_ctr), TMRCTR_DEVICE_ID,
								   TMRCTR_INTERRUPT_ID, TIMER_CNTR_0);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XTmrCtr_SetOptions(&(main_hw->tmr_ctr), TIMER_CNTR_0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&(main_hw->tmr_ctr), TIMER_CNTR_0, RESET_VALUE);
	XTmrCtr_Start(&(main_hw->tmr_ctr), TIMER_CNTR_0);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////


	return XST_SUCCESS;

}


int TmrCtrSetupIntrSystem(XScuGic* IntcInstancePtr,
				 XTmrCtr* TmrCtrInstancePtr,
				 u16 DeviceId,
				 u16 IntrId,
				 u8 TmrCtrNumber)
{
	int Status;



	XScuGic_SetPriorityTriggerType(IntcInstancePtr, IntrId,
				0xA0, 0x3);

	/*
	* Connect the interrupt handler that will be called when an
	* interrupt occurs for the device.
	*/
	Status = XScuGic_Connect(IntcInstancePtr, IntrId,
			 (Xil_ExceptionHandler)XTmrCtr_InterruptHandler,
			 TmrCtrInstancePtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}


	XScuGic_Enable(IntcInstancePtr, IntrId);


	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
					(Xil_ExceptionHandler)
					XScuGic_InterruptHandler,
					IntcInstancePtr);


	XTmrCtr_SetHandler(TmrCtrInstancePtr, TimerCounterHandler,
						   TmrCtrInstancePtr);

	return XST_SUCCESS;
}


void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber)
{
	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;


	if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
		TimerExpired = 1;
	}

}

//Pend for pend_count timer interrupts
void timer_pend(int pend_count){
	while(TimerExpired < pend_count){
		//pend
	}
	TimerExpired = 0;
}
