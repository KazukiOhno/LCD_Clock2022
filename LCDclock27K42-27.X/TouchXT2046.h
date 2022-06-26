/* 
 * File:   TouchXT2046.h
 * Author: K.Ohno
 *
 * Created on December 6, 2020, 11:15 AM
 */

#ifndef TOUCHXT2046_H
#define	TOUCHXT2046_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"

int8_t GetTouchLocation(uint16_t *tx, uint16_t *ty);


#ifdef	__cplusplus
}
#endif

#endif	/* TOUCHXT2046_H */
