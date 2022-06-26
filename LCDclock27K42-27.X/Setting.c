/*
 * 歯車を押したときの各種
 */

#include "Setting.h"

#include <string.h>
#include "main.h"
#include "font.h"
#include "LCD320x240color.h"
#include "Draw.h"
#include "RTC8025.h"
#include "Customizing.h"
#include "TimeAdjust.h"

//以下により、文字列に2バイト文字があってもwarning出なくなる
#pragma jis

//下記オブジェクトの順にリソースを設定する
enum SettingRscObject {
    SettingTitleObj,
    //オブジェクト名
    CustomizingObj,
    TimeDateAdjustObj,
    AboutObj,
    FeatureObj,
    RedigitizingObj,
    //ボタン    
    SettingBtnCloseObj,    
};
    
const char SettingTextTitle1[] = "Setting Menu";
//以下のように定義しないとリソース内の文字列として使えないが、文字列長が固定なので、無駄にメモリを消費する
//1つ1つ文字列を定義しても良いが、個別に定数名を付けなければならないのが面倒
//constだと、Data領域を消費しない。値はプログラム領域に書き込まれることがわかった

const char TextCustomizing[]  = "- Customizing";
const char TextTimeAdjust[]   = "- Time/Date adjusting";
const char TextFeature[]      = "- Feature (same as Opening)";
const char TextRedigitizing[] = "- Re-digitizing or push screen >2s";

#ifdef ENGLISH
//English mode   Defined on main.h
const char *AboutTitle = "Feature of LCD Clock";
const char TextAbout[] = "- About";
const char *AboutMsg[] = {
    "- No need time adjust by GPS unit\r\n",
    "- Easy to set alarm by rotary + switch\r\n",
    "  Smooth by touching screen. Stop by slide switch\r\n",
    "- Can select 3types screen or user setting by touching on the bottom left.\r\n",
    "- Push Gear Button to setting form\r\n",
    "  Customizing parts place, color, etc.\r\n",
    "  Can use favorite font of BMP files on SDcard \r\n",
};
#else
//Japanese mode
const char *AboutTitle = "このLCD Clockの機能\ ";
const char TextAbout[] = "- About (Need SJIS font)";
const char *AboutMsg[] = {
    "◇GPSユニットがあれば時刻修正は不要！\r\n",
    "◇アラーム設定は、つまみ回し+SWと簡単\r\n",
    "　アラーム鳴ったら、画面タッチでスヌーズ。SWオフで完全停止\r\n",
    "◇画面は設定済み3種とユーザ設定の中からお好みで。切替は画面左下部をタッチ\r\n",
    "◇歯車ボタンで、設定画面へ\r\n",
    " ・部品の配置、色などをカスタマイズ可\r\n",
    "　SDカードにbmpファイルを格納すれば、お好きなフォントも使用可\r\n",
};
const char *AboutError = "Can't show the message in Japanese.\r\n   Prepare JIS16.dat";
#endif


uint8_t AboutFlag = 0;

//各行の高さ
#define SettingItemH   30

//設定画面用リソース
MainFormRsc SettingRsc[] = {
    //ID,                x,   y,                 xw,  yw,           fontcode,   fontMag,    str,                color,
    { SettingTitleObj,   0,   0,                 0,   0,            FontNormal, FontMagx22, (char*)SettingTextTitle1, WHITE, },   //タイトル
    //項目名
    { CustomizingObj,    20, 30,                 200, SettingItemH, FontNormal, FontMagx12, (char*)TextCustomizing,  WHITE, },
    { TimeDateAdjustObj, 20, 30+SettingItemH*1,  200, SettingItemH, FontNormal, FontMagx12, (char*)TextTimeAdjust,   WHITE, },
    { AboutObj,          20, 30+SettingItemH*2,  200, SettingItemH, FontNormal, FontMagx12, (char*)TextAbout,        WHITE, },
    { FeatureObj,        20, 30+SettingItemH*3,  200, SettingItemH, FontNormal, FontMagx12, (char*)TextFeature,      WHITE, },
    { RedigitizingObj,   20, 30+SettingItemH*4+5,200, SettingItemH, FontNormal, FontMagx12, (char*)TextRedigitizing, WHITE, },
    //ボタン    
    { SettingBtnCloseObj,240, 200,                70, SettingItemH, FontNormal, FontMagx12, (char*)CloseText,        WHITE, },  // Close
};


/*
 * About
 */
void AboutText() {
    char str[150];

    lcd_fill(BLACK); //画面をクリア(真っ黒)

    //漢字フォントを設定
#ifdef ENGLISH 
    display_setTextSize(FontMagx12);
    display_drawChars(0, 0, AboutTitle, WHITE, TextbgColor, FontMagx12);
#else
    SetKanjiFont(SJIS16);
    if (CurrentKanjiFont.fontcode) {
        display_drawChars(20, 0, (char *)AboutTitle, WHITE, TextbgColor, FontMagx11);
        display_setTextSize(FontMagx11);
#endif
        
        display_setCursor(0, 20);
//        display_setTextPitch(0, 0);
        display_setTextColor(WHITE, BLACK);
        display_setWrap(true);
        
        for (uint8_t jj = 0; jj < 7; jj++) {
            display_puts((char *)AboutMsg[jj]);
        }
        
        display_setWrap(false);  
#ifndef ENGLISH 
        SetKanjiFont(0);
    } else {
        //漢字フォントない時のメッセージ
        display_drawChars(20, 100, (char *)AboutError, WHITE, TextbgColor, FontMagx12);
    }
#endif
    
    display_drawChars(20, 220, (char*)TouchMsg, GREY, TextbgColor, FontMagx12);

}

/*
 * 設定画面をタッチした時の処置
 * 　座標によって処理内容を特定
 */
void TouchProcedureSetting() {
    char str[100];
    
#ifdef DEBUG
    sprintf(str, "TXY=%03d, %03d", TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, BLACK, FontMagx11);
#endif
    if (AboutFlag) {
        //About/Feature表示中なら、タッチしたら、元の画面に戻すのを優先させる
        PushEvent(EventFormUpdate);
        AboutFlag = 0;
        return;        
    } 
    
    if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnCloseObj])) {   //closeボタン
        remount();  //
        ClearBuffer();
        SetEventHandler(MainEventHandler);  //メインに戻る
        PushEvent(EventFormInit);
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[CustomizingObj])) {
        ClearBuffer();
        CopyCustomDataToTemp();
        SetEventHandler(CustomizingEventHandler);  //カスタマイズ
        PushEvent(EventFormInit);
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[RedigitizingObj])) { //再デジタイズ
        TouchAdjust();
        PushEvent(EventFormUpdate);
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[TimeDateAdjustObj])) {
        PopupEventHandler(TimeAdjustEventHandler);   //日時修正
        PushEvent(EventFormInit);
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[AboutObj])) {    //このソフトについて
        AboutText();
        AboutFlag = 1;
    }
    else if (ButtonPush(TouchX, TouchY, &CurrentForm[FeatureObj])) {    //Feature
        OpeningScreen(1);
        AboutFlag = 1;
    }
}
        
        
/*
 * cls=1の時は、画面消去してから描画
 * 設定画面を描画
 */
void DrawSettingForm(uint8_t cls) {
    uint8_t jj;

    if (cls) lcd_fill(TextbgColor);
    
    SetFont(FontNormal);  //初期フォント設定

    //タイトル、項目名表示
    jj = SettingTitleObj;
    display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    
    //各オブジェクトの情報を表示
    for (jj = CustomizingObj; jj < SettingBtnCloseObj; jj++) {
        display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    }
    
    //ボタン描画  Close
    jj = SettingBtnCloseObj; 
    DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 10);

}

/*
 * 設定用イベントハンドラ
 * 
 */
void SettingEventHandler() {
    uint8_t evt;
    char str[100];
    
    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //イベントない時は、無駄に空回りさせないよう、10msのwait
        return;
    }
    
#ifdef DEBUG
    sprintf(str, "e=%03d", evt);
    display_drawChars(250, 0, str, WHITE, TextbgColor, FontMagx11);
#endif    
    switch (evt) {
        case EventFormInit:
            CurrentForm = SettingRsc;
        case EventFormUpdate:
            DrawSettingForm(1);
            break;
        case EventTouchDown:
            TouchProcedureSetting();
            break;
    }
}
