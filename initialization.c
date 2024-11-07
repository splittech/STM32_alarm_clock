#include "stm32f10x.h"

/*----------------------------------------------------------------
 * SystemCoreClockConfigure : 
 *----------------------------------------------------------------*/
void SystemCoreClockConfigure(void) {
	// �������� HSE
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);
	// ������� ��������� HSE
	while ((RCC->CR & RCC_CR_HSERDY) == 0);

	// ������������� HSE � �������� ��������� �������������
	RCC->CFGR = RCC_CFGR_SW_HSE;
	// ������� ����������
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);

	// ��������� ������������ ������ ���������� � ������
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // HCLK = SYSCLK
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1; // APB1 = HCLK
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 = HCLK
	
	// � �������� ��������� ������������ ��� ������ HSI ��� ������������� ����������.
	// ����� ����� ���������� ��������� ������������� ���������� (������� ���� - ������ ������ �������),
	// � ����� ��������� �� ������� �����-�� ������ �������� ��������� �������, ��� ��� �������������
	// ������������ ��������� HSI �� ������ ������� ������� ��� ������. ����� ������� ������� SYSCLK �������� 8���
}

/*----------------------------------------------------------------
 * InitTIM3 : 
 *----------------------------------------------------------------*/
void InitTIM3 (void) {
	// �������� ������
	RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
	// �������� ������� �������
	TIM3 -> CR1 = TIM_CR1_CEN;
	
	// ����������� ������������ ������� ��� ����, ����� ������ �������� �������� ����������
	// ���������� ����� ���������� ������ �������
	TIM3->PSC = (SystemCoreClock) / 1000 - 1;
	TIM3->ARR = 1000 - 1;

	// ��������� ����������
	TIM3->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM3_IRQn);
}

/*----------------------------------------------------------------
 * InitTIM4 : 
 *----------------------------------------------------------------*/
void InitTIM4 (void) {
	// �������� ������
	RCC -> APB1ENR |= RCC_APB1ENR_TIM4EN;
	// �������� ������� �������
	TIM4 -> CR1 = TIM_CR1_CEN;
	
	// ����������� ������������ ������� ��� ����, ����� ������ �������� �������� ����������
	// SystemCoreClock / 2 ��������, ��� ���������� ���������� ����� ���������� ������ ��� �������
	TIM4->PSC = (SystemCoreClock / 2) / 1000 - 1;
	TIM4->ARR = 1000 - 1;

	// ��������� ����������
	TIM4->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM4_IRQn);
}

/*----------------------------------------------------------------
 * EnablePinInput : 
 *----------------------------------------------------------------*/
void EnablePinInput(uint8_t pin_number){
	// �������� ����� ����� �� �������� CRL, ������ �������� �� CRH
	if(pin_number < 8){
		// �������� ����� �������� ��� ���������������� ����
		GPIOA->CRL &= ~((15 << 4*pin_number));
		// MODE = 00: input mode
		GPIOA->CRL |= ((0 << 4*pin_number));
		// INPUT MODE = 10: input with pull up/pull down
		GPIOA->CRL |= ((8 << 4*pin_number));
	}
	else{
		// �� �� ����� ��� �������� CRH
		pin_number -= 8;
		GPIOA->CRH &= ~((15 << 4*pin_number));
		GPIOA->CRH |= ((0 << 4*pin_number));
		GPIOA->CRH |= ((8 << 4*pin_number));
	}
}

/*----------------------------------------------------------------
 * EnablePinOutput :
 *----------------------------------------------------------------*/
void EnablePinOutput(uint8_t pin_number){
	// �������� ����� ����� �� �������� CRL, ������ �������� �� CRH
	if(pin_number < 8){
		// �������� ����� �������� ��� ���������������� ����
		GPIOA->CRL &= ~((15 << 4*pin_number));
		// MODE = 01: output mode (10MHz)
		GPIOA->CRL |= ((1 << 4*pin_number));
		// OUTPUT MODE = 00: output push-pull
		GPIOA->CRL |= ((0 << 4*pin_number));
	}
	else{
		// �� �� ����� ��� �������� CRH
		pin_number -= 8;
		GPIOA->CRH &= ~((15 << 4*pin_number));
		GPIOA->CRH |= ((1 << 4*pin_number));
	}
}

/*----------------------------------------------------------------
 * InitGPIO : 
 *----------------------------------------------------------------*/
void InitGPIO (uint8_t ledPin, uint8_t button1Pin, uint8_t button2Pin, uint8_t button3Pin, uint8_t button4Pin) {
	// ��������� ������������ ����� A
	RCC->APB2ENR |= (1 << 2);
	
	// ����������� ��� �� ����� (��� ����������)
	EnablePinOutput(ledPin);
	// ����������� ���s �� ���� (��� ������)
	EnablePinInput(button1Pin);
	EnablePinInput(button2Pin);
	EnablePinInput(button3Pin);
	EnablePinInput(button4Pin);
}