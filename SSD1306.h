#include "stm32f10x.h"
#include "I2C.h"

// ����� �������
#define SSD1306_ADDRESS  0x78

/*--------------------------------------------------------------------------------
 * OLED_Init (void) : void
 * ������� ��� ������������� ������� � ��� �������������� ���������, �����
 * ���������� ��������� ����� ����������.
 *--------------------------------------------------------------------------------*/
void OLED_Init(void);

/*--------------------------------------------------------------------------------
 * OLED_WriteTime (hourTens : uint8_t 	 - ������� �����.
 *				   hourUnits : uint8_t   - ������� �����.
 *				   minuteTens : uint8_t  - ������� �����.
 * 				   minuteUnits : uint8_t - ������� �����.
 * 				   mode : uint8_t - ����� ������ ����������,
 *									0 - �� ����� ������������,
 *									1 - SET TIME,
 *							    	2 - SET ALARM.
 * 				   highlightedNumber - ����� �������������� ����� ����� �������,
 *								       0 - �� ����� ������������.
 * 				   ) : void
 * ����� �� ����� ����������, ��������� � ���������� �������.
 *--------------------------------------------------------------------------------*/
void OLED_WriteTime(uint8_t hourTens, uint8_t hourUnits, uint8_t minuteTens, uint8_t minuteUnits, uint8_t mode, uint8_t highlightedNumber);