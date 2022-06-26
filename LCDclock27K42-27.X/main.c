/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F27K42
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/*
 * PIC 18F27K42  64MHz, 128KB
 *                   MCLR   RB7 PGD
 *     LCD_CS    (O) RA0    RB6 PGC
 *     LCD_RESET (O) RA1    RB5 (I) G1PPS 
 *     LCD_DCRS  (O) RA2    RB4 (I) RTC_INTB (WPU) IOC
 * AN3 PhotoDiode(I) RA3    RB3 (O) CCP2
 *     T_IRQ     (I) RA4    RB2 (IO) SDA2
 *     T_CS      (O) RA5    RB1 (IO) SCL2
 *                   VSS    RB0 (I) RTC_INTA INT0
 *     ALMSW     (I) RA7    VDD
 *     SDCard_CS (O) RA6    VSS
 *     ROT_B     (I) RC0    RC7 (I) RX1
 *     ROT_A     (I) RC1    RC6 (O)(TX1)
 *     LED   CCP1(I) RC2    RC5 (O) SDO1
 *     SCK1      (O) RC3    RC4 (I) SDI1
 * 
 * �s���A�T�C���́A18F26K22�Ɠ���ɂ��邱�Ƃ��ł��Ă���
 * Peripheral Pin Select (PPS)�̋@�\�ɂ��A�s���A�T�C�������Ȃ莩�R�ɐݒ��
 * 
 * TMR0: �A���[�����̖��Ă��鎞�Ԃ𑪒� 10ms���̊�����
 * TMR1: ���[�^���[�G���R�[�_�̃T���v�����O�@500us���̊�����
 * TMR2: ���d�X�s�[�J�p��4KHz��ECCP2 PWM�𔭐������邽�߂̃^�C�}  250us
 * TMR3: ADC��CCP4���[�h�Ŏg�p���āATMR3=4ms�𗘗p
 * TMR5: �^�b�`�A�X���C�hSW�AGPS�̏�ԃ`�F�b�N�̊��荞�ݗp�^�C�}�@10ms
 * TMR6: �o�b�N���C�g��PWM����p�^�C�} 256us
 * 
 * SPI�C���^�t�F�[�X�FLCD�A�^�b�`�ASD�J�[�h��3�ŋ��p
 * �@SPI1 (SCK1, SDI1, SDO1)+LCD_CS, T_CS, SDCard_CS
 *   �@LCD_RESET, LCD_DCRS�AT_IRQ
 *   LCD�Ƃ͍ő�X�s�[�h��8MHz�A�^�b�`��2MHz�ŒʐM�ASD�J�[�h�́A�ᑬ400kHz�A����8MHz
 * SPIMASTER�́A�J�������f�t�H���g�ŁAMODE3�ɂȂ�̂ŁAMODE0�ɕύX�v
 * �C�����Ă����Ȃ��ƁAGenerate������A�����Ȃ��Ƒ������ƂɂȂ�
 * SD�J�[�h�́AMODE3�ł�MODE0�ł��ǂ��������B�������A18F27K42�ł́A5.3MHz�܂�
 * LCD���A8Mhz���삹���A6.4Mhz�܂Ł@���@SLRCONC�ŃX���[���[�g��max�ɂ��ĉ���
 * �@�@�����Ɠ��l8Mhz����͂ł���悤�ɂȂ���
 * SDSLOW=400kHz
 * SDFAST=8MHz
 * LCD8M=16MHz
 * Touch2M=1.5MHz
 * 
 * �Ɠx�Z���T(PhotoDiode)�́A�A�i���O�œd����荞�� RA3=AN3=CCP4=PhotoDiode
 * 
 * LCD�̃o�b�N���C�g��PWM������g���Ė��邳����
 * �@�@PWM6: �o�b�N���C�g��PWM (Timer6) PWM6=RC2
 * �@�@�Ɠx�Z���T�̌��ʂɊ�Â��APWM6_LoadDutyValue()�Œ���
 * 
 * I2C�C���^�t�F�[�X: RTC�Ɖ��x�Z���T�ŋ��p
 * �@I2C2 (SCL2�ASDA2) 100KHz MFINTOSC
 * �@RTC�́A/INTA, /INTB ���g�p
 *     INTA=RB0=INT0�́ARTC����pullup�A1�b���ƂɊ��荞�ݓ���BEXT_INT�ŏ���
 *     INTB=RB4�́APIC��WPU�A�����芄�荞�݂ŃA���[��������
 * 
 * ���[�^���[�G���R�[�_:RC0/RC1��2�s���̃f�W�^��Input���g���Ĕ��f
 * �@�@RC0=ROT_B, RC1=ROT_A
 * 
 * �A���[��On/Off�pSW�@�@RA7=ALMSW
 * ���d�X�s�[�J: ECCP2=PWM (Timer2)�@CCP2=RB3
 * GPS�Ƃ̃V���A���ʐM: RX1�݂̂Ŕ񓯊��ʐM(UART1)  RX1=RC7
 * �@9600bps, 8bit, parity�Ȃ��AStop1bit
 * GPS��1PPS: SW�Ɠ��l��10ms���Ƀ`�F�b�N���鏈���ŁARB5�����荞��
 * 
 * ����m�F�pLED: RA6=LED
 * �s�����s�������̂ŁARA6��SDCard_CS�Ƌ��p
 * 
 * Revision:
 * Rev.4: 18F26K22�Ɠ����̃v���O�����B�������ASPI�̑��x�����x�� 
 * Rev.5: �@�\�ǉ��B
 * 
 * Rev.10: �v���O���������Ȃ��Ȃ��āA�n�[�h���^�킵�����ɂ����̃v���O�������g���Ɨǂ��B����OK�̂��̂Ƃ���HEX�t�@�C��������
 * Rev.17: �Ƃ肠�����A�J�X�^�}�C�Y�ȊO�͂قڊ��؂��ɂ�������̃v���O����
 * Rev.18: �J�X�^�}�C�Y���̑�C��
 * Rev.20: �J�X�^�}�C�Y���܂߁A�قڊ���
 * Rev.21: SDFAST=4.5MHz�Ƃ��āASDHC�J�[�h���g�p�ł���悤�ɂ����B�R���p�C��C90�ɕύX��10%�قǃo�C�i���k��
 *         User�t�H���g��6�펝�Ă�悤�ɂ����̂ŁA6�̕��i�S���Ɏ��R�ȃt�H���g�������Ƃ��\�ɂ���
 * �@�@�@�@�J�X�^�}�C�Y���A�{�^����������͖̂���`�悵�đΉ��A��ʂ������Ă����͉̂^�p�ōH�v��z��
 * �@�@�@�@�@�����^�b�v���ă|�b�v�A�b�v�����̂����i�����|�b�v�A�b�v�ɕ\���������B
 * �@�@�v���O�����T�C�Y=108146 byte (83%), Data�T�C�Y=7343 byte (90%)
 *     ���������Ƃ����Ă��ǂ����x��
 * Rev.22: �J�X�^�}�C�Y�̑��쐫����B�ݒ�t�@�C����ۑ����鎞�A79�����̃R�����g����͂ł���悤�ɂ����B�Ђ炪�ȁA�J�^�J�i�ɂ��Ή�
 * �@�@�@�O���ŁA�������͂��Ă��\���͑Ή��ł���悤�ɂ��Ă���B
 * Rev.23: ���\�[�X�Ń������𖳑ʂɏ���Ă����̂ŁA�J�X�^�}�C�Y�̉�ʃ��\�[�X��1�s��1�ɂ����BData��81%�܂ň��k
 * �@�@�@�@�t�H���g����fontname.txt�ɐݒ肷��ƁA�|�b�v�A�b�v�ɔ��f�����悤��
 * Rev.24: �t�H���g�t�@�C�����������񎝂Ă�悤�ɁA�I�u�W�F�N�g���Ƀt�H���g���w��ł���ɂ����B
 * Rev.25: �^�b�`��SPI=1.5MHz���ɂ��A�f�[�^����肱�ڂ��Ȃ��悤�ɂȂ���
 * �@�@�@�@SPI�̃X���[���[�g��max�ɂ��A8MHz�ȏ�̓��삪�ł���悤�ɁB
 * �@�@�@�@�����ݎ��A�T�u���[�`���̌Ăяo������߁A�t���O���ŁAstack overflow��reentrant������
 * Rev.26: �\�[�X�t�@�C���̐����B�R�����g�����Ă���Â��v���O�����̍폜�B�R�����g�̍X�V�Ȃ�
 *        �J�X�^�}�C�Y�̃^�b�`������Event���B�ۑ����̃����̓��͂��œK��
 * �@�@�@Prog=125051, Data=6981
 * Rev.27: �J�X�^�}�C�Y�̃t�H���g�{���̐ݒ�������ƍL���邽�߁A�ʉ�ʂŁA�v���r���[�����Ȃ���ݒ�ł���悤�ɁB
 * �@�@�@�@�@�����ɁA�t�H���g�A�F�A�s�b�`�����A�t�H�[�}�b�g�������ʂŐݒ�ł���悤��
 * 
 * 
 * �c����P�̗]�n�Ƃ��ẮA12/24���Ԑ��̐ؑցA�J�����_�[�̉p��Ή����炢��
 * 
 */

#include <string.h>
#include "mcc_generated_files/fatfs/diskio.h"
#include "main.h"
#include "AlarmSound.h"
#include "LCD320x240color.h"
#include "RTC8025.h"
#include "Draw.h"
#include "TouchXT2046.h"
#include "DHT20.h"
#include "Customizing.h"
#include "Setting.h"


//�C�x���g�L���[�̐[��
#define BufferMax 16
//Event��z��ɂ��āA�L���[���������Ă���BPushEvent��PopEvent�ő���
uint8_t Event[BufferMax];  //�C�x���g�������ɂ�����push����
uint8_t PushPoint;
uint8_t PopPoint;
uint8_t ContNum;    //�L���[�ɓ����Ă���x���g��
uint8_t RotaryEventCount = 0;   //�L���[�ɑ�ʂ�Rotary�C�x���g������Ȃ��悤���������邽�߁A�J�E���^�Ɏg��

uint8_t RepeatKey = 0;  //���s�[�g���邩�ǂ���

//�\�����[�h
uint8_t DisplayMode;    // �ǂ̉�ʕ\�����[�h��

//�^�b�`�����p
uint16_t TouchRawX, TouchRawY;    //�^�b�`��������X,Y�̐����W
int16_t TouchX, TouchY;    //�f�B�v���C���W�Ɋ��Z�����^�b�`���W
//���W�␳�p��(20,20)��(300,220)�̃^�b�`��̍��W��ێ�
uint16_t T_x1, T_y1, T_x2, T_y2;    //�␳�p
uint8_t TouchCount;     //����������p�Ɏg�p�B�^�b�`�����0�ɂȂ�
uint16_t PixelData[9];  //3x3�̃s�N�Z���f�[�^��ۑ�����
int16_t PixelXY[2];     //��荞�񂾃f�[�^�̍���̍��W
uint8_t TouchStart;     //�^�b�`�J�n�t���O    bit3:Up, 2:repeat, 1:long, 0:down

//�X���C�hSW
uint8_t SlideSWStatus = 0x0f;   //�����l�͂ǂ̏�Ԃł��Ȃ��l��ݒ肵�Ă���

//GPS����
#define STABLE1PPS  50  // ����擾���������Ƃ݂Ȃ���
uint8_t Count1PPS;  //1PPS���荞�݂̔����񐔂��J�E���g�B�ő�STABLE1PPS�ɂȂ�
//GPS�́A�d���������ƁA1��1�񂾂��̎擾
char Buffer[100];   //GPS�Ƃ̃V���A���ʐM�p�o�b�t�@

//���[�^���[�����p
int8_t RotCount = 0; //�񂵂���(�N���b�N����̃��[�^���[���ƁA����N���b�N��������)
int8_t Accel = 1;   //������]���������ɁA1�N���b�N��Accel�{�ɂ�����
uint8_t RotaryFlag = 0;     //���[�^���[�C�x���g���s�t���O

// ���т́ARTC�ƍ��킹�āA�b�A���A���A�j���A���A���A�N�@�@BCD�t�H�[�}�b�g
uint8_t DateTime[7] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x22};
int8_t TZ = +9; //����^�C���]�[���́AJST�ɌŒ�

//�A���[�������Amm,hh, �A���[�����L���ȗj���@RTC�Ɠ�������
uint8_t AlarmTime[3] = {0, 0, 0x7f};    //mm, hh, wday
uint8_t SmoothAlarmTime[3] = {0, 0, 0x7f};
uint8_t TmpTime[7]; //�ݒ�r���̎�����ێ�
uint8_t RTC1sFlag = 0;  //1�b���Ƃ̊����݃C�x���g���s�t���O
uint8_t GPSFlag = 0;         //GPS�C�x���g���s�t���O

uint8_t AlarmStatus = 0;    //�A���[��������1�ɂ��āAsmooth�����Ȃǂ��s��
uint8_t SmoothCount;    //����X�k�[�Y������

//�����x�Z���T�̒l��ێ��@���ۂ̒l��10�{�ɂ��Đ�����
int16_t Temp, Humidity;

//SD�J�[�h�֌W
FIL FileObject;       //Opening��f�[�^�������ݗp
uint8_t SDcardMount = 0;    //�}�E���g�ł�����1
FATFS Drive;
//SD�J�[�h����f�[�^��ǂݏo�����Ɏg���o�b�t�@
//#define DATASIZE 640
uint8_t SDcardBuffer[DATASIZE];    //���[�J���ł͓��肫��Ȃ��Ă��A�����ł͂܂�������

// mcc_generated_files/fatfs/ff_time.c���ȉ��̂悤�ɏ��������ă^�C���X�^���v�𐳊m��
/*
DWORD get_fattime (void)
{
    // ���т́ARTC�ƍ��킹�āA�b�A���A���A�j���A���A���A�N
    uint8_t dateTime[7] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x21};

    RTC_read(dateTime);
    return decimalToFatTime(2000+Bcd2Hex(dateTime[6]), Bcd2Hex(dateTime[5]), Bcd2Hex(dateTime[4]), Bcd2Hex(dateTime[2]), Bcd2Hex(dateTime[1]), Bcd2Hex(dateTime[0]));
}
 */

//��ʏ�ɔz�u���镔�i�̒�`
ObjectRsc MainRsc1[] = {
    //ID, x, y,         xw, yw,         fontcode,   fontMag,    color, format, attribute, attribute2, xpitchAdj
    { 10, YearX, YearY, YearXw, YearYw, FontNormal, FontMagx22, WHITE, YYYYMMDDw, 0, 0, },
    { 20, TimeX, TimeY, TimeXw, TimeYw, Font7like,  0x77,       LIME,  HHMMss, 0, 0, 0, },    //�Ԋu 2dot�L����
    { 50, AlarmX, AlarmY, 0,    0,      FontNormal, FontMagx22, WHITE, ALM24, GREY, },       //�A���[��off���̐F=GREY
    { 30, TempX, TempY,   0,    0,      FontNormal, FontMagx22, WHITE, DEGC, 0, },
    { 40, HumidityX, HumidityY, 0,0,    FontNormal, FontMagx22, WHITE, 0, 0, },
    { 60, CalendarX, CalendarY, CalendarXstep, CalendarYw, FontSmall, FontMagx11, WHITE, Month3, BLUE, RED, 1, },    //�y���̐F�A�Ԋu1dot�L����
    { 70, GearX, GearY, GearXw, GearYw, 0,          0,          LIGHTGREY, },    //����
};

ObjectRsc MainRsc2[] = {
    { 10, 0,  0, YearXw, YearYw,  FontNormal, FontMagx22,  WHITE, YYYYMMDDw, 0, 0, },
    { 20, 10, 18, TimeXw, TimeYw, FontNormal, FontMagx23,  LIME, HHMMSS, 0, 0, 2, },    //�Ԋu 2dot�L����
    { 50, AlarmX+150, 40,0, 0,    FontNormal, FontMagx12,  WHITE, ALM24, GREY, },         //�A���[��off���̐F=GREY
    { 30, TempX, 0,  0,  0,       FontNormal, FontMagx22,  WHITE, DEGC, 0, },
    { 40, HumidityX, 17, 0, 0,    FontNormal, FontMagx22,  WHITE, 0, 0, },
    { 60, 25, 65, 320, 200,       FontNormal, FontMagx22,  WHITE, Month1, BLUE, RED, 3, },    //�y���̐F�A�Ԋu3dot�L����
    { 70, GearX, 200, GearXw, GearYw, 0,         0,        GREY, },    //����
};

ObjectRsc MainRsc3[] = {
    { 10, 0,  0, YearXw, YearYw, FontNormal, FontMagx22, WHITE, YYYYMMDDw, 0, 0, },
    { 20, AnalogClockX, AnalogClockY, AnalogClockR, AnalogClockR, 0, 0, GREY, ANALOGCLOCK, Large, }, //�傫�Ȏ��v
    { 50, 40, 218,       0,0,   FontNormal,  FontMagx13, WHITE, ALMAMPM, GREY, },         //�A���[��off���̐F=GREY
    { 30, TempX, 0,      0,0,   FontNormal,  FontMagx22, WHITE, DEGC, 0, },
    { 40, HumidityX, 20, 0,0,   FontNormal,  FontMagx21, WHITE, 0, 0, },
    { 60, 320-CalendarXstep, 40, CalendarXstep, CalendarYw, FontSmall, FontMagx11, WHITE, Month3v, BLUE, RED, 1, },
    { 70, GearX-97, 200, GearXw, GearYw, 0,     0,       GREY, },
};

//���[�U���\�����@���J�X�^�}�C�Y�ł������
ObjectRsc CustomRsc[] = {
    { Enable, YearX, YearY,    YearXw, YearYw,    FontNormal, FontMagx22, WHITE,     YYYYMMDDw, 0, 0, },
    { Enable, TimeX+10, TimeY,    TimeXw, TimeYw, Font7seg,   FontMagx11, ROYALBLUE, HHMMss, 90, 0, 2, },
    { Enable, AlarmX, AlarmY,       50,20,        FontNormal, FontMagx22, WHITE,     ALM24, GREY, },  //�A���[����On/Off�����ݒ�H
    { Enable, TempX, TempY,         50,20,        FontNormal, FontMagx22, WHITE,     DEGC, 0, },
    { Enable, HumidityX, HumidityY, 50,20,        FontNormal, FontMagx22, WHITE,     0, 0, },
    { Enable, 5, CalendarY, CalendarXstep, CalendarYw, FontSmall, FontMagx11, WHITE, Month3, BLUE, RED, 2, },
    { 99, GearX, GearY, GearXw, GearYw,           0,          0,          LIGHTGREY, NoData, },    //�F��ύX
};

ObjectRsc *CurrentRsc;    //���ݎQ�Ƃ��ׂ����\�[�X
MainFormRsc *CurrentForm;   //���݂̉�ʃt�H�[��

//DisplayMode�̔ԍ��Ɖ��L�̗v�f�͈�v�����Ă���
//DisplayMode=DisplayMod1�́AMainRsc1�̃��\�[�X
ObjectRsc *RscData[] = {
  MainRsc1,
  MainRsc2,
  MainRsc3,
  CustomRsc,  
};

const char TouchMsg[] = "Touch anywhere to close the screen";

void (*PreviousEventHandler)(void);
void (*EventHandler)(void); //�C�x���g�����̃��[�`����ݒ肵�āA��������s


/*
 * �C�x���g�n���h���̐ݒ�
 * �Ăяo���̂́A���EventHandler
*/
void SetEventHandler(void (* handler)(void)){
    PreviousEventHandler = NULL;
    EventHandler = handler;
}

/*
 * �����点�Ă���C�x���g�n���h����ۑ����āA�V���ȃC�x���g�n���h���𑖂点��
 */
void PopupEventHandler(void (* handler)(void)){
    PreviousEventHandler = EventHandler;    //���݂̃n���h����ۑ�
    EventHandler = handler;
}

/*
 * �ێ����Ă����C�x���g�n���h���ɖ߂�
 */
void ReturnEventHandler(){
    if (PreviousEventHandler) EventHandler = PreviousEventHandler;
}

/*
 * �C�x���g�o�b�t�@�������ς��̎��A1��Ԃ�
 */
uint8_t PushEvent(uint8_t ev) {
    if (ContNum < BufferMax) {
        Event[PushPoint] = ev;
        PushPoint = (PushPoint + 1) % BufferMax;
        ContNum++;  //�o�b�t�@�̓T�C�N���b�N�Ɏg���̂ŁA�����L����������
        return 0;
    } else {
        return 1;   //�o�b�t�@�t���Ƃ����G���[�����A���̏��G���[�`�F�b�N���Ă��Ȃ�
    }
}

/*
 * �C�x���g�o�b�t�@�ɉ��������Ă��Ȃ����́AEventNone��Ԃ�
 */
uint8_t PopEvent() {
    uint8_t ev;
    char str[100];

    if (ContNum > 0) {
        ev = Event[PopPoint];
        PopPoint = (PopPoint + 1) % BufferMax;
        ContNum--;
    } else {
        ev = EventNone;
    }
#ifdef DEBUG
    sprintf(str, "%02d,%02d,%02d", ContNum, PushPoint, PopPoint);
    display_drawChars(250, 30, str, WHITE, TextbgColor, FontMagx11);
#endif

    return ev;
}

void ClearBuffer() {
    PushPoint = 0;
    PopPoint = 0;
    ContNum = 0;    //�L���[�ɓ����Ă��鐔
}

/*
 * 10ms���Ɋ��荞��
 * Touch: �`���^�����O�`�F�b�N�@�@TouchStart���t���O�Ɏg��
 * �X���C�hSW: ��ԃ`�F�b�N
 * GPS���j�b�g: 1PPS���`�F�b�N�B�N���`�F�b�N�ɗ��p
 */
//#define SlideSWoff  0
//#define SlideSWon   8
uint8_t Timer5Flag = 0;
void Timer5Handler() {
    static uint8_t touch_status = 0xff;     //�`���^�����O�����p
    static uint8_t slideSW_status = 0x55;   //�`���^�����O�����p
    static uint8_t G1PPS_status = 0xff;     //�`���^�����O�����p

    if (Timer5Flag) return;
    Timer5Flag = 1;
    //�^�b�`�����A��߂����ɃC�x���g�����B�������ɂȂ��������C�x���g����
    //�^�b�`�ŁA0
    touch_status = (uint8_t)(touch_status << 1) | T_IRQ_GetValue();
    if ((touch_status & 0x3f) == 0x38) {
        //3��A���^�b�`(T_IRQ=0)�Ɗ��m������
        TouchStart |= 0x01;     // TouchDown Flag
        TouchCount = 0; //�J�E���^���Z�b�g
    } else if ((touch_status & 0x07) == 0) {
        //�A���^�b�`(0)���p�����Ă�����A10ms���ɃJ�E���g�A�b�v
        if (TouchCount < 200) {
            TouchCount++;
            if (TouchCount == 200) {
                //200x10ms=2s�������̏���
                //TouchCount��200�ɂȂ����������ALong����
                TouchStart |= 0x02;     //TouchLong Flag
            }
        }
        TouchStart |= 0x04; //�������֌W�Ȃ��A�^�b�`���p�����Ă��鎞  
        if (RepeatKey) RepeatKey++;
    } else if ((touch_status & 0x3f) == 0x07) {
        //�^�b�`���Ȃ���T_IRQ=1�A3��A���Ń^�b�`��߂��Ɣ���
        TouchStart = 0x08;  //TouchUp Flag
    }
    
    //�A���[���p�X���C�hSW�̏�ԃ`�F�b�N
    slideSW_status = (uint8_t)((slideSW_status << 1) | ALMSW_GetValue());
    if ((slideSW_status & 0x0f) == 0x08) {
        if (SlideSWStatus != SlideSWoff) {
            //����܂ŃI����ԂŁA�I�t�Ɣ��肳�ꂽ���A�C�x���g����
            SlideSWStatus = SlideSWoff +1;
        }
    } else if ((slideSW_status & 0x0f) == 0x07) {
        if (SlideSWStatus != SlideSWon) {
            //����܂ŃI�t��ԂŁA�I���Ɣ��肳�ꂽ���A�C�x���g����
            SlideSWStatus = SlideSWon +1;
        }
    }
    
    //GPS���j�b�g���AGPS�q���ǔ��ł����1PPS�����M�����
    if (Count1PPS < STABLE1PPS) {
        //�����M��ԂɂȂ�܂Ŏ��s
        G1PPS_status = (uint8_t)(G1PPS_status << 1) | G1PPS_GetValue();
        if ((G1PPS_status & 0x0f) == 0x08) {
            //�������Low���x���o����(3��A������L�ɂȂ�����=HLLL)
            Count1PPS++;
            if (Count1PPS >= STABLE1PPS) {
                GPSFlag = 1;
            }
        }
    }
    Timer5Flag = 0;

}


/*
 * TMR1��500us���̊��荞�݂ŁA���[�^���[�G���R�[�_�̏�Ԃ��`�F�b�N
 * 24�N���b�N/1��]
 * 1�b��1��]������A24��C�x���g��������B
 * �L�[�̃`���^�����O�h�~�Ɠ��l�̃v���O����
 * 3��=1.5ms�ԁA������ԂȂ�A���̃��x���ƔF��������
 * �@�����̊��荞�݂ŁA1�b�Ԃ�2000��`�F�b�N�ł���B1�b��2��]�������Ƃ��āA
 * �@�@�@24�N���b�N��x2��̏�ԕω��������B1�T�C�N����40��قǃT���v�����O�ł���v�Z�Ȃ̂ŁA
 * �@�@�@�M���ω��͏\����荞�߂�
 * �@��3��̃T���v�����O���ł����]���x�́A1����=(3+3)*500us=3ms�A24�N���b�N*3ms=72ms�Ȃ̂ŁA
 * �@�@1/72ms=13��]�ƁA����Ȃ�1�b�Ԃŉ�]�ł��Ȃ����x���Ȃ̂ŁA�\��
 * �N���b�N�̈���_�ł́AA�[�q�͏��OFF=1�̏��
 * 
 * RotCount���X�V���Ă���
 * �C�x���g�������x��Ă��ARotCount�̒l���g���ď�������̂ŁA�S���̃C�x���g�������K�v�Ƃ���
 * �킯�ł͂Ȃ��A�����������āARotCount=0�Ȃ�c�����C�x���g�̓X�L�b�v�����
 * 
 */
void RotaryHandler() {
    static uint8_t StartRot = 0;    //
    static int8_t Bdata = 0; // ���v���=1�A�����v���=0
    static uint8_t RotA_status = 0xff;  //ROT_A�����ω��_���`�F�b�N
    uint8_t newBdata;

    //A�[�q���̃��x�����`�F�b�N�B��~��=1�A��������0�ƂȂ�A�N���b�N�|�C���g��1�ɖ߂�
    RotA_status = (uint8_t)(RotA_status << 1) | ROT_A_GetValue();

    if ((StartRot == 0) && (RotA_status & 0x07) == 0) {
        //A�[�q��0�ɂȂ�����A��]�J�n�Ɣ��f
        StartRot = 1;
        //��]�J�n�Ɣ��f��������B�[�q�̃��x���ŉ�]�����𔻒�
        Bdata = ROT_B_GetValue();
    } else if ((StartRot == 1) && ((RotA_status & 0x07) == 0x07)) {
        //��]���삵�Ă��āAA�[�q��1�ɂȂ������̏���
        newBdata = ROT_B_GetValue();
        // B�[�q�����A1��0�̎�CW(�C���N��)�A0��1�̎�RCW(�f�N��)
        if (Bdata < newBdata) {
            //A�[�q��H�ɂȂ������AB�[�q��H�Ȃ甽���v�����ɉ�]
            RotCount--;
            if (RotaryEventCount < 2) {
                //���[�^���C�x���g���s�����B�������Ă��ARotCount���g���ď��������̂Ŗ��Ȃ��B
                RotaryFlag = 1;
                RotaryEventCount++;
            }
        } else if (Bdata > newBdata) {
            //A�[�q��H�ɂȂ������AB�[�q��L�Ȃ玞�v�����ɉ�]
            RotCount++;
            if (RotaryEventCount < 2) {
                //���[�^���C�x���g���s�����B�������Ă��ARotCount���g���ď��������̂Ŗ��Ȃ��B
                RotaryFlag = 1;
                RotaryEventCount++;
            }
        }
        //�����Ɖ�]�������ɖ߂����ꍇ�A�J�E���g�������A�I������P�[�X����B
        StartRot = 0;
    }
    
}


/*
 * �A���[��W(RTC_INTB=RB4��IOC)�ɂ�銄�荞��
 * 18F27K42�ł́ANegative�̎w�肪�ł���̂ŁA������ɕύX
 */
void AlarmWHandler() {
    //��ԕω����荞�݂̂��߁A0�ɂȂ����������t���O���Ă�
    AlarmStatus = 1;
    SmoothCount = 0;
}


/*
 * INTA��INT0���荞�݂�
 * 1�b���ƂɊ��荞�݂�����̂ŁA�����̍X�V�C�x���g����
 * �����ݒ���I2C�ʐM�͎~�߂āA�C�x���g�����Ŏ��s���邱�Ƃɂ���
 */
void RTC_handler() {
//*    PushEvent(EventTimeUpdate);
    RTC1sFlag = 1;
    EXT_INT0_InterruptFlagClear();
}

/*
 * ���t���ς�������`�F�b�N���āA�ύX����Γ��t�\���X�V
 * Year/Month/Day/Weekday��EEPROM�Ɋi�[���Ă����A�d�r����ւ���̕ύX�̎�Ԃ��Ȃ�
 */
uint8_t WriteYMD() {
    uint8_t jj, tmp;
    uint8_t changeDate = 0;
    uint16_t addr;

    //������������������Ȃ��悤�ɁA�ω��������������ɂ���
    addr = AddressYMD;
    for (jj = 0; jj < 4; jj++) {
        tmp = DATAEE_ReadByte(addr);
        //�j���A���A���A�N���`�F�b�N
        if (tmp != DateTime[jj+3]) {
            DATAEE_WriteByte(addr, DateTime[jj+3]);
            changeDate = 1;
        }
        addr++;
    }
    return changeDate;
}


/*
 * 1�b���ƂɍX�V����鎞���𗘗p���Ď��{���鏈��
 * ����3��
 * �@�E�b�̈�ʂ̐�����9�̎�:�@�����x�Z���T
 * �@�E�b�̈�ʂ̐�����3��8�̎��F�@�Ɠx�`�F�b�N
 * �@�E����:�@���t�̍X�V�`�F�b�N
 */
void TimeEvent() {
    static uint16_t Brightness = 1023;
    uint16_t BackLight = 100;
    uint16_t data;    

    if ((DateTime[0] & 0x0f) == 9) {
        //�����x�́A�b�̉�1����9�̎��ɍX�V
        Get_TempHumidity(&Temp, &Humidity);
        PushEvent(EventTempUpdate);
        PushEvent(EventHumidityUpdate);
    }   

    //�������ω��������ɏ������Ȃ��ƁA1�b�Ԃɉ�����������Ăяo����Ă��܂�
    else if (((DateTime[0] & 0x0f) % 5) == 3) {
        //�b�̉�1����3,8�̎��ɏƓx�`�F�b�N
        //�Ɠx�Z���T�ŁA���邳��5�b���ƂɎ擾����
        //ADC�̌��ʂ́A12bit��0-4095�B���邢�Ƒ傫�Ȑ��l
        //�������A�Â���ADC�̌��ʂ����Ȃ�΂��
        //���ԂȂ�3.07V@3.29V�ŁA954-958�A��̌u���������ƁA50-150�ʂ������� <-���̃f�[�^�Â�
        //�΂����ጸ���邽�߁A8��̍��v�l�����A���ω�
        //ADC_GetConversion�Ń`���l���ݒ�AADC�J�n-�擾�܂Ŏ��s
        data = ADCC_GetSingleConversion(PhotoDiode);
        Brightness = Brightness/8*7 + data/4;
        //            sprintf(str, "%d", data);
        //            display_drawChars(170, 20, str, WHITE, TextbgColor, FontMagx11);
        // Duty��ύX���ăo�b�N���C�g�̖��邳��ύX
        // Brightness����萔�ȏ�̎��́A�o�b�N���C�g�͂قڏ펞�_���B
        BackLight = Brightness/8*3 + 10;
        // BackLight�́A�Œ�10-max999   0���ƑS����ʌ����Ȃ��Ȃ�̂ŁA+10�Ƃ��Ă���
        if (BackLight >= 1000) BackLight = 999;
        //PWM�́A10�r�b�g�̉𑜓x (TMR6�̎���=PR6=0xff�ɐݒ肵����)
        PWM6_LoadDutyValue(BackLight);
        
#ifdef DEBUG2
        //�P�x�̃��x����\�� (�f�o�b�O�p)
        //            sprintf(str, "Br=%4d", Brightness);
        sprintf(str, "B=%d", BackLight);
        display_drawChars(170, 20, str, WHITE, TextbgColor, FontMagx11);
#endif
    }
    
    //���t���ς�������́A���Ƀg���K�Ȃ��̂ŁA�����Ń`�F�b�N�B����00���̎��������{
    else if (DateTime[1] == 0x00) {
        if (WriteYMD()) { //�X�V����΁A���t���X�V����
            //���t���ς������A�ēxGPS��M�����邽�߁A�J�E���^��0�ɖ߂�
            if (Count1PPS >= STABLE1PPS) Count1PPS = 0;
            PushEvent(EventDateUpdate);
        }
    }
}


/*
 * �X���C�hSW�I�t�̏������[�`��
 */
void SlideSWoffProc() {
   
    AlarmSoundOff();
    RTC_resetAlarm();
    AlarmStatus = 0;    //�X���C�hSW��Off������
}

/*
 * �X���C�hSW�I���̏������[�`��
 * �A���[���Z�b�g���āA���̎�����EEPROM�ɏ�������
 */
void SlideSWonProc() {
    uint8_t jj, tmp;
    uint16_t addr;

    RTC_setAlarmTime(AlarmTime);
    //EEPROM�ɃA���[���������������݁A�d���؂�Ă��ē������A�g����悤��
    addr = AddressAlarm;
    for (jj=0; jj<3; jj++) {
        tmp = DATAEE_ReadByte(addr);
        if (tmp != AlarmTime[jj]) {
            DATAEE_WriteByte(addr, AlarmTime[jj]);
        }
        addr++;
    }
}


/*
 * RotCount��0�łȂ����́A��������
 * ���[�^���[�́A�A���I�ɃC�x���g�������邪�A�C�x���g�������ɏ����ł��Ȃ��Ă��悭�A
 * RotCount=0�łȂ���������������Ηǂ�
 */
void RotaryProcedure() {
    int16_t delta;  //8bit���ƕs�����邱�Ƃ�z��
    uint8_t mm, hh;

    //�m�[�}�����̃��[�^���[SW�̓A���[��������ύX
    if (RotCount == 0) return;

    delta = RotCount;
    RotCount = 0;
    
    mm = Bcd2Hex(AlarmTime[0]);
    if (DisplayMode != DisplayMode3) {
        //������]��������A�{���A4�{���Ł@�@�������Adelta�́Aint8�Ƃ������Ƃɒ���
        if ((delta > 2) || (delta < -2)) {
            //8�{�����܂�
            if (Accel < 8) Accel = Accel * 2;
            delta = delta * Accel;
        }
        else Accel = 1;
    } else {
        delta = delta * 5;  //5���P�ʂ�
        mm = mm - (mm % 5);    //5���P�ʂɐ؂艺��
    }
    
    hh = Bcd2Hex(AlarmTime[1]);
    IncDecTime(delta, &hh, &mm);
    AlarmTime[0] = Hex2Bcd(mm);
    AlarmTime[1] = Hex2Bcd(hh);
    
    //�A���[��SW��on��������ARTC�̃A���[��������ύX
    if (SlideSWStatus == SlideSWon) RTC_setAlarmTime(AlarmTime);
    
//--    PushEvent(EventFormUpdate);    //�C�x���g���Ɣ����x���g�����舫��
    
    //EEPROM�ɏ������ރ^�C�~���O�����B���[�^���[�����邮��񂵂Ă��鎞�́A
    //���x��EEPROM�ɃA���[���������������݂����Ȃ�
    //�A���[��SW��On�ɂ����������Ɍ��肷��
    
}


/*
 * �^�b�`�������Wx,y(�O���t�B�b�N���W�Ɋ��Z��������)���A�w�肳�ꂽ�I�u�W�F�N�g�͈̔͂�
 * �����Ă��邩�`�F�b�N
 * �w�肵���I�u�W�F�N�g���������Ɣ��肳����1��Ԃ��B����ȊO�́A0
 * obj: RscObject�Œ�`���Ă������
 */
int8_t ButtonPush(int16_t x, int16_t y, MainFormRsc *objrsc) {
    int16_t xx, yy;
    
    xx = x - objrsc->x;
    yy = y - objrsc->y;

    if ((xx >= 0) && (xx < objrsc->xw)) {
        if ((yy >= 0) && (yy < objrsc->yw)) {
            //�{�^���̋�`�̈�ɓ����Ă����ꍇ
            return 1;
        }
    }    
    return 0;
}

/*
 * SD�J�[�h���}�E���g����
 * ����mount���Ă�����A��Uunmount
 */
void remount(void) {
    FRESULT res;
    char str[50];

//    if (SDcardMount) f_unmount("");
    res = f_mount(&Drive, "", 0);
    
//    sprintf(str, "res=%d", res);
//    display_drawChars(5, 80, str, WHITE, TextbgColor, FontMagx22);
//    __delay_ms(500);
//    __delay_ms(500);
//    __delay_ms(500);

    if (res == FR_OK) SDcardMount = 1;
    else SDcardMount = 0;
}

/* 
 * �^�b�`�����Ƃ���̎��Ӄf�[�^���擾 (3x3�̗̈�)
 * �f�[�^�́APixelData[]�ɁA����̍��W��PixelXY[]�Ɋi�[
 */
void GetPixelData(int16_t x, int16_t y) {
    uint8_t jj;
    uint8_t rgb[27];    //9�h�b�g���̃f�[�^�i�[�̈�

    PixelXY[0] = x-1;
    PixelXY[1] = y-1;
    addset(PixelXY[0], PixelXY[1], x+1, y+1);
    read_pixel(rgb, 9);
    for (jj = 0; jj < 9; jj++) {
        PixelData[jj] = display_color565(rgb[jj*3], rgb[jj*3+1], rgb[jj*3+2]);
    }
}

/* 
 * �ێ����Ă������f�[�^�ŏ����߂�
 * PixelXY[0]��9999�ȊO��������Ƃ���������t���A�f�[�^�̗L�������m��
 */
void SetPixelData() {
    if (PixelXY[0]!=9999) glcd_array(PixelXY[0], PixelXY[1], 3, 3, (uint8_t *)PixelData);
}

//�^�b�`���ꂽ���̏���
void TouchProcedure() {
    uint8_t mm, hh;
    char str[100];

    //�^�b�`�������Ɉ������
    SetPixelData();     //���̃f�[�^��߂�
    GetPixelData(TouchX, TouchY);   //�����ޏꏊ�̃f�[�^��ǂݍ���
    display_fillRect(TouchX-1, TouchY-1, 3, 3, RED);    // 3x3�̐Ԃ��l�p������
    
#ifdef DEBUG2
    sprintf(str, "%d,%d -> %3d,%3d ", TouchRawX, TouchRawY, TouchX, TouchY);
    display_drawChars(145, 30, str, WHITE, TextbgColor, FontMagx11);
    sprintf(str, "Adj=%d,%d, %d,%d ", T_x1, T_y1, T_x2, T_y2);
    display_drawChars(145, 40, str, WHITE, TextbgColor, FontMagx11);
#endif
    //�A���[�����Ƀ^�b�`������X�k�[�Y
    if (AlarmStatus) {
        AlarmSoundOff();
        AlarmStatus = 0;    //�A���[���~�߂���0��
        RTC_resetAlarm();
        SmoothCount++;
        if (SmoothCount <= 12) {
            //5����ɃA���[���Đݒ�@12��܂ŁA1���Ԍ�܂�
            mm = Bcd2Hex(AlarmTime[0]);
            hh = Bcd2Hex(AlarmTime[1]);
            IncDecTime(SmoothCount*5, &hh, &mm);    //5��*Smooth�񐔂̎����ɐݒ�
            SmoothAlarmTime[0] = Hex2Bcd(mm);
            SmoothAlarmTime[1] = Hex2Bcd(hh);
            //�����ŁARTC�ɃA���[�������Đݒ�
            RTC_setAlarmTime(SmoothAlarmTime);
            
        } else {
            //�X�k�[�Y�I���A�A���[��SW��On�̂܂܂̎�
            //�I���W�i���̃A���[���������Z�b�g������(�����p)
            RTC_setAlarmTime(AlarmTime);
        }
        return;
    }
    
    if (ButtonPush(TouchX, TouchY, (MainFormRsc *)&CurrentRsc[GearObj])) {
        //���ԃA�C�R�����N���b�N������A�ݒ��ʂ�
        //�J�X�^�}�C�Y�p��Date, Time, Alram, Temp, Humi�̏�����
        SetEventHandler(SettingEventHandler);
        ClearBuffer();
        PushEvent(EventFormInit);
    }
    else if (TouchX < 160 && TouchY > 120) {
        //��ʂ̍�����1/4�G���A���^�b�`���ĕ\�����[�h�ύX
        DisplayMode = (DisplayMode +1) % (sizeof(RscData)/sizeof(RscData[0])); //���̏��A�\�����[�h��4��
        DATAEE_WriteByte(AddressDisplayMode, DisplayMode);  //�ύX�����珑����
        PushEvent(EventFormInit);
        Count1PPS = 0;  //�\����������������Ƃ��ɁA�ēxGPS��M

        //�ă}�E���g�������good�BSD�J�[�h�����������Ă��A�\�����[�h�ύX�����炿���ƃA�N�Z�X�ł���
        remount();
    }
    
}

/*
 * �����O�^�b�`
 * �ăf�W�^�C�Y�����{
 */
void TouchLongProcedure() {
            
    //�^�b�`������
//        display_drawChars(250, 140, "T- Long", WHITE, TextbgColor, FontMagx11);
    //�^�b�`�������Őݒ胂�[�h�ֈڍs
//        Mode = Setting;
//    DisplayMode = Setting;
//--    PushEvent(EventFormInit);
    
    AlarmSoundOn(AlarmTwice);
    TouchAdjust();
    PushEvent(EventFormInit);

}


/*
 * �`�F�b�N�T�����v�Z���āA��v���Ă��邩�`�F�b�N����
 * Buffer�ɁA$����*�܂ł̃f�[�^�����鎞�Ɍv�Z����
 * �G���[����΁A1�@�@$��*���Ȃ��ꍇ�A�`�F�b�N�T���v�Z�������Ă��Ȃ��ꍇ�Ȃ�
 */
uint8_t CalcCheckSum(char *buf) {
    char *start;
    char *end;
    uint8_t checksum;
    char chks[3];
    
    start = strchr(buf, '$');
    if (start == NULL) return 1;    //������Ȃ���΁A�G���[
    end = strchr(buf, '*');
    if (end == NULL) return 1;      //������Ȃ���΁A�G���[
    
    checksum = 0;
    start++;    //$�̎�����v�Z�ΏۂƂ���
    while (start < end) {
        checksum = (uint8_t)(checksum ^ (*start)); //xor�v�Z
        start++;
    }
    sprintf(chks, "%02X", checksum);    //�����ŁA�`�F�b�N�T����16�i�̕�����
    
    //�v�Z�����`�F�b�N�T���ƃf�[�^�ł�������`�F�b�N�T��(?�̌���2����)���r
//    if (checksum == strtol(end+1, NULL, 16)) {    //������ł�OK�����A�v���O�����T�C�Y��
    if ((chks[0] == end[1]) && (chks[1] == end[2])) {
        return 0;
    } else {
        //�`�F�b�N�T���G���[�̎�
        return 1;
    }
    
}

/*
 * �擾����GPRMC�̃f�[�^����������
 * GPRMC�ȊO�̃f�[�^�̎��́A�������Ȃ�
 * �G���[��: 1
 */
uint8_t GPRMCdatacheck() {
    uint8_t jj, timeout;
    char *p;
    char str[100];

    //$GPRMC�̃f�[�^�̎���������
    if (strncmp(Buffer, "$GPRMC", 6) != 0) return 1;

//    display_drawChars(0, 85, Buffer, WHITE, TextbgColor, FontMagx11);
    
    //�`�F�b�N�T���̃`�F�b�N
    if (CalcCheckSum(Buffer)) {
//        sprintf(str, "x");
//        display_drawChars(0, 110, str, WHITE, TextbgColor, FontMagx11);
        return 1;   //�G���[��
    }
//    sprintf(str, "o");
//    display_drawChars(0, 110, str, WHITE, TextbgColor, FontMagx11);

    //�f�[�^������Ȃ̂ŁA�K�v�������o��
    // hhmmss: 7-12�����@�@1�ڂ̃f�[�^=�ŏ���,�̌ォ��f�[�^������@�@BCD�`��
    DateTime[2] = (uint8_t)(((Buffer[7]-'0')<<4) + (Buffer[8]-'0'));   //hh
    DateTime[1] = (uint8_t)(((Buffer[9]-'0')<<4) + (Buffer[10]-'0'));  //mm
    DateTime[0] = (uint8_t)(((Buffer[11]-'0')<<4) + (Buffer[12]-'0')); //ss
    
    // ddmmyy: 9�ڂ̃f�[�^=9�Ԗڂ�,�̌ォ��f�[�^������
    p = strchr(Buffer, '$');
    for (jj = 0; jj < 9; jj++) {
        p = strchr(p+1, ',');
        if (p == NULL) return 1;    //������Ȃ�������A��߂�
    }
    DateTime[4] = (uint8_t)(((p[1]-'0')<<4) + (p[2]-'0')); //dd
    DateTime[5] = (uint8_t)(((p[3]-'0')<<4) + (p[4]-'0')); //mm
    DateTime[6] = (uint8_t)(((p[5]-'0')<<4) + (p[6]-'0')); //yy
    
    //�o�x���擾: 5�Ԗڂ̃f�[�^    yyyyy.yy
    //�o�x�ŁA������␳���悤�Ǝv�������ǁA���{�����ł��A�n��ɂ���Ă͂�낵���Ȃ��̂ŁA���~
    /*
    p = strchr(Buffer, '$');
    for (jj = 0; jj < 5; jj++) {
        p = strchr(p+1, ',');
        if (p == NULL) return 1;
    }
    uint8_t len = (strchr(p, '.') - p)-3; //�o�x�̌����@�K������3���ł͂Ȃ��̂�
    for (jj = 0; jj < len; jj++) {
        TZ = TZ*10 + (p[1+jj]-'0');
    }
    //���o��+�A���o��-�ɂ���
    p = strchr(p+1, ',');
    if (p[1] == 'W') TZ = -TZ;
    TZ = (TZ+8) / 15;   //1���ԓ�����o�x��15�x�Ȃ̂ŁA�o�x���玞�����v�Z
     */
    
    //�W������JST (���ݒn�̎���)��
    int8_t hh;
    hh = Bcd2Hex(DateTime[2]) + TZ;    //JST�Ȃ�+9
    if (hh >= 24) {
        //TZ�␳��A�[��12�����߂�����A���t��1���i�߂�
        hh = hh - 24;
        DateTime[4] = Hex2Bcd(Bcd2Hex(DateTime[4]) + 1);  //���t�i�߂�@BCD�Ȃ̂�
    } else if (hh < 0) {
        //TZ���}�C�i�X�̏ꍇ���ꉞ�v���O�������Ă���
        hh = hh + 24;
        DateTime[4] = Hex2Bcd(Bcd2Hex(DateTime[4]) - 1);  //���t�߂� 1����������A0���ɂȂ邪���Ԃ�OK
    }
    DateTime[2] = Hex2Bcd((uint8_t)hh);  //������̎��Ԃ�߂�
    dateErrorCheck(DateTime);   //���t�̒���
    
    //        sprintf(str, "%02x:%02x:%02x %02x/%02x/%02x", DateTime[2], DateTime[1], DateTime[0], DateTime[6], DateTime[5], DateTime[4]);
    //        display_drawChars(0, 110, str, WHITE, TextbgColor, FontMagx11);
    
    //�������擾�ł����̂ŁARTC�ɏ������ށ@����1PPS���g���K�ɂ���̂��ǂ�
    if (DateTime[0] > 0x55) {
        return 1;   //���オ�肪���鎞�́A�G���[�Ƃ��čĎ擾
    }
    
    timeout = 150;
    while (G1PPS_GetValue() == 1) {
        //����1PPS��҂�
        __delay_ms(10); //10ms wait
        timeout--;
        if (timeout == 0) break;    //1.5�b�҂��ăp���X���Ȃ�������A�I��
    }
    DateTime[0] = Hex2Bcd(Bcd2Hex(DateTime[0])+Count1PPS+1);    //GPS��M�����b��+1����
    RTC_setTime(DateTime);
    RTC_setDate(DateTime);
    Count1PPS = STABLE1PPS;     //GPS��M����߂�

    //�����ŃA�b�v�f�[�g�����Ȃ��Ă��A1�b���Ƃ̍X�V�͂���̂ł�߂�
    //���t���ύX�ɂȂ��Ă�����A�X�V�B
    PushEvent(EventDateUpdate);

#ifdef DEBUG2
//    �f�o�b�O�p�Ƀf�[�^�\��
    Buffer[13] = '\0';
    display_drawChars(0, 95, Buffer, WHITE, TextbgColor, FontMagx11);
#endif

    return 0;
}

/*
 * GPS���j�b�g����V���A���f�[�^��M
 * ���ꂪ�Ăяo���ꂽ��ALF�܂ō\�킸�f�[�^�擾
 * �G���[����Β��f
 * �f�[�^��M�́A�ł��邾���A���������Ȃ��ƁA��肱�ڂ�
 * �C�x���g�����́A����������ۂ̏����܂łɃ^�C�����O����̂ŁA�_���ȏꍇ������
 * ��{�I�ɂ��̃��[�`�����ŁA��C�Ɏ�M���Ȃ��Ǝ��s����
 *     //1�s�擾���āA���̏������I���܂ŁA���̃f�[�^�擾�����Ȃ�
 */
void drawGPSindicator(uint16_t color) {
#define GPSindicatorX   3
#define GPSindicatorY   235
#define GPSindicatorR   3
    display_fillCircle(GPSindicatorX, GPSindicatorY, GPSindicatorR, color);    
}

void StartGPS() {
    uint8_t rxData;
    uart1_status_t rxStatus;
    uint8_t errorCount;
    uint8_t start;
    char *BufferP;     //Buffer�̂ǂ��܂ŏ������񂾂��Ƃ����f�[�^�ʒu�������|�C���^
    
    //GPS�̏��: ��M���͉��F�A��������Ɨ�(�d��������Ŏ�M�O�ł�)
    drawGPSindicator(YELLOW);
    
    start = 0;
    errorCount = 0;
    while (1)
        while (UART1_is_rx_ready()) {
            rxData = UART1_Read();
            rxStatus = UART1_get_last_status();
            if (rxStatus.ferr) {
                PushEvent(EventStartGPS);
                return;
            }
            //�ŏ���0x0a������܂ŁA�f�[�^��ǂݔ�΂��B������Astart=1�ɂ��ăf�[�^���擾����
            if (start == 0) {
                if (rxData == 0x0a) {
                    start = 1;
                    BufferP = Buffer;
                }
            } else {
                *BufferP = rxData;
                BufferP++;
                
                if ((BufferP - Buffer == 6) && (strncmp(Buffer, "$GPRMC", 6) != 0)) {
                    //$GPRMC�̃f�[�^�̎�������������̂ŁA������`�F�b�N���������{���͗ǂ�
                    start = 0;
                }

                Count1PPS = 0;  //��M������̃p���X���J�E���g����
                if (rxData == 0x0a) {
                    //���s�R�[�h(0x0a=LF)��������A�f�[�^��M���I��
                    *BufferP = '\0';
                    if (GPRMCdatacheck()) {
                        //�G���[�̎��́A�f�[�^�擾��蒼��
                        drawGPSindicator(RED);
                        start = 0;

                        //�������[�v�ɂȂ�Ȃ��悤�ɁA5��G���[����������A��U�����čēx���s
                        if (errorCount++ > 5) {
                            PushEvent(EventStartGPS);                    
                            return;
                        }
                    }
                    else {
                        //GPS�̏��: ��M���͉��F�A��������Ɨ�(�d��������Ŏ�M�O�ł�)
                        drawGPSindicator(GREEN);
                        return; //����������ȏo��
                    }
                }
                if (BufferP - Buffer > 95) {
                    //�o�b�t�@�����ӂ��̂͂��������̂ŁA�f�[�^�j�����A���f����
                    PushEvent(EventStartGPS);
                    return;
                }
            }
        }
}


/*
 * ���C����ʂ̕\�����X�V����
 * CurrentRsc�̏������ɏ��߂ĕ`�悷�鎞�ɌĂяo�����
 * �ݒ�{�^�����́A�����ŕ`��
 */
void DrawForm() {
    
    //�����\���͕ω����������������\���X�V����̂ŁABCD�ł͂��肦�Ȃ����l��ݒ肵�Ă���
    resetPreDateTime();
    lcd_fill(TextbgColor);

    //�ݒ�{�^��
    DrawGearIcon(CurrentRsc[GearObj].x, CurrentRsc[GearObj].y, CurrentRsc[GearObj].color);
    
    //�����\���X�V
//    DrawTime(DateTime, &CurrentRsc[TimeObj]);
    
    //���t�̍X�V
    DrawDate(&CurrentRsc[DateObj]);    //���t��`��
    DrawCalendar(&CurrentRsc[CalendarObj]);  //���t�X�V������J�����_�[���X�V
    
    //�����x���X�V���ꂽ��
    DrawTemp(Temp, &CurrentRsc[TempObj]);
    DrawHumidity(Humidity, &CurrentRsc[HumidityObj]);
    
    //�A���[���\���̍X�V�@�@�X���C�hSW�̏�ԂŐF��ς���
    DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);

}


/*
 * ���C���̃C�x���g�n���h��
 * 
 */
void MainEventHandler() {
    uint8_t evt;
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
        case EventTimeUpdate:
            RTC_read(DateTime);
            resetCTFG();    //RTC��1�b���Ƃ̊����݌�A�����K�v
            DrawTime(DateTime, &CurrentRsc[TimeObj]);    //�����\���X�V
            TimeEvent();
            break;

        case EventDateUpdate:
            DrawDate(&CurrentRsc[DateObj]);    //���t��`��
            DrawCalendar(&CurrentRsc[CalendarObj]);  //���t�X�V������J�����_�[���X�V
            break;

        case EventFormInit:
            resetCTFG();    //RTC��1�b���Ƃ̊����݌�A�����K�v
            //���\�[�X�����������āA��ʕ`�悷��
            CurrentRsc = RscData[DisplayMode]; //���[�h�ɑΉ��������\�[�X��ݒ�
            DrawForm();
            PixelXY[0] = 9999;  //�f�[�^���Ȃ����Ƃ�����
            break;

        case EventSlideSWoff:
            //�X���C�hSW��Off����
            SlideSWoffProc();
            DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //�A���[���\���̍X�V
            break;
        case EventSlideSWon:
            //�X���C�hSW��On����
            SlideSWonProc();
            DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //�A���[���\���̍X�V
            break;
        case EventRotary:
            RotaryEventCount--;
            RotaryProcedure();
            DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //�A���[���\���̍X�V
            //�A�i���O���v�̏ꍇ�́ADrawTime���Ăяo���Ȃ��ƃA���[���j�̍X�V������Ȃ����A1�b���ɍX�V�����̂ŁA���ɖ��͂Ȃ�
            //�A�i���O���v�S�̂��ĕ\������̂ŁA�����ōX�V���Ă�����قǃ��X�|���X�悭�݂��Ȃ�
            //            DrawTime(DateTime, &CurrentRsc[TimeObj]);    //�����\�����X�V
            break;
            
        case EventTouchDown:
            TouchProcedure();
            break;
        case EventTouchLong:
            TouchLongProcedure();
            break;                    
//        case EventTouchUp:
//            break;
            
        case EventTempUpdate:
            DrawTemp(Temp, &CurrentRsc[TempObj]);
            break;
        case EventHumidityUpdate:
            DrawHumidity(Humidity, &CurrentRsc[HumidityObj]);
            break;
        case EventStartGPS:
            StartGPS();
            break;
    }
    
}


/*
 * DATAEE_WriteByte�̃��[�h��
 */
void DATAEE_WriteWord(uint16_t bAdd, uint16_t bData) {
    DATAEE_WriteByte(bAdd, lo(bData));
    DATAEE_WriteByte(bAdd+1, hi(bData));
}

/*
 * DATAEE_ReadByte�̃��[�h��
 */
uint16_t DATAEE_ReadWord(uint16_t bAdd) {
    uint16_t data = DATAEE_ReadByte(bAdd);
    data += (uint16_t)DATAEE_ReadByte(bAdd+1)<<8;
    return data;
}


/*
 * �^�b�`�̍��W���O���t�B�b�N�̍��W�ɕϊ�
 * ����͊��ɂ���ĕω����邽�߁ATouchXT2046.c���炱���ֈړ�����
 */
void TransCoordination(uint16_t x, uint16_t y, int16_t *xg, int16_t *yg) {
    int32_t xx, yy;

#ifdef DEBUG2
    char str[100];
    sprintf(str, "P1=(%d, %d) P2=(%d, %d)", T_x1, T_y1, T_x2, T_y2);
    display_drawChars(0, 140, str, WHITE, TextbgColor, FontMagx11);
#endif
    
    if ((T_x2 == T_x1) || (T_y2 == T_y1)) {
        *xg = (int16_t)x;
        *yg = (int16_t)y;
        return;
    }

    //�ȉ��̌v�Z���x�ɒ���
    // (20,20)  (300,220)�̃^�b�`��̍��W��T_x1, T_y1, T_x2, T_y2�ɕێ�
    xx =  (int32_t)x - (int32_t)T_x1;
    xx = xx * 280 / ((int32_t)T_x2 - (int32_t)T_x1) + 20;
    if (xx < 0) *xg = 0;
    else if (xx >= LCD_WIDTH) *xg = LCD_WIDTH -1;
    else *xg = (int16_t) xx;
    
    yy = (int32_t)y - (int32_t)T_y1;
    yy = yy * 200 / ((int32_t)T_y2 - (int32_t)T_y1) + 20;
    if (yy < 0) *yg = 0;
    else if (yy >= LCD_HEIGHT) *yg = LCD_HEIGHT -1;
    else *yg = (int16_t) yy;

}


/*
 * �^�b�`���W�����p���b�Z�[�W
 * �Ώۂ̃��b�Z�[�W�𔒂ŁA����ȊO���O���[��
 * num=0/1  ����ȊO�̃G���[�`�F�b�N���Ă��Ȃ�
 */
void TouchAdjMsg(uint8_t num) {
    uint16_t color[] = { WHITE, GREY };
    char str[100];

    //����̏\����`��@���S���W(20,20)
    display_drawLine(16, 20, 24, 20, color[num]);
    display_drawLine(20, 16, 20, 24, color[num]);

    SetFont(FontNormal);
    strcpy(str, "1. Touch + at Up Left");
    display_drawChars(50, 100, str, color[num], TextbgColor, FontMagx12);
    strcpy(str, "2. Touch + at Bottom Right");
    display_drawChars(50, 120, str, color[1-num], TextbgColor, FontMagx12);

    //�E���̏\���@�@���S���W(300, 220))
    display_drawLine(296, 220, 304, 220, color[1-num]);
    display_drawLine(300, 216, 300, 224, color[1-num]);
    
}


/*
 * (20,20)�A(300,220)��2�_���^�b�`�������̍��W��(T_x1, T_y1)�A(T_x2, T_y2)��
 * ��荞�ނ��߂̃��[�`��
 */
void TouchAdjust(void) {
    uint8_t count;
    uint16_t addr;
    char str[100];
    int16_t dx, dy;

    lcd_fill(BLACK); //��ʂ��N���A
    //�ŏ��ɍ�����^�b�`���āA���ɉE�����^�b�`���Ă��炤
    count = 0;
    while (1) {
        if (count == 0) {
            TouchAdjMsg(0);
            // 1��ڂ̃^�b�`�̈ʒu
            while ((TouchStart & 0x01) == 0);
            while (GetTouchLocation(&T_x1, &T_y1) == -1);
            //�^�b�`������s�b�Ƃ��������o��
            AlarmSoundOn(AlarmSingle);
            
            sprintf(str, "X1=%4d, Y1=%4d", T_x1, T_y1);
            display_drawChars(50, 20, str, WHITE, BLACK, FontMagx12);
            
            count++;
        } else {
            TouchAdjMsg(1);            
            // 2��ڂ̃^�b�`�܂ŏ����x���B�����ǂ����āA1��ڂƓ����ɂȂ�������
            __delay_ms(500);
            while ((TouchStart & 0x01) == 0);   //�����݂Ń^�b�`�ƔF�������܂Ń��[�v
            while (GetTouchLocation(&T_x2, &T_y2) == -1);
            //�^�b�`������s�b�Ƃ��������o��
            AlarmSoundOn(AlarmSingle);
            
            sprintf(str, "X2=%4d, Y2=%4d", T_x2, T_y2);
            display_drawChars(100, 200, str, WHITE, BLACK, FontMagx12);
            
            //2��ڂ̃^�b�`��1��ڂƋ߂����́A��蒼������
            //2�_�̋�����X,Y�e1500�ʂ���@(1500/16)^2=8800�@�ȉ��̌v�Z���ƁA18000�ɂȂ�
            dx = (T_x1 - T_x2)/16;  //���Ԃ񉉎Z�����B�܂�2�悵����int16�𒴂��Ȃ��悤
            dy = (T_y1 - T_y2)/16;
            if (dx*dx + dy*dy < 12000) {
                //�������Z�����́A������x
                strcpy(str, "- Try Again -  ");
                display_drawChars(100, 200, str, WHITE, BLACK, FontMagx12);            
                continue;
            }
            
#ifdef DEBUG2
            sprintf(str, "P1=(%d, %d) P2=(%d, %d)", T_x1, T_y1, T_x2, T_y2);
            display_drawChars(0, 140, str, WHITE, TextbgColor, FontMagx11);
            //                for (int8_t dd=0; dd<50; dd++) __delay_ms(1000);
#endif
            __delay_ms(500);
            break;
        }
    }
    //���������^�b�`���W����������
    addr = AddressTouch;
    DATAEE_WriteWord(addr,   T_x1);
    DATAEE_WriteWord(addr+2, T_y1);
    DATAEE_WriteWord(addr+4, T_x2);
    DATAEE_WriteWord(addr+6, T_y2);
    
}


/*
 * 16bit RGB=565�J���[��bmp�t�@�C����ǂݍ��ݕ\������
 * 
 * 0x0000�@(2)	bfType		�t�@�C���^�C�v�@�ʏ��'BM'
 * 0x0002�@(4)	bfSize		�t�@�C���T�C�Y (byte)
 * 0x0006�@(2)	bfReserved1	�\��̈�@��� 0
 * 0x0008�@(2)	bfReserved2	�\��̈�@��� 0
 * 0x000A�@(4)	bfOffBits	�t�@�C���擪����摜�f�[�^�܂ł̃I�t�Z�b�g (byte)
 * 0x000E�@(4)	bcSize		�w�b�_�T�C�Y
 * 0x0012�@(4)	bcWidth		�摜�̕� (�s�N�Z��)
 * 0x0016�@(4)	bcHeight	�摜�̍��� (�s�N�Z��) �����Ȃ�C�摜�f�[�^�͉�������
 *                                               �����Ȃ�C�摜�f�[�^�͏ォ�牺��
 * 0x001A�@(2)	bcPlanes	�v���[�����@��� 1
 * 0x001C�@(2)	bcBitCount	1��f������̃f�[�^�T�C�Y (bit)
 * 
 * �����ł́A16bit�J���[�ƌ��ߑł����Ă���
 */
void ReadBmp16() {
    int16_t x, y, xx;
    int16_t kk;
    UINT actualLength;
    FRESULT res;
    uint16_t num;
    
    if (!SDcardMount) return;
    //�t�@�C������8�����܂ŃT�|�[�g
    res = f_open(&FileObject, "Open8.bmp", FA_READ);    //DHT20�ɕ\�L�ύX
    if (res != FR_OK) return;
    
    //Open
    //�w�b�_�Ǎ�
    f_read(&FileObject, SDcardBuffer, 32, &actualLength); //32�o�C�g���ǂݍ���
    if (SDcardBuffer[bcBitCount] == 16) {
        //16bit�J���[�̃f�[�^�̎��̂ݏ���
        x = SDcardBuffer[bcWidth] + (SDcardBuffer[bcWidth+1]<<8);   //Width
        y = SDcardBuffer[bcHeight];                             //Height
        //�V�[�N���āA�f�[�^�̐擪�܂ňړ�
        f_lseek(&FileObject, SDcardBuffer[bfOffBits]);
        
        //bmp�f�[�^�́A�\������鉺�̍s����n�܂�
        //16bit�J���[��bmp�����Ɍ��肵�Ă���
        for (kk = 0; kk < y; kk++) {
            //DATASIZE���l�������v���O����
            num = (uint16_t)x * 2;  //1�s�̃o�C�g��=x�h�b�g x 16bit
            xx = 0;
            while (num > DATASIZE) {
                f_read(&FileObject, SDcardBuffer, DATASIZE, &actualLength); //�����h�b�g��x2�o�C�g���̃f�[�^��ǂݍ���
                glcd_array(xx, y-1-kk, DATASIZE/2, 1, SDcardBuffer);
                num -= DATASIZE;
                xx = xx + DATASIZE/2;
            }
            f_read(&FileObject, SDcardBuffer, num, &actualLength); //�����h�b�g��x2�o�C�g���̃f�[�^��ǂݍ���
            glcd_array(xx, y-1-kk, num/2, 1, SDcardBuffer+100*0);
        }
    }
    f_close(&FileObject);
}


//UINT outstream (   /* Returns number of bytes sent or stream status */
//    const BYTE *p,  /* Pointer to the data block to be sent */
//    UINT btf        /* >0: Transfer call (Number of bytes to be sent). 0: Sense call */
//)
//{
//    uint8_t dataH, dataL;
//    
////            while (num > DATASIZE) {
////                f_read(&FileObject, SDcardBuffer, DATASIZE, &actualLength); //�����h�b�g��x2�o�C�g���̃f�[�^��ǂݍ���
////                glcd_array(xx, y-1-kk, DATASIZE/2, 1, SDcardBuffer);
////                num -= DATASIZE;
////                xx = xx + DATASIZE/2;
////            }
////            f_read(&FileObject, SDcardBuffer, num, &actualLength); //�����h�b�g��x2�o�C�g���̃f�[�^��ǂݍ���
////            glcd_array(xx, y-1-kk, num/2, 1, SDcardBuffer+100*0);
//
//    UINT cnt = 0;
//
//    if (btf == 0) {     /* Sense call */
//        /* Return stream status (0: Busy, 1: Ready) */
//        /* When once it returned ready to sense call, it must accept a byte at least */
//        /* at subsequent transfer call, or f_forward will fail with FR_INT_ERR. */
//        cnt = 1;    //���1
//    }
//    else {              /* Transfer call */
//        do {    /* Repeat while there is any data to be sent and the stream is ready */
//            dataL = *p++;
//            dataH = *p++;
////            write_data(dataH);
////            write_data(dataL);
//
//            cnt += 2;
//        } while (cnt < btf);
//    }
//
//    return cnt;
//
//}

/*
void ReadBmp16stream() {
    int16_t x, y, xx;
    int16_t kk;
    UINT actualLength;
    FRESULT res;
    uint16_t num;
    UINT dummy;
    
    if (!SDcardMount) return;
    //�t�@�C������8�����܂ŃT�|�[�g
    res = f_open(&FileObject, "Open8.bmp", FA_READ);    //DHT20�ɕ\�L�ύX
    if (res != FR_OK) return;
    
    //Open
    //�w�b�_�Ǎ�
    f_read(&FileObject, SDcardBuffer, 32, &actualLength); //32�o�C�g���ǂݍ���
    if (SDcardBuffer[bcBitCount] == 16) {
        //16bit�J���[�̃f�[�^�̎��̂ݏ���
        x = SDcardBuffer[bcWidth] + (SDcardBuffer[bcWidth+1]<<8);   //Width
        y = SDcardBuffer[bcHeight];                             //Height
        //�V�[�N���āA�f�[�^�̐擪�܂ňړ�
        f_lseek(&FileObject, SDcardBuffer[bfOffBits]);
        
        //bmp�f�[�^�́A�\������鉺�̍s����n�܂�
        //16bit�J���[��bmp�����Ɍ��肵�Ă���
        for (kk = 0; kk < y; kk++) {
            //DATASIZE���l�������v���O����
            num = (uint16_t)x * 2;  //1�s�̃o�C�g��=x�h�b�g x 16bit
            xx = 0;

            while (res == FR_OK && !f_eof(&FileObject)) {
                
                res = f_forward(&FileObject, outstream, 320, &dummy);
            }

//            while (num > DATASIZE) {
//                f_read(&FileObject, SDcardBuffer, DATASIZE, &actualLength); //�����h�b�g��x2�o�C�g���̃f�[�^��ǂݍ���
//                glcd_array(xx, y-1-kk, DATASIZE/2, 1, SDcardBuffer);
//                num -= DATASIZE;
//                xx = xx + DATASIZE/2;
//            }
//            f_read(&FileObject, SDcardBuffer, num, &actualLength); //�����h�b�g��x2�o�C�g���̃f�[�^��ǂݍ���
//            glcd_array(xx, y-1-kk, num/2, 1, SDcardBuffer+100*0);
        }
    }
    f_close(&FileObject);
}
*/

const char TextOpenTitle[] = "LCD Clock 2022 v1.2";
const char *TextOpenMsg[] = {
    "PIC: 18F27K42 128KB\r\n",
    "LCD: 2.8inch 320x240dot, 16bit color+Touch\r\n",
    "RTC: RX-8025NB\r\n",
    "Temp and Humidity Sensor: DHT20\r\n",
    "GPS unit: GYSFDMAXB\r\n",
};
const char TextOpenMsg6[] = "\nAccessing SD card...";

/*
 * waitTouch=0: �\���I����A�����I�ɏ����I��点��ꍇ
 *           1: �^�b�`�����玟�̏����Ɉڍs������
 */
void OpeningScreen(uint8_t waitTouch) {
    char str[100];
    int8_t jj;
    int16_t x, y, w, h;
    //�J���[�o�[�Ɠ������A75%�̔��A���A�V�A���A�΁A�}�[���^�A�ԁA��
    uint16_t color[] = {
        0xBDF7, //��
        0xBDE0, //��
        0x05F7, //�V�A��
        0x05E0, //��
        0xB817, //�}�[���^
        0xB800, //��
        0x0017, //��
    };

    lcd_fill(BLACK); //��ʂ��N���A
    SetFont(FontNormal);
    display_setTextColor(WHITE, BLACK); //Textcolr��TextbgColor��ݒ�
    display_setTextSize(FontMagx12);
    
    display_setCursor(0, 0);
    strcpy(str, TextOpenTitle);
    display_puts(str);
    
    display_setTextPitch(-1, 2);    //�s�b�`�ύX

    display_setCursor(0, 30);
    for (jj = 0; jj < 5; jj++) {
        strcpy(str, TextOpenMsg[jj]);
        display_puts(str);
    }
    
    display_setTextPitch(0, 0); //���ɖ߂�

    if (SDcardMount) {
        //SD�J�[�h������΁Abmp��\������
        strcpy(str, TextOpenMsg6);
        display_puts(str);
        ReadBmp16();
    }

    //�J���[�o�[�\��
    x = 0;
    y = 200;
    w = 40;
    h = 20;
    for (jj=0; jj<7; jj++) {
        display_fillRect(x, y, w, h, color[jj]);
        x += w;
    }

    if (waitTouch) {
        //�^�b�`������A�I������Ƃ������b�Z�[�W�o��
        display_drawChars(20, 221, (char*)TouchMsg, WHITE, WHITE, FontMagx12);
    }
    else {
        for (jj=0; jj<2; jj++) __delay_ms(500);
    }
}

/*
                         Main application
 */
void main(void)
{
    uint8_t jj, kk, dd;
    uint16_t addr;
    char str[100];

    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    //�A���[���̏�����
    AlarmInitialize();  //�A���[������TMR0�̏����ݒ�
    
    // ���[�^���[�pTMR1�̊��荞�ݐݒ�
    TMR1_SetInterruptHandler(RotaryHandler);

    //Touch/SlideSW/GPS�́ATMR5��10ms���荞�݂ŏ�ԃ`�F�b�N
    TMR5_SetInterruptHandler(Timer5Handler);
    TMR5_StartTimer();
    
    //LCD�̏�����
    lcd_init();
    lcd_fill(DARK_BLUE); //��ʂ��N���A

    SetFont(FontNormal);  //�����t�H���g�ݒ�

    //���߂ċN���������́A�S�������A2��ڈȍ~��EEPROM����f�[�^�擾
    if (DATAEE_ReadByte(AddressInit) == 0xff) {
        TouchAdjust();    //�^�b�`�̒��������{���A���̃f�[�^��EEPROM�ɕێ�
      
        DATAEE_WriteByte(AddressInit, 0x55);
        //�A���[��������������
        addr = AddressAlarm;
        for (jj=0; jj<3; jj++) {
            DATAEE_WriteByte(addr++, AlarmTime[jj]);
        }
        DisplayMode = DisplayMode3; //�ŏ��̕\�����[�h�́A�O���f�[�^�s�v��3�ɕύX
        DATAEE_WriteByte(AddressDisplayMode, DisplayMode);
        //�J�X�^���f�[�^�̏�����
        CopyCustomDataToTemp();
        SetCustomData();    //EEPROM�ɂ���������2��ڈȍ~�̋N�����ɓǂݏo���f�[�^������悤�ɂ���
        
    } else {
        //2��ڈȍ~�̓d�������Ȃ�A�^�b�`���W�␳�̃f�[�^��ǂݏo��
        addr = AddressTouch;
        T_x1 = DATAEE_ReadWord(addr);
        T_y1 = DATAEE_ReadWord(addr+2);
        T_x2 = DATAEE_ReadWord(addr+4);
        T_y2 = DATAEE_ReadWord(addr+6);
        
//        sprintf(str, "P1=(%d, %d) P2=(%d, %d)", T_x1, T_y1, T_x2, T_y2);
//        display_drawChars(0, 140, str, WHITE, TextbgColor, FontMagx11);
        
        //���t�A�A���[���ݒ��ǂݏo��
        addr = AddressYMD;
        for (jj=0; jj<4; jj++) {
            dd = DATAEE_ReadByte(addr++);
            if (dd != 0xff) DateTime[jj+3] = dd;    //�K�؂ȏ������݂�����Ȃ������ꍇ�����
        }
        addr = AddressAlarm;
        for (jj=0; jj<3; jj++) {
            AlarmTime[jj] = DATAEE_ReadByte(addr++);
        }
        //�O��I�����̕\�����[�h�ɂ���
        DisplayMode = DATAEE_ReadByte(AddressDisplayMode);
        
        //�J�X�^���ݒ��ǂݏo��
        for (jj=0; jj < OBJECTNUM; jj++) {
            addr = AddressCustom + (uint16_t)jj*32;
            CustomRsc[jj].disp = DATAEE_ReadByte(addr++);
            CustomRsc[jj].x = (int16_t)DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].y = (int16_t)DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].xw = (int16_t)DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].yw = (int16_t)DATAEE_ReadWord(addr);
            addr += 2;
            
            CustomRsc[jj].fontcode = DATAEE_ReadByte(addr++);
            CustomRsc[jj].fontmag = DATAEE_ReadByte(addr++);

            CustomRsc[jj].color = DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].format = DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].attribute = DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].attribute2 = DATAEE_ReadWord(addr);
            addr += 2;
            
            CustomRsc[jj].xpitchAdj = (int8_t)DATAEE_ReadByte(addr++);
        }
        for (jj = 0; jj < NumberOfUserFont; jj++) {
            for (kk = 0; kk < 9; kk++) {
                CustomFontDir[jj][kk] = DATAEE_ReadByte(addr++);
            }
        }    
    }
    
    //GPS�́A�d��������A1PPS�������Ƃ��m�F��AGPS��M������J�n����
    Count1PPS = 0;
    
    //�N����
    AlarmSoundOn(AlarmTwice);

    lcd_fill(BLACK); //��ʂ��N���A

    //�����ŁASD�J�[�h���}�E���g�B�J�[�h���Ȃ��Ă����Ȃ��̂ŃR�����g��
//    sprintf(str, "Trying to mount SD card...");
//    display_drawChars(5, 40, str, WHITE, TextbgColor, FontMagx12);
    remount();

//    if (SDcardMount == 1) sprintf(str, "Found SD card");
//    else sprintf(str, "No SD card");
//    display_drawChars(5, 60, str, WHITE, TextbgColor, FontMagx12);
    
//    f_mount(0,"0:",0);  //unmount disk�@�@���󂱂�͎��s���Ă͂����Ȃ�

    //�I�[�v�j���O��ʂ�\��
    OpeningScreen(0);

    // RTC�̏�����
    //�����́A�悭�킩��Ȃ����ASYSTEM_Initialize���AIOCB���L��������A��̃n���h���̏�Ԃ��ƁA�Ȃ���
    //�n���O����B����InterruptEnable�̑O�ɂ��Ȃ��ƃn���O����B���ꂪ�킩�炸�����Y��
    //IOCB4=0�ɂ��邩�A�n���h���ݒ�(���g���Ȃ��ƃf�t�H���g�Ɠ������_��)������K�v����
    init_RTC(DateTime);
    INT0_SetInterruptHandler(RTC_handler);  //RTC_INTA (1s��)�̊��荞�ݏ���
    IOCBF4_SetInterruptHandler(AlarmWHandler); //�A���[�� Alarm W�̊��荞��

    //�C�x���g�o�b�t�@�̏�����
    ClearBuffer();
    
    // �d����������̍ŏ��̃C�x���g�́A�t�H�[���̏�����
    //DisplayMode�͏�L�Őݒ�ς�
    SetEventHandler(MainEventHandler);
    PushEvent(EventFormInit);
    
    while (1) {
        // Add your application code
        
        //�����݂̃C�x���g����
        if (TouchStart & 0x01) {
            if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) {
                //�����Ń^�b�`���W���擾���āA�C�x���g����
                TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                PushEvent(EventTouchDown);
                //�^�b�`������s�b�Ƃ��������o��
                StopSoundFlag = 0;  //
                AlarmSoundOn(AlarmSingle);
            }
            TouchStart &= 0xfe;
        } else if (TouchStart & 0x02) {
            //������
            if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) { //�����ł����W�擾
                TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                PushEvent(EventTouchLong);
            }
            TouchStart &= 0xfd;
        } else if (TouchStart & 0x04) {
            //���s�[�g���L�����A300ms���Ƀ^�b�`�_�E���C�x���g�𔭐�������
            if (RepeatKey > 30) {
                if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) { //�����ł����W�擾
                    TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                    //���s�[�g���L�����A300ms���Ƀ��s�[�g�C�x���g�𔭐�������
                    PushEvent(EventTouchRepeat);
                    RepeatKey = 1;  //0�ł̓��s�[�gdisable�ɂȂ�̂ŁA1�Ƀ��Z�b�g
                }
            }
            TouchStart &= 0xfb;
        } else if (TouchStart & 0x08) {
            //�^�b�`�A�b�v�@�@���W�̓^�b�`�_�E�����̏����Ŏ擾
            PushEvent(EventTouchUp);
            RepeatKey = 0;  //���s�[�g����
            TouchStart = 0;
        }
        
        //�A���[��
        if (AlarmStatus == 1) {
            AlarmSoundOn(AlarmSound);    //0�Ԃ̉�=�A���[���̍ŏ��̉���炷
            AlarmStatus++;
        }
        
        //�X���C�hSW
        if (SlideSWStatus-1 == SlideSWoff) {
            PushEvent(EventSlideSWoff);
            SlideSWStatus--;
        } else if (SlideSWStatus-1 == SlideSWon) {
            PushEvent(EventSlideSWon);
            SlideSWStatus--;
        }

        //���[�^���[
        if (RotaryFlag) {
            PushEvent(EventRotary);
            RotaryFlag = 0;
        }

        //RTC������
        if (RTC1sFlag) {
            PushEvent(EventTimeUpdate);
            RTC1sFlag = 0;
        }

        //GPS�C�x���g
        if (GPSFlag) {
            PushEvent(EventStartGPS);
            GPSFlag = 0;
        }

        //�T�E���h�t���O����
        if (StopSoundFlag) {
            AlarmSoundOff();
            StopSoundFlag = 0;
        }
        
        EventHandler();
        
#ifdef DEBUG
        char str[100];
        sprintf(str, "%02d,%02d,%02d", ContNum, PushPoint, PopPoint);
        display_drawChars(250, 30, str, WHITE, TextbgColor, FontMagx11);
#endif

    }
}
/**
 End of File
*/