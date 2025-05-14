#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include "ej2.h"

#define H 20
#define W 12

static uint8_t inputA[H][4*W];
static uint8_t inputB[H][4*W];

uint8_t random_int(uint32_t max)
{
    return (uint8_t)(rand() % max);
}

void shuffle(uint32_t max)
{
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            uint8_t blueB = random_int(max);
            if (blueB == 0) blueB++;
            uint8_t blueA = random_int(max);
            if (blueA == 0) blueA++;
            inputA[i][4 * j] = blueA;
            inputA[i][4 * j + 1] = random_int(max);
            inputA[i][4 * j + 2] = random_int(blueB);
            inputA[i][4 * j + 3] = random_int(max);
            inputB[i][4 * j] = blueB;
            inputB[i][4 * j + 1] = random_int(max);
            inputB[i][4 * j + 2] = random_int(256-blueA);
            inputB[i][4 * j + 3] = random_int(max);
        }
    }
}

void shuffleCaso1(uint32_t max)
{
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            uint8_t blueB = random_int(max);
            if (blueB == 0) blueB++;
            uint8_t maxGreen = random_int(max);
            if (maxGreen == 0) maxGreen++;
            uint8_t blueA = random_int(max);
            if (blueA == 0) blueA++;
            inputA[i][4 * j] = blueA;
            inputA[i][4 * j + 1] = maxGreen;
            inputA[i][4 * j + 2] = random_int(blueB);
            inputA[i][4 * j + 3] = random_int(max);
            inputB[i][4 * j] = blueB;
            inputB[i][4 * j + 1] = random_int(maxGreen);
            inputB[i][4 * j + 2] = random_int(256-blueA);
            inputB[i][4 * j + 3] = random_int(max);
        }
    }
}

void shuffleCaso2(uint32_t max)
{
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            uint8_t blueB = random_int(max);
            if (blueB == 0) blueB++;
            uint8_t maxGreen = random_int(max);
            if (maxGreen == 0) maxGreen++;
            uint8_t blueA = random_int(max);
            if (blueA == 0) blueA++;
            inputA[i][4 * j] = blueA;
            inputA[i][4 * j + 1] = random_int(maxGreen);
            inputA[i][4 * j + 2] = random_int(blueB);
            inputA[i][4 * j + 3] = random_int(max);
            inputB[i][4 * j] = blueB;
            inputB[i][4 * j + 1] = maxGreen;
            inputB[i][4 * j + 2] = random_int(256-blueA);
            inputB[i][4 * j + 3] = random_int(max);
        }
    }
}

#define RUN(filename, action) pfile=fopen(filename,"a"); action; fclose(pfile);
#define NL(filename) pfile=fopen(filename,"a"); fprintf(pfile,"\n"); fclose(pfile);

char *filename_ej2 = "salida.propios.ej2.txt";
void test_ej2(char *filename);

int main()
{
    srand(0);
    remove(filename_ej2);
    test_ej2(filename_ej2);
    return 0;
}

void test_ej2(char* filename) {

    void (*func_combinar)(uint8_t*, uint8_t*, uint8_t*, uint32_t, uint32_t);
    if (USE_ASM_IMPL){
        func_combinar = combinarImagenes_asm;
    }else{
        func_combinar = combinarImagenes;
    }

    FILE* pfile;
    shuffleCaso1(256);
    RUN(filename, fprintf(pfile, "== Ejercicio 2 ==\n");) NL(filename)
    RUN(filename, fprintf(pfile, "== Caso 1 ==\n");) NL(filename)

    uint8_t *result = malloc(H*W*4);
    func_combinar((uint8_t*)inputA,(uint8_t*)inputB,(uint8_t*)result,W,H);

    for(int i=0;i<H;++i){
        for(int j=0;j<4*W;++j){
            RUN(filename, fprintf(pfile, "%3d ",result[i*4*W +j]);)
        }
        NL(filename)
    }

    shuffleCaso2(256);
    RUN(filename, fprintf(pfile, "\n== Caso 2 ==\n");) NL(filename)

    func_combinar((uint8_t*)inputA,(uint8_t*)inputB,(uint8_t*)result,W,H);

    for(int i=0;i<H;++i){
        for(int j=0;j<4*W;++j){
            RUN(filename, fprintf(pfile, "%3d ",result[i*W +j]);)
        }
        NL(filename)
    }

    shuffle(256);
    RUN(filename, fprintf(pfile, "\n== Random ==\n");) NL(filename)

    func_combinar((uint8_t*)inputA,(uint8_t*)inputB,(uint8_t*)result,W,H);

    for(int i=0;i<H;++i){
        for(int j=0;j<4*W;++j){
            RUN(filename, fprintf(pfile, "%3d ",result[i*W +j]);)
        }
        NL(filename)
    }

    free(result);
}
