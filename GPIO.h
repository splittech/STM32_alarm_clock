#include "stdint.h"

/*----------------------------------------------------------------
 * GPIO_Init(ledPin : uint8_t - номер пина светодиода
 *			 button1Pin : uint8_t - номер пина первой кнопки
 *			 button2Pin : uint8_t - номер пина второй кнопки
 *			 button3Pin : uint8_t - номер пина третьей кнопки
 *			 button4Pin : uint8_t - номер пина четвертой кнопки
 *			 ) : void
 * Функция для инициализаци портов ввода-вывода, согласно
 * конфигурации проекта.
 *----------------------------------------------------------------*/
void GPIO_Init (uint8_t ledPin, uint8_t button1Pin, uint8_t button2Pin, uint8_t button3Pin, uint8_t button4Pin);