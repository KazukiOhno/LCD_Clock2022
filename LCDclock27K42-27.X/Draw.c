/*
 * File:   Draw.c
 * Author: K.Ohno
 *
 * �`��֌W���[�`��
 */

#include "Draw.h"
#include <string.h>
#include "main.h"
#include "RTC8025.h"
#include "LCD320x240color.h"
#include "Customizing.h"
#include "font.h"

const char WeekDays[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};


//�����\���́A�ω��̂������������ɂ���悤�ɂ��遨�A�i���O���v�݂̂Ŏg�p
uint8_t preTime[3]; //������ێ����Ă����A�ω����`�F�b�N�ł���悤�ɂ���

int16_t CalendarXsize, CalendarYsize;
uint8_t CalendarEJ; //0=J, 1=English

int16_t CustomXsize; //�J�X�^�}�C�Y�̎��A�̈���w�肷�邽�߂Ɏg���@�J�����_�[�݂̂Ɏg�p
int16_t CustomYsize; //�f�W�^���̎����\���ɂ��g�p

char Smaller[2] = "";  //�����\���ŁAbmp�t�H���g�w�莞�A�������t�H���g���w��: "s"�A����ȊO�̎��A""

// ��Βl���擾����֐��}�N����`
#define ABS(x) ((x) < 0 ? -(x) : (x))


/*
 * �t�H���g�t�@�C�����邩��0.bmp�̗L���Ŕ��f
 * �t�@�C������΁A�w�b�_����ǂݎ��A�O���[�o���ϐ���SDcardBuffer�Ɋi�[
 *  0.bmp�f�[�^�����邱�Ƃ��O��B�Ȃ����́A�J�X�^���t�H���g�Ȃ��Ɣ��f���AFontNormal�ŕ`��
 */
uint8_t checkFontFile(char *fontFolder) {
    char filename[30];
    FRESULT res;
    UINT actualLength;
    uint8_t xr, yr;
    FIL file;       //Opening��f�[�^�������ݗp
    
    //0.bmp������O��ŁA���̑傫�����擾����
    sprintf(filename, "/%s/0.bmp", fontFolder);
    if (f_open(&file, filename, FA_READ) != FR_OK) {
        return 1;        //�G���[�̎�
    }
    //�t�@�C�����鎞�́A�w�b�_���ǂݎ��
    res = f_read(&file, SDcardBuffer, 32, &actualLength); //32�o�C�g���ǂݍ���
    f_close(&file);

    CurrentFont.xsize = SDcardBuffer[bcWidth];     //1�o�C�g�Ƒz��
    CurrentFont.ysize = SDcardBuffer[bcHeight];    //1�o�C�g�Ƒz��
    CurrentFont.xpitch = 0;     //X�����̓v���|�[�V���i���Ȃ̂ł��̓s�x
    CurrentFont.ypitch = CurrentFont.ysize;
    
    //�m�[�}���t�H���g�����{�ɂ�����A�w�肳�ꂽ�t�H���g�Ɠ����傫���ɂȂ邩���v�Z
    xr = (CurrentFont.xsize + NormalFontData[Fxsize]/3) / NormalFontData[Fxsize];
    if (xr < 1) xr = 1;
    yr = CurrentFont.ysize / NormalFontData[Fxsize];
    if (yr < 1) yr = 1;
    CurrentFont.mag = (uint8_t)((xr<<4) + yr);

    return 0;   //�t�@�C�����������A����I��
}


/*
 * FontCode�Œ�`����Ă���t�H���g�R�[�h���w��
 * �J�X�^���t�H���g�̏ꍇ�́A�������炳��Ƀt�H���g�t�@�C�������ǂ�K�v������
 * 
 * �����SetKanjiFont�́A���p����B
 * ����������\�����s�����́A�����t�H���g�Ƃ̔{�����v�Z����s����ASetKanjiFont���Ō�Ɏ��{
 * �J�X�^���t�H���g�g�p���́A�������g��Ȃ����Ƃ�O��
 * �@�@�J�X�^���t�H���g�́A�ꕔ����������`����Ă��Ȃ����Ƃ�O��ɁAFontNormal�ŕ⊮������
 */
uint8_t SetFont(uint8_t fontcode) {
   
    if (fontcode >= InternalFontNum) {
        //���̑��̃J�X�^���t�H���g�̏ꍇ�@7�Z�O�܂�
        CurrentFont.data = NULL;    //�����t�H���g�łȂ����Ƃ�����
        CurrentFont.fontcode = MAXFontCode;  //��U�A��������Ԃɂ���
        //�t�H���g�t�@�C���̃`�F�b�N�y�сAsize, pitch, mag���擾
        if (checkFontFile(FontFolder[fontcode])) {
            return 1;  //�t�H���g�t�@�C�����Ȃ����Ȃ�
        }
        
    } else {
        //�����t�H���g�̏ꍇ
        if (fontcode == FontNormal) {
            CurrentFont.data = NormalFontData;      //8x8
        } else if (fontcode == FontSmall) {
            CurrentFont.data = SmallFontData;       //6x8
        } else if (fontcode == Font7like) {
            CurrentFont.data = Font7segData;       //6x8
        } else {
            return 0; //�����ɗ��邱�Ƃ͌���Ȃ����A��L�ȊO�̏ꍇ�́A�����������^�[��
        }
        CurrentFont.xsize = CurrentFont.data[Fxsize];
        CurrentFont.ysize = CurrentFont.data[Fysize];
        CurrentFont.xpitch = CurrentFont.data[Fxpitch];
        CurrentFont.ypitch = CurrentFont.data[Fypitch];
        CurrentFont.mag = 1;    //�{���́A�J�X�^���t�H���g�ݒ莞�ɕύX�����        
    }
    CurrentFont.fontcode = fontcode;
    return 0;
}


/*
 * �t�H���g�����߂��Ƃ��ɁA�w�b�_��񂩂�A��{�����擾
 * �I�t�Z�b�g�f�[�^���擾���Ă���
 * �t�@�C���̓I�[�v���̂܂܂ɂ��� FileFont����ăA�N�Z�X�\
 * 
 * fontname: 0:�����t�@�C��close������
 *    Misaki 1    //8x8�h�b�g�t�H���g
 *    SJIS16 2    //16x16�h�b�g�t�H���g
 *    SJIS24 3    //24x24�h�b�g�t�H���g
 * 
 * CurrentKanjiFont.fontcode�ŁA�����̗L�������𔻒f�Ɏg��
 */
uint8_t KanjiOffsetTable[256];  //�t�@�C�����̊����f�[�^�̂��肩�������e�[�u��

void SetKanjiFont(uint8_t fontcode) {
    char header[64];
    uint16_t headerSize;
    UINT actualLength;
    uint8_t xr, yr;

    //SD�J�[�h�Ȃ���΁A�������Ȃ�
    if (SDcardMount == 0) return;
    //���ɓ����t�H���g�̃t�@�C���I�[�v�����Ă����牽�����Ȃ�
    if (CurrentKanjiFont.fontcode == fontcode) return;
    
    if (CurrentKanjiFont.fontcode != 0) {
        //���Ƀt�@�C�����J���Ă�����A��U����
        f_close(&FileFont);
        CurrentKanjiFont.fontcode = 0;  //��U�A�����𖳌�����Ԃɂ���
        CurrentKanjiFont.mag = FontMagx11;
        if (fontcode == 0) {
            // fontcode��0�̎��́A�t�@�C�����ďI��
            return;
        }
    }

    // �t�@�C�����J���A�f�[�^��ǂݏo��
    //�t�@�C������8�����܂ŃT�|�[�g
    if (f_open(&FileFont, KanjiFontFile[fontcode], FA_READ ) == FR_OK) {
        //Open���A�w�b�_�Ǎ�
        f_read(&FileFont, header, 64, &actualLength); //64�o�C�g���ǂݍ���
        headerSize = header[2] + (uint16_t)header[3]*256; //�w�b�_�̃T�C�Y=64
        CurrentKanjiFont.xsize = header[0x10]; //�t�H���g��X�����̑傫���B�h�b�g��
        CurrentKanjiFont.ysize = header[0x11]; //�t�H���g��Y�����̑傫���B�h�b�g��
        CurrentKanjiFont.xpitch = CurrentKanjiFont.xsize;   //�s�b�`�́A�t�H���g�T�C�Y�Ɠ����ɂ��Ă���
        CurrentKanjiFont.ypitch = CurrentKanjiFont.ysize;
        //��������Ƀm�[�}���t�H���g�̔{���ݒ� .mag�͂��̃t�H���g�̔{���ł͂Ȃ����Ƃɒ���
        //8x8�̎�x11�A16x16�̎�x12�A24x24�̎�x23�Ƃ����������ǂ�����
        xr = (CurrentKanjiFont.xsize/2 + CurrentFont.xsize/2) / CurrentFont.xsize;
        if (xr < 1) xr = 1;
        yr = CurrentKanjiFont.ysize / CurrentFont.ysize;
        if (yr < 1) yr = 1;
        CurrentKanjiFont.mag = (uint8_t)((xr<<4) + yr);
        
        CurrentKanjiFont.data = (uint8_t *)KanjiFontFile[fontcode];
        CurrentKanjiFont.fontcode = fontcode;
        f_read(&FileFont, KanjiOffsetTable, 256, &actualLength); //256�o�C�g���̃e�[�u���ǂݍ���
        //���̃f�[�^�����Ƃɕ\�������̃f�[�^�̈ʒu�����΂₭�v�Z�����邽�߁A�ێ����Ă���
    }
}


/*
 * �J�X�^���t�H���g(bmp�`���̃f�[�^)�̕`��
 * �Ή����镶���́A�\���ɕK�v�Ȃ��̂Ɍ��肵�āA�쐬�J�͂�ߖ񂵂Ă悢�B
 * bmp�t�@�C���Ȃ����́AFontNormal�ŕ\��
 * ���t: 0-9 / () �j��
 * ����: 0-9 :    ������
 * ���x: 0-9 . ��
 * ���x: 0-9 %
 * �J�����_�[: 0-9 �j��
 * 
 * �������w��: Smaller���g��
 * 
 * �G���[�̎��A1
 * 
 */
uint8_t displayCustom_putc(uint8_t c) {
    uint8_t k, x, xf, yf, mm, len;
    uint8_t ii, jj;
    uint16_t pos;
    int16_t y1, y2; //��ʂ��͂ݏo���Ă��Ă��֌W�Ȃ����W
    int16_t xd1, xd2, yd1, yd2; //�\���̈�ɍ��킹�؂�l�߂����W�@�͂ݏo���Ă��Ȃ����͕ω��Ȃ�
    uint8_t fontXsize;
    uint8_t fontYsize;
    char filename[50];
    uint16_t datasize, offset, remainingLine, linenum;
    uint8_t idx;
    UINT actualLength;
    uint8_t line;
    FRESULT res;
    uint8_t prev_size;
    uint8_t prevfontcode, prevmag;
    int16_t xp, yp; //���W
    int16_t px, py; //�{������荞��pitch
    uint8_t rgb[3];

    const char *charname[] = {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
        "colon", "dot", "percent", 
        "degC", "degF", "slash", "(", ")", "space", 
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", 
        "-", 
    };

    // �t�H���g�f�[�^�̃t�@�C�����J���āA�f�[�^��ǂݏo��
    // bmp�t�@�C���́A���̃v���|�[�V���i���t�H���g
    // bmp�t�@�C����ǂ�ŁAX, Y�T�C�Y�擾��A�f�[�^���擾���Ȃ���`��
    if (c >= '0' && c <= '9') idx = c - '0';
    else if (c == ':') idx = 10;
    else if (c == '.') idx = 11;
    else if (c == '%') idx = 12;
    else if (c == FdegC) idx = 13;
    else if (c == FdegF) idx = 14;
    else if (c == '/') idx = 15;
    else if (c == '(') idx = 16;
    else if (c == ')') idx = 17;
    else if (c == ' ') idx = 18;
    else if (c >= FSunday && c <= FSunday+6) idx = 19 + (c-FSunday);
    else if (c == '-') idx = 26;
    else {
        DRAWNORMAL:
        //�Ώۂ̃t�H���g��`�Ȃ����́ANormalFont�ŕ`��
        prevfontcode = CurrentFont.fontcode;
        prevmag = CurrentFont.mag;
        prev_size = (uint8_t)(TextMagX << 4) + TextMagY;
        SetFont(FontNormal);

        //�m�[�}���t�H���g���g��Ȃ��Ƃ����Ȃ����A�J�X�^���t�H���g�Ƃł��邾�����������낦��
        display_setTextSize((uint8_t)(((prevmag >> 4) * TextMagX)<<4) + (prevmag & 0x0f)*TextMagY);
        
        display_putc(c);
        //�t�H���g�����ɖ߂�
        SetFont(prevfontcode);
        display_setTextSize(prev_size);
        return 0;
    }
    
    //�t�H���_���Ńt�H���g��ނ��w�肷��
    sprintf(filename, "/%s/%s%s.bmp", FontFolder[CurrentFont.fontcode], charname[idx], Smaller);
    res = f_open(&FileFont, filename, FA_READ);
    //3�̃G���[�̃P�[�X���z�肳���B�Ώۂ̕������Ȃ��A���������t�H���gdir���Ȃ��A���̑��̃G���[
    if (res == FR_NO_FILE) {
        //�t�H���_�͂����Ă��Ώۂ̕����̃t�@�C�����Ȃ����́A�m�[�}���t�H���g���g��
            goto DRAWNORMAL;    
//            return 0;
    } else if (res != FR_OK) {
        //Open�ł��Ȃ��P�[�X���������B�ǂ��������ɔ�������̂��s���B
        //Open�ł����ɏ��������Ă��āA�\���������̂ŁA������ǉ�
        //�G���[�͂����Ŕ������Ă�������m�F
        remount();      //�ă}�E���g
        if (SDcardMount == 1) {
            if (f_open(&FileFont, filename, FA_READ) != FR_OK) return 10;
        }
        else return 6;  //�}�E���g�ł��Ȃ������ꍇ
    }
    
    res = f_read(&FileFont, SDcardBuffer, 32, &actualLength); //header=32�o�C�g���ǂݍ���
    if (res != FR_OK) {
        f_close(&FileFont);
        return 0; //�G���[�ɂ������^�[��
    }
    
    fontXsize = SDcardBuffer[bcWidth]; //�t�H���g�̑傫���́A�c���Ƃ�255�܂łɐ���
    fontYsize = SDcardBuffer[bcHeight];
    
    //�V�[�N���āA�f�[�^�̐擪�܂ňړ�
    f_lseek(&FileFont, SDcardBuffer[bfOffBits]);

    if (res != FR_OK) {
        f_close(&FileFont);
        return 3;
    }

    len = ((fontXsize+31)/32)*4;        //1�h�b�g�s���̃o�C�g���A4�o�C�g�P��
    //��FfontXsize=40�h�b�g�̎��A�f�[�^��5�o�C�g�A3�o�C�gpadding���āA8�o�C�g/�s
    //    �@�@      41�h�b�g�̎��A�f�[�^��6�o�C�g�A2�o�C�gpadding
    
    linenum = (sizeof SDcardBuffer) / len;   //�o�b�t�@�T�C�Y�ɓ���͉̂��h�b�g�s����
    //�o�b�t�@��640�o�C�g����A5120�h�b�g��������̂ŁAlinenum==0�ɂȂ�Ȃ��O��
    if (linenum >= fontYsize) {
        //�o�b�t�@�ɓ��肫��ꍇ�́A�h�b�g�s��=Y�h�b�g�T�C�Y
        linenum = fontYsize;
    }
    datasize = len * linenum;   //��x�ɓǂݍ��ރf�[�^�T�C�Y(�o�C�g��)
    
    //bmp�̃f�[�^�́A���������ł���ŁA�\��������͉̂�����Ƃ����̂ɒ���
    //��ʂ��͂ݏo���Ă�����ɕ`��ł���悤�ɏ����ǉ�
    //��ʓ��ɓ���悤�ɒ����B�`�悷�鎞�ɁA�͂ݏo���Ă��Ȃ����`�F�b�N�v
//    if (Cursor_x < 0) xd1 = 0;
//    else xd1 = Cursor_x;

//    xd2 = Cursor_x + xsize -1;
//    if (xd2 >= LCD_WIDTH) xd2 = LCD_WIDTH -1;
//
//    //Y�����́A�����`�悷�鎞�́A�����͈͂��ړ�������
//    y1 = Cursor_y;
//    y2 = Cursor_y + pitchY;

    //������`�悳�������B�o�b�t�@�ɓ��肫��Ȃ�bmp���Ή�
    remainingLine = 0;   //�ǂݍ��ގc��s��
    for (jj = 0; jj < fontYsize; jj++) {
        yp = Cursor_y + (fontYsize - jj - 1) * TextMagY;
        if (remainingLine == 0) {
            //�ǂݍ��񂾃f�[�^���g���؂�����A���̃f�[�^��ǂݍ���
            res = f_read(&FileFont, SDcardBuffer, datasize, &actualLength);    //�f�[�^��ǂݍ���
            if (res != FR_OK) {
                f_close(&FileFont);
                return 44;
            }
            remainingLine = actualLength / len;    //�ǂݍ��񂾃f�[�^�̍s��
            pos = 0;
        }
        //addset���āA����x,y���W�w�肵�Ȃ����������`��
        addset(Cursor_x, yp, Cursor_x + fontXsize * TextMagX -1, yp + TextMagY);
        for (ii = 0; ii < fontXsize; ii++) {
            if ((ii % 8) == 0) line = SDcardBuffer[pos++]; //8�h�b�g���Ƀf�[�^����荞��
            xp = Cursor_x + ii * TextMagX;
            if (line & 0x80) {
                // 1�̕`�� �w�i
                if (TextbgColor != Textcolor) {
                    // Back�J���[��front�J���[�ƈႤ���́A���̐F��w�i�Ƃ��ēh��
                    if (TextMagX * TextMagY == 1)
//                        lcd_draw_pixel_at(xp, yp, TextbgColor);
                        draw_pixel(TextbgColor);
                    else
                        display_fillRect(xp, yp, TextMagX, TextMagY, TextbgColor);
                    //�w�i�F��h��Ȃ��ꍇ�A�A�h���X���i�܂Ȃ���肪����
                } else {
//                    xp++;
//                    lcd_set_cursor_x(xp);
//                    read_pixel2nd(rgb, 1);
                    //�`�悵�Ȃ��A�h���X���΂��čĐݒ�
                    addset(xp+TextMagX, yp, Cursor_x + fontXsize * TextMagX -1, yp + TextMagY);
                }
            } else {
                // 0=���̕`��@�O�i
                if (TextMagX * TextMagY == 1) //�g�債�Ă��Ȃ���
//                    lcd_draw_pixel_at(xp, yp, Textcolor);
                    draw_pixel(Textcolor);
                else
                    display_fillRect(xp, yp, TextMagX, TextMagY, Textcolor);
            }
            line <<= 1;
        }
        remainingLine--;
        pos = pos + 3-((pos-1) % 4); //4�o�C�g�P�ʂȂ̂ŁA�؂�グ��
    }    
    
//    for (yf = 0; yf < fontYsize; yf++) {
//        //��ԉ��̍s�̍��W=��������`��
//        if (remainingLine == 0) {
//            //�ǂݍ��񂾃f�[�^���g���؂�����A���̃f�[�^��ǂݍ���
//            res = f_read(&FileFont, SDcardBuffer, datasize, &actualLength);    //�f�[�^��ǂݍ���
//            if (res != FR_OK) {
//                f_close(&FileFont);
//                return 44;
//            }
//            remainingLine = actualLength / len;    //�ǂݍ��񂾃f�[�^�̍s��
//          
//            y1 = y2 - remainingLine*TextMagY;  //�V����y1���W��ݒ�
//            if (y1 < 0) yd1 = 0;
//            else yd1 = y1;
//            if (y2 >= LCD_HEIGHT) yd2 = LCD_HEIGHT -1;
//            else yd2 = y2;
//
//            addset(xd1, yd1, xd2, yd2);    //�`��̈��ݒ�
//            offset = (remainingLine-1) * len;
//        }
//
//        //1�s���̃f�[�^���ǂݍ���ŁA�`�悷��΁A�c���g��̕\�����v���O�������₷��
//        for (mm = 0; mm < TextMagY; mm++) {    //�c�����̔{�����J��Ԃ�
//            k = 0;
//            for (x = 0; x < fontXsize; x++) {
//                if ((x % 8) == 0) line = SDcardBuffer[offset + (k++)]; //8�h�b�g���f�[�^��line��
//                //�������̔{�����̃h�b�g�`����J��Ԃ�
//                for (xf = 0; xf < TextMagX; xf++) {
//                    int16_t xxx = Cursor_x + x * TextMagX + xf;
//                    if ((xxx >= 0) && (xxx < LCD_WIDTH)) {
//                        // Back�J���[��front�J���[�ƈႤ���́A���̐F��w�i�Ƃ��ēh��Ƃ������Ƃ��v����
//                        if (line & 0x80) draw_pixel(TextbgColor);
//                        else draw_pixel(Textcolor);
//                    }
//                }
//                line <<= 1;
//            }
//        }
//        y2 = y1;   //���̕`��
//        remainingLine--;
//        offset -= len;  //1�s���̒������i�߂�(���ۂ͖߂�)
//    }
    f_close(&FileFont);
    
    px = fontXsize * TextMagX + XpitchAdj; //XpitchAdj���t�H���g�̃T�C�Y�ɒǉ�
    Cursor_x += px;
    py = fontYsize * TextMagY + YpitchAdj;
    if (Wrap && (Cursor_x + px) > LCD_WIDTH) {
        Cursor_x = 0;
        Cursor_y += py;
        if (Cursor_y > LCD_HEIGHT) Cursor_y = LCD_HEIGHT;
    }

    return 0;
}


/*
 * �ݒ肳��Ă���J�[�\���ʒu�A�F�A�T�C�Y�����̂܂܎g�����͂�����Ăяo��
 * �r���ŃG���[��������A���~
 */
uint8_t displayCustom_puts(char *s) {
    uint8_t res;
    char str[100];
//    uint16_t preColor = Textcolor;
    
    while (*s) {
        res = displayCustom_putc(*s++);
        if (res) {
//#ifdef DEBUG
            //�����̍X�V������Ȃ���Ԃ̎��A�ǂ��ŃG���[�������Ă��邩�m�F���邽��
//            sprintf(str, "err=%d  ", res);
//            display_drawChars(260, 50, str, WHITE, TextbgColor, FontMagx11);
//#endif
            return 1;  //������ύX   
        }
    }
//    Textcolor = preColor;
    return 0;
    
}


/*
 * �����ŁA�����t�H���g��SD�J�[�h��̃t�H���g���g��������
 */
//uint8_t putsAnyFont(char *str) {
//    if (CurrentFont.data) {
//        display_puts(str);
//    } else {
//        return displayCustom_puts(str);
//    }
//    return 0;
//}

/*
 * �e�L�X�g�\������O����
 */
void presetting(ObjectRsc *rsc) {

    SetFont(rsc->fontcode);   //�t�H���g�̐ݒ�
    display_setTextSize(rsc->fontmag);
    display_setTextPitch(rsc->xpitchAdj, 0);
    display_setCursor(rsc->x, rsc->y);        //�`��J�n���W
//    display_setTextColor(rsc->color, BLACK);
    display_setColor(rsc->color);
    CustomYsize = CurrentFont.ypitch * TextMagY + YpitchAdj;
}


// DateTime�̏�����
void resetPreDateTime() {
    uint8_t jj;
    
    for (jj = 0; jj < 3; jj++) preTime[jj] = 0xff;
}

/*
 * ���\�[�X�f�[�^����ȉ����擾
 * (x,y)���W������̋N�_
 * �����̊O�`���2�h�b�g���傫�߂̗̈��`���Ƒz��
 * format: HHMMss�Ȃ�
 * fontmag: �t�H���g�T�C�Y
 *  �ʏ��7�Z�O���w�肳�ꂽ���AHHMMss��ss�́A������7�Z�O�t�H���g���g�p
 * �@�@�@�@�@�f�t�H���g�ŏ�����7�Z�O�w�肳�ꂽ���́A�����傫��
 * �T�|�[�g����t�H���g�́A7seg�ACustom0��Custom1��3��
 * 
 */
void dispTime(uint8_t *datetime, ObjectRsc *rsc) {
    char ss[10];
//    int16_t tempYsize;

    //�x�[�X�ƂȂ�t�H���g�̃f�B���N�g�����w��
    //Segment7Num��Segment7sNum��z�肵�Ă���
    presetting(rsc);

    //�����������K�v�Ȍ������`�悵�Ă������A�`������������āA����`��ł����Ȃ��Ȃ���
    if (rsc->format == HHMM) {
        sprintf(ss, "%02x:%02x", datetime[2], datetime[1]);
        display_puts(ss);
    }
    else if (rsc->format == HHMMSS) {
        sprintf(ss, "%02x:%02x:%02x", datetime[2], datetime[1], datetime[0]);
        display_puts(ss);
    }
    else if (rsc->format == HHMMss) {
        sprintf(ss, "%02x:%02x", datetime[2], datetime[1]);
        //���̃��[�`���ŁACursor�����I�ɐi��
        display_puts(ss);
        
        //�t�H���g�T�C�Y��������
        if (rsc->fontcode >= InternalFontNum) {
            // User�t�H���g�ŁAsmalll�t�H���g��`�Ȃ����̏��u������
            strcpy(Smaller, "s");
        } else {
            //Y��������1�T�C�Y����������
//            display_setTextSize(rsc->fontmag - 0x01); //MagY��0�ɂȂ��Ă��A�G���[�ɂȂ�Ȃ��悤�ɂȂ��Ă���
            uint8_t xs = (rsc-> fontmag) >> 4;
            uint8_t ys = (rsc-> fontmag) & 0x0f;
            ys = ys / 2 + 1;
            xs = xs / 2 + 1;
            display_setTextSize((uint8_t)(xs << 4) + ys); //MagY��0�ɂȂ��Ă��A�G���[�ɂȂ�Ȃ��悤�ɂȂ��Ă���
            
        }
        sprintf(ss, "%02x", datetime[0]);
        display_puts(ss);
        Smaller[0] = '\0';
    }
    display_setTextPitch(0, 0); //presetting�ƃy�A�Ō��ɖ߂�
}


// ��=0~179�x�܂ŁAsin�Ƃ�256�{�ɂ���sin�e�[�u��
//�@�A���[���j��5���P�ʂɂ����ꍇ�A2.5�x�P��(30�x��12����)���~�����B1�x�P�ʂőË�
//sin(��): ��=�p�x
//180�x�ȍ~�́A-sin(��-180)�Ƃ����`�ŎQ��
//cos(��)=sin(��+90)�Ōv�Z
//1�x�P�ʂ̃e�[�u��
const int16_t sin_table[] = {
    //0    1    2    3    4    5    6    7    8    9
      0,   4,   8,  13,  17,  22,  26,  31,  35,  40,   //00-
     44,  48,  53,  57,  61,  66,  70,  74,  79,  83,   //10-
     87,  91,  95, 100, 104, 108, 112, 116, 120, 124,   //20-
    128, 131, 135, 139, 143, 146, 150, 154, 157, 161,   //30-
    164, 167, 171, 174, 177, 181, 184, 187, 190, 193,   //40-
    196, 198, 201, 204, 207, 209, 212, 214, 217, 219,   //50-
    221, 223, 226, 228, 230, 232, 233, 235, 237, 238,   //60-
    240, 242, 243, 244, 246, 247, 248, 249, 250, 251,   //70-
    252, 252, 253, 254, 254, 255, 255, 255, 255, 255,   //80-
    256, 255, 255, 255, 255, 255, 254, 254, 253, 252,   //90-
    252, 251, 250, 249, 248, 247, 246, 244, 243, 242,   //100-
    240, 238, 237, 235, 233, 232, 230, 228, 226, 223,   //110-
    221, 219, 217, 214, 212, 209, 207, 204, 201, 198,   //120-
    196, 193, 190, 187, 184, 181, 177, 174, 171, 167,   //130-
    164, 161, 157, 154, 150, 146, 143, 139, 135, 131,   //140-
    128, 124, 120, 116, 112, 108, 104, 100,  95,  91,   //150-
     87,  83,  79,  74,  70,  66,  61,  57,  53,  48,   //160-
     44,  40,  35,  31,  26,  22,  17,  13,   8,   4,   //170-
};

//degree�Ŏw�肳�ꂽ�p�x=theta�ɑΉ�����sin��Ԃ�(x256)
int16_t sind(int16_t theta) {
    theta = (theta + 360) % 360;    //theta���}�C�i�X�ł��Ή������邽��

    if (theta >= 180) {
        //-sind(��-180)
        return -sin_table[(UINT)(theta-180)];          //�x�P�ʂ̃e�[�u���p
    } else {
        return sin_table[(UINT)theta];
    }
}

//degree�Ŏw�肳�ꂽ�p�x=theta�ɑΉ�����cos��Ԃ�(x256)
int16_t cosd(int16_t theta) {
    theta = theta+90;
    return sind(theta);
}

/*
 * �A�i���O���v�\��
 * mode: 
 * datetime: ���t����
 * xx, yy: ���S���W
 * size: �O�`�~�̑傫��
 * color: �j�̐F�w��
 */
#define SizeMin  3      //���j�̑���
#define SizeHour 4      //���j�̑���
//#define ColorHour   GREY
//#define ColorMin    GREY
#define ColorSec    RED
#define ColorAlarm  YELLOW

/*
 * �A�i���O���v�̃A���[���j��`��/����
 */
void drawAlarmNeedle(int16_t *x, int16_t *y, uint16_t color) {
    int16_t xd, yd;
    
    //�A���[���j�`��
    display_drawLine(x[0], y[0], x[1], y[1], color);
    
    xd = (int16_t)(x[1]-x[0]);
    yd = (int16_t)(y[1]-y[0]);
    if ( ABS(yd) > ABS(xd)) {
        //�p�x�ɂ��A�ړ����������ς��A������������
        display_drawLine(x[0]+1, y[0], x[1]+1, y[1], color);
        display_drawLine(x[0]-1, y[0], x[1]-1, y[1], color);
    } else {
        display_drawLine(x[0], y[0]-1, x[1], y[1]-1, color);
        display_drawLine(x[0], y[0]+1, x[1], y[1]+1, color);
    }
}

//���j�A���j�̕`��/����
void drawNeedle(int16_t *x, int16_t *y, uint16_t color, uint16_t color2) {
    display_fillTriangle(x[1], y[1], x[2], y[2], x[3], y[3], color);
    display_drawTriangle(x[1], y[1], x[2], y[2], x[3], y[3], color2);
    //���Α��̏o������
    display_fillTriangle(x[4], y[4], x[2], y[2], x[3], y[3], color);
    display_drawTriangle(x[4], y[4], x[2], y[2], x[3], y[3], color2);

}

//�b�j�̕`��/����
void drawSecNeedle(int16_t *x, int16_t *y, uint16_t color) {
    display_drawLine(x[1], y[1], x[2], y[2], color);
    display_fillTriangle(x[0], y[0], x[3], y[3], x[4], y[4], color);
   
}

        
//�A���[���j�̒����͒��j��75%
//���j�F���j�̒����́A�ڐ��̐����3�h�b�g����
//�Z�j
//�b�j
//�ڐ��̒���=3
//�h�b�g�̑傫��=2
void drawAnalogClock(uint8_t *datetime, ObjectRsc *rsc, uint8_t *alarmtime) {
    int16_t rc = rsc->xw /2;   //�O�`�̔��a
    int16_t xc = rsc->x + rc;    //���v�̒��S���W
    int16_t yc = rsc->y + rc;
    uint16_t color = rsc->color;
    uint16_t bcolor;

    int16_t rc1, rcs, rcm, rch, rca;    //���v�̖ڐ��A�b�j�A���j�A���j�A�A���[��
    int16_t rc2;
    int16_t rcs2, rcm2, rcm3, rch2, rch3, rca2;    //���v�̃T�C�Y�A�ڐ��A�b�j�A���j�A���j�A�A���[��
    int16_t x[5], y[5];    //���W�v�Z�������ʂ��i�[
    int16_t angle;
    uint8_t hh, mm, ss, jj, kk;
    static uint8_t almhh, almmm;
    //�O��̍��W
    static int16_t phx[5], phy[5], pmx[5], pmy[5], psx[5], psy[5], pax[2], pay[2];
    //����̍��W
    int16_t chx[5], chy[5], cmx[5], cmy[5], csx[5], csy[5], cax[2], cay[2];
    char str[3];
    int8_t minupdate = 0;
    
    //�O�`�~�̕`��
    display_drawCircle(xc, yc, rc, color);  //size�Ŏw�肳�ꂽ�~
    rc = rc -3;
    display_drawCircle(xc, yc, rc, color);  //���菬���߂̉~

    rc1 = rc -3;        // ��Ԓ������j�̒���=�ڐ��������ɐݒ�
    rca = rc1 *3 /4;    //�A���[���p�̐j�̒����́A���j��75%
    rch = rc1 *7/10;    //�Z�j�̒����́A70%
    rcm = rc1 -5;       //���j�̒����́A�ڐ��̐����3�h�b�g����
    rcs = rc1 -4;       //�b�j�̒����́A����
    rc2 = rc1 -10;      //�����Ղ̐����̈ʒu

    //�A���[���j�̕`�揀��----------------------------------------------------
    //�ŐV�̃A���[���j�̍��W���v�Z
    if (alarmtime != NULL) {
        almmm = Bcd2Hex(alarmtime[0]);
        almhh = Bcd2Hex(alarmtime[1]);
    }
    almhh = almhh % 12u;
    angle = almhh * 30 + almmm/2;   //�p�x�ɕϊ�
    cax[0] = xc;    //���S���W
    cay[0] = yc;
    cax[1] = xc + rca * sind(angle)/256;    //�A���[���j�̐�̍��W
    cay[1] = yc - rca * cosd(angle)/256;
    
    //��������Ȃ������߁A�\���ɕω�����j�������������A����ȊO�͏㏑������
    //�O�̕\��������
    if ((cax[1] == pax[1]) && (cay[1] == pay[1])) {
        //�O�ƍ��W���ς���Ă��Ȃ����́A�����Ȃ��B�ŏ��̕`����������W�ɂ��Ă���̂ŏ������Ȃ�
    } else {
        drawAlarmNeedle(pax, pay, TextbgColor);   //���̕b�j������
        //�`��p�̍��W��ۑ�
        for (jj=0; jj<2; jj++) {
            pax[jj] = cax[jj];
            pay[jj] = cay[jj];
        }
    }

    //���j�̕`�揀��--------------------------------------------------------
    //���j�̈ʒu���ς������A�ĕ`��̑O�ɁA���̎��j�A���j������
    mm = Bcd2Hex(datetime[1]);
    if (preTime[1] != datetime[1]) {
        preTime[1] = datetime[1];
//        mm = Bcd2Hex(datetime[1]);
        hh = Bcd2Hex(datetime[2]);

        //���j�̍��W���v�Z
        angle = mm*6;
        cmx[1] = xc + rcm * sind(angle)/256;    //�j��̍��W
        cmy[1] = yc - rcm * cosd(angle)/256;
    
        rcm2 = SizeMin;       //���j�̕�
        angle = angle +90;  //�j�ɑ΂���90�x�̊p�x
        cmx[2] = xc + rcm2 * sind(angle)/256;
        cmy[2] = yc - rcm2 * cosd(angle)/256;
        cmx[3] = xc - (cmx[2]-xc);
        cmy[3] = yc - (cmy[2]-yc);

        //���Α��̏o������
        rcm3 = 10;      //�o�����蒷��
        angle = angle +90;  //�����90�x����
        cmx[4] = xc + rcm3 * sind(angle)/256;
        cmy[4] = yc - rcm3 * cosd(angle)/256;

        //���j�̍��W�v�Z�F���j���ړ�������A���j���ړ��v�Z (���ۂ�2����)
        //�@���̃f�[�^����荞��Ŏ��j�̊p�x���߂�
        hh = hh % 12;
        angle = hh * 30 + mm/2;   //�p�x�ɕϊ� 8bit�ϐ��ł�NG
        
//        chx[0] = xc;  //���S���W�@�@�@���g�p�Ȃ̂�
//        chy[0] = yc;
        chx[1] = xc + rch * sind(angle)/256;    //�j��̍��W
        chy[1] = yc - rch * cosd(angle)/256;
        
        rch2 = SizeHour;    //���j�̕�
        angle = angle +90;  //�j�ɑ΂���90�x�̊p�x
        chx[2] = xc + rch2 * sind(angle)/256;
        chy[2] = yc - rch2 * cosd(angle)/256;
        chx[3] = xc - (chx[2]-xc);
        chy[3] = yc - (chy[2]-yc);
        
        //���Α��̏o������
        rch3 = 10;      //�o�����蒷��
        angle = angle +90;  //�����90�x����
        chx[4] = xc + rch3 * sind(angle)/256;
        chy[4] = yc - rch3 * cosd(angle)/256;
        
        //�����ύX�ɂȂ莞�Ԃ��ς�鎞�́A���̕ύX�Ɠ����Ɏ��j�̏���
        drawNeedle(phx, phy, TextbgColor, TextbgColor);
        //���j�̏���
        drawNeedle(pmx, pmy, TextbgColor, TextbgColor);
        minupdate = 1;  //���jupdate����

        //�`����W��ۑ�
        for (jj=1; jj<5; jj++) {
            phx[jj] = chx[jj];
            phy[jj] = chy[jj];
            pmx[jj] = cmx[jj];
            pmy[jj] = cmy[jj];
        }
    }

    //�ŐV�̕b�j�̍��W���v�Z
    if (preTime[0] != datetime[0]) {
        preTime[0] = datetime[0];
        ss = Bcd2Hex(datetime[0]);  //0-59�̐��l
        angle = ss*6;
        csx[0] = xc;  //���S���W
        csy[0] = yc;
        csx[1] = xc + rcs * sind(angle)/256;
        csy[1] = yc - rcs * cosd(angle)/256;
        
        rcs2 = 20;   //���Α��ɏo�������
        angle = angle +180;
        csx[2] = xc + rcs2 * sind(angle)/256;
        csy[2] = yc - rcs2 * cosd(angle)/256;
        csx[3] = xc + rcs2 * sind(angle+6)/256;
        csy[3] = yc - rcs2 * cosd(angle+6)/256;
        csx[4] = xc + rcs2 * sind(angle-6)/256;
        csy[4] = yc - rcs2 * cosd(angle-6)/256;
        
        //�b�j������
        drawSecNeedle(psx, psy, TextbgColor);

        //�`����W��ۑ�
        for (jj=0; jj<5; jj++) {
            psx[jj] = csx[jj];
            psy[jj] = csy[jj];
        }
    }
    
    //�ڐ��Ɛ����̕`��------------------------------------------------------------
    SetFont(FontNormal);
    for (jj=0; jj<60; jj++) {
        angle = jj*6;
        x[1] = xc + (rc * sind(angle)+127)/256; //�P����256�Ŋ���Ɛ؎̂ĂɂȂ�̂�+127�Ƃ��Ďl�̌ܓ��I�ɂ��Ă݂�
        y[1] = yc - (rc * cosd(angle)+127)/256;
        x[2] = xc + (rc1 * sind(angle)+127)/256;
        y[2] = yc - (rc1 * cosd(angle)+127)/256;
        
        if (jj%5 == 0) {
            //5�̔{���̏��ɕ����Ղ̐��l��\��
            display_fillCircle(x[2], y[2], 2, color);   //���̏��́A�h�b�g�A���a2
            //�����Օ\��
            if (!((minupdate == 0) && (jj == mm))) {
                //�������Aminupdate=0�ŁAmm��5�̔{���̎��́A�`�悵�Ȃ�(�������Ȃ��ƃ`����)
                //�܂�A���j�������ɂ������Ă��āA�����������Ȃ����́A���l�͕`�悵�Ȃ��Ƃ�������
                x[3] = xc + (rc2 * sind(angle))/256 -4;
                y[3] = yc - (rc2 * cosd(angle))/256 -3;
                //�\�����鐔����ݒ�
                if (jj==0) kk = 12;   //0���̏��́A12�ɐݒ�
                else kk = jj/5;
                if (kk>9) x[3] = x[3] - 3;
                
                sprintf(str, "%d", kk);
                //�w�i�F�𓯂��ɂ���ƁA�w�i�F��h��Ȃ�=�����Ɠ���
                //1�����l��2�����l�ňʒu�𒲐�
                display_drawChars(x[3], y[3], str, color, color, FontMagx11);
            }
        } else {
            display_drawLine(x[1], y[1], x[2], y[2], color);
        }
    }
    
    // AM/PM�̕\��
    //��������Ȃ������߁A�w�i�����͕ω������Ȃ��`����s�����A
    //AM/PM�̐ؑ֎������A�w�i�`����{
    if (Bcd2Hex(preTime[2])/12 != Bcd2Hex(datetime[2])/12) {
        bcolor = TextbgColor;
    }
    else {
        bcolor = color;
    }
    preTime[2] = datetime[2];
    if (datetime[2] < 0x12) strcpy(str, "AM");
    else strcpy(str, "PM");
    //�ߑO�ߌ�̕\����傫���ɉ����ĕς���
    if (rc > 70)
        display_drawChars(xc-14, yc+rc/2, str, color, bcolor, FontMagx22);
    else
        display_drawChars(xc-7, yc+rc/2, str, color, bcolor, FontMagx11);
    
    //��������A�j�̕`������s
    //�A���[���j�`��------------------------------------------------------
    drawAlarmNeedle(pax, pay, ColorAlarm);  //�A���[���j�̐F

    //���j�̕`��------------------------------------------------------
    drawNeedle(phx, phy, color, WHITE);    //���j�̐F�́A����������炤

    //���j�̕`��------------------------------------------------------
    drawNeedle(pmx, pmy, color, WHITE);

    //�b�j�̕`��------------------------------------------------------
    display_fillCircle(xc, yc, 3, ColorSec);    //�b�j�̒����~
    drawSecNeedle(psx, psy, ColorSec);
    
}

/*
 * ���\�[�X�Ŏw�肳�ꂽ�ݒ�Ŏ�����\��
 */
void DrawTime(uint8_t *datetime, ObjectRsc *rsc) {

    if (rsc->disp) {
        //RscID�����̃I�u�W�F�N�g�̗L��(=�\�����邩�ǂ���)�ɗ��p
        if (rsc->format == ANALOGCLOCK) {
            //�O�`�~�̍��W= (140, 130) ���a=90
            //�A�i���O���v�\���̏ꍇ�́A�A���[���j�������ɕ\��
            drawAnalogClock(datetime, rsc, AlarmTime);
        } else {
            dispTime(datetime, rsc);
        }
    }
}


/*
 * ���\�[�X�Ŏw�肳�ꂽ�t�H���g�ɉ����ĕ`��
 * �@�@Small:�@���Ɨj����Normal�t�H���g�g�p����
 * �@�@Normal: �S���m�[�}���ŕ`��
 * ����̍��W: xs, ys
 * �`�悷��N��: year, month
 */
void basicDrawCalendar(uint8_t year, uint8_t month, int16_t xs, int16_t ys, ObjectRsc *rsc) {
    uint8_t jj, kk;
    int16_t xx, yy;
    char str[5];
    int8_t currentDay;    //�ŏ��̓��j�̓��t�A0�ȉ��͑O��
    int8_t maxdays;
    uint8_t yr, mm, dd, wd;
    uint16_t color;         //�����F
//    uint16_t backColor = BGColor; //�w�i�F
    uint8_t thismonth;
    //�j�����Ƃ̐F�w��
    uint16_t dayColor[7];   // = {RED, WHITE, WHITE, WHITE, WHITE, WHITE, BLUE, };
    uint8_t xpitch, ypitch; // �\���s�b�`
    uint8_t mul;

    //�O���A�����̕\���̂��߁A0���A13���Ƃ����ݒ�������̂ŁA�����ŏC��
    if (month == 0) {
        month = 12;
        year--;
    }
    if (month == 13) {
        month = 1;
        year++;
    }

    yr = year;
    mm = month;
    dd = 1;
    wd = getWeekdays(&yr, &mm, &dd);  //1���̗j�����擾
    //�\������ŏ��̓��t�����z�I�ɉ������ݒ�
    //1�������j�Ȃ�1�̂܂܁A���j�Ȃ�ŏ��̓��j����0���A�Ηj�Ȃ�-1�A���j�Ȃ�-2�ɂȂ�
    currentDay = (int8_t)(1- wd);
    
    //�����̃J�����_�[��\�����Ă��邩���`�F�b�N�B�����̓��t�Ɉ�����邩�ǂ������f�����邽��
    if (month == Bcd2Hex(DateTime[5])) thismonth = 1;
    else thismonth = 0;

    //���Y���̍ŏI�����擾�B���̌����������邩��c��
    dd = 31;
    getWeekdays(&yr, &mm, &dd);  //31���̗j�����擾�B���̓����Ȃ��ꍇ�A�������������
    //mm���ύX�ɂȂ�����A31���ł͂Ȃ������Ƃ킩��B
    //dd��31����1���ɕς���Ă�����A���̌���30���������Ƃ킩��B2���A���邤�N���Ή��B
    if (month != mm) {
        maxdays = (int8_t)(31 - dd);    //���̌��̍ŏI��
    } else {
        maxdays = (int8_t) dd;
    }   
    
    //�j�����Ƃ̐F�����\�[�X����擾
    color = rsc->color;
    for (jj = 1; jj < 6; jj++) dayColor[jj] = color;
    dayColor[0] = rsc->attribute2;   //���j���̐F
    dayColor[6] = rsc->attribute;    //�y�j���̐F
    
    //�w�肳�ꂽ��{�t�H���g�����擾
    SetFont(rsc->fontcode);
    display_setTextSize(rsc->fontmag);    //fontsize�̏��4bit=X�{���A����4bit=Y�{��
    xpitch = (uint8_t)(CurrentFont.xsize *TextMagX *2 + rsc->xpitchAdj);  //1��=2����+�X�y�[�X
    ypitch = (uint8_t)(CurrentFont.ypitch * TextMagY);
   
    //�Ώۗ̈���N���A: fontw��14������+7���̃X�y�[�X���AY������8�s��
    //x�̊J�n��1�h�b�g���ɂ��āA�T�C�Y�����̕��傫���B�g��t�������͂ݏo����h�~
    CalendarXsize = xpitch*7 + 10;
    CalendarYsize = ypitch * 8;
    xs++;
    display_fillRect(xs-1, ys, CalendarXsize+1, CalendarYsize+1, TextbgColor);
    
    //���̕`��
    SetFont(FontNormal);
    sprintf(str, "%d", month);
    xx = xs + (int16_t)(xpitch*7 - strlen(str) * CurrentFont.xsize *2)/2;    //�����ɕ\��������
    //���\���͉�����2�{�ŕ`��
    if (rsc->fontcode < InternalFontNum)
        display_drawChars(xx, ys, str, color, TextbgColor, rsc->fontmag +0x10);
    else
        display_drawChars(xx, ys, str, color, TextbgColor, CurrentFont.mag +0x10);

    yy = ys;
    //�j��
    //�j���̕���2�{���Ă��A2���̓��t�̕���苷����΁AX=2�{�ɂ���
    if (CurrentFont.xsize *2 < xpitch) mul = 2;
    else mul = 1;
    yy += ypitch;
    xx = xs+ (xpitch - CurrentFont.xsize * mul)/2;   //�����ɂȂ�悤��
    for (jj=0; jj<7; jj++) {
        if (CalendarEJ) 
            sprintf(str, "%c", WeekDays[jj][0]);  //SMTWTFS�Ƃ����\�L����
        else
            sprintf(str, "%c", FSunday+jj);    //�����ΐ��؋��y�Ƃ����\��
    if (rsc->fontcode < InternalFontNum)
        display_drawChars(xx, yy, str, dayColor[jj], TextbgColor, rsc->fontmag +0x10*(mul-1));
    else
        display_drawChars(xx, yy, str, dayColor[jj], TextbgColor, CurrentFont.mag +0x10);
        xx += xpitch;
    }
    
    //���t
    yy += ypitch;
    SetFont(rsc->fontcode);
    display_setTextPitch(0, 0);

    for (kk = 0; kk < 6; kk++) {
        xx = xs;
        for (jj = 0; jj < 7; jj++) {
            if (currentDay > 0 && currentDay <= maxdays) {
                display_setCursor(xx, yy);
//                display_setTextColor(dayColor[jj], TextbgColor);
                display_setColor(dayColor[jj]);
                
                sprintf(str, "%2d", currentDay);
//                display_drawChars(xx, yy, str, dayColor[jj], TextbgColor, rsc->fontmag);
                display_puts(str);
                
                //�����̓��t�Ɉ������
                if (thismonth && (currentDay == Bcd2Hex(DateTime[4]))) {
                    //���]���Ǝ��F���ɂ����̂Řg��t����
                    display_drawRect(xx-1, yy-2*TextMagY, CurrentFont.xsize *TextMagX *2 +1, ypitch+TextMagY, WHITE);
                }
            }
            currentDay++;
            xx += xpitch;
        }
        yy += ypitch;
    }
}


/*
 * Format�ɉ����ăJ�����_�[��\������
 * ����5��ނ̃t�H�[�}�b�g
 * Month1: ����1�����̃J�����_�[��\���B�t�H���g�T�C�Y�ς��邱�ƂŁA�傫�������������\����
 * Month3: �O��̌����܂߂�3�������̃J�����_�[�����z�u�ŕ\��
 * Month3v:�O��̌����܂߂�3�������̃J�����_�[���c�z�u�ŕ\��
 * �@�����̔N���́A�\�������錎���w��
 * �@BCD�ł͂Ȃ�
 * �\���ʒu�́A���\�[�X�Ŏw��
 * 
 */
void DrawCalendar(ObjectRsc *rsc) {
    uint8_t jj;

    if (rsc->format >= Month1e) CalendarEJ = 1;
    else CalendarEJ = 0;
    if (rsc->disp) {
        if (rsc->format == Month1 + CalendarEJ*(Month1e - Month1)) {
            // 1�����̃J�����_�[��\���@�@�傫���̓t�H���g�T�C�Y�Ŏw��
            basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5]), rsc->x, rsc->y, rsc);
            CustomXsize = CalendarXsize;
            CustomYsize = CalendarYsize;
        } else if (rsc->format == Month3 + CalendarEJ*(Month3e - Month3)) {
            // 3�������̃J�����_�[��\��
            //3�����J�����_��X,Y���W�́A���\�[�X����擾
            //CalendarXsize�́A�ŏ��̌���`�悷��ƌv�Z�����
            for (jj=0; jj<3; jj++) {
                basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5])+jj-1, rsc->x +CalendarXsize*jj, rsc->y, rsc);
            }
            CustomXsize = CalendarXsize *3;
            CustomYsize = CalendarYsize;
        } else if (rsc->format == Month3v + CalendarEJ*(Month3ve - Month3v)) {
            // 3�������̃J�����_�[���c�ɕ\��
            //�c�\���̎��́A2�����������\�����Ȃ����\���������Ă��ǂ��Ƃ���
            //CalendarYsize�́A�ŏ��̌���`�悷��ƌv�Z�����
            for (jj=0; jj<3; jj++) {
                basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5])+jj-1, rsc->x, rsc->y + CalendarYsize*jj, rsc);
            }
            CustomXsize = CalendarXsize;
            CustomYsize = CalendarYsize *3;
        } else if (rsc->format == Month2 + CalendarEJ*(Month2e - Month2)) {
            // 2�������̃J�����_�[��\��
            for (jj=0; jj<2; jj++) {
                basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5])+jj, rsc->x +CalendarXsize*jj, rsc->y, rsc);
            }
            CustomXsize = CalendarXsize *2;
            CustomYsize = CalendarYsize;
        } else if (rsc->format == Month2v + CalendarEJ*(Month2ve - Month2v)) {
            // 2�������̃J�����_�[���c�ɕ\��
            for (jj=0; jj<2; jj++) {
                basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5])+jj, rsc->x, rsc->y + CalendarYsize*jj, rsc);
            }
            CustomXsize = CalendarXsize;
            CustomYsize = CalendarYsize *2;
        }
    }
}

/*
 * �A���[��������\���@�@�X�C�b�`�̏�ԂŐF��ω�������
 * sw SlideSWoff=Alarm off, SlideSWon=Alarm on
 */
void DrawAlarmTime(uint8_t *alarmtime, uint8_t sw, ObjectRsc *rsc) {
    char str[100];
    char ampm[][4] = {"AM ", "PM ", ""};
    uint8_t ap;
    char onOff; //[] = {'-', '+'};    //�I���I�t��+-�ؑց@plus/minus
    uint16_t color;
    uint8_t hr;
    char alarm[5];
    
    if (rsc->format >= iALAMPM) {
        sprintf(alarm, "%c", Fbell);
    } else {
        strcpy(alarm, "ALM");   
    }

    if (rsc->disp) {
        //�A���[�������̕\���F���A�X���C�hSW��On/Off�ŕς���
        if (sw == SlideSWon) {
            //On�̎�
            onOff = '+';
            color = rsc->color;
        } else {
            //Off�̎�
            onOff = '-';
            color = rsc->attribute;  //�A���[���̃I�t���̐F��attribute�Ɋi�[
        }
        
        hr = Bcd2Hex(alarmtime[1]);
        if ((rsc->format == ALMAMPM)||(rsc->format == iALAMPM)) {
            //12���ԕ\��
            if (hr >= 12) {
                ap= 1 ;   //BCD��12���ȍ~�Ȃ�ߌ�  
                hr -= 12;   //12���ԕ\�L�̂��߁A12��������
            }
            else {
                ap = 0;
            }
        } else {
            ap = 2;
        }
        
        sprintf(str, "%s%c %s%02d:%02x", alarm, onOff, ampm[ap], hr, alarmtime[0]);
//        } else {
//            //24���ԕ\��
//            sprintf(str, "ALM%c %02x:%02x", onOff, alarmtime[1], alarmtime[0]);
//        }
        
        presetting(rsc);
        display_setColor(color);    //SW�ɑΉ����ĐF��ύX�����邽��
        display_puts(str);
        display_setTextPitch(0, 0); //presetting�ƃy�A�Ō��ɖ߂�

    }
}

/*
 * �����̓��t��\��
 * �\�����@�Ȃǂ́A���\�[�X���ŁA�w�肷��
 * �@�@format, �t�H���g�Ȃ�
 */
void DrawDate(ObjectRsc *rsc) {
    char str[100];
    uint8_t *datetime = DateTime;
    char sss[50];
    
    if (rsc->disp) {
        switch (rsc->format) {
            case YYYYMMDDw:
                //NormalFont�ł́A�j���̃t�H���g��FSunday=0x17-�ɂ���
                sprintf(str, "20%02x/%02x/%02x(%c)", datetime[6], datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case YYYYMMDDwe:  //�j���̉p��\�L
                sprintf(str, "20%02x/%02x/%02x(%s)", datetime[6], datetime[5], datetime[4], WeekDays[datetime[3]]);
                break;
            case YYYYMMDD:
                sprintf(str, "20%02x/%02x/%02x", datetime[6], datetime[5], datetime[4]);
                break;
            case YYMMDDw:
                sprintf(str, "%02x/%02x/%02x(%c)", datetime[6], datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case YYMMDDwe:
                sprintf(str, "%02x/%02x/%02x(%s)", datetime[6], datetime[5], datetime[4], WeekDays[datetime[3]]);
                break;
            case YYMMDD:
                sprintf(str, "%02x/%02x/%02x", datetime[6], datetime[5], datetime[4]);
                break;
            case MMDDw:
                sprintf(str, "%02x/%02x(%c)", datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case MMDDwe:
                sprintf(str, "%02x/%02x(%s)", datetime[5], datetime[4], WeekDays[datetime[3]]);
                break;
            case MMDD:
                sprintf(str, "%02x/%02x", datetime[5], datetime[4]);
                break;
        };
        presetting(rsc);
        display_puts(str);
        display_setTextPitch(0, 0); //presetting�ƃy�A�Ō��ɖ߂�
    }
}


/*
 * ���x��\��
 */
void DrawTemp(int16_t temp, ObjectRsc *rsc) {
    char str[50];
    int16_t ftemp;

    if (rsc->disp) {
        if (rsc->format == DEGF) {
            //temp�́A10�{�̐ێ��@�@�@10F = 9/5*(10C) + 320
            ftemp = temp * 9*2 +320*10;
            sprintf(str, "%2d%c", ftemp / 100, FdegF);  //
        } else {
            sprintf(str, "%2d.%1d%c", temp / 10, temp % 10, FdegC);  //FdegC=���̃R�[�h
        }
        
        presetting(rsc);
        display_puts(str);
        display_setTextPitch(0, 0); //presetting�ƃy�A�Ō��ɖ߂�
    }
}

/*
 * ���x��\��
 */
void DrawHumidity(int16_t humidity, ObjectRsc *rsc) {
    char str[50];

    if (rsc->disp) {
        presetting(rsc);

        sprintf(str, "%2d%%", humidity / 10);
        display_puts(str);
        display_setTextPitch(0, 0); //presetting�ƃy�A�Ō��ɖ߂�

    }
}

/*
 * ���ԃA�C�R��32x32�h�b�g��(x,y)���W�ɕ`��
 */
void DrawGearIcon(int16_t x, int16_t y, uint16_t color) {
    uint16_t ii, jj, pos;
    int16_t x2, y2;
    uint8_t line;
    
    pos = 0;
    x2 = x + 31; //GearXw-1
    y2 = y + 31;
    //��ʂ��͂ݏo���Ă�����ɕ`��ł���悤�ɏ����ǉ�
    if (x2 >= LCD_WIDTH) x2 = LCD_WIDTH -1;
    if (y2 >= LCD_HEIGHT) y2 = LCD_HEIGHT -1;
    addset(x, y, x2, y2);    //�`��̈��ݒ�

    for (ii = 0; ii <= y2-y; ii++ ) {
        //Y�����́A���[�v�𐧌�
        for (jj = 0; jj < 32; jj++) {
            //8�h�b�g���Ƀf�[�^�ǂݏo��
            if ((jj % 8) == 0) line = GearIcon[pos++];
            if (jj <= x2 - x) {
                //X�����́A�f�[�^��ǂݏo���s����A�ʏ퓮��ŕ`�悵�Ȃ����ƂőΉ�
                if (line & 0x80) draw_pixel(color);
                else draw_pixel(TextbgColor);  //�w�i�F
            }
            line <<= 1;
        }
    }
}


/*
 * �{�^���̒����ɕ�����`�悷��
 * ���W����MainFormRsc��񂩂�擾���邪�A�F�͕ύX�ł���悤�ʈ����ɂ��Ă���
 */
void DrawStrCenter(MainFormRsc rsc, uint16_t color, uint16_t bcolor) {
    int16_t xp, yp;

    //�{�^���̒����Ƀ{�^������\��
    display_setTextSize(rsc.fontmag);
    xp = rsc.x + rsc.xw/2 - (int16_t)strlen(rsc.str)* CurrentFont.xpitch * TextMagX /2;
    yp = rsc.y + rsc.yw/2 - (CurrentFont.ysize-1) * TextMagY /2;
    display_drawChars(xp, yp, rsc.str, color, bcolor, rsc.fontmag);
}

/*
 * �{�^����`��
 * fcolor: �����̐F
 * bcolor: �{�^���̔w�i�F
 * rcolor: �{�^���̘g�̐F���w��
 * �{�^���̊p���ۂ߂�w���
 */
void DrawButton(MainFormRsc rsc, uint16_t fcolor, uint16_t bcolor, uint16_t rcolor, int16_t r) {
    
    display_fillRoundRect(rsc.x, rsc.y, rsc.xw, rsc.yw, r, bcolor);
    display_drawRoundRect(rsc.x, rsc.y, rsc.xw, rsc.yw, r, rcolor);
    //�{�^���̒����Ƀe�L�X�g�\��
    DrawStrCenter(rsc, fcolor, bcolor);

}
