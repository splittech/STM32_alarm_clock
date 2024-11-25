// CMSIS.
#include "stm32f10x.h"

// Собственные библиотеки.
#include "SSD1306.h"
#include "GPIO.h"
#include "I2C.h"
#include "timers.h"
#include "buttons.h"

/*------------------------------ОПИСАНИЕ ПРОЕКТА----------------------------------*/
//  ОПИСАНИЕ: Проект представляет собой устройство "Часы с будильником".
//  ПЕРИФЕРИЯ: Плата Nucleo STM32 f103rb, 4 кнопки, экран SSD1306, активный зуммер.
//	РАБОТА: У устройства есть три режима работы:
//										0 - штатный ражим работы.
//										1 - режим редактирования часов.
//										2 - режим редактировани будильника.
//  Переключение режима происходит циклически при нажатии кнопки смены режима.
//  В режиме редактирования можно редактировать время путем циклического увеличения
//  выбранной цифры при помощи кнопки увеличения цифры. Изменить выбранную цифру
//  можно при помощи кнопки выбора цифры. После выхода из режима редактирования
//  будильника будильник автоматически считается заведенным. Для того, чтобы
//	остановить будильник или сбросить заведенный будильник, необходимо нажать на
//	кнопку остановки будильника.
/*--------------------------------------------------------------------------------*/

/*------------------------------НАСТРОЙКИ ПРОЕКТА---------------------------------*/
uint8_t LED_PIN = 9;					 // Номер пина для зуммера.

uint8_t MODE_BUTTON_PIN = 5;			 // Номер пина для кнопки смены режима.

uint8_t HIGHLIGHT_BUTTON_PIN = 6;		 // Номер пина для кнопки выбора цифры.

uint8_t INCREASE_NUMBER_BUTTON_PIN = 7;  // Номер пина для кнопки увеличения цифры.

uint8_t STOP_ALARM_BUTTON_PIN = 8;		 // Номер пина для кнопки остановки будильника.

uint32_t BUTTON_DEBOUNCE_LIMIT = 10000;	 // Лимит счетчика для программной обработки 
										 //	дребезга кнопки.
/*--------------------------------------------------------------------------------*/

// Режим работы будильника.
uint8_t mode;
// Подсвечиваемая цифра.
uint8_t highlightedNumber;
// Общий счетчик секунд в сутках.
uint32_t totalSeconds;
// Флаг того, что будильник заведен.
uint8_t alarmSet;
// Флаг того, что будильник заведен.
uint8_t screenChanged;
// Флаг для подачи звукового сигнала.
uint8_t buzzer;

// Структура циферблата времени.
struct time{
	uint8_t hourTens;	  // Десятки часов.
	uint8_t hourUnits;	  // Единицы часов.
	uint8_t minuteTens;	  // Десятки минут.
	uint8_t minuteUnits;  // Единицы минут.
};
// Время часов.
struct time clockTime;
// Время будильника.
struct time alarmTime;

// Структура параметров кнопки.
struct button{
	uint8_t number;	  // Номер кнопки.
	uint8_t pin;	  // Номер пина кнопки.
	uint16_t state;	  // Состояние кнопки.
	uint8_t handled;  // Обработана ли кнопка.
};
// Кнопка для изменения режима работы.
struct button modeButton;
// Кнопка для изменения подсвечиваемой цифры.
struct button highlightButton;
// Кнопка для увеличения подсвечиваемой цифры.
struct button incNumberButton;
// Кнопка для остановки будильника или сброса заведенного будильника.
struct button stopAlarmButton;

/*--------------------------------------------------------------------------------
 * SystemCoreClockConfigure (void) : void
 * Функция для настройки тактовой частоты микроконтроллера.
 *--------------------------------------------------------------------------------*/
void SystemCoreClockConfigure(void) {
	// Включаем HSI.
	RCC->CR |= ((uint32_t)RCC_CR_HSION);
	// Ждем запуска HSI.
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);
	// Устанавилваем HSI в качестве источника тактирования.
	RCC->CFGR = RCC_CFGR_SW_HSI;
	// Ожидаем окончания установки.
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
	// Настраиваем делители.
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
	// Выключаем PLL перед настройкой.
	RCC->CR &= ~RCC_CR_PLLON;
	// Настраиваем PLL.
	// PLL configuration:  = HSI/2 * 12 = 48 MHz.
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
	RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL12);
	// Включаем PLL после настройки.
	RCC->CR |= RCC_CR_PLLON;
	// Ожидаем включения PLL.
	while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();
	// Устанавилваем PLL в качестве источника тактирования.
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |=  RCC_CFGR_SW_PLL;
	// Ожидаем окончания установки.
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait till PLL is system clock src
}

/*--------------------------------------------------------------------------------
 * UpdateScreen (void) : void
 * Функция для обновления экрана с использованием текущих параметров.
 *--------------------------------------------------------------------------------*/
void UpdateScreen(void){
	// Если выбран режим редактирования будильника,
	// то будет показываться время будильника.
	if(mode == 2){
		OLED_WriteTime(alarmTime.hourTens,
				   alarmTime.hourUnits,
				   alarmTime.minuteTens,
				   alarmTime.minuteUnits,
				   mode,
				   highlightedNumber);
	}
	// Во всех остальных случаях будет показываться время часов.
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
 * UpdateScreen (pinNumber : uint8_t - номер пина, на котором нужно инвертировать сигнал
				 ) : void
 * Функция для инверсии сигнала указанного пина на порту А.
 *--------------------------------------------------------------------------------*/
void SwitchPin(uint8_t pinNumber){
	// Если есть сигнал на пине.
	if(GPIOA->ODR & (1 << pinNumber)){
		// Сбрасываем его.
		GPIOA->BSRR = 1 << pinNumber + 16;
	}
	// Если нет сигнала на пние.
	else{
		// Устанавливаем его.
		GPIOA->BSRR = 1 << pinNumber;
	}
}

/*----------------------------------------------------------------
 * TIM3_IRQHandler(void) : void
 * Обработчик прерываний таймера TIM3. (раз в секунду)
 *----------------------------------------------------------------*/
void TIM3_IRQHandler () {
	// Сброс флага прерывания.
	TIM3->SR &= ~TIM_SR_UIF;
	// Если в режиме штатной работы.
	if(mode == 0){
		// Увеличиваем общий счетчик секунд.
		totalSeconds++;
		// Сбрасываем, если переполнилось количество секунд в сутках.
		if(totalSeconds == 86400){
			totalSeconds = 0;
		}
		// Записываем в таймер в структуру.
		clockTime.hourTens = totalSeconds / 36000;
		clockTime.hourUnits = totalSeconds / 3600 % 10;
		clockTime.minuteTens = totalSeconds / 600 % 6;
		clockTime.minuteUnits = totalSeconds / 60 % 10;
		// Устанавливаем флаг обновления экрана.
		screenChanged = 1;
	}
}

/*----------------------------------------------------------------
 * TIM3_IRQHandler(void) : void
 * Обработчик прерываний таймера TIM3. (раз в секунду)
 *----------------------------------------------------------------*/
void TIM4_IRQHandler () {
	// Сброс флага прерывания
	TIM4->SR &= ~TIM_SR_UIF;
	// Если нужно издавать сигнал
	if(buzzer || GPIOA->ODR & (1 << LED_PIN)){
		// Инверсия сигнала на пине
		SwitchPin(LED_PIN);
	}
}

/*--------------------------------------------------------------------------------
 * AddTime (timeType : uint8_t - тип часов, для которых добавляется время. 
 *			number : uint8_t   - номер цифры в циферблате, которую нужно увелчить.
 *			) : void
 * Универсальная функция, которая циклическидобавляет единицу к указанной цифре
 * циферблата часов или будильника.
 *--------------------------------------------------------------------------------*/
void AddTime(uint8_t timeType, uint8_t number){
	// Если нужно изменить часы.
	if(timeType == 1){
		// Если нужно изменить первую цифру.
		if(number == 1){
			// Увеличиваем цифру.
			clockTime.hourTens++;
			// Для десятков часов, сбрасываем, если превысили 2.
			if(clockTime.hourTens > 2){
				clockTime.hourTens = 0;
			}
			// Если установили цифру 2 и количество единиц часов больше чем 3
			if(clockTime.hourTens == 2){
				if(clockTime.hourUnits > 3){
					// Уменьшаем единицы часов до 3 - это предел
					// (чтобы не было больше чем 23 часа)
					clockTime.hourUnits = 3;
				}
			}
		}
		// Если нужно изменить вторую цифру.
		else if(number == 2){
			// Увеличиваем цифру.
			clockTime.hourUnits++;
			// Для единиц часов, сбрасываем, если превысили 9.
			if(clockTime.hourUnits > 9){
				clockTime.hourUnits = 0;
			}
			// Если в десятках часов выбрана цифра 2,
			// то сбрасываем, если превысили 3
			if(clockTime.hourTens == 2){
				if(clockTime.hourUnits > 3){
					clockTime.hourUnits = 0;
				}
			}
			
		}
		// Если нужно изменить третью цифру.
		else if(number == 3){
			// Увеличиваем цифру.
			clockTime.minuteTens++;
			// Для десятков минут, сбрасываем, если превысили 5.
			if(clockTime.minuteTens > 5){
				clockTime.minuteTens = 0;
			}
		}
		// Если нужно изменить четвертую цифру.
		else if(number == 4){
			// Увеличиваем цифру.
			clockTime.minuteUnits++;
			// Для единиц минут, сбрасываем, если превысили 5.
			if(clockTime.minuteUnits > 9){
				clockTime.minuteUnits = 0;
			}
		}
		// Обновляем общий счетчки секунд, на основании измененных параметров.
		totalSeconds = clockTime.hourTens * 36000 + clockTime.hourUnits * 3600 + clockTime.minuteTens * 600 + clockTime.minuteUnits * 60;
	}
	// Если нужно изменить часы.
	else{
		// Все то же самое.
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
	// Настройка тактовой частоты
	SystemCoreClockConfigure();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000000); 
	
	// Первичная настройка кнопки смены режима.
	modeButton.number = 0;
	modeButton.pin = MODE_BUTTON_PIN;
	// Первичная настройка кнопки для изменеия выделенной цифры.
	highlightButton.number = 1;
	highlightButton.pin = HIGHLIGHT_BUTTON_PIN;
	// Первичная настройка кнопки увеличения выделенной цифры.
	incNumberButton.number = 2;
	incNumberButton.pin = INCREASE_NUMBER_BUTTON_PIN;
	// Первичная настройка кнопки для остановки будильника.
	stopAlarmButton.number = 3;
	stopAlarmButton.pin = STOP_ALARM_BUTTON_PIN;
	// Инициализация портов ввода-вывода .
	GPIO_Init(LED_PIN, modeButton.pin, highlightButton.pin, incNumberButton.pin, stopAlarmButton.pin);
	// Инициализация интерфейса I2C.
	I2C_Init();
	// Инициализация дисплея.
	OLED_Init();
	UpdateScreen();
	//switchPin(LED_PIN);
	// Инициализация таймеров общего назначения.
	TIM3_Init();
	TIM4_Init();
	while (1) {
		// Обработка кнопки для изменения режима часов.
		modeButton.state = GPIOA->IDR & (1 << modeButton.pin);
		// Программно обрабатываем дребезг.
		if(BUTTONS_DebounceHandler(modeButton.state, modeButton.number, BUTTON_DEBOUNCE_LIMIT)){
			// Если кнопка еще не обработана.
			if(modeButton.handled == 0){
				// Изменяем режим.
				mode++;
				// Сбрасываем по кругу.
				if(mode == 3){
					mode = 0;
				}
				// Если перешли в режим редактирования.
				if(mode > 0){
					// Подсвечиваем первую цифру.
					highlightedNumber = 1;
				}
				// Если перешли в режим штатной работы.
				else{
					// Сбрасываем подсветку цифры.
					highlightedNumber = 0;
				}
				// Устанавливаем флаг для обновления экрана.
				screenChanged = 1;
				// Устанавливаем флаг того, что нажатие обработано.
				modeButton.handled = 1;
			}
		}
		// При отжатии кнопки появляется возможность снова ее обработать.
		else{
			modeButton.handled = 0;
		}
		// Если в режиме штатной работы часов.
		if(mode == 0){
			// Кнопка для остановки будильника.
			stopAlarmButton.state = GPIOA->IDR & (1 << stopAlarmButton.pin);
			if(stopAlarmButton.state == 0){
				// Если кнопка еще не обработана.
				if(stopAlarmButton.handled == 0){
					// Выключаем звуковой сигнал.
					buzzer = 0;
					// Сбрасываем флаг установленного будильника.
					alarmSet = 0;
					// Устанавливаем флаг того, что нажатие обработано.
					stopAlarmButton.handled = 1;
				}
			}
			// При отжатии кнопки появляется возможность снова ее обработать.
			else{
				stopAlarmButton.handled = 0;
			}
			// Если время часов совпало со временем будильника и будильник был заведен.
			if(clockTime.hourTens == alarmTime.hourTens && clockTime.hourUnits == alarmTime.hourUnits && 
			   clockTime.minuteTens == alarmTime.minuteTens && clockTime.minuteUnits == alarmTime.minuteUnits &&
			   alarmSet){
				// Сбрасываем флаг установленного будильника.
				alarmSet = 0;
				// Включаем звуковой сигнал.
				buzzer = 1;
			}
		}
		// Если не в режиме штатной работы часов.
		else{
			if(mode == 2){
				alarmSet = 1;
			}
			// Кнопка для изменения подсвечиваемой цифры.
			highlightButton.state = GPIOA->IDR & (1 << highlightButton.pin);
			// Программно обрабатываем дребезг.
			if(BUTTONS_DebounceHandler(highlightButton.state, highlightButton.number, BUTTON_DEBOUNCE_LIMIT)){
				// Если кнопка еще не обработана.
				if(highlightButton.handled == 0){
					// Циклически увиличиваем подсвечиваемую цифру до 4 включительно.
					highlightedNumber++;
					if(highlightedNumber > 4){
						highlightedNumber = 1;
					}
					// Устанавливаем флаг для обновления экрана.
					screenChanged = 1;
					// Устанавливаем флаг того, что нажатие обработано.
					highlightButton.handled = 1;
				}
			}
			// При отжатии кнопки появляется возможность снова ее обработать.
			else{
				highlightButton.handled = 0;
			}
			// Кнопка для увеличения подсвечиваемой цифры.
			incNumberButton.state = GPIOA->IDR & (1 << incNumberButton.pin);
			// Программно обрабатываем дребезг.
			if(BUTTONS_DebounceHandler(incNumberButton.state, incNumberButton.number, BUTTON_DEBOUNCE_LIMIT)){
				// Если кнопка еще не обработана.
				if(incNumberButton.handled == 0){
					// Увеличиваем подсвеченную цифру.
					AddTime(mode, highlightedNumber);
					// Устанавливаем флаг для обновления экрана.
					screenChanged = 1;
					// Устанавливаем флаг того, что нажатие обработано.
					incNumberButton.handled = 1;
				}
			}
			// При отжатии кнопки появляется возможность снова ее обработать.
			else{
				incNumberButton.handled = 0;
			}
		}
		// Если необходимо обновить экран.
		if(screenChanged){
			// Обновляем экран согласно текущим параметрам.
			UpdateScreen();
			// Сбрасываем флаг того, что необходимо обновить экран.
			screenChanged = 0;
		}
	}
}