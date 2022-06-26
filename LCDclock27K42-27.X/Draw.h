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
    

extern int16_t CustomXsize; //�J�X�^�}�C�Y�̎��A�̈���w�肷�邽�߂Ɏg���@�J�����_�[�݂̂Ɏg�p
extern int16_t CustomYsize; //�f�W�^���̎����\���ɂ��g�p
extern char Smaller[2];  //�������t�H���g�p�g�p��: "s"�A����ȊO�̎��A""


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
