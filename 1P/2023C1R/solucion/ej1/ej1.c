#include "ej1.h"

uint32_t* acumuladoPorCliente(uint8_t cantidadDePagos, pago_t* arr_pagos){
    uint32_t* res = calloc(10, sizeof(uint32_t));
    for (uint8_t i = 0; i < cantidadDePagos; i++) {
        pago_t pago = arr_pagos[i];
        if (pago.aprobado) {
            uint8_t idx = pago.cliente;
            res[idx] += pago.monto;
        }
    }
    return res;
}

uint8_t en_blacklist(char* comercio, char** lista_comercios, uint8_t n){
    for (uint8_t i = 0; i < n; i++) {
        if (strcmp(lista_comercios[i], comercio) == 0) {
            return 1;
        }
    }
    return 0;
}

pago_t** blacklistComercios(uint8_t cantidad_pagos, pago_t* arr_pagos, char** arr_comercios, uint8_t size_comercios){
    uint8_t cant_pagos_blacklisteados = 0;
    for (uint8_t i = 0; i < cantidad_pagos; i++) {
        if (en_blacklist(arr_pagos[i].comercio, arr_comercios, size_comercios)) {
            cant_pagos_blacklisteados++;
        }
    }

    pago_t** res = malloc(cant_pagos_blacklisteados*sizeof(pago_t*));
    uint8_t idx = 0;
    for (uint8_t i = 0; i < cantidad_pagos; i++) {
        if (en_blacklist(arr_pagos[i].comercio, arr_comercios, size_comercios)) {
            res[idx++] = &arr_pagos[i];
        }
    }
    return res;
}


