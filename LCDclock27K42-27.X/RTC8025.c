/*
 * File:   RTC8025.c
 * Author: K.Ohno
 *
 * Real Time Clock (RTC) Module
 * EPSON RX-8025NB I2C-Bus interface
 * 
 *         ______
 * FOE   1|i     |8 VDD = 1.15V - 5.5V
 * F0UT  2|o    o|7 /INTB (Need PU)
 * /INTA 3|o    i|6 SCL + PU=2.2k
 * GND   4|    io|5 SDA + PU=2.2k
 *         ------
 * 
 * I2C: 400 kHz動作可能だが、センサのスピードに合わせ100kHzで使用。
 * I2C信号は、このRTCボード上でpullup
 * /INTAは、ボード上でPullup+LED　　レベルモードで1秒ごとにLowとなるモード(定周期割り込み出力)
 * 　ボード上で赤色LEDを接続し、1秒ごとに点滅モードに設定、1秒更新をPICに知らせる
 * /INTBは、オープンドレイン出力　　外付けでpull up必要なので、WPU指定
 * 　　　　　Alarm_W (/INTB)　アラーム用割り込み
 * FOE=Open(=L)で、FOUT=L(固定)出力となるが、未使用
 */
#include "RTC8025.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"
#include <time.h>

#define RX8025  0x32    //7bitのR/Wビットを除いたアドレス

//RTCのレジスタ設定用定義
//Reg-Eのfield
uint8_t RTC_regE;
#define RTC_WALE 0x80   //1:Weekly Alarmの有効化
#define RTC_DALE 0x40   //1:Weekly Alarmの有効化
#define Time24   0x20   //1:24時間制
#define BCLEN2   0x10   //1=/CLEN2
uint8_t CT2_0 = 0x04; //1Hzパルスモード、/INTA=L

//Reg-Fのfield
//#define regF_default = 0b01011111; //RegFへの書き込みのデフォルト値
uint8_t RTC_regF = 0b01011111; //RegFへの書き込みのデフォルト値
#define RTC_VDSL 0x80u   //0:2.1V、1:1.3Vに電圧低下検出レベル設定
#define RTC_VDET 0x40u   //1:電圧低下の検出あり
#define RTC_XST  0x20u   //0:発信停止の検出あり
#define RTC_PON  0x10u   //1:パワーオンリセット発生の検出あり
#define BCLEN1   0x08u   //1=/CLEN1
#define RTC_CTFG 0x04u   //INTA割り込みがレベルモードの時、0 writeで/INTA=Offとする
#define RTC_WAFG 0x02u   //WALE=1の時、アラームW発生時=1、0writeで/INTB=Off
#define RTC_DAFG 0x01u   //DALE=1の時、アラームD発生時=1


/*
 * xをBCDの2桁に変換
 */
uint8_t Hex2Bcd(uint8_t x) {
    return (uint8_t)(((x / 10) << 4) + (x % 10));
}

uint8_t Bcd2Hex(uint8_t x) {
    return (uint8_t)((x >> 4)*10 + (x & 0xf));
}


/*
 * RTCのアラーム解除
 * WAFGをクリア
 */
void RTC_resetAlarm(void) {

    RTC_INTB_SetLow();
    //reg-EのWALEは、一度0にして、無効化
    I2C2_Write1ByteRegister(RX8025, 0xe0, RTC_regE & ~RTC_WALE); //E:bit7=WALE
    __delay_us(80); //Stopから次のStartまで、62us以上

    RTC_regF = RTC_regF & ~RTC_WAFG; //F:bit1=WAFG=0
    I2C2_Write1ByteRegister(RX8025, 0xf0, RTC_regF);
    __delay_us(80); //次のStartまで、62us以上

}


/*
 * アラーム時間を設定
 * 使用変数：hh(BCD10進), mm(BCD10進), wday (曜日指定しなければ0x7f)
 * アナログ表示の時は、AM/PMを区別させず、現時刻から一番近い時刻でアラーム設定
 */
void RTC_setAlarmTime(uint8_t *alarmtime) {

    //Alarm_WのWALEを一旦リセット
    //reg-EのWALEは、一度0にして、再度有効化しないといけない
    I2C2_Write1ByteRegister(RX8025, 0xe0, RTC_regE & ~RTC_WALE); //E:bit7=WALE
    __delay_us(80); //次のStartまで、62us以上

    I2C2_Write1ByteRegister(RX8025, 0x80, alarmtime[0]);
    __delay_us(80); //Stopから次のStartまで、62us以上
    I2C2_Write1ByteRegister(RX8025, 0x90, alarmtime[1]);
    __delay_us(80); //Stopから次のStartまで、62us以上
    I2C2_Write1ByteRegister(RX8025, 0xa0, alarmtime[2]);
    __delay_us(80); //Stopから次のStartまで、62us以上

    //Alarm_W設定
    //reg-EのWALEを、再度有効化
    RTC_regE = RTC_regE | RTC_WALE;
    I2C2_Write1ByteRegister(RX8025, 0xe0, RTC_regE); //E:bit7=WALE
    __delay_us(80); //Stopから次のStartまで、62us以上
    RTC_regF = RTC_regF & ~RTC_WAFG;
    I2C2_Write1ByteRegister(RX8025, 0xf0, RTC_regF); //E:bit7=WALE
    __delay_us(80); //Stopから次のStartまで、62us以上
    
    //Alarm割込み設定
    //interrupt on change for group IOCB - flag
    RTC_INTB_SetHigh();
}

/*
 * 分単位で、時刻をdmm分(+/-)する
 * 増加分dmmを大きな数値にできるように、16bit変数にした
 */
void IncDecTime(int16_t dmm, uint8_t *hh, uint8_t *mm) {
    int16_t min, hour;
    
    min = (int16_t)*mm;
    hour = (int16_t)*hh;
    min = min + dmm;
    
    if (dmm >= 0) {
        //インクリ時
        while (min > 59) {
            min = min - 60;
            hour = hour + 1;
        }
    } else {
        //デクリ時
        while (min < 0) {
            min = min + 60;
            hour = hour -1;
        }
    }
    if (hour > 23) hour = hour - 24;
    else if (hour < 0) hour = 24 + hour;
    *hh = (uint8_t)hour;
    *mm = (uint8_t)min;
    
}


/*
 * RTCに新たな時刻を設定
 */
void RTC_setTime(uint8_t *datetime) {

    I2C2_Write1ByteRegister(RX8025, 0x00, datetime[0]); //秒
    __delay_us(80); //Stopから次のStartまで、62us以上
    I2C2_Write1ByteRegister(RX8025, 0x10, datetime[1]);
    __delay_us(80); //Stopから次のStartまで、62us以上
    I2C2_Write1ByteRegister(RX8025, 0x20, datetime[2]);
    __delay_us(80); //Stopから次のStartまで、62us以上
}

/*
 * RTC日付設定
 */
void RTC_setDate(uint8_t *datetime) {
    I2C2_Write1ByteRegister(RX8025, 0x30, datetime[3]);
    __delay_us(80); //Stopから次のStartまで、62us以上
    I2C2_Write1ByteRegister(RX8025, 0x40, datetime[4]);
    __delay_us(80); //Stopから次のStartまで、62us以上
    I2C2_Write1ByteRegister(RX8025, 0x50, datetime[5]);
    __delay_us(80); //Stopから次のStartまで、62us以上
    I2C2_Write1ByteRegister(RX8025, 0x60, datetime[6]);
    __delay_us(80); //Stopから次のStartまで、62us以上
}


/*
 * 日付のエラーチェック St[5:6]:月、St[8:9]：日
 * 日が1-31以外の時は、01とする
 * 月が1-12以外の時は、12の剰余をとる　13=1月
 */
void dateErrorCheck(uint8_t *dateTime) {
    uint8_t yy, mm, dd;

    yy = Bcd2Hex(dateTime[6]);
    mm = Bcd2Hex(dateTime[5]);
    dd = Bcd2Hex(dateTime[4]);

    dateTime[3] = getWeekdays(&yy, &mm, &dd);
    
    dateTime[6] = Hex2Bcd(yy);
    dateTime[5] = Hex2Bcd(mm); //適切な月日に。4/31→5/1とか
    dateTime[4] = Hex2Bcd(dd);
}


/*
 * 日付のチェックと曜日の計算
 * 曜日(0:日-6:土)を返す
 * year: 2000年を基準にした0-99年　　0=2000年、99=2099年
 * month: 1-12
 * day: 1-31
 * おかしな日付も修正してくれる。例：4/31　→　5/1に
 */
uint8_t getWeekdays(uint8_t *year, uint8_t *month, uint8_t *day) {
    struct tm t;
    struct tm *t2;
    time_t tim;

    t.tm_year = *year + 2000 - 1900;
    t.tm_mon = *month - 1; //要注意：1月=0
    t.tm_mday = *day;
    t.tm_wday = 5; //初期値は何でもよい
    t.tm_hour = 1; //適当な時間　01:00:00
    t.tm_min = 0;
    t.tm_sec = 0;

    tim = mktime(&t);
    t2 = localtime(&tim);

    *year = (uint8_t)(t2->tm_year - 100);  //1900年基準を2000年基準に
    *month = (uint8_t)(t2->tm_mon) + 1; //適切な月日に。4/31→5/1とか
    *day = (uint8_t)t2->tm_mday;

    return (uint8_t)t2->tm_wday; //曜日を返す
}


/*
 * RTCから年月日、曜日、時分秒をdatetimeに読み出す
 * 1秒ごとの割込み発生後、これを呼び出すので、delayは不要とした
 */
void RTC_read(uint8_t *datetime) {

    __delay_us(80); //Stopから次のStartまで、62us以上
    I2C2_ReadDataBlock(RX8025, 0x00, datetime, 7);
}


/*
 * RegFのCTFGをリセットして、/INTA=Hi-z(プルアップされているのでH)にする
 * 定周期割り込みを使う場合、割り込みが入るごとに、CTFGをリセットしないといけない
 */
void resetCTFG(void) {
    
    //Reg-F CTFGリセット
    RTC_regF = RTC_regF & ~RTC_CTFG; //F: CTFG=0とし、それ以外はリセットしない
    I2C2_Write1ByteRegister(RX8025, 0xf0, RTC_regF);
    __delay_us(80); //Stopから次のStartまで、62us以上
}

//void readReg(void) {
//    //電源投入直後なら、0x00
//    RTC_regE = I2C2_Read1ByteRegister(RX8025, 0xe0);
//    //電源投入直後なら、0x10
//    RTC_regF = I2C2_Read1ByteRegister(RX8025, 0xf0);
//
//}

/*****************************
 * RTCの初期設定
 * RTCの電源が既に入っている場合、時刻情報はそれを使う
 * RTC_regEとRTC_regFを初期化(必要な場合)
 * 電源一度切れても、日付情報は格納されているもので設定
 * 電圧サンプリングは、1秒間に7.8msだけということで、瞬低の検出できないケースあり
 *****************************/
void init_RTC(uint8_t *datetime) {
    
    __delay_us(80); //Stopから次のStartまで、62us以上

    //既にRTCが動作していた(電源入っていた)か確認のため、ここで状態取得
    //まず、Reg-EとFを確認
    //電源投入直後なら、0x00
    RTC_regE = I2C2_Read1ByteRegister(RX8025, 0xe0);
    //電源投入直後なら、0x10
    RTC_regF = I2C2_Read1ByteRegister(RX8025, 0xf0);

    //    debug[0] = RTC_regF;
    
    //bit6-4:VDET=0, /XST=1, PON=0 の時は、RTCは電源が入ったまま正常動作していて、
    //PICが再起動しただけなので、RTCは、CTFGのみ初期化→瞬低検出できないことを想定して
    //プログラム変更
    //RTC_VDET 0x40   //1:電圧低下の検出あり
    //RTC_XST  0x20   //0:発信停止の検出あり　これだけ極性逆
    //RTC_PON  0x10   //1:パワーオンリセット発生の検出あり
    if ((RTC_regF & (RTC_VDET | RTC_XST | RTC_PON)) == RTC_XST) {
        //初期化不要なケース
        RTC_read(datetime);
        //CTFGだけをリセット：レベルモードの時、ここでCTFGのリセットしておかないと、
        //既にCTFGが設定されてしまっている場合、動作しなくなる
        resetCTFG();
//        return;
    }

    //上記以外の時は、RTCの初期化が必要
    __delay_us(80); //Stopから次のStartまで、62us以上
    //VDSL VDET /XST PON /CLEN1 CTFG WAFG DAFG
    //XSTをセットして、次回の発信停止を検出できるようにする
    RTC_regF = RTC_XST | BCLEN1;
    I2C2_Write1ByteRegister(RX8025, 0xf0, RTC_regF);
    __delay_us(80); //Stopから次のStartまで、62us以上

    //bit7 bit6  bit5   bit4  bit3  bit2-0
    //WALE DALE /12,24 /CLEN2 TEST CT2 CT1 CT0
    //E:bit7=WALE, bit5=24時間制, CT=100=レベルモード1s
    RTC_regE = Time24 | BCLEN2 | CT2_0;  //コールド電源投入後の場合は、ここで初期化
    I2C2_Write1ByteRegister(RX8025, 0xe0, RTC_regE);
    __delay_us(80); //Stopから次のStartまで、62us以上

    //日付をチェックして、曜日を設定
    dateErrorCheck(datetime);

    //時刻をRTCに設定
    RTC_setDate(datetime);
    datetime[0] = 0x00; //秒は00とする
    RTC_setTime(datetime);

    I2C2_Write1ByteRegister(RX8025, 0x70, 0x00); //Digital Offset:使用しない場合0をセット

    I2C2_Write1ByteRegister(RX8025, 0x80, 0x00); //Alarm_W ; Minute:使用しない場合0をセット
    I2C2_Write1ByteRegister(RX8025, 0x90, 0x00); //Alarm_W ; Hour:使用しない場合0をセット
    I2C2_Write1ByteRegister(RX8025, 0xa0, 0x00); //Alarm_W ; Weekday:使用しない場合0をセット

    I2C2_Write1ByteRegister(RX8025, 0xb0, 0x00); //Alarm_D ; Minute:使用しない場合0をセット
    I2C2_Write1ByteRegister(RX8025, 0xc0, 0x00); //Alarm_D ; Hour:使用しない場合0をセット

    __delay_us(80); //Stopから次のStartまで、62us以上
    
}
