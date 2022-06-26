/*
 * カスタマイズデータの保存、読み出し
 */

/*
 * SDカードに設定情報のデータを保存、取り出しを行う
 * メモを付与できる。半角79文字まで
 * 最大8個までのファイルを以下のようにDataディレクトリ下に、固定ファイル名で格納
 * /Data/Data1.dat
 *       Data2.dat
 *       Data3.dat
 *       Data4.dat
 *       Data5.dat
 *       Data6.dat
 *       Data7.dat
 *       Data8.dat
 */

#include "CustomData.h"
#include <string.h>
#include <stdlib.h>     //atoi()

#include "main.h"
#include "font.h"
#include "Customizing.h"
#include "LCD320x240color.h"
#include "Draw.h"
#include "Setting.h"
#include "AlarmSound.h"
#include "TouchXT2046.h"


//以下により、文字列に2バイト文字があってもwarning出なくなるということだったが
#pragma jis

const char *DataFolder = "Data";

#define MaxMemo     80
char Memo[MaxMemo];  //ファイルごとに記入できる文字列
char TempMemo[MaxMemo]; //編集用暫定メモ
uint8_t CsrByte = 1;   //カーソルの位置の文字が1バイトか2バイトか
uint8_t Csr;        //カーソル位置
uint8_t CsrDraw;    //描画上の仮想カーソル位置(1行35文字とした場合)
char **CurrentKeyStr;
uint8_t adj[3]; //各行において、短縮表示したか
uint8_t SaveLoad;   //1: Save, 0: Load

#define MemoScrX    35
#define MemoScrY    3

uint8_t TargetDataNo;
char Filename[50];

enum CustomDataRscObject {
    DataTitleObj,
    DatasubTitleObj,
    DeleteMsgObj,   //表示位置は、下部だがプログラムの容易さからここに置く
    //オブジェクト名
    Data1Obj,
    Data2Obj,
    Data3Obj,
    Data4Obj,
    Data5Obj,
    Data6Obj,
    Data7Obj,
    Data8Obj,
    //ボタン
    BtnCloseObj,
    Default1Obj,    //デフォルト設定読出し用
    Default2Obj,
    Default3Obj,
};

#define MAXDataNum 8

const char *DataTextTitle[] = {
    "Load Setting Data",
    "Save Setting Data", 
};
const char DefText[][3] = {
  "1",
  "2",
  "3",
};
const char DataTextsubTitle[] = "/Data/Data?.dat";
const char DeleteMsg[] = "<Long push to delete>";

//各行の高さ
#define DataItemH   20

//Save/Load用リソース
MainFormRsc SaveLoadRsc[] = {
    //ID,               x,   y             xw,  yw,        fontcode,   fontMag,    str,                color,
    { DataTitleObj,     0,   0,            0,   0,         FontNormal, FontMagx22, NULL, WHITE, },   //タイトルは後で設定
    { DatasubTitleObj, 10,  20,            0,   0,         FontNormal, FontMagx12, (char*)DataTextsubTitle, WHITE, },
    { DeleteMsgObj,   150,  20,            0,   0,         FontNormal, FontMagx12, (char*)DeleteMsg, LIGHTGREY, },
    //データリスト
    { Data1Obj,        10, 40,             300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data2Obj,        10, 40+DataItemH*1, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data3Obj,        10, 40+DataItemH*2, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data4Obj,        10, 40+DataItemH*3, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data5Obj,        10, 40+DataItemH*4, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data6Obj,        10, 40+DataItemH*5, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data7Obj,        10, 40+DataItemH*6, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    { Data8Obj,        10, 40+DataItemH*7, 300, DataItemH, FontNormal, FontMagx11, NULL, WHITE, },
    //ボタン
    { BtnCloseObj,    240, 200,             70, 30,        FontNormal, FontMagx12, (char*)CloseText, WHITE, },  // Close
    
    { Default1Obj,     40, 200,             50, 30,        FontNormal, FontMagx12, (char*)DefText[0], WHITE, },  // 1
    { Default2Obj,    100, 200,             50, 30,        FontNormal, FontMagx12, (char*)DefText[1], WHITE, },  // 2
    { Default3Obj,    160, 200,             50, 30,        FontNormal, FontMagx12, (char*)DefText[2], WHITE, },  // 3
};

enum SaveOKObject {
    MsgStr1Obj,
    MsgStr2Obj,
    BtnMemoObj,
    BtnYesObj,
    BtnNoObj,
};

char SaveOKMsg1[30];    //ファイルの有無でメッセージ変えるためここで定義しない
char SaveOKMsg2[15];
char SaveOKMsg3[] = "- Input Memo [...]";
const char BtnYesText[] = "Yes";
const char BtnNoText[] = "No";
const char *TextOverWrite = "Overwrite ?";
const char *TextOK = "OK ?";


//保存OKか確認用
MainFormRsc SaveOKRsc[] = {
    //ID,               x,   y         xw, yw,   fontcode,   fontMag,    str,               color,
    { MsgStr1Obj,    10+30, 10+70,      0,  0,   FontNormal, FontMagx12, (char*)SaveOKMsg1, WHITE, },   //メッセージ
    { MsgStr2Obj,    10+30, 30+70,      0,  0,   FontNormal, FontMagx12, (char*)SaveOKMsg2, WHITE, },   //メッセージ
    
    { BtnMemoObj,    10+30, 55+70,    200, 30,   FontNormal, FontMagx12, (char*)SaveOKMsg3, WHITE, },   //メッセージ
    //ボタン    
    { BtnYesObj,     30+30, 85+70,     70, 30,   FontNormal, FontMagx12, (char*)BtnYesText, WHITE, },  // Yes
    { BtnNoObj,     130+30, 85+70,     70, 30,   FontNormal, FontMagx12, (char*)BtnNoText,  WHITE, },  // No
};

/*
 * Yes/Noを取得する汎用フォーム
 * 文字列の長さにより大きさを設定
 */
MainFormRsc ConfirmFormRsc[] = {
    //ID,               x,   y         xw, yw,   fontcode,   fontmag,    str,  color,
    { MsgStr1Obj,    10+30, 10+80,      0,  0,   FontNormal, FontMagx12, NULL, WHITE, },   //メッセージ
    { MsgStr2Obj,    10+30, 30+80,      0,  0,   FontNormal, FontMagx12, NULL, WHITE, },   //ダミー

    { BtnMemoObj,    10+30, 55+70,    200, 30,   FontNormal, FontMagx12, NULL, WHITE, },   //ダミー
    //ボタン    
    { BtnYesObj,     30+30, 60+80,     70, 30,   FontNormal, FontMagx12, (char*)BtnYesText, WHITE, },  // Yes
    { BtnNoObj,     130+30, 60+80,     70, 30,   FontNormal, FontMagx12, (char*)BtnNoText,  WHITE, },  // No
};

const char *TextNoDir = "Not exist 'Data' directory";
const char *TextWait = "Wait around 45sec...";
const char ErrorNotWrite[] = "Error! Couldn't write";
const char *ErrorNotRead = "Error! Couldn't read";

#define X1 120
#define X2 170
#define X3 220
#define X4 270
#define Y1  80
#define Y2 120
#define Y3 160
#define Y4 200
#define XW  48
#define YW  38

const char MemoInputTitleStr[] = "Edit Memo (max.79)";
//ボタンに表示する文字、兼、入力文字
//半角記号全32種類中28種は7キーに4つずつ割り当て済み。[]{}は割当なし
//Mode: A -> a -> 1 -> あ -> ア -> Aに戻る
//ENGLISHモードの時は、A -> a -> 1 -> A
const char *KeyStr[] = {
    "@-_/", "ABC", "DEF", "BS", 
    "GHI","JKL", "MNO", "<-", 
    "PQRS", "TUV", "WXYZ", "->", 
    "a1あ", "'\":;", ".,?!", "sp", 
};
const char *KeyStrL[] = {
    "=()`", "abc", "def", "BS", 
    "ghi","jkl", "mno", "<-", 
    "pqrs", "tuv", "wxyz", "->", 
    "1あ", "+*^|", "#$%~", "sp", 
};
const char *KeyStr1[] = {
    "1", "2", "3", "BS", 
    "4","5", "6", "<-", 
    "7", "8", "9", "->", 
    "あア", "0", "<>&\\", "sp", 
};
const char *KeyStrH[] = {
    "あ", "か", "さ", "BS", 
    "た","な", "は", "<-", 
    "ま", "や", "ら", "->", 
    "アA", "わ", "、。", "sp", 
};
const char *KeyStrK[] = {
    "ア", "カ", "サ", "BS", 
    "タ","ナ", "ハ", "<-", 
    "マ", "ヤ", "ラ", "->", 
    "Aa1", "ワ", "、。", "sp", 
};

const char **KeyStrList[] = {
    KeyStr,
    KeyStrL,
    KeyStr1,
    KeyStrH,
    KeyStrK,
};

const char *Hira[] = {
    "あいうえお",
    "かきくけこがぎぐげご",   //濁音
    "さしすせそざじずぜぞ",   //濁音
    "",
    "たちつってとだぢづでど",   //濁音　っだけは小文字あり
    "なにぬねの",   
    "はひふへほばびぶべぼぱぴぷぺぽ",   //濁音、撥音
    "",
    "まみむめも",
    "やゆよゃゅょ",       //小文字
    "らりるれろ",
    "",
    "",
    "わをん",
    "、。ー",
};

const char *Kata[] = {
    "アイウエオ",
    "カキクケコガギグゲゴ",   //濁音
    "サシスセソ\ザジズゼゾ",   //濁音
    "",
    "タチツッテトダヂヅデド",   //濁音　っだけは小文字あり
    "ナニヌネノ",   
    "ハヒフヘホバビブベボパピプペポ",   //濁音、撥音
    "",
    "マミムメモ",
    "ヤユヨャュョ",       //小文字
    "ラリルレロ",
    "",
    "",
    "ワヲン",
    "、。ー",
};

enum MemoInputObject {
    MemoInTitleObj,
    MemoObj,
    BtnOKObj,
    BtnCancelObj,

    BtnObj,
};

#define ButtonW 50      //ボタンの横幅
#define ButtonH 40      //ボタンの高さ

/*
 * Memoに文字列を入力させるフォーム
 */
MainFormRsc MemoInputRsc[] = {
    //ID,              x,  y    xw, yw, fontcode,   fontMag,    str,   color,
    { MemoInTitleObj, 10,  3, 300,  20, FontNormal, FontMagx11, (char*)MemoInputTitleStr, WHITE, },   //タイトル
    { MemoObj,        10, 20, 280,  60, FontNormal, FontMagx11, NULL, WHITE, },   //メモ
    //ボタン
    { BtnOKObj,       5, Y4,   40,  30, FontNormal, FontMagx11, (char*)BtnNameOK,   WHITE, },  // OK
    { BtnCancelObj,  50, Y4,   60,  30, FontNormal, FontMagx11, (char*)BtnNameCancel,  WHITE, },  // Cancel

    { BtnObj,        X1, Y1,  200, 160, FontNormal, FontMagx11, NULL, WHITE, },   // 4x4のボタン全体を示す

};



/*
 * 3行表示。1行35文字ずつ表示
 * 現在のプログラムは、原始的にとりあえず1行35文字ずつ表示
 */
void writeMemo(char *tempMemo) {
    uint8_t jj, kk, ll, cn;
    uint16_t code;
    int16_t yy;

    jj = MemoObj;

    // 35文字/1行表示
    display_setTextSize(MemoInputRsc[jj].fontmag);
//    display_setTextPitch(0, 0);
//    display_setTextColor(MemoInputRsc[jj].color, BLACK);
    display_setColor(MemoInputRsc[jj].color);
    
    yy = MemoInputRsc[jj].y;
    display_setCursor(MemoInputRsc[jj].x, yy);
    kk = 0;     //文字列のポインタ
    cn = 0;     //1行の何文字目
    ll = 0;     //adj用

    while (tempMemo[kk] != '\0') {
        if (tempMemo[kk] < 0x80) {
            if (cn >= MemoScrX) {
                adj[ll++] = kk;  //何文字目が次の行の先頭か示す
                cn = 0;
                yy += 20;
                display_setCursor(MemoInputRsc[jj].x, yy);
            }
            display_putc(tempMemo[kk++]);  //
            cn++;
        } else {
            if (cn >= MemoScrX -1) {
                display_putc(' ');  //判断せず常に最後に1文字空白を描画
                adj[ll++] = kk;
                cn = 0;
                yy += 20;
                display_setCursor(MemoInputRsc[jj].x, yy);
            }
            code = tempMemo[kk++];
            code = code*256 + tempMemo[kk++];
            display_putK(code);  //
            cn += 2;
        }
    }

    adj[ll] = kk + 1;   //これを設定しておかないと、カーソル適切に描画できない

    //常に最後に3文字空白を描画し、削除して文字が減った時でも対応できるよう
    for (kk = 0; kk < 3; kk++) {
        if (cn >= MemoScrX) {
            cn = 0;
            yy += 20;
            display_setCursor(MemoInputRsc[jj].x, yy);
        }
        cn++;
        display_putc(' ');  //
    }
    
}


/*
 * テンキー部を描画
 * CurrentKeyStrをmodeに従って設定し、処理を共通化
 */
void DrawTenkey(uint8_t mode) {
    uint8_t idx;
    int16_t xx, yy;
    MainFormRsc rsc;

    //テンキー部のボタン描画
    rsc.fontmag = MemoInputRsc[BtnObj].fontmag;
    rsc.xw = XW;
    rsc.yw = YW;
    
    CurrentKeyStr = (char**)KeyStrList[mode];
    
    idx = 0;
    for (yy = Y1; yy < Y1 + MemoInputRsc[BtnObj].yw; yy = yy + ButtonH) {
        rsc.y = yy;
        for (xx = X1; xx < X1 + MemoInputRsc[BtnObj].xw; xx = xx+ ButtonW) {
            rsc.x = xx;
            rsc.str = CurrentKeyStr[idx++];
            //ボタンの中央にテキスト表示
            DrawButton(rsc, MemoInputRsc[BtnObj].color, TextbgColor, MemoInputRsc[BtnObj].color, 5);
        }
    }
    
    //ひらがな、カタカナは、キーの表示用とは別になるので、ここで変更
    if (mode == 3) CurrentKeyStr = (char **)Hira;
    else if (mode == 4) CurrentKeyStr = (char **)Kata;
    
}

/*
 * テンキータイプの入力フォームを描画
 * mode(0-4): 0=大文字、1=小文字、2=数値、3=かな、4=カナの内、どのキーボードを表示するか
 */
void DrawInputBox(uint8_t mode) {
    uint8_t jj, len, xl;
    int16_t yy;

    //枠描画
    display_fillRoundRect(0,  0, 320, 240, 10, BLACK);
    display_drawRoundRect(0,  0, 320, 240, 10, WHITE);

    //タイトル表示
    jj = MemoInTitleObj;
    display_drawChars(MemoInputRsc[jj].x, MemoInputRsc[jj].y, MemoInputRsc[jj].str, MemoInputRsc[jj].color, TextbgColor, MemoInputRsc[jj].fontmag);
    
    //メモを表示
    jj = MemoObj;
    //罫線を描画　最長の文字列分の罫線の長さにする
    len = MaxMemo + 1;   //最大文字数　2バイト文字により、ずれる場合があるので、+2にする
    for (yy = 38; yy < Y1; yy += 20) {
        if (len > MemoScrX) xl = MemoScrX;
        else xl = len;
        len -= MemoScrX;
        display_drawLine(MemoInputRsc[jj].x, yy, MemoInputRsc[jj].x +xl*8, yy, GREY);
    }

    writeMemo(TempMemo);

    //OK/Cancelボタンの描画
    for (jj = BtnOKObj; jj <= BtnCancelObj; jj++) {
        DrawButton(MemoInputRsc[jj], MemoInputRsc[jj].color, TextbgColor, MemoInputRsc[jj].color, 10);
    }

    DrawTenkey(mode);

}


/*
 * 引き渡された文字列memoにおいて、csrの位置の文字が1バイト文字か2バイト文字のどちらかかを返す
 * 先頭から検索していく
 * 1: 1バイト文字
 * 2: 2バイト文字の後半
 * 3: 2バイト文字の先頭
 */
uint8_t CheckChar(char *memo, uint8_t csr) {
    uint8_t jj, cn;
    
    jj = 0;
    cn = 0;
    while (jj <= csr) {
        if (memo[jj] >= 0x80) {
            cn = 2;
            jj += 2;
        } else {
            cn = 1;
            jj++;
        }
    }
    //2バイト文字の前半(適切なカーソル位置)なら3にする
    if ((cn == 2) && (jj == (csr +2))) cn++;
    return cn;
}


/*
 * csrで指定された文字の下に下線カーソルを描画
 * 前のカーソルは消去: CsrDraw, CsrByteを使う
 * 半角分しか入らない場所に全角が来たときは、次の行に。対象行は仮想的に空白が入る
 * 仮想的なカーソル位置がCsrDrawとなる　　例えば全部半角なら、Csr=CsrDraw
 * 仮想画面は、35文字x3行
 * 
 * 表示する時のカーソル位置を計算
 * カーソル位置の文字の長さ(半角か全角か)も設定
 * 
 */
void DrawCursor(uint8_t csr) {
    int16_t xt, yt;
    uint8_t csrdraw;

    //現在のカーソルの表示を消し、罫線を表示する
    xt = MemoInputRsc[MemoObj].x + (CsrDraw % MemoScrX)*8;
    yt = 38 + (CsrDraw / MemoScrX)*20;
    display_drawRect(xt, yt, 8 * CsrByte, 2, BLACK);
    display_drawRect(xt, yt, 8 * CsrByte, 1, GREY);

    //カーソルの長さを設定
    CsrByte = CheckChar(TempMemo, csr);
    if (CsrByte > 2) {
        //全角上でカーソル位置が2バイト目だったら、1バイト目に補正要　　未実施
        CsrByte = 2;     //カーソルの長さ
    }

    //描画するカーソル位置の補正を実施　　writeMemo()でadj[]が設定済みが必要
    if (csr < adj[0]) csrdraw = csr;
    else if (csr < adj[1]) csrdraw = (csr - adj[0]) + MemoScrX;
    else csrdraw = (csr - adj[1]) + MemoScrX *2;
    
    CsrDraw = csrdraw;
    //カーソルを描画
    display_drawRect(MemoInputRsc[MemoObj].x + (csrdraw % MemoScrX)*8, 38+(csrdraw / MemoScrX)*20, 8 * CsrByte,2, WHITE);

}

/*
 * ひらがなinputハンドラ
 */
void InputEventHandler() {
    uint8_t evt;
    uint8_t len;
    uint8_t pos, presize;   //カーソル
    int16_t tx, ty;
    uint16_t wch;   //2バイトコード
    uint8_t key, lenB;
    uint8_t cnt;   //cnt:何回同じボタンを押したか
    uint8_t ll;
    
    static uint8_t preKey = 0xff;
    static uint8_t first = 1;  //あるキーを押した1回目かどうか
    static uint8_t Aa1 = 0;    //0:A, 1:a, 2:数字, 3:かな, 4:カナ
    static uint8_t time = 0;   //入力確定までの時間を制御
    static uint8_t lateCsr = 0; //文字確定したらカーソルを移動させる

    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //イベントない時は、無駄に空回りさせないよう、10msのwait
        //0.8秒経過したら、入力確定させる
        if (time < 80) {
            time++;
            if (time >= 80) {
                preKey = 0xff;
                if (lateCsr) {
                    Csr += lateCsr;
                    DrawCursor(Csr);
                    lateCsr = 0;
                }
            }
        }
        return;
    }
    
#ifdef DEBUG
    sprintf(str, "e=%03d", evt);
    display_drawChars(250, 0, str, WHITE, TextbgColor, FontMagx11);
#endif

    switch (evt) {
        case EventFormInit:     //Save
            SetKanjiFont(SJIS16);   //Memoの日本語対応
            strcpy(TempMemo, Memo);     //キャンセル対応できるよう、元データは保持して、TempMemoを編集
            DrawInputBox(Aa1);
            Csr = (uint8_t)strlen(TempMemo);     //カーソル位置を最後尾に
            //カーソルバーを描画
            CsrByte = 1;
            CsrDraw = 0;
            DrawCursor(Csr);
            break;

        case EventTouchRepeat:
        case EventTouchDown:
            if (ButtonPush(TouchX, TouchY, &MemoInputRsc[BtnOKObj])) {   //OKボタン
                strcpy(Memo, TempMemo);     //Memoに編集した文字列を設定
                SetKanjiFont(0);
                ReturnEventHandler();   //SaveEventHandlerへ
                PushEvent(EventFormUpdate);
            } else if (ButtonPush(TouchX, TouchY, &MemoInputRsc[BtnCancelObj])) {   //Cancelボタン
                SetKanjiFont(0);
                ReturnEventHandler();   //SaveEventHandlerへ
                PushEvent(EventFormUpdate);
            } else if (ButtonPush(TouchX, TouchY, &MemoInputRsc[BtnObj])) {   //テンキーボタン押下
                tx = (TouchX - MemoInputRsc[BtnObj].x) / ButtonW;
                ty = (TouchY - MemoInputRsc[BtnObj].y) / ButtonH;
                key = (uint8_t)(tx + ty*4);    //4x4のキーマトリックスのどれが押されたか　0-15
                
                if (key == 12) {    //A/a
#ifdef ENGLISH
                    Aa1 = (Aa1 + 1) % 3;    //かなカナは表示させない
#else
                    Aa1 = (Aa1 + 1) % 5;
#endif
                    
                    //キーボードの表記を変更
                    DrawTenkey(Aa1);    //描画と共に、CurrentKeyStrを設定
                    preKey = 0xff;  //切り替えたら、前の状態をリセット要
                } else {
                    len = (uint8_t)strlen(TempMemo);
    
                    if (key == 7) {  //<-  Cursor Left
                        //カーソル位置の前の文字バイト分前に移動
                        if (Csr == 0) return;
                        presize = CheckChar(TempMemo, Csr-1);
                        //もし何らかの問題で、カーソルが2nd byteの位置にあっても以下で処理可
                        if (Csr > presize) Csr -= presize;
                        else Csr = 0;
                        DrawCursor(Csr);
                        RepeatKey = 1;  //リピート有効化
                    } else if (key == 11) {     //->    Cursor Right
                        presize = CheckChar(TempMemo, Csr);
                        if (presize >= 2) presize--;    //カーソルが2バイトでも対応
                        if (Csr < len) Csr += presize;
                        else return;
                        DrawCursor(Csr);
                        RepeatKey = 1;  //リピート有効化                            
                    } else if (key == 3) {      //BS
                        if (Csr > 0) {
                            //BSで1文字前の文字を削除
                            presize = CheckChar(TempMemo, Csr-1);   //消す文字のバイト数
                            if (Csr >= presize) Csr -= presize;  //新しいカーソル位置
                            else return;
                            pos = Csr;
                            //新しいカーソル位置に元の位置以降の文字をコピー
                            while (TempMemo[pos + presize] != '\0') {
                                TempMemo[pos] = TempMemo[pos + presize];
                                pos++;
                            }
                            TempMemo[pos] = '\0';    //終端を設定
                            //終わりの所に残っている元の文字を空白にして、再描画時に消させる
                            // 'abcdefg'   Csr=dの時、上記処理でcをけす
                            // 'abdefg'　　となる
                            writeMemo(TempMemo);    //メモを描画
                        }
                        DrawCursor(Csr);
                    } else {
                        if (key == 15) {    //space
                            wch = ' ';
                            first = 1;
                            if (lateCsr) {
                                //前の文字の確定がまだだったら、確定させる
                                Csr += lateCsr;
                                lateCsr = 0;
                            }
                            time = 77;
                        }
                        else {
                            time = 0;
                            if (key == preKey) {
                                //同じボタンを連続して押した時
                                first = 0;
                                cnt++;  //キーに割り当てられている何回目か
                                cnt = cnt % lenB;
                            } else {
                                first = 1;
                                cnt = 0;    //最初の文字
                                if (lateCsr) {
                                    //前の文字の確定がまだだったら、確定させる
                                    Csr += lateCsr;
                                    lateCsr = 0;
                                }
                                if (Aa1 <= 2) lenB = (uint8_t)strlen(CurrentKeyStr[key]);
                                else lenB = (uint8_t)strlen(CurrentKeyStr[key]) /2;    //かな、カタ
                                
                                if (lenB == 1) preKey = 0xff;
                                else preKey = key;
                            }

                            //wchに対象の文字を設定
                            if (Aa1 <= 2) wch = CurrentKeyStr[key][cnt];
                            else wch = ((uint16_t)CurrentKeyStr[key][cnt*2] << 8) + CurrentKeyStr[key][cnt*2+1]; //2バイト文字
                        }
                        
                        if (first == 0) {
                            //2回目以降は挿入ではなく前の文字を置換
                            if (wch < 0x80) TempMemo[Csr] = (char)wch;
                            else {
                                TempMemo[Csr] = (char)(wch >> 8);
                                TempMemo[Csr+1] = (char)(wch & 0xff);
                            }
                            // 表示を高速化するため、1文字だけ更新
                            display_setCursor(MemoInputRsc[MemoObj].x + (CsrDraw % MemoScrX)*8, MemoInputRsc[MemoObj].y + (CsrDraw / MemoScrX)*20);
                            display_setTextSize(MemoInputRsc[MemoObj].fontmag);
                            display_setTextPitch(0, 0);
                            //                                display_setTextColor(MemoInputRsc[MemoObj].color, BLACK);
                            display_setColor(MemoInputRsc[MemoObj].color);
                            
                            if (wch < 0x80) display_putc((char)wch);  //
                            else display_putK(wch);  //
                            
                        } else {
                            //挿入する文字が何バイトか
                            if (wch < 0x80) ll = 1;
                            else ll = 2;
                            pos = len;  //\0の位置を示す
                            if (pos + ll >= MaxMemo) return;   //文字数制限超えなら処理終了
                            //文字挿入　カーソル以降最後までの文字をサイズ分ずらす
                            while (pos >= Csr) {
                                TempMemo[pos+ll] = TempMemo[pos];
                                if (pos > 0) pos--;
                                else break;
                            }
                            //文字設定
                            if (wch < 0x80) TempMemo[Csr] = (char)wch;
                            else {
                                TempMemo[Csr] = (char)(wch >> 8);
                                TempMemo[Csr+1] = (char)(wch & 0xff);
                            }
                            lateCsr = ll;
                            writeMemo(TempMemo);
                            DrawCursor(Csr);
                        }
                    }
                }
            }
            break;
    }
}


/*
 * ファイルからMemoデータを取得
 */
void LoadMemo(char *filename, char *memo) {
    FRESULT res;
    char str[150];
    char *p;
    uint8_t pos;
    FIL file;

    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) return;   //ちゃんとopenできなかったら終了

    //Memo :の行を読み出す
    memo[0] = '\0';     //内容をクリアする
    while (f_gets(str, sizeof str, &file) != NULL) {     //\nまで読み出す
        p = strtok(str, ":");
        if (strncmp(p, "Memo", 4) == 0) {
            strcpy(memo, str+6);
            pos = (uint8_t)strlen(memo)-1;
            if (memo[pos] == '\n') memo[pos--] = '\0';
            if (memo[pos] == '\r') memo[pos] = '\0';
            break;
        }
    }
    f_close(&file);
}


/* 
 * カスタム設定のデータがあるか確認して、あれば1、なければ0を返す
 * データある時は、その日付情報をdateに設定
 */
int8_t DataExistCheck(uint8_t num, char *date) {
    char filename[50];
    FRESULT res;
    FILINFO finfo;  //fdate: bit15:9: year based on 1980, bit8:5=month, bit4:0=day
                    //ftime: bit15:11=hour, bit10:5=min, bit4:0=sec/2(2秒単位)

    sprintf(filename, "%s/Data%d.dat", DataFolder, num);
    res = f_stat(filename, &finfo); //ファイルの情報を取得
    if (res != FR_OK) {
        return 0;
    } else {
        sprintf(date, "%04u/%02u/%02u %02u:%02u:%02u", (finfo.fdate >> 9) + 1980, (finfo.fdate >> 5) & 0x0f, finfo.fdate & 0x1f, 
                finfo.ftime >> 11, (finfo.ftime >> 5) & 0x3f, (finfo.ftime & 0x1f)*2);
        //Memo情報を読み出す
        LoadMemo(filename, Memo);
        return 1;
    }    
}


/*
 * 保存用フォーム、読出し用フォームを共通化して、ここで描画
 * 違いはタイトルだけ
 * 表示、ファイル番号は、1始まり
 */
void DrawCustomDataSaveLoadForm() {
    uint8_t jj, kk;
    char str[100], date[30];
    
    lcd_fill(TextbgColor);
    
    CurrentForm[DataTitleObj].str = (char*)DataTextTitle[SaveLoad];
    //タイトル、項目名表示
    for (jj = DataTitleObj; jj <= DeleteMsgObj; jj++) {
        display_drawChars(CurrentForm[jj].x, CurrentForm[jj].y, CurrentForm[jj].str, CurrentForm[jj].color, TextbgColor, CurrentForm[jj].fontmag);
    }
    
    remount();  //
    SetKanjiFont(SJIS16);   //Memoの日本語対応
    for (jj = 0; jj < MAXDataNum; jj++) {
        //ファイルあるか確認して、あれば、日付、時間、Memoを取得
        if (DataExistCheck(jj+1, date)) {
            sprintf(str, "%d:%s %s", jj+1, date, Memo); //表示できる文字は限られるが、ここは全部入れる
        } else {
            sprintf(str, "%d: No data", jj+1);
        }        
        kk = jj + Data1Obj;

        display_drawChars(CurrentForm[kk].x, CurrentForm[kk].y, str, CurrentForm[kk].color, TextbgColor, CurrentForm[kk].fontmag);
    }
    SetKanjiFont(0);    //漢字ファイルを閉じる
            
    //ボタン描画  Close
    jj = BtnCloseObj; 
    DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 10);
    //Loadの時だけ、デフォルト設定を読み出すボタンを描画
    if (SaveLoad == 0) {
        for (jj = Default1Obj; jj <= Default3Obj; jj++) {
            DrawButton(CurrentForm[jj], CurrentForm[jj].color, TextbgColor, CurrentForm[jj].color, 5);
        }
    }

}


/*
 * カスタム設定のデータをSDカードから読み出す
 */
FRESULT LoadCustomData(char *filename) {
    FRESULT res;
    uint8_t kk, len;
    char str[150];
    char *p;
    FIL file;

    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) return res;   //ちゃんとopenできなかったら終了

    //1行目のテキストデータを読み出すが、データは不要
    f_gets(str, sizeof str, &file);     //\nまで読み出す
    
    //順不同にさせるには、最初の項目名を見て、格納番号を確定する必要あり
    //      "item : disp,  x,  y, xw, yw,font#,fsize,fratio, color,format,  attr, attr2,xpitchAdj"
    while (f_gets(str, sizeof str, &file) != NULL) {     //\nまで読み出す
        //先頭から:までが項目名
        p = strtok(str, ":");
        kk = 0;
        while (strncmp(CustomSettingText[kk], p, 4) && kk < OBJECTNUM) kk++;
        //読み込んだデータがどの項目か、サーチしてkkに設定
        //一致したものが見つかった場合、ない場合はスキップ
        if (kk < OBJECTNUM) {
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].disp = (uint8_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].x = (int16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].y = (int16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].xw = (int16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].yw = (int16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].fontcode = (uint8_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].fontmag = (uint8_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].color = (uint16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].format = (uint16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].attribute = (uint16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].attribute2 = (uint16_t)strtol(p, NULL, 0);
            p = strtok(NULL, ",");
            if (p != NULL) TempObjData[kk].xpitchAdj = (int8_t)strtol(p, NULL, 0);
            if (kk < NumberOfUserFont) {
                //Gearはフォント指定ない
                p = strtok(NULL, ",");
                if (p != NULL) {
                    while (*p == ' ') p++;    //出力フォーマットが、カンマの後にスペースがあるので、それを排除
                    strcpy(CustomFontDir[kk], p);
//                    //文字列最後の空白、制御文字を削除
//                    len = strlen(CustomFontDir[kk]);
//                    if (len > 0) {
//                        while (CustomFontDir[kk][len-1] < ' ') len--;
//                        CustomFontDir[kk][len] = '\0';
//                    }
                } else {
                    CustomFontDir[kk][0] = '\0';
                }
            }
        }
    }

    f_close(&file);
    return res;
}


/*
 * カスタム設定のデータをSDカードに書き出す
 * 以下のプログラムで動作は問題ない
 * 　しかし、f_write()を複数回実行した時、まともに動作させられなかった→　要調査
 */
FRESULT SaveCustomData(char *filename) {
    FRESULT res;
    uint8_t jj;
    char *fontDir;
    FIL file;

    res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) return res;   //ちゃんとopenできなかったら終了
    //以下で、全部のオブジェクト情報を書き込める
//    res = f_write(&file, TempObjData, sizeof TempObjData, &actualLength);

    //テキストで書き出し、人手で修正ができるようにする。PICではやりにくい。追加コメントを書き込みできるようにする
    //最初の行は、項目名
    f_printf(&file, "item : disp,  x,  y, xw, yw,font#,fsize, color,format,  attr,  attr2,xpitchAdj, Font\r\n");

    //ここでカンマ区切りで各項目ごとにデータ書出  テキストファイルの方が良いとの判断
    for (jj = 0; jj < OBJECTNUM; jj++) {
        if (jj < NumberOfUserFont) fontDir = CustomFontDir[jj];
        else fontDir = NULL;
        f_printf(&file, "%-5s: %4d,%3d,%3d,%3d,%3d,  %2d,  0x%02x, 0x%04x, %4d, 0x%04x, 0x%04x,  %2d, %s,\r\n", 
                CustomSettingText[jj],
                TempObjData[jj].disp, TempObjData[jj].x, TempObjData[jj].y, TempObjData[jj].xw, TempObjData[jj].yw, 
                TempObjData[jj].fontcode, TempObjData[jj].fontmag, TempObjData[jj].color, 
                TempObjData[jj].format, TempObjData[jj].attribute, TempObjData[jj].attribute2, TempObjData[jj].xpitchAdj,
                fontDir);
    }
    f_printf(&file, "Memo :%s\r\n", Memo);
    
    f_close(&file);
    return res;
}

/*
 * SDカードに設定情報保存用のDataフォルダがなければ作成
 */
FRESULT MakeDataDirecotory() {
    char str[100], dir[20];
    FRESULT res;
    
    strcpy(dir, DataFolder);
    res = f_stat(dir, NULL);   //Dataディレクトリがあるか？
    if (res == FR_NO_FILE) {
        //ディレクトリなければ作成
        display_drawChars(10, 180, (char *)TextNoDir, WHITE, TextbgColor, FontMagx12);
        //なければ、ディレクトリを作る
        res = f_mkdir(dir);
        remount();  //このremountは必要、その後、下記実行するが、dummy.datはたぶんtime outで作成できない
        display_drawChars(10, 195, (char *)TextWait, WHITE, TextbgColor, FontMagx12);
        sprintf(str, "%s/dummy.dat", dir);
        SaveCustomData(str);   //この処理がtime outまで約45秒かかる
        //でも、ここではダミーは作成されない。そういう仕様を利用している
    }
    return res;
    
}

/*
 * ファイル既にある時、FileExist=1
 * 　1: 上書きOKかポップアップウインドウを出す
 * 　　上書き必要ない時も、最終確認用に使う
 *   0: どこに書き込むかメッセージを出す
 * 
 */
void DrawWriteConfirmation(uint8_t num, uint8_t FileExist) {
    uint8_t jj;
    
    //枠描画
    display_fillRoundRect(30, 70, 230, 130, 10, BLACK);
    display_drawRoundRect(30, 70, 230, 130, 10, WHITE);
    
    //既存ファイルの有無でメッセージ変える
    if (FileExist) {
        sprintf(SaveOKMsg1, "File #%d already exists.", num);
        strcpy(SaveOKMsg2, TextOverWrite);
    } else {
        sprintf(SaveOKMsg1, "Write to File #%d.", num);
        strcpy(SaveOKMsg2, TextOK);
    }
    //メッセージ表示
    for (jj = MsgStr1Obj; jj <= BtnMemoObj; jj++) {
        display_drawChars(SaveOKRsc[jj].x, SaveOKRsc[jj].y, SaveOKRsc[jj].str, SaveOKRsc[jj].color, TextbgColor, SaveOKRsc[jj].fontmag);
    }
    
    //Yes/Noボタン描画
    for (jj = BtnYesObj; jj <= BtnNoObj; jj++) {
        DrawButton(SaveOKRsc[jj], SaveOKRsc[jj].color, TextbgColor, SaveOKRsc[jj].color, 10);
    }

}


/*
 * 処理終了メッセージを出して、必要ならカスタマイズ画面に戻す
 */
void closeProcedure(char *str, uint8_t backToCustomizing) {
    //メッセージを1秒だけ表示させる
    display_fillRoundRect(10, 100, 250, 50, 10, BLACK);
    display_drawRoundRect(10, 100, 250, 50, 10, WHITE);
    display_drawChars(40, 115, str, WHITE, TextbgColor, FontMagx12);
    for (uint8_t kk = 0; kk < 2; kk++) __delay_ms(500);
    
    //カスタマイズ画面に戻る
    ClearBuffer();
    if (backToCustomizing) {
        SetEventHandler(CustomizingEventHandler);   //カスタマイズ画面に戻る
        PushEvent(EventFormInit);
    } else {
        PushEvent(EventFormUpdate);
    }
}


/*
 * Save/Load　確認用ポップアップフォームのイベントハンドラ
 */
void SaveEventHandler() {
    char str[100];
    uint8_t fileExist;
    uint8_t evt;
    
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
        case EventFormInit:     //Save
            //設定情報を保存するDataディレクトリがあるかどうかチェック
            MakeDataDirecotory();
            //既にファイルあるか確認
            if (f_stat(Filename, NULL) == FR_OK) {
                fileExist = 1;
                LoadMemo(Filename, Memo);   //ファイルある時は、メモを読み出す
            } else {
                fileExist = 0;
                Memo[0] = '\0';     //ファイルない時は、メモはなし
            }
            //以下Updateと共用
        case EventFormUpdate:
            // ファイルに書き込むか最終確認用のメッセージ描画
            // 既存ファイル有無に関わらず、メモを設定することができるようにボタン設ける
            DrawWriteConfirmation(TargetDataNo, fileExist);
            break;
            
        case EventFormClose:

            break;

        case EventTouchDown:
            if (ButtonPush(TouchX, TouchY, &SaveOKRsc[BtnYesObj])) {   //Yesボタン
                //指定されたファイルに保存
                if (SaveCustomData(Filename) == FR_OK) {
                    sprintf(str, "Write Data to #%d", TargetDataNo);    //line938にも同様のものあり
                    closeProcedure(str, 1);
                } else {
                    //書き込みに失敗した時
                    strcpy(str, ErrorNotWrite);
                    closeProcedure(str, 0);
                }
            } else if (ButtonPush(TouchX, TouchY, &SaveOKRsc[BtnNoObj])) {   //Noボタン
                //Noの時　書き込み中止
                ClearBuffer();
                //
                SetEventHandler(CustomDataEventHandler);
                PushEvent(EventFormUpdate);
            } else if (ButtonPush(TouchX, TouchY, &SaveOKRsc[BtnMemoObj])) {   //Memo...ボタン
                //メモを書き込めるようにテンキースタイルの入力フォームに移行
                PopupEventHandler(InputEventHandler);
                PushEvent(EventFormInit);
            }
            break;
    }
}


int8_t CheckDataNum() {

    //どのデータを選択したかチェックして
    for (uint8_t jj=0; jj<MAXDataNum; jj++) {
        if (ButtonPush(TouchX, TouchY, &CurrentForm[jj + Data1Obj])) {
            //どれを選んだか、jj+1が選択した番号(1-8)
            TargetDataNo = jj+1;
            sprintf(Filename, "%s/Data%d.dat", DataFolder, TargetDataNo);   //ファイル名は1始まり
            return 0;
        }
    }
    return 1;   //error
}


/*
 * 保存、読出しのリスト上でタッチした時の処理
 * 
 */
void TouchProcedureCustomData(uint8_t saveload) {
    int8_t jj, kk;
    char str[100];
    
#ifdef DEBUG
    sprintf(str, "TXY=%03d, %03d", TouchX, TouchY);
    display_drawChars(200, 180, str, WHITE, TextbgColor, FontMagx11);
#endif
    
    if (ButtonPush(TouchX, TouchY, &CurrentForm[BtnCloseObj])) {   //closeボタン
        ClearBuffer();
        SetEventHandler(CustomizingEventHandler);   //カスタマイズ画面に戻る
        PushEvent(EventFormInit);
    } else {
        //デフォルト設定を読み出す
        for (jj = 0; jj < 3; jj++) {
            if (ButtonPush(TouchX, TouchY, &CurrentForm[Default1Obj + jj])) {   // 1-3ボタン
                //
                for (kk = 0; kk < OBJECTNUM; kk++) {
                    TempObjData[kk] = RscData[jj][kk];
                }
                sprintf(str, "Load Data from default %d", jj+1);
                closeProcedure(str, 1);    //終了処理　メッセージ表示後、カスタム設定画面へ戻す
                return;
            }
        }
        
        //ファイルから読み出す場合
        if (CheckDataNum()) return; //選択したファイル、TargetDataNoを設定
        
        if (saveload) {
            //保存の時は、ポップアップ出して、上書き確認とか実施
            SetEventHandler(SaveEventHandler);
            PushEvent(EventFormInit);
        } else {
            //指定されたファイルをLoad
            if (LoadCustomData(Filename) == FR_OK) {
                sprintf(str, "Load Data from #%d", TargetDataNo);
                closeProcedure(str, 1);    //終了処理　メッセージ表示後、カスタム設定画面へ戻す
            } else {
                //読出しに失敗した時
                strcpy(str, ErrorNotRead);
                //エラーの時は、カスタム設定画面には戻さない方が使い勝手が良い
                closeProcedure(str, 0);    //終了処理　メッセージ表示後、ファイルリストに戻す
            }
        }
    }
}



/* 
 * ポップアップウインドウにメッセージを表示して、Yesなら0、Noならば1を返す
 */

/*
 * Yes or No
 * Yesなら、対象のファイルを削除する
 */
uint8_t YesNoCheck() {
   
    if (ButtonPush(TouchX, TouchY, &ConfirmFormRsc[BtnYesObj])) {   //Yesボタン
        f_unlink(Filename); //ファイル削除
    } else if (ButtonPush(TouchX, TouchY, &ConfirmFormRsc[BtnNoObj])) {   //Noボタン
        //Noの時
        
    } else {
        return 1;   //Popup維持の場合
    }

    DrawCustomDataSaveLoadForm();   //フォーム書き直す
    ClearBuffer();
    return 0;   //Popup終了の場合

}

/*
 * タッチ長押しで、データの削除
 * ファイルがあって、ポップアップを描画したら、0を返す
 */
uint8_t DeleteData() {
    int8_t jj;
    char str[100];
    
    if (CheckDataNum()) return 1;   //error
    
    //ファイルがあれば、削除するか確認
    if (f_stat(Filename, NULL) == FR_OK) {
        
        //枠描画　　メッセージの長さに応じてサイズ算出できるといいが。
        display_fillRoundRect(30, 80, 230, 110, 10, BLACK);
        display_drawRoundRect(30, 80, 230, 110, 10, WHITE);
        
        sprintf(str, "Delete #%d data OK?", TargetDataNo);
        //メッセージ表示
        jj = MsgStr1Obj;
        display_drawChars(ConfirmFormRsc[jj].x, ConfirmFormRsc[jj].y, str, ConfirmFormRsc[jj].color, TextbgColor, ConfirmFormRsc[jj].fontmag);
        
        //ボタン描画
        for (jj = BtnYesObj; jj <= BtnNoObj; jj++) {
            DrawButton(ConfirmFormRsc[jj], ConfirmFormRsc[jj].color, TextbgColor, ConfirmFormRsc[jj].color, 10);
        }
        return 0;
    }
    return 1;
}


/*
 * カスタムデータを保存もしくは、読み出す時のイベントハンドラ
 * Load/Save Setting Dataという名称
 */
void CustomDataEventHandler() {
    uint8_t evt;
    char str[100];
    static uint8_t push = 0;
    static uint8_t popup = 0;
    
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
        case EventFormInitSave:
            //保存用フォーム:タイトルとsaveLoadで区別
            SaveLoad = 1;
            CurrentForm = SaveLoadRsc;
            DrawCustomDataSaveLoadForm();
            break;
        case EventFormInitLoad:
            //ロード用フォーム:タイトルとsaveLoadで区別
            SaveLoad = 0;
            CurrentForm = SaveLoadRsc;
        case EventFormUpdate:
            DrawCustomDataSaveLoadForm();
            break;
        case EventTouchDown:
            if (popup) popup = YesNoCheck();
            else {
                push = 1;
            }
            break;
        case EventTouchUp:
            //長押し対応のため、upで処理開始
            if (push == 1) {
                push = 0;
                TouchProcedureCustomData(SaveLoad);
            }
            break;
        case EventTouchLong:
            //popup中は、長押しは無視
            if ((push == 1) && (DeleteData() == 0)) {
                popup = 1;
                push = 0;
            }
            break;
    }
}
