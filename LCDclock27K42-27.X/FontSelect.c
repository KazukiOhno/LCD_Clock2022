/*
 * SD�J�[�h��̃t�H���_���t�H���g�t�H���_�Ƒz�肵�āA���X�g�\��
 * ���̃t�H���_�����t�H���g���Ƃ��Ď�荞��
 * 
 */

#include "FontSelect.h"
#include <string.h>

#include "main.h"
#include "font.h"
#include "LCD320x240color.h"
#include "Customizing.h"
#include "FontMag.h"
#include "Draw.h"


enum FontSelectionRscObject {
    TitleFontSelObj,
    BtnOKObj,
    BtnCancelObj,
    
    BtnObj,         //SD�J�[�h��̃t�H���g���X�g max.28
    BtnInternalObj, //�����t�H���g
};

const char TitleFontSelection[] = "Select Font for ";   //for�̌��ɑΏۂ̃I�u�W�F�N�g��

//�t�H���g���e�[�u���̌`�ŁA���X�g���@�@�@4��*7�s��z��
#define FontTableX 10
#define FontTableY 21
#define FolderW 75
#define FolderH 20

MainFormRsc FontSelRsc[] = {
    //ID,              x,   y,   xw, yw, fontcode,   fontMag,    str,                color,
    { TitleFontSelObj, 5,   3,    0,  0, FontNormal, FontMagx12, (char*)TitleFontSelection, WHITE, },   //�^�C�g��
    //�{�^��
    { BtnOKObj,      205, 200,   40, 30, FontNormal, FontMagx12, (char*)BtnNameOK,     WHITE, },  // OK
    { BtnCancelObj,  250, 200,   60, 30, FontNormal, FontMagx12, (char*)BtnNameCancel, WHITE, },  // Cancel
    //�t�H���g���e�[�u����1�̃I�u�W�F�N�g�Ƃ��Ē�`
    { BtnObj,         FontTableX, FontTableY,           FolderW*4+4, FolderH*8+4, FontNormal, FontMagx12, NULL, WHITE, },   // �t�H���_
    //�����t�H���g�̕\���G���A
    { BtnInternalObj, FontTableX, FontTableY+FolderH*7, FolderW*4+4, FolderH+4,   FontNormal, FontMagx12, NULL, WHITE, },   // �����t�H���g
    
};

#define MaxFontNum  28
uint8_t FontNum;
char fontNameList[30][9];   //�t�H���g���̃��X�g�@�@�@�ő�28�܂łɐ�������

/*
 * SD�J�[�h����t�H���_��ǂݏo�� (�t�H���g�t�H���_�ȊO���Ώۂɓ����Ă��܂����C�ɂ��Ȃ�)
 * �t�H���g�f�B���N�g�����X�g��\������
 * 1�t�H���g=FolderW x FolderH�̗̈�ɋL���@�@��4�A�c7��28�܂łɐ�������
 * �擾�����t�H���_��=�t�H���g���́AfontNameList�֊i�[
 */
FRESULT fontFolder(char* path) {
    FRESULT res;
    FILINFO fno;
    FFDIR dir;
    char *fn;   /* ��Unicode�\����z�� */
    int16_t xx, yy;

    res = f_opendir(&dir, path);                       /* �f�B���N�g�����J�� */
    if (res == FR_OK) {
        yy = CurrentForm[BtnObj].y;
        xx = 0;
        FontNum = 0;
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* �f�B���N�g�����ڂ�1�ǂݏo�� */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* �G���[�܂��͍��ږ����̂Ƃ��͔����� */
            if (fno.fname[0] == '.') continue;             /* �h�b�g�G���g���͖��� */
            fn = fno.fname;
            if (fno.fattrib & AM_HID) {
                // Hidden�t�@�C���͕\�����Ȃ�
            } else if (fno.fattrib & AM_DIR) {          /* �f�B���N�g�� */
                sprintf(fontNameList[FontNum], "%s", fn);     //�f�B���N�g��=�t�H���g�t�H���_��
                display_drawChars(CurrentForm[BtnObj].x + xx, yy, fontNameList[FontNum], CurrentForm[BtnObj].color, TextbgColor, CurrentForm[BtnObj].fontmag);
                FontNum++;
                if (xx < 200) xx += FolderW;
                else {
                    xx = 0;
                    yy += FolderH;                    
                }
//            } else {                                       /* �t�@�C�� */
//                printf("%s/%s\n", path, fn);
            }
            if (FontNum >= MaxFontNum) break;   //����t�H���g�t�@�C���̍ő�𐧌��B��ʕ\��������Ȃ�����
        }
    }
    return res;
}


/*
 * �v���r���[�ɂ��{�^���������Ă��܂����Ƃ����邽��
 */
void drawButton() {
    uint8_t jj;

    //�{�^���`��
    for (jj = BtnOKObj; jj <= BtnCancelObj; jj++) {
        DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 10);
    }
}

/*
 * SD�J�[�h���̃t�H���g���X�g��\������
 */
void drawFontSelection() {
    uint8_t jj;
    int16_t xx, yy;
    char str[100];
    
    //�g�`��
    display_fillRoundRect(0,  0, 320, 240, 10, BLACK);
    display_drawRoundRect(0,  0, 320, 240, 10, WHITE);

    SetFont(FontNormal);  //�����t�H���g�ݒ�

    //�^�C�g���\��
    jj = TitleFontSelObj;
    sprintf(str, "%s%s", CurrentForm[jj].str, CustomSettingText[TargetObj]);
    display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);

    //�{�^���`��
    drawButton();

    //�g��`��
    display_drawRoundRect(CurrentForm[BtnObj].x-3, CurrentForm[BtnObj].y-3, CurrentForm[BtnObj].xw, CurrentForm[BtnObj].yw, 0, GREY);
    display_drawRoundRect(CurrentForm[BtnInternalObj].x-3, CurrentForm[BtnInternalObj].y-3, CurrentForm[BtnInternalObj].xw, CurrentForm[BtnInternalObj].yw, 0, GREY);
    
    //�t�H���g�f�B���N�g�������[�g�ȉ��ŒT��
    strcpy(str, "/");
    fontFolder(str);
    
    //�����t�H���g���
    yy = CurrentForm[BtnInternalObj].y;
    xx = CurrentForm[BtnInternalObj].x;
    for (jj = 0; jj < InternalFontNum; jj++) {
        sprintf(str, "%s*", FontFolder[jj]);
        display_drawChars(xx, yy, str, CurrentForm[BtnInternalObj].color, TextbgColor, CurrentForm[BtnInternalObj].fontmag);
        xx += FolderW;
    }
    
}


/*
 * �w�肳�ꂽ�t�H���g�ŁA�Ώۂ̃I�u�W�F�N�g�ɋ߂��`�ŕ\�����Č�����
 */
void PreviewFont(int16_t x, int16_t y, uint8_t fontmag, int8_t xpitch, uint16_t color) {
    static char str[20];
    
    display_setTextSize(fontmag);
    display_setTextPitch(xpitch, 0);
    display_setCursor(x, y);        //�`��J�n���W
    display_setColor(color);
    
    switch (TargetObj) {
        case DateObj:
            sprintf(str, "22/4/5(%c)", FSunday);
            break;
        case TimeObj:
            //small�t�H���g���v���r���[�ł���悤��
            sprintf(str, "2:3");
            display_puts(str);
            
            sprintf(str, "56");
            sprintf(Smaller, "s");  //�������t�H���g�w��
            break;
        case AlarmObj:
            sprintf(str, "ALM+06:30");
            break;
        case TempObj:
            sprintf(str, "-12.3%c", FdegC);
            break;
        case HumidityObj:
            sprintf(str, "45%%");
            break;
        default:
            sprintf(str, "10%c11%c", FSunday +1, FSunday +1);
            break;
    }
    display_puts(str);
    Smaller[0] = '\0';  //���ɖ߂�
    display_setTextPitch(0, 0); //���ɖ߂�
    
}
                        

/*
 * �^�b�`�������̏��u
 * �@���W�ɂ���ď������e�����
 */
void TouchProcedureFontSelect() {
    int16_t xx, yy;
    uint8_t selectFontNum, newfont;
    static int16_t prevX = FontTableX-2;
    static int16_t prevY = FontTableY-2;
    
     if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnObj])) {
        xx = (TouchX - CurrentForm[BtnObj].x) / FolderW;
        yy = (TouchY - CurrentForm[BtnObj].y) / FolderH;
                
        selectFontNum = (uint8_t)(xx + yy * 4);     //�I�����ꂽ�t�H���g�ԍ�
        
        PreviewFont(10, FontTableY+FolderH*8+5, FontMagx11, 0, BLACK);  //�O�̕���������
        
        //�^�b�v�����t�H���g�ɘg��t����
        display_drawRect(prevX, prevY, FolderW, FolderH, BLACK);    //�O�̘g������
        //�V���ɑI�������t�H���g�̍��W��ێ����A�g��`��
        prevX = xx*FolderW + CurrentForm[BtnObj].x-2;
        prevY = yy*FolderH + CurrentForm[BtnObj].y-2;

        if (selectFontNum < FontNum) {
            strcpy(CustomFontDir[FontTemp - FontUser0], fontNameList[selectFontNum]);
            newfont = FontTemp;
        } else if (selectFontNum >= MaxFontNum && selectFontNum < MaxFontNum+ InternalFontNum) {
            //�����t�H���g�́A3�B��ʃ^�b�`�Ŏ擾�ł���t�H���g�ԍ��́AMaxFontNum�ȍ~
            newfont = selectFontNum - MaxFontNum;
            //�{���t�H���g�����i�[���邪�A�����t�H���g�R�[�h��[0]�Ɋi�[
            CustomFontDir[FontTemp - FontUser0][0] = selectFontNum - MaxFontNum;    //'\0';
        } else {
            return;
        }
        
        display_drawRect(prevX, prevY, FolderW, FolderH, WHITE);    //�V���ɘg��`��
        
        SetFont(newfont);
        //�I�������t�H���g���v���r���[
        PreviewFont(10, FontTableY+FolderH*8+5, FontMagx11, 0, WHITE);
        SetFont(FontNormal);  //
        drawButton();   //�v���r���[�Ń{�^���������邱�Ƃ����邽��
        SetFont(newfont);
        
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnCancelObj])) {
        ClearBuffer();
        ReturnEventHandler();
        PushEvent(EventFormInit);
        
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnOKObj])) {
        //OK�̎��A�t�H���g��=�t�H���_�����^�[�Q�b�g�̃I�u�W�F�N�g�̃f�[�^�Ƃ���
        if (CustomFontDir[FontTemp - FontUser0][0] >= ' ') {
            strcpy(CustomFontDir[TargetObj], CustomFontDir[FontTemp - FontUser0]);
            TempObjData[TargetObj].fontcode = (uint8_t)(FontUser0 + TargetObj);
        } else {
            TempObjData[TargetObj].fontcode = CustomFontDir[FontTemp - FontUser0][0];
        }
        
        ClearBuffer();
        ReturnEventHandler();
        PushEvent(EventFormInit);
    }    
}

/*
 * �t�H���g��I������t�H�[���̃C�x���g����
 */
void FontSelectEventHandler() {
    uint8_t evt;
    
    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //�C�x���g�Ȃ����́A���ʂɋ��肳���Ȃ��悤�A10ms��wait
        return;
    }
    
    switch (evt) {
        case EventFormInit:
            CurrentForm = FontSelRsc;
            drawFontSelection();            
            break;
            
        case EventTouchDown:
            TouchProcedureFontSelect();
            break;
    }
    
}
