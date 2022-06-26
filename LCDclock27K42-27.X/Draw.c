/*
 * File:   Draw.c
 * Author: K.Ohno
 *
 * 描画関係ルーチン
 */

#include "Draw.h"
#include <string.h>
#include "main.h"
#include "RTC8025.h"
#include "LCD320x240color.h"
#include "Customizing.h"
#include "font.h"

const char WeekDays[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};


//時刻表示は、変化のあった所だけにするようにする→アナログ時計のみで使用
uint8_t preTime[3]; //時刻を保持しておき、変化をチェックできるようにする

int16_t CalendarXsize, CalendarYsize;
uint8_t CalendarEJ; //0=J, 1=English

int16_t CustomXsize; //カスタマイズの時、領域を指定するために使う　カレンダーのみに使用
int16_t CustomYsize; //デジタルの時刻表示にも使用

char Smaller[2] = "";  //時刻表示で、bmpフォント指定時、小さいフォントを指定: "s"、それ以外の時、""

// 絶対値を取得する関数マクロ定義
#define ABS(x) ((x) < 0 ? -(x) : (x))


/*
 * フォントファイルあるかを0.bmpの有無で判断
 * ファイルあれば、ヘッダ情報を読み取り、グローバル変数のSDcardBufferに格納
 *  0.bmpデータがあることが前提。ない時は、カスタムフォントないと判断し、FontNormalで描画
 */
uint8_t checkFontFile(char *fontFolder) {
    char filename[30];
    FRESULT res;
    UINT actualLength;
    uint8_t xr, yr;
    FIL file;       //Openingやデータ書き込み用
    
    //0.bmpがある前提で、その大きさを取得する
    sprintf(filename, "/%s/0.bmp", fontFolder);
    if (f_open(&file, filename, FA_READ) != FR_OK) {
        return 1;        //エラーの時
    }
    //ファイルある時は、ヘッダ情報読み取る
    res = f_read(&file, SDcardBuffer, 32, &actualLength); //32バイト分読み込む
    f_close(&file);

    CurrentFont.xsize = SDcardBuffer[bcWidth];     //1バイトと想定
    CurrentFont.ysize = SDcardBuffer[bcHeight];    //1バイトと想定
    CurrentFont.xpitch = 0;     //X方向はプロポーショナルなのでその都度
    CurrentFont.ypitch = CurrentFont.ysize;
    
    //ノーマルフォントを何倍にしたら、指定されたフォントと同じ大きさになるかを計算
    xr = (CurrentFont.xsize + NormalFontData[Fxsize]/3) / NormalFontData[Fxsize];
    if (xr < 1) xr = 1;
    yr = CurrentFont.ysize / NormalFontData[Fxsize];
    if (yr < 1) yr = 1;
    CurrentFont.mag = (uint8_t)((xr<<4) + yr);

    return 0;   //ファイルあった時、正常終了
}


/*
 * FontCodeで定義されているフォントコードを指定
 * カスタムフォントの場合は、そこからさらにフォントファイルをたどる必要がある
 * 
 * これとSetKanjiFontは、併用する。
 * 漢字交じり表示を行う時は、内蔵フォントとの倍率を計算する都合上、SetKanjiFontを最後に実施
 * カスタムフォント使用時は、漢字を使わないことを前提
 * 　　カスタムフォントは、一部文字しか定義されていないことを前提に、FontNormalで補完させる
 */
uint8_t SetFont(uint8_t fontcode) {
   
    if (fontcode >= InternalFontNum) {
        //その他のカスタムフォントの場合　7セグ含む
        CurrentFont.data = NULL;    //内蔵フォントでないことを示す
        CurrentFont.fontcode = MAXFontCode;  //一旦、無効化状態にする
        //フォントファイルのチェック及び、size, pitch, mag情報取得
        if (checkFontFile(FontFolder[fontcode])) {
            return 1;  //フォントファイルがない時など
        }
        
    } else {
        //内蔵フォントの場合
        if (fontcode == FontNormal) {
            CurrentFont.data = NormalFontData;      //8x8
        } else if (fontcode == FontSmall) {
            CurrentFont.data = SmallFontData;       //6x8
        } else if (fontcode == Font7like) {
            CurrentFont.data = Font7segData;       //6x8
        } else {
            return 0; //ここに来ることは現状ないが、上記以外の場合は、何もせずリターン
        }
        CurrentFont.xsize = CurrentFont.data[Fxsize];
        CurrentFont.ysize = CurrentFont.data[Fysize];
        CurrentFont.xpitch = CurrentFont.data[Fxpitch];
        CurrentFont.ypitch = CurrentFont.data[Fypitch];
        CurrentFont.mag = 1;    //倍率は、カスタムフォント設定時に変更される        
    }
    CurrentFont.fontcode = fontcode;
    return 0;
}


/*
 * フォントを決めたときに、ヘッダ情報から、基本情報を取得
 * オフセットデータも取得しておく
 * ファイルはオープンのままにする FileFontを介してアクセス可能
 * 
 * fontname: 0:漢字ファイルcloseさせる
 *    Misaki 1    //8x8ドットフォント
 *    SJIS16 2    //16x16ドットフォント
 *    SJIS24 3    //24x24ドットフォント
 * 
 * CurrentKanjiFont.fontcodeで、漢字の有効無効を判断に使う
 */
uint8_t KanjiOffsetTable[256];  //ファイル内の漢字データのありかを示すテーブル

void SetKanjiFont(uint8_t fontcode) {
    char header[64];
    uint16_t headerSize;
    UINT actualLength;
    uint8_t xr, yr;

    //SDカードなければ、何もしない
    if (SDcardMount == 0) return;
    //既に同じフォントのファイルオープンしていたら何もしない
    if (CurrentKanjiFont.fontcode == fontcode) return;
    
    if (CurrentKanjiFont.fontcode != 0) {
        //既にファイルを開いていたら、一旦閉じる
        f_close(&FileFont);
        CurrentKanjiFont.fontcode = 0;  //一旦、漢字を無効化状態にする
        CurrentKanjiFont.mag = FontMagx11;
        if (fontcode == 0) {
            // fontcodeが0の時は、ファイル閉じて終了
            return;
        }
    }

    // ファイルを開き、データを読み出す
    //ファイル名は8文字までサポート
    if (f_open(&FileFont, KanjiFontFile[fontcode], FA_READ ) == FR_OK) {
        //Openし、ヘッダ読込
        f_read(&FileFont, header, 64, &actualLength); //64バイト分読み込む
        headerSize = header[2] + (uint16_t)header[3]*256; //ヘッダのサイズ=64
        CurrentKanjiFont.xsize = header[0x10]; //フォントのX方向の大きさ。ドット数
        CurrentKanjiFont.ysize = header[0x11]; //フォントのY方向の大きさ。ドット数
        CurrentKanjiFont.xpitch = CurrentKanjiFont.xsize;   //ピッチは、フォントサイズと同じにしておく
        CurrentKanjiFont.ypitch = CurrentKanjiFont.ysize;
        //高さを基準にノーマルフォントの倍率設定 .magはこのフォントの倍率ではないことに注意
        //8x8の時x11、16x16の時x12、24x24の時x23という感じが良いかも
        xr = (CurrentKanjiFont.xsize/2 + CurrentFont.xsize/2) / CurrentFont.xsize;
        if (xr < 1) xr = 1;
        yr = CurrentKanjiFont.ysize / CurrentFont.ysize;
        if (yr < 1) yr = 1;
        CurrentKanjiFont.mag = (uint8_t)((xr<<4) + yr);
        
        CurrentKanjiFont.data = (uint8_t *)KanjiFontFile[fontcode];
        CurrentKanjiFont.fontcode = fontcode;
        f_read(&FileFont, KanjiOffsetTable, 256, &actualLength); //256バイト分のテーブル読み込む
        //このデータをもとに表示文字のデータの位置をすばやく計算させるため、保持しておく
    }
}


/*
 * カスタムフォント(bmp形式のデータ)の描画
 * 対応する文字は、表示に必要なものに限定して、作成労力を節約してよい。
 * bmpファイルない時は、FontNormalで表示
 * 日付: 0-9 / () 曜日
 * 時刻: 0-9 :    小文字
 * 温度: 0-9 . ℃
 * 湿度: 0-9 %
 * カレンダー: 0-9 曜日
 * 
 * 小文字指定: Smallerを使う
 * 
 * エラーの時、1
 * 
 */
uint8_t displayCustom_putc(uint8_t c) {
    uint8_t k, x, xf, yf, mm, len;
    uint8_t ii, jj;
    uint16_t pos;
    int16_t y1, y2; //画面をはみ出していても関係ない座標
    int16_t xd1, xd2, yd1, yd2; //表示領域に合わせ切り詰めた座標　はみ出していない時は変化なし
    uint8_t fontXsize;
    uint8_t fontYsize;
    char filename[50];
    uint16_t datasize, offset, remainingLine, linenum;
    uint8_t idx;
    UINT actualLength;
    uint8_t line;
    FRESULT res;
    uint8_t prev_size;
    uint8_t prevfontcode, prevmag;
    int16_t xp, yp; //座標
    int16_t px, py; //倍率を取り込んだpitch
    uint8_t rgb[3];

    const char *charname[] = {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
        "colon", "dot", "percent", 
        "degC", "degF", "slash", "(", ")", "space", 
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", 
        "-", 
    };

    // フォントデータのファイルを開いて、データを読み出す
    // bmpファイルは、一種のプロポーショナルフォント
    // bmpファイルを読んで、X, Yサイズ取得後、データを取得しながら描画
    if (c >= '0' && c <= '9') idx = c - '0';
    else if (c == ':') idx = 10;
    else if (c == '.') idx = 11;
    else if (c == '%') idx = 12;
    else if (c == FdegC) idx = 13;
    else if (c == FdegF) idx = 14;
    else if (c == '/') idx = 15;
    else if (c == '(') idx = 16;
    else if (c == ')') idx = 17;
    else if (c == ' ') idx = 18;
    else if (c >= FSunday && c <= FSunday+6) idx = 19 + (c-FSunday);
    else if (c == '-') idx = 26;
    else {
        DRAWNORMAL:
        //対象のフォント定義ない時は、NormalFontで描画
        prevfontcode = CurrentFont.fontcode;
        prevmag = CurrentFont.mag;
        prev_size = (uint8_t)(TextMagX << 4) + TextMagY;
        SetFont(FontNormal);

        //ノーマルフォントを使わないといけない時、カスタムフォントとできるだけ高さをそろえる
        display_setTextSize((uint8_t)(((prevmag >> 4) * TextMagX)<<4) + (prevmag & 0x0f)*TextMagY);
        
        display_putc(c);
        //フォントを元に戻す
        SetFont(prevfontcode);
        display_setTextSize(prev_size);
        return 0;
    }
    
    //フォルダ名でフォント種類を指定する
    sprintf(filename, "/%s/%s%s.bmp", FontFolder[CurrentFont.fontcode], charname[idx], Smaller);
    res = f_open(&FileFont, filename, FA_READ);
    //3つのエラーのケースが想定される。対象の文字がない、そもそもフォントdirがない、その他のエラー
    if (res == FR_NO_FILE) {
        //フォルダはあっても対象の文字のファイルがない時は、ノーマルフォントを使う
            goto DRAWNORMAL;    
//            return 0;
    } else if (res != FR_OK) {
        //Openできないケースがあった。どういう時に発生するのか不明。
        //Openできずに処理させていて、表示が乱れるので、処理を追加
        //エラーはここで発生していた事例確認
        remount();      //再マウント
        if (SDcardMount == 1) {
            if (f_open(&FileFont, filename, FA_READ) != FR_OK) return 10;
        }
        else return 6;  //マウントできなかった場合
    }
    
    res = f_read(&FileFont, SDcardBuffer, 32, &actualLength); //header=32バイト分読み込む
    if (res != FR_OK) {
        f_close(&FileFont);
        return 0; //エラーにせずリターン
    }
    
    fontXsize = SDcardBuffer[bcWidth]; //フォントの大きさは、縦横とも255までに制限
    fontYsize = SDcardBuffer[bcHeight];
    
    //シークして、データの先頭まで移動
    f_lseek(&FileFont, SDcardBuffer[bfOffBits]);

    if (res != FR_OK) {
        f_close(&FileFont);
        return 3;
    }

    len = ((fontXsize+31)/32)*4;        //1ドット行分のバイト数、4バイト単位
    //例：fontXsize=40ドットの時、データは5バイト、3バイトpaddingして、8バイト/行
    //    　　      41ドットの時、データは6バイト、2バイトpadding
    
    linenum = (sizeof SDcardBuffer) / len;   //バッファサイズに入るのは何ドット行分か
    //バッファは640バイトあり、5120ドット相当あるので、linenum==0にならない前提
    if (linenum >= fontYsize) {
        //バッファに入りきる場合は、ドット行数=Yドットサイズ
        linenum = fontYsize;
    }
    datasize = len * linenum;   //一度に読み込むデータサイズ(バイト数)
    
    //bmpのデータは、下から並んでいるで、表示させるのは下からというのに注意
    //画面をはみ出しても正常に描画できるように処理追加
    //画面内に入るように調整。描画する時に、はみ出していないかチェック要
//    if (Cursor_x < 0) xd1 = 0;
//    else xd1 = Cursor_x;

//    xd2 = Cursor_x + xsize -1;
//    if (xd2 >= LCD_WIDTH) xd2 = LCD_WIDTH -1;
//
//    //Y方向は、分割描画する時は、順次範囲を移動させる
//    y1 = Cursor_y;
//    y2 = Cursor_y + pitchY;

    //下から描画させたい。バッファに入りきらないbmpも対応
    remainingLine = 0;   //読み込む残り行数
    for (jj = 0; jj < fontYsize; jj++) {
        yp = Cursor_y + (fontYsize - jj - 1) * TextMagY;
        if (remainingLine == 0) {
            //読み込んだデータを使い切ったら、次のデータを読み込む
            res = f_read(&FileFont, SDcardBuffer, datasize, &actualLength);    //データを読み込む
            if (res != FR_OK) {
                f_close(&FileFont);
                return 44;
            }
            remainingLine = actualLength / len;    //読み込んだデータの行数
            pos = 0;
        }
        //addsetして、毎回x,y座標指定しない方が高速描画
        addset(Cursor_x, yp, Cursor_x + fontXsize * TextMagX -1, yp + TextMagY);
        for (ii = 0; ii < fontXsize; ii++) {
            if ((ii % 8) == 0) line = SDcardBuffer[pos++]; //8ドット毎にデータを取り込む
            xp = Cursor_x + ii * TextMagX;
            if (line & 0x80) {
                // 1の描画 背景
                if (TextbgColor != Textcolor) {
                    // Backカラーがfrontカラーと違う時は、その色を背景として塗る
                    if (TextMagX * TextMagY == 1)
//                        lcd_draw_pixel_at(xp, yp, TextbgColor);
                        draw_pixel(TextbgColor);
                    else
                        display_fillRect(xp, yp, TextMagX, TextMagY, TextbgColor);
                    //背景色を塗らない場合、アドレスが進まない問題がある
                } else {
//                    xp++;
//                    lcd_set_cursor_x(xp);
//                    read_pixel2nd(rgb, 1);
                    //描画しないアドレスを飛ばして再設定
                    addset(xp+TextMagX, yp, Cursor_x + fontXsize * TextMagX -1, yp + TextMagY);
                }
            } else {
                // 0=黒の描画　前景
                if (TextMagX * TextMagY == 1) //拡大していない時
//                    lcd_draw_pixel_at(xp, yp, Textcolor);
                    draw_pixel(Textcolor);
                else
                    display_fillRect(xp, yp, TextMagX, TextMagY, Textcolor);
            }
            line <<= 1;
        }
        remainingLine--;
        pos = pos + 3-((pos-1) % 4); //4バイト単位なので、切り上げる
    }    
    
//    for (yf = 0; yf < fontYsize; yf++) {
//        //一番下の行の座標=ここから描画
//        if (remainingLine == 0) {
//            //読み込んだデータを使い切ったら、次のデータを読み込む
//            res = f_read(&FileFont, SDcardBuffer, datasize, &actualLength);    //データを読み込む
//            if (res != FR_OK) {
//                f_close(&FileFont);
//                return 44;
//            }
//            remainingLine = actualLength / len;    //読み込んだデータの行数
//          
//            y1 = y2 - remainingLine*TextMagY;  //新しいy1座標を設定
//            if (y1 < 0) yd1 = 0;
//            else yd1 = y1;
//            if (y2 >= LCD_HEIGHT) yd2 = LCD_HEIGHT -1;
//            else yd2 = y2;
//
//            addset(xd1, yd1, xd2, yd2);    //描画領域を設定
//            offset = (remainingLine-1) * len;
//        }
//
//        //1行分のデータずつ読み込んで、描画すれば、縦横拡大の表示もプログラムしやすい
//        for (mm = 0; mm < TextMagY; mm++) {    //縦方向の倍率分繰り返す
//            k = 0;
//            for (x = 0; x < fontXsize; x++) {
//                if ((x % 8) == 0) line = SDcardBuffer[offset + (k++)]; //8ドットずつデータをlineに
//                //横方向の倍率分のドット描画を繰り返す
//                for (xf = 0; xf < TextMagX; xf++) {
//                    int16_t xxx = Cursor_x + x * TextMagX + xf;
//                    if ((xxx >= 0) && (xxx < LCD_WIDTH)) {
//                        // Backカラーがfrontカラーと違う時は、その色を背景として塗るということも要検討
//                        if (line & 0x80) draw_pixel(TextbgColor);
//                        else draw_pixel(Textcolor);
//                    }
//                }
//                line <<= 1;
//            }
//        }
//        y2 = y1;   //次の描画
//        remainingLine--;
//        offset -= len;  //1行分の長さ分進める(実際は戻す)
//    }
    f_close(&FileFont);
    
    px = fontXsize * TextMagX + XpitchAdj; //XpitchAdjをフォントのサイズに追加
    Cursor_x += px;
    py = fontYsize * TextMagY + YpitchAdj;
    if (Wrap && (Cursor_x + px) > LCD_WIDTH) {
        Cursor_x = 0;
        Cursor_y += py;
        if (Cursor_y > LCD_HEIGHT) Cursor_y = LCD_HEIGHT;
    }

    return 0;
}


/*
 * 設定されているカーソル位置、色、サイズをそのまま使う時はこれを呼び出す
 * 途中でエラーあったら、中止
 */
uint8_t displayCustom_puts(char *s) {
    uint8_t res;
    char str[100];
//    uint16_t preColor = Textcolor;
    
    while (*s) {
        res = displayCustom_putc(*s++);
        if (res) {
//#ifdef DEBUG
            //時刻の更新がされない状態の時、どこでエラー発生しているか確認するため
//            sprintf(str, "err=%d  ", res);
//            display_drawChars(260, 50, str, WHITE, TextbgColor, FontMagx11);
//#endif
            return 1;  //ここを変更   
        }
    }
//    Textcolor = preColor;
    return 0;
    
}


/*
 * ここで、内蔵フォントとSDカード上のフォントを使い分ける
 */
//uint8_t putsAnyFont(char *str) {
//    if (CurrentFont.data) {
//        display_puts(str);
//    } else {
//        return displayCustom_puts(str);
//    }
//    return 0;
//}

/*
 * テキスト表示する前処理
 */
void presetting(ObjectRsc *rsc) {

    SetFont(rsc->fontcode);   //フォントの設定
    display_setTextSize(rsc->fontmag);
    display_setTextPitch(rsc->xpitchAdj, 0);
    display_setCursor(rsc->x, rsc->y);        //描画開始座標
//    display_setTextColor(rsc->color, BLACK);
    display_setColor(rsc->color);
    CustomYsize = CurrentFont.ypitch * TextMagY + YpitchAdj;
}


// DateTimeの初期化
void resetPreDateTime() {
    uint8_t jj;
    
    for (jj = 0; jj < 3; jj++) preTime[jj] = 0xff;
}

/*
 * リソースデータから以下を取得
 * (x,y)座標を左上の起点
 * 文字の外形より2ドット分大きめの領域を描画域と想定
 * format: HHMMssなど
 * fontmag: フォントサイズ
 *  通常の7セグを指定された時、HHMMssのssは、小さい7セグフォントを使用
 * 　　　　　デフォルトで小さい7セグ指定された時は、同じ大きさ
 * サポートするフォントは、7seg、Custom0とCustom1の3つ
 * 
 */
void dispTime(uint8_t *datetime, ObjectRsc *rsc) {
    char ss[10];
//    int16_t tempYsize;

    //ベースとなるフォントのディレクトリを指定
    //Segment7NumかSegment7sNumを想定している
    presetting(rsc);

    //書き換えが必要な桁だけ描画していたが、描画を効率化して、毎回描画でも問題なくなった
    if (rsc->format == HHMM) {
        sprintf(ss, "%02x:%02x", datetime[2], datetime[1]);
        display_puts(ss);
    }
    else if (rsc->format == HHMMSS) {
        sprintf(ss, "%02x:%02x:%02x", datetime[2], datetime[1], datetime[0]);
        display_puts(ss);
    }
    else if (rsc->format == HHMMss) {
        sprintf(ss, "%02x:%02x", datetime[2], datetime[1]);
        //このルーチンで、Cursor自動的に進む
        display_puts(ss);
        
        //フォントサイズを小さく
        if (rsc->fontcode >= InternalFontNum) {
            // Userフォントで、smalllフォント定義ない時の処置が未了
            strcpy(Smaller, "s");
        } else {
            //Y方向だけ1サイズ小さくする
//            display_setTextSize(rsc->fontmag - 0x01); //MagYが0になっても、エラーにならないようになっている
            uint8_t xs = (rsc-> fontmag) >> 4;
            uint8_t ys = (rsc-> fontmag) & 0x0f;
            ys = ys / 2 + 1;
            xs = xs / 2 + 1;
            display_setTextSize((uint8_t)(xs << 4) + ys); //MagYが0になっても、エラーにならないようになっている
            
        }
        sprintf(ss, "%02x", datetime[0]);
        display_puts(ss);
        Smaller[0] = '\0';
    }
    display_setTextPitch(0, 0); //presettingとペアで元に戻す
}


// θ=0~179度まで、sinθの256倍にしたsinテーブル
//　アラーム針を5分単位にした場合、2.5度単位(30度で12分割)が欲しい。1度単位で妥協
//sin(θ): θ=角度
//180度以降は、-sin(θ-180)という形で参照
//cos(θ)=sin(θ+90)で計算
//1度単位のテーブル
const int16_t sin_table[] = {
    //0    1    2    3    4    5    6    7    8    9
      0,   4,   8,  13,  17,  22,  26,  31,  35,  40,   //00-
     44,  48,  53,  57,  61,  66,  70,  74,  79,  83,   //10-
     87,  91,  95, 100, 104, 108, 112, 116, 120, 124,   //20-
    128, 131, 135, 139, 143, 146, 150, 154, 157, 161,   //30-
    164, 167, 171, 174, 177, 181, 184, 187, 190, 193,   //40-
    196, 198, 201, 204, 207, 209, 212, 214, 217, 219,   //50-
    221, 223, 226, 228, 230, 232, 233, 235, 237, 238,   //60-
    240, 242, 243, 244, 246, 247, 248, 249, 250, 251,   //70-
    252, 252, 253, 254, 254, 255, 255, 255, 255, 255,   //80-
    256, 255, 255, 255, 255, 255, 254, 254, 253, 252,   //90-
    252, 251, 250, 249, 248, 247, 246, 244, 243, 242,   //100-
    240, 238, 237, 235, 233, 232, 230, 228, 226, 223,   //110-
    221, 219, 217, 214, 212, 209, 207, 204, 201, 198,   //120-
    196, 193, 190, 187, 184, 181, 177, 174, 171, 167,   //130-
    164, 161, 157, 154, 150, 146, 143, 139, 135, 131,   //140-
    128, 124, 120, 116, 112, 108, 104, 100,  95,  91,   //150-
     87,  83,  79,  74,  70,  66,  61,  57,  53,  48,   //160-
     44,  40,  35,  31,  26,  22,  17,  13,   8,   4,   //170-
};

//degreeで指定された角度=thetaに対応したsinを返す(x256)
int16_t sind(int16_t theta) {
    theta = (theta + 360) % 360;    //thetaがマイナスでも対応させるため

    if (theta >= 180) {
        //-sind(θ-180)
        return -sin_table[(UINT)(theta-180)];          //度単位のテーブル用
    } else {
        return sin_table[(UINT)theta];
    }
}

//degreeで指定された角度=thetaに対応したcosを返す(x256)
int16_t cosd(int16_t theta) {
    theta = theta+90;
    return sind(theta);
}

/*
 * アナログ時計表示
 * mode: 
 * datetime: 日付時間
 * xx, yy: 中心座標
 * size: 外形円の大きさ
 * color: 針の色指定
 */
#define SizeMin  3      //分針の太さ
#define SizeHour 4      //時針の太さ
//#define ColorHour   GREY
//#define ColorMin    GREY
#define ColorSec    RED
#define ColorAlarm  YELLOW

/*
 * アナログ時計のアラーム針を描画/消去
 */
void drawAlarmNeedle(int16_t *x, int16_t *y, uint16_t color) {
    int16_t xd, yd;
    
    //アラーム針描画
    display_drawLine(x[0], y[0], x[1], y[1], color);
    
    xd = (int16_t)(x[1]-x[0]);
    yd = (int16_t)(y[1]-y[0]);
    if ( ABS(yd) > ABS(xd)) {
        //角度により、移動する方向を変え、少し太くする
        display_drawLine(x[0]+1, y[0], x[1]+1, y[1], color);
        display_drawLine(x[0]-1, y[0], x[1]-1, y[1], color);
    } else {
        display_drawLine(x[0], y[0]-1, x[1], y[1]-1, color);
        display_drawLine(x[0], y[0]+1, x[1], y[1]+1, color);
    }
}

//時針、分針の描画/消去
void drawNeedle(int16_t *x, int16_t *y, uint16_t color, uint16_t color2) {
    display_fillTriangle(x[1], y[1], x[2], y[2], x[3], y[3], color);
    display_drawTriangle(x[1], y[1], x[2], y[2], x[3], y[3], color2);
    //反対側の出っ張り
    display_fillTriangle(x[4], y[4], x[2], y[2], x[3], y[3], color);
    display_drawTriangle(x[4], y[4], x[2], y[2], x[3], y[3], color2);

}

//秒針の描画/消去
void drawSecNeedle(int16_t *x, int16_t *y, uint16_t color) {
    display_drawLine(x[1], y[1], x[2], y[2], color);
    display_fillTriangle(x[0], y[0], x[3], y[3], x[4], y[4], color);
   
}

        
//アラーム針の長さは長針の75%
//長針：分針の長さは、目盛の線より3ドット内側
//短針
//秒針
//目盛の長さ=3
//ドットの大きさ=2
void drawAnalogClock(uint8_t *datetime, ObjectRsc *rsc, uint8_t *alarmtime) {
    int16_t rc = rsc->xw /2;   //外形の半径
    int16_t xc = rsc->x + rc;    //時計の中心座標
    int16_t yc = rsc->y + rc;
    uint16_t color = rsc->color;
    uint16_t bcolor;

    int16_t rc1, rcs, rcm, rch, rca;    //時計の目盛、秒針、分針、時針、アラーム
    int16_t rc2;
    int16_t rcs2, rcm2, rcm3, rch2, rch3, rca2;    //時計のサイズ、目盛、秒針、分針、時針、アラーム
    int16_t x[5], y[5];    //座標計算した結果を格納
    int16_t angle;
    uint8_t hh, mm, ss, jj, kk;
    static uint8_t almhh, almmm;
    //前回の座標
    static int16_t phx[5], phy[5], pmx[5], pmy[5], psx[5], psy[5], pax[2], pay[2];
    //今回の座標
    int16_t chx[5], chy[5], cmx[5], cmy[5], csx[5], csy[5], cax[2], cay[2];
    char str[3];
    int8_t minupdate = 0;
    
    //外形円の描画
    display_drawCircle(xc, yc, rc, color);  //sizeで指定された円
    rc = rc -3;
    display_drawCircle(xc, yc, rc, color);  //一回り小さめの円

    rc1 = rc -3;        // 一番長い長針の長さ=目盛より内側に設定
    rca = rc1 *3 /4;    //アラーム用の針の長さは、長針の75%
    rch = rc1 *7/10;    //短針の長さは、70%
    rcm = rc1 -5;       //分針の長さは、目盛の線より3ドット内側
    rcs = rc1 -4;       //秒針の長さは、これ
    rc2 = rc1 -10;      //文字盤の数字の位置

    //アラーム針の描画準備----------------------------------------------------
    //最新のアラーム針の座標を計算
    if (alarmtime != NULL) {
        almmm = Bcd2Hex(alarmtime[0]);
        almhh = Bcd2Hex(alarmtime[1]);
    }
    almhh = almhh % 12u;
    angle = almhh * 30 + almmm/2;   //角度に変換
    cax[0] = xc;    //中心座標
    cay[0] = yc;
    cax[1] = xc + rca * sind(angle)/256;    //アラーム針の先の座標
    cay[1] = yc - rca * cosd(angle)/256;
    
    //ちらつきをなくすため、表示に変化ある針だけを消去し、それ以外は上書きする
    //前の表示を消す
    if ((cax[1] == pax[1]) && (cay[1] == pay[1])) {
        //前と座標が変わっていない時は、消さない。最初の描画も同じ座標にしているので消去しない
    } else {
        drawAlarmNeedle(pax, pay, TextbgColor);   //元の秒針を消す
        //描画用の座標を保存
        for (jj=0; jj<2; jj++) {
            pax[jj] = cax[jj];
            pay[jj] = cay[jj];
        }
    }

    //分針の描画準備--------------------------------------------------------
    //分針の位置が変わったら、再描画の前に、元の時針、分針を消す
    mm = Bcd2Hex(datetime[1]);
    if (preTime[1] != datetime[1]) {
        preTime[1] = datetime[1];
//        mm = Bcd2Hex(datetime[1]);
        hh = Bcd2Hex(datetime[2]);

        //分針の座標を計算
        angle = mm*6;
        cmx[1] = xc + rcm * sind(angle)/256;    //針先の座標
        cmy[1] = yc - rcm * cosd(angle)/256;
    
        rcm2 = SizeMin;       //分針の幅
        angle = angle +90;  //針に対して90度の角度
        cmx[2] = xc + rcm2 * sind(angle)/256;
        cmy[2] = yc - rcm2 * cosd(angle)/256;
        cmx[3] = xc - (cmx[2]-xc);
        cmy[3] = yc - (cmy[2]-yc);

        //反対側の出っ張り
        rcm3 = 10;      //出っ張り長さ
        angle = angle +90;  //さらに90度足す
        cmx[4] = xc + rcm3 * sind(angle)/256;
        cmy[4] = yc - rcm3 * cosd(angle)/256;

        //時針の座標計算：分針が移動したら、時針も移動計算 (実際は2分毎)
        //　分のデータも取り込んで時針の角度決める
        hh = hh % 12;
        angle = hh * 30 + mm/2;   //角度に変換 8bit変数ではNG
        
//        chx[0] = xc;  //中心座標　　　未使用なので
//        chy[0] = yc;
        chx[1] = xc + rch * sind(angle)/256;    //針先の座標
        chy[1] = yc - rch * cosd(angle)/256;
        
        rch2 = SizeHour;    //時針の幅
        angle = angle +90;  //針に対して90度の角度
        chx[2] = xc + rch2 * sind(angle)/256;
        chy[2] = yc - rch2 * cosd(angle)/256;
        chx[3] = xc - (chx[2]-xc);
        chy[3] = yc - (chy[2]-yc);
        
        //反対側の出っ張り
        rch3 = 10;      //出っ張り長さ
        angle = angle +90;  //さらに90度足す
        chx[4] = xc + rch3 * sind(angle)/256;
        chy[4] = yc - rch3 * cosd(angle)/256;
        
        //分が変更になり時間が変わる時は、分の変更と同時に時針の消去
        drawNeedle(phx, phy, TextbgColor, TextbgColor);
        //分針の消去
        drawNeedle(pmx, pmy, TextbgColor, TextbgColor);
        minupdate = 1;  //分針updateした

        //描画座標を保存
        for (jj=1; jj<5; jj++) {
            phx[jj] = chx[jj];
            phy[jj] = chy[jj];
            pmx[jj] = cmx[jj];
            pmy[jj] = cmy[jj];
        }
    }

    //最新の秒針の座標を計算
    if (preTime[0] != datetime[0]) {
        preTime[0] = datetime[0];
        ss = Bcd2Hex(datetime[0]);  //0-59の数値
        angle = ss*6;
        csx[0] = xc;  //中心座標
        csy[0] = yc;
        csx[1] = xc + rcs * sind(angle)/256;
        csy[1] = yc - rcs * cosd(angle)/256;
        
        rcs2 = 20;   //反対側に出っ張る量
        angle = angle +180;
        csx[2] = xc + rcs2 * sind(angle)/256;
        csy[2] = yc - rcs2 * cosd(angle)/256;
        csx[3] = xc + rcs2 * sind(angle+6)/256;
        csy[3] = yc - rcs2 * cosd(angle+6)/256;
        csx[4] = xc + rcs2 * sind(angle-6)/256;
        csy[4] = yc - rcs2 * cosd(angle-6)/256;
        
        //秒針を消去
        drawSecNeedle(psx, psy, TextbgColor);

        //描画座標を保存
        for (jj=0; jj<5; jj++) {
            psx[jj] = csx[jj];
            psy[jj] = csy[jj];
        }
    }
    
    //目盛と数字の描画------------------------------------------------------------
    SetFont(FontNormal);
    for (jj=0; jj<60; jj++) {
        angle = jj*6;
        x[1] = xc + (rc * sind(angle)+127)/256; //単純に256で割ると切捨てになるので+127として四捨五入的にしてみた
        y[1] = yc - (rc * cosd(angle)+127)/256;
        x[2] = xc + (rc1 * sind(angle)+127)/256;
        y[2] = yc - (rc1 * cosd(angle)+127)/256;
        
        if (jj%5 == 0) {
            //5の倍数の所に文字盤の数値を表示
            display_fillCircle(x[2], y[2], 2, color);   //時の所は、ドット、半径2
            //文字盤表示
            if (!((minupdate == 0) && (jj == mm))) {
                //ただし、minupdate=0で、mmが5の倍数の時は、描画しない(そうしないとチラつく)
                //つまり、分針が数字にかかっていて、書き換えしない時は、数値は描画しないということ
                x[3] = xc + (rc2 * sind(angle))/256 -4;
                y[3] = yc - (rc2 * cosd(angle))/256 -3;
                //表示する数字を設定
                if (jj==0) kk = 12;   //0時の所は、12に設定
                else kk = jj/5;
                if (kk>9) x[3] = x[3] - 3;
                
                sprintf(str, "%d", kk);
                //背景色を同じにすると、背景色を塗らない=透明と同じ
                //1桁数値と2桁数値で位置を調整
                display_drawChars(x[3], y[3], str, color, color, FontMagx11);
            }
        } else {
            display_drawLine(x[1], y[1], x[2], y[2], color);
        }
    }
    
    // AM/PMの表示
    //ちらつきをなくすため、背景部分は変化させない描画を行うが、
    //AM/PMの切替時だけ、背景描画実施
    if (Bcd2Hex(preTime[2])/12 != Bcd2Hex(datetime[2])/12) {
        bcolor = TextbgColor;
    }
    else {
        bcolor = color;
    }
    preTime[2] = datetime[2];
    if (datetime[2] < 0x12) strcpy(str, "AM");
    else strcpy(str, "PM");
    //午前午後の表示を大きさに応じて変える
    if (rc > 70)
        display_drawChars(xc-14, yc+rc/2, str, color, bcolor, FontMagx22);
    else
        display_drawChars(xc-7, yc+rc/2, str, color, bcolor, FontMagx11);
    
    //ここから、針の描画を実行
    //アラーム針描画------------------------------------------------------
    drawAlarmNeedle(pax, pay, ColorAlarm);  //アラーム針の色

    //時針の描画------------------------------------------------------
    drawNeedle(phx, phy, color, WHITE);    //時針の色は、引数からもらう

    //分針の描画------------------------------------------------------
    drawNeedle(pmx, pmy, color, WHITE);

    //秒針の描画------------------------------------------------------
    display_fillCircle(xc, yc, 3, ColorSec);    //秒針の中央円
    drawSecNeedle(psx, psy, ColorSec);
    
}

/*
 * リソースで指定された設定で時刻を表示
 */
void DrawTime(uint8_t *datetime, ObjectRsc *rsc) {

    if (rsc->disp) {
        //RscIDをそのオブジェクトの有無(=表示するかどうか)に利用
        if (rsc->format == ANALOGCLOCK) {
            //外形円の座標= (140, 130) 半径=90
            //アナログ時計表示の場合は、アラーム針も同時に表示
            drawAnalogClock(datetime, rsc, AlarmTime);
        } else {
            dispTime(datetime, rsc);
        }
    }
}


/*
 * リソースで指定されたフォントに応じて描画
 * 　　Small:　月と曜日はNormalフォント使用する
 * 　　Normal: 全部ノーマルで描画
 * 左上の座標: xs, ys
 * 描画する年月: year, month
 */
void basicDrawCalendar(uint8_t year, uint8_t month, int16_t xs, int16_t ys, ObjectRsc *rsc) {
    uint8_t jj, kk;
    int16_t xx, yy;
    char str[5];
    int8_t currentDay;    //最初の日曜の日付、0以下は前月
    int8_t maxdays;
    uint8_t yr, mm, dd, wd;
    uint16_t color;         //文字色
//    uint16_t backColor = BGColor; //背景色
    uint8_t thismonth;
    //曜日ごとの色指定
    uint16_t dayColor[7];   // = {RED, WHITE, WHITE, WHITE, WHITE, WHITE, BLUE, };
    uint8_t xpitch, ypitch; // 表示ピッチ
    uint8_t mul;

    //前月、次月の表示のため、0月、13月という設定を許すので、ここで修正
    if (month == 0) {
        month = 12;
        year--;
    }
    if (month == 13) {
        month = 1;
        year++;
    }

    yr = year;
    mm = month;
    dd = 1;
    wd = getWeekdays(&yr, &mm, &dd);  //1日の曜日を取得
    //表示する最初の日付を仮想的に何日か設定
    //1日が日曜なら1のまま、月曜なら最初の日曜日は0日、火曜なら-1、水曜なら-2になる
    currentDay = (int8_t)(1- wd);
    
    //今月のカレンダーを表示しているかをチェック。今日の日付に印をつけるかどうか判断させるため
    if (month == Bcd2Hex(DateTime[5])) thismonth = 1;
    else thismonth = 0;

    //当該月の最終日を取得。その月が何日あるかを把握
    dd = 31;
    getWeekdays(&yr, &mm, &dd);  //31日の曜日を取得。その日がない場合、自動調整される
    //mmが変更になったら、31日ではなかったとわかる。
    //ddが31日が1日に変わっていたら、その月は30日だったとわかる。2月、うるう年も対応。
    if (month != mm) {
        maxdays = (int8_t)(31 - dd);    //その月の最終日
    } else {
        maxdays = (int8_t) dd;
    }   
    
    //曜日ごとの色をリソースから取得
    color = rsc->color;
    for (jj = 1; jj < 6; jj++) dayColor[jj] = color;
    dayColor[0] = rsc->attribute2;   //日曜日の色
    dayColor[6] = rsc->attribute;    //土曜日の色
    
    //指定された基本フォント情報を取得
    SetFont(rsc->fontcode);
    display_setTextSize(rsc->fontmag);    //fontsizeの上位4bit=X倍率、下位4bit=Y倍率
    xpitch = (uint8_t)(CurrentFont.xsize *TextMagX *2 + rsc->xpitchAdj);  //1日=2文字+スペース
    ypitch = (uint8_t)(CurrentFont.ypitch * TextMagY);
   
    //対象領域をクリア: fontwで14文字分+7日のスペース分、Y方向は8行分
    //xの開始を1ドット左にして、サイズもその分大きく。枠を付けた時はみ出しを防止
    CalendarXsize = xpitch*7 + 10;
    CalendarYsize = ypitch * 8;
    xs++;
    display_fillRect(xs-1, ys, CalendarXsize+1, CalendarYsize+1, TextbgColor);
    
    //月の描画
    SetFont(FontNormal);
    sprintf(str, "%d", month);
    xx = xs + (int16_t)(xpitch*7 - strlen(str) * CurrentFont.xsize *2)/2;    //中央に表示させる
    //月表示は横方向2倍で描画
    if (rsc->fontcode < InternalFontNum)
        display_drawChars(xx, ys, str, color, TextbgColor, rsc->fontmag +0x10);
    else
        display_drawChars(xx, ys, str, color, TextbgColor, CurrentFont.mag +0x10);

    yy = ys;
    //曜日
    //曜日の幅を2倍しても、2桁の日付の幅より狭ければ、X=2倍にする
    if (CurrentFont.xsize *2 < xpitch) mul = 2;
    else mul = 1;
    yy += ypitch;
    xx = xs+ (xpitch - CurrentFont.xsize * mul)/2;   //中央になるように
    for (jj=0; jj<7; jj++) {
        if (CalendarEJ) 
            sprintf(str, "%c", WeekDays[jj][0]);  //SMTWTFSという表記方式
        else
            sprintf(str, "%c", FSunday+jj);    //日月火水木金土という表示
    if (rsc->fontcode < InternalFontNum)
        display_drawChars(xx, yy, str, dayColor[jj], TextbgColor, rsc->fontmag +0x10*(mul-1));
    else
        display_drawChars(xx, yy, str, dayColor[jj], TextbgColor, CurrentFont.mag +0x10);
        xx += xpitch;
    }
    
    //日付
    yy += ypitch;
    SetFont(rsc->fontcode);
    display_setTextPitch(0, 0);

    for (kk = 0; kk < 6; kk++) {
        xx = xs;
        for (jj = 0; jj < 7; jj++) {
            if (currentDay > 0 && currentDay <= maxdays) {
                display_setCursor(xx, yy);
//                display_setTextColor(dayColor[jj], TextbgColor);
                display_setColor(dayColor[jj]);
                
                sprintf(str, "%2d", currentDay);
//                display_drawChars(xx, yy, str, dayColor[jj], TextbgColor, rsc->fontmag);
                display_puts(str);
                
                //今日の日付に印をつける
                if (thismonth && (currentDay == Bcd2Hex(DateTime[4]))) {
                    //反転だと視認しにくいので枠を付ける
                    display_drawRect(xx-1, yy-2*TextMagY, CurrentFont.xsize *TextMagX *2 +1, ypitch+TextMagY, WHITE);
                }
            }
            currentDay++;
            xx += xpitch;
        }
        yy += ypitch;
    }
}


/*
 * Formatに応じてカレンダーを表示する
 * 現在5種類のフォーマット
 * Month1: 今月1か月のカレンダーを表示。フォントサイズ変えることで、大きくも小さくも表示可
 * Month3: 前後の月を含めて3か月分のカレンダーを横配置で表示
 * Month3v:前後の月を含めて3か月分のカレンダーを縦配置で表示
 * 　引数の年月は、表示させる月を指す
 * 　BCDではない
 * 表示位置は、リソースで指定
 * 
 */
void DrawCalendar(ObjectRsc *rsc) {
    uint8_t jj;

#ifdef ENGLISH
    CalendarEJ = 1;
#else
    CalendarEJ = 0;
#endif
    if (rsc->disp) {
        if (rsc->format == Month1) {
            // 1か月のカレンダーを表示　　大きさはフォントサイズで指定
            basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5]), rsc->x, rsc->y, rsc);
            CustomXsize = CalendarXsize;
            CustomYsize = CalendarYsize;
        } else if (rsc->format == Month3) {
            // 3か月分のカレンダーを表示
            //3か月カレンダのX,Y座標は、リソースから取得
            //CalendarXsizeは、最初の月を描画すると計算される
            for (jj=0; jj<3; jj++) {
                basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5])+jj-1, rsc->x +CalendarXsize*jj, rsc->y, rsc);
            }
            CustomXsize = CalendarXsize *3;
            CustomYsize = CalendarYsize;
        } else if (rsc->format == Month3v) {
            // 3か月分のカレンダーを縦に表示
            //縦表示の時は、2か月分しか表示しない→表示が欠けても良しとする
            //CalendarYsizeは、最初の月を描画すると計算される
            for (jj=0; jj<3; jj++) {
                basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5])+jj-1, rsc->x, rsc->y + CalendarYsize*jj, rsc);
            }
            CustomXsize = CalendarXsize;
            CustomYsize = CalendarYsize *3;
        } else if (rsc->format == Month2) {
            // 2か月分のカレンダーを表示
            for (jj=0; jj<2; jj++) {
                basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5])+jj, rsc->x +CalendarXsize*jj, rsc->y, rsc);
            }
            CustomXsize = CalendarXsize *2;
            CustomYsize = CalendarYsize;
        } else if (rsc->format == Month2v) {
            // 2か月分のカレンダーを縦に表示
            for (jj=0; jj<2; jj++) {
                basicDrawCalendar(Bcd2Hex(DateTime[6]), Bcd2Hex(DateTime[5])+jj, rsc->x, rsc->y + CalendarYsize*jj, rsc);
            }
            CustomXsize = CalendarXsize;
            CustomYsize = CalendarYsize *2;
        }
    }
}

/*
 * アラーム時刻を表示　　スイッチの状態で色を変化させる
 * sw SlideSWoff=Alarm off, SlideSWon=Alarm on
 */
void DrawAlarmTime(uint8_t *alarmtime, uint8_t sw, ObjectRsc *rsc) {
    char str[100];
    char ampm[][3] = {"AM", "PM"};
    uint8_t ap;
    char onOff; //[] = {'-', '+'};    //オンオフで+-切替　plus/minus
    uint16_t color;
    uint8_t hr;
    
    if (rsc->disp) {
        //アラーム時刻の表示色を、スライドSWのOn/Offで変える
        if (sw == SlideSWon) {
            //Onの時
            onOff = '+';
            color = rsc->color;
        } else {
            //Offの時
            onOff = '-';
            color = rsc->attribute;  //アラームのオフ時の色はattributeに格納
        }
        
        if (rsc->format == ALMAMPM) {
            //12時間表示
            hr = Bcd2Hex(alarmtime[1]);
            if (alarmtime[1] >= 0x12) {
                ap= 1 ;   //BCDで12時以降なら午後  
                hr -= 12;   //12時間表記のため、12差し引く
            }
            else {
                ap = 0;
            }
            sprintf(str, "ALM%c %s %02d:%02x", onOff, ampm[ap], hr, alarmtime[0]);
        } else {
            //24時間表示
            sprintf(str, "ALM%c %02x:%02x", onOff, alarmtime[1], alarmtime[0]);
        }
        
        presetting(rsc);
        display_setColor(color);    //SWに対応して色を変更させるため
        display_puts(str);
        display_setTextPitch(0, 0); //presettingとペアで元に戻す

    }
}

/*
 * 今日の日付を表示
 * 表示方法などは、リソース内で、指定する
 * 　　format, フォントなど
 */
void DrawDate(ObjectRsc *rsc) {
    char str[100];
    uint8_t *datetime = DateTime;
    char sss[50];
    
    if (rsc->disp) {
        switch (rsc->format) {
            case YYYYMMDDw:
                //NormalFontでは、曜日のフォントがFSunday=0x17-にある
                sprintf(str, "20%02x/%02x/%02x(%c)", datetime[6], datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case YYYYMMDDwe:  //曜日の英語表記
                sprintf(str, "20%02x/%02x/%02x(%s)", datetime[6], datetime[5], datetime[4], WeekDays[datetime[3]]);
                break;
            case YYYYMMDD:
                sprintf(str, "20%02x/%02x/%02x", datetime[6], datetime[5], datetime[4]);
                break;
            case YYMMDDw:
                sprintf(str, "%02x/%02x/%02x(%c)", datetime[6], datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case YYMMDDwe:
                sprintf(str, "%02x/%02x/%02x(%s)", datetime[6], datetime[5], datetime[4], WeekDays[datetime[3]]);
                break;
            case YYMMDD:
                sprintf(str, "%02x/%02x/%02x", datetime[6], datetime[5], datetime[4]);
                break;
            case MMDDw:
                sprintf(str, "%02x/%02x(%c)", datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case MMDDwe:
                sprintf(str, "%02x/%02x(%s)", datetime[5], datetime[4], WeekDays[datetime[3]]);
                break;
            case MMDD:
                sprintf(str, "%02x/%02x", datetime[5], datetime[4]);
                break;
        };
        presetting(rsc);
        display_puts(str);
        display_setTextPitch(0, 0); //presettingとペアで元に戻す
    }
}


/*
 * 温度を表示
 */
void DrawTemp(int16_t temp, ObjectRsc *rsc) {
    char str[50];
    int16_t ftemp;

    if (rsc->disp) {
        if (rsc->format == DEGF) {
            //tempは、10倍の摂氏　　　10F = 9/5*(10C) + 320
            ftemp = temp * 9*2 +320*10;
            sprintf(str, "%2d%c", ftemp / 100, FdegF);  //
        } else {
            sprintf(str, "%2d.%1d%c", temp / 10, temp % 10, FdegC);  //FdegC=℃のコード
        }
        
        presetting(rsc);
        display_puts(str);
        display_setTextPitch(0, 0); //presettingとペアで元に戻す
    }
}

/*
 * 湿度を表示
 */
void DrawHumidity(int16_t humidity, ObjectRsc *rsc) {
    char str[50];

    if (rsc->disp) {
        presetting(rsc);

        sprintf(str, "%2d%%", humidity / 10);
        display_puts(str);
        display_setTextPitch(0, 0); //presettingとペアで元に戻す

    }
}

/*
 * 歯車アイコン32x32ドットを(x,y)座標に描画
 */
void DrawGearIcon(int16_t x, int16_t y, uint16_t color) {
    uint16_t ii, jj, pos;
    int16_t x2, y2;
    uint8_t line;
    
    pos = 0;
    x2 = x + 31; //GearXw-1
    y2 = y + 31;
    //画面をはみ出しても正常に描画できるように処理追加
    if (x2 >= LCD_WIDTH) x2 = LCD_WIDTH -1;
    if (y2 >= LCD_HEIGHT) y2 = LCD_HEIGHT -1;
    addset(x, y, x2, y2);    //描画領域を設定

    for (ii = 0; ii <= y2-y; ii++ ) {
        //Y方向は、ループを制限
        for (jj = 0; jj < 32; jj++) {
            //8ドット毎にデータ読み出す
            if ((jj % 8) == 0) line = GearIcon[pos++];
            if (jj <= x2 - x) {
                //X方向は、データを読み出す都合上、通常動作で描画しないことで対応
                if (line & 0x80) draw_pixel(color);
                else draw_pixel(TextbgColor);  //背景色
            }
            line <<= 1;
        }
    }
}


/*
 * ボタンの中央に文字を描画する
 * 座標等はMainFormRsc情報から取得するが、色は変更できるよう別引数にしておく
 */
void DrawStrCenter(MainFormRsc rsc, uint16_t color, uint16_t bcolor) {
    int16_t xp, yp;

    //ボタンの中央にボタン名を表示
    display_setTextSize(rsc.fontmag);
    xp = rsc.x + rsc.xw/2 - (int16_t)strlen(rsc.str)* CurrentFont.xpitch * TextMagX /2;
    yp = rsc.y + rsc.yw/2 - (CurrentFont.ysize-1) * TextMagY /2;
    display_drawChars(xp, yp, rsc.str, color, bcolor, rsc.fontmag);
}

/*
 * ボタンを描画
 * fcolor: 文字の色
 * bcolor: ボタンの背景色
 * rcolor: ボタンの枠の色を指定
 * ボタンの角を丸める指定も
 */
void DrawButton(MainFormRsc rsc, uint16_t fcolor, uint16_t bcolor, uint16_t rcolor, int16_t r) {
    
    display_fillRoundRect(rsc.x, rsc.y, rsc.xw, rsc.yw, r, bcolor);
    display_drawRoundRect(rsc.x, rsc.y, rsc.xw, rsc.yw, r, rcolor);
    //ボタンの中央にテキスト表示
    DrawStrCenter(rsc, fcolor, bcolor);

}
