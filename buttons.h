#include "stdint.h"

/*--------------------------------------------------------------------------------
 * BUTTONS_DebounceHandler (buttonState : uint16_t 	- ������ � ����, � �������� 
													  ���������� ������.
 *				   			buttonNumber : uint8_t   - ����� ������.
 *				   			debounceLimit : uint32_t - ����� �������� ������, ����� ��������
													   ��� ����� ��������� ��������������
													   �������.
 * 				   		    ) : uint8_t - �������� ��������� ������ (������, �� ������)
 * ������� ��� ����������� ��������� �������� ������.
 *--------------------------------------------------------------------------------*/
uint8_t BUTTONS_DebounceHandler(uint16_t buttonState, uint8_t buttonNumber, uint32_t debounceLimit);