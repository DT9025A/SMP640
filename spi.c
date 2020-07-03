#include "spi.h"

#ifdef HARDWARE_SPI
void SPI_Init() {
    SPDAT = 0;                  //initial SPI data
    SPSTAT = SPIF | WCOL;       //clear SPI status
#ifdef MASTER
    SPCTL = SPEN | MSTR | SPI_SPEED | CPx;        //master mode
#else
    SPCTL = SPEN;               //slave mode
#endif

}


unsigned char SPI_SendByte (unsigned char dat) {
#ifdef MASTERWITHSS
    SPISS = 0;                  //pull low slave SS
#endif
    SPDAT = dat;                //trigger SPI send
    while (! (SPSTAT & SPIF));  //wait send complete
    SPSTAT = SPIF | WCOL;       //clear SPI status
#ifdef MASTERWITHSS
    SPISS = 1;                  //push high slave SS
#endif
    return SPDAT;               //return received SPI data
}

#else

void SPI_Init() {
    SPIMOSI = 0;
    SPISCLK = 0;
}

unsigned char SPI_SendByte (unsigned char dat) {
    unsigned char temp, r = 0;
    SPISCLK = 0;
    for (temp = 0x80; temp != 0; temp >>= 1) {
        if (temp & dat == 0) {
            SPIMOSI = 0;
        } else {
            SPIMOSI = 1;
        }
        _nop_();
        _nop_();
        SPISCLK = 1;

        _nop_();
        _nop_();
        SPISCLK = 0;
        if (SPIMISO == 1) {
            r |= temp;
        } else {
            r &= ~temp;
        }
    }
    return r;
}

#endif
