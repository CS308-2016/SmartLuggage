/*
*
*Team Id: 8A
*Author List: Aakash Deshpande, Royal Jain
*Filename: lab-2.c
*Theme:	
*Functions: switchPinConfig, detectKeyPress, detectKeyPress2, main
*Global variables: ui8PinData, ui32Period, state1, state2, sw2Status
*
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


// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
*/
uint8_t ui8PinData=2; // Colour of LED
uint32_t ui32Period;
uint8_t state1 = 0; // State of SW2
uint8_t state2 = 0; // State of SW1
unsigned int sw2Status = 0;

/*
*
* Function Name: switchPinConfig()
* Input: none
* Output: none
* Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.
* Example Call: switchPinConfig();
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
*Function Name: detectKeyPress
*Input: void
*Output: char whether SW1 has been pressed
*Logic: Debounce SW1
*Example Call: detectKeyPress()
*
*/

unsigned char detectKeyPress (void){

	unsigned char flag = 0;
	if(state1==0)
	{
		// Read the current state of the GPIO pin
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){
			state1=1;
		}
	}
	else if(state1==1){
		// Read the current state of the GPIO pin
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){
			state1=2;
			flag = 1; // Press recorded
		}
		else{
			state1=0;
		}
	}
	else if(state1==2){
		// Read the current state of the GPIO pin
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){
			state1=0;
		}
	}
	return flag;
}

/*
*
*Function Name: detectKeyPress2
*Input: void
*Output: char whether SW2 has been pressed
*Logic: Debounce SW2
*Example Call: detectKeyPress2()
*
*/
unsigned char detectKeyPress2 (void){

	unsigned char flag = 0;
	if(state2==0)
	{
		// Read the current state of the GPIO pin
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)){
			state2=1;
		}
	}
	else if(state2==1){
		// Read the current state of the GPIO pin
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)){
			state2=2;
			flag = 1; // Press recorded
		}
		else{
			state2=0;
		}
	}
	else if(state2==2){
		// Read the current state of the GPIO pin
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)){
			state2=0;
		}
	}
	return flag;
}

/*
*
*Function Name: main
*Input: void
*Output: void
*Logic: Sets up ISR for Timer0A
*Example Call: 
*
*/
int main(void)
{

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	// Clock setup for Timer 0 A
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	switchPinConfig();

	// Time period of 50 ms
	ui32Period = (SysCtlClockGet() / 10)/ 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

	while(1)
	{}
}

/*
*
*Function Name: Timer0IntHandler
*Input: void
*Output: void
*Logic: Checks for key press and changes LED colour
*Example Call: Called by OS on timer interrupt
*
*/
void Timer0IntHandler (void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	unsigned char press;
	press = detectKeyPress(); // SW2
	if(press == 1) sw2Status++;
	press = detectKeyPress2(); // SW1
	if(press == 1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3, ui8PinData);
		if(ui8PinData==2) {ui8PinData=8;} // Transition from red to green
		else {ui8PinData=ui8PinData/2;}
	}
}
