/* 
 * File:   font.h
 * Author: K.Ohno
 *
 * Created on December 19, 2020, 5:19 PM
 */

#ifndef FONT_H
#define	FONT_H

#ifdef	__cplusplus
extern "C" {
#endif

    
#include "mcc_generated_files/mcc.h"

/*
 * フォントデータ構造: 先頭8バイトに情報
 * 表示する時、大きさ、ピッチ、色は指定できるようにしたら、表現として十分か
 *  デフォルトで持つフォントデータは、NormalFontDataとSmallFontDataの2つ
 *  漢字フォントは、SDカード使える場合に対応
 *  さらにカスタムフォントという形で、ユーザがSDカードに格納したファイルデータを使える
 * 
 * LCD320x240color.cで定義されている変数
 * Cursor_x
 * Cursor_y
 * TextMagX, TextMagY
 * Textcolor
 * TextbgColor
 * Wrap
 * 
 */
    
//ノーマルフォント
// 1バイトキャラクタデータ　8x8ドット
// ASCIIコード：$20-$7Fまで定義、曜日、℃、F、■□を追加
// Font.xlsxにて絵のデータから、ソースコードに貼り付けられるコードを生成している
// 最新フォントは、Font.xlsxの新フォントのシート
/*
 * フォントデータ:横方向8ドット単位に、左側MSB、右側LSBで、1バイトのデータ
 * 　　　　　　　縦方向は、配列の要素として持っている
 * 　この説明だとわかりにくいが、例えば、Kのフォントデータだと、
 * 0xC6, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00, という8バイトになっていて、
 * 以下のような絵となる。
 * ■■□□□■■□
 * ■■□□■■□□
 * ■■□■■□□□
 * ■■■■□□□□
 * ■■□■■□□□
 * ■■□□■■□□
 * ■■□□□■■□
 * □□□□□□□□
 * 
 * この方式では、Y方向のデータは柔軟に持てるが、プロポーショナルフォントの対応はできない
 * X方向のデータは1バイトとして定義しているので、8ドットmaxで、固定幅となる
 * 
 */
// uは、unsignを意味する
#define FSunday 0x16u    //フォントデータ起点の文字コード ASCIIでないので
#define FdegC   0x1Du
#define FdegF   0x1Eu
#define Ffbox   0x1Fu    //Filled Box
#define Fbox    0x7Fu    //Box
    
//配列内の位置
#define Fxsize  0u  //X方向のドット数
#define Fysize  1u  //Y方向のドット数
#define Fxpitch 2u  //X方向のピッチ
#define Fypitch 3u  //Y方向のピッチ
#define Foffset 4u  //データ先頭の文字コード(起点となる)
#define Fstart  8u  //フォントデータの開始位置、裏返すと、8バイト分のヘッダがあるということ
    
//フォントの処理をやりやすくするために、フォントデータの構造を定義しておく
//フォントサイズ、ピッチ、最初の文字コードは、フォントデータ先頭に入っている
typedef struct {
    uint8_t xsize;     //フォントのXサイズ
    uint8_t ysize;     //フォントのYサイズ
    uint8_t xpitch;    //フォントのXピッチ
    uint8_t ypitch;    //フォントのYピッチ
    uint8_t mag;       //半角フォントと比較したサイズ比　8x8ドットで、漢字が16x16だったら、x12
    const uint8_t *data;    //フォントデータ配列へのポインタ
    uint8_t fontcode; //フォント番号
} FontStruct;

//ノーマルフォント　8x8ドットフォント　表示領域は7x7にしているので、8ドットピッチで表示可
extern const uint8_t NormalFontData[];

//カレンダー用　6x8ドットフォント   表示領域は5ドットx7ドット　　数字のみの定義
extern const uint8_t SmallFontData[];

//7セグもどき　5x7ドットフォント   表示領域も5ドットx7ドット　　部品表示に必要なものを定義
//FdegC?:まで
extern const uint8_t Font7segData[];

//現在のフォント
extern FontStruct CurrentFont;

//カスタムフォントは、ヘッダ情報だけ持つ。先頭の4つだけ有効
//Fxsize, Fysize, Fxpitch, Fypitch, Foffset

// 漢字フォント名、ファイルの定義
enum KanjiFontCode {
    Misaki = 1,     //8x8ドットフォント
    SJIS16,         //16x16ドットフォント
    SJIS24,         //24x24ドットフォント
};

// フォントのファイル名の定義。これはプログラムで固定
extern char KanjiFontFile[][13];

extern FontStruct CurrentKanjiFont;

//歯車アイコン 32x32ドット
extern const uint8_t GearIcon[];


#ifdef	__cplusplus
}
#endif

#endif	/* FONT_H */
