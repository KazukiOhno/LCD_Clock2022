/* 
 * File:   AlarmSound.c
 * Author: K.Ohno
 *
 * ���d�X�s�[�J�[
 * TMR2�ň��d�X�s�[�J��炷
 * TMR0�ŉ��̒����𑪒�
 *  
 * ECCP2��PWM���[�h�Ŏg���ATMR2�Ŏ����ݒ�(1us-256us)�ARB3�ɏo��
 * �@�@64MHz/4/16 = 1MHz = 1us�@TMR2�̍ŏ�����
 * �@�@FOSC=64MHz, Prescaler=1:16
 * �@�f�t�H���g250us=4kHz���r�[�v���Ƃ��Đݒ�
 * TMR0: ���Ă��鎞�Ԃ𐧌�@10ms���Ɋ��荞�݂��|���A�������ԁA�������Ԃ𐧌�
 */

#include "AlarmSound.h"


//�A���[���̃p�^�[�����`�@
//�f�[�^�`��:�@10ms��P�ʂɖ��Ă��鎞�ԂƏ����̎��Ԃ��y�A�Œ�`
uint8_t SoundData[][10] = {
    //�f�[�^�̏I����0�Ŏ���
    {10, 20, 10, 60,  0, },     //�s�s�b�ƂȂ炷�ŏ���30�b�p
    {10, 10, 10, 10, 10, 50,  0, },    //�s�s�s�ƂȂ炷30�b-1���p
    {10, 10, 10, 10,  0, },     //�s�s�s�s�Ɩ炷
    { 7,  1,  0, },             //�s�b��1��炷
    { 7, 15, 20,  1,  0, },     //2��s�b�s�[�Ɩ炷
};
uint8_t SoundDataPos;   //�ǂ̉��f�[�^���g���Ă��邩
uint8_t SoundCount;     //��`����Ă钷�����J�E���g�_�E��
uint8_t SoundNum;       //�A���[���̎�ނ����A�o�ߎ��ԑ���ɂ��g�p
uint16_t SoundTime;     //�A���[�������Ă��鎞�Ԃ𑪒� (max 3000=30s�܂�)
uint8_t StopSoundFlag = 0;  //reentrant�ɂȂ�Ȃ��悤�Ƀt���O�Œ�~���w��

/*
 * TMR0�̊��荞�݂̃n���h�� 10ms���ɂ����ɂ���
 * 
 * SoundCount�����Z���āA0�ɂȂ�����A���̃f�[�^=���Ԃ�ݒ�
 * �������A�������ŁATMR2�̃I���I�t����
 */
void SoundHandler() {

    //�A���[���Ȃ��Ă��鎞�Ԃ��v�����A30�b�A60�b�o�߂�����A���[������ύX����
    SoundTime++;    //�����o���Ă���̌o�ߎ��Ԃ��v���@10ms�P��
    if (SoundTime >= 24000) {
        //�A���[����240s=5���ȏ�o�ߎ��́A�����I�Ɏ~�߂�B�ŏ���1���͕ʘg�Ȃ̂ŁA������4���B
//        AlarmSoundOff();  //reentrant�ɂȂ�̂ŁA�t���O�ɕύX
        StopSoundFlag = 1;
        return;
    } else if (SoundTime >= 3000) {
        //�O�̏��u����30s�o�߂�����
        if (SoundNum < AlarmSound2) {
            SoundNum++; //�A���[���������̃��x���ɐi�߂�
            SoundTime = 0;  // �炵�Ă��鎞�Ԃ�0��
            SoundDataPos = 0;
            SoundCount = SoundData[SoundNum][SoundDataPos++];
//            TMR2_Start(); //�����reentrant�ɂȂ邪�A���L�Ȃ�OK
            T2CONbits.TMR2ON = 1;
            return;
        }
    }

    //�����ŉ������
    SoundCount--;
    if (SoundCount == 0) {
        //SoundCount��10ms���荞�ݖ��J�E���g�_�E�����āA0�ɂȂ�����A���̃f�[�^��ǂݍ���
        if (SoundData[SoundNum][SoundDataPos] == 0) {
            if (SoundNum <= AlarmSound2) {
                //���s�[�g����ꍇ
                SoundDataPos = 0;
            } else {
                //1�񂾂��炷�ꍇ
//                AlarmSoundOff();  //reentrant�ɂȂ�̂ŁA�t���O�ɕύX
                StopSoundFlag = 1;
                return;
            }
        }
        SoundCount = SoundData[SoundNum][SoundDataPos++];
        
        //SoundDataPos����̎��T�E���hOff�A�����̎���on�B���łɃC���N������Ă��邱�Ƃɒ���
        if (SoundDataPos % 2) {
//            TMR2_Start(); //�����reentrant�ɂȂ邪�A���L�Ȃ�OK
            T2CONbits.TMR2ON = 1;
        } else {
//            TMR2_Stop(); //�����reentrant�ɂȂ邪�A���L�Ȃ�OK
            T2CONbits.TMR2ON = 0;
        }
    }
}

/*
 * �A���[���T�E���h�J�n
 * �A���[����炷���߂̐ݒ�����{
 */
void AlarmSoundOn(uint8_t sn) {
    //���ł�On�̎��́A�������Ȃ�->��߂āA�V���ȉ��ɕύX
//    if (SoundDataPos != 0) return;
    //�T�E���h�f�[�^�Z�b�g
    SoundNum = sn;  //�T�E���h�̎�ނ�ݒ�
    SoundTime = 0;  //�A���[�������Ă��鎞��
    SoundDataPos = 0;
    SoundCount = SoundData[SoundNum][SoundDataPos++]; //�����l�ݒ�

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
