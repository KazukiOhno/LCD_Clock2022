/* 
 * File:   FontSelect.h
 * Author: ohno
 *
 * Created on April 14, 2022, 9:49 PM
 */

#ifndef FONTSELECT_H
#define	FONTSELECT_H

#ifdef	__cplusplus
extern "C" {
#endif

    
#include "mcc_generated_files/mcc.h"

void FontSelectEventHandler();
void PreviewFont(int16_t x, int16_t y, uint8_t fontmag, int8_t xpitch, uint16_t color);


#ifdef	__cplusplus
}
#endif

#endif	/* FONTSELECT_H */
