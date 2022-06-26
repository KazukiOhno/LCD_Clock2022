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
 * ピンアサインは、18F26K22と同一にすることができている
 * Peripheral Pin Select (PPS)の機能により、ピンアサインがかなり自由に設定可
 * 
 * TMR0: アラーム音の鳴っている時間を測定 10ms毎の割込み
 * TMR1: ロータリーエンコーダのサンプリング　500us毎の割込み
 * TMR2: 圧電スピーカ用に4KHzのECCP2 PWMを発生させるためのタイマ  250us
 * TMR3: ADCがCCP4モードで使用して、TMR3=4msを利用
 * TMR5: タッチ、スライドSW、GPSの状態チェックの割り込み用タイマ　10ms
 * TMR6: バックライトのPWM制御用タイマ 256us
 * 
 * SPIインタフェース：LCD、タッチ、SDカードの3つで共用
 * 　SPI1 (SCK1, SDI1, SDO1)+LCD_CS, T_CS, SDCard_CS
 *   　LCD_RESET, LCD_DCRS、T_IRQ
 *   LCDとは最大スピードの8MHz、タッチは2MHzで通信、SDカードは、低速400kHz、高速8MHz
 * SPIMASTERは、開いた時デフォルトで、MODE3になるので、MODE0に変更要
 * 修正しておかないと、Generateした後、動かないと騒ぐことになる
 * SDカードは、MODE3でもMODE0でもどちらも動作。ただし、18F27K42では、5.3MHzまで
 * LCDも、8Mhz動作せず、6.4Mhzまで　→　SLRCONCでスルーレートをmaxにして解決
 * 　　当初と同様8Mhz動作はできるようになった
 * SDSLOW=400kHz
 * SDFAST=8MHz
 * LCD8M=16MHz
 * Touch2M=1.5MHz
 * 
 * 照度センサ(PhotoDiode)は、アナログで電圧取り込み RA3=AN3=CCP4=PhotoDiode
 * 
 * LCDのバックライトはPWM制御を使って明るさ調整
 * 　　PWM6: バックライトのPWM (Timer6) PWM6=RC2
 * 　　照度センサの結果に基づき、PWM6_LoadDutyValue()で調整
 * 
 * I2Cインタフェース: RTCと温度センサで共用
 * 　I2C2 (SCL2、SDA2) 100KHz MFINTOSC
 * 　RTCは、/INTA, /INTB も使用
 *     INTA=RB0=INT0は、RTC基板上でpullup、1秒ごとに割り込み入る。EXT_INTで処理
 *     INTB=RB4は、PICでWPU、立下り割り込みでアラームを処理
 * 
 * ロータリーエンコーダ:RC0/RC1の2ピンのデジタルInputを使って判断
 * 　　RC0=ROT_B, RC1=ROT_A
 * 
 * アラームOn/Off用SW　　RA7=ALMSW
 * 圧電スピーカ: ECCP2=PWM (Timer2)　CCP2=RB3
 * GPSとのシリアル通信: RX1のみで非同期通信(UART1)  RX1=RC7
 * 　9600bps, 8bit, parityなし、Stop1bit
 * GPSの1PPS: SWと同様に10ms毎にチェックする処理で、RB5から取り込む
 * 
 * 動作確認用LED: RA6=LED
 * ピンが不足したので、RA6をSDCard_CSと共用
 * 
 * Revision:
 * Rev.4: 18F26K22と同等のプログラム。ただし、SPIの速度だけ遅い 
 * Rev.5: 機能追加。
 * 
 * Rev.10: プログラム動かなくなって、ハードも疑わしい時にここのプログラムを使うと良い。動作OKのものというHEXファイルがある
 * Rev.17: とりあえず、カスタマイズ以外はほぼ完ぺきにしたつもりのプログラム
 * Rev.18: カスタマイズ部の大修整
 * Rev.20: カスタマイズ部含め、ほぼ完成
 * Rev.21: SDFAST=4.5MHzとして、SDHCカードも使用できるようにした。コンパイラC90に変更→10%ほどバイナリ縮小
 *         Userフォントを6種持てるようにしたので、6つの部品全部に自由なフォントを持つことも可能にした
 * 　　　　カスタマイズも、ボタンが消えるのは毎回描画して対応、画面が消えていくのは運用で工夫を想定
 * 　　　　　何をタップしてポップアップしたのか部品名をポップアップに表示させた。
 * 　　プログラムサイズ=108146 byte (83%), Dataサイズ=7343 byte (90%)
 *     完成したといっても良いレベル
 * Rev.22: カスタマイズの操作性向上。設定ファイルを保存する時、79文字のコメントを入力できるようにした。ひらがな、カタカナにも対応
 * 　　　外部で、漢字入力しても表示は対応できるようにしている。
 * Rev.23: リソースでメモリを無駄に消費していたので、カスタマイズの画面リソースを1行毎1つにした。Dataが81%まで圧縮
 * 　　　　フォント名をfontname.txtに設定すると、ポップアップに反映されるように
 * Rev.24: フォントファイルをたくさん持てるように、オブジェクト毎にフォントを指定できるにした。
 * Rev.25: タッチのSPI=1.5MHz化により、データを取りこぼさないようになった
 * 　　　　SPIのスルーレートをmaxにより、8MHz以上の動作ができるように。
 * 　　　　割込み時、サブルーチンの呼び出しをやめ、フラグ化で、stack overflowとreentrant問題解決
 * Rev.26: ソースファイルの整理。コメント化してある古いプログラムの削除。コメントの更新など
 *        カスタマイズのタッチ処理もEvent化。保存時のメモの入力も最適化
 * 　　　Prog=125051, Data=6981
 * Rev.27: カスタマイズのフォント倍率の設定をもっと広げるため、別画面で、プレビューを見ながら設定できるように。
 * 　　　　　同時に、フォント、色、ピッチ調整、フォーマットも同一画面で設定できるように
 * 
 * 
 * 残る改善の余地としては、12/24時間制の切替、カレンダーの英語対応くらいか
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


//イベントキューの深さ
#define BufferMax 16
//Eventを配列にして、キューを持たせている。PushEventとPopEventで操作
uint8_t Event[BufferMax];  //イベント発生時にここにpushする
uint8_t PushPoint;
uint8_t PopPoint;
uint8_t ContNum;    //キューに入っているベント数
uint8_t RotaryEventCount = 0;   //キューに大量のRotaryイベントが入らないよう制限させるため、カウンタに使う

uint8_t RepeatKey = 0;  //リピートせるかどうか

//表示モード
uint8_t DisplayMode;    // どの画面表示モードか

//タッチ処理用
uint16_t TouchRawX, TouchRawY;    //タッチした時のX,Yの生座標
int16_t TouchX, TouchY;    //ディプレイ座標に換算したタッチ座標
//座標補正用に(20,20)と(300,220)のタッチ上の座標を保持
uint16_t T_x1, T_y1, T_x2, T_y2;    //補正用
uint8_t TouchCount;     //長押し判定用に使用。タッチ直後に0になる
uint16_t PixelData[9];  //3x3のピクセルデータを保存する
int16_t PixelXY[2];     //取り込んだデータの左上の座標
uint8_t TouchStart;     //タッチ開始フラグ    bit3:Up, 2:repeat, 1:long, 0:down

//スライドSW
uint8_t SlideSWStatus = 0x0f;   //初期値はどの状態でもない値を設定しておく

//GPS処理
#define STABLE1PPS  50  // 何回取得したら安定とみなすか
uint8_t Count1PPS;  //1PPS割り込みの発生回数をカウント。最大STABLE1PPSになる
//GPSは、電源投入時と、1日1回だけの取得
char Buffer[100];   //GPSとのシリアル通信用バッファ

//ロータリー処理用
int8_t RotCount = 0; //回した回数(クリックありのロータリーだと、何回クリックさせたか)
int8_t Accel = 1;   //高速回転させた時に、1クリックをAccel倍にさせる
uint8_t RotaryFlag = 0;     //ロータリーイベント発行フラグ

// 並びは、RTCと合わせて、秒、分、時、曜日、日、月、年　　BCDフォーマット
uint8_t DateTime[7] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x22};
int8_t TZ = +9; //現状タイムゾーンは、JSTに固定

//アラーム時刻、mm,hh, アラームが有効な曜日　RTCと同じ順序
uint8_t AlarmTime[3] = {0, 0, 0x7f};    //mm, hh, wday
uint8_t SmoothAlarmTime[3] = {0, 0, 0x7f};
uint8_t TmpTime[7]; //設定途中の時刻を保持
uint8_t RTC1sFlag = 0;  //1秒ごとの割込みイベント発行フラグ
uint8_t GPSFlag = 0;         //GPSイベント発行フラグ

uint8_t AlarmStatus = 0;    //アラーム鳴動中は1にして、smooth処理などを行う
uint8_t SmoothCount;    //何回スヌーズしたか

//温湿度センサの値を保持　実際の値の10倍にして整数化
int16_t Temp, Humidity;

//SDカード関係
FIL FileObject;       //Openingやデータ書き込み用
uint8_t SDcardMount = 0;    //マウントできたら1
FATFS Drive;
//SDカードからデータを読み出す時に使うバッファ
//#define DATASIZE 640
uint8_t SDcardBuffer[DATASIZE];    //ローカルでは入りきらなくても、ここではまだ入った

// mcc_generated_files/fatfs/ff_time.cを以下のように書き換えてタイムスタンプを正確に
/*
DWORD get_fattime (void)
{
    // 並びは、RTCと合わせて、秒、分、時、曜日、日、月、年
    uint8_t dateTime[7] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x21};

    RTC_read(dateTime);
    return decimalToFatTime(2000+Bcd2Hex(dateTime[6]), Bcd2Hex(dateTime[5]), Bcd2Hex(dateTime[4]), Bcd2Hex(dateTime[2]), Bcd2Hex(dateTime[1]), Bcd2Hex(dateTime[0]));
}
 */

//画面上に配置する部品の定義
ObjectRsc MainRsc1[] = {
    //ID, x, y,         xw, yw,         fontcode,   fontMag,    color, format, attribute, attribute2, xpitchAdj
    { 10, YearX, YearY, YearXw, YearYw, FontNormal, FontMagx22, WHITE, YYYYMMDDw, 0, 0, },
    { 20, TimeX, TimeY, TimeXw, TimeYw, Font7like,  0x77,       LIME,  HHMMss, 0, 0, 0, },    //間隔 2dot広げた
    { 50, AlarmX, AlarmY, 0,    0,      FontNormal, FontMagx22, WHITE, ALM24, GREY, },       //アラームoff時の色=GREY
    { 30, TempX, TempY,   0,    0,      FontNormal, FontMagx22, WHITE, DEGC, 0, },
    { 40, HumidityX, HumidityY, 0,0,    FontNormal, FontMagx22, WHITE, 0, 0, },
    { 60, CalendarX, CalendarY, CalendarXstep, CalendarYw, FontSmall, FontMagx11, WHITE, Month3, BLUE, RED, 1, },    //土日の色、間隔1dot広げた
    { 70, GearX, GearY, GearXw, GearYw, 0,          0,          LIGHTGREY, },    //歯車
};

ObjectRsc MainRsc2[] = {
    { 10, 0,  0, YearXw, YearYw,  FontNormal, FontMagx22,  WHITE, YYYYMMDDw, 0, 0, },
    { 20, 10, 18, TimeXw, TimeYw, FontNormal, FontMagx23,  LIME, HHMMSS, 0, 0, 2, },    //間隔 2dot広げた
    { 50, AlarmX+150, 40,0, 0,    FontNormal, FontMagx12,  WHITE, ALM24, GREY, },         //アラームoff時の色=GREY
    { 30, TempX, 0,  0,  0,       FontNormal, FontMagx22,  WHITE, DEGC, 0, },
    { 40, HumidityX, 17, 0, 0,    FontNormal, FontMagx22,  WHITE, 0, 0, },
    { 60, 25, 65, 320, 200,       FontNormal, FontMagx22,  WHITE, Month1, BLUE, RED, 3, },    //土日の色、間隔3dot広げた
    { 70, GearX, 200, GearXw, GearYw, 0,         0,        GREY, },    //歯車
};

ObjectRsc MainRsc3[] = {
    { 10, 0,  0, YearXw, YearYw, FontNormal, FontMagx22, WHITE, YYYYMMDDw, 0, 0, },
    { 20, AnalogClockX, AnalogClockY, AnalogClockR, AnalogClockR, 0, 0, GREY, ANALOGCLOCK, Large, }, //大きな時計
    { 50, 40, 218,       0,0,   FontNormal,  FontMagx13, WHITE, ALMAMPM, GREY, },         //アラームoff時の色=GREY
    { 30, TempX, 0,      0,0,   FontNormal,  FontMagx22, WHITE, DEGC, 0, },
    { 40, HumidityX, 20, 0,0,   FontNormal,  FontMagx21, WHITE, 0, 0, },
    { 60, 320-CalendarXstep, 40, CalendarXstep, CalendarYw, FontSmall, FontMagx11, WHITE, Month3v, BLUE, RED, 1, },
    { 70, GearX-97, 200, GearXw, GearYw, 0,     0,       GREY, },
};

//ユーザが表示方法をカスタマイズできるもの
ObjectRsc CustomRsc[] = {
    { Enable, YearX, YearY,    YearXw, YearYw,    FontNormal, FontMagx22, WHITE,     YYYYMMDDw, 0, 0, },
    { Enable, TimeX+10, TimeY,    TimeXw, TimeYw, Font7seg,   FontMagx11, ROYALBLUE, HHMMss, 90, 0, 2, },
    { Enable, AlarmX, AlarmY,       50,20,        FontNormal, FontMagx22, WHITE,     ALM24, GREY, },  //アラームのOn/Off両方設定？
    { Enable, TempX, TempY,         50,20,        FontNormal, FontMagx22, WHITE,     DEGC, 0, },
    { Enable, HumidityX, HumidityY, 50,20,        FontNormal, FontMagx22, WHITE,     0, 0, },
    { Enable, 5, CalendarY, CalendarXstep, CalendarYw, FontSmall, FontMagx11, WHITE, Month3, BLUE, RED, 2, },
    { 99, GearX, GearY, GearXw, GearYw,           0,          0,          LIGHTGREY, NoData, },    //色を変更
};

ObjectRsc *CurrentRsc;    //現在参照すべきリソース
MainFormRsc *CurrentForm;   //現在の画面フォーム

//DisplayModeの番号と下記の要素は一致させてある
//DisplayMode=DisplayMod1は、MainRsc1のリソース
ObjectRsc *RscData[] = {
  MainRsc1,
  MainRsc2,
  MainRsc3,
  CustomRsc,  
};

const char TouchMsg[] = "Touch anywhere to close the screen";

void (*PreviousEventHandler)(void);
void (*EventHandler)(void); //イベント処理のルーチンを設定して、これを実行


/*
 * イベントハンドラの設定
 * 呼び出すのは、常にEventHandler
*/
void SetEventHandler(void (* handler)(void)){
    PreviousEventHandler = NULL;
    EventHandler = handler;
}

/*
 * 今走らせているイベントハンドラを保存して、新たなイベントハンドラを走らせる
 */
void PopupEventHandler(void (* handler)(void)){
    PreviousEventHandler = EventHandler;    //現在のハンドラを保存
    EventHandler = handler;
}

/*
 * 保持していたイベントハンドラに戻す
 */
void ReturnEventHandler(){
    if (PreviousEventHandler) EventHandler = PreviousEventHandler;
}

/*
 * イベントバッファがいっぱいの時、1を返す
 */
uint8_t PushEvent(uint8_t ev) {
    if (ContNum < BufferMax) {
        Event[PushPoint] = ev;
        PushPoint = (PushPoint + 1) % BufferMax;
        ContNum++;  //バッファはサイクリックに使うので、いくつ有効かを示す
        return 0;
    } else {
        return 1;   //バッファフルというエラーだが、今の所エラーチェックしていない
    }
}

/*
 * イベントバッファに何も入っていない時は、EventNoneを返す
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
    ContNum = 0;    //キューに入っている数
}

/*
 * 10ms毎に割り込み
 * Touch: チャタリングチェック　　TouchStartをフラグに使う
 * スライドSW: 状態チェック
 * GPSユニット: 1PPSをチェック。起動チェックに利用
 */
//#define SlideSWoff  0
//#define SlideSWon   8
uint8_t Timer5Flag = 0;
void Timer5Handler() {
    static uint8_t touch_status = 0xff;     //チャタリング処理用
    static uint8_t slideSW_status = 0x55;   //チャタリング処理用
    static uint8_t G1PPS_status = 0xff;     //チャタリング処理用

    if (Timer5Flag) return;
    Timer5Flag = 1;
    //タッチした、やめた時にイベント発生。長押しになった時もイベント発生
    //タッチで、0
    touch_status = (uint8_t)(touch_status << 1) | T_IRQ_GetValue();
    if ((touch_status & 0x3f) == 0x38) {
        //3回連続タッチ(T_IRQ=0)と感知したら
        TouchStart |= 0x01;     // TouchDown Flag
        TouchCount = 0; //カウンタリセット
    } else if ((touch_status & 0x07) == 0) {
        //連続タッチ(0)が継続していたら、10ms毎にカウントアップ
        if (TouchCount < 200) {
            TouchCount++;
            if (TouchCount == 200) {
                //200x10ms=2s長押しの処理
                //TouchCountが200になった時だけ、Long判定
                TouchStart |= 0x02;     //TouchLong Flag
            }
        }
        TouchStart |= 0x04; //長押し関係なく、タッチが継続している時  
        if (RepeatKey) RepeatKey++;
    } else if ((touch_status & 0x3f) == 0x07) {
        //タッチしないとT_IRQ=1、3回連続でタッチやめたと判定
        TouchStart = 0x08;  //TouchUp Flag
    }
    
    //アラーム用スライドSWの状態チェック
    slideSW_status = (uint8_t)((slideSW_status << 1) | ALMSW_GetValue());
    if ((slideSW_status & 0x0f) == 0x08) {
        if (SlideSWStatus != SlideSWoff) {
            //それまでオン状態で、オフと判定された時、イベント発生
            SlideSWStatus = SlideSWoff +1;
        }
    } else if ((slideSW_status & 0x0f) == 0x07) {
        if (SlideSWStatus != SlideSWon) {
            //それまでオフ状態で、オンと判定された時、イベント発生
            SlideSWStatus = SlideSWon +1;
        }
    }
    
    //GPSユニットが、GPS衛星追尾できると1PPSが発信される
    if (Count1PPS < STABLE1PPS) {
        //安定受信状態になるまで実行
        G1PPS_status = (uint8_t)(G1PPS_status << 1) | G1PPS_GetValue();
        if ((G1PPS_status & 0x0f) == 0x08) {
            //きちんとLowレベル出たら(3回連続してLになった時=HLLL)
            Count1PPS++;
            if (Count1PPS >= STABLE1PPS) {
                GPSFlag = 1;
            }
        }
    }
    Timer5Flag = 0;

}


/*
 * TMR1の500us毎の割り込みで、ロータリーエンコーダの状態をチェック
 * 24クリック/1回転
 * 1秒で1回転したら、24回イベント発生する。
 * キーのチャタリング防止と同様のプログラム
 * 3回=1.5ms間、同じ状態なら、そのレベルと認識させる
 * 　→この割り込みで、1秒間に2000回チェックできる。1秒に2回転させたとして、
 * 　　　24クリック分x2回の状態変化が発生。1サイクルで40回ほどサンプリングできる計算なので、
 * 　　　信号変化は十分取り込める
 * 　→3回のサンプリングができる回転速度は、1周期=(3+3)*500us=3ms、24クリック*3ms=72msなので、
 * 　　1/72ms=13回転と、そんなに1秒間で回転できないレベルなので、十分
 * クリックの安定点では、A端子は常にOFF=1の状態
 * 
 * RotCountを更新していく
 * イベント処理が遅れても、RotCountの値を使って処理するので、全部のイベントを処理必要という
 * わけではなく、処理完了して、RotCount=0なら残ったイベントはスキップされる
 * 
 */
void RotaryHandler() {
    static uint8_t StartRot = 0;    //
    static int8_t Bdata = 0; // 時計回り=1、反時計回り=0
    static uint8_t RotA_status = 0xff;  //ROT_Aだけ変化点をチェック
    uint8_t newBdata;

    //A端子側のレベルをチェック。停止時=1、動かすと0となり、クリックポイントで1に戻る
    RotA_status = (uint8_t)(RotA_status << 1) | ROT_A_GetValue();

    if ((StartRot == 0) && (RotA_status & 0x07) == 0) {
        //A端子が0になったら、回転開始と判断
        StartRot = 1;
        //回転開始と判断した時のB端子のレベルで回転方向を判定
        Bdata = ROT_B_GetValue();
    } else if ((StartRot == 1) && ((RotA_status & 0x07) == 0x07)) {
        //回転動作していて、A端子が1になった時の処理
        newBdata = ROT_B_GetValue();
        // B端子側が、1→0の時CW(インクリ)、0→1の時RCW(デクリ)
        if (Bdata < newBdata) {
            //A端子がHになった時、B端子がHなら反時計方向に回転
            RotCount--;
            if (RotaryEventCount < 2) {
                //ロータリイベント発行制限。制限しても、RotCountを使って処理されるので問題なし。
                RotaryFlag = 1;
                RotaryEventCount++;
            }
        } else if (Bdata > newBdata) {
            //A端子がHになった時、B端子がLなら時計方向に回転
            RotCount++;
            if (RotaryEventCount < 2) {
                //ロータリイベント発行制限。制限しても、RotCountを使って処理されるので問題なし。
                RotaryFlag = 1;
                RotaryEventCount++;
            }
        }
        //ちゃんと回転せず元に戻った場合、カウントさせず、終了するケースあり。
        StartRot = 0;
    }
    
}


/*
 * アラームW(RTC_INTB=RB4のIOC)による割り込み
 * 18F27K42では、Negativeの指定ができるので、そちらに変更
 */
void AlarmWHandler() {
    //状態変化割り込みのため、0になった時だけフラグ立てる
    AlarmStatus = 1;
    SmoothCount = 0;
}


/*
 * INTAをINT0割り込みで
 * 1秒ごとに割り込みが入るので、時刻の更新イベント発生
 * 割込み中のI2C通信は止めて、イベント処理で実行することにした
 */
void RTC_handler() {
//*    PushEvent(EventTimeUpdate);
    RTC1sFlag = 1;
    EXT_INT0_InterruptFlagClear();
}

/*
 * 日付が変わったかチェックして、変更あれば日付表示更新
 * Year/Month/Day/WeekdayをEEPROMに格納しておき、電池入れ替え後の変更の手間を省く
 */
uint8_t WriteYMD() {
    uint8_t jj, tmp;
    uint8_t changeDate = 0;
    uint16_t addr;

    //何回も書き換えをしないように、変化あった時だけにする
    addr = AddressYMD;
    for (jj = 0; jj < 4; jj++) {
        tmp = DATAEE_ReadByte(addr);
        //曜日、日、月、年をチェック
        if (tmp != DateTime[jj+3]) {
            DATAEE_WriteByte(addr, DateTime[jj+3]);
            changeDate = 1;
        }
        addr++;
    }
    return changeDate;
}


/*
 * 1秒ごとに更新される時刻を利用して実施する処理
 * 現状3つ
 * 　・秒の一位の数字が9の時:　温湿度センサ
 * 　・秒の一位の数字が3か8の時：　照度チェック
 * 　・毎時:　日付の更新チェック
 */
void TimeEvent() {
    static uint16_t Brightness = 1023;
    uint16_t BackLight = 100;
    uint16_t data;    

    if ((DateTime[0] & 0x0f) == 9) {
        //温湿度は、秒の下1桁が9の時に更新
        Get_TempHumidity(&Temp, &Humidity);
        PushEvent(EventTempUpdate);
        PushEvent(EventHumidityUpdate);
    }   

    //時刻が変化した時に処理しないと、1秒間に何回もここが呼び出されてしまう
    else if (((DateTime[0] & 0x0f) % 5) == 3) {
        //秒の下1桁が3,8の時に照度チェック
        //照度センサで、明るさを5秒ごとに取得する
        //ADCの結果は、12bitの0-4095。明るいと大きな数値
        //ただし、暗いとADCの結果がかなりばらつく
        //昼間なら3.07V@3.29Vで、954-958、夜の蛍光灯下だと、50-150位だったか <-このデータ古い
        //ばらつきを低減するため、8回の合計値を取り、平均化
        //ADC_GetConversionでチャネル設定、ADC開始-取得まで実行
        data = ADCC_GetSingleConversion(PhotoDiode);
        Brightness = Brightness/8*7 + data/4;
        //            sprintf(str, "%d", data);
        //            display_drawChars(170, 20, str, WHITE, TextbgColor, FontMagx11);
        // Dutyを変更してバックライトの明るさを変更
        // Brightnessが一定数以上の時は、バックライトはほぼ常時点灯。
        BackLight = Brightness/8*3 + 10;
        // BackLightは、最低10-max999   0だと全く画面見えなくなるので、+10としている
        if (BackLight >= 1000) BackLight = 999;
        //PWMは、10ビットの解像度 (TMR6の周期=PR6=0xffに設定した時)
        PWM6_LoadDutyValue(BackLight);
        
#ifdef DEBUG2
        //輝度のレベルを表示 (デバッグ用)
        //            sprintf(str, "Br=%4d", Brightness);
        sprintf(str, "B=%d", BackLight);
        display_drawChars(170, 20, str, WHITE, TextbgColor, FontMagx11);
#endif
    }
    
    //日付が変わったかは、特にトリガないので、ここでチェック。毎時00分の時だけ実施
    else if (DateTime[1] == 0x00) {
        if (WriteYMD()) { //更新あれば、日付も更新する
            //日付が変わったら、再度GPS受信させるため、カウンタを0に戻す
            if (Count1PPS >= STABLE1PPS) Count1PPS = 0;
            PushEvent(EventDateUpdate);
        }
    }
}


/*
 * スライドSWオフの処理ルーチン
 */
void SlideSWoffProc() {
   
    AlarmSoundOff();
    RTC_resetAlarm();
    AlarmStatus = 0;    //スライドSWでOffした時
}

/*
 * スライドSWオンの処理ルーチン
 * アラームセットして、その時刻をEEPROMに書き込む
 */
void SlideSWonProc() {
    uint8_t jj, tmp;
    uint16_t addr;

    RTC_setAlarmTime(AlarmTime);
    //EEPROMにアラーム時刻を書き込み、電源切れても再投入時、使えるように
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
 * RotCountが0でない時は、処理する
 * ロータリーは、連続的にイベント発生するが、イベント発生毎に処理できなくてもよく、
 * RotCount=0でない時だけ処理すれば良い
 */
void RotaryProcedure() {
    int16_t delta;  //8bitだと不足することを想定
    uint8_t mm, hh;

    //ノーマル時のロータリーSWはアラーム時刻を変更
    if (RotCount == 0) return;

    delta = RotCount;
    RotCount = 0;
    
    mm = Bcd2Hex(AlarmTime[0]);
    if (DisplayMode != DisplayMode3) {
        //早く回転させたら、倍速、4倍速で　　ただし、deltaは、int8ということに注意
        if ((delta > 2) || (delta < -2)) {
            //8倍加速まで
            if (Accel < 8) Accel = Accel * 2;
            delta = delta * Accel;
        }
        else Accel = 1;
    } else {
        delta = delta * 5;  //5分単位に
        mm = mm - (mm % 5);    //5分単位に切り下げ
    }
    
    hh = Bcd2Hex(AlarmTime[1]);
    IncDecTime(delta, &hh, &mm);
    AlarmTime[0] = Hex2Bcd(mm);
    AlarmTime[1] = Hex2Bcd(hh);
    
    //アラームSWがonだったら、RTCのアラーム時刻を変更
    if (SlideSWStatus == SlideSWon) RTC_setAlarmTime(AlarmTime);
    
//--    PushEvent(EventFormUpdate);    //イベントだと反応遅く使い勝手悪い
    
    //EEPROMに書き込むタイミングが問題。ロータリーをぐるぐる回している時は、
    //何度もEEPROMにアラーム時刻を書き込みたくない
    //アラームSWをOnにした時だけに限定する
    
}


/*
 * タッチした座標x,y(グラフィック座標に換算したもの)が、指定されたオブジェクトの範囲に
 * 入っているかチェック
 * 指定したオブジェクトを押したと判定されると1を返す。それ以外は、0
 * obj: RscObjectで定義しているもの
 */
int8_t ButtonPush(int16_t x, int16_t y, MainFormRsc *objrsc) {
    int16_t xx, yy;
    
    xx = x - objrsc->x;
    yy = y - objrsc->y;

    if ((xx >= 0) && (xx < objrsc->xw)) {
        if ((yy >= 0) && (yy < objrsc->yw)) {
            //ボタンの矩形領域に入っていた場合
            return 1;
        }
    }    
    return 0;
}

/*
 * SDカードをマウントする
 * 既にmountしていたら、一旦unmount
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
 * タッチしたところの周辺データを取得 (3x3の領域)
 * データは、PixelData[]に、左上の座標はPixelXY[]に格納
 */
void GetPixelData(int16_t x, int16_t y) {
    uint8_t jj;
    uint8_t rgb[27];    //9ドット分のデータ格納領域

    PixelXY[0] = x-1;
    PixelXY[1] = y-1;
    addset(PixelXY[0], PixelXY[1], x+1, y+1);
    read_pixel(rgb, 9);
    for (jj = 0; jj < 9; jj++) {
        PixelData[jj] = display_color565(rgb[jj*3], rgb[jj*3+1], rgb[jj*3+2]);
    }
}

/* 
 * 保持しておいたデータで書き戻す
 * PixelXY[0]が9999以外だったらという制限を付け、データの有効性を確保
 */
void SetPixelData() {
    if (PixelXY[0]!=9999) glcd_array(PixelXY[0], PixelXY[1], 3, 3, (uint8_t *)PixelData);
}

//タッチされた時の処理
void TouchProcedure() {
    uint8_t mm, hh;
    char str[100];

    //タッチした所に印をつける
    SetPixelData();     //元のデータを戻す
    GetPixelData(TouchX, TouchY);   //書込む場所のデータを読み込む
    display_fillRect(TouchX-1, TouchY-1, 3, 3, RED);    // 3x3の赤い四角を書く
    
#ifdef DEBUG2
    sprintf(str, "%d,%d -> %3d,%3d ", TouchRawX, TouchRawY, TouchX, TouchY);
    display_drawChars(145, 30, str, WHITE, TextbgColor, FontMagx11);
    sprintf(str, "Adj=%d,%d, %d,%d ", T_x1, T_y1, T_x2, T_y2);
    display_drawChars(145, 40, str, WHITE, TextbgColor, FontMagx11);
#endif
    //アラーム中にタッチしたらスヌーズ
    if (AlarmStatus) {
        AlarmSoundOff();
        AlarmStatus = 0;    //アラーム止めたら0に
        RTC_resetAlarm();
        SmoothCount++;
        if (SmoothCount <= 12) {
            //5分後にアラーム再設定　12回まで、1時間後まで
            mm = Bcd2Hex(AlarmTime[0]);
            hh = Bcd2Hex(AlarmTime[1]);
            IncDecTime(SmoothCount*5, &hh, &mm);    //5分*Smooth回数の時刻に設定
            SmoothAlarmTime[0] = Hex2Bcd(mm);
            SmoothAlarmTime[1] = Hex2Bcd(hh);
            //ここで、RTCにアラーム時刻再設定
            RTC_setAlarmTime(SmoothAlarmTime);
            
        } else {
            //スヌーズ終了、アラームSWはOnのままの時
            //オリジナルのアラーム時刻をセットし直す(翌日用)
            RTC_setAlarmTime(AlarmTime);
        }
        return;
    }
    
    if (ButtonPush(TouchX, TouchY, (MainFormRsc *)&CurrentRsc[GearObj])) {
        //歯車アイコンをクリックしたら、設定画面へ
        //カスタマイズ用のDate, Time, Alram, Temp, Humiの初期化
        SetEventHandler(SettingEventHandler);
        ClearBuffer();
        PushEvent(EventFormInit);
    }
    else if (TouchX < 160 && TouchY > 120) {
        //画面の左下の1/4エリアをタッチして表示モード変更
        DisplayMode = (DisplayMode +1) % (sizeof(RscData)/sizeof(RscData[0])); //今の所、表示モードは4つ
        DATAEE_WriteByte(AddressDisplayMode, DisplayMode);  //変更したら書込む
        PushEvent(EventFormInit);
        Count1PPS = 0;  //表示を書き換えするときに、再度GPS受信

        //再マウント→これはgood。SDカード抜き差ししても、表示モード変更したらちゃんとアクセスできた
        remount();
    }
    
}

/*
 * ロングタッチ
 * 再デジタイズを実施
 */
void TouchLongProcedure() {
            
    //タッチ長押し
//        display_drawChars(250, 140, "T- Long", WHITE, TextbgColor, FontMagx11);
    //タッチ長押しで設定モードへ移行
//        Mode = Setting;
//    DisplayMode = Setting;
//--    PushEvent(EventFormInit);
    
    AlarmSoundOn(AlarmTwice);
    TouchAdjust();
    PushEvent(EventFormInit);

}


/*
 * チェックサムを計算して、一致しているかチェックする
 * Bufferに、$から*までのデータがある時に計算する
 * エラーあれば、1　　$や*がない場合、チェックサム計算が合っていない場合など
 */
uint8_t CalcCheckSum(char *buf) {
    char *start;
    char *end;
    uint8_t checksum;
    char chks[3];
    
    start = strchr(buf, '$');
    if (start == NULL) return 1;    //見つからなければ、エラー
    end = strchr(buf, '*');
    if (end == NULL) return 1;      //見つからなければ、エラー
    
    checksum = 0;
    start++;    //$の次から計算対象とする
    while (start < end) {
        checksum = (uint8_t)(checksum ^ (*start)); //xor計算
        start++;
    }
    sprintf(chks, "%02X", checksum);    //ここで、チェックサムを16進の文字列化
    
    //計算したチェックサムとデータでもらったチェックサム(?の後ろの2文字)を比較
//    if (checksum == strtol(end+1, NULL, 16)) {    //こちらでもOKだが、プログラムサイズ大
    if ((chks[0] == end[1]) && (chks[1] == end[2])) {
        return 0;
    } else {
        //チェックサムエラーの時
        return 1;
    }
    
}

/*
 * 取得したGPRMCのデータを処理する
 * GPRMC以外のデータの時は、何もしない
 * エラー時: 1
 */
uint8_t GPRMCdatacheck() {
    uint8_t jj, timeout;
    char *p;
    char str[100];

    //$GPRMCのデータの時だけ処理
    if (strncmp(Buffer, "$GPRMC", 6) != 0) return 1;

//    display_drawChars(0, 85, Buffer, WHITE, TextbgColor, FontMagx11);
    
    //チェックサムのチェック
    if (CalcCheckSum(Buffer)) {
//        sprintf(str, "x");
//        display_drawChars(0, 110, str, WHITE, TextbgColor, FontMagx11);
        return 1;   //エラー時
    }
//    sprintf(str, "o");
//    display_drawChars(0, 110, str, WHITE, TextbgColor, FontMagx11);

    //データが正常なので、必要情報を取り出す
    // hhmmss: 7-12文字　　1つ目のデータ=最初の,の後からデータがある　　BCD形式
    DateTime[2] = (uint8_t)(((Buffer[7]-'0')<<4) + (Buffer[8]-'0'));   //hh
    DateTime[1] = (uint8_t)(((Buffer[9]-'0')<<4) + (Buffer[10]-'0'));  //mm
    DateTime[0] = (uint8_t)(((Buffer[11]-'0')<<4) + (Buffer[12]-'0')); //ss
    
    // ddmmyy: 9つ目のデータ=9番目の,の後からデータがある
    p = strchr(Buffer, '$');
    for (jj = 0; jj < 9; jj++) {
        p = strchr(p+1, ',');
        if (p == NULL) return 1;    //見つからなかったら、やめる
    }
    DateTime[4] = (uint8_t)(((p[1]-'0')<<4) + (p[2]-'0')); //dd
    DateTime[5] = (uint8_t)(((p[3]-'0')<<4) + (p[4]-'0')); //mm
    DateTime[6] = (uint8_t)(((p[5]-'0')<<4) + (p[6]-'0')); //yy
    
    //経度を取得: 5番目のデータ    yyyyy.yy
    //経度で、時差を補正しようと思ったけど、日本国内でも、地域によってはよろしくないので、中止
    /*
    p = strchr(Buffer, '$');
    for (jj = 0; jj < 5; jj++) {
        p = strchr(p+1, ',');
        if (p == NULL) return 1;
    }
    uint8_t len = (strchr(p, '.') - p)-3; //経度の桁数　必ずしも3桁ではないので
    for (jj = 0; jj < len; jj++) {
        TZ = TZ*10 + (p[1+jj]-'0');
    }
    //東経は+、西経は-にする
    p = strchr(p+1, ',');
    if (p[1] == 'W') TZ = -TZ;
    TZ = (TZ+8) / 15;   //1時間当たり経度で15度なので、経度から時差を計算
     */
    
    //標準時をJST (現在地の時刻)に
    int8_t hh;
    hh = Bcd2Hex(DateTime[2]) + TZ;    //JSTなら+9
    if (hh >= 24) {
        //TZ補正後、深夜12時を過ぎたら、日付を1日進める
        hh = hh - 24;
        DateTime[4] = Hex2Bcd(Bcd2Hex(DateTime[4]) + 1);  //日付進める　BCDなので
    } else if (hh < 0) {
        //TZがマイナスの場合も一応プログラムしておく
        hh = hh + 24;
        DateTime[4] = Hex2Bcd(Bcd2Hex(DateTime[4]) - 1);  //日付戻す 1日だったら、0日になるがたぶんOK
    }
    DateTime[2] = Hex2Bcd((uint8_t)hh);  //調整後の時間を戻す
    dateErrorCheck(DateTime);   //日付の調整
    
    //        sprintf(str, "%02x:%02x:%02x %02x/%02x/%02x", DateTime[2], DateTime[1], DateTime[0], DateTime[6], DateTime[5], DateTime[4]);
    //        display_drawChars(0, 110, str, WHITE, TextbgColor, FontMagx11);
    
    //日時が取得できたので、RTCに書き込む　次の1PPSをトリガにするのが良い
    if (DateTime[0] > 0x55) {
        return 1;   //桁上がりがある時は、エラーとして再取得
    }
    
    timeout = 150;
    while (G1PPS_GetValue() == 1) {
        //次の1PPSを待つ
        __delay_ms(10); //10ms wait
        timeout--;
        if (timeout == 0) break;    //1.5秒待ってパルス来なかったら、終了
    }
    DateTime[0] = Hex2Bcd(Bcd2Hex(DateTime[0])+Count1PPS+1);    //GPS受信した秒に+1する
    RTC_setTime(DateTime);
    RTC_setDate(DateTime);
    Count1PPS = STABLE1PPS;     //GPS受信をやめる

    //ここでアップデートさせなくても、1秒ごとの更新はあるのでやめる
    //日付が変更になっていたら、更新。
    PushEvent(EventDateUpdate);

#ifdef DEBUG2
//    デバッグ用にデータ表示
    Buffer[13] = '\0';
    display_drawChars(0, 95, Buffer, WHITE, TextbgColor, FontMagx11);
#endif

    return 0;
}

/*
 * GPSユニットからシリアルデータ受信
 * これが呼び出されたら、LFまで構わずデータ取得
 * エラーあれば中断
 * データ受信は、できるだけ連続処理しないと、取りこぼす
 * イベント処理は、発生から実際の処理までにタイムラグあるので、ダメな場合が多い
 * 基本的にこのルーチン内で、一気に受信しないと失敗する
 *     //1行取得して、その処理が終わるまで、次のデータ取得させない
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
    char *BufferP;     //Bufferのどこまで書き込んだかというデータ位置を示すポインタ
    
    //GPSの状態: 受信中は黄色、完了すると緑(電源投入後で受信前でも)
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
            //最初の0x0aが来るまで、データを読み飛ばす。来たら、start=1にしてデータを取得する
            if (start == 0) {
                if (rxData == 0x0a) {
                    start = 1;
                    BufferP = Buffer;
                }
            } else {
                *BufferP = rxData;
                BufferP++;
                
                if ((BufferP - Buffer == 6) && (strncmp(Buffer, "$GPRMC", 6) != 0)) {
                    //$GPRMCのデータの時だけ処理するので、それもチェックした方が本当は良い
                    start = 0;
                }

                Count1PPS = 0;  //受信完了後のパルスをカウントする
                if (rxData == 0x0a) {
                    //改行コード(0x0a=LF)が来たら、データ受信を終了
                    *BufferP = '\0';
                    if (GPRMCdatacheck()) {
                        //エラーの時は、データ取得やり直し
                        drawGPSindicator(RED);
                        start = 0;

                        //無限ループにならないように、5回エラー発生したら、一旦抜けて再度実行
                        if (errorCount++ > 5) {
                            PushEvent(EventStartGPS);                    
                            return;
                        }
                    }
                    else {
                        //GPSの状態: 受信中は黄色、完了すると緑(電源投入後で受信前でも)
                        drawGPSindicator(GREEN);
                        return; //ここが正常な出口
                    }
                }
                if (BufferP - Buffer > 95) {
                    //バッファがあふれるのはおかしいので、データ破棄し、中断する
                    PushEvent(EventStartGPS);
                    return;
                }
            }
        }
}


/*
 * メイン画面の表示を更新する
 * CurrentRscの情報を元に初めて描画する時に呼び出される
 * 設定ボタン等は、ここで描画
 */
void DrawForm() {
    
    //時刻表示は変化があった所だけ表示更新するので、BCDではありえない数値を設定しておく
    resetPreDateTime();
    lcd_fill(TextbgColor);

    //設定ボタン
    DrawGearIcon(CurrentRsc[GearObj].x, CurrentRsc[GearObj].y, CurrentRsc[GearObj].color);
    
    //時刻表示更新
//    DrawTime(DateTime, &CurrentRsc[TimeObj]);
    
    //日付の更新
    DrawDate(&CurrentRsc[DateObj]);    //日付を描画
    DrawCalendar(&CurrentRsc[CalendarObj]);  //日付更新したらカレンダーも更新
    
    //温湿度が更新されたら
    DrawTemp(Temp, &CurrentRsc[TempObj]);
    DrawHumidity(Humidity, &CurrentRsc[HumidityObj]);
    
    //アラーム表示の更新　　スライドSWの状態で色を変える
    DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);

}


/*
 * メインのイベントハンドラ
 * 
 */
void MainEventHandler() {
    uint8_t evt;
    char str[100];

    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //イベントない時は、無駄に空回りさせないよう、10msのwait
        return;
    }
#ifdef DEBUG
    sprintf(str, "e=%03d", evt);
    display_drawChars(250, 40, str, WHITE, TextbgColor, FontMagx11);
#endif
    
    switch (evt) {
        case EventTimeUpdate:
            RTC_read(DateTime);
            resetCTFG();    //RTCの1秒ごとの割込み後、解除必要
            DrawTime(DateTime, &CurrentRsc[TimeObj]);    //時刻表示更新
            TimeEvent();
            break;

        case EventDateUpdate:
            DrawDate(&CurrentRsc[DateObj]);    //日付を描画
            DrawCalendar(&CurrentRsc[CalendarObj]);  //日付更新したらカレンダーも更新
            break;

        case EventFormInit:
            resetCTFG();    //RTCの1秒ごとの割込み後、解除必要
            //リソースを初期化して、画面描画する
            CurrentRsc = RscData[DisplayMode]; //モードに対応したリソースを設定
            DrawForm();
            PixelXY[0] = 9999;  //データがないことを示す
            break;

        case EventSlideSWoff:
            //スライドSWのOff処理
            SlideSWoffProc();
            DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //アラーム表示の更新
            break;
        case EventSlideSWon:
            //スライドSWのOn処理
            SlideSWonProc();
            DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //アラーム表示の更新
            break;
        case EventRotary:
            RotaryEventCount--;
            RotaryProcedure();
            DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //アラーム表示の更新
            //アナログ時計の場合は、DrawTimeも呼び出さないとアラーム針の更新がされないが、1秒毎に更新されるので、特に問題はない
            //アナログ時計全体を再表示するので、ここで更新してもそれほどレスポンスよくみえない
            //            DrawTime(DateTime, &CurrentRsc[TimeObj]);    //時刻表示も更新
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
 * DATAEE_WriteByteのワード版
 */
void DATAEE_WriteWord(uint16_t bAdd, uint16_t bData) {
    DATAEE_WriteByte(bAdd, lo(bData));
    DATAEE_WriteByte(bAdd+1, hi(bData));
}

/*
 * DATAEE_ReadByteのワード版
 */
uint16_t DATAEE_ReadWord(uint16_t bAdd) {
    uint16_t data = DATAEE_ReadByte(bAdd);
    data += (uint16_t)DATAEE_ReadByte(bAdd+1)<<8;
    return data;
}


/*
 * タッチの座標をグラフィックの座標に変換
 * これは環境によって変化あるため、TouchXT2046.cからここへ移動した
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

    //以下の計算精度に注意
    // (20,20)  (300,220)のタッチ上の座標をT_x1, T_y1, T_x2, T_y2に保持
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
 * タッチ座標調整用メッセージ
 * 対象のメッセージを白で、それ以外をグレーで
 * num=0/1  それ以外のエラーチェックしていない
 */
void TouchAdjMsg(uint8_t num) {
    uint16_t color[] = { WHITE, GREY };
    char str[100];

    //左上の十字を描画　中心座標(20,20)
    display_drawLine(16, 20, 24, 20, color[num]);
    display_drawLine(20, 16, 20, 24, color[num]);

    SetFont(FontNormal);
    strcpy(str, "1. Touch + at Up Left");
    display_drawChars(50, 100, str, color[num], TextbgColor, FontMagx12);
    strcpy(str, "2. Touch + at Bottom Right");
    display_drawChars(50, 120, str, color[1-num], TextbgColor, FontMagx12);

    //右下の十字　　中心座標(300, 220))
    display_drawLine(296, 220, 304, 220, color[1-num]);
    display_drawLine(300, 216, 300, 224, color[1-num]);
    
}


/*
 * (20,20)、(300,220)の2点をタッチした時の座標を(T_x1, T_y1)、(T_x2, T_y2)に
 * 取り込むためのルーチン
 */
void TouchAdjust(void) {
    uint8_t count;
    uint16_t addr;
    char str[100];
    int16_t dx, dy;

    lcd_fill(BLACK); //画面をクリア
    //最初に左上をタッチして、次に右下をタッチしてもらう
    count = 0;
    while (1) {
        if (count == 0) {
            TouchAdjMsg(0);
            // 1回目のタッチの位置
            while ((TouchStart & 0x01) == 0);
            while (GetTouchLocation(&T_x1, &T_y1) == -1);
            //タッチしたらピッという音を出す
            AlarmSoundOn(AlarmSingle);
            
            sprintf(str, "X1=%4d, Y1=%4d", T_x1, T_y1);
            display_drawChars(50, 20, str, WHITE, BLACK, FontMagx12);
            
            count++;
        } else {
            TouchAdjMsg(1);            
            // 2回目のタッチまで少し遅延。反応良すぎて、1回目と同じになったため
            __delay_ms(500);
            while ((TouchStart & 0x01) == 0);   //割込みでタッチと認識されるまでループ
            while (GetTouchLocation(&T_x2, &T_y2) == -1);
            //タッチしたらピッという音を出す
            AlarmSoundOn(AlarmSingle);
            
            sprintf(str, "X2=%4d, Y2=%4d", T_x2, T_y2);
            display_drawChars(100, 200, str, WHITE, BLACK, FontMagx12);
            
            //2回目のタッチが1回目と近い時は、やり直させる
            //2点の距離がX,Y各1500位ある　(1500/16)^2=8800　以下の計算だと、18000になる
            dx = (T_x1 - T_x2)/16;  //たぶん演算早い。また2乗した時int16を超えないよう
            dy = (T_y1 - T_y2)/16;
            if (dx*dx + dy*dy < 12000) {
                //距離が短い時は、もう一度
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
    //調整したタッチ座標を書き込む
    addr = AddressTouch;
    DATAEE_WriteWord(addr,   T_x1);
    DATAEE_WriteWord(addr+2, T_y1);
    DATAEE_WriteWord(addr+4, T_x2);
    DATAEE_WriteWord(addr+6, T_y2);
    
}


/*
 * 16bit RGB=565カラーのbmpファイルを読み込み表示する
 * 
 * 0x0000　(2)	bfType		ファイルタイプ　通常は'BM'
 * 0x0002　(4)	bfSize		ファイルサイズ (byte)
 * 0x0006　(2)	bfReserved1	予約領域　常に 0
 * 0x0008　(2)	bfReserved2	予約領域　常に 0
 * 0x000A　(4)	bfOffBits	ファイル先頭から画像データまでのオフセット (byte)
 * 0x000E　(4)	bcSize		ヘッダサイズ
 * 0x0012　(4)	bcWidth		画像の幅 (ピクセル)
 * 0x0016　(4)	bcHeight	画像の高さ (ピクセル) 正数なら，画像データは下から上へ
 *                                               負数なら，画像データは上から下へ
 * 0x001A　(2)	bcPlanes	プレーン数　常に 1
 * 0x001C　(2)	bcBitCount	1画素あたりのデータサイズ (bit)
 * 
 * ここでは、16bitカラーと決め打ちしている
 */
void ReadBmp16() {
    int16_t x, y, xx;
    int16_t kk;
    UINT actualLength;
    FRESULT res;
    uint16_t num;
    
    if (!SDcardMount) return;
    //ファイル名は8文字までサポート
    res = f_open(&FileObject, "Open8.bmp", FA_READ);    //DHT20に表記変更
    if (res != FR_OK) return;
    
    //Open
    //ヘッダ読込
    f_read(&FileObject, SDcardBuffer, 32, &actualLength); //32バイト分読み込む
    if (SDcardBuffer[bcBitCount] == 16) {
        //16bitカラーのデータの時のみ処理
        x = SDcardBuffer[bcWidth] + (SDcardBuffer[bcWidth+1]<<8);   //Width
        y = SDcardBuffer[bcHeight];                             //Height
        //シークして、データの先頭まで移動
        f_lseek(&FileObject, SDcardBuffer[bfOffBits]);
        
        //bmpデータは、表示される下の行から始まる
        //16bitカラーのbmpだけに限定している
        for (kk = 0; kk < y; kk++) {
            //DATASIZEを考慮したプログラム
            num = (uint16_t)x * 2;  //1行のバイト数=xドット x 16bit
            xx = 0;
            while (num > DATASIZE) {
                f_read(&FileObject, SDcardBuffer, DATASIZE, &actualLength); //水平ドット数x2バイト分のデータを読み込む
                glcd_array(xx, y-1-kk, DATASIZE/2, 1, SDcardBuffer);
                num -= DATASIZE;
                xx = xx + DATASIZE/2;
            }
            f_read(&FileObject, SDcardBuffer, num, &actualLength); //水平ドット数x2バイト分のデータを読み込む
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
////                f_read(&FileObject, SDcardBuffer, DATASIZE, &actualLength); //水平ドット数x2バイト分のデータを読み込む
////                glcd_array(xx, y-1-kk, DATASIZE/2, 1, SDcardBuffer);
////                num -= DATASIZE;
////                xx = xx + DATASIZE/2;
////            }
////            f_read(&FileObject, SDcardBuffer, num, &actualLength); //水平ドット数x2バイト分のデータを読み込む
////            glcd_array(xx, y-1-kk, num/2, 1, SDcardBuffer+100*0);
//
//    UINT cnt = 0;
//
//    if (btf == 0) {     /* Sense call */
//        /* Return stream status (0: Busy, 1: Ready) */
//        /* When once it returned ready to sense call, it must accept a byte at least */
//        /* at subsequent transfer call, or f_forward will fail with FR_INT_ERR. */
//        cnt = 1;    //常に1
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
    //ファイル名は8文字までサポート
    res = f_open(&FileObject, "Open8.bmp", FA_READ);    //DHT20に表記変更
    if (res != FR_OK) return;
    
    //Open
    //ヘッダ読込
    f_read(&FileObject, SDcardBuffer, 32, &actualLength); //32バイト分読み込む
    if (SDcardBuffer[bcBitCount] == 16) {
        //16bitカラーのデータの時のみ処理
        x = SDcardBuffer[bcWidth] + (SDcardBuffer[bcWidth+1]<<8);   //Width
        y = SDcardBuffer[bcHeight];                             //Height
        //シークして、データの先頭まで移動
        f_lseek(&FileObject, SDcardBuffer[bfOffBits]);
        
        //bmpデータは、表示される下の行から始まる
        //16bitカラーのbmpだけに限定している
        for (kk = 0; kk < y; kk++) {
            //DATASIZEを考慮したプログラム
            num = (uint16_t)x * 2;  //1行のバイト数=xドット x 16bit
            xx = 0;

            while (res == FR_OK && !f_eof(&FileObject)) {
                
                res = f_forward(&FileObject, outstream, 320, &dummy);
            }

//            while (num > DATASIZE) {
//                f_read(&FileObject, SDcardBuffer, DATASIZE, &actualLength); //水平ドット数x2バイト分のデータを読み込む
//                glcd_array(xx, y-1-kk, DATASIZE/2, 1, SDcardBuffer);
//                num -= DATASIZE;
//                xx = xx + DATASIZE/2;
//            }
//            f_read(&FileObject, SDcardBuffer, num, &actualLength); //水平ドット数x2バイト分のデータを読み込む
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
 * waitTouch=0: 表示終了後、自動的に処理終わらせる場合
 *           1: タッチしたら次の処理に移行させる
 */
void OpeningScreen(uint8_t waitTouch) {
    char str[100];
    int8_t jj;
    int16_t x, y, w, h;
    //カラーバーと同じく、75%の白、黄、シアン、緑、マゼンタ、赤、青
    uint16_t color[] = {
        0xBDF7, //白
        0xBDE0, //黄
        0x05F7, //シアン
        0x05E0, //緑
        0xB817, //マゼンタ
        0xB800, //赤
        0x0017, //青
    };

    lcd_fill(BLACK); //画面をクリア
    SetFont(FontNormal);
    display_setTextColor(WHITE, BLACK); //TextcolrとTextbgColorを設定
    display_setTextSize(FontMagx12);
    
    display_setCursor(0, 0);
    strcpy(str, TextOpenTitle);
    display_puts(str);
    
    display_setTextPitch(-1, 2);    //ピッチ変更

    display_setCursor(0, 30);
    for (jj = 0; jj < 5; jj++) {
        strcpy(str, TextOpenMsg[jj]);
        display_puts(str);
    }
    
    display_setTextPitch(0, 0); //元に戻す

    if (SDcardMount) {
        //SDカードがあれば、bmpを表示する
        strcpy(str, TextOpenMsg6);
        display_puts(str);
        ReadBmp16();
    }

    //カラーバー表示
    x = 0;
    y = 200;
    w = 40;
    h = 20;
    for (jj=0; jj<7; jj++) {
        display_fillRect(x, y, w, h, color[jj]);
        x += w;
    }

    if (waitTouch) {
        //タッチしたら、終了するというメッセージ出す
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

    //アラームの初期化
    AlarmInitialize();  //アラーム音のTMR0の初期設定
    
    // ロータリー用TMR1の割り込み設定
    TMR1_SetInterruptHandler(RotaryHandler);

    //Touch/SlideSW/GPSは、TMR5の10ms割り込みで状態チェック
    TMR5_SetInterruptHandler(Timer5Handler);
    TMR5_StartTimer();
    
    //LCDの初期化
    lcd_init();
    lcd_fill(DARK_BLUE); //画面をクリア

    SetFont(FontNormal);  //初期フォント設定

    //初めて起動した時は、全初期化、2回目以降はEEPROMからデータ取得
    if (DATAEE_ReadByte(AddressInit) == 0xff) {
        TouchAdjust();    //タッチの調整を実施し、そのデータをEEPROMに保持
      
        DATAEE_WriteByte(AddressInit, 0x55);
        //アラーム時刻を初期化
        addr = AddressAlarm;
        for (jj=0; jj<3; jj++) {
            DATAEE_WriteByte(addr++, AlarmTime[jj]);
        }
        DisplayMode = DisplayMode3; //最初の表示モードは、外部データ不要な3に変更
        DATAEE_WriteByte(AddressDisplayMode, DisplayMode);
        //カスタムデータの初期化
        CopyCustomDataToTemp();
        SetCustomData();    //EEPROMにも書き込み2回目以降の起動時に読み出すデータがあるようにする
        
    } else {
        //2回目以降の電源投入なら、タッチ座標補正のデータを読み出す
        addr = AddressTouch;
        T_x1 = DATAEE_ReadWord(addr);
        T_y1 = DATAEE_ReadWord(addr+2);
        T_x2 = DATAEE_ReadWord(addr+4);
        T_y2 = DATAEE_ReadWord(addr+6);
        
//        sprintf(str, "P1=(%d, %d) P2=(%d, %d)", T_x1, T_y1, T_x2, T_y2);
//        display_drawChars(0, 140, str, WHITE, TextbgColor, FontMagx11);
        
        //日付、アラーム設定を読み出す
        addr = AddressYMD;
        for (jj=0; jj<4; jj++) {
            dd = DATAEE_ReadByte(addr++);
            if (dd != 0xff) DateTime[jj+3] = dd;    //適切な書き込みがされなかった場合を回避
        }
        addr = AddressAlarm;
        for (jj=0; jj<3; jj++) {
            AlarmTime[jj] = DATAEE_ReadByte(addr++);
        }
        //前回終了時の表示モードにする
        DisplayMode = DATAEE_ReadByte(AddressDisplayMode);
        
        //カスタム設定を読み出す
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
    
    //GPSは、電源投入後、1PPS来たことを確認後、GPS受信動作を開始する
    Count1PPS = 0;
    
    //起動音
    AlarmSoundOn(AlarmTwice);

    lcd_fill(BLACK); //画面をクリア

    //ここで、SDカードをマウント。カードがなくても問題ないのでコメント化
//    sprintf(str, "Trying to mount SD card...");
//    display_drawChars(5, 40, str, WHITE, TextbgColor, FontMagx12);
    remount();

//    if (SDcardMount == 1) sprintf(str, "Found SD card");
//    else sprintf(str, "No SD card");
//    display_drawChars(5, 60, str, WHITE, TextbgColor, FontMagx12);
    
//    f_mount(0,"0:",0);  //unmount disk　　現状これは実行してはいけない

    //オープニング画面を表示
    OpeningScreen(0);

    // RTCの初期化
    //原因は、よくわからないが、SYSTEM_Initialize時、IOCBが有効化され、空のハンドラの状態だと、なぜか
    //ハングする。ここInterruptEnableの前にしないとハングする。それがわからず相当悩んだ
    //IOCB4=0にするか、ハンドラ設定(中身がないとデフォルトと同じくダメ)をする必要あり
    init_RTC(DateTime);
    INT0_SetInterruptHandler(RTC_handler);  //RTC_INTA (1s毎)の割り込み処理
    IOCBF4_SetInterruptHandler(AlarmWHandler); //アラーム Alarm Wの割り込み

    //イベントバッファの初期化
    ClearBuffer();
    
    // 電源投入直後の最初のイベントは、フォームの初期化
    //DisplayModeは上記で設定済み
    SetEventHandler(MainEventHandler);
    PushEvent(EventFormInit);
    
    while (1) {
        // Add your application code
        
        //割込みのイベント処理
        if (TouchStart & 0x01) {
            if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) {
                //ここでタッチ座標を取得して、イベント発生
                TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                PushEvent(EventTouchDown);
                //タッチしたらピッという音を出す
                StopSoundFlag = 0;  //
                AlarmSoundOn(AlarmSingle);
            }
            TouchStart &= 0xfe;
        } else if (TouchStart & 0x02) {
            //長押し
            if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) { //ここでも座標取得
                TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                PushEvent(EventTouchLong);
            }
            TouchStart &= 0xfd;
        } else if (TouchStart & 0x04) {
            //リピートが有効時、300ms毎にタッチダウンイベントを発生させる
            if (RepeatKey > 30) {
                if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) { //ここでも座標取得
                    TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                    //リピートが有効時、300ms毎にリピートイベントを発生させる
                    PushEvent(EventTouchRepeat);
                    RepeatKey = 1;  //0ではリピートdisableになるので、1にリセット
                }
            }
            TouchStart &= 0xfb;
        } else if (TouchStart & 0x08) {
            //タッチアップ　　座標はタッチダウン中の処理で取得
            PushEvent(EventTouchUp);
            RepeatKey = 0;  //リピート解除
            TouchStart = 0;
        }
        
        //アラーム
        if (AlarmStatus == 1) {
            AlarmSoundOn(AlarmSound);    //0番の音=アラームの最初の音を鳴らす
            AlarmStatus++;
        }
        
        //スライドSW
        if (SlideSWStatus-1 == SlideSWoff) {
            PushEvent(EventSlideSWoff);
            SlideSWStatus--;
        } else if (SlideSWStatus-1 == SlideSWon) {
            PushEvent(EventSlideSWon);
            SlideSWStatus--;
        }

        //ロータリー
        if (RotaryFlag) {
            PushEvent(EventRotary);
            RotaryFlag = 0;
        }

        //RTC割込み
        if (RTC1sFlag) {
            PushEvent(EventTimeUpdate);
            RTC1sFlag = 0;
        }

        //GPSイベント
        if (GPSFlag) {
            PushEvent(EventStartGPS);
            GPSFlag = 0;
        }

        //サウンドフラグ処理
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