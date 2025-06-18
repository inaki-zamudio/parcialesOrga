A) El esquema de memoria seria el mismo que el del taller pero del lado de la memoria virtual habria que agregar debajo de la memoria compartida dos paginas de memoria de nivel 3 read-write. Estas paginas o van a estar mapeadas dentro del kernel en la direccion fisica 0xB8000 o mapeados tambien dentro del kernel a la direccion fisica 0x1E000.

B) Como el enunciado no aclara si al principio la memoria de video arranca mapeada a la primer tarea o se mapea por primera vez cuando se suelta el tab voy a asumir que se mapea por primera vez la primera vez que se suelta el tab.

Lo que habria que cambiar es en `mmu_init_task_dir` tambien mapear las dos paginas virtuales a la pantalla dummy. Para eso definimos las constantes en el archivo `defines.h`:

```h
#define VADDR_VIDEO_MEMORY_START 0x8004000
#define DUMMY_VIDEO_MEMORY_START 0x1E000
#define REAL_VIDEO_MEMORY_START 0xB8000
```

Y en la funcion `mmu_init_task_dir` vamos a agregar que se mapeen para todas las tareas las dos paginas de video dummy. 

```c
paddr_t mmu_init_task_dir(paddr_t phy_start) {
  // Inicializamos la tarea y hacemos que apunte al kernel
  paddr_t pd = mmu_next_free_kernel_page();
  copy_page(pd, (paddr_t)kpd);
  //copiamos la page table del kernel
  paddr_t pt = mmu_next_free_kernel_page();
  copy_page(pt, (paddr_t)kpt);

  // Inicializamos las dos paginas para el codigo nivel 3 read only
  mmu_map_page(pd, TASK_CODE_VIRTUAL, phy_start, READ_ONLY_USER_ATTR);

  mmu_map_page(pd,TASK_CODE_VIRTUAL + PAGE_SIZE, phy_start + PAGE_SIZE, READ_ONLY_USER_ATTR); // Preguntar si no hay que incrementar phy start

  // Definimos la pila en el area libre de kernel
  // El area libre de tareas va a ser para usuario, asi que lo hacemos dentro de una pagina nueva
 
  mmu_map_page(pd, TASK_STACK_BASE - PAGE_SIZE, mmu_next_free_user_page(), READ_WRITE_USER_ATTR);
  // Como la pila va de abajo hacia arriba le restamos para que arranque en 8k - 1 en vez de 4k
  // Definimos el compartido en kernel
  //phy_shared_init
  mmu_map_page(pd, TASK_SHARED_PAGE, SHARED, READ_ONLY_USER_ATTR); // Probar que el physical de shared sea un free user page

  // Mapeo las dos paginas de video dummy
  mmu_map_page(pd, VADDR_VIDEO_MEMORY_START, DUMMY_VIDEO_MEMORY_START, MMU_P | MMU_U | MMU_W);
  mmu_map_page(pd, VADDR_VIDEO_MEMORY_START + PAGE_SIZE, DUMMY_VIDEO_MEMORY_START + PAGE_SIZE, MMU_P | MMU_U | MMU_W);
  return (paddr_t) pd;
}
```

Si la consigna dijera que la que empieza con el video real es la primer tarea le agregaria un parametro a esta funcion que sea el id de la tarea y si el id que recibe la funcion es 1 mapea la de verdad y si no la dummy. 

C) Para indicar que tarea tiene el video voy a agregar una variable global en el archivo `sched.c` que se va a llamar `current_task_with_video` que va a arrancar en -1 representando que no hay ninguna tarea que tiene mapeada el video real. 

Asi que agrego en `sched.c`: 

```c
int current_task_with_video = -1;
```

D) Para realizar el cambio de pantalla al soltar la tecla tab lo que hay que hacer es modificar la rutina de atencion del teclado para que si el scancode de la interrupcion es 0x8F se haga el cambio de mapeo de la pantalla.

Por lo que primero modifico la rutina de atencion del teclado: 

```asm
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33

_isr33:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    ; 2. Leemos la tecla desde el teclado y la procesamos
    in al, 0x60
    cmp al, 0x8F
    jne .fin
    ; Si estamos aca es porque hay que cambiar cual es la proxima tarea que se le va a dar video
    push al ; Guardo el valor del scancode
    call alternar_video
    pop al
    .fin:
    push al
    call tasks_input_process
    add esp, 1
    popad
    iret
```

Y en `sched.c` defino la funcion `alternar_video`:

```c
void alternar_video(){
    // Si existe una tarea que ahora tiene el video se lo desmapeamos
    if(current_task_with_video != -1){
        paddr_t cr3_tarea_con_video_vieja = obtener_cr3(sched_tasks[current_task_with_video].selector);
        // Le desmapeamos la direccion real
        mmu_unmap_page(cr3_tarea_con_video_vieja, VADDR_VIDEO_MEMORY_START);
        mmu_unmap_page(cr3_tarea_con_video_vieja, VADDR_VIDEO_MEMORY_START + PAGE_SIZE);
        // Le mapeamos la direccion dummy
        mmu_map_page(cr3_tarea_con_video_vieja, VADDR_VIDEO_MEMORY_START, DUMMY_VIDEO_MEMORY_START, MMU_P | MMU_U | MMU_W);
        mmu_map_page(cr3_tarea_con_video_vieja, VADDR_VIDEO_MEMORY_START + PAGE_SIZE, DUMMY_VIDEO_MEMORY_START + PAGE_SIZE, MMU_P | MMU_U | MMU_W);
    }
    else{ // No hay ninguna tarea con video actualmente asi que le damos el valor de 0 para que despues se le asigne el 1 en el loop
        current_task_with_video = 0;
    }
    int8_t i;
    for (i = (current_task + 1); (i % MAX_TASKS) != current_task; i++) {
        // Si esta tarea está disponible la ejecutamos
        if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE) {
            break;
        }
    }
    // Ajustamos i para que esté entre 0 y MAX_TASKS-1
    i = i % MAX_TASKS;
    // Asumo que siempre la tarea que encontramos es ejecutable, asi que le doy el video a esa tarea
    paddr_t cr3_nueva_tarea_con_video = obtener_cr3(sched_tasks[i].selector);
    mmu_map_page(cr3_nueva_tarea_con_video, VADDR_VIDEO_MEMORY_START, REAL_VIDEO_MEMORY_START, MMU_P | MMU_U | MMU_W);
    mmu_map_page(cr3_nueva_tarea_con_video, VADDR_VIDEO_MEMORY_START + PAGE_SIZE, REAL_VIDEO_MEMORY_START + PAGE_SIZE, MMU_P | MMU_U | MMU_W);
}
```

La funcion auxiliar `obtener_cr3` en `sched.c` importando la gdt en ese archivo: 

```c
pd_entry_t* obtener_cr3(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    return tss_pointer.cr3;
}
```

E) Para que la tarea sepa si es su turno de usar la pantalla se me ocurren dos posibilidades. La primera seria que todas las tareas tengan mapeada una pagina donde el primer byte indica que tarea tiene acceso a la pantalla. Todas las tareas van a tener mapeada una direccion fisica y virtual en comun en solo lectura nivel 3 y el kernel la va a tener mapeada como lectura escritura nivel 0, por lo que el kernel va a ser el unico que va a poder escribir ahi informandoles a las tareas quien es la que esta con acceso a video real en este momento. 

La otra opcion seria implementar una syscall que devuelva que tarea es la que tiene actualmente el video, entonces cuando una tarea quiere saber quien tiene video llama, por ejemplo, a la interrupcion numero 80 y en la interrupcion accedemos a la variable `current_task_with_video` y la devolvemos. Para que las tareas puedan usar esta syscall tiene que ser una interrupcion de nivel 3.

F) Para que la tarea no tenga que redibujar la pantalla lo que se podria hacer es que el kernel "administre" dos paginas para cada tarea que va a contener una copia de lo que dibujaron durante su tiempo con la pantalla. Asi que cuando una tarea termine de usar la pantalla el kernel se va a ocupar de copiar todos los datos en pantalla a la copia y cuando le vuelva a tocar su tiempo en pantalla lo que va a hacer antes de mapearla va a ser restaurar lo que habia en la pantalla vieja pegando esa copia en la pantalla verdadera. 
