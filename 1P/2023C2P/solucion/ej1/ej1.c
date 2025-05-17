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
    listElem_t* act = pList->first;
    while (act != NULL) {
        if (strcmp(act->data->pagador, usuario) == 0) {
            aprobados += act->data->aprobado;
        }
        act = act->next;
    }
    return aprobados;
}

uint8_t contar_pagos_rechazados(list_t* pList, char* usuario){
    uint8_t rechazados = 0;
    listElem_t* act = pList->first;
    while (act != NULL) {
        if (strcmp(act->data->pagador, usuario) == 0) {
            if (!act->data->aprobado) rechazados++;
        }
        act = act->next;
    }
    return rechazados;
}


pagoSplitted_t* split_pagos_usuario(list_t* pList, char* usuario){

}