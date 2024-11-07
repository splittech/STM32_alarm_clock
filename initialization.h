#include "stdint.h"

void SystemCoreClockConfigure(void);
void InitTIM3(void);
void InitTIM4(void);
void InitGPIO(uint8_t ledPin, uint8_t button1Pin, uint8_t button2Pin, uint8_t button3Pin, uint8_t button4Pin);
