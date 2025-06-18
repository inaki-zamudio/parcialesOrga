A) Para definir las nuevas tareas habria que modificar el archivo tasks.c agregandole nuevos tipos donde cada tipo va a tener una direccion fisica del inicio del codigo de cada tarea nueva. Y en tasks_init crear las 5 tareas cada una con su tipo correspondiente.

Tambien habria que crear una nueva funcion para inicializar una tarea pero que sea de nivel 0. La funcion mmu_init_shared_task_dir seria identica a init_task_dir pero no va a tener atributos de usuario si no que de kernel y en vez de que la pila sea una pagina de usuario va a ser de kernel. 

```c
tss_t tss_create_user_task(paddr_t code_start) {
  if(code_start == SHARED_TASK_CODE_START){
    uint32_t cr3 = mmu_init_shared_task_dir(code_start);
  }
  else{
    uint32_t cr3 = mmu_init_task_dir(code_start);
  }
  uint32_t cr3 = mmu_init_task_dir(code_start);
  vaddr_t stack = TASK_STACK_BASE - 1 ; 
  vaddr_t code_virt = TASK_CODE_VIRTUAL;
  // Como queremos tambien un stack de nivel 0 pedimos la pagina y lo agregamos al directorio con un map
  vaddr_t stack0 = mmu_next_free_kernel_page();
  //mmu_map_page(cr3, stack0, (paddr_t)stack0, MMU_P | MMU_W);
  vaddr_t esp0 = stack0 + (PAGE_SIZE - 1); 
  return (tss_t) {
    .cr3 = cr3,
    .esp = stack,
    .ebp = stack,
    .eip = code_virt,
    .cs = GDT_CODE_3_SEL,
    .ds = GDT_DATA_3_SEL,
    .es = GDT_DATA_3_SEL,
    .fs = GDT_DATA_3_SEL,
    .gs = GDT_DATA_3_SEL,
    .ss = GDT_DATA_3_SEL,
    .ss0 = GDT_DATA_0_SEL,
    .esp0 = esp0,
    .eflags = EFLAGS_IF,
  };
}
```

No voy a agregar a la tarea de nivel 0 al array de tareas del scheduler, pero si defino una variable global dentro del scheduler que se va a llamar `selector_sexta_tarea` que va a contener el selector de la sexta tarea. 

Tambien vamos a definir una variable global en `sched.c` que se va a llamar `cantidad_tareas_esperando_shared` que si es 5 en `sched_next_task` vamos a siempre devolver el selector de la tarea shared porque todas las demas van a estar pausadas. 

```c
int8_t current_task = 0;
int8_t cantidad_tareas_esperando_shared = 0;
```

Para que la interrupcion sea agregada a la idt y pueda ser llamada por las demas tareas la vamos a definir. 

Para agregar la syscall nueva vamos a definir la interrupcion nueva en `idt_init()`.

Como las syscalls suelen definirse a partir del numero de interrupcion 80 vamos a definirla como la numero 80.

Para que pueda ser llamads desde las tareas va teber que ser definida como `IDT_ENTRY3` en `int_init`. Tambien en `isr.h` hay que agregar:

```h
void _isr80();
```

B) En la syscall lo que se hace es pausar la tarea actual, incrementar en uno la cantidad de tareas que estan pausadas esperando y saltar a la siguiente tarea. Si la cantidad de tareas que se pausan esperando a la sexta tarea son 5 entonces `sched_next_task` va a devolver el selector de la sexta tarea y se va a ejecutar esa hasta que termine. Esa tarea se va a encargar de hacer lo que tenga que calcular y luego despausar todas las tareas.
En cambio si todavia hay tareas que se puedan ejecutar se van a ejecutar las demas tareas. 

Cada tarea va a guardar su eax actual en el array de tareas del scheduler para que la sexta lo pueda usar. Para eso agrego un atributo al struct `sched_entry_t`: 

```c
typedef struct {
  int16_t selector;
  task_state_t state;
  uint32_t eax_para_la_sexta_tarea
} sched_entry_t;
```

Y cambio la implementacion de `sched_add_task` para que inicialize el atributo nuevo en 0.

```c
int8_t sched_add_task(uint16_t selector) {
  kassert(selector != 0, "No se puede agregar el selector nulo");

  // Se busca el primer slot libre para agregar la tarea
  for (int8_t i = 0; i < MAX_TASKS; i++) {
    if (sched_tasks[i].state == TASK_SLOT_FREE) {
      sched_tasks[i] = (sched_entry_t) {
        .selector = selector,
	    .state = TASK_PAUSED,
        .eax_para_la_sexta_tarea = 0;
      };
      return i;
    }
  }
  kassert(false, "No task slots available");
}
```

La implementacion de la syscall en `isr.asm` va a ser: 

```asm
global _isr32

_isr32:
    pushad
    push eax 
    call modificar_eax_del_array
    add esp, 4
    push DWORD [current_task]
    call sched_disable_task
    add esp, 4
    inc word [cantidad_tareas_esperando_shared]
    ; Saltamos a la siguiente tarea
    call sched_next_task
    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset] 

    .fin:
    popad 
    iret
```

Y el codigo de `modificar_eax_del_array` va a ser dentro de `sched.c`: 

```c
void moficar_eax_del_array(uint32_t valor){
    sched_tasks[current_task].eax_para_la_sexta_tarea = valor;
}
```

D) Ahora cambia la implementacion de `sched_next_task` para que en caso de que las 5 tareas esten experando a la sexta directamente se devuelva el selector de la sexta. 

```c
uint16_t sched_next_task(void) {
  // Si ninguna tarea esta viva porque estan todas esperando a la shared devolvemos el selector de la shared para que se salte alli
  if(cantidad_tareas_esperando_shared == 5){
    return selector_sexta_tarea;
  }
  // Buscamos la próxima tarea viva (comenzando en la actual)
  int8_t i;
  for (i = (current_task + 1); (i % MAX_TASKS) != current_task; i++) {
    // Si esta tarea está disponible la ejecutamos
    if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE) {
      break;
    }
  }

  // Ajustamos i para que esté entre 0 y MAX_TASKS-1
  i = i % MAX_TASKS;

  // Si la tarea que encontramos es ejecutable entonces vamos a correrla.
  if (sched_tasks[i].state == TASK_RUNNABLE) {
    current_task = i;
    return sched_tasks[i].selector;
  }

  // En el peor de los casos no hay ninguna tarea viva. Usemos la idle como
  // selector.
  return GDT_IDX_TASK_IDLE << 3;
}
```
C) La tarea que procesa los resultados aparte de procesar los resultados se va a encargar de al terminar despausar a todas las demas tareas para que puedan seguir ejecutandose. Para acceder al eax de las tareas y usar sus datos hay varias opciones, como que para obtenerlos recorra las pilas de sus tss y agarre los eaxs de ahi, o definir un area de memoria compartida donde cada tarea pone ahi su eax para que la sexta tarea acceda el dato o lo que "implemente" que es que directamente se guarda como un atributo nuevo en `sched_tasks` que me parece que es lo mas facil. 

Entonces la sexta tarea va a usar los datos de las tareas, despausarlas y por ultimo modificar el dato que haga que ella corra todo el tiempo. Lo que va a terminar devolviendo es la suma de todos los eaxs. 

Despues para guardar el resultado en el eax de las demas tareas o se puede hacer durante el codigo de la tarea que va a meterse en la pila de cada tarea y modificar el eax de la pila. Otra opcion seria delegar eso a una syscall que va a llamar la sexta tarea al terminar. 

Otra opcion es que se haga en la syscall luego de saltar a la siguiente tarea, porque cuando volves es porque ya se calculo lo que tenga que calcular la sexta. Pero para eso la sexta tarea deberia poner la informacion en algun lado, como una pagina de memoria donde levantar el dato. 

Lo implemento directamente en C.

```c
void sexta_tarea(){
    uint32_t res = 0;
    // Asumo que en el array estan solamente las 5 tareas originales y que no hay mas tareas
    for (int8_t i = 0; i < MAX_TASKS; i++){
        res = res + sched_tasks[i].eax_para_la_sexta_tarea;
    }
    // Ahora ponemos el resultado en el eax de la tarea y la despausamos
     for (int8_t i = 0; i < MAX_TASKS; i++){
        modificar_eax(sched_tasks[i]->selector, res);
        sched_enable_task(i);
    }
    cantidad_tareas_esperando_shared = 0; // Con esto hacemos que se puedan ejecutar de nuevo las tareas normalmente
}

void modificar_eax(uint16_t segsel, uint32_t dato){
  uint16_t idx = segsel >> 3;
  tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
  uint32_t* stack_pointer = tss_pointer->esp;

  stack_pointer[7] = dato;
}
```
