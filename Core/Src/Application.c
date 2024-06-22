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
#include "App/app_x-cube-ai.h"
#include "App/network.h"
#include "stdio.h"

/* User PV */

/* Draw Box Data Val */
uint16_t DrawPosX_Data = 0;
uint16_t DrawPosY_Data = 0;
uint16_t DrawPosX_Act = 0;
uint16_t DrawPosY_Act = 0;
uint8_t DrawPos_Data[INPUT_DATA_ZONE_HEIGHT][INPUT_DATA_ZONE_HEIGHT] = { 0, };
float DrawPos_Data_Full[INPUT_DATA_ZONE_HEIGHT * INPUT_DATA_ZONE_HEIGHT] = { 0.0f, };
/* AI Input and Output Val */
AI_ALIGNED(4)
static ai_i8 in_data[AI_NETWORK_IN_1_SIZE_BYTES];
AI_ALIGNED(4)
static ai_i8 out_data[AI_NETWORK_OUT_1_SIZE_BYTES];
/* Prediction Result */
float predictionval[10];
float predictionval_Sort[10][2];

/* Pre-Func */
static void Run_DrawZone(uint16_t Xpos, uint16_t Ypos);
static void Run_Predict();
static void Run_Clear();



/**
 * \fn void Show_Menu()
 * \brief Make Menu detail
 */
void Show_Menu()
{
	/* Make Draw Zone Box */
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

	/* Make Act-Data Zone Box */
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_DrawRect(INPUT_DATA_ZONE_START_X-2, INPUT_DATA_ZONE_START_Y-2, INPUT_DATA_ZONE_WIDTH+2, INPUT_DATA_ZONE_HEIGHT+2);

	/* Make Prediction Result Box */
	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	BSP_LCD_FillRect(PRED_ZONE_START_X, PRED_ZONE_START_Y, PRED_ZONE_WIDTH, PRED_ZONE_HEIGHT);

	/* Make AI Run Button */
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(PRED_BUTTON_START_X, PRED_BUTTON_START_Y, PRED_BUTTON_WIDTH, PRED_BUTTON_HEIGHT);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_DisplayStringAt(PRED_BUTTON_START_X+25, PRED_BUTTON_START_Y + 25, (uint8_t*) "Run", LEFT_MODE);

	/* Make Clear Button */
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(CLR_BUTTON_START_X, CLR_BUTTON_START_Y, CLR_BUTTON_WIDTH, CLR_BUTTON_HEIGHT);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_DisplayStringAt(CLR_BUTTON_START_X + 15, CLR_BUTTON_START_Y + 25, (uint8_t*) "Clear", LEFT_MODE);
}

/**
 * \fn bool Check_Touch_Zone(uint16_t, uint16_t)
 * \brief
 * \param Xpos : input Data Xpos
 * \param Ypos : input Data Ypos
 * \return
 */
bool Check_Touch_Zone(uint16_t Xpos, uint16_t Ypos)
{
	if (Xpos > (DRAW_ZONE_START_X) && Xpos < (DRAW_ZONE_START_X + DRAW_ZONE_WIDTH))
	{
		if (Ypos > (DRAW_ZONE_START_Y) && Ypos < (DRAW_ZONE_START_Y + DRAW_ZONE_HEIGHT))
		{
			/* Run Draw */
			Run_DrawZone(Xpos, Ypos);
			return true;
		}
	}

	if (Xpos > PRED_BUTTON_START_X && Xpos < (PRED_BUTTON_START_X + PRED_BUTTON_WIDTH))
	{
		if (Ypos > (PRED_BUTTON_START_Y) && Ypos < (PRED_BUTTON_START_Y + PRED_BUTTON_HEIGHT))
		{
			/* Run AI Prediction */
			Run_Predict();
			return true;
		}
	}

	if (Xpos > CLR_BUTTON_START_X && Xpos < (CLR_BUTTON_START_X + CLR_BUTTON_WIDTH))
	{
		if (Ypos > (CLR_BUTTON_START_Y) && Ypos < (CLR_BUTTON_START_Y + CLR_BUTTON_HEIGHT))
		{
			/* Run Draw Zone and Input Data Clear */
			Run_Clear();
			return true;
		}
	}

	return false;
}

/**
 * \fn void Run_DrawZone(uint16_t, uint16_t)
 * \brief
 * \param Xpos : input data Xpos
 * \param Ypos : input data Ypos
 */
static void Run_DrawZone(uint16_t Xpos, uint16_t Ypos)
{
	DrawPosX_Data = Xpos - DRAW_ZONE_START_X;
	DrawPosY_Data = Ypos - DRAW_ZONE_START_Y;
	DrawPosX_Data = DrawPosX_Data / DRAW_BOX;
	DrawPosY_Data = DrawPosY_Data / DRAW_BOX;

	if(DrawPosY_Data <= INPUT_DATA_ZONE_HEIGHT && DrawPosX_Data <= INPUT_DATA_ZONE_WIDTH)
	{
		/* Set input Data in 28 x 28 array */
		DrawPos_Data[DrawPosY_Data][DrawPosX_Data] = 1;
	}

	/* +++++++++++++++++++++++++++++++++++ */

	DrawPosX_Act = DRAW_ZONE_START_X + (DrawPosX_Data * 10);
	DrawPosY_Act = DRAW_ZONE_START_Y + (DrawPosY_Data * 10);

	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_FillRect(DrawPosX_Act, DrawPosY_Act, DRAW_BOX, DRAW_BOX);
}

/**
 * \fn void Preprocessing_InputData()
 * \brief
 */
static void Preprocessing_InputData()
{
	uint16_t p1 = 0;

	/* Make 2 axis data to 1 axis Data */
	for (uint16_t y = 0; y < INPUT_DATA_ZONE_HEIGHT; y++)
	{
		for (uint16_t x = 0; x < INPUT_DATA_ZONE_WIDTH; x++)
		{
			if (DrawPos_Data[y][x] == 1)
			{
				/* Draw Act-Data Zone */
				BSP_LCD_DrawPixel(INPUT_DATA_ZONE_START_X + x, INPUT_DATA_ZONE_START_Y + y, LCD_COLOR_YELLOW);
				DrawPos_Data_Full[x + (y * INPUT_DATA_ZONE_WIDTH)] = 1.0f;
			}
		}
	}

	/* Make byte data to float data for AI run */
	for (uint16_t i = 0; i < (INPUT_DATA_ZONE_HEIGHT * INPUT_DATA_ZONE_WIDTH); i++)
	{
		uint8_t *pt = (uint8_t*) &DrawPos_Data_Full[i];

		for (uint8_t j = 0; j < 4; j++)
		{
			in_data[p1++] = pt[j];
		}
	}
}

/**
 * \fn void Preprocessing_OutputData()
 * \brief
 */
static void Reprocessing_OutputData()
{
	uint16_t p1 = 0;
	float tempf;
	uint32_t temp32;

	float temp_sort = 0.0f;
	uint8_t temp_pos = 0;

	/* Make output data(10 x 4 byte) to float type (10 flaot) */
	for (uint8_t i = 0; i < 10; i++)
	{
		uint8_t *p = (uint8_t*)&tempf;
		temp32 = (uint8_t) out_data[p1 + 3];
		temp32 = (temp32 << 8) | (uint8_t) out_data[p1 + 2];
		temp32 = (temp32 << 8) | (uint8_t) out_data[p1 + 1];
		temp32 = (temp32 << 8) | (uint8_t) out_data[p1];
		p1 += 4;
		for (uint8_t j = 0; j < 4; j++)
		{
			p[j] = temp32 >> (8 * j);
		}
		predictionval[i] = tempf * 100;
	}

	/* Sort prediction result */
	for (uint8_t cc = 0; cc < 10; cc++)
	{
		for (uint8_t c = 0; c < 10; c++)
		{
			if (temp_sort <= predictionval[c])
			{
				temp_pos = c;
				temp_sort = predictionval[c];
			}
			else
			{
			}
		}
		predictionval_Sort[cc][0] = temp_sort;
		predictionval_Sort[cc][1] = temp_pos;
		predictionval[temp_pos] = 0.0f;
		temp_sort = 0.0f;
	}
}
/**
 * \fn void Run_Predict()
 * \brief
 */
static void Run_Predict()
{
	char strBuffer[30] = {0,};

	/* ************************************ */
	Preprocessing_InputData();

	User_Ai_Run(in_data, out_data);

	Reprocessing_OutputData();
	/* ************************************ */

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_GRAY);
	BSP_LCD_SetFont(&Font20);

	/* Draw Result top 3 prediction */
	sprintf(strBuffer, "%.0f (%.2f)", predictionval_Sort[0][1], predictionval_Sort[0][0]);
	BSP_LCD_DisplayStringAt(PRED_ZONE_START_X+15, PRED_ZONE_START_Y+20,
			(uint8_t*) strBuffer, LEFT_MODE);

	sprintf(strBuffer, "%.0f (%.2f)", predictionval_Sort[1][1], predictionval_Sort[1][0]);
	BSP_LCD_DisplayStringAt(PRED_ZONE_START_X+15, PRED_ZONE_START_Y+50,
			(uint8_t*) strBuffer, LEFT_MODE);

	sprintf(strBuffer, "%.0f (%.2f)", predictionval_Sort[2][1], predictionval_Sort[2][0]);
		BSP_LCD_DisplayStringAt(PRED_ZONE_START_X+15, PRED_ZONE_START_Y+80,
			(uint8_t*) strBuffer, LEFT_MODE);
}

/**
 * \fn void Run_Clear()
 * \brief
 */
static void Run_Clear()
{
	/* Reset input and output data */
	DrawPosX_Act = 0;
	DrawPosY_Act = 0;
	memset(DrawPos_Data, 0, sizeof(DrawPos_Data));
	memset(DrawPos_Data_Full, 0, sizeof(DrawPos_Data_Full));

	/* Re-draw Menu */
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

