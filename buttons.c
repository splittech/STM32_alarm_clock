#include "buttons.h"
#include "stdint.h"

// ������ ���������, ������ ������ ����� ���� �������.
uint32_t buttonCounts[4];
// ������ �������� ��������� ������, �������� ��������������.
uint8_t buttonStates[4];

/*--------------------------------------------------------------------------------
 * BUTTONS_DebounceHandler (buttonState : uint16_t 	- ������ � ����, � �������� 
 *													  ���������� ������.
 *				   			buttonNumber : uint8_t   - ����� ������.
 *				   			debounceLimit : uint32_t - ����� �������� ������, ����� ��������
 *													   ��� ����� ��������� ��������������
 *													   �������.
 * 				   		    ) : uint8_t - �������� ��������� ������ (������, �� ������)
 * ������� ��� ����������� ��������� �������� ������.
 *--------------------------------------------------------------------------------*/
uint8_t BUTTONS_DebounceHandler(uint16_t pinState, uint8_t buttonNumber, uint32_t debounceLimit){
	// ���� ���� ������ �� ����.
	if(pinState != 0){
		// ���� ������� ������ ����, ��������� ���.
		if(buttonCounts[buttonNumber] > 0){
			buttonCounts[buttonNumber]--;
		}
		// ���� �������� ���� ����������� ��������� ����� � ��������� "�� ������".
		else{
			buttonStates[buttonNumber] = 0;		
		}
	}
	// ���� ��� ������� �� ����.
	else{
		// ���� ������� ������ ������, ����������� ���.
		if(buttonCounts[buttonNumber] < debounceLimit){
			buttonCounts[buttonNumber]++;
		}
		// ���� �������� ������ ����������� ��������� ����� � ��������� "������".
		else{
			buttonStates[buttonNumber] = 1;
		}
	}
	// ���������� ��������� ��������������� ������.
	return buttonStates[buttonNumber];
}
