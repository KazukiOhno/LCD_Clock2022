/*
 * カスタマイズ関係の処置
 */

#include "Customizing.h"

#include <string.h>

#include "main.h"
#include "font.h"
#include "LCD320x240color.h"
#include "AlarmSound.h"
#include "Draw.h"
#include "RTC8025.h"
#include "CustomData.h"
#include "FontSelect.h"
#include "ColorSetting.h"
#include "FontMag.h"
#include "AlarmSound.h"


uint8_t TargetObj;  //どのオブジェクトを変更しようとしているか　　DateObjとかTimeObjとか
uint8_t TargetItem; //どの項目を対象としているか　　FontとかSizeとか

//時刻、日付の設定変更

//プログラムしやすいように配列に。並びは、下記リソースと同じDate,Time,Alarm,Temp,Humidity
//設定時にテンポラリに保持する、OKならCustomRscに書込む
ObjectRsc TempObjData[OBJECTNUM];   //カスタマイズする情報を保持
ObjectRsc tempRsc[OBJECTNUM];    //暫定リソース

uint16_t ItemFColor;
//ポップアップ
int16_t PopX, PopY, PopXW, PopYW, Yoffset;
uint8_t PopUp;  //ポップアップ中なら、1

#define FRAMESIZE 4     //ポップアップの枠サイズ


//カスタマイズ用の画面表示要リソース
//下記オブジェクトの順にリソースを設定する
enum CustomSettingRscObject {
    TitleSettingObj,
    SubTitleSettingObj,
    //ボタン
    SettingBtnSaveObj,
    SettingBtnLoadObj,
    SettingBtnOKObj,
    SettingBtnCancelObj,
    //オブジェクト名
    SettingDateObj,
    SettingTimeObj,
    SettingAlarmObj,
    SettingTempObj,
    SettingHumidityObj,
    SettingCalendarObj,
    SettingGearObj,
};

//enum ItemNum {
//    ItemFont,
//    ItemFontSize,
//    ItemXpitch,
//    ItemColor,
//    ItemXY,
//    ItemFormat,
//};

const char CustomTextTitle1[] = "Customizing";
const char CustomTextTitle2[] = "Item  Font  Size Xp Color  X,  Y  fmt";
const char CustomSettingText[][6] = { "Date", "Time", "Alarm", "Temp", "Humid", "Calen", "Gear", };
const char BtnNameSave[] = "Save";
const char BtnNameLoad[] = "Load";
const char BtnNameOK[] = "OK";
const char BtnNameCancel[] = "Cancel";
const char CloseText[] = "Close";

//各列のX座標
#define XCheckbox   0
#define XItem       20
#define XFont       65
#define XFSize      130
#define XPitch      155
#define XColor      185
#define XPos        215
#define XFormat     300

//各列の幅
#define XCheckboxW  (XItem - XCheckbox)
#define XItemW      (XFont - XItem)
#define XFontW      (XFSize - XFont)
#define XFSizeW     (XPitch - XFSize)
#define XPitchW     (XColor - XPitch)
#define XColorW     (XPos - XColor)
#define XPosW       (XFormat - XPos)
#define XFormatW    (LCD_WIDTH- XFormat)

//各行の高さ
#define ItemH   20


/*
 * カスタマイズ用の画面構成
 *    Item    Font Mag Xp Color Position format　フォーマット
 *    Date     o    o   o  o     o        o　　　9種
 *    Time     o    o   o  o     o        o　　　デジタル3種アナログ1種　24時間表示のみ
 *    Alarm    o    o   o  o     o        o　　　2種=24時間表記/12時間　　オフ時の色指定も可
 *    Temp     o    o   o  o     o        o　　　2種　華氏/摂氏
 *    Humi     o    o   o  o     o        X　　　
 *    Calendar o    o   o  o     o        o　　　5種　　色指定は、平日、土日それぞれ可
 *    Gear     X    X   X  o     o        X
 *
 * 　　フォントは、内蔵2種。ノーマル、スモール
 * 　　　　スモールは、カレンダー用に
 * 　　　SDカード上にユーザフォントを格納して対応可。データはモノクロbmp形式で保存。
 * 　　　　7セグは、時刻用に数字、コロンを定義。このデータはSDカード上に配置
 * 　追加の装飾は、attributeを使う
 *               attribute     attribute2
 *    Time       サイズ                        アナログ時計
 *    Alarm      オフ時の色　　　　　　　　　　　オン時の色はcolorに設定
 *    Calendar   日曜の色       土曜の色　　　　平日の色は、colorに設定
 * 
 */
//設定画面用リソース
MainFormRsc SettingRsc2[] = {
    //ID,                 x,    y, xw,yw, fontcode,   fontMag,    str,                   color, format, attribute, attribute2, xpitchAdj
    { TitleSettingObj,    0,     0,  0,0, FontNormal, FontMagx22, (char*)CustomTextTitle1, WHITE, 0, },   //タイトル
    { SubTitleSettingObj, XItem, 20, 0,0, FontNormal, FontMagx12, (char*)CustomTextTitle2, WHITE, 0, },   //各項目名

    //ボタン　　                                                                                     attribute:背景色
    { SettingBtnSaveObj,   10, 210, 50, 25, FontNormal, FontMagx12, (char*)BtnNameSave,   WHITE, 0, BLACK, },  // Save
    { SettingBtnLoadObj,   80, 210, 50, 25, FontNormal, FontMagx12, (char*)BtnNameLoad,   WHITE, 0, BLACK, },  // Load
    { SettingBtnOKObj,    160, 210, 60, 25, FontNormal, FontMagx22, (char*)BtnNameOK,     BLACK, 0, WHITE, },  // OK
    { SettingBtnCancelObj,240, 210, 60, 25, FontNormal, FontMagx12, (char*)BtnNameCancel, BLACK, 0, WHITE, },  // Cancel

    //項目名: IDを使用/未使用のフラグに使う
    { Enable,   XItem, 40, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[0], WHITE, 0, },     //Date
    { Enable,   XItem, 60, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[1], WHITE, 0, },     //Time
    { Enable,   XItem, 80, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[2], WHITE, 0, },     //Alarm
    { Enable,   XItem,100, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[3], WHITE, 0, },     //Temp
    { Enable,   XItem,120, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[4], WHITE, NoData, }, //Humidity
    { Enable,   XItem,140, XItemW, ItemH, FontNormal, FontMagx12, (char*)CustomSettingText[5], WHITE, 0, },     //Calendar
    { Disable,  XItem,160, XItemW, ItemH, NoData8,    FontMagx12, (char*)CustomSettingText[6], WHITE, NoData, 0, 0, NoData8, },

};


char NameFontNormal[] = "Normal";
char NameFontSmall[] = "Small";
char NameFont7like[] = "7like";         //SDカードに頼らないよう、内部7セグを用意
char NameFont7seg[] = "7segsl";           //カスタムフォントと同様の処理をするが、デフォルトで設定
char NameFont7segSmall[] = "7segslS";     //カスタムフォントと同様の処理をするが、デフォルトで設定
char FontOther[] = "Custom...";

#define NumberOfFont    InternalFontNum+1   //項目数=ポップアップの行数

//カスタマイズでポップアップで表示するフォント名
//内蔵フォントと、外部カスタムフォントかという選択肢に変更
//カスタムフォント名はpopupには示さず、別ウインドウで選択する形
char *UserFontName[NumberOfFont] = {
    NameFontNormal,   //実際には内蔵フォントなので、固定名
    NameFontSmall,
    NameFont7like,
    FontOther,
};


//フォントサイズの選択肢
//格納するコードとポップアップの行番号の対照表
//2つめを選択したらx12のサイズだと認識するのは簡単。
//逆に現在の設定がx13だった時、何番目かは、検索しないと3番目と認識できない
uint8_t FontMag[] = {
    FontMagx11,
    FontMagx12,
    FontMagx13,
    FontMagx21,
    FontMagx22,
    FontMagx23,
    FontMagx32,
    FontMagx33,
    FontMagOther,
};
//ポップアップに表示するテキスト
const char *FontMagText[] = {
  "11:x*1,y*1",
  "12:x*1,y*2",
  "13:x*1,y*3",
  "21:x*2,y*1",
  "22:x*2,y*2",
  "23:x*2,y*3",
  "32:x*3,y*2",
  "33:x*3,y*3",
  "Other"
};

//X方向ピッチ調整の選択肢
//設定画面のPopup表示用テキスト
const char *FontXpitchText[] = {
  "default -2",
  "default -1",
  "default",
  "default +1",
  "default +2",
  "default +3",
  "default +4",
  "default +5",
  "default +6",
  "default +7",
  "default +8",
};

//日付の表示形式　DateFormatと順序同一
const char *DateFormatText[] = {
    "0 YYYY/MM/DD(w)",    //曜日付き
    "1 YYYY/MM/DD(we)",   //英語曜日付き
    "2 YYYY/MM/DD",
    "3 YY/MM/DD(w)",
    "4 YY/MM/DD(we)",
    "5 YY/MM/DD",
    "6 MM/DD(w)",
    "7 MM/DD(we)",
    "8 MM/DD",
};
     
//時刻表示の形式　順序はTimeFormatと同じ　　　　12/24表示は対応できていない
const char *TimeFormatText[] = {
    "0 HH:MM",
    "1 HH:MMss",
    "2 HH:MM:SS",
    "A ANALOG",
};

//アナログ時計の半径サイズ　　xwには、この2倍の数値が格納される
uint8_t AnalogClockSize[] = {
    90,
    80,
    70,
    60,
    50,
};
//アナログ時計の大きさ　表記は、感覚的に　　AnalogClockFormatと順序同一
//データは、attributeに格納
const char *AnalogClockSizeText[] = {
    "Large",  //"r=90",
    "Mid L",  //"r=80",
    "mid S",  //"r=70",
    "Small",  //"r=60",
    "Tiny",   //"r=50",
};

//アラーム表示の形式　AlarmFormatと順序同一
const char *AlarmFormatText[] = {
    "12h ALM AM/PM",
    "24h ALM",
};

//温度の表示形式　TempFormatと順序同一
const char *TempFormatText[] = {
    "C DegC",
    "F DegF",
};

//カレンダー表示の形式　CalendarFormatと順序同一
const char *CalendarFormatText[] = {
    "1month",
    "2month",   //横並び
    "2v month", //縦並び
    "3month",
    "3v month",
};


/*
 * カスタムフォントのフォルダ名(8文字max)
 * カスタマイズした後、保存必要
 */
//#define NumberOfUserFont   6 //カスタムフォント数 +1は、暫定フォント用
char CustomFontDir[NumberOfUserFont+1][9] = {
    "", "", "", "", "", "", "",
};

/* 
 * SDカードに格納されているフォルダ名をフォント名(最大8文字)
 * 順序は、FontCodeと合わせている。先頭2つは、内蔵フォントなので、フォルダ不要
 * カスタマイズ画面でのフォント名の表示にも使うので、内蔵フォントも入れている
 */
char *FontFolder[] = {
    NameFontNormal, //"Normal",   //実際には内蔵フォントなので、これは不要だが
    NameFontSmall,  //"Small",
    NameFont7like,  //7セグもどき
    NameFont7seg,   //"7segsl",
    NameFont7segSmall,  //少し小さい7セグフォント
    CustomFontDir[0],   // Dateでカスタムフォント適用時のフォント名=フォルダ
    CustomFontDir[1],
    CustomFontDir[2],
    CustomFontDir[3],
    CustomFontDir[4],
    CustomFontDir[5],
    CustomFontDir[6],   // 一時的に使用するフォント名用。カスタマイズ中とか。
};

//各オブジェクトのフォーマットの数
uint8_t FormatNum[OBJECTNUM] = {
    MAXDateFormat,
    MAXTimeFormat,
    MAXAlarmFormat,
    MAXTempFormat,
    0,
    MAXCalendarFormat,    
};

//Formatのポップアップに表示させるオブジェクト毎の文字列をひとまとめにしたリスト
const char *FormatTextList[] = {
  (const char*)DateFormatText,
  (const char*)TimeFormatText,
  (const char*)AlarmFormatText,
  (const char*)TempFormatText,
  (const char*)NULL,
  (const char*)CalendarFormatText,
};


//Prototype
void drawFontSize(MainFormRsc rsc, ObjectRsc data);
void drawObjectTable(uint8_t num, uint8_t objnum);


/*
 * カスタム設定の値をコピーして、設定変更してもCancelボタン押した時に元に戻せるようにする
 * 
 */
void CopyCustomDataToTemp() {
    uint8_t jj;
    
    for (jj = 0; jj < OBJECTNUM; jj++) {
        TempObjData[jj] = CustomRsc[jj];
    }
}


/*
 * 設定がOKの時に、CustomRscに値を反映させる。EEPROMへも書き込む
 */
void SetCustomData() {
    uint8_t jj, kk;
    uint16_t addr;
    
    for (jj=0; jj < OBJECTNUM; jj++) {
        CustomRsc[jj] = TempObjData[jj];
        
        //EEPROMに記録して保持させる  32バイトまで格納できるようにしてある
        addr = AddressCustom + (uint16_t)jj*32;
        DATAEE_WriteByte(addr++, CustomRsc[jj].disp);
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].x);
        addr += 2;
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].y);
        addr += 2;
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].xw);
        addr += 2;
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].yw);
        addr += 2;

        DATAEE_WriteByte(addr++, CustomRsc[jj].fontcode);
        DATAEE_WriteByte(addr++, CustomRsc[jj].fontmag);

        DATAEE_WriteWord(addr, CustomRsc[jj].color);
        addr += 2;
        DATAEE_WriteWord(addr, CustomRsc[jj].format);
        addr += 2;
        DATAEE_WriteWord(addr, CustomRsc[jj].attribute);
        addr += 2;
        DATAEE_WriteWord(addr, CustomRsc[jj].attribute2);
        addr += 2;

        DATAEE_WriteByte(addr++, CustomRsc[jj].xpitchAdj);
    }
    // カスタムフォント情報をEEPROMに保存
    for (jj = 0; jj < NumberOfUserFont; jj++) {
        for (kk = 0; kk < 9; kk++) {
            DATAEE_WriteByte(addr++, CustomFontDir[jj][kk]);
        } 
    }    
}


void setObjectSize(uint8_t obj) {
    tempRsc[obj].xw = Cursor_x - tempRsc[obj].x;
    if (tempRsc[obj].fontcode < InternalFontNum)
        tempRsc[obj].yw = CurrentFont.ysize * TextMagY;
    else
        tempRsc[obj].yw = CustomYsize;

}

void drawItem(uint8_t obj) {
    int16_t temp = 123;
    int16_t humidity = 345;
    uint8_t dateTime[7] = {0x56, 0x34, 0x12, 0x00, 0x31, 0x12, 0x21};
    uint8_t alarmTime[3] = {0x34, 0x12, 0x7f};    //mm, hh, wday
  
    switch (obj) {
        case GearObj:
            DrawGearIcon(tempRsc[obj].x, tempRsc[obj].y, tempRsc[obj].color);
            break;
        case TimeObj:
            resetPreDateTime(); //アナログ時計では使用
            if (tempRsc[obj].disp) {
                DrawTime(dateTime, &tempRsc[obj]);
                //描画した時の情報を利用して、枠サイズを保存
                if (tempRsc[obj].format == ANALOGCLOCK) {
                    tempRsc[obj].yw = tempRsc[obj].xw;
                } else {
                    tempRsc[obj].xw = Cursor_x - tempRsc[obj].x;
                    tempRsc[obj].yw = CustomYsize;
                }
            }
            break;
        case DateObj:
            if (tempRsc[obj].disp) {
                DrawDate(&tempRsc[obj]);    //日付を描画
                //描画した時の情報を利用して、枠サイズを保存
                setObjectSize(obj);
            }
            break;
        case TempObj:
            if (tempRsc[obj].disp) {
                DrawTemp(temp, &tempRsc[obj]);
                setObjectSize(obj);
            }
            break;
        case HumidityObj:
            if (tempRsc[obj].disp) {
                DrawHumidity(humidity, &tempRsc[obj]);
                setObjectSize(obj);
            }
            break;
        case AlarmObj:
            if (tempRsc[obj].disp) {
                DrawAlarmTime(alarmTime, SlideSWon, &tempRsc[obj]);
                setObjectSize(obj);
            }
            break;
        case CalendarObj:
            if (tempRsc[obj].disp) {
                DrawCalendar(&tempRsc[obj]);
                tempRsc[obj].xw = CustomXsize;
                tempRsc[obj].yw = CustomYsize;
            }
            break;
    }
}


/*
 * 現在の設定条件を使って、画面を描画する
 * 
 * 位置変更設定の時、イメージがわかるように、tempRscに設定途中のデータを書き込む
 * 位置を変更したtempRscのデータは、キャンセルで破棄されるので、暫定の暫定
 */
void drawXYscreen(uint8_t item) {
    uint8_t jj;
    char str[100];

    lcd_fill(TextbgColor);
    
    //部品描画　7つの部品全部描画
    for (jj = DateObj; jj < OBJECTNUM; jj++) {
        tempRsc[jj] = TempObjData[jj];  //暫定表示用のリソースに現在の設定をコピー
        drawItem(jj);
    }
    
    sprintf(str, "Touch screen to put %s", CustomSettingText[item]);
    SetFont(FontNormal);
    display_drawChars(10, 50, str, WHITE, TextbgColor, FontMagx12);
    
    //設定対象を枠で囲む
    display_drawRect(tempRsc[item].x, tempRsc[item].y, tempRsc[item].xw, tempRsc[item].yw, RED);
    
}


void drawUpdateItem(uint8_t item) {
    char str[100];
    
    drawItem(item);
    //移動先の位置に矩形を描画
    display_drawRect(tempRsc[item].x, tempRsc[item].y, tempRsc[item].xw, tempRsc[item].yw, RED);
    
    //座標を表示
    sprintf(str, "(%3d, %3d)", tempRsc[item].x, tempRsc[item].y);
    display_fillRect(140, 118, 80, 10, WHITE);
    SetFont(FontNormal);
    display_drawChars(140, 120, str, BLUE, WHITE, FontMagx12);
    
    PushEvent(EventFormUpdate);

}


/*
 * 位置設定用イベントハンドラ
 * 
 * 初期のタッチキャリブレーションと同様な処置
 * 画面をクリアして、メッセージ表示して、画面のどこかをタッチしたら、そこをX,Yとする
 * TargetObj: どのオブジェクトが対象か
 */
void XYsetEventHandler() {
    uint8_t evt;
    char str[100];
    uint8_t jj;
    uint8_t item = TargetObj;

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
        case EventXYFormInit:
            CurrentForm = SettingRsc2;  //フォームはCustomizingと同じものを利用するがボタン2つだけ
            //イメージ表示
            drawXYscreen(TargetObj);

        case EventFormUpdate:
            //OKとCancelボタンを描画
            for (jj = SettingBtnOKObj; jj <= SettingBtnCancelObj; jj++) {
                SetFont(FontNormal);
                DrawButton(CurrentForm[jj], CurrentForm[jj].color, LIGHTGREY, WHITE, 10);
            }
            break;
        case EventRotary:
            RotaryEventCount--;

            //元の枠内を消す
            display_fillRect(tempRsc[item].x, tempRsc[item].y, tempRsc[item].xw, tempRsc[item].yw, BLACK);

            //スライドSWで、X方向か、Y方向のどちらに動かすか決める
            if (SlideSWStatus == SlideSWoff) {
                tempRsc[item].x += RotCount;
                if (tempRsc[item].x < 0) tempRsc[item].x = 0;
                if (tempRsc[item].x > LCD_WIDTH) tempRsc[item].x = LCD_WIDTH;
            } else {
                tempRsc[item].y += RotCount;
                if (tempRsc[item].y < 0) tempRsc[item].y = 0;
                if (tempRsc[item].y > LCD_HEIGHT) tempRsc[item].y = LCD_HEIGHT;
            }
            RotCount = 0;
            drawUpdateItem(item);

            break;
            
        case EventTouchDown:
        case EventTouchRepeat:
            // タッチの位置
            if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnOKObj])) { //OKの時
                TempObjData[item].x = tempRsc[item].x;
                TempObjData[item].y = tempRsc[item].y;
                //処理を戻す
                SetEventHandler(CustomizingEventHandler);
                PushEvent(EventFormInit);
            } else if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnCancelObj])) { //Cancelの時
                SetEventHandler(CustomizingEventHandler);
                PushEvent(EventFormInit);
            } else {
                //元の枠を消す
                display_fillRect(tempRsc[item].x, tempRsc[item].y, tempRsc[item].xw, tempRsc[item].yw, BLACK);
                tempRsc[item].x = TouchX;
                tempRsc[item].y = TouchY;
                
                drawUpdateItem(item);
                RepeatKey = 1;
            }
            break;
    }
}


/*
 * ポップアップのボックス内をタッチした時の処理
 * どの項目をタッチしたか判断
 */
void TouchProcedurePopup() {
    int16_t xx, yy;
    uint8_t select;
    char str[100];

#ifdef DEBUG2
    sprintf(str, "T=%3d,%3d", TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, TextbgColor, FontMagx11);
#endif

    //ボックスの外なら、キャンセル
    xx = TouchX - PopX;
    yy = TouchY - (PopY + FRAMESIZE);
    if ((xx >= 0) && (xx < PopXW)) {
        if ((yy >= Yoffset) && (yy < PopYW-FRAMESIZE)) {
            //ボタンの矩形領域に入っていた場合
            select = (uint8_t)(yy - Yoffset) / (CurrentFont.ypitch * 2);
            if (TargetItem == ItemFont) {     //フォント選択
                if (select >= InternalFontNum) {
                    //Custom...をタッチした時
                    PopupEventHandler(FontSelectEventHandler);
                    PushEvent(EventFormInit);
                    return;
                } else {
                    TempObjData[TargetObj].fontcode = select;
                }
            } else if (TargetItem == ItemFontSize) {    //フォントサイズ
                if (TargetObj == TimeObj && TempObjData[TargetObj].format == ANALOGCLOCK) {
                    //アナログ時計の時、フォントサイズは大きさに利用
                    //それに応じて、xwを変更。半径に利用
                    TempObjData[TargetObj].attribute = select;
                    TempObjData[TargetObj].xw = AnalogClockSize[select] *2;    //xwは半径ではなく、全体のサイズ
                    TempObjData[TargetObj].yw = TempObjData[TargetObj].xw;
                } else {
                    if (FontMag[select] == FontMagOther) {
                        //選択肢以外のFontMagの設定用フォームに移動
                        SetEventHandler(FontMagEventHandler);
                        PushEvent(EventFormInit);
                        //引き渡すデータ:フォント名、サイズ等は、TempObjData, TargetObjから取得
                        return;
                    } else TempObjData[TargetObj].fontmag = FontMag[select];
                }
            } else if (TargetItem == ItemXpitch) {       //Xピッチ調整
                TempObjData[TargetObj].xpitchAdj = (int8_t) select-2;  //0番目はdefault-2
            } else if (TargetItem == ItemFormat) {         //フォーマット
                TempObjData[TargetObj].format = select;
                if (TempObjData[TargetObj].format == ANALOGCLOCK) {
                    //アナログ時計に変更した時、初期化として、Large=0最大サイズを指定
                    TempObjData[TargetObj].xw = AnalogClockSize[Large] *2;
                    TempObjData[TargetObj].yw = TempObjData[TargetObj].xw;
                    TempObjData[TargetObj].attribute = Large;
                    drawFontSize(CurrentForm[TargetObj], TempObjData[TargetObj]);
                }
            }
        }
    }

    //キャンセル処理
    display_fillRect(PopX, PopY, PopXW, PopYW, TextbgColor);
    PushEvent(EventFormUpdate);
    PopUp = 0;  //ポップアップ終了
}

/*
 * 指定されたリストをボックス表示させる共通ルーチン
 * num:       項目数
 * listText:  表示する項目の文字列リスト
 * popx: ポップアップするX座標は、呼び出し側が指定するようにした。
 */
uint16_t SelectedNo;    //現在選択している項目番号 (先頭=0(項目数-1))
void drawPopupForm(uint8_t num, char **listText, char *title, int16_t popx) {
    int16_t x, y;
    uint8_t jj, len;
    char str[100];
    uint16_t fc, bc;

    if (title != NULL) Yoffset = 10;
    else Yoffset = 0;

    SetFont(FontNormal);
    //タッチしたターゲットの座標データ
    //リストの文字列の最長をチェックして横幅決定
    PopXW = 0;
    for (jj = 0; jj < num; jj++) {
        len = (uint8_t)strlen(listText[jj]);
        if (len > PopXW) PopXW = len;
    }
    PopXW = PopXW * CurrentFont.xpitch + FRAMESIZE*2;     //横幅    
    
    PopX = popx;   //TargetItemを使った方が良いか？
    PopY = CurrentForm[TargetObj + SettingDateObj].y;
    PopYW = num * CurrentFont.ypitch * 2 + FRAMESIZE*2; //Y縦倍率*2
    PopYW += Yoffset;    //タイトル分、縦を増やす   

    //画面の外にはみ出さないように表示位置を補正
    if (PopX + PopXW > LCD_WIDTH-5) PopX =  - (PopXW - (LCD_WIDTH-5));
    if (PopY + PopYW > LCD_HEIGHT) PopY = - (PopYW - LCD_HEIGHT);
    
    display_fillRect(PopX, PopY, PopXW, PopYW, TextbgColor);
    x = PopX + FRAMESIZE;
    y = PopY + FRAMESIZE;
    
    //どのアイテムをポップアップしたかわかるように小さくトップに表示
    if (title != NULL) {
        display_drawChars(x+5, y, title, WHITE, TextbgColor, FontMagx11);
        y = y + Yoffset;
    }
    
    for (jj = 0; jj < num; jj++) {
        sprintf(str, "%s", listText[jj]);
        //現在設定されているものは反転させる
        if (jj == SelectedNo) {
            fc = TextbgColor;
            bc = WHITE;
        }
        else {
            fc = WHITE;
            bc = TextbgColor;
        }
        display_drawChars(x, y, str, fc, bc, FontMagx12);
        y = y + 20;
    }
    display_drawRect(PopX, PopY, PopXW, PopYW, WHITE);
    PopUp = 1;  //ポップアップ中

}


/*
 * 設定画面をタッチした時の処置
 * 　座標によって処理内容を特定
 */
void TouchProcedureCustomizing() {
    int16_t yy;
    char str[100];
    uint8_t rscnum;
    
#ifdef DEBUG
    sprintf(str, "TXY=%03d, %03d", TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, TextbgColor, FontMagx11);
#endif
    
    if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnCancelObj])) {
        ClearBuffer();
        SetEventHandler(MainEventHandler);
        PushEvent(EventFormInit);
        
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnOKObj])) {
        //OKの時は、設定された情報をCustomRscに取り込み、EEPROMに書き込む
        SetCustomData();
        
        ClearBuffer();
        SetEventHandler(MainEventHandler);
        PushEvent(EventFormInit);
//        sprintf(str, "%d", TargetItem);
//        display_drawChars(0, 230, str, WHITE, TextbgColor, FontMagx11);
//        __delay_ms(500);

    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnSaveObj])) { //Saveの時
        SetEventHandler(CustomDataEventHandler);
        PushEvent(EventFormInitSave);
    
    } else if (ButtonPush(TouchX, TouchY, &CurrentForm[SettingBtnLoadObj])) { //Loadの時
        SetEventHandler(CustomDataEventHandler);
        PushEvent(EventFormInitLoad);
    
    } else {
        //まずどの行にタッチしたかチェックして、その後、どの項目(列)かをチェックする
        // rscnumがリソースのオブジェクトを指し、TargetObjが7種のオブジェクトのどれかを示す
        TargetObj = DateObj;   // = 0
        while (TargetObj < OBJECTNUM) {
            rscnum = SettingDateObj + TargetObj;
            yy = TouchY - CurrentForm[rscnum].y;
            if ((yy >= 0) && (yy < CurrentForm[rscnum].yw)) {
                //どの行=オブジェクトが対象か決定されたので、どの列がタッチされたか検索
                //Enable/Disableはチェックボックス分を判定に加える
                if ((TouchX >= 0) && (TouchX < XCheckboxW + XItemW)) {
                    //Itemの列をタッチした時
                    //Gearはdisableできないようスキップさせる　　現状CustomRscのGearのRscID=99
                    if (TempObjData[TargetObj].disp != 99) {
                        //対象のobjが押されていたら状態を反転させる。
                        if (TempObjData[TargetObj].disp == Disable)
                            TempObjData[TargetObj].disp = Enable;
                        else
                            TempObjData[TargetObj].disp = Disable;
                        drawObjectTable(TargetObj, TargetObj + SettingDateObj);    //グレーアウト対応
                    }
                    return;  //ループ終了
                }
                // disp==Disableの場合、他の設定はできないようにここで終了
                if (TempObjData[TargetObj].disp == Disable) return;

                //Fontの列がタッチされたか
                if ((TouchX >= XFont) && (TouchX < XFont + XFontW)) {
                    //選択肢ない場合は、中止
                    if (CurrentForm[rscnum].fontcode == NoData8) return;
                    //対象のobjが押されていたら
                    TargetItem = ItemFont;    //フォント
                    SelectedNo = TempObjData[TargetObj].fontcode;   //現在値
                    if (SelectedNo >= InternalFontNum) SelectedNo = Font7seg;  //カスタムフォントだった時は、全部これ
                    drawPopupForm(NumberOfFont, (char **)UserFontName, (char*)CustomSettingText[TargetObj], XFont); //表示するフォント数をNumberOfFontで指定
                    return;  //ループ終了                    
                }
                
                //FontSizeの列がタッチされたか
                if ((TouchX >= XFSize) && (TouchX < XFSize + XFSizeW)) {
                    //そのオブジェクトに選択肢ない場合は、中止
                    if (CurrentForm[rscnum].fontcode == NoData8) return;
                    TargetItem = ItemFontSize;  //フォントサイズ
                    //対象のobjが押されていたらポップアップ　　アナログ時計モード時は、サイズ設定
                    if (TargetObj == TimeObj && TempObjData[TargetObj].format == ANALOGCLOCK) {
                        SelectedNo = TempObjData[TargetObj].attribute;   //現在値
                        drawPopupForm(sizeof AnalogClockSize, (char **)AnalogClockSizeText, (char*)CustomSettingText[TargetObj], XFSize);
                    }  else {
                        //fontsizeは、0基点のシリアル番号でないので、現在のフォントサイズと一致するシリアル番号をサーチする
                        SelectedNo = 0;
                        //現在値を検索
                        while (FontMag[SelectedNo] != TempObjData[TargetObj].fontmag) SelectedNo++;
                        drawPopupForm(sizeof FontMag, (char **)FontMagText, (char*)CustomSettingText[TargetObj], XFSize);
                    }
                    return;  //ループ終了                    
                }
                
                //Xピッチの列がタッチされたか
                if ((TouchX >= XPitch) && (TouchX < XPitch + XPitchW)) {
                    //選択肢ない場合は、中止
                    if (CurrentForm[rscnum].xpitchAdj == NoData8) return;
                    TargetItem = ItemXpitch;    //フォントピッチ
                    //対象のobjが押されていたら
                    SelectedNo = (uint8_t)(TempObjData[TargetObj].xpitchAdj+2);   //現在値
                    drawPopupForm(MAXXpitchNum, (char **)FontXpitchText, (char*)CustomSettingText[TargetObj], XPitch);
                    return;  //ループ終了
                }

                //色設定の列がタッチされたか
                if ((TouchX >= XColor) && (TouchX < XColor + XColorW)) {
                    TargetItem = ItemColor;    //色の列を選択
                    //対象のobjが押されていたら
                    PopupEventHandler(ColorSettingEventHandler);
                    PushEvent(EventColorFormInit);
                    return;  //ループ終了
                }

                //位置設定の列がタッチされたか
                if ((TouchX >= XPos) && (TouchX < XPos + XPosW)) {
                    TargetItem = ItemXY;    //位置の列を選択
                    //対象のobjが押されていたら
                    SetEventHandler(XYsetEventHandler);
                    PushEvent(EventXYFormInit); //画面描画
                    return;  //ループ終了
                }

                //フォーマットの列がタッチされたか
                if ((TouchX >= XFormat) && (TouchX < XFormat + XFormatW)) {
                    //選択肢ない場合は、中止
                    if (CurrentForm[rscnum].format == NoData) return;
                    TargetItem = ItemFormat;    //フォーマット
                    //対象のobjが押されていたら
                    SelectedNo = TempObjData[TargetObj].format;   //現在値
                    //項目数でYサイズ、最大項目の横幅でXサイズ決定し、項目を表示
                    drawPopupForm(FormatNum[TargetObj], (char **)FormatTextList[TargetObj], NULL, XFormat);

                    return;  //ループ終了
                }
                return;
            }
            TargetObj++;
        }
        
    }
}


/*
 * 個々の項目の表示
 * チェックボックスは、X=XCheckboxに固定
 */
void drawCheckBox(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    //項目名のリソースを共有し、RscIDをその項目を表示させるかどうかの設定に利用
    sprintf(str, "%c", Fbox);   // Fbox=0x7fは□のコード
    display_drawChars(XCheckbox, rsc.y, str, rsc.color, TextbgColor, FontMagx22);
    if (data.disp != Disable) {
        strcpy(str, "v");   //チェックは、vで代用
        display_drawChars(XCheckbox+1, rsc.y-1, str, rsc.color, rsc.color, FontMagx22);
    }
}

void drawFontName(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    if (rsc.fontcode == NoData8) strcpy(str, "-");
    else sprintf(str, "%-10s", FontFolder[data.fontcode]);   //フォント名=フォルダ名
    display_drawChars(XFont, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

void drawFontSize(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    if (rsc.fontcode == NoData8) strcpy(str, "-");
    else sprintf(str, "%2X", data.fontmag);
    display_drawChars(XFSize, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

void drawXpitch(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    if (rsc.xpitchAdj == NoData8) strcpy(str, "-");
    else sprintf(str, "%+2d", data.xpitchAdj);
    display_drawChars(XPitch, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

void drawColor(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    //漢字フォント使わず、ノーマルに■を作った
    sprintf(str, "%c", Ffbox);   //FontNormalの■のコード=Ffbox
    display_drawChars(XColor, rsc.y, str, data.color, TextbgColor, rsc.fontmag); //フォントサイズ22にしないといけないかも
    display_drawChars(XColor, rsc.y, str, data.color, TextbgColor, FontMagx22);

}

void drawXY(MainFormRsc rsc, ObjectRsc data) {
    char str[100];
    
    sprintf(str, "(%3d,%3d)", data.x, data.y);
    display_drawChars(XPos, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

/*
 * 選択されているフォーマットの表示
 */
void drawFormat(MainFormRsc rsc, ObjectRsc data, uint8_t num) {
    char str[100];
    
    if (rsc.format == NoData) strcpy(str, "-");
    else {
        switch (num) {
            case 0: //Date
                sprintf(str, "%d", data.format);
                break;
            case 1: //Time
                sprintf(str, "%c", *TimeFormatText[data.format]);
                break;
            case 2: //Alarm
                sprintf(str, "%c", *AlarmFormatText[data.format]);
                break;
            case 3: //Temp
                sprintf(str, "%c", *TempFormatText[data.format]);
                break;
            case 5: //Calendar
                sprintf(str, "%s", CalendarFormatText[data.format]);
                str[2] = '\0';
                break;
        }
    }
    display_drawChars(XFormat, rsc.y, str, ItemFColor, TextbgColor, rsc.fontmag);
}

/*
 * カスタマイズの表を構成する指定された1行を描画
 * num: RscObjectの番号をもらう　0?OBJECTNUM
 * objnum: MainFormRscのどの項目か示す
 */
void drawObjectTable(uint8_t num, uint8_t objnum) {
    char str[50];
    
    if (num < GearObj) drawCheckBox(CurrentForm[objnum], TempObjData[num]);
    display_drawChars(CurrentForm[objnum].x, CurrentForm[objnum].y, CurrentForm[objnum].str, CurrentForm[objnum].color, TextbgColor, CurrentForm[objnum].fontmag);
    
    //Disableの時、項目名以外をグレーアウトさせる
    if (TempObjData[num].disp == Disable) ItemFColor = GREY;
    else ItemFColor = CurrentForm[objnum].color;
    
    drawFontName(CurrentForm[objnum], TempObjData[num]);

    //アナログ時計の時は、フォントサイズの所は、時計のサイズを表示
    if (num == TimeObj && TempObjData[num].format == ANALOGCLOCK) {
        sprintf(str, "%s", AnalogClockSizeText[TempObjData[num].attribute]);
        str[2] = '\0';  //先頭2文字だけ画面に表示
        display_drawChars(XFSize, CurrentForm[objnum].y, str, ItemFColor, TextbgColor, CurrentForm[objnum].fontmag);
    } else {
        drawFontSize(CurrentForm[objnum], TempObjData[num]);
    }
    drawXpitch(CurrentForm[objnum], TempObjData[num]);
    drawColor(CurrentForm[objnum], TempObjData[num]);
    drawXY(CurrentForm[objnum], TempObjData[num]);
    drawFormat(CurrentForm[objnum], TempObjData[num], num);
    
}

/*
 * cls=1の時は、画面消去してから描画
 * 設定画面を描画
 */
void DrawCustomizingForm(uint8_t cls) {
    uint8_t jj;

    if (cls) lcd_fill(BLACK);
    
    SetFont(FontNormal);  //初期フォント設定

    //タイトル、項目名表示
    for (jj = TitleSettingObj; jj<= SubTitleSettingObj; jj++) {
        display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    }
    
    //各オブジェクトの情報を表示
    for (jj = DateObj; jj < OBJECTNUM; jj++) {
        drawObjectTable(jj, jj + SettingDateObj);
    }
    
    //ボタン描画  Save, Load, OK, Cancel
    for (jj = SettingBtnSaveObj; jj <= SettingBtnCancelObj; jj++) {
        DrawButton(CurrentForm[jj], CurrentForm[jj].color, CurrentForm[jj].attribute, CurrentForm[jj].color, 10);
    }
}


/*
 * カスタマイズ用イベントハンドラ
 * カスタマイズ項目を全部画面に表示させ、変更したいものをタッチして修正できる
 * 
 */
void CustomizingEventHandler() {
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
            DrawCustomizingForm(0);     //画面消去なし
            break;
        case EventFormInit:
            CurrentForm = SettingRsc2;
            DrawCustomizingForm(1);     //画面消去あり
            PopUp = 0;
            break;
        case EventRotary:
            break;
        case EventTouchDown:
            //ポップアップしているかどうかで処理変更
            if (PopUp) TouchProcedurePopup();
            else TouchProcedureCustomizing();
            break;
    }
}
