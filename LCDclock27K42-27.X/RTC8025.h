/* 
 * File:   RTC8025.h
 * Author: K.Ohno
 *
 * Created on November 21, 2020, 8:51 PM
 */

#ifndef RTC8025_H
#define	RTC8025_H

#ifdef	__cplusplus
extern "C" {
#endif

    
#include "mcc_generated_files/mcc.h"

// -------------- プロトタイプ宣言 --------------------------
uint8_t Hex2Bcd(uint8_t x);
uint8_t Bcd2Hex(uint8_t x);

void RTC_resetAlarm(void);
void RTC_setAlarmTime(uint8_t *alarmtime);

void IncDecTime(int16_t dmm, uint8_t *hh, uint8_t *mm);
void RTC_setTime(uint8_t *datetime);
void RTC_setDate(uint8_t *datetime);
void dateErrorCheck(uint8_t *dateTime);
uint8_t getWeekdays(uint8_t *year, uint8_t *month, uint8_t *day);
void RTC_read(uint8_t *datetime);
void resetCTFG(void);

void init_RTC(uint8_t *datetime);



#ifdef	__cplusplus
}
#endif

#endif	/* RTC8025_H */
