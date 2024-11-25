// CMSIS.
#include "stm32f10x.h"

// ����������� ����������.
#include "SSD1306.h"
#include "GPIO.h"
#include "I2C.h"
#include "timers.h"
#include "buttons.h"

/*------------------------------�������� �������----------------------------------*/
//  ��������: ������ ������������ ����� ���������� "���� � �����������".
//  ���������: ����� Nucleo STM32 f103rb, 4 ������, ����� SSD1306, �������� ������.
//	������: � ���������� ���� ��� ������ ������:
//										0 - ������� ����� ������.
//										1 - ����� �������������� �����.
//										2 - ����� ������������� ����������.
//  ������������ ������ ���������� ���������� ��� ������� ������ ����� ������.
//  � ������ �������������� ����� ������������� ����� ����� ������������ ����������
//  ��������� ����� ��� ������ ������ ���������� �����. �������� ��������� �����
//  ����� ��� ������ ������ ������ �����. ����� ������ �� ������ ��������������
//  ���������� ��������� ������������� ��������� ����������. ��� ����, �����
//	���������� ��������� ��� �������� ���������� ���������, ���������� ������ ��
//	������ ��������� ����������.
/*--------------------------------------------------------------------------------*/

/*------------------------------��������� �������---------------------------------*/
uint8_t LED_PIN = 9;					 // ����� ���� ��� �������.

uint8_t MODE_BUTTON_PIN = 5;			 // ����� ���� ��� ������ ����� ������.

uint8_t HIGHLIGHT_BUTTON_PIN = 6;		 // ����� ���� ��� ������ ������ �����.

uint8_t INCREASE_NUMBER_BUTTON_PIN = 7;  // ����� ���� ��� ������ ���������� �����.

uint8_t STOP_ALARM_BUTTON_PIN = 8;		 // ����� ���� ��� ������ ��������� ����������.

uint32_t BUTTON_DEBOUNCE_LIMIT = 10000;	 // ����� �������� ��� ����������� ��������� 
										 //	�������� ������.
/*--------------------------------------------------------------------------------*/

// ����� ������ ����������.
uint8_t mode;
// �������������� �����.
uint8_t highlightedNumber;
// ����� ������� ������ � ������.
uint32_t totalSeconds;
// ���� ����, ��� ��������� �������.
uint8_t alarmSet;
// ���� ����, ��� ��������� �������.
uint8_t screenChanged;
// ���� ��� ������ ��������� �������.
uint8_t buzzer;

// ��������� ���������� �������.
struct time{
	uint8_t hourTens;	  // ������� �����.
	uint8_t hourUnits;	  // ������� �����.
	uint8_t minuteTens;	  // ������� �����.
	uint8_t minuteUnits;  // ������� �����.
};
// ����� �����.
struct time clockTime;
// ����� ����������.
struct time alarmTime;

// ��������� ���������� ������.
struct button{
	uint8_t number;	  // ����� ������.
	uint8_t pin;	  // ����� ���� ������.
	uint16_t state;	  // ��������� ������.
	uint8_t handled;  // ���������� �� ������.
};
// ������ ��� ��������� ������ ������.
struct button modeButton;
// ������ ��� ��������� �������������� �����.
struct button highlightButton;
// ������ ��� ���������� �������������� �����.
struct button incNumberButton;
// ������ ��� ��������� ���������� ��� ������ ����������� ����������.
struct button stopAlarmButton;

/*--------------------------------------------------------------------------------
 * SystemCoreClockConfigure (void) : void
 * ������� ��� ��������� �������� ������� ����������������.
 *--------------------------------------------------------------------------------*/
void SystemCoreClockConfigure(void) {
	// �������� HSI.
	RCC->CR |= ((uint32_t)RCC_CR_HSION);
	// ���� ������� HSI.
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);
	// ������������� HSI � �������� ��������� ������������.
	RCC->CFGR = RCC_CFGR_SW_HSI;
	// ������� ��������� ���������.
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
	// ����������� ��������.
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
	// ��������� PLL ����� ����������.
	RCC->CR &= ~RCC_CR_PLLON;
	// ����������� PLL.
	// PLL configuration:  = HSI/2 * 12 = 48 MHz.
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
	RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL12);
	// �������� PLL ����� ���������.
	RCC->CR |= RCC_CR_PLLON;
	// ������� ��������� PLL.
	while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();
	// ������������� PLL � �������� ��������� ������������.
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |=  RCC_CFGR_SW_PLL;
	// ������� ��������� ���������.
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait till PLL is system clock src
}

/*--------------------------------------------------------------------------------
 * UpdateScreen (void) : void
 * ������� ��� ���������� ������ � �������������� ������� ����������.
 *--------------------------------------------------------------------------------*/
void UpdateScreen(void){
	// ���� ������ ����� �������������� ����������,
	// �� ����� ������������ ����� ����������.
	if(mode == 2){
		OLED_WriteTime(alarmTime.hourTens,
				   alarmTime.hourUnits,
				   alarmTime.minuteTens,
				   alarmTime.minuteUnits,
				   mode,
				   highlightedNumber);
	}
	// �� ���� ��������� ������� ����� ������������ ����� �����.
	else{
		OLED_WriteTime(clockTime.hourTens,
				   clockTime.hourUnits,
				   clockTime.minuteTens,
				   clockTime.minuteUnits,
				   mode,
				   highlightedNumber);
	}
}

/*--------------------------------------------------------------------------------
 * UpdateScreen (pinNumber : uint8_t - ����� ����, �� ������� ����� ������������� ������
				 ) : void
 * ������� ��� �������� ������� ���������� ���� �� ����� �.
 *--------------------------------------------------------------------------------*/
void SwitchPin(uint8_t pinNumber){
	// ���� ���� ������ �� ����.
	if(GPIOA->ODR & (1 << pinNumber)){
		// ���������� ���.
		GPIOA->BSRR = 1 << pinNumber + 16;
	}
	// ���� ��� ������� �� ����.
	else{
		// ������������� ���.
		GPIOA->BSRR = 1 << pinNumber;
	}
}

/*----------------------------------------------------------------
 * TIM3_IRQHandler(void) : void
 * ���������� ���������� ������� TIM3. (��� � �������)
 *----------------------------------------------------------------*/
void TIM3_IRQHandler () {
	// ����� ����� ����������.
	TIM3->SR &= ~TIM_SR_UIF;
	// ���� � ������ ������� ������.
	if(mode == 0){
		// ����������� ����� ������� ������.
		totalSeconds++;
		// ����������, ���� ������������� ���������� ������ � ������.
		if(totalSeconds == 86400){
			totalSeconds = 0;
		}
		// ���������� � ������ � ���������.
		clockTime.hourTens = totalSeconds / 36000;
		clockTime.hourUnits = totalSeconds / 3600 % 10;
		clockTime.minuteTens = totalSeconds / 600 % 6;
		clockTime.minuteUnits = totalSeconds / 60 % 10;
		// ������������� ���� ���������� ������.
		screenChanged = 1;
	}
}

/*----------------------------------------------------------------
 * TIM3_IRQHandler(void) : void
 * ���������� ���������� ������� TIM3. (��� � �������)
 *----------------------------------------------------------------*/
void TIM4_IRQHandler () {
	// ����� ����� ����������
	TIM4->SR &= ~TIM_SR_UIF;
	// ���� ����� �������� ������
	if(buzzer || GPIOA->ODR & (1 << LED_PIN)){
		// �������� ������� �� ����
		SwitchPin(LED_PIN);
	}
}

/*--------------------------------------------------------------------------------
 * AddTime (timeType : uint8_t - ��� �����, ��� ������� ����������� �����. 
 *			number : uint8_t   - ����� ����� � ����������, ������� ����� ��������.
 *			) : void
 * ������������� �������, ������� ������������������� ������� � ��������� �����
 * ���������� ����� ��� ����������.
 *--------------------------------------------------------------------------------*/
void AddTime(uint8_t timeType, uint8_t number){
	// ���� ����� �������� ����.
	if(timeType == 1){
		// ���� ����� �������� ������ �����.
		if(number == 1){
			// ����������� �����.
			clockTime.hourTens++;
			// ��� �������� �����, ����������, ���� ��������� 2.
			if(clockTime.hourTens > 2){
				clockTime.hourTens = 0;
			}
			// ���� ���������� ����� 2 � ���������� ������ ����� ������ ��� 3
			if(clockTime.hourTens == 2){
				if(clockTime.hourUnits > 3){
					// ��������� ������� ����� �� 3 - ��� ������
					// (����� �� ���� ������ ��� 23 ����)
					clockTime.hourUnits = 3;
				}
			}
		}
		// ���� ����� �������� ������ �����.
		else if(number == 2){
			// ����������� �����.
			clockTime.hourUnits++;
			// ��� ������ �����, ����������, ���� ��������� 9.
			if(clockTime.hourUnits > 9){
				clockTime.hourUnits = 0;
			}
			// ���� � �������� ����� ������� ����� 2,
			// �� ����������, ���� ��������� 3
			if(clockTime.hourTens == 2){
				if(clockTime.hourUnits > 3){
					clockTime.hourUnits = 0;
				}
			}
			
		}
		// ���� ����� �������� ������ �����.
		else if(number == 3){
			// ����������� �����.
			clockTime.minuteTens++;
			// ��� �������� �����, ����������, ���� ��������� 5.
			if(clockTime.minuteTens > 5){
				clockTime.minuteTens = 0;
			}
		}
		// ���� ����� �������� ��������� �����.
		else if(number == 4){
			// ����������� �����.
			clockTime.minuteUnits++;
			// ��� ������ �����, ����������, ���� ��������� 5.
			if(clockTime.minuteUnits > 9){
				clockTime.minuteUnits = 0;
			}
		}
		// ��������� ����� ������� ������, �� ��������� ���������� ����������.
		totalSeconds = clockTime.hourTens * 36000 + clockTime.hourUnits * 3600 + clockTime.minuteTens * 600 + clockTime.minuteUnits * 60;
	}
	// ���� ����� �������� ����.
	else{
		// ��� �� �� �����.
		if(number == 1){
			alarmTime.hourTens++;
			if(alarmTime.hourTens > 2){
				alarmTime.hourTens = 0;
			}
			if(alarmTime.hourTens == 2){
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
	// ��������� �������� �������
	SystemCoreClockConfigure();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000000); 
	
	// ��������� ��������� ������ ����� ������.
	modeButton.number = 0;
	modeButton.pin = MODE_BUTTON_PIN;
	// ��������� ��������� ������ ��� �������� ���������� �����.
	highlightButton.number = 1;
	highlightButton.pin = HIGHLIGHT_BUTTON_PIN;
	// ��������� ��������� ������ ���������� ���������� �����.
	incNumberButton.number = 2;
	incNumberButton.pin = INCREASE_NUMBER_BUTTON_PIN;
	// ��������� ��������� ������ ��� ��������� ����������.
	stopAlarmButton.number = 3;
	stopAlarmButton.pin = STOP_ALARM_BUTTON_PIN;
	// ������������� ������ �����-������ .
	GPIO_Init(LED_PIN, modeButton.pin, highlightButton.pin, incNumberButton.pin, stopAlarmButton.pin);
	// ������������� ���������� I2C.
	I2C_Init();
	// ������������� �������.
	OLED_Init();
	UpdateScreen();
	//switchPin(LED_PIN);
	// ������������� �������� ������ ����������.
	TIM3_Init();
	TIM4_Init();
	while (1) {
		// ��������� ������ ��� ��������� ������ �����.
		modeButton.state = GPIOA->IDR & (1 << modeButton.pin);
		// ���������� ������������ �������.
		if(BUTTONS_DebounceHandler(modeButton.state, modeButton.number, BUTTON_DEBOUNCE_LIMIT)){
			// ���� ������ ��� �� ����������.
			if(modeButton.handled == 0){
				// �������� �����.
				mode++;
				// ���������� �� �����.
				if(mode == 3){
					mode = 0;
				}
				// ���� ������� � ����� ��������������.
				if(mode > 0){
					// ������������ ������ �����.
					highlightedNumber = 1;
				}
				// ���� ������� � ����� ������� ������.
				else{
					// ���������� ��������� �����.
					highlightedNumber = 0;
				}
				// ������������� ���� ��� ���������� ������.
				screenChanged = 1;
				// ������������� ���� ����, ��� ������� ����������.
				modeButton.handled = 1;
			}
		}
		// ��� ������� ������ ���������� ����������� ����� �� ����������.
		else{
			modeButton.handled = 0;
		}
		// ���� � ������ ������� ������ �����.
		if(mode == 0){
			// ������ ��� ��������� ����������.
			stopAlarmButton.state = GPIOA->IDR & (1 << stopAlarmButton.pin);
			if(stopAlarmButton.state == 0){
				// ���� ������ ��� �� ����������.
				if(stopAlarmButton.handled == 0){
					// ��������� �������� ������.
					buzzer = 0;
					// ���������� ���� �������������� ����������.
					alarmSet = 0;
					// ������������� ���� ����, ��� ������� ����������.
					stopAlarmButton.handled = 1;
				}
			}
			// ��� ������� ������ ���������� ����������� ����� �� ����������.
			else{
				stopAlarmButton.handled = 0;
			}
			// ���� ����� ����� ������� �� �������� ���������� � ��������� ��� �������.
			if(clockTime.hourTens == alarmTime.hourTens && clockTime.hourUnits == alarmTime.hourUnits && 
			   clockTime.minuteTens == alarmTime.minuteTens && clockTime.minuteUnits == alarmTime.minuteUnits &&
			   alarmSet){
				// ���������� ���� �������������� ����������.
				alarmSet = 0;
				// �������� �������� ������.
				buzzer = 1;
			}
		}
		// ���� �� � ������ ������� ������ �����.
		else{
			if(mode == 2){
				alarmSet = 1;
			}
			// ������ ��� ��������� �������������� �����.
			highlightButton.state = GPIOA->IDR & (1 << highlightButton.pin);
			// ���������� ������������ �������.
			if(BUTTONS_DebounceHandler(highlightButton.state, highlightButton.number, BUTTON_DEBOUNCE_LIMIT)){
				// ���� ������ ��� �� ����������.
				if(highlightButton.handled == 0){
					// ���������� ����������� �������������� ����� �� 4 ������������.
					highlightedNumber++;
					if(highlightedNumber > 4){
						highlightedNumber = 1;
					}
					// ������������� ���� ��� ���������� ������.
					screenChanged = 1;
					// ������������� ���� ����, ��� ������� ����������.
					highlightButton.handled = 1;
				}
			}
			// ��� ������� ������ ���������� ����������� ����� �� ����������.
			else{
				highlightButton.handled = 0;
			}
			// ������ ��� ���������� �������������� �����.
			incNumberButton.state = GPIOA->IDR & (1 << incNumberButton.pin);
			// ���������� ������������ �������.
			if(BUTTONS_DebounceHandler(incNumberButton.state, incNumberButton.number, BUTTON_DEBOUNCE_LIMIT)){
				// ���� ������ ��� �� ����������.
				if(incNumberButton.handled == 0){
					// ����������� ������������ �����.
					AddTime(mode, highlightedNumber);
					// ������������� ���� ��� ���������� ������.
					screenChanged = 1;
					// ������������� ���� ����, ��� ������� ����������.
					incNumberButton.handled = 1;
				}
			}
			// ��� ������� ������ ���������� ����������� ����� �� ����������.
			else{
				incNumberButton.handled = 0;
			}
		}
		// ���� ���������� �������� �����.
		if(screenChanged){
			// ��������� ����� �������� ������� ����������.
			UpdateScreen();
			// ���������� ���� ����, ��� ���������� �������� �����.
			screenChanged = 0;
		}
	}
}