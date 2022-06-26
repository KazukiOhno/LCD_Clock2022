/* 
 * File:   customize.h
 * Author: ohno
 * 
 * Created on 2021/12/
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CUSTOMIZE_H
#define	CUSTOMIZE_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */


#include "mcc_generated_files/mcc.h"
#include "main.h"

extern const char BtnNameOK[];      // = "OK";
extern const char BtnNameCancel[];  // = "Cancel";
extern const char CloseText[];      // = "Close";
extern const char CustomSettingText[][6];   // = { "Date", "Time", "Alarm", "Temp", "Humid", "Calen", "Gear", };

extern ObjectRsc TempObjData[OBJECTNUM];   //カスタマイズする情報を保持
extern uint16_t SelectedNo; //現在選択している項目番号 (先頭=0(項目数-1))
extern uint8_t TargetObj;   //どのオブジェクトを変更しようとしているか　　DateObjとかTimeObjとか
extern uint8_t TargetItem; //どの項目を対象としているか　　FontとかSizeとか

/*
 * カスタムフォントのフォルダ名(8文字max)
 * カスタマイズした後、保存必要
 */
#define NumberOfUserFont   6 //カスタムフォント数
extern char CustomFontDir[NumberOfUserFont+1][9];   // +1は、暫定フォント用

extern char NameFontNormal[];   // = "Normal";
extern char NameFontSmall[];    // = "Small";
extern char NameFont7like[];    // = "like7";   //SDカードに頼らないよう、内部7セグを用意
extern char NameFont7seg[];     // = "7segsl";           //カスタムフォントと同様の処理をするが、デフォルトで設定
extern char NameFont7segSmall[];   // = "7segslS";     //カスタムフォントと同様の処理をするが、デフォルトで設定

enum ItemNum {
    ItemFont,
    ItemFontSize,
    ItemXpitch,
    ItemColor,
    ItemXY,
    ItemFormat,
};

/* 
 * SDカードに格納されているフォルダ名をフォント名(最大8文字)
 * 順序は、FontCodeと合わせている。先頭2つは、内蔵フォントなので、フォルダ不要
 * カスタマイズ画面でのフォント名の表示にも使うので、内蔵フォントも入れている
 */
extern char *FontFolder[];

//X方向ピッチ調整の選択肢  -2 - 8の11種
#define MAXXpitchNum    11
extern const char *FontXpitchText[];
extern uint8_t FormatNum[OBJECTNUM];
extern const char *FormatTextList[];


void drawPopupForm(uint8_t num, char **listText, char *title, int16_t popx);
void TouchProcedurePopup(void);

void CopyCustomDataToTemp(void);
void SetCustomData(void);
void CustomizingEventHandler(void);


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* CUSTOMIZE_H */
