*Header file for configuring interrupts on the Xilinx Zynq 7000
 *
 * Interrupt ID's are copied below for reference
 *
 * Created on: Jul 16, 2019
 *     Author: Sam Condon
 */

#ifndef SRC_INTERRUPT_CONFIG_H_
#define SRC_INTERRUPT_CONFIG_H_


#include "xscugic.h"
#include "xparameters.h"
#include "xil_exception.h"

/*
//Interrupt ID's copied from xparameters_ps.h for quick reference/////////////////////

//Shared Peripheral Interrupts (SPI)
#define XPS_CORE_PARITY0_INT_ID         32U
#define XPS_CORE_PARITY1_INT_ID         33U
#define XPS_L2CC_INT_ID                 34U
#define XPS_OCMINTR_INT_ID              35U
#define XPS_ECC_INT_ID                  36U
#define XPS_PMU0_INT_ID                 37U
#define XPS_PMU1_INT_ID                 38U
#define XPS_SYSMON_INT_ID               39U
#define XPS_DVC_INT_ID                  40U

#define XPS_WDT_INT_ID                  41U
#define XPS_TTC0_0_INT_ID               42U
#define XPS_TTC0_1_INT_ID               43U
#define XPS_TTC0_2_INT_ID               44U
#define XPS_DMA0_ABORT_INT_ID           45U
#define XPS_DMA0_INT_ID                 46U
#define XPS_DMA1_INT_ID                 47U
#define XPS_DMA2_INT_ID                 48U
#define XPS_DMA3_INT_ID                 49U
#define XPS_SMC_INT_ID                  50U
#define XPS_QSPI_INT_ID                 51U
#define XPS_GPIO_INT_ID                 52U
#define XPS_USB0_INT_ID                 53U
#define XPS_GEM0_INT_ID                 54U
#define XPS_GEM0_WAKE_INT_ID            55U
#define XPS_SDIO0_INT_ID                56U
#define XPS_I2C0_INT_ID                 57U
#define XPS_SPI0_INT_ID                 58U
#define XPS_UART0_INT_ID                59U
#define XPS_CAN0_INT_ID                 60U
#define XPS_FPGA0_INT_ID                61U
#define XPS_FPGA1_INT_ID                62U
#define XPS_FPGA2_INT_ID                63U
#define XPS_FPGA3_INT_ID                64U
#define XPS_FPGA4_INT_ID                65U
#define XPS_FPGA5_INT_ID                66U
#define XPS_FPGA6_INT_ID                67U
#define XPS_FPGA7_INT_ID                68U
#define XPS_TTC1_0_INT_ID               69U
#define XPS_TTC1_1_INT_ID               70U
#define XPS_TTC1_2_INT_ID               71U
#define XPS_DMA4_INT_ID                 72U
#define XPS_DMA5_INT_ID                 73U
#define XPS_DMA6_INT_ID                 74U
#define XPS_DMA7_INT_ID                 75U
#define XPS_USB1_INT_ID                 76U
#define XPS_GEM1_INT_ID                 77U
#define XPS_GEM1_WAKE_INT_ID            78U
#define XPS_SDIO1_INT_ID                79U
#define XPS_I2C1_INT_ID                 80U
#define XPS_SPI1_INT_ID                 81U
#define XPS_UART1_INT_ID                82U
#define XPS_CAN1_INT_ID                 83U
#define XPS_FPGA8_INT_ID                84U
#define XPS_FPGA9_INT_ID                85U
#define XPS_FPGA10_INT_ID               86U
#define XPS_FPGA11_INT_ID               87U
#define XPS_FPGA12_INT_ID               88U
#define XPS_FPGA13_INT_ID               89U
#define XPS_FPGA14_INT_ID               90U
#define XPS_FPGA15_INT_ID               91U

//Private Peripheral Interrupts (PPI)
#define XPS_GLOBAL_TMR_INT_ID           27U      SCU Global Timer interrupt
#define XPS_FIQ_INT_ID                  28U      FIQ from FPGA fabric
#define XPS_SCU_TMR_INT_ID              29U      SCU Private Timer interrupt
#define XPS_SCU_WDT_INT_ID              30U      SCU Private WDT interrupt
#define XPS_IRQ_INT_ID                  31U      IRQ from FPGA fabric
*/
///////////////////////////////////////////////////////////////////////

/*Interrupt Structure:
 *      Contains all data needed to configure an interrupt
 *      on the Zynq 7000
 */
typedef struct{

        u32 IntId; //Interrupt ID of device
        void* CallBack; //Pointer to device instance that will be generating interrupts
        void (*interrupt_handler)(void* CallBack); //Function pointer to interrupt service routine

} Interrupt;


//Function prototypes
void interrupt_setup(Interrupt* IntrPtr, XScuGic* GicInstancePtr, u32 IntId_, void* CallBack_, void (*interrupt_handler_)(void* CallBack));
void arminterrupt_setup(XScuGic* GicInstancePtr);
void scugic_initialize(XScuGic* GicInstancePtr, Interrupt* IntrPtr);


#endif /* SRC_INTERRUPT_CONFIG_H_ */

