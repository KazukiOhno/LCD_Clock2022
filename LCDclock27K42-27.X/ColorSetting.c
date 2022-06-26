/*
 * ColorSetting EventHandler
 * 
 * 2022/5/15
 * 
 * Color Setting for Customizing
 * 
 */

#include "ColorSetting.h"
#include "main.h"
#include "LCD320x240color.h"
#include "Customizing.h"
#include "Draw.h"
#include "font.h"

const char TitleColorSetting[] = "Color Setting";
const char RGBtext[][2] = {"R", "G", "B"};
const char RGBtext2[] = "Actual Color";
const char BtnSelect0[] = "On";
const char BtnSelect1[] = "Off";
const char BtnWeekday[] = "Day";
const char BtnSatday[] = "Sat";
const char BtnSunday[] = "Sun";


//�J���[�ݒ�p�ɁA���L�I�u�W�F�N�g�̏��Ƀ��\�[�X��ݒ肷��
enum ColorRscObject {
    TitleObj,
    RbarObj,    //1
    GbarObj,
    BbarObj,
    ColorBoxObj,
    OKBtnObj,   //9
    CancelBtnObj,
    Select0BtnObj,
    Select1BtnObj,
    SelectWeekdayBtnObj,
    SelectSatBtnObj,
    SelectSunBtnObj,
};

MainFormRsc ColorSettingRsc[] = {
    //ID,        x, y, xw, yw, fontcode,   fontMag,    str,                      color, format, attribute
    { TitleObj,   0,  0, 0, 0, FontNormal, FontMagx22, (char*)TitleColorSetting, WHITE, 0, },  //Title
    //RGB�̃J���[�o�[�A�㕔�Ƀ^�C�g���A�����ɒl�����̃��\�[�X�𗘗p���ĕ`��
    { RbarObj, 30, 40, 30, 128, FontNormal, FontMagx22, (char*)RGBtext[0], WHITE, 0, },  //Rbar
    { GbarObj, 80, 40, 30, 128, FontNormal, FontMagx22, (char*)RGBtext[1], WHITE, 0, },  //Gbar
    { BbarObj,130, 40, 30, 128, FontNormal, FontMagx22, (char*)RGBtext[2], WHITE, 0, },  //Bbar
    //�ݒ肵���F��\������box�A����у^�C�g��
    { ColorBoxObj, 180, 40, 100, 100, FontNormal, FontMagx12, (char*)RGBtext2, WHITE, 0, },

    { OKBtnObj,     170, 200, 60, 30, FontNormal, FontMagx12, (char*)BtnNameOK,     WHITE, 0, },  // OK
    { CancelBtnObj, 250, 200, 60, 30, FontNormal, FontMagx12, (char*)BtnNameCancel, WHITE, 0, },  // Cancel
    
    //�A���[���̐F�ݒ�̎������L����������Z���N�^�@�@�@�A���[���I���A�I�t���ꂼ��̐F��ݒ�ł���悤
    { Select0BtnObj, 30, 200, 50, 30, FontNormal, FontMagx12, (char*)BtnSelect0, WHITE, 0, },  // Alarm On
    { Select1BtnObj, 80, 200, 50, 30, FontNormal, FontMagx12, (char*)BtnSelect1, WHITE, 0, },  // Alarm Off
    
    //�J�����_�[�̗j���̐F�ݒ�̎������L�������Z���N�^�@�@�@�����A�y�A���̐F�����ꂼ��ݒ�ł���悤
    { SelectWeekdayBtnObj, 20, 200, 40, 30, FontNormal, FontMagx12, (char*)BtnWeekday, WHITE, 0, },  // ����
    { SelectSatBtnObj,     60, 200, 40, 30, FontNormal, FontMagx12, (char*)BtnSatday,  WHITE, 0, },  // �y�j
    { SelectSunBtnObj,    100, 200, 40, 30, FontNormal, FontMagx12, (char*)BtnSunday,  WHITE, 0, },  // ���j
    
};

uint8_t TargetColor;    //���[�^���ŕω�������Ώۂ̐F�@0-2=R,G,B
uint8_t tRGB[3];        //�e�F�ɕ������J���[�R�[�h
uint16_t tempMultiRGB[3];   //�����F�̐ݒ肪�K�v�ȃA���[���A�J�����_�[�̐F���b��i�[
uint8_t Selector;   //�A���[���A�j���̐F��ݒ肷�鎞�A�ǂ̍��ڂ̐F��I�����Ă��邩


/*
 * �~���̂ǂ̈ʒu���A�����w��ʒu�ɕ`��
 *    �O�̖����������Ă������`��
 * x,y�́A���̂Ƃ����������w��
 */
void DrawArrow(int16_t x, int16_t y, uint8_t rgb) {
    static int16_t preY[3];    //RGB���ꂼ��̖��̈ʒu��ێ�
    
    //�O�̖����������Ă������`��
    display_fillRect(x - 13, preY[rgb] - 3, 14, 7, TextbgColor);
    preY[rgb] = y;
    
    display_fillTriangle(x, y, x-3, y-3, x-3, y+3, WHITE);
    display_fillRect(x-13, y-1, 10, 3, WHITE);
}

/*
 * �^�b�v������A���[�^���񂵂��肵�āA�J���[�R�[�h�ω����������ɁA�e�F�̐��l�A���A���݂̐F���X�V
 * color=0-2    RGB�̂ǂꂩ���X�V
 */
void updateColorBar(uint8_t rgb) {
    uint8_t kk, yy;
    uint16_t color;
    char str[10];
    
    //���Y�F�̒l���e�J���[�o�[�̉��ɕ\��
    kk = RbarObj + rgb;
    
    //���[�^���œ������Ώۂ̒l��ԐF�ɂ���
    if (rgb == TargetColor) color = RED;
    else color = CurrentForm[kk].color;
    sprintf(str, "%3d", tRGB[rgb]);  //RGB�̏�
    display_drawChars(CurrentForm[kk].x, CurrentForm[kk].y+CurrentForm[kk].yw +8, str, color, TextbgColor, FontMagx12);
    
    //����`��
    if (rgb==1) yy = (64 - tRGB[rgb])*2;  //G�́A6bit
    else yy = (32 - tRGB[rgb])*4;       //RB�́A5bit
    //�O�̖����������Ă������`��
    DrawArrow(CurrentForm[kk].x -2, CurrentForm[kk].y + yy-2, rgb);        
}

void updateColorBox() {
    uint8_t kk;
    uint16_t color;
    
    //���݂̐ݒ�����̐F���E����box�ɕ\��
    kk = ColorBoxObj;
    color = (uint16_t)((tRGB[0] <<11) | (tRGB[1] <<5) | tRGB[2]);
    display_drawRect(CurrentForm[kk].x -1, CurrentForm[kk].y-1, CurrentForm[kk].xw +2, CurrentForm[kk].yw +2, WHITE);
    display_fillRect(CurrentForm[kk].x, CurrentForm[kk].y, CurrentForm[kk].xw, CurrentForm[kk].yw, color);

}


/*
 * �F�ݒ�p�̏�����ʂ�`��
 * �@�ω��Ȃ������Ȃ̂ŁA1�񂾂��\������Ηǂ�����
 */
void DrawBaseForm() {
    char str[100];
    uint8_t jj, kk, mm;
    uint16_t color;
    
    SetFont(FontNormal);
    //�^�C�g���ƑΏۂƂȂ�I�u�W�F�N�g����\��
    sprintf(str, "%s: %s", CurrentForm[0].str, CustomSettingText[TargetObj]);
    display_drawChars(CurrentForm[0].x, CurrentForm[0].y, str, CurrentForm[0].color, TextbgColor, CurrentForm[0].fontmag);
            
    kk = RbarObj;
    for (jj=0; jj<3; jj++) {
        //�e�J���[�o�[�̖��́@�@�o�[�̍��ォ��A+10, -20���N�_�ɕ`��
        display_drawChars(CurrentForm[kk].x+10, CurrentForm[kk].y-20, CurrentForm[kk].str, CurrentForm[kk].color, TextbgColor, CurrentForm[kk].fontmag);
        //�J���[�o�[�̃O���f�[�V�����\��
        display_drawRect(CurrentForm[kk].x -1, CurrentForm[kk].y-1, CurrentForm[kk].xw +2, CurrentForm[kk].yw +2, WHITE);
        for (mm = 0; mm < CurrentForm[kk].yw; mm++) {
            color = mm/4;   //�c128�h�b�g����̂ŁAG���܂߂āA5bit�J���[�Ƃ݂Ȃ��A4�h�b�g���ɋP�x��ύX���Ă���
            if (jj==1) color = color << 6;  //G�́A6bit���炷�ƁA���傤�Ǐ��5bit���ω�����`
            else if (jj==0) color = color << 11;    //R�́A11bit�V�t�g
            display_fillRect(CurrentForm[kk].x, CurrentForm[kk].y + CurrentForm[kk].yw - mm-1, CurrentForm[kk].xw, 1, color);
        }
        kk++;
    }
    //�ݒ肵���F��\������Actual Color��box�̏�ɕ\������^�C�g��
    jj = ColorBoxObj;
    display_drawChars(CurrentForm[jj].x +5, CurrentForm[jj].y -20, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);

    //�{�^���`��  Cancel, OK
    for (jj = OKBtnObj; jj <= CancelBtnObj; jj++) {
        DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 10);
    }
    
}

/*
 * �F�ݒ�p�̏�����ʂŕω����镔����`��
 */
void DrawColorSettingForm() {
    uint8_t jj, kk;
    uint16_t fc, bc;

    //���A�J���[�̒l�A���ۂ̐F��`��    
    for (jj = 0; jj < 3; jj++) {
        updateColorBar(jj);
    }
    updateColorBox();
    
    if (TargetObj == AlarmObj) {
        //�A���[���̎��́A�Z���N�^�ŃI�����ƃI�t���̐F�ݒ�ł���悤�ɁB
        for (kk = Select0BtnObj; kk <= Select1BtnObj; kk++) {
            //�{�^���̒����Ƀ{�^������\��
            if (kk- Select0BtnObj == Selector) {
                //�I������Ă��鎞
                fc = TextbgColor;
                bc = CurrentForm[kk].color;
            } else {
                //�I������Ă��Ȃ���
                fc = CurrentForm[kk].color;
                bc = TextbgColor;
            }
            DrawButton(CurrentForm[kk], fc, bc, CurrentForm[kk].color, 0);
        }
    } else if (TargetObj == CalendarObj) {
        //�J�����_�[�̗j���F�̎w��
        for (kk = SelectWeekdayBtnObj; kk <= SelectSunBtnObj; kk++) {
            //�{�^���̒����Ƀ{�^������\��
            if (kk-  SelectWeekdayBtnObj == Selector) {
                //�I������Ă��鎞
                fc = TextbgColor;
                bc = CurrentForm[kk].color;
            } else {
                //�I������Ă��Ȃ���
                fc = CurrentForm[kk].color;
                bc = TextbgColor;
            }
            DrawButton(CurrentForm[kk], fc, bc, CurrentForm[kk].color, 0);
        }
    }
}

/*
 * ���[�^���[�́A�A���I�ɃC�x���g�������邪�A�C�x���g�������ɏ����ł��Ȃ��Ă��悭�A
 * RotCount=0�łȂ���������������Ηǂ�
 * �Ώۂ̐F�̒l���㉺������
 */
void RotaryProcedureColor() {
    uint8_t jj, kk;
    int16_t delta;

    //�m�[�}�����̃��[�^���[SW�̓A���[��������ύX
    if (RotCount == 0) return;
    delta = RotCount;
    RotCount = 0;   //����������0�ɖ߂�

    jj = TargetColor;
    if (tRGB[jj] + delta < 0 ) {
        tRGB[jj] = 0; //�ŏ��l��0   
    } else {
        if (TargetColor == 1) kk = 2;   //G�́A6bit�J���[�Ȃ̂�2�{
        else kk = 1;

        if (tRGB[jj] + delta >= 32*kk) tRGB[jj] = 32*kk -1; //max�𒴂��Ȃ��悤
        else tRGB[jj] = tRGB[jj] + (uint8_t)delta;
        
    }
    updateColorBar(jj);
    updateColorBox();

}

/*
 * �F�ݒ�o�[��ŁA�^�b�`���ꂽ���̏���
 */
void TouchProcedureColorSetting() {
    uint8_t jj, kk;
    int16_t yd;
    char str[100];

#ifdef DEBUG2
    sprintf(str, "T=%3d,%3d", TouchX, TouchY);
//        sprintf(str, "T=(%4d,%4d)(%3d,%3d)", TouchRawX, TouchRawY, TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, TextbgColor, FontMagx11);
#endif

    //�J���[�o�[�̃G���A�̂ǂ����^�b�`���������`�F�b�N
    kk = RbarObj;    //�J���[�o�[��Rsc
    for (jj = 0; jj< 3; jj++) {
        if (ButtonPush(TouchX, TouchY, &CurrentForm[kk])) {
            yd = TouchY - CurrentForm[kk].y;
            if (jj==1) tRGB[jj] = (uint8_t)(63 - yd/2); //G�́A64�K����2�h�b�g��
            else tRGB[jj] = (uint8_t)(31 - yd/4);       //RB�́A32�K����4�h�b�g��
            TargetColor = jj;   //�Ώۂ̐F���w��
            updateColorBar(jj);
            updateColorBox();
            RepeatKey = 1;
            return;
        }
        kk++;
    }
    
    tempMultiRGB[Selector] = (uint16_t)((tRGB[0] <<11) | (tRGB[1] <<5) | tRGB[2]);   //���̃f�[�^���i�[
    if (ButtonPush(TouchX, TouchY, &CurrentForm[OKBtnObj])) {
        //OK�{�^�����������̏��u�@�@�A���[���A�J�����_�[�A���̑��Ɋւ�炸�ȉ��œ���
        TempObjData[TargetObj].color = tempMultiRGB[0];
        TempObjData[TargetObj].attribute = tempMultiRGB[1]; //�A���[���I�t�̐F�́Aattribute��
        //�y�j�̐F�́Aattribute�ɁB
        TempObjData[TargetObj].attribute2 = tempMultiRGB[2];    //���j�̐F�́Aattribute2��
        
        ReturnEventHandler();
        PushEvent(EventFormInit);
    
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[CancelBtnObj])) {
        //Cancel�����ꂽ��
        ReturnEventHandler();
        PushEvent(EventFormInit);
        
    } else {
        //�Z���N�^�{�^��������Alarm�ƃJ�����_�[�̃^�u�̏���
        if (TargetObj == AlarmObj) {
            for (jj = Select0BtnObj; jj <= Select1BtnObj; jj++) {
                if (ButtonPush(TouchX, TouchY, &CurrentForm[jj])) {
                    Selector = jj - Select0BtnObj;
                    PushEvent(EventColorFormUpdate);
                    return;
                }
            }
        } else if (TargetObj == CalendarObj) {
            for (jj = SelectWeekdayBtnObj; jj <= SelectSunBtnObj; jj++) {
                //�����A�y�j�A���j
                if (ButtonPush(TouchX, TouchY, &CurrentForm[jj])) {
                    Selector = jj - SelectWeekdayBtnObj;
                    PushEvent(EventColorFormUpdate);
                    return;
                }
            }
        }
    }
}


/*
 * �F�ݒ�p�C�x���g�n���h��
 * 
 */
void ColorSettingEventHandler() {
    uint8_t evt, jj, kk;
    char str[100];
    
    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //�C�x���g�Ȃ����́A���ʂɋ��肳���Ȃ��悤�A10ms��wait
        return;
    }
#ifdef DEBUG
    sprintf(str, "e=%03d", evt);
    display_drawChars(250, 40, str, WHITE, TextbgColor, FontMagx11);
#endif
    
    switch (evt) {
        case EventColorFormInit:
            lcd_fill(TextbgColor);
            //���̃f�[�^��temp�ϐ��ɐݒ�
            CurrentForm = ColorSettingRsc;
            tempMultiRGB[0] = TempObjData[TargetObj].color;
            tempMultiRGB[1] = TempObjData[TargetObj].attribute;
            tempMultiRGB[2] = TempObjData[TargetObj].attribute2;

            Selector = 0;   //�����l�́A0���ύX�^�[�Q�b�g�@�@�I�u�W�F�N�g�ɂ�蕡�����鎞��
            DrawBaseForm();
            //�ȍ~�AUpdate�Ə������ʉ�
        case EventColorFormUpdate:
            //Selector��ύX�������͂�������
            tRGB[0] = tempMultiRGB[Selector] >> 11;            //R
            tRGB[1] = (tempMultiRGB[Selector] >> 5) & 0x3f;    //G
            tRGB[2] = tempMultiRGB[Selector] & 0x1f;           //B
            TargetColor = 0;    //Red���������A�Ώۂ̐F�Ƃ���
            DrawColorSettingForm();
            break;
            
        case EventRotary:
            RotaryEventCount--;
            RotaryProcedureColor();
            break;
        case EventTouchDown:
            //���Y�F�̒l�𔒂ŕ\��
            kk = RbarObj;
            for (jj = 0; jj < 3; jj++) {
                sprintf(str, "%3d", tRGB[jj]);  //RGB�̏�
                display_drawChars(CurrentForm[kk].x, CurrentForm[kk].y+CurrentForm[kk].yw +8, str, CurrentForm[kk].color, TextbgColor, FontMagx12);
                kk++;
            }
        case EventTouchRepeat:
            TouchProcedureColorSetting();
            break;
    }
}
