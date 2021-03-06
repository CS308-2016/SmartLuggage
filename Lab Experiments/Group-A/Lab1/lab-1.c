/*
*
*Team Id: 8A
*Author List: Aakash Deshpande, Royal Jain
*Filename: lab-1.c
*Theme:	
*Functions: setup, ledPinConfig, switchPinConfig, main
*Global variables: ui8PinData, sw2Status
*
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "examples/boards/ek-tm4c123gxl/drivers/buttons.h"
#include "examples/boards/ek-tm4c123gxl/drivers/rgb.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"


// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
*/
uint8_t ui8PinData = 2; // Colour of LED
unsigned int sw2Status = 0; // Number of SW2 press

/*
*
* Function Name: setup()
* Input: none
* Output: none
* Description: Set crystal frequency and enable GPIO Peripherals  
* Example Call: setup();
*
*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*
*
* Function Name: ledPinConfig()
* Input: none
* Output: none
* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.
* Example Call: ledPinConfig();
*
*/

void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

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

	//GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN); // Set Pin-0 of PORT F as Input
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

/*
*
*Function Name: main
*Input: void
*Output: void
*Logic: Sets up LED pins and checks for button press
*Example Call: 
*
*/
int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();

	while(1)
	{

		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)){

			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3, ui8PinData);
          	if(ui8PinData==2) {ui8PinData=8;} // Transition from red to green
			else {ui8PinData=ui8PinData/2;}

			// To not register a single press repeatedly, we enter a loop
          	while(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) {};
		}
		else{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3, 0x00);
		}

		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){

			sw2Status++;

			// To not register a single press repeatedly, we enter a loop
			while(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) {};
		}
	}
}
