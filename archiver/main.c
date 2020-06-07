//
//  main.c
//  archivator
//
//  Created by Алексей Трутнев on 07.06.2020.
//  Copyright © 2020 Алексей Трутнев. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"
#include <string.h>

int main(int argc, const char * argv[]) {
    if(argc != 3){
        puts("not enough input arguments (you need 3 txt files)\n");
        return 1;
    }
    unsigned char codes[SIZE][CODE_SIZE] = { 0 };
    long bitstringLength, fileLength;
    PackFile(argv[1], argv[2], &bitstringLength, &fileLength);
    printf("File packed to: %s\n", argv[2]);
    UnpackFile(argv[2], argv[3], bitstringLength, fileLength);
    printf("File unpacked to: %s\n", argv[3]);
    return 0;
}
