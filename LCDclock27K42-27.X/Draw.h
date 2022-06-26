/* 
 * File:   Draw.h
 * Author: K.Ohno
 *
 * Created on December 20, 2020, 3:50 PM
 */

#ifndef DRAW_H
#define	DRAW_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "mcc_generated_files/mcc.h"
#include "main.h"
    

extern int16_t CustomXsize; //カスタマイズの時、領域を指定するために使う　カレンダーのみに使用
extern int16_t CustomYsize; //デジタルの時刻表示にも使用
extern char Smaller[2];  //小さいフォント用使用時: "s"、それ以外の時、""


void resetPreDateTime(void);

uint8_t SetFont(uint8_t font);
void SetKanjiFont(uint8_t fontcode);

uint8_t displayCustom_puts(char *s);

void DrawTime(uint8_t *datetime, ObjectRsc *rsc);
void DrawDate(ObjectRsc *rsc);
void DrawTemp(int16_t temp, ObjectRsc *rsc);
void DrawHumidity(int16_t humidity, ObjectRsc *rsc);
void DrawAlarmTime(uint8_t *alarmtime, uint8_t sw, ObjectRsc *rsc);
void DrawCalendar(ObjectRsc *rsc);

void DrawGearIcon(int16_t x, int16_t y, uint16_t color);

void DrawStrCenter(MainFormRsc rsc, uint16_t color, uint16_t bcolor);
void DrawButton(MainFormRsc rsc, uint16_t fcolor, uint16_t bcolor, uint16_t rcolor, int16_t r);
    
#ifdef	__cplusplus
}
#endif

#endif	/* DRAW_H */
