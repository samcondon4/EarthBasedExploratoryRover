/*
 * interrupt_config.c
 *
 *  Created on: Jul 16, 2019
 *      Author: sammyc
 */
/* Contains high level functions to ease use of Xilinx interrupt API
 * Created interrupt structure for potential to dynamically update an ISR
 */

#include "interrupt_config.h"

//Setup interrupt structure and call initialization functions
void interrupt_setup(Interrupt* IntrPtr, XScuGic* GicInstancePtr, u32 IntId_, void* CallBack_,
					 void (*interrupt_handler_)(void* CallBack)){

		IntrPtr->IntId = IntId_;
        IntrPtr->CallBack = CallBack_;
        IntrPtr->interrupt_handler = interrupt_handler_;

        scugic_initialize(GicInstancePtr, IntrPtr);
        arminterrupt_setup(GicInstancePtr);

}

//Connect low level GIC Interrupt Handler to exception interface on processor
void arminterrupt_setup(XScuGic* GicInstancePtr){
        Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, GicInstancePtr);
        Xil_ExceptionEnable();
}

//Configure GIC controller and enable specific interrupt
void scugic_initialize(XScuGic* GicInstancePtr, Interrupt* IntrPtr){
		XScuGic_Config* GicCfgPtr;
		GicCfgPtr = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
        XScuGic_CfgInitialize(GicInstancePtr, GicCfgPtr, GicCfgPtr -> CpuBaseAddress);
        XScuGic_Connect(GicInstancePtr, IntrPtr -> IntId, IntrPtr -> interrupt_handler, IntrPtr -> CallBack);
        XScuGic_Enable(GicInstancePtr, IntrPtr -> IntId);
}

