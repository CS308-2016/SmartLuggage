/*

* Team Id: 8B

* Author: Depen Morwani, Paramdeep Singh

* Description: 

* Filename: lab-2.c

* Functions: setup(), ledPinConfig(), switchPinConfig(), main(), detectKeyPress(), Timer0IntHandler()

* Global Variables: none

*/
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|  GPIO_PIN_2 | GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.

}

void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4 | GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4 | GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

int32_t sw1=1;//Switch 1 state
int32_t sw2=1;//Switch 2 state
uint8_t ui8LED = 2;
int32_t sw2status = 0;

int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();
	uint32_t ui32Period;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet() / 50);
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
	while(1)
	{
	}
}

int detectKeyPress(int32_t sw)
{
	int D0 = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
	int D1 = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
	switch(sw){
		case 1:
			switch(sw1){
				case 1:
					if(!D0){
						sw1=2;
					}
					return 0;
				case 2:
					if(!D0){
						sw1=3;
						return 1;
					}
					else{
						return 0;
					}
				case 3:
					if(!D0){
						return 0;
					}
					else{
						sw1=1;
						return 0;
					}
			}
		case 2:
			switch(sw2){
				case 1:
					if(!D1){
						sw2=2;
					}
					return 0;
				case 2:
					if(!D1){
						sw2=3;
						return 1;
					}
					else{
						return 0;
					}
				case 3:
					if(!D1){
						return 0;
					}
					else{
						sw2=1;
						return 0;
					}
			}
	}
	return 0;
}


void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state
	int keyp = detectKeyPress(1);
	if(keyp==1){
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		if (ui8LED == 8)
		{
			ui8LED = 2;
		}
		else
		{
			ui8LED = ui8LED*2;
		}
	}
	keyp = detectKeyPress(2);
	sw2status = sw2status + keyp;
}
