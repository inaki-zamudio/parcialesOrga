;########### LISTA DE FUNCIONES IMPORTADAS
extern strPrint
extern strDelete
extern strClone

extern malloc
extern free
extern fprintf

global countVowels
global createLettersQuantityArray
global getMaxVowels
;########### SECCION DE DATOS
section .data

;########### SECCION DE TEXTO (PROGRAMA)
section .text


%define LQ_SIZE 24

%define LETTER_A 0x61
%define LETTER_E 0x65
%define LETTER_I 0x69
%define LETTER_O 0x6F
%define LETTER_U 0x75
; rdi, rsi, rdx, rcx, r8, and r9.

; uint8_t countVowels(char* word);
countVowels:
    ; COMPLETAR


;letters_quantity_t* createLettersQuantityArray(uint8_t size);
createLettersQuantityArray:
    ; COMPLETAR

; char* getMaxVowels(letters_quantity_t* wq_array, uint8_t array_size);
getMaxVowels:
    ; COMPLETAR
