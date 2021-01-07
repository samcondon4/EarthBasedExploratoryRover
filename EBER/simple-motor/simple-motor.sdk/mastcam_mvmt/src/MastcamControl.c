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
#include "stdlib.h"
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
typedef enum {RUNNING, REPROGRAM} RunState;


//System States//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************
 * Program:
 * 		This state create the list of mastcam programmed states.
 *
 * return:
 * 		returns a ProgramState enumerator that tells main how
 * 		to respond. Either ALLOC or DONE should be returned.
 * 		When ALLOC is returned, main allocates a new mastcam
 * 		state and adds it to the list of states. When DONE
 * 		is returned, main transitions states to begin moving
 * 		through the newly programmed states.
 **************************************************************/
ProgramState Program(MainHw *Main_Hw, Mastcam *Mast, Mastcam_State *CurState, int *StateNum);
void Program_position(MainHw *MainHw, Mastcam *Mast, Mastcam_State *CurState);
void Program_channel(MainHw *MainHw, Mastcam *Mast, Mastcam_State *CurState);
void Program_duration(MainHw *MainHw, Mastcam *Mast, Mastcam_State *CurState);

/*****************************************************************
 * Run:
 * 		This state runs through the sequence of mastcam states
 * 		that were programmed by Program.
 *
 ******************************************************************/
RunState Run(MainHw *Main_Hw, Mastcam *Mast, Mastcam_State *CurState, int StateNum);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	RunState Run_State;
	int status;
	///////////////////////////////////////

	//user input variables
    //unsigned char rec_byte;
    //int rec_cnt;


    //debug print flags//
    char main_print;

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

    CurState = &Head;
    Tail = &Head;
    StateNum = 0;
    SystemState = PROGRAM;
    main_print = 0;
    /////////////////////////////////////////////////////////////////



    while(1){

    	switch(SystemState){
			case(PROGRAM):
				if(main_print == 0){
					xil_printf("\n\nSystemState: PROGRAM \n\r");
					main_print = 1;
				}
				ProgState = Program(&Main_Hw, &Mast, CurState, &StateNum);
				switch(ProgState){
					case(ALLOC):
						//Allocate new Mastcam state and append to list
						if(StateNum == 0){
							//current state is head. No need to allocate a new state yet.
						}
						else{
							CurState = (Mastcam_State *)malloc(sizeof(Mastcam_State));
							Tail->next_state = CurState;
							CurState->prev_state = Tail;
							Tail = CurState;

						}
					break;

					case(PROG_STATE):
						//do nothing here, allow Program to finish populating state data
					break;

					case(DONE):
						SystemState = RUN;
						main_print = 0;
					break;
				}
			break;

			case(RUN):
				if(main_print == 0){
					xil_printf("\n\nSystemState: RUN \n\r");
					main_print = 1;
				}
				Run_State = Run(&Main_Hw, &Mast, &Head, StateNum);
				switch(Run_State){
					case(RUNNING):
						//do nothing, system is just running
					break;

					case(REPROGRAM):
						SystemState = PROGRAM;
						main_print = 0;
						CurState = Head;
						for(int i = 0; i < StateNum - 1; i++){
							CurState = CurState->next;
						}

						StateNum = 0;
					break;
				}
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
ProgramState Program(MainHw *Main_Hw, Mastcam *Mast, Mastcam_State *CurState, int *StateNum){

	int rec_cnt = 0;
	unsigned char rec_byte;

	char prog_flag;

	static ProgramState prog_state = ALLOC;
	static ProgramState prog_state_next = ALLOC;

	prog_flag = 0;
	switch(prog_state_next){

		case(ALLOC):
			//next state is always PROG_STATE from ALLOC
			prog_state = ALLOC;
			prog_state_next = PROG_STATE;
		break;

		case(PROG_STATE):
			prog_state = PROG_STATE;
			if(*StateNum == 0){
				prog_flag = 1;
			}
			else{
				xil_printf("\n Program another state? (y) for yes, (n) to begin sequencing between states. \n\r");
				//wait for user to input data
				while(rec_cnt == 0){
					timer_pend(1);
					rec_cnt = XUartPs_Recv(&(Main_Hw->ps_uart), &rec_byte, 16);
				}

				switch(rec_byte){
					case(y):
						prog_flag = 1;
					break;

					case(n):
						prog_flag = 0;
						prog_state_next = DONE;
					break;

					default:
						//user did not enter meaningful data so remain in this state.
						prog_state_next = PROG_STATE;
					break;
				}
			}

			if(prog_flag == 1){
				*StateNum += 1;
				xil_printf("\n State number: %i \n\r Program sequence beginning... \n\r", *StateNum);
				Program_position(Main_Hw, Mast, CurState);
				//Program_channel(Main_Hw, Mast, CurState);
				//Program_duration(Main_Hw, Mast, CurState);
			}

		break;

		case(DONE):
			prog_state = DONE;
			prog_state_next = ALLOC;
			xil_printf("programming sequence complete! \n\r");
		break;
	}

	return prog_state;
}

void Program_position(MainHw *Main_Hw, Mastcam *Mast, Mastcam_State *Curstate){
	xil_printf("program position! \n\r");
}



RunState Run(MainHw *Main_Hw, Mastcam *Mast, Mastcam_State *CurState, int StateNum){

	static RunState run_state;
	static RunState run_state_next;

	static char print_flag;
	unsigned char rec_byte;

	int rec_cnt = 0;

	switch(run_state_next){
		case(RUNNING):
			run_state = RUNNING;
			if(print_flag == 0){
				xil_printf("Sequence Running! Press (r) to set a new sequence \n\r");
				print_flag = 1;
			}
			//get user input
			while(rec_cnt == 0){
				timer_pend(1);
				rec_cnt = XUartPs_Recv(&(Main_Hw->ps_uart), &rec_byte, 16);
			}

			switch(rec_byte){
				case(r):
					run_state_next = REPROGRAM;
				break;

				default:
					run_state_next = RUNNING;
				break;
			}
		break;

		case(REPROGRAM):
			run_state = REPROGRAM;
			run_state_next = RUNNING;
			print_flag = 0;
		break;
	}

	return run_state;
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
