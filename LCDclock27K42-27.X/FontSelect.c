/*
 * SDカード上のフォルダをフォントフォルダと想定して、リスト表示
 * そのフォルダ名をフォント名として取り込む
 * 
 */

#include "FontSelect.h"
#include <string.h>

#include "main.h"
#include "font.h"
#include "LCD320x240color.h"
#include "Customizing.h"
#include "FontMag.h"
#include "Draw.h"


enum FontSelectionRscObject {
    TitleFontSelObj,
    BtnOKObj,
    BtnCancelObj,
    
    BtnObj,         //SDカード上のフォントリスト max.28
    BtnInternalObj, //内蔵フォント
};

const char TitleFontSelection[] = "Select Font for ";   //forの後ろに対象のオブジェクト名

//フォントをテーブルの形で、リスト化　　　4列*7行を想定
#define FontTableX 10
#define FontTableY 21
#define FolderW 75
#define FolderH 20

MainFormRsc FontSelRsc[] = {
    //ID,              x,   y,   xw, yw, fontcode,   fontMag,    str,                color,
    { TitleFontSelObj, 5,   3,    0,  0, FontNormal, FontMagx12, (char*)TitleFontSelection, WHITE, },   //タイトル
    //ボタン
    { BtnOKObj,      205, 200,   40, 30, FontNormal, FontMagx12, (char*)BtnNameOK,     WHITE, },  // OK
    { BtnCancelObj,  250, 200,   60, 30, FontNormal, FontMagx12, (char*)BtnNameCancel, WHITE, },  // Cancel
    //フォント名テーブルを1つのオブジェクトとして定義
    { BtnObj,         FontTableX, FontTableY,           FolderW*4+4, FolderH*8+4, FontNormal, FontMagx12, NULL, WHITE, },   // フォルダ
    //内蔵フォントの表示エリア
    { BtnInternalObj, FontTableX, FontTableY+FolderH*7, FolderW*4+4, FolderH+4,   FontNormal, FontMagx12, NULL, WHITE, },   // 内蔵フォント
    
};

#define MaxFontNum  28
uint8_t FontNum;
char fontNameList[30][9];   //フォント名のリスト　　　最大28個までに制限する

/*
 * SDカードからフォルダを読み出す (フォントフォルダ以外も対象に入ってしまうが気にしない)
 * フォントディレクトリリストを表示する
 * 1フォント=FolderW x FolderHの領域に記入　　横4個、縦7個の28個までに制限する
 * 取得したフォルダ名=フォント名は、fontNameListへ格納
 */
FRESULT fontFolder(char* path) {
    FRESULT res;
    FILINFO fno;
    FFDIR dir;
    char *fn;   /* 非Unicode構成を想定 */
    int16_t xx, yy;

    res = f_opendir(&dir, path);                       /* ディレクトリを開く */
    if (res == FR_OK) {
        yy = CurrentForm[BtnObj].y;
        xx = 0;
        FontNum = 0;
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* ディレクトリ項目を1個読み出す */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* エラーまたは項目無しのときは抜ける */
            if (fno.fname[0] == '.') continue;             /* ドットエントリは無視 */
            fn = fno.fname;
            if (fno.fattrib & AM_HID) {
                // Hiddenファイルは表示しない
            } else if (fno.fattrib & AM_DIR) {          /* ディレクトリ */
                sprintf(fontNameList[FontNum], "%s", fn);     //ディレクトリ=フォントフォルダ名
                display_drawChars(CurrentForm[BtnObj].x + xx, yy, fontNameList[FontNum], CurrentForm[BtnObj].color, TextbgColor, CurrentForm[BtnObj].fontmag);
                FontNum++;
                if (xx < 200) xx += FolderW;
                else {
                    xx = 0;
                    yy += FolderH;                    
                }
//            } else {                                       /* ファイル */
//                printf("%s/%s\n", path, fn);
            }
            if (FontNum >= MaxFontNum) break;   //現状フォントファイルの最大を制限。画面表示しきれないため
        }
    }
    return res;
}


/*
 * プレビューによりボタンが消えてしまうことがあるため
 */
void drawButton() {
    uint8_t jj;

    //ボタン描画
    for (jj = BtnOKObj; jj <= BtnCancelObj; jj++) {
        DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 10);
    }
}

/*
 * SDカード内のフォントリストを表示する
 */
void drawFontSelection() {
    uint8_t jj;
    int16_t xx, yy;
    char str[100];
    
    //枠描画
    display_fillRoundRect(0,  0, 320, 240, 10, BLACK);
    display_drawRoundRect(0,  0, 320, 240, 10, WHITE);

    SetFont(FontNormal);  //初期フォント設定

    //タイトル表示
    jj = TitleFontSelObj;
    sprintf(str, "%s%s", CurrentForm[jj].str, CustomSettingText[TargetObj]);
    display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);

    //ボタン描画
    drawButton();

    //枠を描画
    display_drawRoundRect(CurrentForm[BtnObj].x-3, CurrentForm[BtnObj].y-3, CurrentForm[BtnObj].xw, CurrentForm[BtnObj].yw, 0, GREY);
    display_drawRoundRect(CurrentForm[BtnInternalObj].x-3, CurrentForm[BtnInternalObj].y-3, CurrentForm[BtnInternalObj].xw, CurrentForm[BtnInternalObj].yw, 0, GREY);
    
    //フォントディレクトリをルート以下で探す
    strcpy(str, "/");
    fontFolder(str);
    
    //内蔵フォントを提示
    yy = CurrentForm[BtnInternalObj].y;
    xx = CurrentForm[BtnInternalObj].x;
    for (jj = 0; jj < InternalFontNum; jj++) {
        sprintf(str, "%s*", FontFolder[jj]);
        display_drawChars(xx, yy, str, CurrentForm[BtnInternalObj].color, TextbgColor, CurrentForm[BtnInternalObj].fontmag);
        xx += FolderW;
    }
    
}


/*
 * 指定されたフォントで、対象のオブジェクトに近い形で表示して見せる
 */
void PreviewFont(int16_t x, int16_t y, uint8_t fontmag, int8_t xpitch, uint16_t color) {
    static char str[20];
    
    display_setTextSize(fontmag);
    display_setTextPitch(xpitch, 0);
    display_setCursor(x, y);        //描画開始座標
    display_setColor(color);
    
    switch (TargetObj) {
        case DateObj:
            sprintf(str, "22/4/5(%c)", FSunday);
            break;
        case TimeObj:
            //smallフォントもプレビューできるように
            sprintf(str, "2:3");
            display_puts(str);
            
            sprintf(str, "56");
            sprintf(Smaller, "s");  //小さいフォント指定
            break;
        case AlarmObj:
            sprintf(str, "ALM+06:30");
            break;
        case TempObj:
            sprintf(str, "-12.3%c", FdegC);
            break;
        case HumidityObj:
            sprintf(str, "45%%");
            break;
        default:
            sprintf(str, "10%c11%c", FSunday +1, FSunday +1);
            break;
    }
    display_puts(str);
    Smaller[0] = '\0';  //元に戻す
    display_setTextPitch(0, 0); //元に戻す
    
}
                        

/*
 * タッチした時の処置
 * 　座標によって処理内容を特定
 */
void TouchProcedureFontSelect() {
    int16_t xx, yy;
    uint8_t selectFontNum, newfont;
    static int16_t prevX = FontTableX-2;
    static int16_t prevY = FontTableY-2;
    
     if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnObj])) {
        xx = (TouchX - CurrentForm[BtnObj].x) / FolderW;
        yy = (TouchY - CurrentForm[BtnObj].y) / FolderH;
                
        selectFontNum = (uint8_t)(xx + yy * 4);     //選択されたフォント番号
        
        PreviewFont(10, FontTableY+FolderH*8+5, FontMagx11, 0, BLACK);  //前の文字を消す
        
        //タップしたフォントに枠を付ける
        display_drawRect(prevX, prevY, FolderW, FolderH, BLACK);    //前の枠を消す
        //新たに選択したフォントの座標を保持し、枠を描画
        prevX = xx*FolderW + CurrentForm[BtnObj].x-2;
        prevY = yy*FolderH + CurrentForm[BtnObj].y-2;

        if (selectFontNum < FontNum) {
            strcpy(CustomFontDir[FontTemp - FontUser0], fontNameList[selectFontNum]);
            newfont = FontTemp;
        } else if (selectFontNum >= MaxFontNum && selectFontNum < MaxFontNum+ InternalFontNum) {
            //内蔵フォントは、3つ。画面タッチで取得できるフォント番号は、MaxFontNum以降
            newfont = selectFontNum - MaxFontNum;
            //本来フォント名を格納するが、内蔵フォントコードを[0]に格納
            CustomFontDir[FontTemp - FontUser0][0] = selectFontNum - MaxFontNum;    //'\0';
        } else {
            return;
        }
        
        display_drawRect(prevX, prevY, FolderW, FolderH, WHITE);    //新たに枠を描画
        
        SetFont(newfont);
        //選択したフォントをプレビュー
        PreviewFont(10, FontTableY+FolderH*8+5, FontMagx11, 0, WHITE);
        SetFont(FontNormal);  //
        drawButton();   //プレビューでボタンが消えることがあるため
        SetFont(newfont);
        
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnCancelObj])) {
        ClearBuffer();
        ReturnEventHandler();
        PushEvent(EventFormInit);
        
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnOKObj])) {
        //OKの時、フォント名=フォルダ名をターゲットのオブジェクトのデータとする
        if (CustomFontDir[FontTemp - FontUser0][0] >= ' ') {
            strcpy(CustomFontDir[TargetObj], CustomFontDir[FontTemp - FontUser0]);
            TempObjData[TargetObj].fontcode = (uint8_t)(FontUser0 + TargetObj);
        } else {
            TempObjData[TargetObj].fontcode = CustomFontDir[FontTemp - FontUser0][0];
        }
        
        ClearBuffer();
        ReturnEventHandler();
        PushEvent(EventFormInit);
    }    
}

/*
 * フォントを選択するフォームのイベント処理
 */
void FontSelectEventHandler() {
    uint8_t evt;
    
    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //イベントない時は、無駄に空回りさせないよう、10msのwait
        return;
    }
    
    switch (evt) {
        case EventFormInit:
            CurrentForm = FontSelRsc;
            drawFontSelection();            
            break;
            
        case EventTouchDown:
            TouchProcedureFontSelect();
            break;
    }
    
}
