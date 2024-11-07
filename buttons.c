#include "buttons.h"
#include "stdint.h"

// Массив счетчиков, каждая кнопка имеет свой счетчик
uint32_t buttonCount[4];

/*--------------------------------------------------------------------------------
 * BUTTONS_DebounceHandler (buttonState : uint16_t 	- сигнал с пина, к которому 
											  присоедена кнопка.
 *				   buttonNumber : uint8_t   - номер кнопки.
 *				   debounceLimit : uint32_t - лимит счетчика кнопки, после которого
											  она будет считаться гарантированно
											  нажатой.
 * 				   ) : uint8_t - условное состояние кнопки (нажата, не нажата)
 * Функция для программной обработки дребезга кнопки.
 *--------------------------------------------------------------------------------*/
uint8_t BUTTONS_DebounceHandler(uint16_t buttonState, uint8_t buttonNumber, uint32_t debounceLimit){
	// Если кнопка не нажата
	if(buttonState == 0){
		// Если счетчик больше нуля, уменьшаем его
		if(buttonCount[buttonNumber] > 0){
			buttonCount[buttonNumber]--;
			// Считаем, что кнопка нажата
			return 1;
		}
		else{
			if(buttonState == 0){
				// Кнопка гарантированно не нажата
				return 0; 
			}			
		}
	}
	// Если кнопка нажата
	else{
		// Если счетчик меньше лимита, увеличиваем его
		if(buttonCount[buttonNumber] < debounceLimit){
			buttonCount[buttonNumber]++;
			// Считаем, что кнопка не нажата
			return 0;
		}
		else{
			if(buttonState != 0){
				// Кнопка гарантированно нажата
				return 1; 
			}
		}
	}
}
