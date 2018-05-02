#include "stm8s.h"
#include "stm8s_exti.h"
#include "stm8s_tim4.h"
#include "nrf24l01.h"
#include "shared_global_vars.h"
#include <stdlib.h>
#include <string.h>

volatile bool rf_interrupt = FALSE;

uint8_t address[5] = { 0x01, 0x01, 0x01, 0x01, 0x01 };

void process_message(char *message);

main()
{
	bool on = FALSE;
	nRF24L01 rf;
	
	rf.ss.port = GPIOA;
	rf.ss.pin = GPIO_PIN_1;
	rf.ce.port = GPIOA;
	rf.ce.pin = GPIO_PIN_2;
	
	GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_IN_PU_IT);
	EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA, EXTI_SENSITIVITY_FALL_ONLY);
	
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_SLOW);
	
	enableInterrupts();
	
	nRF24L01_begin(&rf);
	nRF24L01_listen(&rf, 0, address);
	
	while (1) {
		if (rf_interrupt) {
			rf_interrupt = FALSE;
			
			while (nRF24L01_data_received(&rf)) {
				nRF24L01Message msg;
				
				nRF24L01_read_received_data(&rf, &msg);
				process_message((char *)msg.data);
			}
			
			nRF24L01_listen(&rf, 0, address);
		}
	}
}

void process_message(char *message) {
	if (strcmp(message, "ON") == 0)
		GPIO_WriteHigh(GPIOB, GPIO_PIN_5);
	else if (strcmp(message, "OFF") == 0)
		GPIO_WriteLow(GPIOB, GPIO_PIN_5);
}