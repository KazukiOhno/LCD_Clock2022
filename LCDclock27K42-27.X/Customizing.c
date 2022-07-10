/*
 * �J�X�^�}�C�Y�֌W�̏��u
 */

#include "Customizing.h"

#include <string.h>

#include "main.h"
#include "font.h"
#include "LCD320x240color.h"
#include "AlarmSound.h"
#include "Draw.h"
#include "RTC8025.h"
#include "CustomData.h"
#include "FontSelect.h"
#include "ColorSetting.h"
#include "FontMag.h"
#include "AlarmSound.h"


uint8_t TargetObj;  //�ǂ̃I�u�W�F�N�g��ύX���悤�Ƃ��Ă��邩�@�@DateObj�Ƃ�TimeObj�Ƃ�
uint8_t TargetItem; //�ǂ̍��ڂ�ΏۂƂ��Ă��邩�@�@Font�Ƃ�Size�Ƃ�

//�����A���t�̐ݒ�ύX

//�v���O�������₷���悤�ɔz��ɁB���т́A���L���\�[�X�Ɠ���Date,Time,Alarm,Temp,Humidity
//�ݒ莞�Ƀe���|�����ɕێ�����AOK�Ȃ�CustomRsc�ɏ�����
ObjectRsc TempObjData[OBJECTNUM];   //�J�X�^�}�C�Y�������ێ�
ObjectRsc tempRsc[OBJECTNUM];    //�b�胊�\�[�X

uint16_t ItemFColor;
//�|�b�v�A�b�v
int16_t PopX, PopY, PopXW, PopYW, Yoffset;
uint8_t PopUp;  //�|�b�v�A�b�v���Ȃ�A1

#define FRAMESIZE 4     //�|�b�v�A�b�v�̘g�T�C�Y


//�J�X�^�}�C�Y�p�̉�ʕ\���v���\�[�X
//���L�I�u�W�F�N�g�̏��Ƀ��\�[�X��ݒ肷��
enum CustomSettingRscObject {
    TitleSettingObj,
    SubTitleSettingObj,
    //�{�^��
    SettingBtnSaveObj,
    SettingBtnLoadObj,
    SettingBtnOKObj,
    SettingBtnCancelObj,
    //�I�u�W�F�N�g��
    SettingDateObj,
    SettingTimeObj,
    SettingAlarmObj,
    SettingTempObj,
    SettingHumidityObj,
    SettingCalendarObj,
    SettingGearObj,
};

//enum ItemNum {
//    ItemFont,
//    ItemFontSize,
//    ItemXpitch,
//    ItemColor,
//    ItemXY,
//    ItemFormat,
//};

const char CustomTextTitle1[] = "Customizing";
const char CustomTextTitle2[] = "Item  Font  Size Xp Color  X,  Y  fmt";
const char CustomSettingText[][6] = { "Date", "Time", "Alarm", "Temp", "Humid", "Calen", "Gear", };
const char BtnNameSave[] = "Save";
const char BtnNameLoad[] = "Load";
const char BtnNameOK[] = "OK";
const char BtnNameCancel[] = "Cancel";
const char CloseText[] = "Close";

//�e���X���W
#define XCheckbox   0
#define XItem       20
#define XFont       65
#define XFSize      130
#define XPitch      155
#define XColor      185
#define XPos        215
#define XFormat     300

//�e��̕�
#define XCheckboxW  (XItem - XCheckbox)
#define XItemW      (XFont - XItem)
#define XFontW      (XFSize - XFont)
#define XFSizeW     (XPitch - XFSize)
#define XPitchW     (XColor - XPitch)
#define XColorW     (XPos - XColor)
#define XPosW       (XFormat - XPos)
#define XFormatW    (LCD_WIDTH- XFormat)

//�e�s�̍���
#define ItemH   20


/*
 * �J�X�^�}�C�Y�p�̉�ʍ\��
 *    Item    Font Mag Xp Color Position format�@�t�H�[�}�b�g
 *    Date     o    o   o  o     o        o�@�@�@9��
 *    Time     o    o   o  o     o        o�@�@�@�f�W�^��3��A�i���O1��@24���ԕ\���̂�
 *    Alarm    o    o   o  o     o        o�@�@�@2��=24���ԕ\�L/12���ԁ@�@�I�t���̐F�w�����
 *    Temp     o    o   o  o     o        o�@�@�@2��@�؎�/�ێ�
 *    Humi     o    o   o  o     o        X�@�@�@
 *    Calendar o    o   o  o     o        o�@�@�@5��@�@�F�w��́A�����A�y�����ꂼ���
 *    Gear     X    X   X  o     o        X
 *
 * �@�@�t�H���g�́A����2��B�m�[�}���A�X���[��
 * �@�@�@�@�X���[���́A�J�����_�[�p��
 * �@�@�@SD�J�[�h��Ƀ��[�U�t�H���g���i�[���đΉ��B�f�[�^�̓��m�N��bmp�`���ŕۑ��B
 * �@�@�@�@7�Z�O�́A�����p�ɐ����A�R�������`�B���̃f�[�^��SD�J�[�h��ɔz�u
 * �@�ǉ��̑����́Aattribute���g��
 *               attribute     attribute2
 *    Time       �T�C�Y                        �A�i���O���v
 *    Alarm      �I�t���̐F�@�@�@�@�@�@�@�@�@�@�@�I�����̐F��color�ɐݒ�
 *    Calendar   ���j�̐F       �y�j�̐F�@�@�@�@�����̐F�́Acolor�ɐݒ�
 * 
 */
//�ݒ��ʗp���\�[�X
MainFormRsc SettingRsc2[] = {
    //ID,                 x,    y, xw,yw, fontcode,   fontMag,    str,                   color, format, attribute, attribute2, xpitchAdj
    { TitleSettingObj,    0,     0,  0,0, FontNormal, FontMagx22, (char*)CustomTextTitle1, WHITE, 0, },   //�^�C�g��
    { SubTitleSettingObj, XItem, 20, 0,0, FontNormal, FontMagx12, (char*)CustomTextTitle2, WHITE, 0, },   //�e���ږ�

    //�{�^���@�@                                                                                     attribute:�w�i�F
    { SettingBtnSaveObj,   10, 210, 50, 25, FontNormal, FontMagx12, (char*)BtnNameSave,   WHITE, 0, BLACK, },  // Save
    { SettingBtnLoadObj,   80, 210, 50, 25, FontNormal, FontMagx12, (char*)BtnNameLoad,   WHITE, 0, BLACK, },  // Load
    { SettingBtnOKObj,    160, 210, 60, 25, FontNormal, FontMagx22, (char*)BtnNameOK,     BLACK, 0, WHITE, },  // OK
    { SettingBtnCancelObj,240, 210, 60, 25, FontNormal, FontMagx12, (char*)BtnNameCancel, BLACK, 0, WHITE, },  // Cancel

    //���ږ�: ID���g�p/���g�p�̃t���O�Ɏg��
    { Enable,   XItem, 40, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[0], WHITE, 0, },     //Date
    { Enable,   XItem, 60, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[1], WHITE, 0, },     //Time
    { Enable,   XItem, 80, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[2], WHITE, 0, },     //Alarm
    { Enable,   XItem,100, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[3], WHITE, 0, },     //Temp
    { Enable,   XItem,120, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[4], WHITE, NoData, }, //Humidity
    { Enable,   XItem,140, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[5], WHITE, 0, },     //Calendar
    { Disable,  XItem,160, XItemW, ItemH, NoData8,    FontMagx12, (char*)CustomSettingText[6], WHITE, NoData, 0, 0, NoData8, },

};


char NameFontNormal[] = "Normal";
char NameFontSmall[] = "Small";
char NameFont7like[] = "7like";         //SD�J�[�h�ɗ���Ȃ��悤�A����7�Z�O��p��
char NameFont7seg[] = "7segsl";           //�J�X�^���t�H���g�Ɠ��l�̏��������邪�A�f�t�H���g�Őݒ�
char NameFont7segSmall[] = "7segslS";     //�J�X�^���t�H���g�Ɠ��l�̏��������邪�A�f�t�H���g�Őݒ�
char FontOther[] = "Custom...";

#define NumberOfFont    InternalFontNum+1   //���ڐ�=�|�b�v�A�b�v�̍s��

//�J�X�^�}�C�Y�Ń|�b�v�A�b�v�ŕ\������t�H���g��
//�����t�H���g�ƁA�O���J�X�^���t�H���g���Ƃ����I�����ɕύX
//�J�X�^���t�H���g����popup�ɂ͎������A�ʃE�C���h�E�őI������`
char *UserFontName[NumberOfFont] = {
    NameFontNormal,   //���ۂɂ͓����t�H���g�Ȃ̂ŁA�Œ薼
    NameFontSmall,
    NameFont7like,
    FontOther,
};


//�t�H���g�T�C�Y�̑I����
//�i�[����R�[�h�ƃ|�b�v�A�b�v�̍s�ԍ��̑Ώƕ\
//2�߂�I��������x12�̃T�C�Y���ƔF������̂͊ȒP�B
//�t�Ɍ��݂̐ݒ肪x13���������A���Ԗڂ��́A�������Ȃ���3�ԖڂƔF���ł��Ȃ�
uint8_t FontMag[] = {
    FontMagx11,
    FontMagx12,
    FontMagx13,
    FontMagx21,
    FontMagx22,
    FontMagx23,
    FontMagx32,
    FontMagx33,
    FontMagOther,
};
//�|�b�v�A�b�v�ɕ\������e�L�X�g
const char *FontMagText[] = {
  "11:x*1,y*1",
  "12:x*1,y*2",
  "13:x*1,y*3",
  "21:x*2,y*1",
  "22:x*2,y*2",
  "23:x*2,y*3",
  "32:x*3,y*2",
  "33:x*3,y*3",
  "Other"
};

//X�����s�b�`�����̑I����
//�ݒ��ʂ�Popup�\���p�e�L�X�g
const char *FontXpitchText[] = {
  "default -2",
  "default -1",
  "default",
  "default +1",
  "default +2",
  "default +3",
  "default +4",
  "default +5",
  "default +6",
  "default +7",
  "default +8",
};

//���t�̕\���`���@DateFormat�Ə�������
const char *DateFormatText[] = {
    "0 YYYY/MM/DD(w)",    //�j���t��
    "1 YYYY/MM/DD(we)",   //�p��j���t��
    "2 YYYY/MM/DD",
    "3 YY/MM/DD(w)",
    "4 YY/MM/DD(we)",
    "5 YY/MM/DD",
    "6 MM/DD(w)",
    "7 MM/DD(we)",
    "8 MM/DD",
};
     
//�����\���̌`���@������TimeFormat�Ɠ����@�@�@�@12/24�\���͑Ή��ł��Ă��Ȃ�
const char *TimeFormatText[] = {
    "0 HH:MM",
    "1 HH:MMss",
    "2 HH:MM:SS",
    "A ANALOG",
};

//�A�i���O���v�̔��a�T�C�Y�@�@xw�ɂ́A����2�{�̐��l���i�[�����
uint8_t AnalogClockSize[] = {
    90,
    80,
    70,
    60,
    50,
};
//�A�i���O���v�̑傫���@�\�L�́A���o�I�Ɂ@�@AnalogClockFormat�Ə�������
//�f�[�^�́Aattribute�Ɋi�[
const char *AnalogClockSizeText[] = {
    "Large",  //"r=90",
    "Mid L",  //"r=80",
    "mid S",  //"r=70",
    "Small",  //"r=60",
    "Tiny",   //"r=50",
};

//�A���[���\���̌`���@AlarmFormat�Ə�������
const char *AlarmFormatText[] = {
    "1:12h ALM",
    "2:24h ALM",
    "3:12h ` ",
    "4:24h ` ",
};

//���x�̕\���`���@TempFormat�Ə�������
const char *TempFormatText[] = {
    "C DegC",
    "F DegF",
};

//�J�����_�[�\���̌`���@CalendarFormat�Ə�������
const char *CalendarFormatText[] = {
    "1month",
    "2month",   //������
    "2v month", //�c����
    "3month",
    "3v month",
    "1month(E)",    //�j���̉p��\��
    "2month(E)",   //������
    "2v month(E)", //�c����
    "3month(E)",
    "3v month(E)",
};


/*
 * �J�X�^���t�H���g�̃t�H���_��(8����max)
 * �J�X�^�}�C�Y������A�ۑ��K�v
 */
//#define NumberOfUserFont   6 //�J�X�^���t�H���g�� +1�́A�b��t�H���g�p
char CustomFontDir[NumberOfUserFont+1][9] = {
    "", "", "", "", "", "", "",
};

/* 
 * SD�J�[�h�Ɋi�[����Ă���t�H���_�����t�H���g��(�ő�8����)
 * �����́AFontCode�ƍ��킹�Ă���B�擪2�́A�����t�H���g�Ȃ̂ŁA�t�H���_�s�v
 * �J�X�^�}�C�Y��ʂł̃t�H���g���̕\���ɂ��g���̂ŁA�����t�H���g������Ă���
 */
char *FontFolder[] = {
    NameFontNormal, //"Normal",   //���ۂɂ͓����t�H���g�Ȃ̂ŁA����͕s�v����
    NameFontSmall,  //"Small",
    NameFont7like,  //7�Z�O���ǂ�
    NameFont7seg,   //"7segsl",
    NameFont7segSmall,  //����������7�Z�O�t�H���g
    CustomFontDir[0],   // Date�ŃJ�X�^���t�H���g�K�p���̃t�H���g��=�t�H���_
    CustomFontDir[1],
    CustomFontDir[2],
    CustomFontDir[3],
    CustomFontDir[4],
    CustomFontDir[5],
    CustomFontDir[6],   // �ꎞ�I�Ɏg�p����t�H���g���p�B�J�X�^�}�C�Y���Ƃ��B
};

//�e�I�u�W�F�N�g�̃t�H�[�}�b�g�̐�
uint8_t FormatNum[OBJECTNUM] = {
    MAXDateFormat,
    MAXTimeFormat,
    MAXAlarmFormat,
    MAXTempFormat,
    0,
    MAXCalendarFormat,    
};

//Format�̃|�b�v�A�b�v�ɕ\��������I�u�W�F�N�g���̕�������ЂƂ܂Ƃ߂ɂ������X�g
const char *FormatTextList[] = {
  (const char*)DateFormatText,
  (const char*)TimeFormatText,
  (const char*)AlarmFormatText,
  (const char*)TempFormatText,
  (const char*)NULL,
  (const char*)CalendarFormatText,
};


//Prototype
void drawFontSize(MainFormRsc rsc, ObjectRsc data);
void drawObjectTable(uint8_t num, uint8_t objnum);


/*
 * �J�X�^���ݒ�̒l���R�s�[���āA�ݒ�ύX���Ă�Cancel�{�^�����������Ɍ��ɖ߂���悤�ɂ���
 * 
 */
void CopyCustomDataToTemp() {
    uint8_t jj;
    
    for (jj = 0; jj < OBJECTNUM; jj++) {
        TempObjData[jj] = CustomRsc[jj];
    }
}


/*
 * �ݒ肪OK�̎��ɁACustomRsc�ɒl�𔽉f������BEEPROM�ւ���������
 */
void SetCustomData() {
    uint8_t jj, kk;
    uint16_t addr;
    
    for (jj=0; jj < OBJECTNUM; jj++) {
        CustomRsc[jj] = TempObjData[jj];
        
        //EEPROM�ɋL�^���ĕێ�������  32�o�C�g�܂Ŋi�[�ł���悤�ɂ��Ă���
        addr = AddressCustom + (uint16_t)jj*32;
        DATAEE_WriteByte(addr++, CustomRsc[jj].disp);
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].x);
        addr += 2;
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].y);
        addr += 2;
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].xw);
        addr += 2;
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].yw);
        addr += 2;

        DATAEE_WriteByte(addr++, CustomRsc[jj].fontcode);
        DATAEE_WriteByte(addr++, CustomRsc[jj].fontmag);

        DATAEE_WriteWord(addr, CustomRsc[jj].color);
        addr += 2;
        DATAEE_WriteWord(addr, CustomRsc[jj].format);
        addr += 2;
        DATAEE_WriteWord(addr, CustomRsc[jj].attribute);
        addr += 2;
        DATAEE_WriteWord(addr, CustomRsc[jj].attribute2);
        addr += 2;

        DATAEE_WriteByte(addr++, CustomRsc[jj].xpitchAdj);
    }
    // �J�X�^���t�H���g����EEPROM�ɕۑ�
    for (jj = 0; jj < NumberOfUserFont; jj++) {
        for (kk = 0; kk < 9; kk++) {
            DATAEE_WriteByte(addr++, CustomFontDir[jj][kk]);
        } 
    }    
}


void setObjectSize(uint8_t obj) {
    tempRsc[obj].xw = Cursor_x - tempRsc[obj].x;
    if (tempRsc[obj].fontcode < InternalFontNum)
        tempRsc[obj].yw = CurrentFont.ysize * TextMagY;
    else
        tempRsc[obj].yw = CustomYsize;

}

void drawItem(uint8_t obj) {
    int16_t temp = 123;
    int16_t humidity = 345;
    uint8_t dateTime[7] = {0x56, 0x34, 0x12, 0x00, 0x31, 0x12, 0x21};
    uint8_t alarmTime[3] = {0x34, 0x12, 0x7f};    //mm, hh, wday
  
    switch (obj) {
        case GearObj:
            DrawGearIcon(tempRsc[obj].x, tempRsc[obj].y, tempRsc[obj].color);
            break;
        case TimeObj:
            resetPreDateTime(); //�A�i���O���v�ł͎g�p
            if (tempRsc[obj].disp) {
                DrawTime(dateTime, &tempRsc[obj]);
                //�`�悵�����̏��𗘗p���āA�g�T�C�Y��ۑ�
                if (tempRsc[obj].format == ANALOGCLOCK) {
                    tempRsc[obj].yw = tempRsc[obj].xw;
                } else {
                    tempRsc[obj].xw = Cursor_x - tempRsc[obj].x;
                    tempRsc[obj].yw = CustomYsize;
                }
            }
            break;
        case DateObj:
            if (tempRsc[obj].disp) {
                DrawDate(&tempRsc[obj]);    //���t��`��
                //�`�悵�����̏��𗘗p���āA�g�T�C�Y��ۑ�
                setObjectSize(obj);
            }
            break;
        case TempObj:
            if (tempRsc[obj].disp) {
                DrawTemp(temp, &tempRsc[obj]);
                setObjectSize(obj);
            }
            break;
        case HumidityObj:
            if (tempRsc[obj].disp) {
                DrawHumidity(humidity, &tempRsc[obj]);
                setObjectSize(obj);
            }
            break;
        case AlarmObj:
            if (tempRsc[obj].disp) {
                DrawAlarmTime(alarmTime, SlideSWon, &tempRsc[obj]);
                setObjectSize(obj);
            }
            break;
        case CalendarObj:
            if (tempRsc[obj].disp) {
                DrawCalendar(&tempRsc[obj]);
                tempRsc[obj].xw = CustomXsize;
                tempRsc[obj].yw = CustomYsize;
            }
            break;
    }
}


/*
 * ���݂̐ݒ�������g���āA��ʂ�`�悷��
 * 
 * �ʒu�ύX�ݒ�̎��A�C���[�W���킩��悤�ɁAtempRsc�ɐݒ�r���̃f�[�^����������
 * �ʒu��ύX����tempRsc�̃f�[�^�́A�L�����Z���Ŕj�������̂ŁA�b��̎b��
 */
void drawXYscreen(uint8_t item) {
    uint8_t jj;
    char str[100];

    lcd_fill(TextbgColor);
    
    //���i�`��@7�̕��i�S���`��
    for (jj = DateObj; jj < OBJECTNUM; jj++) {
        tempRsc[jj] = TempObjData[jj];  //�b��\���p�̃��\�[�X�Ɍ��݂̐ݒ���R�s�[
        drawItem(jj);
    }
    
    sprintf(str, "Touch screen to put %s", CustomSettingText[item]);
    SetFont(FontNormal);
    display_drawChars(10, 50, str, WHITE, TextbgColor, FontMagx12);
    
    //�ݒ�Ώۂ�g�ň͂�
    display_drawRect(tempRsc[item].x, tempRsc[item].y, tempRsc[item].xw, tempRsc[item].yw, RED);
    
}


void drawUpdateItem(uint8_t item) {
    char str[100];
    
    drawItem(item);
    //�ړ���̈ʒu�ɋ�`��`��
    display_drawRect(tempRsc[item].x, tempRsc[item].y, tempRsc[item].xw, tempRsc[item].yw, RED);
    
    //���W��\��
    sprintf(str, "(%3d, %3d)", tempRsc[item].x, tempRsc[item].y);
    display_fillRect(140, 118, 80, 10, WHITE);
    SetFont(FontNormal);
    display_drawChars(140, 120, str, BLUE, WHITE, FontMagx12);
    
    PushEvent(EventFormUpdate);

}


/*
 * �ʒu�ݒ�p�C�x���g�n���h��
 * 
 * �����̃^�b�`�L�����u���[�V�����Ɠ��l�ȏ��u
 * ��ʂ��N���A���āA���b�Z�[�W�\�����āA��ʂ̂ǂ������^�b�`������A������X,Y�Ƃ���
 * TargetObj: �ǂ̃I�u�W�F�N�g���Ώۂ�
 */
void XYsetEventHandler() {
    uint8_t evt;
    char str[100];
    uint8_t jj;
    uint8_t item = TargetObj;

    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //�C�x���g�Ȃ����́A���ʂɋ��肳���Ȃ��悤�A10ms��wait
        return;
    }
    
#ifdef DEBUG
    sprintf(str, "e=%03d", evt);
    display_drawChars(250, 0, str, WHITE, TextbgColor, FontMagx11);
#endif    
    switch (evt) {
        case EventXYFormInit:
            CurrentForm = SettingRsc2;  //�t�H�[����Customizing�Ɠ������̂𗘗p���邪�{�^��2����
            //�C���[�W�\��
            drawXYscreen(TargetObj);

        case EventFormUpdate:
            //OK��Cancel�{�^����`��
            for (jj = SettingBtnOKObj; jj <= SettingBtnCancelObj; jj++) {
                SetFont(FontNormal);
                DrawButton(CurrentForm[jj], CurrentForm[jj].color, LIGHTGREY, WHITE, 10);
            }
            break;
        case EventRotary:
            RotaryEventCount--;

            //���̘g��������
            display_fillRect(tempRsc[item].x, tempRsc[item].y, tempRsc[item].xw, tempRsc[item].yw, BLACK);

            //�X���C�hSW�ŁAX�������AY�����̂ǂ���ɓ����������߂�
            if (SlideSWStatus == SlideSWoff) {
                tempRsc[item].x += RotCount;
                if (tempRsc[item].x < 0) tempRsc[item].x = 0;
                if (tempRsc[item].x > LCD_WIDTH) tempRsc[item].x = LCD_WIDTH;
            } else {
                tempRsc[item].y += RotCount;
                if (tempRsc[item].y < 0) tempRsc[item].y = 0;
                if (tempRsc[item].y > LCD_HEIGHT) tempRsc[item].y = LCD_HEIGHT;
            }
            RotCount = 0;
            drawUpdateItem(item);

            break;
            
        case EventTouchDown:
        case EventTouchRepeat:
            // �^�b�`�̈ʒu
            if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnOKObj])) { //OK�̎�
                TempObjData[item].x = tempRsc[item].x;
                TempObjData[item].y = tempRsc[item].y;
                //������߂�
                SetEventHandler(CustomizingEventHandler);
                PushEvent(EventFormInit);
            } else if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnCancelObj])) { //Cancel�̎�
                SetEventHandler(CustomizingEventHandler);
                PushEvent(EventFormInit);
            } else {
                //���̘g������
                display_fillRect(tempRsc[item].x, tempRsc[item].y, tempRsc[item].xw, tempRsc[item].yw, BLACK);
                tempRsc[item].x = TouchX;
                tempRsc[item].y = TouchY;
                
                drawUpdateItem(item);
                RepeatKey = 1;
            }
            break;
    }
}


/*
 * �|�b�v�A�b�v�̃{�b�N�X�����^�b�`�������̏���
 * �ǂ̍��ڂ��^�b�`���������f
 */
void TouchProcedurePopup() {
    int16_t xx, yy;
    uint8_t select;
    char str[100];

#ifdef DEBUG2
    sprintf(str, "T=%3d,%3d", TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, TextbgColor, FontMagx11);
#endif

    //�{�b�N�X�̊O�Ȃ�A�L�����Z��
    xx = TouchX - PopX;
    yy = TouchY - (PopY + FRAMESIZE);
    if ((xx >= 0) && (xx < PopXW)) {
        if ((yy >= Yoffset) && (yy < PopYW-FRAMESIZE)) {
            //�{�^���̋�`�̈�ɓ����Ă����ꍇ
            select = (uint8_t)(yy - Yoffset) / (CurrentFont.ypitch * 2);
            if (TargetItem == ItemFont) {     //�t�H���g�I��
                if (select >= InternalFontNum) {
                    //Custom...���^�b�`������
                    PopupEventHandler(FontSelectEventHandler);
                    PushEvent(EventFormInit);
                    return;
                } else {
                    TempObjData[TargetObj].fontcode = select;
                }
            } else if (TargetItem == ItemFontSize) {    //�t�H���g�T�C�Y
                if (TargetObj == TimeObj && TempObjData[TargetObj].format == ANALOGCLOCK) {
                    //�A�i���O���v�̎��A�t�H���g�T�C�Y�͑傫���ɗ��p
                    //����ɉ����āAxw��ύX�B���a�ɗ��p
                    TempObjData[TargetObj].attribute = select;
                    TempObjData[TargetObj].xw = AnalogClockSize[select] *2;    //xw�͔��a�ł͂Ȃ��A�S�̂̃T�C�Y
                    TempObjData[TargetObj].yw = TempObjData[TargetObj].xw;
                } else {
                    if (FontMag[select] == FontMagOther) {
                        //�I�����ȊO��FontMag�̐ݒ�p�t�H�[���Ɉړ�
                        SetEventHandler(FontMagEventHandler);
                        PushEvent(EventFormInit);
                        //�����n���f�[�^:�t�H���g���A�T�C�Y���́ATempObjData, TargetObj����擾
                        return;
                    } else TempObjData[TargetObj].fontmag = FontMag[select];
                }
            } else if (TargetItem == ItemXpitch) {       //X�s�b�`����
                TempObjData[TargetObj].xpitchAdj = (int8_t) select-2;  //0�Ԗڂ�default-2
            } else if (TargetItem == ItemFormat) {         //�t�H�[�}�b�g
                TempObjData[TargetObj].format = select;
                
                if ((TargetObj == TimeObj)&&(TempObjData[TargetObj].format == ANALOGCLOCK)) {
                    //�A�i���O���v�ɕύX�������A�������Ƃ��āALarge=0�ő�T�C�Y���w��
                    TempObjData[TargetObj].xw = AnalogClockSize[Large] *2;
                    TempObjData[TargetObj].yw = TempObjData[TargetObj].xw;
                    TempObjData[TargetObj].attribute = Large;
                    drawFontSize(CurrentForm[TargetObj], TempObjData[TargetObj]);
                }
            }
        }
    }

    //�L�����Z������
    display_fillRect(PopX, PopY, PopXW, PopYW, TextbgColor);
    PushEvent(EventFormUpdate);
    PopUp = 0;  //�|�b�v�A�b�v�I��
}

/*
 * �w�肳�ꂽ���X�g���{�b�N�X�\�������鋤�ʃ��[�`��
 * num:       ���ڐ�
 * listText:  �\�����鍀�ڂ̕����񃊃X�g
 * popx: �|�b�v�A�b�v����X���W�́A�Ăяo�������w�肷��悤�ɂ����B
 */
uint16_t SelectedNo;    //���ݑI�����Ă��鍀�ڔԍ� (�擪=0(���ڐ�-1))
void drawPopupForm(uint8_t num, char **listText, char *title, int16_t popx) {
    int16_t x, y;
    uint8_t jj, len;
    char str[100];
    uint16_t fc, bc;

    if (title != NULL) Yoffset = 10;
    else Yoffset = 0;

    SetFont(FontNormal);
    //�^�b�`�����^�[�Q�b�g�̍��W�f�[�^
    //���X�g�̕�����̍Œ����`�F�b�N���ĉ�������
    PopXW = 0;
    for (jj = 0; jj < num; jj++) {
        len = (uint8_t)strlen(listText[jj]);
        if (len > PopXW) PopXW = len;
    }
    PopXW = PopXW * CurrentFont.xpitch + FRAMESIZE*2;     //����    
    
    PopX = popx;   //TargetItem���g���������ǂ����H
    PopY = CurrentForm[TargetObj + SettingDateObj].y;
    PopYW = num * CurrentFont.ypitch * 2 + FRAMESIZE*2; //Y�c�{��*2
    PopYW += Yoffset;    //�^�C�g�����A�c�𑝂₷   

    //��ʂ̊O�ɂ͂ݏo���Ȃ��悤�ɕ\���ʒu��␳
    if (PopX + PopXW > LCD_WIDTH-5) PopX =  - (PopXW - (LCD_WIDTH-5));
    if (PopY + PopYW > LCD_HEIGHT) PopY = - (PopYW - LCD_HEIGHT);
    
    display_fillRect(PopX, PopY, PopXW, PopYW, TextbgColor);
    x = PopX + FRAMESIZE;
    y = PopY + FRAMESIZE;
    
    //�ǂ̃A�C�e�����|�b�v�A�b�v�������킩��悤�ɏ������g�b�v�ɕ\��
    if (title != NULL) {
        display_drawChars(x+5, y, title, WHITE, TextbgColor, FontMagx11);
        y = y + Yoffset;
    }
    
    for (jj = 0; jj < num; jj++) {
        sprintf(str, "%s", listText[jj]);
        //���ݐݒ肳��Ă�����͔̂��]������
        if (jj == SelectedNo) {
            fc = TextbgColor;
            bc = WHITE;
        }
        else {
            fc = WHITE;
            bc = TextbgColor;
        }
        display_drawChars(x, y, str, fc, bc, FontMagx12);
        y = y + 20;
    }
    display_drawRect(PopX, PopY, PopXW, PopYW, WHITE);
    PopUp = 1;  //�|�b�v�A�b�v��

}


/*
 * �ݒ��ʂ��^�b�`�������̏��u
 * �@���W�ɂ���ď������e�����
 */
void TouchProcedureCustomizing() {
    int16_t yy;
    char str[100];
    uint8_t rscnum;
    
#ifdef DEBUG
    sprintf(str, "TXY=%03d, %03d", TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, TextbgColor, FontMagx11);
#endif
    
    if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnCancelObj])) {
        ClearBuffer();
        SetEventHandler(MainEventHandler);
        PushEvent(EventFormInit);
        
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnOKObj])) {
        //OK�̎��́A�ݒ肳�ꂽ����CustomRsc�Ɏ�荞�݁AEEPROM�ɏ�������
        SetCustomData();
        
        ClearBuffer();
        SetEventHandler(MainEventHandler);
        PushEvent(EventFormInit);
//        sprintf(str, "%d", TargetItem);
//        display_drawChars(0, 230, str, WHITE, TextbgColor, FontMagx11);
//        __delay_ms(500);

    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnSaveObj])) { //Save�̎�
        SetEventHandler(CustomDataEventHandler);
        PushEvent(EventFormInitSave);
    
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnLoadObj])) { //Load�̎�
        SetEventHandler(CustomDataEventHandler);
        PushEvent(EventFormInitLoad);
    
    } else {
        //�܂��ǂ̍s�Ƀ^�b�`�������`�F�b�N���āA���̌�A�ǂ̍���(��)�����`�F�b�N����
        // rscnum�����\�[�X�̃I�u�W�F�N�g���w���ATargetObj��7��̃I�u�W�F�N�g�̂ǂꂩ������
        TargetObj = DateObj;   // = 0
        while (TargetObj < OBJECTNUM) {
            rscnum = SettingDateObj + TargetObj;
            yy = TouchY - CurrentForm[rscnum].y;
            if ((yy >= 0) && (yy < CurrentForm[rscnum].yw)) {
                //�ǂ̍s=�I�u�W�F�N�g���Ώۂ����肳�ꂽ�̂ŁA�ǂ̗񂪃^�b�`���ꂽ������
                //Enable/Disable�̓`�F�b�N�{�b�N�X���𔻒�ɉ�����
                if ((TouchX >= 0) && (TouchX < XCheckboxW + XItemW)) {
                    //Item�̗���^�b�`������
                    //Gear��disable�ł��Ȃ��悤�X�L�b�v������@�@����CustomRsc��Gear��RscID=99
                    if (TempObjData[TargetObj].disp != 99) {
                        //�Ώۂ�obj��������Ă������Ԃ𔽓]������B
                        if (TempObjData[TargetObj].disp == Disable)
                            TempObjData[TargetObj].disp = Enable;
                        else
                            TempObjData[TargetObj].disp = Disable;
                        drawObjectTable(TargetObj, TargetObj + SettingDateObj);    //�O���[�A�E�g�Ή�
                    }
                    return;  //���[�v�I��
                }
                // disp==Disable�̏ꍇ�A���̐ݒ�͂ł��Ȃ��悤�ɂ����ŏI��
                if (TempObjData[TargetObj].disp == Disable) return;

                //Font�̗񂪃^�b�`���ꂽ��
                if ((TouchX >= XFont) && (TouchX < XFont + XFontW)) {
                    //�I�����Ȃ��ꍇ�́A���~
                    if (CurrentForm[rscnum].fontcode == NoData8) return;
                    //�Ώۂ�obj��������Ă�����
                    TargetItem = ItemFont;    //�t�H���g
                    SelectedNo = TempObjData[TargetObj].fontcode;   //���ݒl
                    if (SelectedNo >= InternalFontNum) SelectedNo = Font7seg;  //�J�X�^���t�H���g���������́A�S������
                    drawPopupForm(NumberOfFont, (char **)UserFontName, (char*)CustomSettingText[TargetObj], XFont); //�\������t�H���g����NumberOfFont�Ŏw��
                    return;  //���[�v�I��                    
                }
                
                //FontSize�̗񂪃^�b�`���ꂽ��
                if ((TouchX >= XFSize) && (TouchX < XFSize + XFSizeW)) {
                    //���̃I�u�W�F�N�g�ɑI�����Ȃ��ꍇ�́A���~
                    if (CurrentForm[rscnum].fontcode == NoData8) return;
                    TargetItem = ItemFontSize;  //�t�H���g�T�C�Y
                    //�Ώۂ�obj��������Ă�����|�b�v�A�b�v�@�@�A�i���O���v���[�h���́A�T�C�Y�ݒ�
                    if (TargetObj == TimeObj && TempObjData[TargetObj].format == ANALOGCLOCK) {
                        SelectedNo = TempObjData[TargetObj].attribute;   //���ݒl
                        drawPopupForm(sizeof AnalogClockSize, (char **)AnalogClockSizeText, (char*)CustomSettingText[TargetObj], XFSize);
                    }  else {
                        //fontsize�́A0��_�̃V���A���ԍ��łȂ��̂ŁA���݂̃t�H���g�T�C�Y�ƈ�v����V���A���ԍ����T�[�`����
                        SelectedNo = 0;
                        //���ݒl������
                        while (FontMag[SelectedNo] != TempObjData[TargetObj].fontmag) SelectedNo++;
                        drawPopupForm(sizeof FontMag, (char **)FontMagText, (char*)CustomSettingText[TargetObj], XFSize);
                    }
                    return;  //���[�v�I��                    
                }
                
                //X�s�b�`�̗񂪃^�b�`���ꂽ��
                if ((TouchX >= XPitch) && (TouchX < XPitch + XPitchW)) {
                    //�I�����Ȃ��ꍇ�́A���~
                    if (CurrentForm[rscnum].xpitchAdj == NoData8) return;
                    TargetItem = ItemXpitch;    //�t�H���g�s�b�`
                    //�Ώۂ�obj��������Ă�����
                    SelectedNo = (uint8_t)(TempObjData[TargetObj].xpitchAdj+2);   //���ݒl
                    drawPopupForm(MAXXpitchNum, (char **)FontXpitchText, (char*)CustomSettingText[TargetObj], XPitch);
                    return;  //���[�v�I��
                }

                //�F�ݒ�̗񂪃^�b�`���ꂽ��
                if ((TouchX >= XColor) && (TouchX < XColor + XColorW)) {
                    TargetItem = ItemColor;    //�F�̗��I��
                    //�Ώۂ�obj��������Ă�����
                    PopupEventHandler(ColorSettingEventHandler);
                    PushEvent(EventColorFormInit);
                    return;  //���[�v�I��
                }

                //�ʒu�ݒ�̗񂪃^�b�`���ꂽ��
                if ((TouchX >= XPos) && (TouchX < XPos + XPosW)) {
                    TargetItem = ItemXY;    //�ʒu�̗��I��
                    //�Ώۂ�obj��������Ă�����
                    SetEventHandler(XYsetEventHandler);
                    PushEvent(EventXYFormInit); //��ʕ`��
                    return;  //���[�v�I��
                }

                //�t�H�[�}�b�g�̗񂪃^�b�`���ꂽ��
                if ((TouchX >= XFormat) && (TouchX < XFormat + XFormatW)) {
                    //�I�����Ȃ��ꍇ�́A���~
                    if (CurrentForm[rscnum].format == NoData) return;
                    TargetItem = ItemFormat;    //�t�H�[�}�b�g
                    //�Ώۂ�obj��������Ă�����
                    SelectedNo = TempObjData[TargetObj].format;   //���ݒl
                    //���ڐ���Y�T�C�Y�A�ő區�ڂ̉�����X�T�C�Y���肵�A���ڂ�\��
                    drawPopupForm(FormatNum[TargetObj], (char **)FormatTextList[TargetObj], NULL, XFormat);

                    return;  //���[�v�I��
                }
                return;
            }
            TargetObj++;
        }
        
    }
}


/*
 * �X�̍��ڂ̕\��
 * �`�F�b�N�{�b�N�X�́AX=XCheckbox�ɌŒ�
 */
void drawCheckBox(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    //���ږ��̃��\�[�X�����L���ARscID�����̍��ڂ�\�������邩�ǂ����̐ݒ�ɗ��p
    sprintf(str, "%c", Fbox);   // Fbox=0x7f�́��̃R�[�h
    display_drawChars(XCheckbox, rsc.y, str, rsc.color, TextbgColor, FontMagx22);
    if (data.disp != Disable) {
        strcpy(str, "v");   //�`�F�b�N�́Av�ő�p
        display_drawChars(XCheckbox+1, rsc.y-1, str, rsc.color, rsc.color, FontMagx22);
    }
}

void drawFontName(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    if (rsc.fontcode == NoData8) strcpy(str, "-");
    else sprintf(str, "%-10s", FontFolder[data.fontcode]);   //�t�H���g��=�t�H���_��
    display_drawChars(XFont, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

void drawFontSize(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    if (rsc.fontcode == NoData8) strcpy(str, "-");
    else sprintf(str, "%2X", data.fontmag);
    display_drawChars(XFSize, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

void drawXpitch(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    if (rsc.xpitchAdj == NoData8) strcpy(str, "-");
    else sprintf(str, "%+2d", data.xpitchAdj);
    display_drawChars(XPitch, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

void drawColor(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    //�����t�H���g�g�킸�A�m�[�}���Ɂ��������
    sprintf(str, "%c", Ffbox);   //FontNormal�́��̃R�[�h=Ffbox
    display_drawChars(XColor, rsc.y, str, data.color, TextbgColor, rsc.fontmag); //�t�H���g�T�C�Y22�ɂ��Ȃ��Ƃ����Ȃ�����
    display_drawChars(XColor, rsc.y, str, data.color, TextbgColor, FontMagx22);

}

void drawXY(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    sprintf(str, "(%3d,%3d)", data.x, data.y);
    display_drawChars(XPos, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

/*
 * �I������Ă���t�H�[�}�b�g�̕\��
 */
void drawFormat(MainFormRsc rsc, ObjectRsc data, uint8_t num) {
    char str[100];
    
    if (rsc.format == NoData) strcpy(str, "-");
    else {
        switch (num) {
            case 0: //Date
                sprintf(str, "%d", data.format);
                break;
            case 1: //Time
                sprintf(str, "%c", *TimeFormatText[data.format]);
                break;
            case 2: //Alarm
                sprintf(str, "%c", *AlarmFormatText[data.format]);
                break;
            case 3: //Temp
                sprintf(str, "%c", *TempFormatText[data.format]);
                break;
            case 5: //Calendar
                sprintf(str, "%s", CalendarFormatText[data.format]);
                str[2] = '\0';
                break;
        }
    }
    display_drawChars(XFormat, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

/*
 * �J�X�^�}�C�Y�̕\���\������w�肳�ꂽ1�s��`��
 * num: RscObject�̔ԍ������炤�@0?OBJECTNUM
 * objnum: MainFormRsc�̂ǂ̍��ڂ�����
 */
void drawObjectTable(uint8_t num, uint8_t objnum) {
    char str[50];
    
    if (num < GearObj) drawCheckBox(CurrentForm[objnum], TempObjData[num]);
    display_drawChars(CurrentForm[objnum].x, CurrentForm[objnum].y, CurrentForm[objnum].str, CurrentForm[objnum].color, TextbgColor, CurrentForm[objnum].fontmag);
    
    //Disable�̎��A���ږ��ȊO���O���[�A�E�g������
    if (TempObjData[num].disp == Disable) ItemFColor = GREY;
    else ItemFColor = CurrentForm[objnum].color;
    
    drawFontName(CurrentForm[objnum], TempObjData[num]);

    //�A�i���O���v�̎��́A�t�H���g�T�C�Y�̏��́A���v�̃T�C�Y��\��
    if (num == TimeObj && TempObjData[num].format == ANALOGCLOCK) {
        sprintf(str, "%s", AnalogClockSizeText[TempObjData[num].attribute]);
        str[2] = '\0';  //�擪2����������ʂɕ\��
        display_drawChars(XFSize, CurrentForm[objnum].y, str, ItemFColor, TextbgColor, CurrentForm[objnum].fontmag);
    } else {
        drawFontSize(CurrentForm[objnum], TempObjData[num]);
    }
    drawXpitch(CurrentForm[objnum], TempObjData[num]);
    drawColor(CurrentForm[objnum], TempObjData[num]);
    drawXY(CurrentForm[objnum], TempObjData[num]);
    drawFormat(CurrentForm[objnum], TempObjData[num], num);
    
}

/*
 * cls=1�̎��́A��ʏ������Ă���`��
 * �ݒ��ʂ�`��
 */
void DrawCustomizingForm(uint8_t cls) {
    uint8_t jj;

    if (cls) lcd_fill(BLACK);
    
    SetFont(FontNormal);  //�����t�H���g�ݒ�

    //�^�C�g���A���ږ��\��
    for (jj = TitleSettingObj; jj<= SubTitleSettingObj; jj++) {
        display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    }
    
    //�e�I�u�W�F�N�g�̏���\��
    for (jj = DateObj; jj < OBJECTNUM; jj++) {
        drawObjectTable(jj, jj + SettingDateObj);
    }
    
    //�{�^���`��  Save, Load, OK, Cancel
    for (jj = SettingBtnSaveObj; jj <= SettingBtnCancelObj; jj++) {
        DrawButton(CurrentForm[jj], CurrentForm[jj].color, CurrentForm[jj].attribute, CurrentForm[jj].color, 10);
    }
}


/*
 * �J�X�^�}�C�Y�p�C�x���g�n���h��
 * �J�X�^�}�C�Y���ڂ�S����ʂɕ\�������A�ύX���������̂��^�b�`���ďC���ł���
 * 
 */
void CustomizingEventHandler() {
    uint8_t evt;
//    char str[100];
    
    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //�C�x���g�Ȃ����́A���ʂɋ��肳���Ȃ��悤�A10ms��wait
        return;
    }
    
#ifdef DEBUG
    sprintf(str, "e=%03d", evt);
    display_drawChars(250, 0, str, WHITE, TextbgColor, FontMagx11);
#endif    
    switch (evt) {
        case EventFormUpdate:
            DrawCustomizingForm(0);     //��ʏ����Ȃ�
            break;
        case EventFormInit:
            CurrentForm = SettingRsc2;
            DrawCustomizingForm(1);     //��ʏ�������
            PopUp = 0;
            break;
        case EventRotary:
            break;
        case EventTouchDown:
            //�|�b�v�A�b�v���Ă��邩�ǂ����ŏ����ύX
            if (PopUp) TouchProcedurePopup();
            else TouchProcedureCustomizing();
            break;
    }
}
