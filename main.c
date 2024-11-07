#include "stm32f10x.h"

#include "initialization.h"
#include "buttons.h"

uint8_t LED_PIN = 7;
uint8_t BUTTON_1_PIN = 6;
uint8_t BUTTON_2_PIN = 7;
uint8_t BUTTON_3_PIN = 8;
uint8_t BUTTON_4_PIN = 9;

uint32_t DEBOUNCE_LIMIT = 100;

uint16_t buttonState;
uint8_t debouncedButtonState;
uint32_t isHandled;
uint32_t counter;

void SwitchPin (uint8_t pin_number) {
	if(GPIOA->ODR & 1 << pin_number){
		GPIOA->BSRR = 1 << pin_number + 16;
	}
	else{
		GPIOA->BSRR = 1 << pin_number;
	}
}

void TIM3_IRQHandler () {
	// —брос флага прерывани€
	TIM3->SR &= ~TIM_SR_UIF;
	
	
}

void TIM4_IRQHandler () {
	// —брос флага прерывани€
	TIM4->SR &= ~TIM_SR_UIF;
}


int main (void) {
	SystemCoreClockConfigure();
	SystemCoreClockUpdate();
	
	InitTIM3();
	InitTIM4();
	
	InitGPIO(LED_PIN, BUTTON_1_PIN, BUTTON_2_PIN, BUTTON_3_PIN, BUTTON_4_PIN);
	
	while(1){
		buttonState = GPIOA->IDR & 1 << BUTTON_1_PIN;
		debouncedButtonState = DebounceHandler(buttonState, 0, DEBOUNCE_LIMIT);
		if(debouncedButtonState){
			if(isHandled == 0){
				SwitchPin(LED_PIN);
				isHandled = 1;
				counter++;
			}
		}
		else{
			isHandled = 0;
		}
	}
}
