//
//  main.c
//  archivator
//
//  Created by Алексей Трутнев on 07.06.2020.
//  Copyright © 2020 Алексей Трутнев. All rights reserved.
//

#include <stdio.h>
#include "huffman.h"

int main(int argc, char * argv[]) {
    if(argc != 3){
        puts("Not enough input arguments (you need 3 txt files)\n");
        return 1;
    }
    PackFile(argv[1], argv[2]);
    printf("File packed to: %s\n", argv[2]);
    UnpackFile(argv[2], argv[3]);
    printf("File unpacked to: %s\n", argv[3]);
    return 0;
}
