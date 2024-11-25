#include "SSD1306.h"

#include "delay.h"
#include "stdint.h"

// ����� ������ "SET TIME" ��� ������ �������.
uint8_t SET_TIME_MESSAGE[63] = 	{0x00, 0x46, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00, 0x7F, 0x49, 0x49,
								 0x49, 0x49, 0x00, 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00,
							     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x7F, 0x01,
								 0x01, 0x00, 0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, 0x00, 0x7F,
								 0x02, 0x04, 0x02, 0x7F, 0x00, 0x00, 0x7F, 0x49, 0x49, 0x49, 0x49,
								 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// ����� ������ "SET ALARM" ��� ������ �������.
uint8_t SET_ALARM_MESSAGE[65] = {0x00, 0x46, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00, 0x7F, 0x49, 0x49,
								 0x49, 0x49, 0x00, 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00,
								 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x09, 0x09, 0x09,
								 0x7E, 0x00, 0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x7E,
								 0x09, 0x09, 0x09, 0x7E, 0x00, 0x00, 0x7F, 0x09, 0x09, 0x09, 0x76,
								 0x00, 0x00, 0x7F, 0x02, 0x04, 0x02, 0x7F, 0x00, 0x00, 0x00};
// ����� ��� ��������� ����� �� ������ ����.
uint8_t HIGHLIGHT_NUMBER[24] = 	{0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
								 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0};
// ������ ���� ��� ����������� �� ������ �����.
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
// ������ ������ ������ ����� (������ �������� �������).
uint8_t modePanel[128] = 	   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};
// ������ ��������� ���� (������ �������� �������).
uint8_t highlightPanel[128] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};
// ������ ����� (4-8 �������� �������), �� ��������� 22:22.
uint8_t timePanel[80] =		{0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,
							 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0xF0, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
							 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,
							 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0xF0, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
							 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,};
 
/*--------------------------------------------------------------------------------
 * OLED_SendByte (data : uint8_t - ����, ������� ���������� ���������
 *				  mod : uint8_t  - ����� �������� ������,
 *								   0 - �������� �������,
 *								   1 - �������� ������.
 *				  ) : void
 * �������� ���� ���������� ����� ��������� I2C � ��������� ������.
 *--------------------------------------------------------------------------------*/
void OLED_SendByte (uint8_t data, uint8_t mod)
{
	// �������� ������ START.
	d_I2C_Start();						
	while(!d_StartEnd);
	// ������ � ������� DR ���� ������ ����������.
	d_I2C_Byte(SSD1306_ADDRESS);
	// ������� ����� ��������.
	while(!d_AdrSendEnd);
	d_I2C_SR2_Clear();			
	
	// ����� ������ � ����� ������.
	if(mod)
		// ���������� 0x00 - ����� �������.
		d_I2C_Byte(0x00);
	else
		// ���������� 0x40 - ����� �������� ������.
		d_I2C_Byte(0x40);
	while(!d_ByteSendEnd);
	// ���������� ������ ��� �������, � ����������� �� ������.
	d_I2C_Byte(data);					// Send data to OLED	
	while(!d_ByteSendEnd);
	// ���������� ������ STOP.
	d_I2C_Stop();
	// ���������� ��� ��������� �������� �����������.
	while(d_I2C_WaitBusy);	
}

/*--------------------------------------------------------------------------------
 * OLED_Reset (void) : void
 * ������� ��� ������ ������� �������� ��� ������������, ���������� ����� ������
 * ������������������� �������� � ������������ ���������.
 *--------------------------------------------------------------------------------*/
void OLED_Reset(void) {
	// �������� ���.
	GPIOA->CRH   &= ~15ul;
	GPIOA->CRH   |=  1ul ;
	// ������ ������������������ �������� � ����������� ���������.
	// �� ���� RES ���������� �������.
	GPIOA->BSRR = GPIO_BSRR_BR9;
	GPIOA->BSRR = GPIO_BSRR_BS8;
	// �������� 1��.
	DelayMicro(1000);
	// ����� ����� ���������� ����.
	GPIOA->BSRR = ~GPIO_BSRR_BS8;
	GPIOA->BSRR = GPIO_BSRR_BR8;
	// �������� 10��.
	DelayMicro(10000);
	// ����� ����� ���������� �������.
	GPIOA->BSRR = ~GPIO_BSRR_BR8;
	GPIOA->BSRR = GPIO_BSRR_BS8;
	// �������� 10��.
	DelayMicro(10000);
}

/*--------------------------------------------------------------------------------
 * OLED_Clear (void) : void
 * ������� ��� �������� ������, ��������� ��� �������� ������.
 *--------------------------------------------------------------------------------*/
void OLED_Clear (void)
{
	uint16_t i;
	// �������� ������ START.
	d_I2C_Start();											
	while(!d_StartEnd);
	// �������� �����.
	d_I2C_Byte(SSD1306_ADDRESS);		
	while(!d_AdrSendEnd);
	d_I2C_SR2_Clear();										
	// ����� �������� ������.
	d_I2C_Byte(0x40);												
	// ���������� ������� ����� � �������.
	for(i = 0; i < 1024; i++)
	{
		d_I2C_Byte(0x00);
		while(!d_ByteSendEnd);	
	}
	// ���������� ������ STOP.
	d_I2C_Stop();												
	while(I2C1->SR2 & I2C_SR2_BUSY);												
}

/*--------------------------------------------------------------------------------
 * OLED_Init (void) : void
 * ������� ��� ������������� ������� � ��� �������������� ���������, �����
 * ���������� ��������� ����� ����������.
 *--------------------------------------------------------------------------------*/
void OLED_Init(void) {
	// ����� ��� ��� ���������� ����� ����� �������� �������.
	OLED_Reset ();
	// �������� �������.
	// ��������� ������� ����� ����������.
	OLED_SendByte (0xAE, 1);
	// ����� ���������.
	OLED_SendByte (0x20, 1);
	OLED_SendByte (0x00, 1);
	// Remap ����� � ��������.
	OLED_SendByte (0xA1, 1);
	OLED_SendByte (0xC8, 1);
	// �������� ������� �������.
	OLED_SendByte (0x8D, 1);
	OLED_SendByte (0x14, 1);
	OLED_SendByte (0xAF, 1);
	// ��������� ���������� � ��������� ������ ����� � ��������.
	OLED_SendByte (0x21, 1);
	OLED_SendByte (0, 1);
	OLED_SendByte (127, 1);
	OLED_SendByte (0x22, 1);
	OLED_SendByte (0, 1);
	OLED_SendByte (7, 1);
	// �������� ������� (�� ������ ������).
	OLED_Clear();
}

/*--------------------------------------------------------------------------------
 * OLED_UpdateScreen (void) : void
 * ��������� ����� �����������, ����������� � ���������� modePanel,
 * highlightPanel, clockPanel.
 *--------------------------------------------------------------------------------*/
void OLED_UpdateScreen(void)
{
	// ��������������� ����������.
	uint16_t i;
	uint8_t j;
	uint8_t high;
	uint8_t low;
	// �������� ������ START.
	d_I2C_Start();											
	while(!d_StartEnd);
	// �������� �����
	d_I2C_Byte(SSD1306_ADDRESS);		
	while(!d_AdrSendEnd);
	d_I2C_SR2_Clear();										
	// ����� �������� ������.
	d_I2C_Byte(0x40);
	// ����������� ������ �������������� �����.
	for(i = 0; i < 128; i++){
		d_I2C_Byte(modePanel[i]);
		while(!d_ByteSendEnd);
	}
	// ����������� ��������� ������������� �����.
	for(i = 0; i < 128; i++){
		d_I2C_Byte(highlightPanel[i]);	
		while(!d_ByteSendEnd);
	}
	// ������ ������ (��������).
	for(i = 256; i < 384; i++)
	{
		d_I2C_Byte(0x00);
		while(!d_ByteSendEnd);	
	}
	// ����������� ������ ����� (������� �� �������� �� 4 �����).
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
	
	// ���������� ������ STOP.
	d_I2C_Stop();												
	while(I2C1->SR2 & I2C_SR2_BUSY);												
}

/*--------------------------------------------------------------------------------
 * OLED_WriteInTimePanel (number : uint8_t - �����, ������� ���������� ��������.
 *						  base : uint8_t   - ���� ��������� ����� �� �������, ��������� 
 *											 ��� ���������� ������ ����� �� ���� ������.
 *						  ) : void
 * ������ ����� � ������������ ����� �� ������ �������.
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
void OLED_WriteTime(uint8_t hourTens, uint8_t hourUnits, uint8_t minuteTens, uint8_t minuteUnits, uint8_t mode, uint8_t highlightedNumber){
	// ��������� ������ �������.
	OLED_WriteInTimePanel(hourTens, 0);
	OLED_WriteInTimePanel(hourUnits, 4);
	OLED_WriteInTimePanel(minuteTens, 9);
	OLED_WriteInTimePanel(minuteUnits, 13);
	
	uint16_t i;
	// ��������� ������ �������.
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
	// ��������� ������ ��������� ����.
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
	// ����� ������� ����������� ������� �� �����.
	OLED_UpdateScreen();
}