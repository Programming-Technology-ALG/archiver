//
//  main.h
//  archivator
//
//  Created by Алексей Трутнев on 07.06.2020.
//  Copyright © 2020 Алексей Трутнев. All rights reserved.
//

#define CODE_SIZE 40

typedef struct node {
    unsigned char symb;
    unsigned char isSymb;
    unsigned int freq;
    unsigned char code[CODE_SIZE];
    int level;
    struct node * left, *right, *next;
}NODE;

NODE * MakeNodeFromFreq(unsigned char symb, unsigned int freq);
NODE * MakeNodeFromNode(const NODE * left, const NODE * right);
NODE * MakeListFromArray(int * freq);
NODE * MakeTreeFromList(NODE * head);
void Add2List(NODE ** head, NODE * pnew);
void MakeCodeInTree(NODE* root, int level, unsigned char* prev_code, unsigned char added);
void PrintTree(NODE* root);
void MakeTableFromTree(NODE * root, char (*code)[CODE_SIZE]);

typedef union bit2char {
    char symb;
    struct bit {
        unsigned b1 : 1;
        unsigned b2 : 1;
        unsigned b3 : 1;
        unsigned b4 : 1;
        unsigned b5 : 1;
        unsigned b6 : 1;
        unsigned b7 : 1;
        unsigned b8 : 1;
    }mbit;
}BIT2CHAR;

void PrintTableCodes(unsigned char(*code) [CODE_SIZE]);
void WriteFile(const char * to, char * res, int len, int tail);
void PackFile(const char * from, const char * to);
void UnpackFile(const char * from, const char * to);
long CalcFreqSymb(const char * from, int* freq);
char* MakeBitString(const char * from, int len, unsigned char(*code)[CODE_SIZE]);
char* FromBitToChar(const char * str, int * tail, int * len);
char* stringToBinary(char* s, int len, long bitstringLength);
