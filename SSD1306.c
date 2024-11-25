#include "SSD1306.h"

#include "delay.h"
#include "stdint.h"

// Образ строки "SET TIME" для панели режимов.
uint8_t SET_TIME_MESSAGE[63] = 	{0x00, 0x46, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00, 0x7F, 0x49, 0x49,
								 0x49, 0x49, 0x00, 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00,
							     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x7F, 0x01,
								 0x01, 0x00, 0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, 0x00, 0x7F,
								 0x02, 0x04, 0x02, 0x7F, 0x00, 0x00, 0x7F, 0x49, 0x49, 0x49, 0x49,
								 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// Образ строки "SET ALARM" для панели режимов.
uint8_t SET_ALARM_MESSAGE[65] = {0x00, 0x46, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00, 0x7F, 0x49, 0x49,
								 0x49, 0x49, 0x00, 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00,
								 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x09, 0x09, 0x09,
								 0x7E, 0x00, 0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x7E,
								 0x09, 0x09, 0x09, 0x7E, 0x00, 0x00, 0x7F, 0x09, 0x09, 0x09, 0x76,
								 0x00, 0x00, 0x7F, 0x02, 0x04, 0x02, 0x7F, 0x00, 0x00, 0x00};
// Образ для подсветки цифры на панели цифр.
uint8_t HIGHLIGHT_NUMBER[24] = 	{0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
								 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0};
// Образы цифр для отображения на панели часов.
uint8_t NUMBERS[10][15] =	{{0xFF, 0xFF, 0xFF,		// ######
							  0xFF, 0x00, 0xFF,		// ##  ##
							  0xFF, 0x00, 0xFF,		// ##  ##
							  0xFF, 0x00, 0xFF,		// ##  ##
							  0xFF, 0xFF, 0xFF,},	// ######

							 {0xFF, 0xFF, 0x00,		// ####
							  0x00, 0xFF, 0x00,		//   ##
							  0x00, 0xFF, 0x00,		//   ##
							  0x00, 0xFF, 0x00,		//   ##
							  0xFF, 0xFF, 0xFF,},	// ######
							 
							 {0xFF, 0xFF, 0xFF,		// ######
							  0x00, 0x00, 0xFF,		//     ##
							  0xFF, 0xFF, 0xFF,		// ######
							  0xFF, 0x00, 0x00,		// ##
							  0xFF, 0xFF, 0xFF,},	// ######
							 
							 {0xFF, 0xFF, 0xFF,		// ######
							  0x00, 0x00, 0xFF,		//     ##
							  0xFF, 0xFF, 0xFF,		// ######
							  0x00, 0x00, 0xFF,		//     ##
							  0xFF, 0xFF, 0xFF,},	// ######
							 
							 {0xFF, 0x00, 0xFF,		// ##  ##
							  0xFF, 0x00, 0xFF,		// ##  ##
							  0xFF, 0xFF, 0xFF,		// ######
							  0x00, 0x00, 0xFF,		//     ##
							  0x00, 0x00, 0xFF,},	//     ##
							 
							 {0xFF, 0xFF, 0xFF,		// ######
							  0xFF, 0x00, 0x00,     // ##
							  0xFF, 0xFF, 0xFF,		// ######
							  0x00, 0x00, 0xFF,		//     ##
							  0xFF, 0xFF, 0xFF,},	// ######
							 
							 {0xFF, 0xFF, 0xFF,		// ######
							  0xFF, 0x00, 0x00,		// ##
							  0xFF, 0xFF, 0xFF,		// ######
							  0xFF, 0x00, 0xFF,		// ##  ##
							  0xFF, 0xFF, 0xFF,},	// ######
							 
							 {0xFF, 0xFF, 0xFF,		// ######
							  0x00, 0x00, 0xFF,		//     ##
							  0x00, 0x00, 0xFF,		//     ##
							  0x00, 0x00, 0xFF,		//     ##
							  0x00, 0x00, 0xFF,},	//     ##
							 
							 {0xFF, 0xFF, 0xFF,		// ######
							  0xFF, 0x00, 0xFF,		// ##  ##
							  0xFF, 0xFF, 0xFF,		// ######
							  0xFF, 0x00, 0xFF,		// ##  ##
							  0xFF, 0xFF, 0xFF,},	// ######
							 
							 {0xFF, 0xFF, 0xFF,		// ######
							  0xFF, 0x00, 0xFF,		// ##  ##
							  0xFF, 0xFF, 0xFF,		// ######
							  0x00, 0x00, 0xFF,		//     ##
							  0xFF, 0xFF, 0xFF,}};	// ######
// Панель режима работы часов (первая страница дисплея).
uint8_t modePanel[128] = 	   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};
// Панель подсветки цифр (вторая страница дисплея).
uint8_t highlightPanel[128] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};
// Панель часов (4-8 страница дисплея), по умолчанию 22:22.
uint8_t timePanel[80] =		{0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,
							 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0xF0, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
							 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,
							 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0xF0, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
							 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,};
 
/*--------------------------------------------------------------------------------
 * OLED_SendByte (data : uint8_t - байт, который необходимо отправить
 *				  mod : uint8_t  - режим передачи данных,
 *								   0 - передача команды,
 *								   1 - передача данных.
 *				  ) : void
 * Отправка бита информации через интерфейс I2C с указанием режима.
 *--------------------------------------------------------------------------------*/
void OLED_SendByte (uint8_t data, uint8_t mod)
{
	// Передаем сигнал START.
	d_I2C_Start();						
	while(!d_StartEnd);
	// Кладем в регистр DR байт адреса устройства.
	d_I2C_Byte(SSD1306_ADDRESS);
	// Ожидаем конца отправки.
	while(!d_AdrSendEnd);
	d_I2C_SR2_Clear();			
	
	// Режим команд и режим данных.
	if(mod)
		// Отправляем 0x00 - режим команды.
		d_I2C_Byte(0x00);
	else
		// Отправляем 0x40 - режим передачи данных.
		d_I2C_Byte(0x40);
	while(!d_ByteSendEnd);
	// Отправляем данные или команду, в зависимости от режима.
	d_I2C_Byte(data);					// Send data to OLED	
	while(!d_ByteSendEnd);
	// Отправляем сигнал STOP.
	d_I2C_Stop();
	// Убеждаемся что физически передача закончилась.
	while(d_I2C_WaitBusy);	
}

/*--------------------------------------------------------------------------------
 * OLED_Reset (void) : void
 * Функция для сброса дисплея согласно его документации, происходит путем подачи
 * последовательсности сигналов с определенной задержкой.
 *--------------------------------------------------------------------------------*/
void OLED_Reset(void) {
	// Включаем пин.
	GPIOA->CRH   &= ~15ul;
	GPIOA->CRH   |=  1ul ;
	// Подаем последовательность сигналов с необходимой задержкой.
	// На вход RES логическая единица.
	GPIOA->BSRR = GPIO_BSRR_BR9;
	GPIOA->BSRR = GPIO_BSRR_BS8;
	// Задержка 1мс.
	DelayMicro(1000);
	// После этого логический ноль.
	GPIOA->BSRR = ~GPIO_BSRR_BS8;
	GPIOA->BSRR = GPIO_BSRR_BR8;
	// Задержка 10мс.
	DelayMicro(10000);
	// Затем снова логическая единица.
	GPIOA->BSRR = ~GPIO_BSRR_BR8;
	GPIOA->BSRR = GPIO_BSRR_BS8;
	// Задержка 10мс.
	DelayMicro(10000);
}

/*--------------------------------------------------------------------------------
 * OLED_Clear (void) : void
 * Функция для отчистки экрана, заполняет все страницы нулями.
 *--------------------------------------------------------------------------------*/
void OLED_Clear (void)
{
	uint16_t i;
	// Посылаем сигнал START.
	d_I2C_Start();											
	while(!d_StartEnd);
	// Посылаем адрес.
	d_I2C_Byte(SSD1306_ADDRESS);		
	while(!d_AdrSendEnd);
	d_I2C_SR2_Clear();										
	// Режим передачи данных.
	d_I2C_Byte(0x40);												
	// Отправляем нулевые байты в дисплей.
	for(i = 0; i < 1024; i++)
	{
		d_I2C_Byte(0x00);
		while(!d_ByteSendEnd);	
	}
	// Отправляем сигнал STOP.
	d_I2C_Stop();												
	while(I2C1->SR2 & I2C_SR2_BUSY);												
}

/*--------------------------------------------------------------------------------
 * OLED_Init (void) : void
 * Функция для инициализации дисплея и его первоначальной настройки, после
 * выполнения настройки экран отчищается.
 *--------------------------------------------------------------------------------*/
void OLED_Init(void) {
	// Перед тем как отправлять байты нужно сбросить дисплей.
	OLED_Reset ();
	// Передаем команды.
	// Выключаем дисплей перед настройкой.
	OLED_SendByte (0xAE, 1);
	// Режим адресации.
	OLED_SendByte (0x20, 1);
	OLED_SendByte (0x00, 1);
	// Remap строк и столбцов.
	OLED_SendByte (0xA1, 1);
	OLED_SendByte (0xC8, 1);
	// Включаем дисплей обратно.
	OLED_SendByte (0x8D, 1);
	OLED_SendByte (0x14, 1);
	OLED_SendByte (0xAF, 1);
	// Установка начального и конечного адреса строк и столбцов.
	OLED_SendByte (0x21, 1);
	OLED_SendByte (0, 1);
	OLED_SendByte (127, 1);
	OLED_SendByte (0x22, 1);
	OLED_SendByte (0, 1);
	OLED_SendByte (7, 1);
	// Отчистка дисплея (на всякий случай).
	OLED_Clear();
}

/*--------------------------------------------------------------------------------
 * OLED_UpdateScreen (void) : void
 * Заполняет экран информацией, находящейся в переменных modePanel,
 * highlightPanel, clockPanel.
 *--------------------------------------------------------------------------------*/
void OLED_UpdateScreen(void)
{
	// Вспомагательные переменные.
	uint16_t i;
	uint8_t j;
	uint8_t high;
	uint8_t low;
	// Посылаем сигнал START.
	d_I2C_Start();											
	while(!d_StartEnd);
	// Посылаем адрес
	d_I2C_Byte(SSD1306_ADDRESS);		
	while(!d_AdrSendEnd);
	d_I2C_SR2_Clear();										
	// Режим передачи данных.
	d_I2C_Byte(0x40);
	// Отображение режима редактирования часов.
	for(i = 0; i < 128; i++){
		d_I2C_Byte(modePanel[i]);
		while(!d_ByteSendEnd);
	}
	// Отображение подсветки редактируемой цифры.
	for(i = 0; i < 128; i++){
		d_I2C_Byte(highlightPanel[i]);	
		while(!d_ByteSendEnd);
	}
	// Пустая строка (страница).
	for(i = 256; i < 384; i++)
	{
		d_I2C_Byte(0x00);
		while(!d_ByteSendEnd);	
	}
	// Отображение панели часов (разбита на элементы по 4 байта).
	for(i = 0; i < 80; i++){
		high = (timePanel[i] & 0xF0) | (timePanel[i] & 0xF0) >> 4;
		low = (timePanel[i] & 0x0F) | (timePanel[i] & 0x0F) << 4;
		for(j = 0; j < 4; j++){
			d_I2C_Byte(high);
			while(!d_ByteSendEnd);
		}
		for(j = 0; j < 4; j++){
			d_I2C_Byte(low);
			while(!d_ByteSendEnd);
		}
	}
	
	// Отправляем сигнал STOP.
	d_I2C_Stop();												
	while(I2C1->SR2 & I2C_SR2_BUSY);												
}

/*--------------------------------------------------------------------------------
 * OLED_WriteInTimePanel (number : uint8_t - цифра, которую необходимо записать.
 *						  base : uint8_t   - база положения цифры на дисплее, считается 
 *											 как количество байтов слева от края панели.
 *						  ) : void
 * Запись цифры в определенное место на панели времени.
 *--------------------------------------------------------------------------------*/
void OLED_WriteInTimePanel(uint8_t number, uint8_t  base){
	uint16_t i;
	uint16_t offset;
	for(i = 0; i < 15; i++){
		offset = i % 3 + i / 3 * 16;
		timePanel[base + offset] = NUMBERS[number][i];
	}
}

/*--------------------------------------------------------------------------------
 * OLED_WriteTime (hourTens : uint8_t 	 - десятки часов.
 *				   hourUnits : uint8_t   - единицы часов.
 *				   minuteTens : uint8_t  - десятки минут.
 * 				   minuteUnits : uint8_t - единицы минут.
 * 				   mode : uint8_t - режим работы будильника,
 *									0 - не будет отображаться,
 *									1 - SET TIME,
 *							    	2 - SET ALARM.
 * 				   highlightedNumber - номер подсвечиваемой цифры слева направо,
 *								       0 - не будет отображаться.
 * 				   ) : void
 * Вывод на экран информации, указанной в аргументах функции.
 *--------------------------------------------------------------------------------*/
void OLED_WriteTime(uint8_t hourTens, uint8_t hourUnits, uint8_t minuteTens, uint8_t minuteUnits, uint8_t mode, uint8_t highlightedNumber){
	// Заполняем панель времени.
	OLED_WriteInTimePanel(hourTens, 0);
	OLED_WriteInTimePanel(hourUnits, 4);
	OLED_WriteInTimePanel(minuteTens, 9);
	OLED_WriteInTimePanel(minuteUnits, 13);
	
	uint16_t i;
	// Заполняем панель режимов.
	if(mode == 1){
		for(i = 0; i < 63; i++){
			modePanel[i] = SET_TIME_MESSAGE[i];
		}
	}
	else{
		for(i = 0; i < 63; i++){
			modePanel[i] = 0x00;
		}
	}
	if(mode == 2){
		for(i = 0; i < 65; i++){
			modePanel[i + 63] = SET_ALARM_MESSAGE[i];
		}
	}
	else{
		for(i = 0; i < 65; i++){
			modePanel[i + 63] = 0x00;
		}
	}
	// Заполняем панель подсветки цифр.
	if(highlightedNumber == 0){
		for(i = 0; i < 128; i++){
			highlightPanel[i] = 0x00;
		}
	}
	else{
		uint16_t offset = (highlightedNumber - 1) * 32;
		if(offset > 48){
			offset += 8;
		}
		for(i = 0; i < offset; i++){
			highlightPanel[i] = 0x00;
		}
		for(i = offset; i < offset + 24; i++){
			highlightPanel[i] = 0xF0;
		}
		for(i = offset + 24; i < 128; i++){
			highlightPanel[i] = 0x00;
		}
	}
	// Вывод образов заполненных панелей на экран.
	OLED_UpdateScreen();
}