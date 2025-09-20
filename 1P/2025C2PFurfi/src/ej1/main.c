#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define WITH_ABI_MESSAGE
#include "../../test_utils/test-utils.h"
#include "../ejs.h"

int main(int argc, char* argv[]) {
    printf("Tuit: \n");
    printf("mensaje[140]: %zu bytes\n", offsetof(struct tuit_s, mensaje));
    printf("favoritos: %zu bytes\n", offsetof(struct tuit_s, favoritos));
    printf("retuits: %zu bytes\n", offsetof(struct tuit_s, retuits));
    printf("id_autor: %zu bytes\n", offsetof(struct tuit_s, id_autor));
}
