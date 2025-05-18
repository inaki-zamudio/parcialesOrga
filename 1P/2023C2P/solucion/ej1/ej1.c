#include "ej1.h"

list_t* listNew(){
  list_t* l = (list_t*) malloc(sizeof(list_t));
  l->first=NULL;
  l->last=NULL;
  return l;
}

void listAddLast(list_t* pList, pago_t* data){
    listElem_t* new_elem= (listElem_t*) malloc(sizeof(listElem_t));
    new_elem->data=data;
    new_elem->next=NULL;
    new_elem->prev=NULL;
    if(pList->first==NULL){
        pList->first=new_elem;
        pList->last=new_elem;
    } else {
        pList->last->next=new_elem;
        new_elem->prev=pList->last;
        pList->last=new_elem;
    }
}


void listDelete(list_t* pList){
    listElem_t* actual= (pList->first);
    listElem_t* next;
    while(actual != NULL){
        next=actual->next;
        free(actual);
        actual=next;
    }
    free(pList);
}

uint8_t contar_pagos_aprobados(list_t* pList, char* usuario){
    uint8_t aprobados = 0;
    if (pList != NULL) {
        listElem_t* act = pList->first;
        while (act != NULL) {
            if (strcmp(act->data->cobrador, usuario) == 0 && act->data->aprobado == 1) {
                aprobados++;
            }
            act = act->next;
        }
    }
    return aprobados;
}

uint8_t contar_pagos_rechazados(list_t* pList, char* usuario){
    uint8_t rechazados = 0;
    if (pList != NULL) {
        listElem_t* act = pList->first;
        while (act != NULL) {
            if (strcmp(act->data->cobrador, usuario) == 0 && act->data->aprobado == 0) {
                rechazados++;
            }
            act = act->next;
        }
    }
    return rechazados;
}


pagoSplitted_t* split_pagos_usuario(list_t* pList, char* usuario){
    pagoSplitted_t* res = malloc(sizeof(pagoSplitted_t));
    res->cant_aprobados = contar_pagos_aprobados(pList, usuario);
    res->cant_rechazados = contar_pagos_rechazados(pList, usuario);
    res->aprobados = malloc(8*res->cant_aprobados);
    res->rechazados = malloc(8*res->cant_rechazados);

    if (pList != NULL) {
        listElem_t* act = pList->first;
        uint8_t ultapr = 0;
        uint8_t ultrech = 0;
        while (act != NULL) {
            if (strcmp(act->data->cobrador, usuario) == 0) {
                if (act->data->aprobado == 1) {
                    res->aprobados[ultapr] = act;
                    ultapr++;
                }
                if (act->data->aprobado == 0) {
                    res->rechazados[ultrech] = act;
                    ultrech++;
                }
            }
            act = act->next;
        }
    }
    return res;
}