#include "buttons.h"
#include "stdint.h"

// Массив счетчиков, каждая кнопка имеет свой счетчик.
uint32_t buttonCounts[4];
// Массив конечных состояний кнопок, являются переключаелями.
uint8_t buttonStates[4];

/*--------------------------------------------------------------------------------
 * BUTTONS_DebounceHandler (buttonState : uint16_t 	- сигнал с пина, к которому 
 *													  присоедена кнопка.
 *				   			buttonNumber : uint8_t   - номер кнопки.
 *				   			debounceLimit : uint32_t - лимит счетчика кнопки, после которого
 *													   она будет считаться гарантированно
 *													   нажатой.
 * 				   		    ) : uint8_t - условное состояние кнопки (нажата, не нажата)
 * Функция для программной обработки дребезга кнопки.
 *--------------------------------------------------------------------------------*/
uint8_t BUTTONS_DebounceHandler(uint16_t pinState, uint8_t buttonNumber, uint32_t debounceLimit){
	// Если есть сигнал на пине.
	if(pinState != 0){
		// Если счетчик больше нуля, уменьшаем его.
		if(buttonCounts[buttonNumber] > 0){
			buttonCounts[buttonNumber]--;
		}
		// Если достигли нуля переключаем состояние копки в положение "не нажата".
		else{
			buttonStates[buttonNumber] = 0;		
		}
	}
	// Если нет сигнала на пине.
	else{
		// Если счетчик меньше лимита, увеличиваем его.
		if(buttonCounts[buttonNumber] < debounceLimit){
			buttonCounts[buttonNumber]++;
		}
		// Если достигли лимита переключаем состояние копки в положение "нажата".
		else{
			buttonStates[buttonNumber] = 1;
		}
	}
	// Возвращаем состояние соответствующей кнопки.
	return buttonStates[buttonNumber];
}
