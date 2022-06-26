/*
 * File:   TouchXT2046.c
 * Author: K.Ohno
 *
 *Color LCD 320x240に搭載のタッチインタフェース用汎用ルーチン
 * 
 * LCDボード上の5ピンインタフェース
 * T_IRQ: タッチした時、Lowになる
 * T_DO: PICのSDIに接続
 * T_DIN: PICのSDOに接続
 * T_CS: タッチのチップセレクト　LowでActive
 * T_CLK: SPIインタフェース用クロック
 * 
 * SPIインタフェースをLCDと共有する。CSで通信する対象を選択
 * 最大対応速度=2Mbps-> 1.5MHz
 * 
 */

#include "TouchXT2046.h" 

#ifdef DEBUG
#include "LCD320x240color.h"
#endif

/*
 * メインプログラムで以下のような設定を行う
 * 
 * 最新版では、IOC割り込みを使わず、ボタン処理と同様にタイマで状態を確認して、処置するようにした
 * 
 */

/*
 * タッチ座標の生の値を取得する
 * 12ビットの座標を取得
 */
void GetTouchRawXY(uint16_t *tx, uint16_t *ty) {
    uint8_t data[2];

    //S A2 A1 A0 MODE SER/DFR PD1 PD0
    //1 1  0  1
    //  YP=+IN X-Position を測定(横向きにしているので、Y軸方向)
    SPI1_ExchangeByte(0b11010011);
    SPI1_ReadBlock(data, 2);    //12ビット、2バイト相当を読み込む
    *ty = ((uint16_t)data[0] << 4) + (data[1] >> 4);
    
    // Y-Position (今の使い方ではX座標)を取得
    SPI1_ExchangeByte(0b10010011);
    SPI1_ReadBlock(data, 2);    //12ビット、2バイト相当を読み込む
    *tx = ((uint16_t)data[0] << 4) + (data[1] >> 4);
}


/*
 * タッチしたX,Y座標を返す
 * エラーの時、-1を返す
 * oversamplingを15から8にしてみたが、あまり結果は良くなかった
 * 
 * タッチした座標情報はこのルーチンでのみ取得
 */
int8_t GetTouchLocation(uint16_t *tx, uint16_t *ty) {
    int8_t jj;
    uint8_t oversampling = 15;   //タッチ座標を15回取り込み平均化する
    uint16_t sample_sumX;   //12bitデータなので、16回(=4bit)まで加算できる
    uint16_t sample_sumY;
    uint16_t samplesX[16], samplesY[16];    //oversamplingの数に合わせること
    uint8_t num;
    uint16_t avg, diff, overrange;
    char str[100];

    if (T_IRQ_GetValue() != 0) return -1;

    if (SPI1_Open(Touch2M_CONFIG) == false) return -1;
    //2MHz　デフォルト設定8MHzにしているので、タッチ用に低速にする
    T_CS_SetLow(); //タッチCSをLowに
    
    // oversampling回測定し、平均取る
    sample_sumX = 0;
    sample_sumY = 0;
    for (jj = 0; jj < oversampling; jj++) {
        GetTouchRawXY(&samplesX[jj], &samplesY[jj]);
        sample_sumX += samplesX[jj];
        sample_sumY += samplesY[jj];
    }
    SPI1_ExchangeByte(0b10000000);  // PowerDownにして、IRQをEnabledにする
    
    T_CS_SetHigh(); //CSをHighに
    SPI1_Close();
    
    avg = sample_sumX / oversampling;
    num = oversampling;
    overrange = avg / 10;    //平均値の10%
    for (jj = 0; jj < oversampling; jj++) {
        if (avg > samplesX[jj]) diff = avg - samplesX[jj];
        else diff = samplesX[jj] - avg;
        if (diff > overrange) {
            //平均より外れているデータを削除
            sample_sumX -= samplesX[jj];
            num--;
        }
    }
    if (num > 0) *tx = sample_sumX / num;
    else return -1;  //有効データないので、エラー
    
    avg = sample_sumY / oversampling;
    num = oversampling;
    overrange = avg / 10;    //平均値の10%　　当初80にしていた
    for (jj = 0; jj < oversampling; jj++) {
        if (avg > samplesY[jj]) diff = avg - samplesY[jj];
        else diff = samplesY[jj] - avg;
        if (diff > overrange) {
            //平均より大きく外れているデータを削除
            sample_sumY -= samplesY[jj];
            num--;
        }
    }
    if (num > 0) *ty = sample_sumY / num;
    else return -1;  //有効データないので、エラー

#ifdef DEBUG
    sprintf(str, "T1=(%d, %d)", *tx, *ty);
    display_drawChars(0, 160, str, WHITE, TextbgColor, FontMagx11);
#endif

    return 0;    
}
