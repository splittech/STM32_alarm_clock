#include "stm32f10x.h"

/*----------------------------------------------------------------
 * SystemCoreClockConfigure : 
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
	
	// В качестве источника тактирования был выбран HSI без использования множителей.
	// Такой выбор обусловлен требуемой автономностью устройства (частота ниже - меньше расход энергии),
	// в целом будильник не требует какой-то особой точности тактового сигнала, так что использование
	// относительно неточного HSI не должно вызвать проблем при работе. Таким образом частота SYSCLK составит 8МГц
}

/*----------------------------------------------------------------
 * InitTIM3 : 
 *----------------------------------------------------------------*/
void InitTIM3 (void) {
	// Включаем таймер
	RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
	// Включаем счетчик таймера
	TIM3 -> CR1 = TIM_CR1_CEN;
	
	// Настраиваем предделитель таймера для того, чтобы удобно задавать интервал прерываний
	// Прерывание будет вызываться каждую секунду
	TIM3->PSC = (SystemCoreClock) / 1000 - 1;
	TIM3->ARR = 1000 - 1;

	// Разрешаем прерывания
	TIM3->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM3_IRQn);
}

/*----------------------------------------------------------------
 * InitTIM4 : 
 *----------------------------------------------------------------*/
void InitTIM4 (void) {
	// Включаем таймер
	RCC -> APB1ENR |= RCC_APB1ENR_TIM4EN;
	// Включаем счетчик таймера
	TIM4 -> CR1 = TIM_CR1_CEN;
	
	// Настраиваем предделитель таймера для того, чтобы удобно задавать интервал прерываний
	// SystemCoreClock / 2 означает, что обработчик прерывания будет вызываться каждые пол секунды
	TIM4->PSC = (SystemCoreClock / 2) / 1000 - 1;
	TIM4->ARR = 1000 - 1;

	// Разрешаем прерывания
	TIM4->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM4_IRQn);
}

/*----------------------------------------------------------------
 * EnablePinInput : 
 *----------------------------------------------------------------*/
void EnablePinInput(uint8_t pin_number){
	// Половина пинов порта на регистре CRL, другая половина на CRH
	if(pin_number < 8){
		// Отчищаем часть регистра для соответствующего пина
		GPIOA->CRL &= ~((15 << 4*pin_number));
		// MODE = 00: input mode
		GPIOA->CRL |= ((0 << 4*pin_number));
		// INPUT MODE = 10: input with pull up/pull down
		GPIOA->CRL |= ((8 << 4*pin_number));
	}
	else{
		// То же самое для регистра CRH
		pin_number -= 8;
		GPIOA->CRH &= ~((15 << 4*pin_number));
		GPIOA->CRH |= ((0 << 4*pin_number));
		GPIOA->CRH |= ((8 << 4*pin_number));
	}
}

/*----------------------------------------------------------------
 * EnablePinOutput :
 *----------------------------------------------------------------*/
void EnablePinOutput(uint8_t pin_number){
	// Половина пинов порта на регистре CRL, другая половина на CRH
	if(pin_number < 8){
		// Отчищаем часть регистра для соответствующего пина
		GPIOA->CRL &= ~((15 << 4*pin_number));
		// MODE = 01: output mode (10MHz)
		GPIOA->CRL |= ((1 << 4*pin_number));
		// OUTPUT MODE = 00: output push-pull
		GPIOA->CRL |= ((0 << 4*pin_number));
	}
	else{
		// То же самое для регистра CRH
		pin_number -= 8;
		GPIOA->CRH &= ~((15 << 4*pin_number));
		GPIOA->CRH |= ((1 << 4*pin_number));
	}
}

/*----------------------------------------------------------------
 * InitGPIO : 
 *----------------------------------------------------------------*/
void InitGPIO (uint8_t ledPin, uint8_t button1Pin, uint8_t button2Pin, uint8_t button3Pin, uint8_t button4Pin) {
	// Разрешаем тактирование порта A
	RCC->APB2ENR |= (1 << 2);
	
	// Настраиваем пин на выход (для светодиода)
	EnablePinOutput(ledPin);
	// Настраиваем пинs на вход (для кнопок)
	EnablePinInput(button1Pin);
	EnablePinInput(button2Pin);
	EnablePinInput(button3Pin);
	EnablePinInput(button4Pin);
}