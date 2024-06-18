/*
 * Application.c
 *
 *  Created on: Jun 12, 2024
 *      Author: LeeJH
 */
/* include */
#include "Application.h"

#include "stm32f769i_discovery_lcd.h"
#include "stm32f769i_discovery_ts.h"
#include "app_x-cube-ai.h"
#include "network.h"

/* User PV */
uint16_t DrawPosX_Data = 0;
uint16_t DrawPosY_Data = 0;
uint16_t DrawPosX_Act = 0;
uint16_t DrawPosY_Act = 0;
uint8_t DrawPos_Data[INPUT_DATA_ZONE_HEIGHT][INPUT_DATA_ZONE_HEIGHT] = { 0, };
float DrawPos_Data_Full[INPUT_DATA_ZONE_HEIGHT * INPUT_DATA_ZONE_HEIGHT] = { 0.0f, };
AI_ALIGNED(4)
static ai_i8 in_data[AI_NETWORK_IN_1_SIZE_BYTES];
AI_ALIGNED(4)
static ai_i8 out_data[AI_NETWORK_OUT_1_SIZE_BYTES];

float predictionval[10];
char cNumbers[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

/* Pre-Func */
static void Run_DrawZone(uint16_t Xpos, uint16_t Ypos);
static void Run_Predict();
static void Run_Clear();

/**
 *
 */
void Show_Menu()
{
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_DrawRect(DRAW_ZONE_START_X - 1, DRAW_ZONE_START_Y - 1, DRAW_ZONE_WIDTH, DRAW_ZONE_HEIGHT);

	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	for (uint8_t c = 1; c < DRAW_ZONE_WIDTH / DRAW_BOX; c++)
	{
		BSP_LCD_DrawVLine(DRAW_ZONE_START_X + (c * DRAW_BOX), DRAW_ZONE_START_Y, DRAW_ZONE_HEIGHT);
	}
	for (uint8_t c = 1; c < DRAW_ZONE_HEIGHT / DRAW_BOX; c++)
	{
		BSP_LCD_DrawHLine(DRAW_ZONE_START_X, DRAW_ZONE_START_Y + (c * DRAW_BOX), DRAW_ZONE_WIDTH);
	}

	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_DrawRect(INPUT_DATA_ZONE_START_X-2, INPUT_DATA_ZONE_START_Y-2, INPUT_DATA_ZONE_WIDTH+2, INPUT_DATA_ZONE_HEIGHT+2);

	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	BSP_LCD_FillRect(PRED_ZONE_START_X, PRED_ZONE_START_Y, PRED_ZONE_WIDTH, PRED_ZONE_HEIGHT);

	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(PRED_BUTTON_START_X, PRED_BUTTON_START_Y, PRED_BUTTON_WIDTH, PRED_BUTTON_HEIGHT);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_DisplayStringAt(PRED_BUTTON_START_X+25, PRED_BUTTON_START_Y + 25, (uint8_t*) "Run", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(CLR_BUTTON_START_X, CLR_BUTTON_START_Y, CLR_BUTTON_WIDTH, CLR_BUTTON_HEIGHT);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_DisplayStringAt(CLR_BUTTON_START_X + 15, CLR_BUTTON_START_Y + 25, (uint8_t*) "Clear", LEFT_MODE);
}

/**
 *
 */
bool Check_Touch_Zone(uint16_t Xpos, uint16_t Ypos)
{
	if (Xpos > (DRAW_ZONE_START_X) && Xpos < (DRAW_ZONE_START_X + DRAW_ZONE_WIDTH))
	{
		if (Ypos > (DRAW_ZONE_START_Y) && Ypos < (DRAW_ZONE_START_Y + DRAW_ZONE_HEIGHT))
		{
			Run_DrawZone(Xpos, Ypos);
			return true;
		}
	}

	if (Xpos > PRED_BUTTON_START_X && Xpos < (PRED_BUTTON_START_X + PRED_BUTTON_WIDTH))
	{
		if (Ypos > (PRED_BUTTON_START_Y) && Ypos < (PRED_BUTTON_START_Y + PRED_BUTTON_HEIGHT))
		{
			Run_Predict();
			return true;
		}
	}

	if (Xpos > CLR_BUTTON_START_X && Xpos < (CLR_BUTTON_START_X + CLR_BUTTON_WIDTH))
	{
		if (Ypos > (CLR_BUTTON_START_Y) && Ypos < (CLR_BUTTON_START_Y + CLR_BUTTON_HEIGHT))
		{
			Run_Clear();
			return true;
		}
	}

	return false;
}

/**
 *
 */
static void Run_DrawZone(uint16_t Xpos, uint16_t Ypos)
{
	DrawPosX_Data = Xpos - DRAW_ZONE_START_X;
	DrawPosY_Data = Ypos - DRAW_ZONE_START_Y;
	DrawPosX_Data = DrawPosX_Data / DRAW_BOX;
	DrawPosY_Data = DrawPosY_Data / DRAW_BOX;

	if(DrawPosY_Data <= INPUT_DATA_ZONE_HEIGHT && DrawPosX_Data <= INPUT_DATA_ZONE_WIDTH)
	{
		DrawPos_Data[DrawPosY_Data][DrawPosX_Data] = 1;
	}

	/* +++++++++++++++++++++++++++++++++++ */

	DrawPosX_Act = DRAW_ZONE_START_X + (DrawPosX_Data * 10);
	DrawPosY_Act = DRAW_ZONE_START_Y + (DrawPosY_Data * 10);

	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_FillRect(DrawPosX_Act, DrawPosY_Act, DRAW_BOX, DRAW_BOX);
}

/**
 *
 */
static void Run_Predict()
{
	uint16_t num1 = 0, num2 = 0;
	float dummyfloat;
	uint32_t u32dummy;

	for(uint16_t y = 0; y < INPUT_DATA_ZONE_HEIGHT; y++)
	{
		for(uint16_t x = 0; x < INPUT_DATA_ZONE_WIDTH; x++)
		{
			if(DrawPos_Data[y][x] == 1)
			{
				BSP_LCD_DrawPixel(INPUT_DATA_ZONE_START_X + x, INPUT_DATA_ZONE_START_Y + y, LCD_COLOR_YELLOW);
				DrawPos_Data_Full[x + (y * INPUT_DATA_ZONE_WIDTH)] = 1.0f;
			}
		}
	}

	for(uint16_t i = 0; i < (INPUT_DATA_ZONE_HEIGHT*INPUT_DATA_ZONE_WIDTH); i++)
	{
		uint8_t *bfr = (uint8_t*)&DrawPos_Data_Full[i];

		for (uint8_t j = 0; j < 4; j++)
		{
			in_data[num1++] = bfr[j];
		}
	}

	aiRun(in_data, out_data);

	for (uint8_t i = 0; i < 10; i++)
	{
		uint8_t *p = (uint8_t*) (void*) &dummyfloat;
		u32dummy = (uint8_t) out_data[num2 + 3];
		u32dummy = (u32dummy << 8) | (uint8_t) out_data[num2 + 2];
		u32dummy = (u32dummy << 8) | (uint8_t) out_data[num2 + 1];
		u32dummy = (u32dummy << 8) | (uint8_t) out_data[num2];
		num2 += 4;
		for (uint8_t j = 0; j < 4; j++)
		{
			p[j] = u32dummy >> (8 * j);
		}
		predictionval[i] = dummyfloat * 100;

	}


	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_GRAY);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_DisplayStringAt(PRED_ZONE_START_X+25, PRED_ZONE_START_Y+20, (uint8_t*) "Predict", LEFT_MODE);
	BSP_LCD_DisplayStringAt(PRED_ZONE_START_X+15, PRED_ZONE_START_Y+50, (uint8_t*) "Result is", LEFT_MODE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(PRED_ZONE_START_X+70, PRED_ZONE_START_Y+90, (uint8_t*) "9", LEFT_MODE);
}

/**
 *
 */
static void Run_Clear()
{
	DrawPosX_Act = 0;
	DrawPosY_Act = 0;
	memset(DrawPos_Data, 0, sizeof(DrawPos_Data));
	memset(DrawPos_Data_Full, 0, sizeof(DrawPos_Data_Full));

	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_DrawRect(DRAW_ZONE_START_X - 1, DRAW_ZONE_START_Y - 1, DRAW_ZONE_WIDTH, DRAW_ZONE_HEIGHT);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(DRAW_ZONE_START_X, DRAW_ZONE_START_Y, DRAW_ZONE_WIDTH - 1, DRAW_ZONE_HEIGHT - 1);

	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	for (uint8_t c = 1; c < DRAW_ZONE_WIDTH / DRAW_BOX; c++)
	{
		BSP_LCD_DrawVLine(DRAW_ZONE_START_X + (c * DRAW_BOX), DRAW_ZONE_START_Y, DRAW_ZONE_HEIGHT);
	}
	for (uint8_t c = 1; c < DRAW_ZONE_HEIGHT / DRAW_BOX; c++)
	{
		BSP_LCD_DrawHLine(DRAW_ZONE_START_X, DRAW_ZONE_START_Y + (c * DRAW_BOX), DRAW_ZONE_WIDTH);
	}

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(INPUT_DATA_ZONE_START_X, INPUT_DATA_ZONE_START_Y, INPUT_DATA_ZONE_WIDTH, INPUT_DATA_ZONE_HEIGHT);

	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	BSP_LCD_FillRect(PRED_ZONE_START_X, PRED_ZONE_START_Y, PRED_ZONE_WIDTH, PRED_ZONE_HEIGHT);
}


/* User Code */

