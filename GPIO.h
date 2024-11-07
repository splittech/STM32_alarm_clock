#include "stdint.h"

/*----------------------------------------------------------------
 * GPIO_Init(ledPin : uint8_t - ����� ���� ����������
 *			 button1Pin : uint8_t - ����� ���� ������ ������
 *			 button2Pin : uint8_t - ����� ���� ������ ������
 *			 button3Pin : uint8_t - ����� ���� ������� ������
 *			 button4Pin : uint8_t - ����� ���� ��������� ������
 *			 ) : void
 * ������� ��� ������������ ������ �����-������, ��������
 * ������������ �������.
 *----------------------------------------------------------------*/
void GPIO_Init (uint8_t ledPin, uint8_t button1Pin, uint8_t button2Pin, uint8_t button3Pin, uint8_t button4Pin);