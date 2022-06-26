/*
 * 日付、時刻の手動調整の処置
 */

#include "TimeAdjust.h"
#include <string.h>

#include "main.h"
#include "Draw.h"
#include "Customizing.h"
#include "LCD320x240color.h"
#include "RTC8025.h"
#include "Setting.h"

uint8_t SettingItem;    //変更対象のアイテム
uint8_t TmpTime[7];     //設定途中の時刻を保持

#define SettingYear      0x11
#define SettingMonth     0x12
#define SettingDay       0x13
#define SettingTime      0x14

#define MonthX      YearX + YearXw
#define MonthXw     3*8*2   //3桁x8ドットx2

#define DayX        MonthX + MonthXw
#define DayXw       50  //2+3桁x8ドットx2

#define TimebtnYw   30  //Timeのボタン高さ
#define YMDbtnYw    30  //YMDのボタン高さ=共通

#define OKX       250
#define OKY       130
#define OKXw      65
#define OKYw      45

#define CancelX       250
#define CancelY       OKY + 60
#define CancelXw      65
#define CancelYw      45

//ボタン座標データ: 左上の座標と、W, Hを指定
//Calendarはメイン画面でのみ使用
//それ以外は、設定画面でのみ使用

const char TimeAdjTextTitle[] = "Time Adj.";

//ボタン描画を共通ルーチンで行うため、BtnAbout-BtnCancelは、連続させること
enum {
    Title,
    BtnYear,     //1
    BtnMonth,    //2
    BtnDay,      //3
    BtnTime,     //4

    BtnYearUp,      //
    BtnYearDown,    //
    BtnMonthUp,     //
    BtnMonthDown,   //
    BtnDayUp,       //
    BtnDayDown,     //
    BtnTimeUp,      //
    BtnTimeDown,    //
    
    BtnOK,       //
    BtnCancel,   //

    ButtonNum   //ボタンの数
} Button;

//日時の設定画面用リソース
MainFormRsc TimeAdjustRsc[] = {
    //ID,       x,       y,        xw, yw,        fontcode,   fontMag,    str, color, format, attribute, attribute2, xpitchAdj
    { Title,    230,         0,    0,0,           FontNormal, FontMagx13, (char*)TimeAdjTextTitle, WHITE, },   //タイトル
    //    
    { BtnYear, YearX-10,  40-5,   YearXw,  30,    FontNormal, FontMagx22, NULL, WHITE, YYYYMMDD, }, // Year
    { BtnMonth,MonthX-10, 40-5,   MonthXw, 30,    FontNormal, FontMagx22, NULL, WHITE, }, // Month
    { BtnDay,  DayX-5,    40-5,   DayXw,   30,    FontNormal, FontMagx22, NULL, WHITE, }, // Day
    { BtnTime, TimeX,     135, TimeXw+20, TimeYw+5, Font7like, 0x67, NULL, LIME, HHMM, 0, 0, 2, }, // Time

    { BtnYearUp,   YearX-10,  40-7-YMDbtnYw,  YearXw-5,  YMDbtnYw, },    //Year up
    { BtnYearDown, YearX-10,  40-5+30+2,      YearXw-5,  YMDbtnYw, },    //Year down
    { BtnMonthUp,  MonthX-10, 40-7-YMDbtnYw,  MonthXw,   YMDbtnYw, },    //Month up
    { BtnMonthDown,MonthX-10, 40-5+30+2,      MonthXw,   YMDbtnYw, },    //Month down
    { BtnDayUp,    DayX-5,    40-7-YMDbtnYw,  DayXw,     YMDbtnYw, },    //Day up
    { BtnDayDown,  DayX-5,    40-5+30+2,      DayXw,     YMDbtnYw, },    //Day down
    { BtnTimeUp,   TimeX,     135-TimebtnYw-2,TimeXw+20, TimebtnYw, },   //Time up
    { BtnTimeDown, TimeX,     135+TimeYw+3+7, TimeXw+20, TimebtnYw, },   //Time down

    { BtnOK,     OKX,     OKY,     OKXw,     OKYw,     FontNormal, FontMagx22, (char*)BtnNameOK,     WHITE, 0,},    //OK
    { BtnCancel, CancelX, CancelY, CancelXw, CancelYw, FontNormal, FontMagx12, (char*)BtnNameCancel, WHITE, 0,},    //Cancel
};


/*
 * 時刻の表示：　フォーカス当たっている時は赤で表示
 */
void UpdateTime() {
    ObjectRsc tmpRsc;    //暫定リソース

    tmpRsc.disp = 1;
    tmpRsc.x = CurrentForm[BtnTime].x +7;
    tmpRsc.y = CurrentForm[BtnTime].y +7;
    tmpRsc.fontcode = CurrentForm[BtnTime].fontcode;
    tmpRsc.fontmag = CurrentForm[BtnTime].fontmag;  //フォントサイズ
    tmpRsc.format = CurrentForm[BtnTime].format;
    tmpRsc.xpitchAdj = CurrentForm[BtnTime].xpitchAdj;
            
    if (SettingItem == SettingTime) tmpRsc.color = RED;
    else tmpRsc.color = CurrentForm[BtnTime].color;

    DrawTime(TmpTime, &tmpRsc);
}

/*
 * 日付の表示：　フォーカス当たっている所は赤で表示
 */
void UpdateYMD() {
    uint16_t color;
    char str[50], str2[5];

    SetFont(FontNormal);   //フォントの設定
    display_setTextSize(FontMagx22);
//    display_setTextPitch(0, 0);
    display_setCursor(CurrentForm[BtnYear].x+10, CurrentForm[BtnYear].y +8);

    //年の表示
    if (SettingItem == SettingYear) color = RED;
    else color = CurrentForm[BtnYear].color;
    display_setColor(color);
    sprintf(str, "20%02x", TmpTime[6]);
    display_puts(str);  //これだと、その後座標指定することなく文字が書ける
    display_setColor(CurrentForm[BtnYear].color);
    strcpy(str2, "/");
    display_puts(str2);
    
    //月の表示
    if (SettingItem == SettingMonth) color = RED;
    else color = WHITE;
    display_setColor(color);
    sprintf(str, "%02x", TmpTime[5]);
    display_puts(str);  //これだと、その後座標指定することなく文字が書ける
    display_setColor(CurrentForm[BtnMonth].color);
    display_puts(str2);
    
    //日の表示
    if (SettingItem ==SettingDay) color = RED;
    else color = WHITE;
    display_setColor(color);
    sprintf(str, "%02x", TmpTime[4]);
    display_puts(str);  //これだと、その後座標指定することなく文字が書ける
}

/*
 * 日付、時間の設定用画面描画
 * ロータリーでの変更対象は、素直に対象の文字色を変更するだけにした。
 * 点滅とかの方が良いかもしれない
 * 
 * settingMode = SettingYear, SettingMonth, SettingDay, SettingTime
 * フォーカスがどこにあるかを示す
 */
void DrawTimeAdjustForm(uint8_t cls) {
    uint8_t jj;
    int16_t y1, y2;
    int16_t xx[3];
    
    if (cls) lcd_fill(TextbgColor);     // 画面をクリア(黒)
    
    //タイトル描画
    display_drawChars(CurrentForm[Title].x, CurrentForm[Title].y, CurrentForm[Title].str, CurrentForm[Title].color, TextbgColor, CurrentForm[Title].fontmag);
    
    //ボタン描画  Cancel, OK
    for (jj = BtnOK; jj <= BtnCancel; jj++) {
        DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 15);
    }
    
    //時計の表示
    UpdateTime();
    //日付の描画
    UpdateYMD();
    
    //タッチでも変更できるように、三角ボタンを描画　全部で8つ
    //アップ側の三角表示
    xx[0] = CurrentForm[BtnYearUp].x + 40;
    xx[1] = CurrentForm[BtnMonthUp].x + 25;
    xx[2] = CurrentForm[BtnDayUp].x + 25;

    y1 = CurrentForm[BtnYearUp].y + 7;
    y2 = y1 + CurrentForm[BtnYearUp].yw - 10;

    for (jj=0; jj<3; jj++) {
        display_fillTriangle(xx[jj], y1, xx[jj]-15, y2, xx[jj]+15, y2, GREY);
        display_drawTriangle(xx[jj], y1, xx[jj]-15, y2, xx[jj]+15, y2, WHITE);
    }

    //ダウン側の三角表示
    y2 = CurrentForm[BtnYearDown].y + 3;
    y1 = y2 + CurrentForm[BtnYearDown].yw - 10;
    for (jj=0; jj<3; jj++) {
        display_fillTriangle(xx[jj], y1, xx[jj]-15, y2, xx[jj]+15, y2, GREY);
        display_drawTriangle(xx[jj], y1, xx[jj]-15, y2, xx[jj]+15, y2, WHITE);
    }
    
    //時刻用の三角表示
    xx[0] = CurrentForm[BtnTimeUp].x + CurrentForm[BtnTimeUp].xw/2;
    y1 = CurrentForm[BtnTimeUp].y + 5;
    y2 = y1 + CurrentForm[BtnTimeUp].yw - 10;
    display_fillTriangle(xx[0], y1, xx[0]-40, y2, xx[0]+40, y2, GREY);
    display_drawTriangle(xx[0], y1, xx[0]-40, y2, xx[0]+40, y2, WHITE);

    y2 = CurrentForm[BtnTimeDown].y +3;
    y1 = y2 + CurrentForm[BtnTimeDown].yw - 10;
    display_fillTriangle(xx[0], y1, xx[0]-40, y2, xx[0]+40, y2, GREY);
    display_drawTriangle(xx[0], y1, xx[0]-40, y2, xx[0]+40, y2, WHITE);

    //デバッグ用にタッチの範囲に四角を描画
    for (jj = BtnYear; jj <= BtnTimeDown; jj++) {
        display_drawRoundRect(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].xw, CurrentForm[jj].yw, 5, GREY);
    }
}

/*
 * 日付をdelta分、前後させる
 */
void YMDclac(uint8_t mode, int8_t delta) {
    uint8_t yy, mo, dd;

    yy = Bcd2Hex(TmpTime[6]);
    mo = Bcd2Hex(TmpTime[5]);
    dd = Bcd2Hex(TmpTime[4]);
            
    if (mode == SettingYear) {
        yy = (uint8_t)(((yy + delta -1 + 99) % 99) + 1);  // 1-99年まで
    } else if (mode == SettingMonth) {
        mo = (uint8_t)(((mo + delta -1 + 12) % 12) + 1);
    } else if (mode == SettingDay) {
        dd = (uint8_t)(((dd + delta -1 + 31) % 31) + 1);
    }
    //曜日を計算
    TmpTime[3] = getWeekdays(&yy, &mo, &dd);
    TmpTime[6] = Hex2Bcd(yy);
    TmpTime[5] = Hex2Bcd(mo);
    TmpTime[4] = Hex2Bcd(dd);
}

/*
 * 時刻をdelta分前後させる
 */
void Timecalc(int8_t delta) {
   uint8_t mm, hh;
   
   mm = (uint8_t)Bcd2Hex(TmpTime[1]);
   hh = (uint8_t)Bcd2Hex(TmpTime[2]);
   IncDecTime(delta, &hh, &mm);
   TmpTime[1] = Hex2Bcd(mm);
   TmpTime[2] = Hex2Bcd(hh);
}


/*
 * タッチした座標をもらって、どのボタンが押されたか判定する
 * タッチ長押しで早送りさせる
 */
void TouchProcedureTimeAdjust() {
    uint8_t jj;
    
    if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnOK])) {
        //変化有無にかかわらず、RTC、EEPROMを更新
        for (jj = 1; jj < 7; jj++) DateTime[jj] = TmpTime[jj];
        DateTime[0] = 0x00;     //秒は00とする
        RTC_setTime(DateTime);  //時刻変更時のみ
        RTC_setDate(DateTime);
        //日付を変更したら、EEPROMに書き込む
        WriteYMD();
//        ReturnEventHandler();
        SetEventHandler(MainEventHandler);  //settingに戻さず、メインに戻す
        PushEvent(EventFormInit);
        return;
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnYear])) {
        //ロータリーの対象を選択(赤くする)
        SettingItem = SettingYear;
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnMonth])) {
        SettingItem = SettingMonth;
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnDay])) {
        SettingItem = SettingDay;
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnTime])) {
        SettingItem = SettingTime;
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnYearUp])) {   //どのボタンが押されたかチェック
        SettingItem = SettingYear;
        YMDclac(SettingYear, 1);
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnYearDown])) {
        SettingItem = SettingYear;
        YMDclac(SettingYear, -1);
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnMonthUp])) {
        SettingItem = SettingMonth;
        YMDclac(SettingMonth, 1);
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnMonthDown])) {
        SettingItem = SettingMonth;
        YMDclac(SettingMonth, -1);
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnDayUp])) {
        SettingItem = SettingDay;
        YMDclac(SettingDay, 1);
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnDayDown])) {
        SettingItem = SettingDay;
        YMDclac(SettingDay, -1);
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnTimeUp])) {
        SettingItem = SettingTime;
        Timecalc(+Accel);
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnTimeDown])) {
        SettingItem = SettingTime;
        Timecalc(-Accel);
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnCancel])) {
        ReturnEventHandler();
        PushEvent(EventFormInit);
        return;
    } else {
        //上記以外の場合は、ここでリターンさせ、加速処理をスキップ
        return;
    }
//    if (Accel < 16) Accel = Accel * 2;
    //30倍加速まで、リニアに加速させる
    if (Accel < 30) Accel++;
    PushEvent(EventFormUpdate);

}


/*
 * ロータリーの処理　　高速回転で8倍速まで加速
 */
void AdjRotaryProcedure() {
    int8_t delta, jj;

    if (RotCount == 0) return;

    delta = RotCount;
    RotCount = 0;
    
    //早く回転させたら、倍速、4倍速で　　ただし、deltaは、int8ということに注意
    if ((delta > 2) || (delta < -2)) {
        //8倍加速まで
        if (Accel < 8) Accel = Accel * 2;
        delta = delta * Accel;
    }
    else Accel = 1;
    
    if (SettingItem == SettingTime) {
        Timecalc(delta);
    } else if (SettingItem >= SettingYear && SettingItem <= SettingDay) {
        YMDclac(SettingItem, delta);
    } else return;
    PushEvent(EventFormUpdate);
    
}

/*
 * 日付、時刻設定用イベントハンドラ
 * 
 */
void TimeAdjustEventHandler() {
    uint8_t evt, jj;
    char str[100];
    
    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //イベントない時は、無駄に空回りさせないよう、10msのwait
        return;
    }
    
#ifdef DEBUG
//    sprintf(str, "e=%03d", evt);
//    display_drawChars(250, 0, str, WHITE, BLACK, FontMagx11);
#endif    
    switch (evt) {
        case EventFormUpdate:
            UpdateTime();
            UpdateYMD();
            break;
        case EventFormInit:
            CurrentForm = TimeAdjustRsc;
            //設定用に現時刻から仮変数へコピー
            for (jj = 0; jj < 7; jj++) TmpTime[jj] = DateTime[jj];

            DrawTimeAdjustForm(1);
            break;
        case EventRotary:
            RotaryEventCount--;
            AdjRotaryProcedure();
            break;
        case EventTouchDown:
            Accel = 1;  //最初にタッチした時に1に戻す
            RepeatKey = 1;

        case EventTouchRepeat:
            TouchProcedureTimeAdjust();
            break;
    }
}
