#include "stm32f10x.h"
#include "SSD1306.h"
#include "GPIO.h"
#include "I2C.h"
#include "timers.h"
#include "buttons.h"


uint8_t LED_PIN = 9;
uint8_t BUTTON_1_PIN = 5;
uint8_t BUTTON_2_PIN = 6;
uint8_t BUTTON_3_PIN = 7;
uint8_t BUTTON_4_PIN = 8;

uint8_t highlightedNumber;
uint8_t mode;
uint32_t totalSeconds;
uint8_t alarmSet;
uint8_t buzzer;

struct time{
	uint8_t hourTens;
	uint8_t hourUnits;
	uint8_t minuteTens;
	uint8_t minuteUnits;	
};
struct time clockTime;
struct time alarmTime;

struct button{
	uint8_t number;
	uint8_t pin;
	uint16_t state;
	uint8_t handled;	
};
struct button modeButton;
struct button highlightButton;
struct button incNumberButton;
struct button stopAlarmButton;

void SystemCoreClockConfigure(void) {

	RCC->CR |= ((uint32_t)RCC_CR_HSION);                     // Enable HSI
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  // Wait for HSI Ready

	RCC->CFGR = RCC_CFGR_SW_HSI;                             // HSI is system clock
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  // Wait for HSI used as system clock

	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;                         // HCLK = SYSCLK
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;                        // APB1 = HCLK/4
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;                        // APB2 = HCLK

	RCC->CR &= ~RCC_CR_PLLON;                                // Disable PLL

	//  PLL configuration:  = HSI/2 * 12 = 48 MHz
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
	RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL12);

	RCC->CR |= RCC_CR_PLLON;                                 // Enable PLL
	while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           // Wait till PLL is ready

	RCC->CFGR &= ~RCC_CFGR_SW;                               // Select PLL as system clock source
	RCC->CFGR |=  RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait till PLL is system clock src
}

void updateScreen(){
	if(mode == 2){
		OLED_WriteTime(alarmTime.hourTens,
				   alarmTime.hourUnits,
				   alarmTime.minuteTens,
				   alarmTime.minuteUnits,
				   mode,
				   highlightedNumber);
	}
	else{
		OLED_WriteTime(clockTime.hourTens,
				   clockTime.hourUnits,
				   clockTime.minuteTens,
				   clockTime.minuteUnits,
				   mode,
				   highlightedNumber);
	}
}

/*----------------------------------------------------------------
 * TIM3_IRQHandler(void) : void
 * Обработчик прерываний таймера TIM3. (раз в секунду)
 *----------------------------------------------------------------*/
void TIM3_IRQHandler () {
	// Сброс флага прерывания
	TIM3->SR &= ~TIM_SR_UIF;
	
	if(mode == 0){
		totalSeconds++;
		if(totalSeconds == 86400){
			totalSeconds = 0;
		}
		clockTime.hourTens = totalSeconds / 36000;
		clockTime.hourUnits = totalSeconds / 3600 % 10;
		clockTime.minuteTens = totalSeconds / 600 % 6;
		clockTime.minuteUnits = totalSeconds / 60 % 10;
		
		updateScreen();
	}
}

void switchPin(uint8_t pinNumber){
	if(GPIOA->ODR & (1 << pinNumber)){
		GPIOA->BSRR = 1 << pinNumber + 16;
	}
	else{
		GPIOA->BSRR = 1 << pinNumber;
	}
}

/*----------------------------------------------------------------
 * TIM3_IRQHandler(void) : void
 * Обработчик прерываний таймера TIM3. (раз в секунду)
 *----------------------------------------------------------------*/
void TIM4_IRQHandler () {
	// Сброс флага прерывания
	TIM4->SR &= ~TIM_SR_UIF;
	
	if(buzzer){
		switchPin(LED_PIN);
	}
}

void addTime(uint8_t timeType, uint8_t number){
	if(timeType == 1){
		if(number == 1){
			clockTime.hourTens++;
			if(clockTime.hourTens > 2){
				clockTime.hourTens = 0;
				if(clockTime.hourUnits > 3){
					clockTime.hourUnits = 3;
				}
			}
		}
		else if(number == 2){
			clockTime.hourUnits++;
			if(clockTime.hourTens == 2){
				if(clockTime.hourUnits > 3){
					clockTime.hourUnits = 0;
				}
			}
			if(clockTime.hourUnits > 9){
				clockTime.hourUnits = 0;
			}
		}
		else if(number == 3){
			clockTime.minuteTens++;
			if(clockTime.minuteTens > 5){
				clockTime.minuteTens = 0;
			}
		}
		else if(number == 4){
			clockTime.minuteUnits++;
			if(clockTime.minuteUnits > 9){
				clockTime.minuteUnits = 0;
			}
		}
		totalSeconds = clockTime.hourTens * 36000 + clockTime.hourUnits * 3600 + clockTime.minuteTens * 600 + clockTime.minuteUnits * 60;
	}
	else{
		if(number == 1){
			alarmTime.hourTens++;
			if(alarmTime.hourTens > 2){
				alarmTime.hourTens = 0;
				if(alarmTime.hourUnits > 3){
					alarmTime.hourUnits = 3;
				}
			}
		}
		else if(number == 2){
			alarmTime.hourUnits++;
			if(alarmTime.hourTens == 2){
				if(alarmTime.hourUnits > 3){
					alarmTime.hourUnits = 0;
				}
			}
			if(alarmTime.hourUnits > 9){
				alarmTime.hourUnits = 0;
			}
		}
		else if(number == 3){
			alarmTime.minuteTens++;
			if(alarmTime.minuteTens > 5){
				alarmTime.minuteTens = 0;
			}
		}
		else if(number == 4){
			alarmTime.minuteUnits++;
			if(alarmTime.minuteUnits > 9){
				alarmTime.minuteUnits = 0;
			}
		}
	}
}


int main (void) {	
	SystemCoreClockConfigure();
	SystemCoreClockUpdate();
	
	SysTick_Config(SystemCoreClock / 1000000); 
	
	modeButton.number = 0;
	modeButton.pin = BUTTON_1_PIN;
	highlightButton.number = 1;
	highlightButton.pin = BUTTON_2_PIN;
	incNumberButton.number = 2;
	incNumberButton.pin = BUTTON_3_PIN;
	stopAlarmButton.number = 3;
	stopAlarmButton.pin = BUTTON_4_PIN;
	
	GPIO_Init(LED_PIN, BUTTON_1_PIN, BUTTON_2_PIN, BUTTON_3_PIN, BUTTON_4_PIN);
	I2C_Init();	
	OLED_Init();	
	TIM3_Init();
	TIM4_Init();
	switchPin(LED_PIN);
	
	while (1) {
		// Кнопка для изменения режима часов
		modeButton.state = GPIOA->IDR & (1 << modeButton.pin);
		if(modeButton.state == 0){
			if(modeButton.handled == 0){
				mode++;
				if(mode == 3){
					mode = 0;
				}
				if(mode > 0){
					highlightedNumber = 1;
				}
				else{
					highlightedNumber = 0;
				}
				updateScreen();
				modeButton.handled = 1;
			}
		}
		else{
			modeButton.handled = 0;
		}
		// Если в режиме штатной работы часов
		if(mode == 0){
			// Кнопка для изменения подсвечиваемой цифры
			stopAlarmButton.state = GPIOA->IDR & (1 << stopAlarmButton.pin);
			if(stopAlarmButton.state == 0){
				if(stopAlarmButton.handled == 0){
					buzzer = 0;
					stopAlarmButton.handled = 1;
				}
			}
			else{
				stopAlarmButton.handled = 0;
			}
			if(clockTime.hourTens == alarmTime.hourTens && clockTime.hourUnits == alarmTime.hourUnits && 
			   clockTime.minuteTens == alarmTime.minuteTens && clockTime.minuteUnits == alarmTime.minuteUnits &&
			   alarmSet){
				alarmSet = 0;
				buzzer = 1;
			}
		}
		// Если не в режиме штатной работы часов
		else{
			// Кнопка для изменения подсвечиваемой цифры
			highlightButton.state = GPIOA->IDR & (1 << highlightButton.pin);
			if(highlightButton.state == 0){
				if(highlightButton.handled == 0){
					highlightedNumber++;
					if(highlightedNumber > 4){
						highlightedNumber = 1;
					}
					updateScreen();
					highlightButton.handled = 1;
				}
			}
			else{
				highlightButton.handled = 0;
			}
			// Кнопка для увеличения подсвечиваемой цифры
			incNumberButton.state = GPIOA->IDR & (1 << incNumberButton.pin);
			if(incNumberButton.state == 0){
				if(incNumberButton.handled == 0){
					addTime(mode, highlightedNumber);
					updateScreen();
					incNumberButton.handled = 1;
				}
			}
			else{
				incNumberButton.handled = 0;
			}
		}
	}
}