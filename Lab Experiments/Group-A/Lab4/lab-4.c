/*
*
*Team Id: 8A
*Author List: Aakash Deshpande, Royal Jain
*Filename: lab-4.c
*Theme:	
*Functions: switchPinConfig,  UARTIntHandler, main, test1
*Global variables: ui32TempValueS
*
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include"inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include"driverlib/gpio.h"


// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))
volatile uint32_t ui32TempValueS = 25;


/*
*
*Function Name: switchPinConfig
*Input: void
*Output: void
*Logic: Removes lock on Pin0 and sets Pin0, Pin4 as input
*Example Call: switchPinConfig()
*
*/
void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);

	//GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

/*
*
*Function Name: UARTIntHandler
*Input: void
*Output: void
*Logic: Reads temperature to be set in mode 2 and displays it, closes read after 2 sec
*Example Call: Called by OS on UART recieve/buffer overflow interrupt
*
*/
void UARTIntHandler(void)
{

	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status

	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts

	char c = UARTCharGetNonBlocking(UART0_BASE);
	if( c != 'S') return ;

	UARTCharPut(UART0_BASE, 'E');
	UARTCharPut(UART0_BASE, 'n');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'r');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, 'T');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'm');
	UARTCharPut(UART0_BASE, 'p');
	UARTCharPut(UART0_BASE, ':');


	int s = 0;

	SysCtlDelay(2*SysCtlClockGet());

	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		c = UARTCharGetNonBlocking(UART0_BASE);
		UARTCharPutNonBlocking(UART0_BASE,c);
		s  = s*10 + c-'0';

		SysCtlDelay(SysCtlClockGet());
	}

	UARTCharPut(UART0_BASE, '\r');
	UARTCharPut(UART0_BASE, '\n');
	UARTCharPut(UART0_BASE, 'S');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, 'T');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'm');
	UARTCharPut(UART0_BASE, 'p');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'r');
	UARTCharPut(UART0_BASE, 'a');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, 'u');
	UARTCharPut(UART0_BASE, 'r');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, 'u');
	UARTCharPut(UART0_BASE, 'p');
	UARTCharPut(UART0_BASE, 'd');
	UARTCharPut(UART0_BASE, 'a');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'd');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, 'o');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, (s/10 + '0'));
	UARTCharPut(UART0_BASE, (s%10 + '0'));
	UARTCharPut(UART0_BASE, 167);
	UARTCharPut(UART0_BASE, 'C');
	UARTCharPut(UART0_BASE, '\r');
	UARTCharPut(UART0_BASE, '\n');

	ui32TempValueS = s;

}

/*
*
*Function Name: main
*Input: void
*Output: void
*Logic: Sets up ADC coversion of temperature and UART, checks current temperature against set temperature and glows LED
*Example Call: 
*
*/
int main(void) {
	uint32_t ui32ADC0Value[4];
	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;


	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	switchPinConfig();
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

	while(1)
	{
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;


			int tensc = ui32TempValueC/10;
			int unitsc = ui32TempValueC%10;

			char tc = tensc + '0';
			char uc = unitsc + '0';

			int tenss = ui32TempValueS/10;
			int unitss = ui32TempValueS%10;

			char ts = tenss + '0';
			char us = unitss + '0';

			char d = 167;


			UARTCharPut(UART0_BASE, 'C');
			UARTCharPut(UART0_BASE, 'U');
			UARTCharPut(UART0_BASE, 'R');
			UARTCharPut(UART0_BASE, 'R');
			UARTCharPut(UART0_BASE, 'E');
			UARTCharPut(UART0_BASE, 'N');
			UARTCharPut(UART0_BASE, 'T');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 'T');
			UARTCharPut(UART0_BASE, 'E');
			UARTCharPut(UART0_BASE, 'M');
			UARTCharPut(UART0_BASE, 'P');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, '=');
			UARTCharPut(UART0_BASE, tc);
			UARTCharPut(UART0_BASE, uc);
			UARTCharPut(UART0_BASE, d);
			UARTCharPut(UART0_BASE, 'C');
			UARTCharPut(UART0_BASE, ',');

			UARTCharPut(UART0_BASE, 'S');
			UARTCharPut(UART0_BASE, 'E');
			UARTCharPut(UART0_BASE, 'T');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 'T');
			UARTCharPut(UART0_BASE, 'E');
			UARTCharPut(UART0_BASE, 'M');
			UARTCharPut(UART0_BASE, 'P');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, '=');
			UARTCharPut(UART0_BASE, ts);
			UARTCharPut(UART0_BASE, us);
			UARTCharPut(UART0_BASE, d);
			UARTCharPut(UART0_BASE, 'C');
			UARTCharPut(UART0_BASE, '\r');
			UARTCharPut(UART0_BASE, '\n');

			SysCtlDelay(SysCtlClockGet()); //delay 1 sec

			if(ui32TempValueC < ui32TempValueS)
			{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3, 8);
			}
			else
			{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3, 2);
			}
	}

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
*
*Function Name: test1
*Input: void
*Output: void
*Logic: Implements mode 1. Sets up ADC coversion of temperature and UART, displays current temperature
*Example Call: test1
*
*/
int test1(void) {
	uint32_t ui32ADC0Value[4];
	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;
	volatile uint32_t ui32TempValueS = 25;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	while(1) {
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false)) {
		}
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;

		int tens = ui32TempValueC/10;
		int units = ui32TempValueC%10;

		char t = tens + '0';
		char u = units + '0';

		char d = 167;

		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, 'U');
		UARTCharPut(UART0_BASE, 'R');
		UARTCharPut(UART0_BASE, 'R');
		UARTCharPut(UART0_BASE, 'E');
		UARTCharPut(UART0_BASE, 'N');
		UARTCharPut(UART0_BASE, 'T');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'T');
		UARTCharPut(UART0_BASE, 'E');
		UARTCharPut(UART0_BASE, 'M');
		UARTCharPut(UART0_BASE, 'P');
		UARTCharPut(UART0_BASE, 'E');
		UARTCharPut(UART0_BASE, 'R');
		UARTCharPut(UART0_BASE, 'A');
		UARTCharPut(UART0_BASE, 'T');
		UARTCharPut(UART0_BASE, 'U');
		UARTCharPut(UART0_BASE, 'R');
		UARTCharPut(UART0_BASE, 'E');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, t);
		UARTCharPut(UART0_BASE, u);
		UARTCharPut(UART0_BASE, d);
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE, '\n');

		SysCtlDelay(SysCtlClockGet()); //delay 1 sec
	}


}

