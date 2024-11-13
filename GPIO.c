#include "GPIO.h"

#include "stm32f10x.h"

/*----------------------------------------------------------------
 * GPIO_EnablePinInput(pin_number : uint8_t - номер пина для 
 *											  включения
 *					   ) : void
 * Функция для включения пина в качестве выходного для порта A по
 * его номеру.
 *----------------------------------------------------------------*/
void GPIO_EnablePinInput(uint8_t pin_number){
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
 * GPIO_EnablePinInput(pin_number : uint8_t - номер пина для 
 *											  включения
 *					   ) : void
 * Функция для включения пина в качестве входного для порта A по
 * его номеру.
 *----------------------------------------------------------------*/
void GPIO_EnablePinOutput(uint8_t pin_number){
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
 * GPOI_Init(ledPin : uint8_t - номер пина светодиода
 *			 button1Pin : uint8_t - номер пина первой кнопки
 *			 button2Pin : uint8_t - номер пина второй кнопки
 *			 button3Pin : uint8_t - номер пина третьей кнопки
 *			 button4Pin : uint8_t - номер пина четвертой кнопки
 *			 ) : void
 * Функция для инициализаци портов ввода-вывода, согласно
 * конфигурации проекта.
 *----------------------------------------------------------------*/
void GPIO_Init (uint8_t ledPin, uint8_t button1Pin, uint8_t button2Pin, uint8_t button3Pin, uint8_t button4Pin) {
	// Разрешаем тактирование порта A
	RCC->APB2ENR |= (1 << 2);
	
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	
	// Настраиваем пин на выход (для светодиода)
	GPIO_EnablePinOutput(ledPin);
	// Настраиваем пины на вход (для кнопок)
	GPIO_EnablePinInput(button1Pin);
	GPIO_EnablePinInput(button2Pin);
	GPIO_EnablePinInput(button3Pin);
	GPIO_EnablePinInput(button4Pin);
}