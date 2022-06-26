/* 
 * File:   DHT20.h
 * Author: ohno
 *
 * Created on January 22, 2022, 4:19 PM
 */

#ifndef DHT20_H
#define	DHT20_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "mcc_generated_files/mcc.h"

// センサから温湿度読み出し、その値を返す
void Get_TempHumidity(int16_t *temp, int16_t *humidity);


#ifdef	__cplusplus
}
#endif

#endif	/* DHT20_H */
