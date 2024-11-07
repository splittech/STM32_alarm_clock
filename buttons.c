#include "stdint.h"

uint32_t buttonCount[4];

uint8_t DebounceHandler(uint16_t buttonState, uint8_t buttonNumber, uint32_t debounceLimit){
	if(buttonState == 0){
		if(buttonCount[buttonNumber] > 0){
			buttonCount[buttonNumber]--;
			return 1;
		}
		else{
			if(buttonState == 0){
				return 0; 
			}			
		}
	}
	else{
		if(buttonCount[buttonNumber] < debounceLimit){
			buttonCount[buttonNumber]++;
			return 0;
		}
		else{
			if(buttonState != 0){
				return 1; 
			}
		}
	}
}