#include "buttons.h"
#include "stdint.h"

// ������ ���������, ������ ������ ����� ���� �������
uint32_t buttonCount[4];

/*--------------------------------------------------------------------------------
 * BUTTONS_DebounceHandler (buttonState : uint16_t 	- ������ � ����, � �������� 
											  ���������� ������.
 *				   buttonNumber : uint8_t   - ����� ������.
 *				   debounceLimit : uint32_t - ����� �������� ������, ����� ��������
											  ��� ����� ��������� ��������������
											  �������.
 * 				   ) : uint8_t - �������� ��������� ������ (������, �� ������)
 * ������� ��� ����������� ��������� �������� ������.
 *--------------------------------------------------------------------------------*/
uint8_t BUTTONS_DebounceHandler(uint16_t buttonState, uint8_t buttonNumber, uint32_t debounceLimit){
	// ���� ������ �� ������
	if(buttonState == 0){
		// ���� ������� ������ ����, ��������� ���
		if(buttonCount[buttonNumber] > 0){
			buttonCount[buttonNumber]--;
			// �������, ��� ������ ������
			return 1;
		}
		else{
			if(buttonState == 0){
				// ������ �������������� �� ������
				return 0; 
			}			
		}
	}
	// ���� ������ ������
	else{
		// ���� ������� ������ ������, ����������� ���
		if(buttonCount[buttonNumber] < debounceLimit){
			buttonCount[buttonNumber]++;
			// �������, ��� ������ �� ������
			return 0;
		}
		else{
			if(buttonState != 0){
				// ������ �������������� ������
				return 1; 
			}
		}
	}
}
