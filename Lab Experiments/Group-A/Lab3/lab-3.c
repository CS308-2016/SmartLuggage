/*
*
*Team Id: 8A
*Author List: Aakash Deshpande, Royal Jain
*Filename: lab-3.c
*Theme:	
*Functions: detectKeyPress, detectKeyPress2, main, Timer0IntHandler
*Global variables: ui8PinData, ui32Period, state1, state2, sw2Status, t, ctr, lp, press1, press2, ctrsw1, count1, count2
*
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"

#define PWM_FREQUENCY 55

uint8_t ui8PinData = 2; // Colour of LED
uint32_t ui32Period; // Debouncing period

uint8_t state1 = 0; // State of SW2
uint8_t state2 = 0; // State of SW1

unsigned int sw2Status = 0;

int t = 100000; // Frequency of auto mode
int ctr = 1; // State os system (manual or auto)

int lp = 0; // Long press of SW2
unsigned char press1; // SW1 press signal
unsigned char press2; // SW2 press signal
int count1 = 0; // Number of SW1 press
int count2 = 0; // Number of SW2 press
int ctrsw1 = 0; // Number of SW1 press

volatile uint8_t ui8Adjust;
volatile uint8_t ui8Adjust1;
volatile uint8_t ui8Adjust2;
volatile uint8_t ui8Adjust3;

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
			count1 = 0 ;
		}
		else
		{
			flag = 1;
			state1=0;
		}
	}
	else if(state1==2){
		count1++;
		// Read the current state of the GPIO pin

		flag = 2;
		if(count1 >= 50)flag = 3;

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
			count2=0;
		}
		else{
			flag = 1;
			state2=0;
		}
	}
	else if(state2==2){
		// Read the current state of the GPIO pin
		count2++;
		// Read the current state of the GPIO pin

		flag = 2;
		if(count2 >= 50)flag = 3;

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
*Logic: Sets up PWM module for LEDs and changes frequencies as per global variables
*Example Call: 
*
*/
int main(void)
{
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	ui8Adjust = 254;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);


	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);


	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);

	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);


	//GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
	//GPIOPinConfigure(GPIO_PF1_M1PWM7);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);

	//ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);


	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, (254-ui8Adjust) * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);



	PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, ui32Load);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, (254-ui8Adjust) * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_1);


	ui32Period = (SysCtlClockGet() / 10)/ 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);


	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);



	while(1)
	{

		SysCtlDelay(t);

		if(ctr == 1)
		{
			ui8Adjust--;

			if (ui8Adjust < 10)
			{
				ui8Adjust = 254;
				ctr = 2;
				continue;
			}

			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, (254-ui8Adjust) * ui32Load / 1000);
		}

		if(ctr == 2)
		{
			ui8Adjust--;

			if (ui8Adjust < 10)
			{
				ui8Adjust = 254;
				ctr = 3;
				continue;
			}

			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, (254-ui8Adjust) * ui32Load / 1000);
		}

		if(ctr == 3)
		{
			ui8Adjust--;

			if (ui8Adjust < 10)
			{
				ui8Adjust = 254;
				ctr = 1;
				continue;
			}


			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, (254-ui8Adjust) * ui32Load / 1000);
		}

		if(ctr == 4)
		{
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);

		}

		if(ctr == 5)
		{
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust * ui32Load / 1000);
		}

		if(ctr == 6)
		{
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust * ui32Load / 1000);
		}

	}

}

/*
*
*Function Name: Timer0IntHandler
*Input: void
*Output: void
*Logic: Checks for key press, remembers them and implements state machine to move to required state
*Example Call: Called by OS on timer interrupt
*
*/
void Timer0IntHandler (void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	press1 = detectKeyPress(); // SW2
	press2 = detectKeyPress2(); // SW1

	// Mode 2 : Manual
	if(ctr > 3)
	{

		if(press1 == 0 && lp && ctrsw1 >= 2)
		{
			lp = 0;
			ctrsw1 = 0;
			ui8Adjust1 = 0;
			ui8Adjust2 = ui8Adjust;
			ui8Adjust3 = 0;
			ctr = 5;
		}

		else if(press1 == 0 && lp && ctrsw1 == 1)
		{
			lp = 0;
			ctrsw1 = 0;
			ui8Adjust1 = ui8Adjust;
			ui8Adjust2 = 0;
			ui8Adjust3 = 0;
			ctr = 4;
		}

		else if(press1 == 0 && lp && ctrsw1 == 0)
		{
			lp = 0;
		}

		else if(press1 == 0 && press2 == 0) return;

		else if(press1 == 0 && press2 == 1)
		{
			ui8Adjust -= 10;

			if (ui8Adjust < 10)
			{
				ui8Adjust = 10;
			}

		}

		else if(press1 == 1 && press2 == 0)
		{
			ui8Adjust += 10;

			if (ui8Adjust > 244)
			{
				ui8Adjust = 244;
			}

		}

		else if((press1 == 2 || press1 == 3) && press2 == 1)
		{
			lp = 1;
			ctrsw1++;
		}

		else if(press1 == 3 && press2 == 3)
		{
			lp = 0;
			ctrsw1 = 0;

			ui8Adjust1 = 0;
			ui8Adjust2 = 0;
			ui8Adjust3 = ui8Adjust;
			ctr = 6;
		}

		else if(press1 == 3)
		{
			lp = 1;
		}

		return ;
	}

	// Mode 1 : Automatic
	if(press1 == 0 && lp && ctrsw1 >= 2)
	{
		lp = 0;
		ctrsw1 = 0;
		ui8Adjust1 = 0;
		ui8Adjust2 = ui8Adjust;
		ui8Adjust3 = 0;
		ctr = 5;
	}

	else if(press1 == 0 && lp && ctrsw1 == 1)
	{
		lp = 0;
		ctrsw1 = 0;
		ui8Adjust1 = ui8Adjust;
		ui8Adjust2 = 0;
		ui8Adjust3 = 0;
		ctr = 4;
	}

	else if(press1 == 0 && lp && ctrsw1 == 0)
	{
		lp = 0;
	}

	else if(press1 == 0 && press2 == 0) return;

	else if(press1 == 0 && press2 == 1)
	{
		t = t/2;
	}

	else if(press1 == 1 && press2 == 0)
	{
		t = t*2;
	}

	else if((press1 == 2 || press1 == 3) && press2 == 1)
	{
		lp = 1;
		ctrsw1++;
	}

	else if(press1 == 3 && press2 == 3)
	{
		lp = 0;
		ctrsw1 = 0;

		ui8Adjust1 = 0;
		ui8Adjust2 = 0;
		ui8Adjust3 = ui8Adjust;
		ctr = 6;
	}

	else if(press1 == 3)
	{
		lp = 1;
	}
}

