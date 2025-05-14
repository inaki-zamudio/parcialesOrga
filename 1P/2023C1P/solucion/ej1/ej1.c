#include "ej1.h"

uint32_t cuantosTemplosClasicos_c(templo *temploArr, size_t temploArr_len){
    uint32_t res = 0;
    for (int i = 0; i < temploArr_len; i++) {
        templo temp = temploArr[i];
        uint8_t largo = temp.colum_largo;
        uint8_t corto = temp.colum_corto;
        if (largo == 2*corto + 1) {
            res++;
        }
    }
    return res;
}
  
templo* templosClasicos_c(templo *temploArr, size_t temploArr_len){
    uint32_t tam_res = cuantosTemplosClasicos_c(temploArr, temploArr_len);
    templo* nuevoarr = malloc(tam_res*sizeof(templo));
    uint32_t ult_indice = 0;
    for (uint32_t i = 0; i < temploArr_len; i++) {
        templo temp = temploArr[i];
        uint8_t largo = temp.colum_largo;
        uint8_t corto = temp.colum_corto;
        if (largo == 2*corto + 1) {
            nuevoarr[ult_indice] = temp;
            ult_indice++;
        }
    }
    return nuevoarr;
}
