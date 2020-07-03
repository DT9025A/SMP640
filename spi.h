#ifndef _SPI_H_
#define _SPI_H_

#include <STC12C5A60S2.h>
#include <intrins.h>

#define SPIF        0x80        //SPSTAT.7
#define WCOL        0x40        //SPSTAT.6
#define SSIG        0x80        //SPCTL.7
#define SPEN        0x40        //SPCTL.6
#define DORD        0x20        //SPCTL.5
#define MSTR        0x10        //SPCTL.4
#define CPOL        0x08        //SPCTL.3
#define CPHA        0x04        //SPCTL.2
#define SPDHH       0x00        //CPU_CLK/4
#define SPDH        0x01        //CPU_CLK/16
#define SPDL        0x02        //CPU_CLK/64
#define SPDLL       0x03        //CPU_CLK/128

#define MASTER
#define SPI_SPEED    SPDLL
#define CPx          0			//CPOL|CPHA

sbit SPIMOSI = P1 ^ 5;
sbit SPIMISO = P1 ^ 6;
sbit SPISCLK = P1 ^ 7;

unsigned char SPI_SendByte (unsigned char dat);
void SPI_Init();
//void SPI_GenNCycle(unsigned char n,bit mosi);
//bit SPI_1Bit(bit b);

//Ä£ÄâSPIÎªCPOL=0 CPHA=0
#define HARDWARE_SPI

#ifdef MASTERWITHSS
#define MASTER
sbit SPISS      =   P1 ^ 3;     //SPI slave select, connect to slave' SS(P1.4) pin
#endif

#endif