/*
 * ColorSetting EventHandler
 * 
 * 2022/5/15
 * 
 * Color Setting for Customizing
 * 
 */

#include "ColorSetting.h"
#include "main.h"
#include "LCD320x240color.h"
#include "Customizing.h"
#include "Draw.h"
#include "font.h"

const char TitleColorSetting[] = "Color Setting";
const char RGBtext[][2] = {"R", "G", "B"};
const char RGBtext2[] = "Actual Color";
const char BtnSelect0[] = "On";
const char BtnSelect1[] = "Off";
const char BtnWeekday[] = "Day";
const char BtnSatday[] = "Sat";
const char BtnSunday[] = "Sun";


//カラー設定用に、下記オブジェクトの順にリソースを設定する
enum ColorRscObject {
    TitleObj,
    RbarObj,    //1
    GbarObj,
    BbarObj,
    ColorBoxObj,
    OKBtnObj,   //9
    CancelBtnObj,
    Select0BtnObj,
    Select1BtnObj,
    SelectWeekdayBtnObj,
    SelectSatBtnObj,
    SelectSunBtnObj,
};

MainFormRsc ColorSettingRsc[] = {
    //ID,        x, y, xw, yw, fontcode,   fontMag,    str,                      color, format, attribute
    { TitleObj,   0,  0, 0, 0, FontNormal, FontMagx22, (char*)TitleColorSetting, WHITE, 0, },  //Title
    //RGBのカラーバー、上部にタイトル、下部に値をこのリソースを利用して描画
    { RbarObj, 30, 40, 30, 128, FontNormal, FontMagx22, (char*)RGBtext[0], WHITE, 0, },  //Rbar
    { GbarObj, 80, 40, 30, 128, FontNormal, FontMagx22, (char*)RGBtext[1], WHITE, 0, },  //Gbar
    { BbarObj,130, 40, 30, 128, FontNormal, FontMagx22, (char*)RGBtext[2], WHITE, 0, },  //Bbar
    //設定した色を表示するbox、およびタイトル
    { ColorBoxObj, 180, 40, 100, 100, FontNormal, FontMagx12, (char*)RGBtext2, WHITE, 0, },

    { OKBtnObj,     170, 200, 60, 30, FontNormal, FontMagx12, (char*)BtnNameOK,     WHITE, 0, },  // OK
    { CancelBtnObj, 250, 200, 60, 30, FontNormal, FontMagx12, (char*)BtnNameCancel, WHITE, 0, },  // Cancel
    
    //アラームの色設定の時だけ有効化させるセレクタ　　　アラームオン、オフそれぞれの色を設定できるよう
    { Select0BtnObj, 30, 200, 50, 30, FontNormal, FontMagx12, (char*)BtnSelect0, WHITE, 0, },  // Alarm On
    { Select1BtnObj, 80, 200, 50, 30, FontNormal, FontMagx12, (char*)BtnSelect1, WHITE, 0, },  // Alarm Off
    
    //カレンダーの曜日の色設定の時だけ有効化かセレクタ　　　平日、土、日の色をそれぞれ設定できるよう
    { SelectWeekdayBtnObj, 20, 200, 40, 30, FontNormal, FontMagx12, (char*)BtnWeekday, WHITE, 0, },  // 平日
    { SelectSatBtnObj,     60, 200, 40, 30, FontNormal, FontMagx12, (char*)BtnSatday,  WHITE, 0, },  // 土曜
    { SelectSunBtnObj,    100, 200, 40, 30, FontNormal, FontMagx12, (char*)BtnSunday,  WHITE, 0, },  // 日曜
    
};

uint8_t TargetColor;    //ロータリで変化させる対象の色　0-2=R,G,B
uint8_t tRGB[3];        //各色に分けたカラーコード
uint16_t tempMultiRGB[3];   //複数色の設定が必要なアラーム、カレンダーの色を暫定格納
uint8_t Selector;   //アラーム、曜日の色を設定する時、どの項目の色を選択しているか


/*
 * 諧調のどの位置か、矢印を指定位置に描画
 *    前の矢印を消去してから矢印を描画
 * x,yは、矢印のとがった所を指定
 */
void DrawArrow(int16_t x, int16_t y, uint8_t rgb) {
    static int16_t preY[3];    //RGBそれぞれの矢印の位置を保持
    
    //前の矢印を消去してから矢印を描画
    display_fillRect(x - 13, preY[rgb] - 3, 14, 7, TextbgColor);
    preY[rgb] = y;
    
    display_fillTriangle(x, y, x-3, y-3, x-3, y+3, WHITE);
    display_fillRect(x-13, y-1, 10, 3, WHITE);
}

/*
 * タップしたり、ロータリ回したりして、カラーコード変化させた時に、各色の数値、矢印、現在の色を更新
 * color=0-2    RGBのどれかを更新
 */
void updateColorBar(uint8_t rgb) {
    uint8_t kk, yy;
    uint16_t color;
    char str[10];
    
    //当該色の値を各カラーバーの下に表示
    kk = RbarObj + rgb;
    
    //ロータリで動かす対象の値を赤色にする
    if (rgb == TargetColor) color = RED;
    else color = CurrentForm[kk].color;
    sprintf(str, "%3d", tRGB[rgb]);  //RGBの順
    display_drawChars(CurrentForm[kk].x, CurrentForm[kk].y+CurrentForm[kk].yw +8, str, color, TextbgColor, FontMagx12);
    
    //矢印を描画
    if (rgb==1) yy = (64 - tRGB[rgb])*2;  //Gは、6bit
    else yy = (32 - tRGB[rgb])*4;       //RBは、5bit
    //前の矢印を消去してから矢印を描画
    DrawArrow(CurrentForm[kk].x -2, CurrentForm[kk].y + yy-2, rgb);        
}

void updateColorBox() {
    uint8_t kk;
    uint16_t color;
    
    //現在の設定条件の色を右側のboxに表示
    kk = ColorBoxObj;
    color = (uint16_t)((tRGB[0] <<11) | (tRGB[1] <<5) | tRGB[2]);
    display_drawRect(CurrentForm[kk].x -1, CurrentForm[kk].y-1, CurrentForm[kk].xw +2, CurrentForm[kk].yw +2, WHITE);
    display_fillRect(CurrentForm[kk].x, CurrentForm[kk].y, CurrentForm[kk].xw, CurrentForm[kk].yw, color);

}


/*
 * 色設定用の初期画面を描画
 * 　変化ない部分なので、1回だけ表示すれば良いもの
 */
void DrawBaseForm() {
    char str[100];
    uint8_t jj, kk, mm;
    uint16_t color;
    
    SetFont(FontNormal);
    //タイトルと対象となるオブジェクト名を表示
    sprintf(str, "%s: %s", CurrentForm[0].str, CustomSettingText[TargetObj]);
    display_drawChars(CurrentForm[0].x, CurrentForm[0].y, str, CurrentForm[0].color, TextbgColor, CurrentForm[0].fontmag);
            
    kk = RbarObj;
    for (jj=0; jj<3; jj++) {
        //各カラーバーの名称　　バーの左上から、+10, -20を起点に描画
        display_drawChars(CurrentForm[kk].x+10, CurrentForm[kk].y-20, CurrentForm[kk].str, CurrentForm[kk].color, TextbgColor, CurrentForm[kk].fontmag);
        //カラーバーのグラデーション表示
        display_drawRect(CurrentForm[kk].x -1, CurrentForm[kk].y-1, CurrentForm[kk].xw +2, CurrentForm[kk].yw +2, WHITE);
        for (mm = 0; mm < CurrentForm[kk].yw; mm++) {
            color = mm/4;   //縦128ドットあるので、Gも含めて、5bitカラーとみなし、4ドット毎に輝度を変更していく
            if (jj==1) color = color << 6;  //Gは、6bitずらすと、ちょうど上位5bitが変化する形
            else if (jj==0) color = color << 11;    //Rは、11bitシフト
            display_fillRect(CurrentForm[kk].x, CurrentForm[kk].y + CurrentForm[kk].yw - mm-1, CurrentForm[kk].xw, 1, color);
        }
        kk++;
    }
    //設定した色を表示するActual Colorのboxの上に表示するタイトル
    jj = ColorBoxObj;
    display_drawChars(CurrentForm[jj].x +5, CurrentForm[jj].y -20, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);

    //ボタン描画  Cancel, OK
    for (jj = OKBtnObj; jj <= CancelBtnObj; jj++) {
        DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 10);
    }
    
}

/*
 * 色設定用の初期画面で変化ある部分を描画
 */
void DrawColorSettingForm() {
    uint8_t jj, kk;
    uint16_t fc, bc;

    //矢印、カラーの値、実際の色を描画    
    for (jj = 0; jj < 3; jj++) {
        updateColorBar(jj);
    }
    updateColorBox();
    
    if (TargetObj == AlarmObj) {
        //アラームの時は、セレクタでオン時とオフ時の色設定できるように。
        for (kk = Select0BtnObj; kk <= Select1BtnObj; kk++) {
            //ボタンの中央にボタン名を表示
            if (kk- Select0BtnObj == Selector) {
                //選択されている時
                fc = TextbgColor;
                bc = CurrentForm[kk].color;
            } else {
                //選択されていない時
                fc = CurrentForm[kk].color;
                bc = TextbgColor;
            }
            DrawButton(CurrentForm[kk], fc, bc, CurrentForm[kk].color, 0);
        }
    } else if (TargetObj == CalendarObj) {
        //カレンダーの曜日色の指定
        for (kk = SelectWeekdayBtnObj; kk <= SelectSunBtnObj; kk++) {
            //ボタンの中央にボタン名を表示
            if (kk-  SelectWeekdayBtnObj == Selector) {
                //選択されている時
                fc = TextbgColor;
                bc = CurrentForm[kk].color;
            } else {
                //選択されていない時
                fc = CurrentForm[kk].color;
                bc = TextbgColor;
            }
            DrawButton(CurrentForm[kk], fc, bc, CurrentForm[kk].color, 0);
        }
    }
}

/*
 * ロータリーは、連続的にイベント発生するが、イベント発生毎に処理できなくてもよく、
 * RotCount=0でない時だけ処理すれば良い
 * 対象の色の値を上下させる
 */
void RotaryProcedureColor() {
    uint8_t jj, kk;
    int16_t delta;

    //ノーマル時のロータリーSWはアラーム時刻を変更
    if (RotCount == 0) return;
    delta = RotCount;
    RotCount = 0;   //処理したら0に戻す

    jj = TargetColor;
    if (tRGB[jj] + delta < 0 ) {
        tRGB[jj] = 0; //最小値は0   
    } else {
        if (TargetColor == 1) kk = 2;   //Gは、6bitカラーなので2倍
        else kk = 1;

        if (tRGB[jj] + delta >= 32*kk) tRGB[jj] = 32*kk -1; //maxを超えないよう
        else tRGB[jj] = tRGB[jj] + (uint8_t)delta;
        
    }
    updateColorBar(jj);
    updateColorBox();

}

/*
 * 色設定バー上で、タッチされた時の処理
 */
void TouchProcedureColorSetting() {
    uint8_t jj, kk;
    int16_t yd;
    char str[100];

#ifdef DEBUG2
    sprintf(str, "T=%3d,%3d", TouchX, TouchY);
//        sprintf(str, "T=(%4d,%4d)(%3d,%3d)", TouchRawX, TouchRawY, TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, TextbgColor, FontMagx11);
#endif

    //カラーバーのエリアのどこをタッチしたかをチェック
    kk = RbarObj;    //カラーバーのRsc
    for (jj = 0; jj< 3; jj++) {
        if (ButtonPush(TouchX, TouchY, &CurrentForm[kk])) {
            yd = TouchY - CurrentForm[kk].y;
            if (jj==1) tRGB[jj] = (uint8_t)(63 - yd/2); //Gは、64階調を2ドット毎
            else tRGB[jj] = (uint8_t)(31 - yd/4);       //RBは、32階調を4ドット毎
            TargetColor = jj;   //対象の色を指定
            updateColorBar(jj);
            updateColorBox();
            RepeatKey = 1;
            return;
        }
        kk++;
    }
    
    tempMultiRGB[Selector] = (uint16_t)((tRGB[0] <<11) | (tRGB[1] <<5) | tRGB[2]);   //元のデータを格納
    if (ButtonPush(TouchX, TouchY, &CurrentForm[OKBtnObj])) {
        //OKボタン押した時の処置　　アラーム、カレンダー、その他に関わらず以下で統一
        TempObjData[TargetObj].color = tempMultiRGB[0];
        TempObjData[TargetObj].attribute = tempMultiRGB[1]; //アラームオフの色は、attributeに
        //土曜の色は、attributeに。
        TempObjData[TargetObj].attribute2 = tempMultiRGB[2];    //日曜の色は、attribute2に
        
        ReturnEventHandler();
        PushEvent(EventFormInit);
    
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[CancelBtnObj])) {
        //Cancel押された時
        ReturnEventHandler();
        PushEvent(EventFormInit);
        
    } else {
        //セレクタボタンがあるAlarmとカレンダーのタブの処理
        if (TargetObj == AlarmObj) {
            for (jj = Select0BtnObj; jj <= Select1BtnObj; jj++) {
                if (ButtonPush(TouchX, TouchY, &CurrentForm[jj])) {
                    Selector = jj - Select0BtnObj;
                    PushEvent(EventColorFormUpdate);
                    return;
                }
            }
        } else if (TargetObj == CalendarObj) {
            for (jj = SelectWeekdayBtnObj; jj <= SelectSunBtnObj; jj++) {
                //平日、土曜、日曜
                if (ButtonPush(TouchX, TouchY, &CurrentForm[jj])) {
                    Selector = jj - SelectWeekdayBtnObj;
                    PushEvent(EventColorFormUpdate);
                    return;
                }
            }
        }
    }
}


/*
 * 色設定用イベントハンドラ
 * 
 */
void ColorSettingEventHandler() {
    uint8_t evt, jj, kk;
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
        case EventColorFormInit:
            lcd_fill(TextbgColor);
            //元のデータをtemp変数に設定
            CurrentForm = ColorSettingRsc;
            tempMultiRGB[0] = TempObjData[TargetObj].color;
            tempMultiRGB[1] = TempObjData[TargetObj].attribute;
            tempMultiRGB[2] = TempObjData[TargetObj].attribute2;

            Selector = 0;   //初期値は、0が変更ターゲット　　オブジェクトにより複数ある時も
            DrawBaseForm();
            //以降、Updateと処理共通化
        case EventColorFormUpdate:
            //Selectorを変更した時はここから
            tRGB[0] = tempMultiRGB[Selector] >> 11;            //R
            tRGB[1] = (tempMultiRGB[Selector] >> 5) & 0x3f;    //G
            tRGB[2] = tempMultiRGB[Selector] & 0x1f;           //B
            TargetColor = 0;    //Redを初期時、対象の色とする
            DrawColorSettingForm();
            break;
            
        case EventRotary:
            RotaryEventCount--;
            RotaryProcedureColor();
            break;
        case EventTouchDown:
            //当該色の値を白で表示
            kk = RbarObj;
            for (jj = 0; jj < 3; jj++) {
                sprintf(str, "%3d", tRGB[jj]);  //RGBの順
                display_drawChars(CurrentForm[kk].x, CurrentForm[kk].y+CurrentForm[kk].yw +8, str, CurrentForm[kk].color, TextbgColor, FontMagx12);
                kk++;
            }
        case EventTouchRepeat:
            TouchProcedureColorSetting();
            break;
    }
}
