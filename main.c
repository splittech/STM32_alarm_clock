#include "stm32f10x.h"

#include "initialization.h"
#include "buttons.h"

uint8_t LED_PIN = 7;
uint8_t BUTTON_1_PIN = 5;
uint8_t BUTTON_2_PIN = 6;
uint8_t BUTTON_3_PIN = 8;
uint8_t BUTTON_4_PIN = 9;

uint32_t DEBOUNCE_LIMIT = 100;

uint16_t buttonState;
uint8_t debouncedButtonState;
uint32_t isHandled;
uint32_t counter;

/*----------------------------------------------------------------
 * SystemCoreClockConfigure(void) : void
 * Функция для настройки тактовой частоты микроконтроллера
 *----------------------------------------------------------------*/
void SystemCoreClockConfigure(void) {
	// Включаем HSE
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);
	// Ожидаем включения HSE
	while ((RCC->CR & RCC_CR_HSERDY) == 0);

	// Устанавилваем HSE в качестве источника такстирования
	RCC->CFGR = RCC_CFGR_SW_HSE;
	// Ожидаем завершения
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);

	// Настройка тактирования самого процессора и портов
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // HCLK = SYSCLK
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1; // APB1 = HCLK
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 = HCLK
}

void SwitchPin (uint8_t pin_number) {
	if(GPIOA->ODR & 1 << pin_number){
		GPIOA->BSRR = 1 << pin_number + 16;
	}
	else{
		GPIOA->BSRR = 1 << pin_number;
	}
}

void TIM3_IRQHandler () {
	// Сброс флага прерывания
	TIM3->SR &= ~TIM_SR_UIF;
	
	
}

void TIM4_IRQHandler () {
	// Сброс флага прерывания
	TIM4->SR &= ~TIM_SR_UIF;
}


int main (void) {
	SystemCoreClockConfigure();
	SystemCoreClockUpdate();
	
	//SysTick_Config(SystemCoreClock / 1000000);
	
	TIM3_Init();
	TIM4_Init();
	
	GPIO_Init(LED_PIN, BUTTON_1_PIN, BUTTON_2_PIN, BUTTON_3_PIN, BUTTON_4_PIN);
	
	while(1){
		buttonState = GPIOA->IDR & 1 << BUTTON_1_PIN;
		debouncedButtonState = BUTTONS_DebounceHandler(buttonState, 0, DEBOUNCE_LIMIT);
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
