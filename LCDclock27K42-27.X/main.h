/* 
 * File:   main.h
 * Author: K.Ohno
 *
 * Created on January 3, 2021, 12:33 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

//#define DEBUG
//#define ENGLISH

#include "mcc_generated_files/mcc.h"
    
//EEPROM���f�[�^�z�u(0x00-0x3ff)�@�@�d����������A��������ݒ���ǂݏo��
#define AddressInit         0x00    //1�o�C�g�@�@�����lFF�A�������񂾂�55
#define AddressDisplayMode  0x07    //1�o�C�g
//���t
#define AddressYMD          0x08    //4�o�C�g�@�j���A���A���A�N�̏�
#define AddressAlarm        0x0c    //3�o�C�g�@���A���A�j���̏�
//#define AddressAlarmNo      0x0f     //1�o�C�g�@�@�A���[���p�^�[���̕ύX�p
#define AddressTouch        0x18    //8�o�C�g�@�@�^�b�`�␳�p��2�����̍��W
#define AddressCustom       0x20    //�J�X�^������ێ��B����21�o�C�g/object�K�v(32�o�C�g�m�ۂ��Ă���)�@*7�I�u�W�F�N�g 0x20-0xff

#define hi(a) ((a) >> 8)
#define lo(a) ((a) & 0xff)

//�\���ʒu�̃��\�[�X  X,Y���W�ƁAX/Y�T�C�Y
#define TimeX       20
#define TimeY       35
#define TimeXw      (4*20+6+2)*2
#define TimeYw      27*2+4

#define YearX       30
#define YearY       10
#define YearXw      5*8*2     //4+1��x8�h�b�g��2�{�T�C�Y
#define YearYw      8*2+2   //8�h�b�g��2�{+2�h�b�g
    
#define AlarmX      50
#define AlarmY      120
#define AlarmXw     100
#define AlarmYw     8*2+2

#define TempX       230
#define TempY       100
#define TempXw      50
#define TempYw      15

#define HumidityX   240
#define HumidityY   120
#define HumidityXw  50
#define HumidityYw  15

#define CalendarX    10
#define CalendarY   155
#define CalendarXw  100
#define CalendarYw   80
#define CalendarXstep  100

//�A�i���O���v�̍��W�́A���v�̍���̍��W
#define AnalogClockX    10
#define AnalogClockY    25
#define AnalogClockR    180 //���a�ɕύX

//���ԃ{�^��
#define GearX   280
#define GearY   0
#define GearXw  32
#define GearYw  32


//BMP�w�b�_
#define bfOffBits   0x0a        //�t�@�C���擪����摜�f�[�^�܂ł̃I�t�Z�b�g (byte)
#define bcWidth     0x12        //�s�N�Z���� (�T�C�Y4byte)
#define bcHeight    0x16        //�s�N�Z������ (�T�C�Y4byte)
#define bcBitCount  0x1C        //1��f������̃f�[�^�T�C�Y (bit)

//�\�����[�h
typedef enum {
    DisplayMode1,       //0x00    //���v�\���傫���A3�����J�����_�[�\��
    DisplayMode2,       //0x01    //�J�����_�[�傫���\��
    DisplayMode3,       //0x02    //�A�i���O���v�\��
    DisplayCustom,      //0x04    //�J�X�^���\��
    MODENUM,            //        //���[�h�̎�ނ̐��������I�ɐݒ肳���
} DispMode;


/* �t�H���g�R�[�h: Font�Ŏn�܂閼�̂Œ�`
 * FontFolder�Ə������킹��
 * FontCode�́A�t�H���g�ԍ��ŁA�����t�H���g�Ƃ�1:1�Ή����Ă��邪�A
 * �J�X�^���t�H���g�́A���ꂼ��̃I�u�W�F�N�g��1:1�Ή����Ă��āA���ۂ̃t�H���g�̃f�[�^�Ƃ�
 * �R�Â���FontFolder�ōs���Ă���Ƃ����̂��኱��₱�����B
 * �J�X�^���t�H���g���ɌŗL�̃t�H���g�R�[�h(1�o�C�g)�t���Ă��悢���A�Ǘ����ʓ|
 */
#define InternalFontNum     3
enum FontCode {
    FontNormal,     //8x8
    FontSmall,      //6x8
    Font7like,      //6x8 7�Z�O���ǂ������t�H���g
    Font7seg,        //7�Z�O
    Font7segSmall,   //7�Z�O��
    FontUser0,       //�J�X�^���t�H���g for DateObj
    FontUser1,       //�J�X�^���t�H���g for TimeObj
    FontUser2,       //�J�X�^���t�H���g for AlarmObj
    FontUser3,       //�J�X�^���t�H���g for TempObj
    FontUser4,       //�J�X�^���t�H���g for HumidityObj
    FontUser5,       //�J�X�^���t�H���g for CalendarObj
    FontTemp,       //�ݒ蒆�̉��̃t�H���g���i�[
    MAXFontCode,
};


//���t�\���̌`���A�t�H�[�}�b�g���Ƃ��Ďg��
enum DateFormat {
    YYYYMMDDw,  //�j���t��
    YYYYMMDDwe,  //�p��j���t��
    YYYYMMDD,
    YYMMDDw,
    YYMMDDwe,
    YYMMDD,
    MMDDw,
    MMDDwe,
    MMDD,
    MAXDateFormat,  //�v�f��
};

enum TimeFormat {
    HHMM,
    HHMMss,
    HHMMSS,
    ANALOGCLOCK,
    MAXTimeFormat,  //�v�f��
};

enum AnalogClockFormat {
    Large,
    Mid_L,
    Mid_S,
    Small,
    Tiny,
    MAXAnalogSize,  //�v�f��
};

enum AlarmFormat {
    ALMAMPM,  //AM/PM�\�L
    ALM24,    //24���ԕ\�L
    MAXAlarmFormat, //�v�f��
};

//���x�\���̌`��
enum TempFormat {
    DEGC,
    DEGF,
    MAXTempFormat,  //�v�f��
};

//�J�����_�[�̕\��
enum CalendarFormat {
    Month1,
    Month2,
    Month2v,
    Month3,
    Month3v,
    MAXCalendarFormat,  //�v�f��
};


//�I�u�W�F�N�g��\�����邩�ǂ���
enum {
    Disable,
    Enable,
};

//���C����ʂ̃��C�A�E�g���w�肷�郊�\�[�X
typedef struct {
    uint8_t RscID;  //���\�[�XID �s�v�����������@�J�X�^�}�C�Y�ŁAenable���Ƃ��Ďg��
    int16_t x;     //�\���ʒu�̍���̍��W
    int16_t y;
    int16_t xw;    //�I�u�W�F�N�g�̃T�C�Y
    int16_t yw;
    uint8_t fontcode;    //�t�H���g�w��F�t�H���g�����i�[���Ă���z���̔ԍ�
    uint8_t fontmag;   //�\�����鎞�A�t�H���g�̔{��
    char *str;          //������ւ̃|�C���^
    uint16_t color;
    uint16_t format;    //�����\���̃t�H�[�}�b�g��HH:MM��HH:MM:ss���Ƃ��AYYYY/MM/DD��YY/MM/DD��MM/DD�A
                        //�J�����_�[�̕\��(���J�����_�A��J�����_�A3�������J�����_)�A�Ȃǂ������Ŏw��
    uint16_t attribute; //���炩�̒ǉ���� Alarm��Off���̐F�A�J�����_�[�̓y�j���̐F�A�A�i���O���v�̑傫��
    uint16_t attribute2; //���炩�̒ǉ���� �J�����_�[�̓��j�̐F
    int8_t xpitchAdj;     //�t�H���g�`�掞��X�s�b�`�̒����ʂ��w��B�@0�Ȃ�t�H���g�̕��ɏ]��    
//    uint16_t ypitch;    //��{�I��1�s�\���Ȃ̂ŕs�v�Ȃ̂ŁA�������ߖ�̂��߂ɂ��A�R�����g��
} MainFormRsc;

typedef struct {
    uint8_t disp;       //�\�������邩�ǂ���
    int16_t x, y;      //�ŏ��̕����̍���̍��W
    int16_t xw;     //�I�u�W�F�N�g��X�T�C�Y
    int16_t yw;     //�I�u�W�F�N�g��Y�T�C�Y
    uint8_t fontcode;    //�t�H���g���w�肷��ԍ��@FontCode�Œ�`����Ă��鐔�l
    uint8_t fontmag;   //�t�H���g�T�C�Y attribute�ɑ���������
    uint16_t color;     //�F
    uint16_t format;     //�\���`��
    uint16_t attribute;  //���炩�̒ǉ����@Saturday color
    uint16_t attribute2; //�ǉ����@Sunday color
    int8_t xpitchAdj;    //�t�H���g��X�s�b�`�̒�����
//    uint8_t ypitch;    //�t�H���g��Y�s�b�`�@��{�I��1�s�\���Ȃ̂ŁA�s�v
} ObjectRsc;


//���L�I�u�W�F�N�g�̏��Ƀ��\�[�X��ݒ肷��
enum RscObject {
    DateObj,        //0
    TimeObj,        //1
    AlarmObj,       //2
    TempObj,        //3
    HumidityObj,    //4
    CalendarObj,    //5
    GearObj,        //6
    OBJECTNUM,      //��`����Ă���I�u�W�F�N�g�̐�
};


#define NoData      9999     //�t�H�[�}�b�g���������Ȃ����Ƃ������BGear�Ƃ�Humidity�Ƃ�
#define NoData8     0x7f        //8bit�ϐ��p

extern ObjectRsc CustomRsc[];
extern MainFormRsc *CurrentForm;   //���݂̉�ʃt�H�[��

extern ObjectRsc *RscData[];


//�C�x���g�̔ԍ�
enum EventNum {
    EventNone,      //=0
    EventTimeUpdate,    //1�b���Ƃɔ���  �����̍X�V����������
    EventSlideSWoff,
    EventSlideSWon,
    EventAlarm,   //�����݂ŉ��炷�̂ŁA�C�x���g�Ƃ��Ă͕s�v�����[�^���[�񂵂ăA���[�������ύX�������ɔ���
    EventTouchDown, //�ŏ��Ƀ^�b�`�ƔF���������ɔ�������C�x���g
    EventTouchUp,   //�^�b�`����߂����ɔ�������C�x���g�@Long���ǂ����ŏ����ύX����Ȃ炱�����H
    EventTouchLong,
    EventTouchRepeat,   //�������ςȂ��̎�
    EventRotary,
    EventFormInit,
    EventFormUpdate,
    EventStartGPS,
    EventDateUpdate,    //���t�ύX���@    ���t�̕\���X�V�K�v�Ȏ��B�J�����_�[�̕\����
//    EventAlarmUpdate,   //�A���[�������ύX��
    EventTempUpdate,        //���x�̍X�V��������
    EventHumidityUpdate,    //���x�̍X�V��������
    EventCalendarUpate,     //�J�����_�[�@�K�v���邩�H
    EventColorFormInit,
    EventColorFormUpdate,
//    EventGotGPSdata,
    EventXYFormInit,
    
    EventFormInitSave,  //�J�X�^���f�[�^�̕ۑ��A�ǂݏo���p
    EventFormInitLoad,
    EventFormClose,
    
        
};

//RTC�����p
//Sec, Min, Hour, Wday, Day, Month, Year��z��
//�l�́ABCD10�i�Ƃ��邱�Ƃɒ��ӁB�擪�́ASec�̃��W�X�^�A�h���X
//�����l:2021/1/1 00:00:00
// ���т́ARTC�ƍ��킹�āA�b�A���A���A�j���A���A���A�N
extern uint8_t DateTime[7]; // = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x21};
extern uint8_t AlarmTime[3]; // = {0, 0, 0x7f};    //mm, hh, wday
extern uint8_t RotaryFlag;      //���[�^���[�C�x���g���s�t���O
extern uint8_t RTC1sFlag;       //1�b���Ƃ̊����݃C�x���g���s�t���O
extern uint8_t GPSFlag;         //GPS�C�x���g���s�t���O

extern const char TouchMsg[];
extern uint8_t SDcardMount;    //�}�E���g�ł�����1
//SD�J�[�h����f�[�^��ǂݏo�����Ɏg���o�b�t�@
#define DATASIZE 320    //LCD��320�h�b�g�����C�����@40�o�C�g�̔{�����]�܂���
extern uint8_t SDcardBuffer[DATASIZE];    //���[�J���ł͓��肫��Ȃ��Ă��AData space�ɂ͂܂�������

extern int8_t Accel;   //������]���������ɁA1�N���b�N��Accel�{�ɂ�����
extern uint8_t RotaryEventCount;
extern uint8_t RotaryFlag;      //���[�^���[�C�x���g���s�t���O
extern int16_t TouchX, TouchY;    //�f�B�v���C���W�Ɋ��Z�����^�b�`���W
extern uint8_t TouchCount;     //����������p�Ɏg�p�B�^�b�`�����0�ɂȂ�
extern uint8_t TouchStart;
extern uint16_t PixelData[9];   //3x3�̃s�N�Z���f�[�^��ۑ�����
extern int16_t PixelXY[2];
extern int8_t RotCount; //�񂵂���(�N���b�N����̃��[�^���[���ƁA����N���b�N��������)
extern uint8_t SlideSWStatus;   //�����݂Ńt���O�Ƃ��Ďg���A�������ꂽ���Ԏw��ɂ��g��
//��̓I�ɂ́Aon�ɕω��������ASlideSWon+1�Ƃ����ݒ�ɂ��āA�������ꂽ��-1���āASlideSWon�ƕω�������
#define SlideSWoff  0
#define SlideSWon   8   // SlideSWoff�ƊԂ��󂯂Ă���

extern uint8_t RepeatKey;


void SetEventHandler(void (* Handler)(void));
void PopupEventHandler(void (* Handler)(void));
void ReturnEventHandler(void);
uint8_t PushEvent(uint8_t ev);
uint8_t PopEvent(void);
void ClearBuffer(void);
void MainEventHandler(void);
int8_t ButtonPush(int16_t x, int16_t y, MainFormRsc *objrsc);

uint8_t WriteYMD(void);


/*
 * (20,20)  (300,220)�̃^�b�`��̍��W��T_x1, T_y1, T_x2, T_y2�ɕێ����āA
 * LCD�̍��W�ɍ����悤�␳����
 */
void TransCoordination(uint16_t x, uint16_t y, int16_t *xg, int16_t *yg);
void remount(void);
void DATAEE_WriteWord(uint16_t bAdd, uint16_t bData);
uint16_t DATAEE_ReadWord(uint16_t bAdd);
void TouchAdjust(void);

void OpeningScreen(uint8_t waitTouch);


#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */
