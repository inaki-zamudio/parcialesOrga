### Características del sistema que se pide:
- Las tareas tendrán acceso directo a la memoria de video
- La memoria física de video está dentro del rango 0xB8000-0xB9FFF
- Sólo una tarea a la vez con la memoria físíca de video mapeada
- El resto de las tareas tendrán una pantalla _dummy_ en el rango 0x1E000-0x1FFFF
- La memoria de video se mapeará siempre en el rango virtual 0x08004000-0x08005FFF
- Soltar la tecla TAB (scancode al apretar: 0x0F. Al soltar: 0x8F) cambia la tarea actualmente en pantalla, de forma cíclica.

a)

| r/d |     Mem. virtual     |   Mem. Física  |
|-----|----------------------|----------------|
|  r  | x08004000-0x08005FFF | xB8000-0xB9FFF |
|  d  | x08004000-0x08005FFF | x1E000-0x1FFFF |



Es decir, necesitamos mapear una página de memoria virtual a, o bien la página que comienza en la dirección física 0xB8000, correspondiente a la pantalla real, o bien a la dirección físíca 0x1E000, que corresponde a la pantalla dummy.

b)

Para cada tarea que se cree en el sistema, vamos a arrancar mapeándole, en la dirección virtual 0x08004000, la dirección 0x1E000 que corresponde a la tarea dummy. Para eso, modificamos, en mmu.c, la función `mmu_init_task_dir`:

```c
#define TASK_SCREEN_VIRTUAL 0x08004000
#define TASK_SCREEN_DUMMY 0x1E000
#define TASK_SCREEN_REAL 0xB8000

paddr_t mmu_init_task_dir(paddr_t phy_start) {
  // tenemos que hacer la estructura de paginación para nuestra tarea
  // en primer lugar tenemos que reservar una página para el directorio de páginas
  pd_entry_t* directorio = (pd_entry_t*)mmu_next_free_kernel_page();
  zero_page((paddr_t)directorio);

  // hacer un identity maping con el kernel
  directorio[0] = kpd[0];

  // luego una página para la tabla de páginas a la que:
  // luego direccionaremos a dos páginas para el código (como sólo lectura)
  for(uint32_t i=0; i<TASK_CODE_PAGES; i++){
    paddr_t phy = phy_start + i*PAGE_SIZE;
    vaddr_t virt = TASK_CODE_VIRTUAL + i*PAGE_SIZE;

    mmu_map_page((uint32_t) directorio, virt, phy, (MMU_U | MMU_P));
  }
  // una página para el stack (como lectura/escritura)
  paddr_t phy_stack = mmu_next_free_user_page();
  mmu_map_page((uint32_t) directorio, TASK_STACK_BASE - PAGE_SIZE, phy_stack, (MMU_P | MMU_U | MMU_W));
  
  // y una de memoria compartida
  mmu_map_page((uint32_t) directorio, TASK_STACK_BASE, SHARED_MEM, (MMU_P | MMU_U ));
 
  // dos páginas para la memoria de video
  mmu_map_page((uint32_t) directorio, TASK_SCREEN_VIRTUAL, TASK_SCREEN_DUMMY, MMU_P | MMU_W | MMU_R);
  mmu_map_page((uint32_t) directorio, TASK_SCREEN_VIRTUAL + PAGE_SIZE, TASK_SCREEN_REAL + PAGE_SIZE, MMU_P | MMU_W | MMU_R);

  return (paddr_t) directorio;
}
```

c)

Habría que guardar, en una variable global, el id de la tarea que tenga la memoria de video. Podríamos, en sched.c, agregar:

```c
int8_t current_task_with_video = -1;
```

que comienza en -1 porque, como mapeamos la pantalla dummy a todas las tareas, ninguna arranca teniendo la real.

d)

Cuando se ejecute el handler del teclado, si el scancode de la tecla que se tocó es 0x8F, entonces tenemos que hacer que la tarea `(current_task_with_video+1) % MAX_TASKS` obtenga acceso a la memoria real de video. Esto se hará desmapeando la memoria real de video de la tarea `current_task_with_video`, mapearle la memoria dummy, y luego desmapearle la dummy a la `current_task_with_video+1 % MAX_TASKS` y mapearle la real:

Entonces, habrá que modificar el handler de interrupción del teclado de la siguiente forma:

```nasm
...
%define TAB_RELEASE 0x8F
...
global _isr33

; COMPLETAR: Implementar la rutina
_isr33:
    pushad

    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1

    ; 2. Leemos la tecla desde el teclado y la procesamos
    in al, 0x60
    cmp al, TAB_RELEASE
    jne .fin

    ; si llegamos acá, estamos en el caso en el que se soltó TAB:
    push ax ; hay que guardarse el eax porque es no volátil
    call alternar_video
    pop al

    .fin:
    push eax

    call tasks_input_process

    add esp, 4

    popad

    iret
```

```c
void alternar_video() {
  int8_t task_id_prox_video_real = (current_task_with_video + 1) % MAX_TASKS;
  if (current_task_with_video != -1) {
    // alguna tiene la memoria de video, hay que desmapearsela
    uint16_t ctwv_segsel = sched_tasks[current_task_with_video].segsel;
    uint32_t cr3_current_task_with_video = obtenerCR3(ctwv_segsel);
    mmu_unmap_page(cr3_current_task_with_video, TASK_SCREEN_VIRTUAL);
    mmu_unmap_page(cr3_current_task_with_video, TASK_SCREEN_VIRTUAL + PAGE_SIZE);
  } // hay que mapearle la pantalla a la nueva current_task_with_video
  uint32_t cr3_nueva_tarea_con_video = obtenerCR3(sched_tasks[current_task_with_video++].selector); // incremento el valor de current_task_with_video 
  mmu_map_page(cr3_nueva_tarea_con_video, TASK_SCREEN_VIRTUAL, TASK_SCREEN_REAL, MMU_P | MMU_U | MMU_W);
  mmu_map_page(cr3_nueva_tarea_con_video, TASK_SCREEN_VIRTUAL + PAGE_SIZE, TASK_SCREEN_REAL + PAGE_SIZE, MMU_P | MMU_U | MMU_W);
}

uint32_t obtenerCR3(uint16_t segsel) {
  tss_t* tss = obtenerTSS(segsel);
  return tss->cr3;
}

tss_t* obtenerTSS(uint16_t segsel) {
  return gdb[selector >> 3].base;
}
```
