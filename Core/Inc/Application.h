/*
 * Application.h
 *
 *  Created on: Jun 12, 2024
 *      Author: LeeJH
 */
#include "stdint.h"
#include "stdbool.h"

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#define DRAW_ZONE_WIDTH		280
#define DRAW_ZONE_HEIGHT	280
#define DRAW_ZONE_START_X	120
#define DRAW_ZONE_START_Y	100
#define DRAW_BOX			10

#define INPUT_DATA_ZONE_WIDTH		28
#define INPUT_DATA_ZONE_HEIGHT		28
#define INPUT_DATA_ZONE_START_X		450
#define INPUT_DATA_ZONE_START_Y		100

#define PRED_ZONE_WIDTH		150
#define PRED_ZONE_HEIGHT	150
#define PRED_ZONE_START_X	525
#define PRED_ZONE_START_Y	100

#define PRED_BUTTON_WIDTH		100
#define PRED_BUTTON_HEIGHT		70
#define PRED_BUTTON_START_X		480
#define PRED_BUTTON_START_Y		300

#define CLR_BUTTON_WIDTH		100
#define CLR_BUTTON_HEIGHT		70
#define CLR_BUTTON_START_X		620
#define CLR_BUTTON_START_Y		300

void Show_Menu();
bool Check_Touch_Zone(uint16_t Xpos, uint16_t Ypos);


#endif /* INC_APPLICATION_H_ */
