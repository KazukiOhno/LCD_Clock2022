/*
 * �J�X�^�}�C�Y�f�[�^�̕ۑ��A�ǂݏo��
 */

/*
 * SD�J�[�h�ɐݒ���̃f�[�^��ۑ��A���o�����s��
 * ������t�^�ł���B���p79�����܂�
 * �ő�8�܂ł̃t�@�C�����ȉ��̂悤��Data�f�B���N�g�����ɁA�Œ�t�@�C�����Ŋi�[
 * /Data/Data1.dat
 *       Data2.dat
 *       Data3.dat
 *       Data4.dat
 *       Data5.dat
 *       Data6.dat
 *       Data7.dat
 *       Data8.dat
 */

#include "CustomData.h"
#include <string.h>
#include <stdlib.h>     //atoi()

#include "main.h"
#include "font.h"
#include "Customizing.h"
#include "LCD320x240color.h"
#include "Draw.h"
#include "Setting.h"
#include "AlarmSound.h"
#include "TouchXT2046.h"


//�ȉ��ɂ��A�������2�o�C�g�����������Ă�warning�o�Ȃ��Ȃ�Ƃ������Ƃ�������
#pragma jis

const char *DataFolder = "Data";

#define MaxMemo     80
char Memo[MaxMemo];  //�t�@�C�����ƂɋL���ł��镶����
char TempMemo[MaxMemo]; //�ҏW�p�b�胁��
uint8_t CsrByte = 1;   //�J�[�\���̈ʒu�̕�����1�o�C�g��2�o�C�g��
uint8_t Csr;        //�J�[�\���ʒu
uint8_t CsrDraw;    //�`���̉��z�J�[�\���ʒu(1�s35�����Ƃ����ꍇ)
char **CurrentKeyStr;
uint8_t adj[3]; //�e�s�ɂ����āA�Z�k�\��������
uint8_t SaveLoad;   //1: Save, 0: Load

#define MemoScrX    35
#define MemoScrY    3

uint8_t TargetDataNo;
char Filename[50];

enum CustomDataRscObject {
    DataTitleObj,
    DatasubTitleObj,
    DeleteMsgObj,   //�\���ʒu�́A���������v���O�����̗e�Ղ����炱���ɒu��
    //�I�u�W�F�N�g��
    Data1Obj,
    Data2Obj,
    Data3Obj,
    Data4Obj,
    Data5Obj,
    Data6Obj,
    Data7Obj,
    Data8Obj,
    //�{�^��
    BtnCloseObj,
    Default1Obj,    //�f�t�H���g�ݒ�Ǐo���p
    Default2Obj,
    Default3Obj,
};

#define MAXDataNum 8

const char *DataTextTitle[] = {
    "Load Setting Data",
    "Save Setting Data", 
};
const char DefText[][3] = {
  "1",
  "2",
  "3",
};
const char DataTextsubTitle[] = "/Data/Data?.dat";
const char DeleteMsg[] = "<Long push to delete>";

//�e�s�̍���
#define DataItemH   20

//Save/Load�p���\�[�X
MainFormRsc SaveLoadRsc[] = {
    //ID,               x,   y             xw,  yw,        fontcode,   fontMag,    str,                color,
    { DataTitleObj,     0,   0,            0,   0,         FontNormal, FontMagx22, NULL, WHITE, },   //�^�C�g���͌�Őݒ�
    { DatasubTitleObj, 10,  20,            0,   0,         FontNormal, FontMagx12, (char*)DataTextsubTitle, WHITE, },
    { DeleteMsgObj,   150,  20,            0,   0,         FontNormal, FontMagx12, (char*)DeleteMsg, LIGHTGREY, },
    //�f�[�^���X�g
    { Data1Obj,        10, 40,             300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data2Obj,        10, 40+DataItemH*1, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data3Obj,        10, 40+DataItemH*2, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data4Obj,        10, 40+DataItemH*3, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data5Obj,        10, 40+DataItemH*4, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data6Obj,        10, 40+DataItemH*5, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data7Obj,        10, 40+DataItemH*6, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data8Obj,        10, 40+DataItemH*7, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    //�{�^��
    { BtnCloseObj,    240, 200,             70, 30,        FontNormal, FontMagx12, (char*)CloseText, WHITE, },  // Close
    
    { Default1Obj,     40, 200,             50, 30,        FontNormal, FontMagx12, (char*)DefText[0], WHITE, },  // 1
    { Default2Obj,    100, 200,             50, 30,        FontNormal, FontMagx12, (char*)DefText[1], WHITE, },  // 2
    { Default3Obj,    160, 200,             50, 30,        FontNormal, FontMagx12, (char*)DefText[2], WHITE, },  // 3
};

enum SaveOKObject {
    MsgStr1Obj,
    MsgStr2Obj,
    BtnMemoObj,
    BtnYesObj,
    BtnNoObj,
};

char SaveOKMsg1[30];    //�t�@�C���̗L���Ń��b�Z�[�W�ς��邽�߂����Œ�`���Ȃ�
char SaveOKMsg2[15];
char SaveOKMsg3[] = "- Input Memo [...]";
const char BtnYesText[] = "Yes";
const char BtnNoText[] = "No";
const char *TextOverWrite = "Overwrite ?";
const char *TextOK = "OK ?";


//�ۑ�OK���m�F�p
MainFormRsc SaveOKRsc[] = {
    //ID,               x,   y         xw, yw,   fontcode,   fontMag,    str,               color,
    { MsgStr1Obj,    10+30, 10+70,      0,  0,   FontNormal, FontMagx12, (char*)SaveOKMsg1, WHITE, },   //���b�Z�[�W
    { MsgStr2Obj,    10+30, 30+70,      0,  0,   FontNormal, FontMagx12, (char*)SaveOKMsg2, WHITE, },   //���b�Z�[�W
    
    { BtnMemoObj,    10+30, 55+70,    200, 30,   FontNormal, FontMagx12, (char*)SaveOKMsg3, WHITE, },   //���b�Z�[�W
    //�{�^��    
    { BtnYesObj,     30+30, 85+70,     70, 30,   FontNormal, FontMagx12, (char*)BtnYesText, WHITE, },  // Yes
    { BtnNoObj,     130+30, 85+70,     70, 30,   FontNormal, FontMagx12, (char*)BtnNoText,  WHITE, },  // No
};

/*
 * Yes/No���擾����ėp�t�H�[��
 * ������̒����ɂ��傫����ݒ�
 */
MainFormRsc ConfirmFormRsc[] = {
    //ID,               x,   y         xw, yw,   fontcode,   fontmag,    str,  color,
    { MsgStr1Obj,    10+30, 10+80,      0,  0,   FontNormal, FontMagx12, NULL, WHITE, },   //���b�Z�[�W
    { MsgStr2Obj,    10+30, 30+80,      0,  0,   FontNormal, FontMagx12, NULL, WHITE, },   //�_�~�[

    { BtnMemoObj,    10+30, 55+70,    200, 30,   FontNormal, FontMagx12, NULL, WHITE, },   //�_�~�[
    //�{�^��    
    { BtnYesObj,     30+30, 60+80,     70, 30,   FontNormal, FontMagx12, (char*)BtnYesText, WHITE, },  // Yes
    { BtnNoObj,     130+30, 60+80,     70, 30,   FontNormal, FontMagx12, (char*)BtnNoText,  WHITE, },  // No
};

const char *TextNoDir = "Not exist 'Data' directory";
const char *TextWait = "Wait around 45sec...";
const char ErrorNotWrite[] = "Error! Couldn't write";
const char *ErrorNotRead = "Error! Couldn't read";

#define X1 120
#define X2 170
#define X3 220
#define X4 270
#define Y1  80
#define Y2 120
#define Y3 160
#define Y4 200
#define XW  48
#define YW  38

const char MemoInputTitleStr[] = "Edit Memo (max.79)";
//�{�^���ɕ\�����镶���A���A���͕���
//���p�L���S32��ޒ�28���7�L�[��4�����蓖�čς݁B[]{}�͊����Ȃ�
//Mode: A -> a -> 1 -> �� -> �A -> A�ɖ߂�
//ENGLISH���[�h�̎��́AA -> a -> 1 -> A
const char *KeyStr[] = {
    "@-_/", "ABC", "DEF", "BS", 
    "GHI","JKL", "MNO", "<-", 
    "PQRS", "TUV", "WXYZ", "->", 
    "a1��", "'\":;", ".,?!", "sp", 
};
const char *KeyStrL[] = {
    "=()`", "abc", "def", "BS", 
    "ghi","jkl", "mno", "<-", 
    "pqrs", "tuv", "wxyz", "->", 
    "1��", "+*^|", "#$%~", "sp", 
};
const char *KeyStr1[] = {
    "1", "2", "3", "BS", 
    "4","5", "6", "<-", 
    "7", "8", "9", "->", 
    "���A", "0", "<>&\\", "sp", 
};
const char *KeyStrH[] = {
    "��", "��", "��", "BS", 
    "��","��", "��", "<-", 
    "��", "��", "��", "->", 
    "�AA", "��", "�A�B", "sp", 
};
const char *KeyStrK[] = {
    "�A", "�J", "�T", "BS", 
    "�^","�i", "�n", "<-", 
    "�}", "��", "��", "->", 
    "Aa1", "��", "�A�B", "sp", 
};

const char **KeyStrList[] = {
    KeyStr,
    KeyStrL,
    KeyStr1,
    KeyStrH,
    KeyStrK,
};

const char *Hira[] = {
    "����������",
    "��������������������",   //����
    "��������������������",   //����
    "",
    "�������ĂƂ����Âł�",   //�����@�������͏���������
    "�Ȃɂʂ˂�",   
    "�͂Ђӂւق΂тԂׂڂς҂Ղ؂�",   //�����A����
    "",
    "�܂݂ނ߂�",
    "�������",       //������
    "������",
    "",
    "",
    "�����",
    "�A�B�[",
};

const char *Kata[] = {
    "�A�C�E�G�I",
    "�J�L�N�P�R�K�M�O�Q�S",   //����
    "�T�V�X�Z�\\�U�W�Y�[�]",   //����
    "",
    "�^�`�c�b�e�g�_�a�d�f�h",   //�����@�������͏���������
    "�i�j�k�l�m",   
    "�n�q�t�w�z�o�r�u�x�{�p�s�v�y�|",   //�����A����
    "",
    "�}�~������",
    "������������",       //������
    "����������",
    "",
    "",
    "������",
    "�A�B�[",
};

enum MemoInputObject {
    MemoInTitleObj,
    MemoObj,
    BtnOKObj,
    BtnCancelObj,

    BtnObj,
};

#define ButtonW 50      //�{�^���̉���
#define ButtonH 40      //�{�^���̍���

/*
 * Memo�ɕ��������͂�����t�H�[��
 */
MainFormRsc MemoInputRsc[] = {
    //ID,              x,  y    xw, yw, fontcode,   fontMag,    str,   color,
    { MemoInTitleObj, 10,  3, 300,  20, FontNormal, FontMagx11, (char*)MemoInputTitleStr, WHITE, },   //�^�C�g��
    { MemoObj,        10, 20, 280,  60, FontNormal, FontMagx11, NULL, WHITE, },   //����
    //�{�^��
    { BtnOKObj,       5, Y4,   40,  30, FontNormal, FontMagx11, (char*)BtnNameOK,   WHITE, },  // OK
    { BtnCancelObj,  50, Y4,   60,  30, FontNormal, FontMagx11, (char*)BtnNameCancel,  WHITE, },  // Cancel

    { BtnObj,        X1, Y1,  200, 160, FontNormal, FontMagx11, NULL, WHITE, },   // 4x4�̃{�^���S�̂�����

};



/*
 * 3�s�\���B1�s35�������\��
 * ���݂̃v���O�����́A���n�I�ɂƂ肠����1�s35�������\��
 */
void writeMemo(char *tempMemo) {
    uint8_t jj, kk, ll, cn;
    uint16_t code;
    int16_t yy;

    jj = MemoObj;

    // 35����/1�s�\��
    display_setTextSize(MemoInputRsc[jj].fontmag);
//    display_setTextPitch(0, 0);
//    display_setTextColor(MemoInputRsc[jj].color, BLACK);
    display_setColor(MemoInputRsc[jj].color);
    
    yy = MemoInputRsc[jj].y;
    display_setCursor(MemoInputRsc[jj].x, yy);
    kk = 0;     //������̃|�C���^
    cn = 0;     //1�s�̉�������
    ll = 0;     //adj�p

    while (tempMemo[kk] != '\0') {
        if (tempMemo[kk] < 0x80) {
            if (cn >= MemoScrX) {
                adj[ll++] = kk;  //�������ڂ����̍s�̐擪������
                cn = 0;
                yy += 20;
                display_setCursor(MemoInputRsc[jj].x, yy);
            }
            display_putc(tempMemo[kk++]);  //
            cn++;
        } else {
            if (cn >= MemoScrX -1) {
                display_putc(' ');  //���f������ɍŌ��1�����󔒂�`��
                adj[ll++] = kk;
                cn = 0;
                yy += 20;
                display_setCursor(MemoInputRsc[jj].x, yy);
            }
            code = tempMemo[kk++];
            code = code*256 + tempMemo[kk++];
            display_putK(code);  //
            cn += 2;
        }
    }

    adj[ll] = kk + 1;   //�����ݒ肵�Ă����Ȃ��ƁA�J�[�\���K�؂ɕ`��ł��Ȃ�

    //��ɍŌ��3�����󔒂�`�悵�A�폜���ĕ��������������ł��Ή��ł���悤
    for (kk = 0; kk < 3; kk++) {
        if (cn >= MemoScrX) {
            cn = 0;
            yy += 20;
            display_setCursor(MemoInputRsc[jj].x, yy);
        }
        cn++;
        display_putc(' ');  //
    }
    
}


/*
 * �e���L�[����`��
 * CurrentKeyStr��mode�ɏ]���Đݒ肵�A���������ʉ�
 */
void DrawTenkey(uint8_t mode) {
    uint8_t idx;
    int16_t xx, yy;
    MainFormRsc rsc;

    //�e���L�[���̃{�^���`��
    rsc.fontmag = MemoInputRsc[BtnObj].fontmag;
    rsc.xw = XW;
    rsc.yw = YW;
    
    CurrentKeyStr = (char**)KeyStrList[mode];
    
    idx = 0;
    for (yy = Y1; yy < Y1 + MemoInputRsc[BtnObj].yw; yy = yy + ButtonH) {
        rsc.y = yy;
        for (xx = X1; xx < X1 + MemoInputRsc[BtnObj].xw; xx = xx+ ButtonW) {
            rsc.x = xx;
            rsc.str = CurrentKeyStr[idx++];
            //�{�^���̒����Ƀe�L�X�g�\��
            DrawButton(rsc, MemoInputRsc[BtnObj].color, TextbgColor, MemoInputRsc[BtnObj].color, 5);
        }
    }
    
    //�Ђ炪�ȁA�J�^�J�i�́A�L�[�̕\���p�Ƃ͕ʂɂȂ�̂ŁA�����ŕύX
    if (mode == 3) CurrentKeyStr = (char **)Hira;
    else if (mode == 4) CurrentKeyStr = (char **)Kata;
    
}

/*
 * �e���L�[�^�C�v�̓��̓t�H�[����`��
 * mode(0-4): 0=�啶���A1=�������A2=���l�A3=���ȁA4=�J�i�̓��A�ǂ̃L�[�{�[�h��\�����邩
 */
void DrawInputBox(uint8_t mode) {
    uint8_t jj, len, xl;
    int16_t yy;

    //�g�`��
    display_fillRoundRect(0,  0, 320, 240, 10, BLACK);
    display_drawRoundRect(0,  0, 320, 240, 10, WHITE);

    //�^�C�g���\��
    jj = MemoInTitleObj;
    display_drawChars(MemoInputRsc[jj].x, MemoInputRsc[jj].y, MemoInputRsc[jj].str, MemoInputRsc[jj].color, TextbgColor, MemoInputRsc[jj].fontmag);
    
    //������\��
    jj = MemoObj;
    //�r����`��@�Œ��̕����񕪂̌r���̒����ɂ���
    len = MaxMemo + 1;   //�ő啶�����@2�o�C�g�����ɂ��A�����ꍇ������̂ŁA+2�ɂ���
    for (yy = 38; yy < Y1; yy += 20) {
        if (len > MemoScrX) xl = MemoScrX;
        else xl = len;
        len -= MemoScrX;
        display_drawLine(MemoInputRsc[jj].x, yy, MemoInputRsc[jj].x +xl*8, yy, GREY);
    }

    writeMemo(TempMemo);

    //OK/Cancel�{�^���̕`��
    for (jj = BtnOKObj; jj <= BtnCancelObj; jj++) {
        DrawButton(MemoInputRsc[jj], MemoInputRsc[jj].color, TextbgColor, MemoInputRsc[jj].color, 10);
    }

    DrawTenkey(mode);

}


/*
 * �����n���ꂽ������memo�ɂ����āAcsr�̈ʒu�̕�����1�o�C�g������2�o�C�g�����̂ǂ��炩����Ԃ�
 * �擪���猟�����Ă���
 * 1: 1�o�C�g����
 * 2: 2�o�C�g�����̌㔼
 * 3: 2�o�C�g�����̐擪
 */
uint8_t CheckChar(char *memo, uint8_t csr) {
    uint8_t jj, cn;
    
    jj = 0;
    cn = 0;
    while (jj <= csr) {
        if (memo[jj] >= 0x80) {
            cn = 2;
            jj += 2;
        } else {
            cn = 1;
            jj++;
        }
    }
    //2�o�C�g�����̑O��(�K�؂ȃJ�[�\���ʒu)�Ȃ�3�ɂ���
    if ((cn == 2) && (jj == (csr +2))) cn++;
    return cn;
}


/*
 * csr�Ŏw�肳�ꂽ�����̉��ɉ����J�[�\����`��
 * �O�̃J�[�\���͏���: CsrDraw, CsrByte���g��
 * ���p����������Ȃ��ꏊ�ɑS�p�������Ƃ��́A���̍s�ɁB�Ώۍs�͉��z�I�ɋ󔒂�����
 * ���z�I�ȃJ�[�\���ʒu��CsrDraw�ƂȂ�@�@�Ⴆ�ΑS�����p�Ȃ�ACsr=CsrDraw
 * ���z��ʂ́A35����x3�s
 * 
 * �\�����鎞�̃J�[�\���ʒu���v�Z
 * �J�[�\���ʒu�̕����̒���(���p���S�p��)���ݒ�
 * 
 */
void DrawCursor(uint8_t csr) {
    int16_t xt, yt;
    uint8_t csrdraw;

    //���݂̃J�[�\���̕\���������A�r����\������
    xt = MemoInputRsc[MemoObj].x + (CsrDraw % MemoScrX)*8;
    yt = 38 + (CsrDraw / MemoScrX)*20;
    display_drawRect(xt, yt, 8 * CsrByte, 2, BLACK);
    display_drawRect(xt, yt, 8 * CsrByte, 1, GREY);

    //�J�[�\���̒�����ݒ�
    CsrByte = CheckChar(TempMemo, csr);
    if (CsrByte > 2) {
        //�S�p��ŃJ�[�\���ʒu��2�o�C�g�ڂ�������A1�o�C�g�ڂɕ␳�v�@�@�����{
        CsrByte = 2;     //�J�[�\���̒���
    }

    //�`�悷��J�[�\���ʒu�̕␳�����{�@�@writeMemo()��adj[]���ݒ�ς݂��K�v
    if (csr < adj[0]) csrdraw = csr;
    else if (csr < adj[1]) csrdraw = (csr - adj[0]) + MemoScrX;
    else csrdraw = (csr - adj[1]) + MemoScrX *2;
    
    CsrDraw = csrdraw;
    //�J�[�\����`��
    display_drawRect(MemoInputRsc[MemoObj].x + (csrdraw % MemoScrX)*8, 38+(csrdraw / MemoScrX)*20, 8 * CsrByte,2, WHITE);

}

/*
 * �Ђ炪��input�n���h��
 */
void InputEventHandler() {
    uint8_t evt;
    uint8_t len;
    uint8_t pos, presize;   //�J�[�\��
    int16_t tx, ty;
    uint16_t wch;   //2�o�C�g�R�[�h
    uint8_t key, lenB;
    uint8_t cnt;   //cnt:���񓯂��{�^������������
    uint8_t ll;
    
    static uint8_t preKey = 0xff;
    static uint8_t first = 1;  //����L�[��������1��ڂ��ǂ���
    static uint8_t Aa1 = 0;    //0:A, 1:a, 2:����, 3:����, 4:�J�i
    static uint8_t time = 0;   //���͊m��܂ł̎��Ԃ𐧌�
    static uint8_t lateCsr = 0; //�����m�肵����J�[�\�����ړ�������

    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //�C�x���g�Ȃ����́A���ʂɋ��肳���Ȃ��悤�A10ms��wait
        //0.8�b�o�߂�����A���͊m�肳����
        if (time < 80) {
            time++;
            if (time >= 80) {
                preKey = 0xff;
                if (lateCsr) {
                    Csr += lateCsr;
                    DrawCursor(Csr);
                    lateCsr = 0;
                }
            }
        }
        return;
    }
    
#ifdef DEBUG
    sprintf(str, "e=%03d", evt);
    display_drawChars(250, 0, str, WHITE, TextbgColor, FontMagx11);
#endif

    switch (evt) {
        case EventFormInit:     //Save
            SetKanjiFont(SJIS16);   //Memo�̓��{��Ή�
            strcpy(TempMemo, Memo);     //�L�����Z���Ή��ł���悤�A���f�[�^�͕ێ����āATempMemo��ҏW
            DrawInputBox(Aa1);
            Csr = (uint8_t)strlen(TempMemo);     //�J�[�\���ʒu���Ō����
            //�J�[�\���o�[��`��
            CsrByte = 1;
            CsrDraw = 0;
            DrawCursor(Csr);
            break;

        case EventTouchRepeat:
        case EventTouchDown:
            if (ButtonPush(TouchX, TouchY, &MemoInputRsc[BtnOKObj])) {   //OK�{�^��
                strcpy(Memo, TempMemo);     //Memo�ɕҏW�����������ݒ�
                SetKanjiFont(0);
                ReturnEventHandler();   //SaveEventHandler��
                PushEvent(EventFormUpdate);
            } else if (ButtonPush(TouchX, TouchY, &MemoInputRsc[BtnCancelObj])) {   //Cancel�{�^��
                SetKanjiFont(0);
                ReturnEventHandler();   //SaveEventHandler��
                PushEvent(EventFormUpdate);
            } else if (ButtonPush(TouchX, TouchY, &MemoInputRsc[BtnObj])) {   //�e���L�[�{�^������
                tx = (TouchX - MemoInputRsc[BtnObj].x) / ButtonW;
                ty = (TouchY - MemoInputRsc[BtnObj].y) / ButtonH;
                key = (uint8_t)(tx + ty*4);    //4x4�̃L�[�}�g���b�N�X�̂ǂꂪ�����ꂽ���@0-15
                
                if (key == 12) {    //A/a
#ifdef ENGLISH
                    Aa1 = (Aa1 + 1) % 3;    //���ȃJ�i�͕\�������Ȃ�
#else
                    Aa1 = (Aa1 + 1) % 5;
#endif
                    
                    //�L�[�{�[�h�̕\�L��ύX
                    DrawTenkey(Aa1);    //�`��Ƌ��ɁACurrentKeyStr��ݒ�
                    preKey = 0xff;  //�؂�ւ�����A�O�̏�Ԃ����Z�b�g�v
                } else {
                    len = (uint8_t)strlen(TempMemo);
    
                    if (key == 7) {  //<-  Cursor Left
                        //�J�[�\���ʒu�̑O�̕����o�C�g���O�Ɉړ�
                        if (Csr == 0) return;
                        presize = CheckChar(TempMemo, Csr-1);
                        //�������炩�̖��ŁA�J�[�\����2nd byte�̈ʒu�ɂ����Ă��ȉ��ŏ�����
                        if (Csr > presize) Csr -= presize;
                        else Csr = 0;
                        DrawCursor(Csr);
                        RepeatKey = 1;  //���s�[�g�L����
                    } else if (key == 11) {     //->    Cursor Right
                        presize = CheckChar(TempMemo, Csr);
                        if (presize >= 2) presize--;    //�J�[�\����2�o�C�g�ł��Ή�
                        if (Csr < len) Csr += presize;
                        else return;
                        DrawCursor(Csr);
                        RepeatKey = 1;  //���s�[�g�L����                            
                    } else if (key == 3) {      //BS
                        if (Csr > 0) {
                            //BS��1�����O�̕������폜
                            presize = CheckChar(TempMemo, Csr-1);   //���������̃o�C�g��
                            if (Csr >= presize) Csr -= presize;  //�V�����J�[�\���ʒu
                            else return;
                            pos = Csr;
                            //�V�����J�[�\���ʒu�Ɍ��̈ʒu�ȍ~�̕������R�s�[
                            while (TempMemo[pos + presize] != '\0') {
                                TempMemo[pos] = TempMemo[pos + presize];
                                pos++;
                            }
                            TempMemo[pos] = '\0';    //�I�[��ݒ�
                            //�I���̏��Ɏc���Ă��錳�̕������󔒂ɂ��āA�ĕ`�掞�ɏ�������
                            // 'abcdefg'   Csr=d�̎��A��L������c������
                            // 'abdefg'�@�@�ƂȂ�
                            writeMemo(TempMemo);    //������`��
                        }
                        DrawCursor(Csr);
                    } else {
                        if (key == 15) {    //space
                            wch = ' ';
                            first = 1;
                            if (lateCsr) {
                                //�O�̕����̊m�肪�܂���������A�m�肳����
                                Csr += lateCsr;
                                lateCsr = 0;
                            }
                            time = 77;
                        }
                        else {
                            time = 0;
                            if (key == preKey) {
                                //�����{�^����A�����ĉ�������
                                first = 0;
                                cnt++;  //�L�[�Ɋ��蓖�Ă��Ă��鉽��ڂ�
                                cnt = cnt % lenB;
                            } else {
                                first = 1;
                                cnt = 0;    //�ŏ��̕���
                                if (lateCsr) {
                                    //�O�̕����̊m�肪�܂���������A�m�肳����
                                    Csr += lateCsr;
                                    lateCsr = 0;
                                }
                                if (Aa1 <= 2) lenB = (uint8_t)strlen(CurrentKeyStr[key]);
                                else lenB = (uint8_t)strlen(CurrentKeyStr[key]) /2;    //���ȁA�J�^
                                
                                if (lenB == 1) preKey = 0xff;
                                else preKey = key;
                            }

                            //wch�ɑΏۂ̕�����ݒ�
                            if (Aa1 <= 2) wch = CurrentKeyStr[key][cnt];
                            else wch = ((uint16_t)CurrentKeyStr[key][cnt*2] << 8) + CurrentKeyStr[key][cnt*2+1]; //2�o�C�g����
                        }
                        
                        if (first == 0) {
                            //2��ڈȍ~�͑}���ł͂Ȃ��O�̕�����u��
                            if (wch < 0x80) TempMemo[Csr] = (char)wch;
                            else {
                                TempMemo[Csr] = (char)(wch >> 8);
                                TempMemo[Csr+1] = (char)(wch & 0xff);
                            }
                            // �\�������������邽�߁A1���������X�V
                            display_setCursor(MemoInputRsc[MemoObj].x + (CsrDraw % MemoScrX)*8, MemoInputRsc[MemoObj].y + (CsrDraw / MemoScrX)*20);
                            display_setTextSize(MemoInputRsc[MemoObj].fontmag);
                            display_setTextPitch(0, 0);
                            //                                display_setTextColor(MemoInputRsc[MemoObj].color, BLACK);
                            display_setColor(MemoInputRsc[MemoObj].color);
                            
                            if (wch < 0x80) display_putc((char)wch);  //
                            else display_putK(wch);  //
                            
                        } else {
                            //�}�����镶�������o�C�g��
                            if (wch < 0x80) ll = 1;
                            else ll = 2;
                            pos = len;  //\0�̈ʒu������
                            if (pos + ll >= MaxMemo) return;   //���������������Ȃ珈���I��
                            //�����}���@�J�[�\���ȍ~�Ō�܂ł̕������T�C�Y�����炷
                            while (pos >= Csr) {
                                TempMemo[pos+ll] = TempMemo[pos];
                                if (pos > 0) pos--;
                                else break;
                            }
                            //�����ݒ�
                            if (wch < 0x80) TempMemo[Csr] = (char)wch;
                            else {
                                TempMemo[Csr] = (char)(wch >> 8);
                                TempMemo[Csr+1] = (char)(wch & 0xff);
                            }
                            lateCsr = ll;
                            writeMemo(TempMemo);
                            DrawCursor(Csr);
                        }
                    }
                }
            }
            break;
    }
}


/*
 * �t�@�C������Memo�f�[�^���擾
 */
void LoadMemo(char *filename, char *memo) {
    FRESULT res;
    char str[150];
    char *p;
    uint8_t pos;
    FIL file;

    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) return;   //������open�ł��Ȃ�������I��

    //Memo :�̍s��ǂݏo��
    memo[0] = '\0';     //���e���N���A����
    while (f_gets(str, sizeof str, &file) != NULL) {     //\n�܂œǂݏo��
        p = strtok(str, ":");
        if (strncmp(p, "Memo", 4) == 0) {
            strcpy(memo, str+6);
            pos = (uint8_t)strlen(memo)-1;
            if (memo[pos] == '\n') memo[pos--] = '\0';
            if (memo[pos] == '\r') memo[pos] = '\0';
            break;
        }
    }
    f_close(&file);
}


/* 
 * �J�X�^���ݒ�̃f�[�^�����邩�m�F���āA�����1�A�Ȃ����0��Ԃ�
 * �f�[�^���鎞�́A���̓��t����date�ɐݒ�
 */
int8_t DataExistCheck(uint8_t num, char *date) {
    char filename[50];
    FRESULT res;
    FILINFO finfo;  //fdate: bit15:9: year based on 1980, bit8:5=month, bit4:0=day
                    //ftime: bit15:11=hour, bit10:5=min, bit4:0=sec/2(2�b�P��)

    sprintf(filename, "%s/Data%d.dat", DataFolder, num);
    res = f_stat(filename, &finfo); //�t�@�C���̏����擾
    if (res != FR_OK) {
        return 0;
    } else {
        sprintf(date, "%04u/%02u/%02u %02u:%02u:%02u", (finfo.fdate >> 9) + 1980, (finfo.fdate >> 5) & 0x0f, finfo.fdate & 0x1f, 
                finfo.ftime >> 11, (finfo.ftime >> 5) & 0x3f, (finfo.ftime & 0x1f)*2);
        //Memo����ǂݏo��
        LoadMemo(filename, Memo);
        return 1;
    }    
}


/*
 * �ۑ��p�t�H�[���A�Ǐo���p�t�H�[�������ʉ����āA�����ŕ`��
 * �Ⴂ�̓^�C�g������
 * �\���A�t�@�C���ԍ��́A1�n�܂�
 */
void DrawCustomDataSaveLoadForm() {
    uint8_t jj, kk;
    char str[100], date[30];
    
    lcd_fill(TextbgColor);
    
    CurrentForm[DataTitleObj].str = (char*)DataTextTitle[SaveLoad];
    //�^�C�g���A���ږ��\��
    for (jj = DataTitleObj; jj <= DeleteMsgObj; jj++) {
        display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    }
    
    remount();  //
    SetKanjiFont(SJIS16);   //Memo�̓��{��Ή�
    for (jj = 0; jj < MAXDataNum; jj++) {
        //�t�@�C�����邩�m�F���āA����΁A���t�A���ԁAMemo���擾
        if (DataExistCheck(jj+1, date)) {
            sprintf(str, "%d:%s %s", jj+1, date, Memo); //�\���ł��镶���͌����邪�A�����͑S�������
        } else {
            sprintf(str, "%d: No data", jj+1);
        }        
        kk = jj + Data1Obj;

        display_drawChars(CurrentForm[kk].x, CurrentForm[kk].y, str, CurrentForm[kk].color, TextbgColor, CurrentForm[kk].fontmag);
    }
    SetKanjiFont(0);    //�����t�@�C�������
            
    //�{�^���`��  Close
    jj = BtnCloseObj; 
    DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 10);
    //Load�̎������A�f�t�H���g�ݒ��ǂݏo���{�^����`��
    if (SaveLoad == 0) {
        for (jj = Default1Obj; jj <= Default3Obj; jj++) {
            DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 5);
        }
    }

}


/*
 * �J�X�^���ݒ�̃f�[�^��SD�J�[�h����ǂݏo��
 */
FRESULT LoadCustomData(char *filename) {
    FRESULT res;
    uint8_t kk, len;
    char str[150];
    char *p;
    FIL file;

    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) return res;   //������open�ł��Ȃ�������I��

    //1�s�ڂ̃e�L�X�g�f�[�^��ǂݏo�����A�f�[�^�͕s�v
    f_gets(str, sizeof str, &file);     //\n�܂œǂݏo��
    
    //���s���ɂ�����ɂ́A�ŏ��̍��ږ������āA�i�[�ԍ����m�肷��K�v����
    //      "item : disp,  x,  y, xw, yw,font#,fsize,fratio, color,format,  attr, attr2,xpitchAdj"
    while (f_gets(str, sizeof str, &file) != NULL) {     //\n�܂œǂݏo��
        //�擪����:�܂ł����ږ�
        p = strtok(str, ":");
        kk = 0;
        while (strncmp(CustomSettingText[kk], p, 4) && kk < OBJECTNUM) kk++;
        //�ǂݍ��񂾃f�[�^���ǂ̍��ڂ��A�T�[�`����kk�ɐݒ�
        //��v�������̂����������ꍇ�A�Ȃ��ꍇ�̓X�L�b�v
        if (kk < OBJECTNUM) {
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].disp = (uint8_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].x = (int16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].y = (int16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].xw = (int16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].yw = (int16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].fontcode = (uint8_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].fontmag = (uint8_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].color = (uint16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].format = (uint16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].attribute = (uint16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].attribute2 = (uint16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].xpitchAdj = (int8_t)strtol(p, NULL, 0);
            if (kk < NumberOfUserFont) {
                //Gear�̓t�H���g�w��Ȃ�
                p = strtok(NULL, ",");
                if (p != NULL) {
                    while (*p == ' ') p++;    //�o�̓t�H�[�}�b�g���A�J���}�̌�ɃX�y�[�X������̂ŁA�����r��
                    strcpy(CustomFontDir[kk], p);
//                    //������Ō�̋󔒁A���䕶�����폜
//                    len = strlen(CustomFontDir[kk]);
//                    if (len > 0) {
//                        while (CustomFontDir[kk][len-1] < ' ') len--;
//                        CustomFontDir[kk][len] = '\0';
//                    }
                } else {
                    CustomFontDir[kk][0] = '\0';
                }
            }
        }
    }

    f_close(&file);
    return res;
}


/*
 * �J�X�^���ݒ�̃f�[�^��SD�J�[�h�ɏ����o��
 * �ȉ��̃v���O�����œ���͖��Ȃ�
 * �@�������Af_write()�𕡐�����s�������A�܂Ƃ��ɓ��삳�����Ȃ��������@�v����
 */
FRESULT SaveCustomData(char *filename) {
    FRESULT res;
    uint8_t jj;
    char *fontDir;
    FIL file;

    res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) return res;   //������open�ł��Ȃ�������I��
    //�ȉ��ŁA�S���̃I�u�W�F�N�g�����������߂�
//    res = f_write(&file, TempObjData, sizeof TempObjData, &actualLength);

    //�e�L�X�g�ŏ����o���A�l��ŏC�����ł���悤�ɂ���BPIC�ł͂��ɂ����B�ǉ��R�����g���������݂ł���悤�ɂ���
    //�ŏ��̍s�́A���ږ�
    f_printf(&file, "item : disp,  x,  y, xw, yw,font#,fsize, color,format,  attr,  attr2,xpitchAdj, Font\r\n");

    //�����ŃJ���}��؂�Ŋe���ڂ��ƂɃf�[�^���o  �e�L�X�g�t�@�C���̕����ǂ��Ƃ̔��f
    for (jj = 0; jj < OBJECTNUM; jj++) {
        if (jj < NumberOfUserFont) fontDir = CustomFontDir[jj];
        else fontDir = NULL;
        f_printf(&file, "%-5s: %4d,%3d,%3d,%3d,%3d,  %2d,  0x%02x, 0x%04x, %4d, 0x%04x, 0x%04x,  %2d, %s,\r\n", 
                CustomSettingText[jj],
                TempObjData[jj].disp, TempObjData[jj].x, TempObjData[jj].y, TempObjData[jj].xw, TempObjData[jj].yw, 
                TempObjData[jj].fontcode, TempObjData[jj].fontmag, TempObjData[jj].color, 
                TempObjData[jj].format, TempObjData[jj].attribute, TempObjData[jj].attribute2, TempObjData[jj].xpitchAdj,
                fontDir);
    }
    f_printf(&file, "Memo :%s\r\n", Memo);
    
    f_close(&file);
    return res;
}

/*
 * SD�J�[�h�ɐݒ���ۑ��p��Data�t�H���_���Ȃ���΍쐬
 */
FRESULT MakeDataDirecotory() {
    char str[100], dir[20];
    FRESULT res;
    
    strcpy(dir, DataFolder);
    res = f_stat(dir, NULL);   //Data�f�B���N�g�������邩�H
    if (res == FR_NO_FILE) {
        //�f�B���N�g���Ȃ���΍쐬
        display_drawChars(10, 180, (char *)TextNoDir, WHITE, TextbgColor, FontMagx12);
        //�Ȃ���΁A�f�B���N�g�������
        res = f_mkdir(dir);
        remount();  //����remount�͕K�v�A���̌�A���L���s���邪�Adummy.dat�͂��Ԃ�time out�ō쐬�ł��Ȃ�
        display_drawChars(10, 195, (char *)TextWait, WHITE, TextbgColor, FontMagx12);
        sprintf(str, "%s/dummy.dat", dir);
        SaveCustomData(str);   //���̏�����time out�܂Ŗ�45�b������
        //�ł��A�����ł̓_�~�[�͍쐬����Ȃ��B���������d�l�𗘗p���Ă���
    }
    return res;
    
}

/*
 * �t�@�C�����ɂ��鎞�AFileExist=1
 * �@1: �㏑��OK���|�b�v�A�b�v�E�C���h�E���o��
 * �@�@�㏑���K�v�Ȃ������A�ŏI�m�F�p�Ɏg��
 *   0: �ǂ��ɏ������ނ����b�Z�[�W���o��
 * 
 */
void DrawWriteConfirmation(uint8_t num, uint8_t FileExist) {
    uint8_t jj;
    
    //�g�`��
    display_fillRoundRect(30, 70, 230, 130, 10, BLACK);
    display_drawRoundRect(30, 70, 230, 130, 10, WHITE);
    
    //�����t�@�C���̗L���Ń��b�Z�[�W�ς���
    if (FileExist) {
        sprintf(SaveOKMsg1, "File #%d already exists.", num);
        strcpy(SaveOKMsg2, TextOverWrite);
    } else {
        sprintf(SaveOKMsg1, "Write to File #%d.", num);
        strcpy(SaveOKMsg2, TextOK);
    }
    //���b�Z�[�W�\��
    for (jj = MsgStr1Obj; jj <= BtnMemoObj; jj++) {
        display_drawChars(SaveOKRsc[jj].x, SaveOKRsc[jj].y, SaveOKRsc[jj].str, SaveOKRsc[jj].color, TextbgColor, SaveOKRsc[jj].fontmag);
    }
    
    //Yes/No�{�^���`��
    for (jj = BtnYesObj; jj <= BtnNoObj; jj++) {
        DrawButton(SaveOKRsc[jj], SaveOKRsc[jj].color, TextbgColor, SaveOKRsc[jj].color, 10);
    }

}


/*
 * �����I�����b�Z�[�W���o���āA�K�v�Ȃ�J�X�^�}�C�Y��ʂɖ߂�
 */
void closeProcedure(char *str, uint8_t backToCustomizing) {
    //���b�Z�[�W��1�b�����\��������
    display_fillRoundRect(10, 100, 250, 50, 10, BLACK);
    display_drawRoundRect(10, 100, 250, 50, 10, WHITE);
    display_drawChars(40, 115, str, WHITE, TextbgColor, FontMagx12);
    for (uint8_t kk = 0; kk < 2; kk++) __delay_ms(500);
    
    //�J�X�^�}�C�Y��ʂɖ߂�
    ClearBuffer();
    if (backToCustomizing) {
        SetEventHandler(CustomizingEventHandler);   //�J�X�^�}�C�Y��ʂɖ߂�
        PushEvent(EventFormInit);
    } else {
        PushEvent(EventFormUpdate);
    }
}


/*
 * Save/Load�@�m�F�p�|�b�v�A�b�v�t�H�[���̃C�x���g�n���h��
 */
void SaveEventHandler() {
    char str[100];
    uint8_t fileExist;
    uint8_t evt;
    
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
        case EventFormInit:     //Save
            //�ݒ����ۑ�����Data�f�B���N�g�������邩�ǂ����`�F�b�N
            MakeDataDirecotory();
            //���Ƀt�@�C�����邩�m�F
            if (f_stat(Filename, NULL) == FR_OK) {
                fileExist = 1;
                LoadMemo(Filename, Memo);   //�t�@�C�����鎞�́A������ǂݏo��
            } else {
                fileExist = 0;
                Memo[0] = '\0';     //�t�@�C���Ȃ����́A�����͂Ȃ�
            }
            //�ȉ�Update�Ƌ��p
        case EventFormUpdate:
            // �t�@�C���ɏ������ނ��ŏI�m�F�p�̃��b�Z�[�W�`��
            // �����t�@�C���L���Ɋւ�炸�A������ݒ肷�邱�Ƃ��ł���悤�Ƀ{�^���݂���
            DrawWriteConfirmation(TargetDataNo, fileExist);
            break;
            
        case EventFormClose:

            break;

        case EventTouchDown:
            if (ButtonPush(TouchX, TouchY, &SaveOKRsc[BtnYesObj])) {   //Yes�{�^��
                //�w�肳�ꂽ�t�@�C���ɕۑ�
                if (SaveCustomData(Filename) == FR_OK) {
                    sprintf(str, "Write Data to #%d", TargetDataNo);    //line938�ɂ����l�̂��̂���
                    closeProcedure(str, 1);
                } else {
                    //�������݂Ɏ��s������
                    strcpy(str, ErrorNotWrite);
                    closeProcedure(str, 0);
                }
            } else if (ButtonPush(TouchX, TouchY, &SaveOKRsc[BtnNoObj])) {   //No�{�^��
                //No�̎��@�������ݒ��~
                ClearBuffer();
                //
                SetEventHandler(CustomDataEventHandler);
                PushEvent(EventFormUpdate);
            } else if (ButtonPush(TouchX, TouchY, &SaveOKRsc[BtnMemoObj])) {   //Memo...�{�^��
                //�������������߂�悤�Ƀe���L�[�X�^�C���̓��̓t�H�[���Ɉڍs
                PopupEventHandler(InputEventHandler);
                PushEvent(EventFormInit);
            }
            break;
    }
}


int8_t CheckDataNum() {

    //�ǂ̃f�[�^��I���������`�F�b�N����
    for (uint8_t jj=0; jj<MAXDataNum; jj++) {
        if (ButtonPush(TouchX, TouchY, &CurrentForm[jj + Data1Obj])) {
            //�ǂ��I�񂾂��Ajj+1���I�������ԍ�(1-8)
            TargetDataNo = jj+1;
            sprintf(Filename, "%s/Data%d.dat", DataFolder, TargetDataNo);   //�t�@�C������1�n�܂�
            return 0;
        }
    }
    return 1;   //error
}


/*
 * �ۑ��A�Ǐo���̃��X�g��Ń^�b�`�������̏���
 * 
 */
void TouchProcedureCustomData(uint8_t saveload) {
    int8_t jj, kk;
    char str[100];
    
#ifdef DEBUG
    sprintf(str, "TXY=%03d, %03d", TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, TextbgColor, FontMagx11);
#endif
    
    if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnCloseObj])) {   //close�{�^��
        ClearBuffer();
        SetEventHandler(CustomizingEventHandler);   //�J�X�^�}�C�Y��ʂɖ߂�
        PushEvent(EventFormInit);
    } else {
        //�f�t�H���g�ݒ��ǂݏo��
        for (jj = 0; jj < 3; jj++) {
            if (ButtonPush(TouchX, TouchY, &CurrentForm[Default1Obj + jj])) {   // 1-3�{�^��
                //
                for (kk = 0; kk < OBJECTNUM; kk++) {
                    TempObjData[kk] = RscData[jj][kk];
                }
                sprintf(str, "Load Data from default %d", jj+1);
                closeProcedure(str, 1);    //�I�������@���b�Z�[�W�\����A�J�X�^���ݒ��ʂ֖߂�
                return;
            }
        }
        
        //�t�@�C������ǂݏo���ꍇ
        if (CheckDataNum()) return; //�I�������t�@�C���ATargetDataNo��ݒ�
        
        if (saveload) {
            //�ۑ��̎��́A�|�b�v�A�b�v�o���āA�㏑���m�F�Ƃ����{
            SetEventHandler(SaveEventHandler);
            PushEvent(EventFormInit);
        } else {
            //�w�肳�ꂽ�t�@�C����Load
            if (LoadCustomData(Filename) == FR_OK) {
                sprintf(str, "Load Data from #%d", TargetDataNo);
                closeProcedure(str, 1);    //�I�������@���b�Z�[�W�\����A�J�X�^���ݒ��ʂ֖߂�
            } else {
                //�Ǐo���Ɏ��s������
                strcpy(str, ErrorNotRead);
                //�G���[�̎��́A�J�X�^���ݒ��ʂɂ͖߂��Ȃ������g�����肪�ǂ�
                closeProcedure(str, 0);    //�I�������@���b�Z�[�W�\����A�t�@�C�����X�g�ɖ߂�
            }
        }
    }
}



/* 
 * �|�b�v�A�b�v�E�C���h�E�Ƀ��b�Z�[�W��\�����āAYes�Ȃ�0�ANo�Ȃ��1��Ԃ�
 */

/*
 * Yes or No
 * Yes�Ȃ�A�Ώۂ̃t�@�C�����폜����
 */
uint8_t YesNoCheck() {
   
    if (ButtonPush(TouchX, TouchY, &ConfirmFormRsc[BtnYesObj])) {   //Yes�{�^��
        f_unlink(Filename); //�t�@�C���폜
    } else if (ButtonPush(TouchX, TouchY, &ConfirmFormRsc[BtnNoObj])) {   //No�{�^��
        //No�̎�
        
    } else {
        return 1;   //Popup�ێ��̏ꍇ
    }

    DrawCustomDataSaveLoadForm();   //�t�H�[����������
    ClearBuffer();
    return 0;   //Popup�I���̏ꍇ

}

/*
 * �^�b�`�������ŁA�f�[�^�̍폜
 * �t�@�C���������āA�|�b�v�A�b�v��`�悵����A0��Ԃ�
 */
uint8_t DeleteData() {
    int8_t jj;
    char str[100];
    
    if (CheckDataNum()) return 1;   //error
    
    //�t�@�C��������΁A�폜���邩�m�F
    if (f_stat(Filename, NULL) == FR_OK) {
        
        //�g�`��@�@���b�Z�[�W�̒����ɉ����ăT�C�Y�Z�o�ł���Ƃ������B
        display_fillRoundRect(30, 80, 230, 110, 10, BLACK);
        display_drawRoundRect(30, 80, 230, 110, 10, WHITE);
        
        sprintf(str, "Delete #%d data OK?", TargetDataNo);
        //���b�Z�[�W�\��
        jj = MsgStr1Obj;
        display_drawChars(ConfirmFormRsc[jj].x, ConfirmFormRsc[jj].y, str, ConfirmFormRsc[jj].color, TextbgColor, ConfirmFormRsc[jj].fontmag);
        
        //�{�^���`��
        for (jj = BtnYesObj; jj <= BtnNoObj; jj++) {
            DrawButton(ConfirmFormRsc[jj], ConfirmFormRsc[jj].color, TextbgColor, ConfirmFormRsc[jj].color, 10);
        }
        return 0;
    }
    return 1;
}


/*
 * �J�X�^���f�[�^��ۑ��������́A�ǂݏo�����̃C�x���g�n���h��
 * Load/Save Setting Data�Ƃ�������
 */
void CustomDataEventHandler() {
    uint8_t evt;
    char str[100];
    static uint8_t push = 0;
    static uint8_t popup = 0;
    
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
        case EventFormInitSave:
            //�ۑ��p�t�H�[��:�^�C�g����saveLoad�ŋ��
            SaveLoad = 1;
            CurrentForm = SaveLoadRsc;
            DrawCustomDataSaveLoadForm();
            break;
        case EventFormInitLoad:
            //���[�h�p�t�H�[��:�^�C�g����saveLoad�ŋ��
            SaveLoad = 0;
            CurrentForm = SaveLoadRsc;
        case EventFormUpdate:
            DrawCustomDataSaveLoadForm();
            break;
        case EventTouchDown:
            if (popup) popup = YesNoCheck();
            else {
                push = 1;
            }
            break;
        case EventTouchUp:
            //�������Ή��̂��߁Aup�ŏ����J�n
            if (push == 1) {
                push = 0;
                TouchProcedureCustomData(SaveLoad);
            }
            break;
        case EventTouchLong:
            //popup���́A�������͖���
            if ((push == 1) && (DeleteData() == 0)) {
                popup = 1;
                push = 0;
            }
            break;
    }
}
