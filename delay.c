#include "delay.h"

void Delay500ms() {	//@11.0592MHz
    unsigned char i, j, k;

    i = 22;
    j = 3;
    k = 227;
    do {
        do {
            while (--k);
        } while (--j);
    } while (--i);
}

void Delay2ms() {	//@11.0592MHz
    unsigned char i, j;

    i = 33;
    j = 66;
    do {
        while (--j);
    } while (--i);
}

void Delay100ms() {	//@11.0592MHz
    unsigned char i, j, k;

    i = 5;
    j = 52;
    k = 195;
    do {
        do {
            while (--k);
        } while (--j);
    } while (--i);
}
