/*
 * ssi_apa102.c
 *
 * Initializes ssi module between the tiva and the apa102 LED array of lights.
 * Intended: lights will be at their brightest setting and will be only Red, Green or Blue
 *
 * 			Author: James Connolly
 * 			Date: September 6, 2015
 */


#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#define CORD_LENGTH 	120

void main(void) {
	uint32_t color = 0;
	uint32_t CORD_INDEX;
	uint32_t BRIT_GREE = 65280;
	uint32_t BLURED = 255 ;
	uint32_t colorcon = BRIT_GREE - 6144;

	// We set the system clock to 16 mHZ
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
	// enable the SSIO module with this function
	// we must also enable GPIO module A, because the SSIO module uses pins A2-A5
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);


	// Sets the GPIO pins up, due to the fact they're mux'd
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);

	//Sets up pins 5, 4, 3, 2 in
	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                   GPIO_PIN_2);

	// Sets up the SSIO systemcount
	// SSI_FRF_MOTO_MODE_0 is chosen, apposed to any other mode, because in the apa102, data is read
	// on the rising edge, and propegated on the falling edge.
	// This means that data is "loaded" into the register on a rising edge, and set off through the SSI
	// on a falling edge I think.
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 10, 16);

	SSIEnable(SSI0_BASE);
while(1){
	SSIDataPut(SSI0_BASE, 0);
	while(SSIBusy(SSI0_BASE)) {}
	SSIDataPut(SSI0_BASE, 0);
	while(SSIBusy(SSI0_BASE)) {}

	for(CORD_INDEX = 0; CORD_INDEX < 160; ++CORD_INDEX){
 	if(CORD_INDEX % 2 == 0)
			color = color | colorcon;
		switch(CORD_INDEX % 6){
			case(0): //goes blu
				color = color | BLURED / 4;
				break;
			case(3): //goes green
				color = color | BRIT_GREE / 4;
				break;
			case(5): //goes red
				color = color | BLURED / 4;
				break;
			default:
				break;
		}
		SSIDataPut(SSI0_BASE, color);
		//wait for things to finish
		while(SSIBusy(SSI0_BASE)) {}
		 color = 0;
	}

	for(CORD_INDEX = 0; CORD_INDEX < 2; CORD_INDEX++){
		SSIDataPut(SSI0_BASE, 0);
		while(SSIBusy(SSI0_BASE)) {}
	}

	SSIDataPut(SSI0_BASE, 0);
	while(SSIBusy(SSI0_BASE)) {}
	SSIDataPut(SSI0_BASE, 0);
	while(SSIBusy(SSI0_BASE)) {}
	color = 0;
	for(CORD_INDEX = 0; CORD_INDEX < 160; ++CORD_INDEX){
	 	if(CORD_INDEX % 2 == 0)
				color = color | 57344;
	 	SSIDataPut(SSI0_BASE, color);
	 	//wait for things to finish
	 	while(SSIBusy(SSI0_BASE)) {}
	 	 color = 0;
	}

	for(CORD_INDEX = 0; CORD_INDEX < 2; CORD_INDEX++){
			SSIDataPut(SSI0_BASE, 0);
			while(SSIBusy(SSI0_BASE)) {}
	}
}
	//SSIDisable(SSI0_BASE);

	return;
}
