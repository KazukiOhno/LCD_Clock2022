/*
 * File:   LCD320x240color.c
 * Author: K.Ohno
 *
 * 「PICのブログ」http://picceri.blogspot.com/2019/01/320240-lcd.html
 * からライブラリをいただき、それをベースにMCC用、かつ8bit PIC用に書き換えました。
 * 2020/11/2
 * 
 */
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                            GFX_Library.c                          ////
////                                                                   ////
////                 Graphics library for mikroC compiler.             ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////               This is a free software with NO WARRANTY.           ////
////                     https://simple-circuit.com/                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////

#include "LCD320x240color.h"
#include "mcc_generated_files/drivers/spi_master.h"
#include "Draw.h"
#include "font.h"


#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

// 絶対値を取得する関数マクロ定義
#define ABS(x) ((x) < 0 ? -(x) : (x))

/*
 * Cursor_x,y: 文字表示の起点となる座標。2文字目は、表示するフォントのサイズ、
 * 　　　　　その表示する時の倍率、ピッチで計算する
 * 　見た目を変更したい場合、文字の大きさと左右上下の文字との間隔を変更する
 * リソースの中にxpitchAdjという文字間調整用変数もある
 * フォントは、高さは揃えているが、プロポーショナル対応で横幅は一定でないので、処理に注意必要
 * 内蔵フォントは、横幅固定
 */
int16_t
  Cursor_x = 0,          // x location to start print()ing text
  Cursor_y = 0;          // y location to start print()ing text
uint16_t
  Textcolor = WHITE,    // 16-bit background color for print()
  TextbgColor = BLACK;  // 16-bit text color for print()
uint8_t TextMagX, TextMagY;
bool
  Wrap = false;           ///< If set, 'Wrap' text at right edge of display
int8_t XpitchAdj = 0;
int8_t YpitchAdj = 0;

//SD card
FIL FileFont;   //フォントファイル用

// Prototype


//MCCのSPIのルーチンを呼び出すように変更
/**
 * This function send data or register-information via SPI to the LC-Display
 * @param value Value to send to the LCD-Controller
 */
void write_command(uint8_t cmd) {
    LCD_DCRS_SetLow();  //コマンドモード
    SPI1_ExchangeByte(cmd);
    LCD_DCRS_SetHigh();  //デフォルトをデータモードとさせる
}

//データの連続処理が可能になるよう、DCRSは基本High
//以下の形でも動作するが、プログラムサイズは変わらなかった
//#define write_data(data)     SPI1_ExchangeByte(data)
void write_data(uint8_t data) {
    SPI1_ExchangeByte(data);
}

//wordデータの書き込み
// Intelと同じLower byte first
void write_wdata(uint16_t data) {
    
    SPI1_ExchangeByte(data >> 8);
    SPI1_ExchangeByte(data & 0xff);
//    SPI1_WriteBlock(&data, 2);    //これはダメ。順序が逆になってしまう
}

//wordデータの読出し
uint16_t read_wdata() {
    uint16_t data;

    data = SPI1_ExchangeByte(0);
    data = (data << 8) | SPI1_ExchangeByte(0);
    
    return data;
}

uint8_t read_data() {
    return SPI1_ExchangeByte(0);
    
}

/**************************************************************************
    @brief   Set text font color with custom background color
    @param   c   16-bit 5-6-5 Color to draw text with
    @param   b   16-bit 5-6-5 Color to draw background/fill with
 **************************************************************************/
void display_setColor(uint16_t c) {
    Textcolor   = c;
}

void display_setTextColor(uint16_t c, uint16_t b) {
    Textcolor   = c;
    TextbgColor = b;
}

/**************************************************************************
    @brief   Set text 'magnification' size. Each increase in s makes 1 pixel that much bigger.
    @param  s  Desired text size. 1 is default 2 is twice.
 **************************************************************************/
void display_setTextSize(uint8_t s) {
    uint8_t mag = (s > 0) ? s : FontMagx11;

    //Textsizeの上位4bit=X倍率、下位4bit=Y倍率
    TextMagX = (uint8_t) (mag >> 4);
    TextMagY = mag & 0x0fu;
    //エラー回避のため、最低サイズを1とする
    if (TextMagX < 1) TextMagX = 1;
    if (TextMagY < 1) TextMagY = 1;
}

/*
 * 現在のフォントのピッチをベースに、フォントの拡大率を乗じて、調整分加算
 */
void display_setTextPitch(int8_t adjX, int8_t adjY) {

    XpitchAdj = adjX;
    YpitchAdj = adjY;
}


void display_setWrap(bool w) {
    Wrap = w;
}

// データを書き込むエリアの指定  描画範囲を設定
// 4つの引数はすべて画面上の座標
void lcd_addset(int16_t x, int16_t y, int16_t xx, int16_t yy) {
    
    write_command(0x2A);
    write_wdata((uint16_t)x);
    write_wdata((uint16_t)xx);
    write_command(0x2B);
    write_wdata((uint16_t)y);
    write_wdata((uint16_t)yy);
    write_command(0x2C);    
}


void addset(int16_t x, int16_t y, int16_t xx, int16_t yy) {

    if (!SPI1_Open(LCD8M_CONFIG)) return;
    //ちゃんと開けたら
    LCD_CS_SetLow();    //CSをLowに
    lcd_addset(x, y, xx, yy);
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();

}
/*******************************************************************************
 * Set the cursor to the coordinate (x,_)
 */
uint8_t lcd_set_cursor_x(uint16_t x) {
    
    if ( x >= LCD_WIDTH ) return EXIT_FAILURE;

    if (!SPI1_Open(LCD8M_CONFIG)) return EXIT_FAILURE;
    //ちゃんと開けたら
    LCD_CS_SetLow();    //CSをLowに
    write_command(0x2A);    //Column Address Set
    write_wdata(x);
    write_wdata(x);
    write_command(0x2C);
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();
    
    return EXIT_SUCCESS;
}

/*******************************************************************************
 * Set the cursor to the coordinate (_,y)
 */
uint8_t lcd_set_cursor_y(uint16_t y) {
    
    if( y >= LCD_HEIGHT ) return EXIT_FAILURE;
    
    if (!SPI1_Open(LCD8M_CONFIG)) return EXIT_FAILURE;
    //ちゃんと開けたら
    LCD_CS_SetLow();    //CSをLowに
    write_command(0x2B);    // Page Address Set
    write_wdata(y);
    write_wdata(y);
    write_command(0x2C);
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();

    return EXIT_SUCCESS;
}

/*******************************************************************************
 * Set the cursor at position (x,y)
 */
uint8_t lcd_set_cursor(uint16_t x, uint16_t y) {
    if ( lcd_set_cursor_x((uint16_t)x) || lcd_set_cursor_y((uint16_t)y) ) {
        return EXIT_FAILURE;
    }	
    return EXIT_SUCCESS;
}

/**************************************************************************
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
 **************************************************************************/
void display_setCursor(int16_t x, int16_t y) {
    Cursor_x = x;
    Cursor_y = y;
}


/*******************************************************************************
 * Draw a Pixel at the actual position
 */
uint8_t draw_pixel(uint16_t color) {

    if (!SPI1_Open(LCD8M_CONFIG)) return EXIT_FAILURE;
    //ちゃんと開けたら
    LCD_CS_SetLow();    //CSをLowに
    write_wdata(color);
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();

    return EXIT_SUCCESS;
}

/*******************************************************************************
 * This function will draw a Pixel at (x,y) with color
 */
void lcd_draw_pixel_at(int16_t x, int16_t y, uint16_t color) {
    if ( !lcd_set_cursor((uint16_t)x, (uint16_t)y) ) {
        draw_pixel(color);
    }
}


/**************************************************************************
    @brief   Given 8-bit red, green and blue values, return a 'packed'
             16-bit color value in '565' RGB format (5 bits red, 6 bits
             green, 5 bits blue). This is just a mathematical operation,
             no hardware is touched.
    @param   red    8-bit red brightnesss (0 = off, 255 = max).
    @param   green  8-bit green brightnesss (0 = off, 255 = max).
    @param   blue   8-bit blue brightnesss (0 = off, 255 = max).
    @return  'Packed' 16-bit color value (565 format).
 * red,green,blueは、1bit 右シフトされた形で引き渡される
 * R: bit6-2の5bitにデータ
 * G: bit6-1の6bitにデータ
 * B: bit6-2の5bitにデータ
 **************************************************************************/
uint16_t display_color565(uint8_t red, uint8_t green, uint8_t blue) {
    return ((uint16_t)(red & 0x7C) << 9) | ((uint16_t)(green & 0x7E) << 4) | ((blue & 0x7C) >> 2);
}


/*
 * 現在のアドレスのデータ=色情報をnumドット分読み出す
 * 先頭ドットの色を565colorフォーマットで返す
 * 
 * この状態だとエラーが判別できない
 */
uint16_t read_pixel(uint8_t *rgbdata, uint8_t num) {
    uint8_t dummy, jj;
    
    if (!SPI1_Open(LCD8M_CONFIG)) return EXIT_FAILURE;
    //ちゃんと開けたら
    LCD_CS_SetLow();    //CSをLowに
    write_command(0x2E);    // Memory Read
    dummy = read_data();    // 最初の1バイトはダミー
    
    for (jj=0; jj<num; jj++) {
//        rgbdata[jj*3+0] = read_data();   //R: bit6-2の5bitにデータ
//        rgbdata[jj*3+1] = read_data();   //G: bit6-1の6bitにデータ
//        rgbdata[jj*3+2] = read_data();   //B: bit6-2の5bitにデータ
        SPI1_ReadBlock(&rgbdata[jj*3], 3);  //3byteでRGBデータ取得
    }
    write_command(0x2C);
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();
    //
//    return display_color565(rgbdata[0], rgbdata[1], rgbdata[2]);
    return (((uint16_t)rgbdata[0] & 0x7c) << 9) | (((uint16_t)rgbdata[1] & 0x7e) << 4) | ((rgbdata[2] & 0x7c) >> 2);
}

uint16_t read_pixel2nd(uint8_t *rgbdata, uint8_t num) {
    uint8_t dummy, jj;
    
    if (!SPI1_Open(LCD8M_CONFIG)) return EXIT_FAILURE;
    //ちゃんと開けたら
    LCD_CS_SetLow();    //CSをLowに
    write_command(0x3E);    // Memory Read
    dummy = read_data();    // 最初の1バイトはダミー
    
    for (jj=0; jj<num; jj++) {
//        rgbdata[jj*3+0] = read_data();   //R: bit6-2の5bitにデータ
//        rgbdata[jj*3+1] = read_data();   //G: bit6-1の6bitにデータ
//        rgbdata[jj*3+2] = read_data();   //B: bit6-2の5bitにデータ
        SPI1_ReadBlock(&rgbdata[jj*3], 3);
    }
    write_command(0x2C);
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();
    return (((uint16_t)rgbdata[0] & 0x7c) << 9) | (((uint16_t)rgbdata[1] & 0x7e) << 4) | ((rgbdata[2] & 0x7c) >> 2);
}

/*
 * code: 2バイトShift JISコード
 * data: フォントのデータへのポインタ。サイズは、FontXsize * FontYsize /8
 */
uint8_t ReadFontData(uint16_t code, uint8_t *data) {
    DWORD ofsAdd;   //32bit
    DWORD add1stCode;   //上位バイト毎に格納されているオフセットデータの場所を保持
    uint16_t size;   //1文字当たりのデータサイズ(バイト)
    UINT actualLength;
    uint8_t error = 1;

//    if (SDcardMount == 0) return;
    // ファイルが開いていたら、データを読み出す
    if (CurrentKanjiFont.data == NULL) return error;

    //ヘッダサイズ+(漢字上位バイト-0x80)*4のアドレスへシーク
    if (code < 0xe000) {
        //上位バイトが0x80-0x9fの時
        add1stCode = ((code >> 8) - 0x80)*4;
    } else {
        //上位バイトが0xe0-0xffの時
        add1stCode = ((code >> 8) - 0xc0)*4;
    }
       
    //フォント格納アドレスを取得
    ofsAdd = KanjiOffsetTable[add1stCode+3];
    ofsAdd = (ofsAdd<<8) + KanjiOffsetTable[add1stCode+2];
    ofsAdd = (ofsAdd<<8) + KanjiOffsetTable[add1stCode+1];
    ofsAdd = (ofsAdd<<8) + KanjiOffsetTable[add1stCode];
    
    //対象の文字コード上位バイトの先頭のデータが格納される4バイトのアドレス
    size = (uint16_t)CurrentKanjiFont.xsize * CurrentKanjiFont.ysize / 8; //フォントデータの大きさ(バイト)
    ofsAdd = ofsAdd + ((code & 0xff) - 0x40) * size;    //2バイト目
    if (f_lseek(&FileFont, ofsAdd) == FR_OK) {
        if (f_read(&FileFont, data, size, &actualLength) == FR_OK) {
            error = 0;
        }
    }
    return error;
}


/**************************************************************************
    @brief  Print one byte/character of data
    @param  c  The 8-bit ascii character to write
 * 
 * display_putcと同様の処理の漢字フォント版
 * 
 * オリジナルの描画ルーチンは、1ドットずつ描画するので、非常に非効率
 * 漢字1文字ずつ表示しているのが見えるほど遅い
 * 1ドットずつX,Y座標を指定して、描画というのを繰り返しているので、描画領域を
 * 指定して、あとは描画だけというアルゴリズムの方が確実に早くできるはず。
 * ただし、背景色と文字色両方を指定した色で描画するなら問題ないが、背景は元の
 * 描画を残すような表示にしたい場合、何かやり方があるのか現状不明
 * →　問題はここにはなかった。SDカードへのアクセスが多すぎたのが原因だった
 * 
 * 漢字フォントデータは、従来と異なり、横方向の8ドットを1バイトデータとする
 * 素直なフォーマットに変更した。(従来は、縦8ドットを1バイトデータ)
 * 
 **************************************************************************/
void display_putK(uint16_t c) {
    uint8_t ii, jj;
    uint16_t pos;
    uint8_t line;
    uint8_t fontXsize = CurrentKanjiFont.xsize;
    uint8_t fontYsize = CurrentKanjiFont.ysize;
    uint8_t fontdata[72];   //24x24ドットフォントまで対応できるように
    int16_t xp, yp; //座標
    int16_t px, py; //倍率を取り込んだpitch

    //フォントデータを取り込む
    if (!ReadFontData(c, fontdata)) {
        //エラーでない時だけ処理
        pos = 0;   //データの先頭位置
        for (jj = 0; jj < fontYsize; jj++) {
            yp = Cursor_y + jj * TextMagY;
            for (ii = 0; ii < fontXsize; ii++) {
                if ((ii % 8) == 0) {
                    line = fontdata[pos++]; //8ドット毎にデータを取り込む
                }
                xp = Cursor_x + ii * TextMagX;
                if (line & 0x80) {
                    // 1の描画
                    if (TextMagX * TextMagY == 1) //拡大していない時
                        lcd_draw_pixel_at(xp, yp, Textcolor);
                    else
                        display_fillRect(xp, yp, TextMagX, TextMagY, Textcolor);
                } else {
                    // 0の描画
                    if (TextbgColor != Textcolor) {
                        // Backカラーがfrontカラーと違う時は、その色を背景として塗る
                        if (TextMagX * TextMagY == 1)
                            lcd_draw_pixel_at(xp, yp, TextbgColor);
                        else
                            display_fillRect(xp, yp, TextMagX, TextMagY, TextbgColor);
                    }
                }
                line <<= 1;
            }
        }
    }
    
    px = CurrentKanjiFont.xpitch * TextMagX + XpitchAdj;
    Cursor_x += px;
    if (Wrap && (Cursor_x + px > LCD_WIDTH)) {
        Cursor_x = 0;
        py = CurrentKanjiFont.ypitch * TextMagY + YpitchAdj;
        Cursor_y += py;
        if (Cursor_y > LCD_HEIGHT) Cursor_y = LCD_HEIGHT;
    }
    if (Cursor_x > LCD_WIDTH) Cursor_x = LCD_WIDTH;

}


/**************************************************************************
    @brief  Print one byte/character of data
    @param  c  The 8-bit ascii character to write
 * CurrentFontで指定された内蔵フォントを1文字描画
 * \rで、左端に戻る
 * \nで、次の行に行くけど、X座標は変わらない
 * CurrentFontのデータを使って描画
 * 描画時に使用する変数として、以下を参照する
 *   Cursor_x, Cursor_y, Textcolor, TextbgColor, 
 *   TextMagX, TextMagY, CurrentKanjiFont.mag, 
 *   XpitchAdj, YpitchAdj
 **************************************************************************/
void display_putc(char c) {
    uint8_t ii, jj;
    uint16_t pos;
    uint8_t line;
    uint8_t fontXsize = CurrentFont.xsize;
    uint8_t fontYsize = CurrentFont.ysize;
    uint8_t xmul, ymul;
    int16_t xp, yp; //座標
    int16_t px, py; //倍率を取り込んだpitch

    xmul = (uint8_t) (TextMagX * (CurrentKanjiFont.mag >> 4));      //X方向倍率
    ymul = (uint8_t) (TextMagY * (CurrentKanjiFont.mag & 0x0f));    //Y方向倍率
//    if (c == ' ' && Cursor_x == 0 && Wrap)  //行の先頭の空白は削除
//        return;
    if (c == '\r') {    //改行=Carriage return　左端にカーソル戻す
        Cursor_x = 0;
        return;
    }
    if (c == '\n') {    //改行=Line Feed　イメージ的には次の行へ行く
        Cursor_y += CurrentFont.ypitch * ymul;  //漢字の高さに合うように調整
        return;
    }
    //暫定的に7セグフォントの:の横幅縮める
    if ((c == ':') && (CurrentFont.data == Font7segData)) fontXsize = 3;
    
    //フォントデータの先頭位置を計算　　1文字のデータは、X方向のバイト数 x Yサイズ
    pos = (uint16_t) Fstart + (c - CurrentFont.data[Foffset]) * ((fontXsize+7)/8) * fontYsize;
    //フォントデータの1ドットずつ描画
    for (jj = 0; jj < fontYsize; jj++) {
        yp = Cursor_y + jj * ymul;
        for (ii = 0; ii < fontXsize; ii++) {
            if ((ii % 8) == 0) {
                if (c == ' ') line = 0; //SmallFontの空白コードはASCIIと違うので、ここで対策
                else line = CurrentFont.data[pos++]; //8ドット毎にデータを取り込む
            }
            xp = Cursor_x + ii * xmul;
            if (line & 0x80) {
                // 1の描画
                if (xmul * ymul == 1) //拡大していない時
                    lcd_draw_pixel_at(xp, yp, Textcolor);
                else    //拡大されている時
                    display_fillRect(xp, yp, xmul, ymul, Textcolor);
            } else {
                // 0の描画
                if (TextbgColor != Textcolor) {
                    // Backカラーがfrontカラーと違う時は、その色を背景として塗る
                    if (xmul * ymul == 1)
                        lcd_draw_pixel_at(xp, yp, TextbgColor);
                    else
                        display_fillRect(xp, yp, xmul, ymul, TextbgColor);
                }
            }
            line <<= 1;
        }
    }

    //次の文字の位置を設定。改行処理も実施
    if ((c == ':') && (CurrentFont.data == Font7segData)) px = 4 * xmul + XpitchAdj;    //7seglikeの:の幅を狭くする　暫定対策
    else px = CurrentFont.xpitch * xmul + XpitchAdj;
    Cursor_x += px;
    if (Wrap && (Cursor_x + px > LCD_WIDTH)) {
        Cursor_x = 0;
        py = CurrentFont.ypitch * ymul + YpitchAdj;
        Cursor_y += py;
        if (Cursor_y > LCD_HEIGHT) Cursor_y = LCD_HEIGHT;
    }
    if (Cursor_x > LCD_WIDTH) Cursor_x = LCD_WIDTH;

}


/*
 * 設定されているカーソル位置、色、サイズをそのまま使う時はこれを呼び出す
 * 漢字交じり対応
 */
void display_puts(char *s) {
    uint16_t code;
    
    if (CurrentFont.data) {
        //内蔵フォント、漢字フォントの描画
        while (*s) {
            if (*s < 0x80) {
                display_putc(*s++);  //
            } else {
                code = *s++;
                code = code*256 + *s++;
                display_putK(code);  //
            }
        }
    } else {
        //カスタムフォントの描画
        displayCustom_puts(s);
    }

}


/**************************************************************************
   @brief   Draw a single character
    @param    x   Bottom left corner x coordinate
    @param    y   Bottom left corner y coordinate
    @param    c   The 8-bit font-indexed character (likely ascii)  shift jis can be accepted
    @param    color 16-bit 5-6-5 Color to draw chraracter with
    @param    bg 16-bit 5-6-5 Color to fill background with (if same as color, no background)
    @param    size  Font magnification level, 0x11 is 'original' size
                          縦横 x倍させるパラメータ
 カーソル位置、色、サイズは、別関数で意識的に設定したものが、ここで勝手に変わってしまわないように元に戻している
 **************************************************************************/
void display_drawChars(int16_t x, int16_t y, char *s, uint16_t color, uint16_t bg, uint8_t size) {
  int16_t prev_x     = Cursor_x,
          prev_y     = Cursor_y;
  uint16_t prev_color = Textcolor,
           prev_bg    = TextbgColor;
  uint8_t prev_magx  = TextMagX,
          prev_magy  = TextMagY;

  display_setCursor(x, y);
  display_setTextSize(size);
//  display_setTextPitch(0, 0);
  display_setTextColor(color, bg);
  
  display_puts(s);  //ここを変更
  
  display_setCursor(prev_x, prev_y);
  display_setTextColor(prev_color, prev_bg);
  TextMagX = prev_magx;
  TextMagY = prev_magy;
}


/**************************************************************************
   @brief    Draw a perfectly horizontal line
   @param    x   Left-most x coordinate
   @param    y   Left-most y coordinate
   @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
 **************************************************************************/
void drawHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    if ( (x < LCD_WIDTH) && (y < LCD_WIDTH) && w) {
        uint8_t hi = color >>8;
        uint8_t lo = color & 0xff;

        if ((x + w - 1) >= LCD_WIDTH) w = LCD_WIDTH  - x;
        
        if (!SPI1_Open(LCD8M_CONFIG)) return;
        //ちゃんと開けたら
        LCD_CS_SetLow();    //CSをLowに
        lcd_addset(x, y,  x + w-1, y);
        while (w--) {
            write_data(hi);
            write_data(lo);
        }
        LCD_CS_SetHigh();    //CSをHighに
        SPI1_Close();
    }
}

/**************************************************************************
   @brief    Draw a perfectly vertical line
   @param    x   Top-most x coordinate
   @param    y   Top-most y coordinate
   @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
 **************************************************************************/
void drawVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    if ( (x < LCD_WIDTH) && (y < LCD_WIDTH) && h) {  
        uint8_t hi = color >>8;
        uint8_t lo = color & 0xff;

        if ((y + h - 1) >= LCD_WIDTH) h = LCD_WIDTH - y;

        if (!SPI1_Open(LCD8M_CONFIG)) return;
        //ちゃんと開けたら
        LCD_CS_SetLow();    //CSをLowに
        lcd_addset(x, y, x, y+h-1);
        while (h--) {
            write_data(hi);
            write_data(lo);
        }
        LCD_CS_SetHigh();    //CSをHighに
        SPI1_Close();
    }
}

/**************************************************************************
   @brief    Write a line.  Bresenham's algorithm - thx wikpedia
   @param    x0  Start point x coordinate
   @param    y0  Start point y coordinate
   @param    x1  End point x coordinate
   @param    y1  End point y coordinate
   @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    bool steep = ABS((int16_t)(y1 - y0)) > ABS((int16_t)(x1 - x0));
    int16_t dx, dy, err, ystep;
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    dx = x1 - x0;
    dy = ABS((int16_t)(y1 - y0));

    err = dx / 2;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            lcd_draw_pixel_at(y0, x0, color);
        } else {
            lcd_draw_pixel_at(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

/**************************************************************************
   @brief    Draw a line
   @param    x0  Start point x coordinate
   @param    y0  Start point y coordinate
   @param    x1  End point x coordinate
   @param    y1  End point y coordinate
   @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {

    if (x0 == x1) {
        if (y0 > y1) _swap_int16_t(y0, y1);
        drawVLine(x0, y0, y1 - y0 + 1, color);
    } else if (y0 == y1) {
        if (x0 > x1) _swap_int16_t(x0, x1);
        drawHLine(x0, y0, x1 - x0 + 1, color);
    } else {
        writeLine(x0, y0, x1, y1, color);
    }
}


/**************************************************************************
   @brief   Draw a triangle with no fill color
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        int16_t x2, int16_t y2, uint16_t color) {
    display_drawLine(x0, y0, x1, y1, color);
    display_drawLine(x1, y1, x2, y2, color);
    display_drawLine(x2, y2, x0, y0, color);
}

/*******************************************************************************
 * This function will draw a filled rect from (x0,y0) to (x1,y1) with color
 */
void lcd_fill_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    uint16_t ii, jj;
    uint8_t hi = color >> 8, 
            lo = color & 0xff;

    if ( (y0 >= LCD_HEIGHT) || (x0 >= LCD_WIDTH) ) {
        return;
    }
    
    if ( x1 >= LCD_WIDTH ) x1 = LCD_WIDTH -1;
    if ( y1 >= LCD_HEIGHT ) y1 = LCD_HEIGHT -1;

    if (!SPI1_Open(LCD8M_CONFIG)) return;
    //ちゃんと開けたら
    LCD_CS_SetLow();    //CSをLowに
    lcd_addset(x0, y0, x1, y1); //これで描画範囲を制限する
    jj=0;
    while (jj++ < y1 - y0 +1) {
        ii=0; 
        while (ii++ < x1 - x0 +1) {
            write_data(hi); //制限された矩形領域をアドレス指定せず描画
            write_data(lo);
        }
    }
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();
} 
/**************************************************************************
   @brief    Fill a rectangle completely with one color. Update in subclasses if desired!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to fill with
 **************************************************************************/
void display_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    if (w && h) {   // Nonzero width and height?
        lcd_fill_rect(x, y, x+w-1, y+h-1, color);
    }
}


/**************************************************************************
   @brief   Draw a rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    drawHLine(x, y, w, color);
    drawHLine(x, y+h-1, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x+w-1, y, h, color);

}

/**************************************************************************
   @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of the circle we're doing
    @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            lcd_draw_pixel_at(x0 + x, y0 + y, color);
            lcd_draw_pixel_at(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            lcd_draw_pixel_at(x0 + x, y0 - y, color);
            lcd_draw_pixel_at(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            lcd_draw_pixel_at(x0 - y, y0 + x, color);
            lcd_draw_pixel_at(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            lcd_draw_pixel_at(x0 - y, y0 - x, color);
            lcd_draw_pixel_at(x0 - x, y0 - y, color);
        }
    }
}

/**************************************************************************
    @brief  Quarter-circle drawer with fill, used for circles and roundrects
    @param  x0       Center-point x coordinate
    @param  y0       Center-point y coordinate
    @param  r        Radius of circle
    @param  corners  Mask bits indicating which quarters we're doing
    @param  delta    Offset from center-point, used for round-rects
    @param  color    16-bit 5-6-5 Color to fill with
 **************************************************************************/
void display_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, 
        uint8_t corners, int16_t delta, uint16_t color) {
    
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;
    int16_t px    = x;
    int16_t py    = y;

    delta++; // Avoid some +1's in the loop

    while(x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        // These checks avoid double-drawing certain lines, important
        // for the SSD1306 library which has an INVERT drawing mode.
        if(x < (y + 1)) {
            if(corners & 1) drawVLine(x0+x, y0-y, 2*y+delta, color);
            if(corners & 2) drawVLine(x0-x, y0-y, 2*y+delta, color);
        }
        if(y != py) {
            if(corners & 1) drawVLine(x0+py, y0-px, 2*px+delta, color);
            if(corners & 2) drawVLine(x0-py, y0-px, 2*px+delta, color);
            py = y;
        }
        px = x;
    }
}

/**************************************************************************
   @brief    Draw a circle outline
   @param    x0   Center-point x coordinate
   @param    y0   Center-point y coordinate
   @param    r   Radius of circle
   @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    lcd_draw_pixel_at(x0  , y0+r, color);
    lcd_draw_pixel_at(x0  , y0-r, color);
    lcd_draw_pixel_at(x0+r, y0  , color);
    lcd_draw_pixel_at(x0-r, y0  , color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        lcd_draw_pixel_at(x0 + x, y0 + y, color);
        lcd_draw_pixel_at(x0 - x, y0 + y, color);
        lcd_draw_pixel_at(x0 + x, y0 - y, color);
        lcd_draw_pixel_at(x0 - x, y0 - y, color);
        lcd_draw_pixel_at(x0 + y, y0 + x, color);
        lcd_draw_pixel_at(x0 - y, y0 + x, color);
        lcd_draw_pixel_at(x0 + y, y0 - x, color);
        lcd_draw_pixel_at(x0 - y, y0 - x, color);
    }
}


/**************************************************************************
   @brief    Draw a circle with filled color
   @param    x0   Center-point x coordinate
   @param    y0   Center-point y coordinate
   @param    r   Radius of circle
   @param    color 16-bit 5-6-5 Color to fill with
 **************************************************************************/
void display_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    
    drawVLine(x0, y0-r, 2*r+1, color);
    display_fillCircleHelper(x0, y0, r, 3, 0, color);
}


/**************************************************************************
   @brief   Draw a rounded rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius) r = max_radius;
    
    drawHLine(x+r  , y    , w-2*r, color); // Top
    drawHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    drawVLine(x    , y+r  , h-2*r, color); // Left
    drawVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    display_drawCircleHelper(x+r    , y+r    , r, 1, color);
    display_drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    display_drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    display_drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
    
}

/**************************************************************************
   @brief   Draw a rounded rectangle with fill color
   @param    x   Top left corner x coordinate
   @param    y   Top left corner y coordinate
   @param    w   Width in pixels
   @param    h   Height in pixels
   @param    r   Radius of corner rounding
   @param    color 16-bit 5-6-5 Color to draw/fill with
 **************************************************************************/
void display_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis

    if (r > max_radius) r = max_radius;
    
    // smarter version
    lcd_fill_rect(x+r, y, x+r+w-2*r-1, y+h-1, color);
    // draw four corners
    display_fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    display_fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
    
}

//ここから下は、別のソース。　今後、良さそうなルーチンを取り込む予定として、コメント化
/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */
///////////////////////////////////////////////////////////////////////////////

//uint8_t  display_getRotation();
//uint16_t getCursorX(void);
//uint16_t getCursorY(void);
//uint16_t display_getWidth();
//uint16_t display_getHeight();
//
////************************* Non User Functions *************************//
//void writeLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
//uint8_t printNumber(uint32_t n, int8_t n_width, uint8_t _flags);
//void printFloat(float float_n, int8_t f_width, int8_t decimal, uint8_t _flags);
//void v_printf(const char *fmt, va_list arp);
////////////////////////////////////////////////////////////////////////////


/**************************************************************************
   @brief     Draw a triangle with color-fill
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to fill/draw with
 **************************************************************************/
void display_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
  int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last, dx01, dy01, dx02, dy02, dx12, dy12;
    //以下は、int32_tだったが、結果がおかしい時あり
    //以下のようにキャストしたら動いたようが、たぶん32bit変数不要と判断
    //        b   = (int16_t)x0 +(int16_t) sb / dy02;
    int16_t sa   = 0;
    int16_t sb   = 0;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    
    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        drawHLine(a, y0, b-a+1, color);
        return;
    }
    
    dx01 = x1 - x0;
    dy01 = y1 - y0;
    dx02 = x2 - x0;
    dy02 = y2 - y0;
    dx12 = x2 - x1;
    dy12 = y2 - y1;
    
    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it
    
    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
         a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
         b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if(a > b) _swap_int16_t(a,b);
        drawHLine(a, y, b-a+1, color);
    }
    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    
    // sprintfの書式に2つ以上の変数の設定を入れると、2つ目以降のデータがおかしく表示される場合あり
    //    sprintf(str, "%d, y=%d", sa, sb, y);
    
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
         a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
         b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if(a > b) _swap_int16_t(a,b);
        drawHLine(a, y, b-a+1, color);
    }

}

/*
 * (x,y)を起点に、xw,yw幅の箱の領域にdataを書き込んでいく
 * 16bitカラーのデータを描画
 */
void glcd_array(int16_t x, int16_t y, int16_t xw, int16_t yw, uint8_t *data)
{
    uint16_t a, b;
    uint16_t *dt;
    
    if (!SPI1_Open(LCD8M_CONFIG)) return;
    //ちゃんと開けたら
    dt = (uint16_t *)data;
    LCD_CS_SetLow();    //CSをLowに
    lcd_addset(x, y, x+xw-1, y+yw-1);
    b = 0;
    a = (uint16_t)(xw * yw);    //データワード数
    while (b < a) {
//        write_data(data[b+1]);   //bmpデータはリトルエンディアンだが、LCDは逆なので
//        write_data(data[b]);
//        b += 2;
        write_wdata(dt[b++]); //リトルエンディアン対応
    }
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();
}


/*******************************************************************************
 * This function fill the hole TFT with a user defined Color.
 * 画面クリア
 */
void lcd_fill(uint16_t color) {   
    uint16_t width, height;
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xff;

    if (!SPI1_Open(LCD8M_CONFIG)) return;
    //ちゃんと開けたら
    LCD_CS_SetLow();    //CSをLowに
    lcd_addset(0, 0, LCD_WIDTH -1, LCD_HEIGHT -1); //これで描画範囲を制限する
//    height = LCD_HEIGHT;
    for (height = 0; height < LCD_HEIGHT; height++) {
//    while (height--) {
//        width = LCD_WIDTH;
        for (width = 0; width < LCD_WIDTH; width++) {
//        while (width--) {
            write_data(hi);
            write_data(lo);
        }
    }
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();
}


/*
 * LCDディスプレイの初期化
 * エラーなら、EXIT_FAILURE
 * OKなら、EXIT_SUCCESS
 */
uint8_t lcd_init(void)
{
    //LCD Reset
    LCD_RESET_SetLow();
    __delay_ms(1);  //Reset pulse duration > 10us
    LCD_RESET_SetHigh();
    __delay_ms(15); // >5ms

    if (!SPI1_Open(LCD8M_CONFIG)) return EXIT_FAILURE;

    //ちゃんと開けたら
    LCD_CS_SetLow();    //CSをLowに
    // Power control A
    write_command(0xCB);
    write_data(0x39);
    write_data(0x2C);
    write_data(0x00);
    write_data(0x34);
    write_data(0x02);
    
    // Power control B
    write_command(0xCF);
    write_data(0x00);
    write_data(0xC1);   //A2h
    write_data(0x30);
    
    // Driver timing control A
    write_command(0xE8);
    write_data(0x85);   //84h
    write_data(0x00);   //11h
    write_data(0x78);   //7Ah
    
    // Driver timing control B
    write_command(0xEA);
    write_data(0x00);   //66h
    write_data(0x00);
    
    // Power on sequence control
    write_command(0xED);
    write_data(0x64);   //55h
    write_data(0x03);   //01h
    write_data(0x12);   //23h
    write_data(0x81);   //01h
    
    // Pump ratio control
    write_command(0xF7);
    write_data(0x20);   //10h
    
    // Power Control 1
    write_command(0xC0); //Power control
    write_data(0x23); //VRH[5:0]   0x23=4.60V
    
    // Power Control 2
    write_command(0xC1); //Power control
    write_data(0x10); //SAP[2:0];BT[3:0]
    
    // VCOM Control 1
    write_command(0xC5); //VCM control
    write_data(0x3E);   // 4.25V
    write_data(0x28);   // -1.5V
    
    // VCOM Control 2
    write_command(0xC7); //VCM control2
    write_data(0x86); // VCOM offset voltage: VMH ? 58 VML ? 58
    
    // Memory Access Control
    write_command(0x36); // Memory Access Control
    write_data(0xf8); //
    
    // COLMOD: Pixel Format Set
    write_command(0x3A);
    write_data(0x55);   //16 bits/pixel
    
    // Frame Rate Control (In Normal Mode/Full Colors)
    write_command(0xB1);
    write_data(0x00);
    write_data(0x18);   // 79Hz,  0x1B=70Hz (default)
    
    // Display Function Control
    write_command(0xB6); // Display Function Control
    write_data(0x08);
    write_data(0x82);
    write_data(0x27);
    
    // Enable 3G
    write_command(0xF2); // 3Gamma Function Disable
    write_data(0x00);   // Disable 3 gamma control
    
    // Gamma Set
    write_command(0x26); //Gamma curve selected
    write_data(0x01);   // Gammana curve 1 (G2.2))
    
    // Positive Gamma Correction
    write_command(0xE0); //Set Gamma
    write_data(0x0F);
    write_data(0x31);
    write_data(0x2B);
    write_data(0x0C);
    write_data(0x0E);
    write_data(0x08);
    write_data(0x4E);
    write_data(0xF1);
    write_data(0x37);
    write_data(0x07);
    write_data(0x10);
    write_data(0x03);
    write_data(0x0E);
    write_data(0x09);
    write_data(0x00);
    
    // Negative Gamma Correction
    write_command(0xE1); //Set Gamma
    write_data(0x00);
    write_data(0x0E);
    write_data(0x14);
    write_data(0x03);
    write_data(0x11);
    write_data(0x07);
    write_data(0x31);
    write_data(0xC1);
    write_data(0x48);
    write_data(0x08);
    write_data(0x0F);
    write_data(0x0C);
    write_data(0x31);
    write_data(0x36);
    write_data(0x0F);
    
    write_command(0x11); //Sleep Out
    __delay_ms(10); // It will be necessary to wait 5ms before sending next command
    write_command(0x29); //Display ON
    
    write_command(0x2c);
    
    LCD_CS_SetHigh();    //CSをHighに
    SPI1_Close();
    return EXIT_SUCCESS;

}


// end of library code.
