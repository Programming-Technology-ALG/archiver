//
//  huffman.c
//  archivator
//
//  Created by Алексей Трутнев on 07.06.2020.
//  Copyright © 2020 Алексей Трутнев. All rights reserved.
//

#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CODE_SIZE 40
#define SIZE 256
#define BIT8 8

void WriteFile(const char * to, char * res, int len, int tail){
    FILE* fw = fopen(to, "wb");
    if (!fw)
        return;
    for (int i = 0; i < len; ++i)
        fprintf(fw, "%c", res[i]);
    fclose(fw);
}

void PackFile(const char * from, const char * to){
    int freq[SIZE] = { 0 };
    long len = CalcFreqSymb(from, freq);
    NODE * head = 0;
    head = MakeListFromArray(freq) ;
    NODE * root = 0;
    root = MakeTreeFromList(head) ;
    MakeCodeInTree(root->left, 0, (unsigned char *)"", '0');
    MakeCodeInTree(root->right, 0, (unsigned char *)"", '1');
    unsigned char codes[SIZE][CODE_SIZE] = { 0 };
    MakeTableFromTree(root, (char (*)[40])codes);
    PrintTableCodes (codes) ;
    char * bitstring = MakeBitString(from, (int)len, codes);
    int reslen = 0;
    int tail = 0;
    char * res = FromBitToChar(bitstring, &tail, &reslen);
    WriteFile(to, res, reslen, tail);
    free(bitstring);
    free(res);
}

void UnpackFile(const char * from, const char * to){
    FILE* fr = fopen("code.txt", "rb");
    if (!fr)
        return;
    fseek(fr, 0, SEEK_END);
    long length = ftell(fr);
    fseek(fr, 0, SEEK_SET);
    int tail;
    char tr;
    fscanf(fr,"%i%c", &tail, &tr);
    length = length - ftell(fr);
    unsigned char* str = (unsigned char*)malloc(length * sizeof(unsigned char));
    unsigned char codes[SIZE][CODE_SIZE] = { 0 };
    unsigned char* symb = (unsigned char*)malloc(SIZE * sizeof(char));
    int k = 0;
    while (length > 0) {
        fscanf(fr, "%c - %s%c", &symb[k], str, &tr);
        length -= strlen((char *)str)+5;
        strcpy((char *)codes[symb[k]], (char *)str);
        k++;
    }
    symb[k] = '\0';
    fclose(fr);
    free(str);
    fr = fopen(to, "rb");
    fseek(fr, 0, SEEK_END);
    length = ftell(fr);
    str = (unsigned char*)malloc(length * sizeof(unsigned char));
    fseek(fr, 0, SEEK_SET);
    for (int i = 0; i < length; i++)
        fscanf(fr,"%c", &str[i]);
    str[length] = '\0';
    fseek(fr, 0, SEEK_SET);
    char *binary = (char*)malloc(((length * BIT8)+1) * sizeof(char));
    k = 0;
    for (int i = 0; i < length; i++){
        for (int j = 0; j < 8; j++){
            if (str[i] & (1 << j))
                binary[i + k] = '1';
            else
                binary[i + k] = '0';
            k++;
        }
        k--;
    }
    fclose(fr);
    binary[length + k] = '\0';
    if (tail == 0) tail = 8;
    int n = (int)strlen(binary) - tail;
    k = 0;
    char* buf = (char*)malloc(CODE_SIZE * sizeof(char));
    strcpy(buf, "\0");
    int p = 0;
    FILE* fw = fopen(to, "wb");
    for (int i = 0; i < n; i++){
        buf[p] = binary[i];
        p++;
        buf[p] = '\0';
        for (int j = 0; j < strlen((char *)symb); j++){
            if (strcmp((char *)codes[symb[j]], buf) == 0){
                fprintf(fw,"%c", symb[j]);
                p = 0;
                strcpy(buf, "\0");
                break;
            }
        }
    }
    fclose(fw);
    free(buf);
    free(binary);
    free(symb);
    return;
}

long CalcFreqSymb(const char * from, int* freq){
    FILE* fr = fopen(from, "rb");
    if (!fr)
        return -1;
    fseek(fr, 0, SEEK_END);
    long length = ftell(fr);
    fseek(fr, 0, SEEK_SET);
    for (int i = 0; i < length; ++i)
        freq[(unsigned char)fgetc(fr)] ++;
    fclose(fr);
    return length;
}

NODE * MakeNodeFromFreq(unsigned char symb, unsigned int freq){
    NODE * res = (NODE*)malloc(sizeof(NODE));
    res->freq = freq;
    res->isSymb = 1;
    res->symb = symb;
    res->left = res->right = res->next = 0;
    return res;
}

NODE * MakeNodeFromNode(const NODE * left, const NODE * right){
    NODE * res = (NODE*)malloc(sizeof(NODE));
    res->freq = left->freq + right->freq;
    res->isSymb = 0;
    res->left = (struct node *)left;
    res->right = (struct node *)right;
    res->next = 0;
    return res;
}

void Add2List(NODE ** head, NODE * pnew){
    while (*head){
        if ((*head)->freq > pnew->freq)
            break;
        head = &((*head)->next);
    }
    pnew->next = *head;
    *head = pnew;
}

NODE * MakeListFromArray(int * freq){
    NODE * head = 0;
    for (int i = 0; i < SIZE; ++i){
        if(freq[i])
            Add2List(&head, MakeNodeFromFreq((unsigned char)i, (unsigned int)freq[i]));
    }
    return head;
}

NODE * MakeTreeFromList(NODE * head){
    while (head && head->next){
        NODE * left = head;
        NODE * right = head->next;
        Add2List(&(head->next->next), MakeNodeFromNode(left, right));
        head = head->next->next;
    }
    return head;
}

void MakeCodeInTree(NODE* root, int level, unsigned char* prev_code, unsigned char added){
    if (root){
        strcpy((char *)root->code, (const char *)prev_code);
        root->code[level] = added;
        root->code[level + 1] = 0;
        root->level = level;
        MakeCodeInTree(root->left, level + 1, root->code, '0');
        MakeCodeInTree(root->right, level + 1, root->code, '1');
    }
}

void PrintTree(NODE* root){
    if (root){
        PrintTree(root->left);
        printf("%c - %d (%d) [%s] %d\n", root->symb, root->freq, root->isSymb, root->code, root->level);
        PrintTree(root->right);
    }
}

void MakeTableFromTree(NODE * root, char (*code)[CODE_SIZE]){
    if (root){
        MakeTableFromTree(root->left, code);
        if (root->isSymb)
            strcpy(code[root->symb], (const char *)root->code);
        MakeTableFromTree(root->right, code);
    }
}

void PrintTableCodes(unsigned char(*code) [CODE_SIZE]){
    FILE* fw = fopen("codes.txt", "w");
    if (!fw)
        return;
    for (int i = 0; i < SIZE; ++i){
        if(strlen((const char *)code[i]))
            fprintf(fw, "%c - %s\n", (unsigned char)i, code[i]);
    }
    fclose(fw);
}

char* MakeBitString(const char * from, int len, unsigned char(*code)[CODE_SIZE]) {
    char * bitstr = (char*)calloc(len * BIT8, sizeof(char));
    FILE* fr = fopen(from, "rb");
    for (int i = 0; i < len; ++i){
        unsigned char c = fgetc(fr);
        strcat(bitstr, (const char *)code[c]);
    }
    fclose(fr);
    return bitstr;
}

char* FromBitToChar(const char * str, int * tail, int * len){
    int count = (int)strlen(str) / BIT8;
    *tail = strlen(str) % BIT8;
    *len = count + 1;
    BIT2CHAR symb;
    char * res = (char*)malloc((*len) * sizeof(char));
    for (int i = 0; i < *len; ++i){
        symb.mbit.b1 = str[i*BIT8 + 0];
        symb.mbit.b2 = str[i*BIT8 + 1];
        symb.mbit.b3 = str[i*BIT8 + 2];
        symb.mbit.b4 = str[i*BIT8 + 3];
        symb.mbit.b5 = str[i*BIT8 + 4];
        symb.mbit.b6 = str[i*BIT8 + 5];
        symb.mbit.b7 = str[i*BIT8 + 6];
        symb.mbit.b8 = str[i*BIT8 + 7];
        res[i] = symb.symb;
    }
    return res;
}
