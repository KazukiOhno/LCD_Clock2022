/*
 * ���Ԃ��������Ƃ��̊e��
 */

#include "Setting.h"

#include <string.h>
#include "main.h"
#include "font.h"
#include "LCD320x240color.h"
#include "Draw.h"
#include "RTC8025.h"
#include "Customizing.h"
#include "TimeAdjust.h"

//�ȉ��ɂ��A�������2�o�C�g�����������Ă�warning�o�Ȃ��Ȃ�
#pragma jis

//���L�I�u�W�F�N�g�̏��Ƀ��\�[�X��ݒ肷��
enum SettingRscObject {
    SettingTitleObj,
    //�I�u�W�F�N�g��
    CustomizingObj,
    TimeDateAdjustObj,
    AboutObj,
    FeatureObj,
    RedigitizingObj,
    //�{�^��    
    SettingBtnCloseObj,    
};
    
const char SettingTextTitle1[] = "Setting Menu";
//�ȉ��̂悤�ɒ�`���Ȃ��ƃ��\�[�X���̕�����Ƃ��Ďg���Ȃ����A�����񒷂��Œ�Ȃ̂ŁA���ʂɃ������������
//1��1��������`���Ă��ǂ����A�ʂɒ萔����t���Ȃ���΂Ȃ�Ȃ��̂��ʓ|
//const���ƁAData�̈������Ȃ��B�l�̓v���O�����̈�ɏ������܂�邱�Ƃ��킩����

const char TextCustomizing[]  = "- Customizing";
const char TextTimeAdjust[]   = "- Time/Date adjusting";
const char TextFeature[]      = "- Feature (same as Opening)";
const char TextRedigitizing[] = "- Re-digitizing or push screen >2s";

#ifdef ENGLISH
//English mode   Defined on main.h
const char *AboutTitle = "Feature of LCD Clock";
const char TextAbout[] = "- About";
const char *AboutMsg[] = {
    "- No need time adjust by GPS unit\r\n",
    "- Easy to set alarm by rotary + switch\r\n",
    "  Smooth by touching screen. Stop by slide switch\r\n",
    "- Can select 3types screen or user setting by touching on the bottom left.\r\n",
    "- Push Gear Button to setting form\r\n",
    "  Customizing parts place, color, etc.\r\n",
    "  Can use favorite font of BMP files on SDcard \r\n",
};
#else
//Japanese mode
const char *AboutTitle = "����LCD Clock�̋@�\\ ";
const char TextAbout[] = "- About (Need SJIS font)";
const char *AboutMsg[] = {
    "��GPS���j�b�g������Ύ����C���͕s�v�I\r\n",
    "���A���[���ݒ�́A�܂݉�+SW�ƊȒP\r\n",
    "�@�A���[��������A��ʃ^�b�`�ŃX�k�[�Y�BSW�I�t�Ŋ��S��~\r\n",
    "����ʂ͐ݒ�ς�3��ƃ��[�U�ݒ�̒����炨�D�݂ŁB�ؑւ͉�ʍ��������^�b�`\r\n",
    "�����ԃ{�^���ŁA�ݒ��ʂ�\r\n",
    " �E���i�̔z�u�A�F�Ȃǂ��J�X�^�}�C�Y��\r\n",
    "�@SD�J�[�h��bmp�t�@�C�����i�[����΁A���D���ȃt�H���g���g�p��\r\n",
};
const char *AboutError = "Can't show the message in Japanese.\r\n   Prepare JIS16.dat";
#endif


uint8_t AboutFlag = 0;

//�e�s�̍���
#define SettingItemH   30

//�ݒ��ʗp���\�[�X
MainFormRsc SettingRsc[] = {
    //ID,                x,   y,                 xw,  yw,           fontcode,   fontMag,    str,                color,
    { SettingTitleObj,   0,   0,                 0,   0,            FontNormal, FontMagx22, (char*)SettingTextTitle1, WHITE, },   //�^�C�g��
    //���ږ�
    { CustomizingObj,    20, 30,                 200, SettingItemH, FontNormal, FontMagx12, (char*)TextCustomizing,  WHITE, },
    { TimeDateAdjustObj, 20, 30+SettingItemH*1,  200, SettingItemH, FontNormal, FontMagx12, (char*)TextTimeAdjust,   WHITE, },
    { AboutObj,          20, 30+SettingItemH*2,  200, SettingItemH, FontNormal, FontMagx12, (char*)TextAbout,        WHITE, },
    { FeatureObj,        20, 30+SettingItemH*3,  200, SettingItemH, FontNormal, FontMagx12, (char*)TextFeature,      WHITE, },
    { RedigitizingObj,   20, 30+SettingItemH*4+5,200, SettingItemH, FontNormal, FontMagx12, (char*)TextRedigitizing, WHITE, },
    //�{�^��    
    { SettingBtnCloseObj,240, 200,                70, SettingItemH, FontNormal, FontMagx12, (char*)CloseText,        WHITE, },  // Close
};


/*
 * About
 */
void AboutText() {
    char str[150];

    lcd_fill(BLACK); //��ʂ��N���A(�^����)

    //�����t�H���g��ݒ�
#ifdef ENGLISH 
    display_setTextSize(FontMagx12);
    display_drawChars(0, 0, AboutTitle, WHITE, TextbgColor, FontMagx12);
#else
    SetKanjiFont(SJIS16);
    if (CurrentKanjiFont.fontcode) {
        display_drawChars(20, 0, (char *)AboutTitle, WHITE, TextbgColor, FontMagx11);
        display_setTextSize(FontMagx11);
#endif
        
        display_setCursor(0, 20);
//        display_setTextPitch(0, 0);
        display_setTextColor(WHITE, BLACK);
        display_setWrap(true);
        
        for (uint8_t jj = 0; jj < 7; jj++) {
            display_puts((char *)AboutMsg[jj]);
        }
        
        display_setWrap(false);  
#ifndef ENGLISH 
        SetKanjiFont(0);
    } else {
        //�����t�H���g�Ȃ����̃��b�Z�[�W
        display_drawChars(20, 100, (char *)AboutError, WHITE, TextbgColor, FontMagx12);
    }
#endif
    
    display_drawChars(20, 220, (char*)TouchMsg, GREY, TextbgColor, FontMagx12);

}

/*
 * �ݒ��ʂ��^�b�`�������̏��u
 * �@���W�ɂ���ď������e�����
 */
void TouchProcedureSetting() {
    char str[100];
    
#ifdef DEBUG
    sprintf(str, "TXY=%03d, %03d", TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, BLACK, FontMagx11);
#endif
    if (AboutFlag) {
        //About/Feature�\�����Ȃ�A�^�b�`������A���̉�ʂɖ߂��̂�D�悳����
        PushEvent(EventFormUpdate);
        AboutFlag = 0;
        return;        
    } 
    
    if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnCloseObj])) {   //close�{�^��
        remount();  //
        ClearBuffer();
        SetEventHandler(MainEventHandler);  //���C���ɖ߂�
        PushEvent(EventFormInit);
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[CustomizingObj])) {
        ClearBuffer();
        CopyCustomDataToTemp();
        SetEventHandler(CustomizingEventHandler);  //�J�X�^�}�C�Y
        PushEvent(EventFormInit);
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[RedigitizingObj])) { //�ăf�W�^�C�Y
        TouchAdjust();
        PushEvent(EventFormUpdate);
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[TimeDateAdjustObj])) {
        PopupEventHandler(TimeAdjustEventHandler);   //�����C��
        PushEvent(EventFormInit);
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[AboutObj])) {    //���̃\�t�g�ɂ���
        AboutText();
        AboutFlag = 1;
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[FeatureObj])) {    //Feature
        OpeningScreen(1);
        AboutFlag = 1;
    }
}
        
        
/*
 * cls=1�̎��́A��ʏ������Ă���`��
 * �ݒ��ʂ�`��
 */
void DrawSettingForm(uint8_t cls) {
    uint8_t jj;

    if (cls) lcd_fill(TextbgColor);
    
    SetFont(FontNormal);  //�����t�H���g�ݒ�

    //�^�C�g���A���ږ��\��
    jj = SettingTitleObj;
    display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    
    //�e�I�u�W�F�N�g�̏���\��
    for (jj = CustomizingObj; jj < SettingBtnCloseObj; jj++) {
        display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    }
    
    //�{�^���`��  Close
    jj = SettingBtnCloseObj; 
    DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 10);

}

/*
 * �ݒ�p�C�x���g�n���h��
 * 
 */
void SettingEventHandler() {
    uint8_t evt;
    char str[100];
    
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
        case EventFormInit:
            CurrentForm = SettingRsc;
        case EventFormUpdate:
            DrawSettingForm(1);
            break;
        case EventTouchDown:
            TouchProcedureSetting();
            break;
    }
}
