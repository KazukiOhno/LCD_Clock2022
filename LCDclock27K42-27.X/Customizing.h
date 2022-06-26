/* 
 * File:   customize.h
 * Author: ohno
 * 
 * Created on 2021/12/
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CUSTOMIZE_H
#define	CUSTOMIZE_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */


#include "mcc_generated_files/mcc.h"
#include "main.h"

extern const char BtnNameOK[];      // = "OK";
extern const char BtnNameCancel[];  // = "Cancel";
extern const char CloseText[];      // = "Close";
extern const char CustomSettingText[][6];   // = { "Date", "Time", "Alarm", "Temp", "Humid", "Calen", "Gear", };

extern ObjectRsc TempObjData[OBJECTNUM];   //�J�X�^�}�C�Y�������ێ�
extern uint16_t SelectedNo; //���ݑI�����Ă��鍀�ڔԍ� (�擪=0(���ڐ�-1))
extern uint8_t TargetObj;   //�ǂ̃I�u�W�F�N�g��ύX���悤�Ƃ��Ă��邩�@�@DateObj�Ƃ�TimeObj�Ƃ�
extern uint8_t TargetItem; //�ǂ̍��ڂ�ΏۂƂ��Ă��邩�@�@Font�Ƃ�Size�Ƃ�

/*
 * �J�X�^���t�H���g�̃t�H���_��(8����max)
 * �J�X�^�}�C�Y������A�ۑ��K�v
 */
#define NumberOfUserFont   6 //�J�X�^���t�H���g��
extern char CustomFontDir[NumberOfUserFont+1][9];   // +1�́A�b��t�H���g�p

extern char NameFontNormal[];   // = "Normal";
extern char NameFontSmall[];    // = "Small";
extern char NameFont7like[];    // = "like7";   //SD�J�[�h�ɗ���Ȃ��悤�A����7�Z�O��p��
extern char NameFont7seg[];     // = "7segsl";           //�J�X�^���t�H���g�Ɠ��l�̏��������邪�A�f�t�H���g�Őݒ�
extern char NameFont7segSmall[];   // = "7segslS";     //�J�X�^���t�H���g�Ɠ��l�̏��������邪�A�f�t�H���g�Őݒ�

enum ItemNum {
    ItemFont,
    ItemFontSize,
    ItemXpitch,
    ItemColor,
    ItemXY,
    ItemFormat,
};

/* 
 * SD�J�[�h�Ɋi�[����Ă���t�H���_�����t�H���g��(�ő�8����)
 * �����́AFontCode�ƍ��킹�Ă���B�擪2�́A�����t�H���g�Ȃ̂ŁA�t�H���_�s�v
 * �J�X�^�}�C�Y��ʂł̃t�H���g���̕\���ɂ��g���̂ŁA�����t�H���g������Ă���
 */
extern char *FontFolder[];

//X�����s�b�`�����̑I����  -2 - 8��11��
#define MAXXpitchNum    11
extern const char *FontXpitchText[];
extern uint8_t FormatNum[OBJECTNUM];
extern const char *FormatTextList[];


void drawPopupForm(uint8_t num, char **listText, char *title, int16_t popx);
void TouchProcedurePopup(void);

void CopyCustomDataToTemp(void);
void SetCustomData(void);
void CustomizingEventHandler(void);


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* CUSTOMIZE_H */
