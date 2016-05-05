/*

* Team Id: 8B

* Author: Depen Morwani, Paramdeep Singh

* Description: 

* Filename: main_1.c

* Functions: main(), detectKeyPress()

* Global Variables: ui32TempAvg, ui32TempValueC, ui32TempValueF, settemp

*/
#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"inc/hw_ints.h"
#include"driverlib/interrupt.h"
#include"driverlib/debug.h"
#include"driverlib/adc.h"

volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;
volatile int settemp;

int main(void) {
	settemp = 25;
	uint32_t ui32ADC0Value[4];
	SysCtlClockSet(
			SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
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
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	while(1){
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
		char m[] = "Current Temperature is    *C, Set Temperature is    *C";
		m[23]=(ui32TempValueC/10 % 10) + '0';
		m[24]=(ui32TempValueC%10) + '0';
		m[49]=(settemp/10 % 10) + '0';
		m[50]=(settemp%10) + '0';
		int i;
		for(i=0;m[i];i++){
			UARTCharPut(UART0_BASE, m[i]);
		}
		if(ui32TempValueC < settemp){
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,8);
		}
		else{
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,2);
		}
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE, '\n');
		SysCtlDelay(1000000);
	}
}

void UARTIntHandler(void) {
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
	if(UARTCharGetNonBlocking(UART0_BASE) == 'S'){
		char m[] = "Enter The Temperature : ";
		char h[100] = "Set temperature Updated to ";
		int i;
		int l;
		l=0;
		for(i=0;m[i];i++){
					UARTCharPut(UART0_BASE, m[i]);
				}
		int x=0;
		while(1){
			char c;
			if(UARTCharsAvail(UART0_BASE)){
				c= UARTCharGetNonBlocking(UART0_BASE);
				UARTCharPut(UART0_BASE, c);
				if(c=='\r') break;
				h[27+l]=c;
				x=10*x+c-'0';
				l++;
			}
		}
		settemp = x;
		for(i=0;i<27+l;i++){
							UARTCharPut(UART0_BASE, h[i]);
						}
		UARTCharPut(UART0_BASE, '*');
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE,'\n');
	}
}
