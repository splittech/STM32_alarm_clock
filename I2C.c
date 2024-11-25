#include "I2C.h"

#include "stdint.h"

#define I2C_MODE_I2C                     0x00000000U

/*--------------------------------------------------------------------------------
 * I2C_Init (void) : void
 * Функция для инициализации итерфейса I2C. SCL - PB8, SDA - PB9.
 *--------------------------------------------------------------------------------*/
void I2C_Init(void)
{
		// Временная переменная для чтения.
		uint32_t tmpreg;
		// Разрешение тактирования в режиме альтернативной функции.
		SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
		// задержка после разрешения тактирования (реализована вот так через чтение из регистра).
		tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
		SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
		// Настройка PB8, PB9 в качестве выходных пинов в режиме open-drain.
		SET_BIT(GPIOB->CRH, GPIO_CRH_CNF9_1 | GPIO_CRH_CNF8_1 | GPIO_CRH_CNF9_0 | GPIO_CRH_CNF8_0 |\
				GPIO_CRH_MODE9_1 | GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_0 ); 
		// Разрешение тактирования первого интерфейса I2C (их есть два).
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
		// Задержка после разрешения.
		tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
		// Перед настройкой интерфейса отключаем шину I2C.
		CLEAR_BIT(I2C1->CR1, I2C_CR1_PE);
		// Найстрока частоты передачи данных (синхронизирующего сигнала SCL).
		MODIFY_REG(I2C1->CR2, I2C_CR2_FREQ, 36);
		MODIFY_REG(I2C1->TRISE, I2C_TRISE_TRISE, 36 + 1); 
		// Thigh = Tlow = 180 * (1/36) = 5 mcs => f = 100 kHz.
		MODIFY_REG(I2C1->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), 180); 
		// Выбираем режим работы как I2C (есть еще SMBUS).
		MODIFY_REG(I2C1->CR1, I2C_CR1_SMBUS | I2C_CR1_SMBTYPE | I2C_CR1_ENARP, I2C_MODE_I2C);
		// Обратно включаем шину I2C после настройки.
		SET_BIT(I2C1->CR1, I2C_CR1_PE);
		// Разрешаем отправку сигнала подтверждения.
		MODIFY_REG(I2C1->CR1, I2C_CR1_ACK, I2C_CR1_ACK);
		// Устанавливается 7-битный собственный адрес (если вдруг будет использоваться в качестве ведомого).
		// В данном случае поставили адрес как 0, не предусматриваем в качестве ведомого.
		MODIFY_REG(I2C1->OAR2, I2C_OAR2_ADD2, 0);
		// Переназначение интерфейса I2C с PB6,PB7 на PB8,PB9.
		// так как на плате nucleo используются другие пины.
		AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
}
