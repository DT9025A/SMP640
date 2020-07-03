#include "sensor.h"

void Init_ADC() {
    //设置P1
    P1ASF = 1 << ADC_R_PIN;
    AUXR1 |= 1 << 2;	 	//1:ADC_RES<<8+ADC_RESL
    ADC_RES = 0;
    ADC_RESL = 0;
    ADC_CONTR = 0x80 | ADC_SPEED << 5;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

unsigned int Read_ADC () {
    //开始转换
    ADC_CONTR |= 0x08;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    //等待转换完成
    while ((ADC_CONTR & 0x10) == 0);
    //清标志位
    ADC_CONTR &= 0xEF;
    return (ADC_RES << 8) | (ADC_RESL);
}

float Read_Temperature () {
    float r = Read_Res();
    return (1.0 / (log (r / CONST_R) / CONST_B + 1.0 / CONST_25ADDK) - CONST_K);
}

float Read_Res() {
    float RProp = 0.0;
    unsigned char i;

    for (i = 0; i < CONST_SAMPLE_N; i++) {
        RProp += (float)Read_ADC ();
    }

    RProp = RProp / 4;
    if (RProp > CONST_ADC_MAX)
        RProp = CONST_ADC_MAX;
    RProp = CONST_R * RProp / ((CONST_ADC_MAX + CONST_ADJ) - RProp);
    if (RProp > 1000)
        RProp = 0;

    return RProp;
}

unsigned char IsPaperExist() {
    return (PHOTO_PIN == 1 ? 0 : 1);
}