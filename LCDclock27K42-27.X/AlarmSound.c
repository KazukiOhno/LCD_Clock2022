/* 
 * File:   AlarmSound.c
 * Author: K.Ohno
 *
 * 圧電スピーカー
 * TMR2で圧電スピーカを鳴らす
 * TMR0で音の長さを測定
 *  
 * ECCP2をPWMモードで使い、TMR2で周期設定(1us-256us)、RB3に出力
 * 　　64MHz/4/16 = 1MHz = 1us　TMR2の最小周期
 * 　　FOSC=64MHz, Prescaler=1:16
 * 　デフォルト250us=4kHzをビープ音として設定
 * TMR0: 鳴っている時間を制御　10ms毎に割り込みを掛け、発音時間、消音時間を制御
 */

#include "AlarmSound.h"


//アラームのパターンを定義　
//データ形式:　10msを単位に鳴っている時間と消音の時間をペアで定義
uint8_t SoundData[][10] = {
    //データの終わりは0で示す
    {10, 20, 10, 60,  0, },     //ピピッとならす最初の30秒用
    {10, 10, 10, 10, 10, 50,  0, },    //ピピピとならす30秒-1分用
    {10, 10, 10, 10,  0, },     //ピピピピと鳴らす
    { 7,  1,  0, },             //ピッと1回鳴らす
    { 7, 15, 20,  1,  0, },     //2回ピッピーと鳴らす
};
uint8_t SoundDataPos;   //どの音データを使っているか
uint8_t SoundCount;     //定義されてる長さをカウントダウン
uint8_t SoundNum;       //アラームの種類を特定、経過時間測定にも使用
uint16_t SoundTime;     //アラームが鳴っている時間を測定 (max 3000=30sまで)
uint8_t StopSoundFlag = 0;  //reentrantにならないようにフラグで停止を指示

/*
 * TMR0の割り込みのハンドラ 10ms毎にここにくる
 * 
 * SoundCountを減算して、0になったら、次のデータ=時間を設定
 * 発音か、消音かで、TMR2のオンオフする
 */
void SoundHandler() {

    //アラームなっている時間を計測し、30秒、60秒経過したらアラーム音を変更する
    SoundTime++;    //音を出してからの経過時間を計測　10ms単位
    if (SoundTime >= 24000) {
        //アラームが240s=5分以上経過時は、強制的に止める。最初の1分は別枠なので、ここは4分。
//        AlarmSoundOff();  //reentrantになるので、フラグに変更
        StopSoundFlag = 1;
        return;
    } else if (SoundTime >= 3000) {
        //前の処置から30s経過したら
        if (SoundNum < AlarmSound2) {
            SoundNum++; //アラーム音を次のレベルに進める
            SoundTime = 0;  // 鳴らしている時間を0に
            SoundDataPos = 0;
            SoundCount = SoundData[SoundNum][SoundDataPos++];
//            TMR2_Start(); //これはreentrantになるが、下記ならOK
            T2CONbits.TMR2ON = 1;
            return;
        }
    }

    //ここで音を作る
    SoundCount--;
    if (SoundCount == 0) {
        //SoundCountを10ms割り込み毎カウントダウンして、0になったら、次のデータを読み込む
        if (SoundData[SoundNum][SoundDataPos] == 0) {
            if (SoundNum <= AlarmSound2) {
                //リピートする場合
                SoundDataPos = 0;
            } else {
                //1回だけ鳴らす場合
//                AlarmSoundOff();  //reentrantになるので、フラグに変更
                StopSoundFlag = 1;
                return;
            }
        }
        SoundCount = SoundData[SoundNum][SoundDataPos++];
        
        //SoundDataPosが奇数の時サウンドOff、偶数の時はon。すでにインクリされていることに注意
        if (SoundDataPos % 2) {
//            TMR2_Start(); //これはreentrantになるが、下記ならOK
            T2CONbits.TMR2ON = 1;
        } else {
//            TMR2_Stop(); //これはreentrantになるが、下記ならOK
            T2CONbits.TMR2ON = 0;
        }
    }
}

/*
 * アラームサウンド開始
 * アラームを鳴らすための設定を実施
 */
void AlarmSoundOn(uint8_t sn) {
    //すでにOnの時は、何もしない->やめて、新たな音に変更
//    if (SoundDataPos != 0) return;
    //サウンドデータセット
    SoundNum = sn;  //サウンドの種類を設定
    SoundTime = 0;  //アラームが鳴っている時間
    SoundDataPos = 0;
    SoundCount = SoundData[SoundNum][SoundDataPos++]; //初期値設定

    TMR2_Start();
    TMR0_Reload();
    TMR0_StartTimer();    
}

void AlarmSoundOff(void) {
    TMR2_Stop();
    TMR0_StopTimer();

}

void AlarmInitialize(void) {
    AlarmSoundOff();
    TMR0_SetInterruptHandler(SoundHandler);
}
