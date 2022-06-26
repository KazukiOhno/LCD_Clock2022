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
    
//EEPROM内データ配置(0x00-0x3ff)　　電源落ちた後、ここから設定情報読み出す
#define AddressInit         0x00    //1バイト　　初期値FF、書き込んだら55
#define AddressDisplayMode  0x07    //1バイト
//日付
#define AddressYMD          0x08    //4バイト　曜日、日、月、年の順
#define AddressAlarm        0x0c    //3バイト　分、時、曜日の順
//#define AddressAlarmNo      0x0f     //1バイト　　アラームパターンの変更用
#define AddressTouch        0x18    //8バイト　　タッチ補正用の2か所の座標
#define AddressCustom       0x20    //カスタム情報を保持。現状21バイト/object必要(32バイト確保しておく)　*7オブジェクト 0x20-0xff

#define hi(a) ((a) >> 8)
#define lo(a) ((a) & 0xff)

//表示位置のリソース  X,Y座標と、X/Yサイズ
#define TimeX       20
#define TimeY       35
#define TimeXw      (4*20+6+2)*2
#define TimeYw      27*2+4

#define YearX       30
#define YearY       10
#define YearXw      5*8*2     //4+1桁x8ドットの2倍サイズ
#define YearYw      8*2+2   //8ドットの2倍+2ドット
    
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

//アナログ時計の座標は、時計の左上の座標
#define AnalogClockX    10
#define AnalogClockY    25
#define AnalogClockR    180 //直径に変更

//歯車ボタン
#define GearX   280
#define GearY   0
#define GearXw  32
#define GearYw  32


//BMPヘッダ
#define bfOffBits   0x0a        //ファイル先頭から画像データまでのオフセット (byte)
#define bcWidth     0x12        //ピクセル幅 (サイズ4byte)
#define bcHeight    0x16        //ピクセル高さ (サイズ4byte)
#define bcBitCount  0x1C        //1画素あたりのデータサイズ (bit)

//表示モード
typedef enum {
    DisplayMode1,       //0x00    //時計表示大きく、3か月カレンダー表示
    DisplayMode2,       //0x01    //カレンダー大きい表示
    DisplayMode3,       //0x02    //アナログ時計表示
    DisplayCustom,      //0x04    //カスタム表示
    MODENUM,            //        //モードの種類の数が自動的に設定される
} DispMode;


/* フォントコード: Fontで始まる名称で定義
 * FontFolderと順序合わせて
 * FontCodeは、フォント番号で、内蔵フォントとは1:1対応しているが、
 * カスタムフォントは、それぞれのオブジェクトと1:1対応していて、実際のフォントのデータとの
 * 紐づけはFontFolderで行っているというのが若干ややこしい。
 * カスタムフォント毎に固有のフォントコード(1バイト)付けてもよいが、管理が面倒
 */
#define InternalFontNum     3
enum FontCode {
    FontNormal,     //8x8
    FontSmall,      //6x8
    Font7like,      //6x8 7セグもどき内蔵フォント
    Font7seg,        //7セグ
    Font7segSmall,   //7セグ小
    FontUser0,       //カスタムフォント for DateObj
    FontUser1,       //カスタムフォント for TimeObj
    FontUser2,       //カスタムフォント for AlarmObj
    FontUser3,       //カスタムフォント for TempObj
    FontUser4,       //カスタムフォント for HumidityObj
    FontUser5,       //カスタムフォント for CalendarObj
    FontTemp,       //設定中の仮のフォントを格納
    MAXFontCode,
};


//日付表示の形式、フォーマット情報として使う
enum DateFormat {
    YYYYMMDDw,  //曜日付き
    YYYYMMDDwe,  //英語曜日付き
    YYYYMMDD,
    YYMMDDw,
    YYMMDDwe,
    YYMMDD,
    MMDDw,
    MMDDwe,
    MMDD,
    MAXDateFormat,  //要素数
};

enum TimeFormat {
    HHMM,
    HHMMss,
    HHMMSS,
    ANALOGCLOCK,
    MAXTimeFormat,  //要素数
};

enum AnalogClockFormat {
    Large,
    Mid_L,
    Mid_S,
    Small,
    Tiny,
    MAXAnalogSize,  //要素数
};

enum AlarmFormat {
    ALMAMPM,  //AM/PM表記
    ALM24,    //24時間表記
    MAXAlarmFormat, //要素数
};

//温度表示の形式
enum TempFormat {
    DEGC,
    DEGF,
    MAXTempFormat,  //要素数
};

//カレンダーの表示
enum CalendarFormat {
    Month1,
    Month2,
    Month2v,
    Month3,
    Month3v,
    MAXCalendarFormat,  //要素数
};


//オブジェクトを表示するかどうか
enum {
    Disable,
    Enable,
};

//メイン画面のレイアウトを指定するリソース
typedef struct {
    uint8_t RscID;  //リソースID 不要だったかも　カスタマイズで、enable情報として使う
    int16_t x;     //表示位置の左上の座標
    int16_t y;
    int16_t xw;    //オブジェクトのサイズ
    int16_t yw;
    uint8_t fontcode;    //フォント指定：フォント名を格納している配列上の番号
    uint8_t fontmag;   //表示する時、フォントの倍率
    char *str;          //文字列へのポインタ
    uint16_t color;
    uint16_t format;    //時刻表示のフォーマットをHH:MMかHH:MM:ssかとか、YYYY/MM/DDかYY/MM/DDかMM/DD、
                        //カレンダーの表示(小カレンダ、大カレンダ、3か月小カレンダ)、などをここで指定
    uint16_t attribute; //何らかの追加情報 AlarmのOff時の色、カレンダーの土曜日の色、アナログ時計の大きさ
    uint16_t attribute2; //何らかの追加情報 カレンダーの日曜の色
    int8_t xpitchAdj;     //フォント描画時のXピッチの調整量を指定。　0ならフォントの幅に従う    
//    uint16_t ypitch;    //基本的に1行表示なので不要なので、メモリ節約のためにも、コメント化
} MainFormRsc;

typedef struct {
    uint8_t disp;       //表示させるかどうか
    int16_t x, y;      //最初の文字の左上の座標
    int16_t xw;     //オブジェクトのXサイズ
    int16_t yw;     //オブジェクトのYサイズ
    uint8_t fontcode;    //フォントを指定する番号　FontCodeで定義されている数値
    uint8_t fontmag;   //フォントサイズ attributeに相当する情報
    uint16_t color;     //色
    uint16_t format;     //表示形式
    uint16_t attribute;  //何らかの追加情報　Saturday color
    uint16_t attribute2; //追加情報　Sunday color
    int8_t xpitchAdj;    //フォントのXピッチの調整量
//    uint8_t ypitch;    //フォントのYピッチ　基本的に1行表示なので、不要
} ObjectRsc;


//下記オブジェクトの順にリソースを設定する
enum RscObject {
    DateObj,        //0
    TimeObj,        //1
    AlarmObj,       //2
    TempObj,        //3
    HumidityObj,    //4
    CalendarObj,    //5
    GearObj,        //6
    OBJECTNUM,      //定義されているオブジェクトの数
};


#define NoData      9999     //フォーマット情報を持たないことを示す。GearとかHumidityとか
#define NoData8     0x7f        //8bit変数用

extern ObjectRsc CustomRsc[];
extern MainFormRsc *CurrentForm;   //現在の画面フォーム

extern ObjectRsc *RscData[];


//イベントの番号
enum EventNum {
    EventNone,      //=0
    EventTimeUpdate,    //1秒ごとに発生  時刻の更新があったら
    EventSlideSWoff,
    EventSlideSWon,
    EventAlarm,   //割込みで音鳴らすので、イベントとしては不要→ロータリー回してアラーム時刻変更した時に発生
    EventTouchDown, //最初にタッチと認識した時に発生するイベント
    EventTouchUp,   //タッチをやめた時に発生するイベント　Longかどうかで処理変更するならここか？
    EventTouchLong,
    EventTouchRepeat,   //押しっぱなしの時
    EventRotary,
    EventFormInit,
    EventFormUpdate,
    EventStartGPS,
    EventDateUpdate,    //日付変更時　    日付の表示更新必要な時。カレンダーの表示も
//    EventAlarmUpdate,   //アラーム時刻変更時
    EventTempUpdate,        //温度の更新あったら
    EventHumidityUpdate,    //湿度の更新あったら
    EventCalendarUpate,     //カレンダー　必要あるか？
    EventColorFormInit,
    EventColorFormUpdate,
//    EventGotGPSdata,
    EventXYFormInit,
    
    EventFormInitSave,  //カスタムデータの保存、読み出し用
    EventFormInitLoad,
    EventFormClose,
    
        
};

//RTC処理用
//Sec, Min, Hour, Wday, Day, Month, Yearを配列化
//値は、BCD10進とすることに注意。先頭は、Secのレジスタアドレス
//初期値:2021/1/1 00:00:00
// 並びは、RTCと合わせて、秒、分、時、曜日、日、月、年
extern uint8_t DateTime[7]; // = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x21};
extern uint8_t AlarmTime[3]; // = {0, 0, 0x7f};    //mm, hh, wday
extern uint8_t RotaryFlag;      //ロータリーイベント発行フラグ
extern uint8_t RTC1sFlag;       //1秒ごとの割込みイベント発行フラグ
extern uint8_t GPSFlag;         //GPSイベント発行フラグ

extern const char TouchMsg[];
extern uint8_t SDcardMount;    //マウントできたら1
//SDカードからデータを読み出す時に使うバッファ
#define DATASIZE 320    //LCDの320ドット数ライン分　40バイトの倍数が望ましい
extern uint8_t SDcardBuffer[DATASIZE];    //ローカルでは入りきらなくても、Data spaceにはまだ入った

extern int8_t Accel;   //高速回転させた時に、1クリックをAccel倍にさせる
extern uint8_t RotaryEventCount;
extern uint8_t RotaryFlag;      //ロータリーイベント発行フラグ
extern int16_t TouchX, TouchY;    //ディプレイ座標に換算したタッチ座標
extern uint8_t TouchCount;     //長押し判定用に使用。タッチ直後に0になる
extern uint8_t TouchStart;
extern uint16_t PixelData[9];   //3x3のピクセルデータを保存する
extern int16_t PixelXY[2];
extern int8_t RotCount; //回した回数(クリックありのロータリーだと、何回クリックさせたか)
extern uint8_t SlideSWStatus;   //割込みでフラグとして使い、処理されたら状態指定にも使う
//具体的には、onに変化した時、SlideSWon+1という設定にして、処理されたら-1して、SlideSWonと変化させる
#define SlideSWoff  0
#define SlideSWon   8   // SlideSWoffと間を空けている

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
 * (20,20)  (300,220)のタッチ上の座標をT_x1, T_y1, T_x2, T_y2に保持して、
 * LCDの座標に合うよう補正する
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
