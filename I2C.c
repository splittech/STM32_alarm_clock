#include "I2C.h"

#include "stdint.h"

#define I2C_MODE_I2C                     0x00000000U

/*--------------------------------------------------------------------------------
 * I2C_Init (void) : void
 * ������� ��� ������������� ��������� I2C. SCL - PB8, SDA - PB9.
 *--------------------------------------------------------------------------------*/
void I2C_Init(void)
{
		// ��������� ���������� ��� ������.
		uint32_t tmpreg;
		// ���������� ������������ � ������ �������������� �������.
		SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
		// �������� ����� ���������� ������������ (����������� ��� ��� ����� ������ �� ��������).
		tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
		SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
		// ��������� PB8, PB9 � �������� �������� ����� � ������ open-drain.
		SET_BIT(GPIOB->CRH, GPIO_CRH_CNF9_1 | GPIO_CRH_CNF8_1 | GPIO_CRH_CNF9_0 | GPIO_CRH_CNF8_0 |\
				GPIO_CRH_MODE9_1 | GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_0 ); 
		// ���������� ������������ ������� ���������� I2C (�� ���� ���).
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
		// �������� ����� ����������.
		tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
		// ����� ���������� ���������� ��������� ���� I2C.
		CLEAR_BIT(I2C1->CR1, I2C_CR1_PE);
		// ��������� ������� �������� ������ (����������������� ������� SCL).
		MODIFY_REG(I2C1->CR2, I2C_CR2_FREQ, 36);
		MODIFY_REG(I2C1->TRISE, I2C_TRISE_TRISE, 36 + 1); 
		// Thigh = Tlow = 180 * (1/36) = 5 mcs => f = 100 kHz.
		MODIFY_REG(I2C1->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), 180); 
		// �������� ����� ������ ��� I2C (���� ��� SMBUS).
		MODIFY_REG(I2C1->CR1, I2C_CR1_SMBUS | I2C_CR1_SMBTYPE | I2C_CR1_ENARP, I2C_MODE_I2C);
		// ������� �������� ���� I2C ����� ���������.
		SET_BIT(I2C1->CR1, I2C_CR1_PE);
		// ��������� �������� ������� �������������.
		MODIFY_REG(I2C1->CR1, I2C_CR1_ACK, I2C_CR1_ACK);
		// ��������������� 7-������ ����������� ����� (���� ����� ����� �������������� � �������� ��������).
		// � ������ ������ ��������� ����� ��� 0, �� ��������������� � �������� ��������.
		MODIFY_REG(I2C1->OAR2, I2C_OAR2_ADD2, 0);
		// �������������� ���������� I2C � PB6,PB7 �� PB8,PB9.
		// ��� ��� �� ����� nucleo ������������ ������ ����.
		AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
}
