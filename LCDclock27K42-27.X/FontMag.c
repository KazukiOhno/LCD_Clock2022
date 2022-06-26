/*
 * File:   FontMag.c
 * Author: ohno
 *
 * Created on June 1, 2022, 9:58 PM
 */
/*
 * フォントサイズの選択肢をプルダウンにしていたが、もっと種類を増やしたい
 * スクロールバーが欲しいが難しいので、別フォームで実行する方式に
 * 
 */

#include "FontMag.h"
#include "main.h"
#include "LCD320x240color.h"
#include "Draw.h"
#include "Customizing.h"
#include "FontSelect.h"
#include "ColorSetting.h"


enum {
    TitleMagObj,
    FontDisp,
    MagXdecObj,
    MagXObj,
    MagXincObj,
    MagYincObj,
    MagYObj,
    MagYdecObj,
    FontSelObj,
    ColorObj,
    BtnClose,
    XpObj,
    XpdecObj,
    XpincObj,
    FormatObj,
};

const char TitleMagForm1[] = "Font Magnification";
const char TextFontSel[] = "Font";
const char TextColor[] = "Color";
char TextXp[10];
const char TextXpdec[] = "-";
const char TextXpinc[] = "+";
const char TextFormat[] = "Format";
uint8_t TempMagX, TempMagY;
uint8_t PopUp;  //ポップアップ中なら、1


MainFormRsc MagForm[] = {
    //ID,           x,    y, xw,  yw, fontcode,   fontMag,    str,                   color, format, attribute, attribute2, xpitchAdj
    { TitleMagObj,  0,    0,  0,   0, FontNormal, FontMagx12, (char*)TitleMagForm1, WHITE, 0, },   //タイトル
    
    { FontDisp,     5,   20, 275,170, FontNormal, FontMagx11, NULL, WHITE, 0, },   // 文字表示領域
    
    { MagXdecObj,  80,  190,  20, 20, FontNormal, 0,          NULL, WHITE, 0, },   // ←
    { MagXObj,    130,  195,  20, 20, FontNormal, FontMagx22, NULL, WHITE, 0, },   //X mag
    { MagXincObj, 180,  190,  20, 20, FontNormal, 0,          NULL, WHITE, 0, },   // →

    { MagYincObj, 295,   40,  20, 20, FontNormal, 0,          NULL, WHITE, 0, },   // ▲
    { MagYObj,    295,   95,  20, 20, FontNormal, FontMagx12, NULL, WHITE, 0, },   //Y mag
    { MagYdecObj, 295,  140,  20, 20, FontNormal, 0,          NULL, WHITE, 0, },   // ▼
    
    //ボタン　　                                                                     color, format, 
    { FontSelObj, 200,    0,  50, 20, FontNormal, FontMagx12, (char*)TextFontSel,   WHITE, 0, },    //フォント選択
    { ColorObj,   260,    0,  50, 20, FontNormal, FontMagx12, (char*)TextColor,     WHITE, 0, },    //色選択
    
    { BtnClose,   240,  213,  70, 25, FontNormal, FontMagx12, (char*)CloseText,     WHITE, 0, },  // Close
    //ボタン上にXピッチ表示
    { XpObj,       20,  212,  70, 25, FontNormal, FontMagx12, (char*)TextXp,    WHITE, 0, },    //Xpitch選択
    { XpdecObj,     0,  217,  20, 25, FontNormal, FontMagx22, (char*)TextXpdec, WHITE, 0, },   // -
    { XpincObj,    92,  217,  20, 25, FontNormal, FontMagx22, (char*)TextXpinc, WHITE, 0, },   // +
    //フォーマット
    { FormatObj,  120,  212,  70, 25, FontNormal, FontMagx12, (char*)TextFormat, WHITE, 0, },   //Format選択
    
};


/*
 * フォントの大きさを目で見てわかるように表示
 * color指定して、消去対応
 */
void drawPreview(uint16_t color) {
    
    SetFont(TempObjData[TargetObj].fontcode);
    PreviewFont(CurrentForm[FontDisp].x+2, CurrentForm[FontDisp].y+2, (uint8_t)(TempMagX << 4) + TempMagY, TempObjData[TargetObj].xpitchAdj, color);
    SetFont(FontNormal);  //デフォルトのフォントに戻す

}

/*
 * Xmag、Ymagの値を描画
 */
void drawMagNumber(uint8_t mag, uint8_t obj) {
    char str[5];
    
    sprintf(str, "%2d", mag);
    display_drawChars(CurrentForm[obj].x, CurrentForm[obj].y, str, CurrentForm[obj].color, TextbgColor, CurrentForm[obj].fontmag);
    
}

/*
 * ボタン上に現在の値を表記
 */
void drawXpitchButton() {
    
    sprintf(TextXp, "Xp=%2d", TempObjData[TargetObj].xpitchAdj);
    
    CurrentForm[XpObj].str = TextXp;
    DrawButton(CurrentForm[XpObj], CurrentForm[XpObj].color, TextbgColor, CurrentForm[XpObj].color, 0);

}

void drawFormatButton() {
    
//    sprintf(TextXp, "Format=%2d", TempObjData[TargetObj].format);
//    CurrentForm[XpObj].str = TextXp;
    DrawButton(CurrentForm[FormatObj], CurrentForm[FormatObj].color, TextbgColor, CurrentForm[FormatObj].color, 0);

}

/*
 * フォント倍率設定用フォーム描画
 */
void DrawFontMagForm(uint8_t cls) {
    uint8_t jj;
    int16_t x[3], y[3];

    if (cls) lcd_fill(BLACK);

    //プレビューするbox    
    display_drawRect(CurrentForm[FontDisp].x, CurrentForm[FontDisp].y, CurrentForm[FontDisp].xw, CurrentForm[FontDisp].yw, GREY);
    drawPreview(TempObjData[TargetObj].color);
    
    //タイトル描画
    jj = TitleMagObj;
    display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    
    drawMagNumber(TempMagX, MagXObj);
    drawMagNumber(TempMagY, MagYObj);
    
    //X方向　左三角
    x[0] = CurrentForm[MagXdecObj].x;
    y[0] = CurrentForm[MagXdecObj].y+CurrentForm[MagXdecObj].yw/2;
    x[1] = CurrentForm[MagXdecObj].x+CurrentForm[MagXdecObj].xw;
    y[1] = CurrentForm[MagXdecObj].y;
    x[2] = x[1];
    y[2] = CurrentForm[MagXdecObj].y+CurrentForm[MagXdecObj].yw;
    display_fillTriangle(x[0], y[0], x[1], y[1], x[2], y[2], GREY);
    display_drawTriangle(x[0], y[0], x[1], y[1], x[2], y[2], WHITE);
    //X方向　右三角
    x[0] = CurrentForm[MagXincObj].x;
    y[0] = CurrentForm[MagXincObj].y;
    x[1] = x[0];
    y[1] = CurrentForm[MagXincObj].y+CurrentForm[MagXincObj].yw;
    x[2] = CurrentForm[MagXincObj].x+CurrentForm[MagXincObj].xw;
    y[2] = CurrentForm[MagXincObj].y+CurrentForm[MagXincObj].yw/2;
    display_fillTriangle(x[0], y[0], x[1], y[1], x[2], y[2], GREY);
    display_drawTriangle(x[0], y[0], x[1], y[1], x[2], y[2], WHITE);
    //Y方向　▲
    x[0] = CurrentForm[MagYincObj].x+CurrentForm[MagYincObj].xw/2;
    y[0] = CurrentForm[MagYincObj].y;
    x[1] = CurrentForm[MagYincObj].x;
    y[1] = CurrentForm[MagYincObj].y+CurrentForm[MagYincObj].yw;
    x[2] = CurrentForm[MagYincObj].x+CurrentForm[MagYincObj].xw;
    y[2] = y[1];
    display_fillTriangle(x[0], y[0], x[1], y[1], x[2], y[2], GREY);
    display_drawTriangle(x[0], y[0], x[1], y[1], x[2], y[2], WHITE);
    //Y方向　▼
    x[0] = CurrentForm[MagYdecObj].x;
    y[0] = CurrentForm[MagYdecObj].y;
    x[1] = CurrentForm[MagYdecObj].x+CurrentForm[MagYdecObj].xw;
    y[1] = y[0];
    x[2] = CurrentForm[MagYdecObj].x+CurrentForm[MagYdecObj].xw/2;
    y[2] = CurrentForm[MagYdecObj].y+CurrentForm[MagYdecObj].yw;
    display_fillTriangle(x[0], y[0], x[1], y[1], x[2], y[2], GREY);
    display_drawTriangle(x[0], y[0], x[1], y[1], x[2], y[2], WHITE);
    
    //ボタン描画  Font, Color, Close    OK/Cancelは止める　　Cancel処理のために別途変数を持っておかねばならないので
    for (jj = FontSelObj; jj <= BtnClose; jj++) {
        DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 10);
    }

    // Xピッチ -/+
    for (jj = XpdecObj; jj <= XpincObj; jj++) {
        display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    }
    //Xpitchボタン
    drawXpitchButton();
    //フォーマットボタン
    drawFormatButton();
    
}

/*
 * 仮のMagをオブジェクトデータに格納
 */
void setMag() {
    TempObjData[TargetObj].fontmag = (uint8_t)(TempMagX << 4) + TempMagY;
}

/*
 * タッチの処理
 */
void TouchProcedureFontMag() {

    if (ButtonPush(TouchX, TouchY, &CurrentForm[MagXdecObj])) {
        if (TempMagX > 1) {
            drawPreview(BLACK);
            TempMagX--;
            PushEvent(EventFormUpdate);     //フォントサイズによって、ボタン類が消えてしまうので、全部描画させる
        }
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[MagXincObj])) {
        if (TempMagX < 15) {
            drawPreview(BLACK);
            TempMagX++;
            PushEvent(EventFormUpdate);     //フォントサイズによって、ボタン類が消えてしまうので、全部描画させる
        }
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[MagYdecObj])) {
        if (TempMagY > 1) {
            drawPreview(BLACK);
            TempMagY--;
            PushEvent(EventFormUpdate);     //フォントサイズによって、ボタン類が消えてしまうので、全部描画させる
        }
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[MagYincObj])) {
        if (TempMagY < 15) {
            drawPreview(BLACK);
            TempMagY++;
            PushEvent(EventFormUpdate);     //フォントサイズによって、ボタン類が消えてしまうので、全部描画させる
        }
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[XpdecObj])) {
        //ポップアップの選択肢より広め設定可
        if (TempObjData[TargetObj].xpitchAdj > -10) {
            drawPreview(BLACK);
            TempObjData[TargetObj].xpitchAdj--;
            drawXpitchButton();
            PushEvent(EventFormUpdate);     //フォントサイズによって、ボタン類が消えてしまうので、全部描画させる
        }
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[XpincObj])) {
        //ポップアップの選択肢より広め設定可
        if (TempObjData[TargetObj].xpitchAdj < 15) {
            drawPreview(BLACK);
            TempObjData[TargetObj].xpitchAdj++;
            drawXpitchButton();
            PushEvent(EventFormUpdate);     //フォントサイズによって、ボタン類が消えてしまうので、全部描画させる
        }
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnClose])) {
        //設定されたサイズを格納
        setMag();
        ClearBuffer();
        SetEventHandler(CustomizingEventHandler);
        PushEvent(EventFormInit);
        return;
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[FontSelObj])) {
        //設定されたサイズを格納
        setMag();
        PopupEventHandler(FontSelectEventHandler);
        PushEvent(EventFormInit);
        return;
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[ColorObj])) {
        //設定されたサイズを格納
        setMag();
        PopupEventHandler(ColorSettingEventHandler);
        PushEvent(EventColorFormInit);
        return;

    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[XpObj])) {
        //Xピッチのポップアップ
        TargetItem = ItemXpitch;    //フォントピッチ
        //対象のobjが押されていたら
        SelectedNo = (uint8_t)(TempObjData[TargetObj].xpitchAdj + 2);   //現在値
        drawPopupForm(MAXXpitchNum, (char **)FontXpitchText, (char*)CustomSettingText[TargetObj], CurrentForm[XpObj].x+20);
        PopUp = 1;
        return;
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[FormatObj])) {
        //フォーマットのポップアップ
        TargetItem = ItemFormat;    //フォーマット
        //対象のobjが押されていたら
        SelectedNo = TempObjData[TargetObj].format;   //現在値
        drawPopupForm(FormatNum[TargetObj], (char **)FormatTextList[TargetObj], NULL, CurrentForm[FormatObj].x+20);
        PopUp = 1;
        return;
    }
}


void FontMagEventHandler() {
    uint8_t evt;
//    char str[100];
    
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
        case EventFormUpdate:
            DrawFontMagForm(0);     //画面消去なし
            break;
        case EventFormInit:
            SetFont(FontNormal);  //初期フォント設定
            CurrentForm = MagForm;
            TempMagX = TempObjData[TargetObj].fontmag >> 4;
            TempMagY = TempObjData[TargetObj].fontmag & 0x0f;
            DrawFontMagForm(1);     //画面消去あり
            PopUp = 0;  //ポップアップ中
            break;
        case EventRotary:
            break;
            
        case EventTouchRepeat:
            TouchProcedureFontMag();
            break;
            
        case EventTouchDown:
            if (PopUp) {
                drawPreview(BLACK);     //元のものを消す
                TouchProcedurePopup();   //Customizing内のルーチンを利用
                drawPreview(TempObjData[TargetObj].color);  //新しい設定で描画
            }
            else {
                TouchProcedureFontMag();
                RepeatKey = 1;  //リピート有効化                            
            }
            break;            
    }
}
