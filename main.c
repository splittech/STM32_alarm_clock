#include "stm32f10x.h"
#include "SSD1306.h"
#include "GPIO.h"
#include "I2C.h"
#include "timers.h"


uint8_t highlightedNumber;
uint8_t mode;
uint32_t totalSeconds;

struct time{
	uint8_t hours;
	uint8_t minutes;	
};

struct time clockTime;
struct time alarmTime;

void SystemCoreClockConfigure(void) {

	RCC->CR |= ((uint32_t)RCC_CR_HSION);                     // Enable HSI
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  // Wait for HSI Ready

	RCC->CFGR = RCC_CFGR_SW_HSI;                             // HSI is system clock
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  // Wait for HSI used as system clock

	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;                         // HCLK = SYSCLK
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;                        // APB1 = HCLK 	(I2C bus)
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;                        // APB2 = HCLK		(GPIO bus)

	RCC->CR &= ~RCC_CR_PLLON;                                // Disable PLL

	//  PLL configuration:  = HSI/2 * 9 = 36 MHz
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
	RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL9 );

	RCC->CR |= RCC_CR_PLLON;                                 // Enable PLL
	while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           // Wait till PLL is ready

	RCC->CFGR &= ~RCC_CFGR_SW;                               // Select PLL as system clock source
	RCC->CFGR |=  RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait till PLL is system clock src
}

void updateScreen(){
	OLED_WriteTime(clockTime.hours / 10,
				   clockTime.hours % 10,
				   clockTime.minutes / 10,
				   clockTime.minutes % 10,
				   mode,
				   highlightedNumber);
}

/*----------------------------------------------------------------
 * TIM3_IRQHandler(void) : void
 * Обработчик прерываний таймера TIM3. (раз в секунду)
 *----------------------------------------------------------------*/
void TIM3_IRQHandler () {
	// Сброс флага прерывания
	TIM3->SR &= ~TIM_SR_UIF;
	
//	totalSeconds++;
//	if(totalSeconds == 86400){
//		totalSeconds = 0;
//	}
//	clockTime.minutes = totalSeconds % 60;
//	clockTime.hours = totalSeconds / 60;
//	
//	mode++;
//	if(mode > 2){
//		mode = 0;
//	}
//	
//	highlightedNumber++;
//	if(highlightedNumber > 4){
//		highlightedNumber = 0;
//	}
//	
//	updateScreen();
}

/*----------------------------------------------------------------
 * TIM3_IRQHandler(void) : void
 * Обработчик прерываний таймера TIM3. (раз в секунду)
 *----------------------------------------------------------------*/
void TIM4_IRQHandler () {
	// Сброс флага прерывания
	TIM4->SR &= ~TIM_SR_UIF;
	
	totalSeconds += 60;
	if(totalSeconds == 86400){
		totalSeconds = 0;
	}
	clockTime.minutes = totalSeconds / 60 % 60;
	clockTime.hours = totalSeconds / 3600;
	
	mode++;
	if(mode > 2){
		mode = 0;
	}
	
	highlightedNumber++;
	if(highlightedNumber > 4){
		highlightedNumber = 0;
	}
	
	updateScreen();
}

int main (void) {	
	SystemCoreClockConfigure();
	SystemCoreClockUpdate();
	
	SysTick_Config(SystemCoreClock / 1000000); 
	
	totalSeconds = 86400 - 1200;
	
	GPIO_Init(1, 1, 1, 1, 1);
	I2C_Init();	
	OLED_Init();	
	TIM3_Init();
	TIM4_Init();
	
	OLED_WriteTime(1, 2, 3, 4, 2, 4);
	
	while (1) {
	}
}