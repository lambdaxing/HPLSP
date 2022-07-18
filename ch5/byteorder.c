#include <stdio.h>

void byteorder()
{
    union
    {
        short value;
        char union_bytes[sizeof(short)];
    } test;
    test.value = 0x0102;
    if((test.union_bytes[0] == 1) && (test.union_bytes[1] == 2)) {
        // 1 是高位在低地址，2 是低位在高地址
        printf("big endian\n");
    }
    else if((test.union_bytes[0] == 2) && (test.union_bytes[1] == 1)) {
        // 2 是低位在低地址，1 是高位在高地址
        printf("little endian\n");
    }else {
        printf("unknow...\n");
    }
}

int main(void) 
{
    byteorder();
}