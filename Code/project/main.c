
/*
* Team ID :- 8
* Author List :- Paramdeep Singh, Depen Morwani, Aakash Deshpande, Royal Jain
 * Filename :- main.c
 * Theme :- Implements state machine for IR and sends signal to bluetooth module and recieves from mobile applicaation
 * Functions :- main and Timer0IntHandler
 * Global variables :- state, lock, ctr, t1, ui32ADC01Value, a, c, UC, LC
 */



#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <driverlib/adc.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/tm4c123gh6pm.h>

#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

uint32_t ui32ADC01Value[4];
volatile uint32_t t1;
int state = 2;
char lock = 'U';
int ctr = 1;


/*
   Function name :- Timer0IntHandler
   Input :- void
   Output :- void
   Logic :- Implements sgtate machine for IR
   Example Call :- Timer0IntHandler(void)
*/

void Timer0IntHandler(void)
{

	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ROM_ADCIntClear(ADC0_BASE, 1);
	ROM_ADCProcessorTrigger(ADC0_BASE, 1);
	while(!ROM_ADCIntStatus(ADC0_BASE, 1, false)) {}
	ROM_ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC01Value);
	t1 = (ui32ADC01Value[0] + ui32ADC01Value[1] + ui32ADC01Value[2] + ui32ADC01Value[3] + 2)/4;

	switch(state){
		case 1:
			if(t1>50){
				state=1;
			} else {
				state=2;
			}
		case 2:
			if(t1<50){
				state=3;
				ctr=1;
			} else {
				state=1;
			}
		case 3:

			if(t1>50)
			{
				state=1;
			}
	}

	if(state==3 && lock == 'L' && ctr == 1)
	{
		UARTCharPut(UART1_BASE,'O');
		ctr++;
	}
}


int a = 1 ;
char c;

int UC = 0;
int LC = 0;



/*
   Function name :- main
   Input :- void
   Output :- int
   Logic :- Initializes IR and UART ports and sends/recieves signals
   Example Call :- main()
*/

int main(void)
	{
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|  GPIO_PIN_2 | GPIO_PIN_3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	uint32_t ui32Period = (SysCtlClockGet() / 10) / 10;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -	1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE,TIMER_A);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);

	ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
	ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH11);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH11);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH11);
	ROM_ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH11|ADC_CTL_IE|ADC_CTL_END);
	ROM_ADCSequenceEnable(ADC0_BASE, 1);

	ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTEnable(UART1_BASE);


	while(1)
	{
		//UARTCharPut(UART1_BASE,lock);
		a++;
		SysCtlDelay(1000);

		if (UARTCharsAvail(UART1_BASE))
		{
			 c = UARTCharGet(UART1_BASE);

			 if(c == 'U') UC++;
			 if(c == 'L') LC++;
			 if( c == 'L' && lock == 'U')
			 {
				 ctr = 1;
				 lock = c;
			 }
			 if(c == 'U' && lock == 'L')
			 {
				 ctr = 1;
				 lock = c;
			 }
		}
	}
}
