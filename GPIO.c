#include "GPIO.h"

#include "stm32f10x.h"

/*----------------------------------------------------------------
 * GPIO_EnablePinInput(pin_number : uint8_t - ����� ���� ��� 
 *											  ���������
 *					   ) : void
 * ������� ��� ��������� ���� � �������� ��������� ��� ����� A ��
 * ��� ������.
 *----------------------------------------------------------------*/
void GPIO_EnablePinInput(uint8_t pin_number){
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
 * GPIO_EnablePinInput(pin_number : uint8_t - ����� ���� ��� 
 *											  ���������
 *					   ) : void
 * ������� ��� ��������� ���� � �������� �������� ��� ����� A ��
 * ��� ������.
 *----------------------------------------------------------------*/
void GPIO_EnablePinOutput(uint8_t pin_number){
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
 * GPOI_Init(ledPin : uint8_t - ����� ���� ����������
 *			 button1Pin : uint8_t - ����� ���� ������ ������
 *			 button2Pin : uint8_t - ����� ���� ������ ������
 *			 button3Pin : uint8_t - ����� ���� ������� ������
 *			 button4Pin : uint8_t - ����� ���� ��������� ������
 *			 ) : void
 * ������� ��� ������������ ������ �����-������, ��������
 * ������������ �������.
 *----------------------------------------------------------------*/
void GPIO_Init (uint8_t ledPin, uint8_t button1Pin, uint8_t button2Pin, uint8_t button3Pin, uint8_t button4Pin) {
	// ��������� ������������ ����� A
	RCC->APB2ENR |= (1 << 2);
	
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	
	// ����������� ��� �� ����� (��� ����������)
	GPIO_EnablePinOutput(ledPin);
	// ����������� ���� �� ���� (��� ������)
	GPIO_EnablePinInput(button1Pin);
	GPIO_EnablePinInput(button2Pin);
	GPIO_EnablePinInput(button3Pin);
	GPIO_EnablePinInput(button4Pin);
}