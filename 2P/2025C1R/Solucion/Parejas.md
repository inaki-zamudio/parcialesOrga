Las tareas forman pareja compartiendo la dirección de memoria virtual `0xC0C00000`, que debería apuntar en una pareja a la misma página física. 
Tenemos que crear las 3 nuevas syscalls. Para eso definiríamos en `idt.c` 3 nuevas entradas para las 3 syscalls, las 3 de nivel 3. 

```
IDT_ENTRY3(80);
IDT_ENTRY3(81);
IDT_ENTRY3(82);
```
Esto deja inicializada la IDT con las 3 nuevas syscalls. Ahora, deberíamos definir en `isr.h` las 3 nuevas declaraciones de rutina de atención para cada syscall. 

¿Qué estructuras nuevas o modificación de las actuales deberíamos incluir para implementar las rutinas de atención? Como no deberíamos modificar el `task_state_t` porque querríamos que este estado fuera independiente del estado de la pareja de la tarea, lo podemos agregar en `sched_entry_t`. Además, agregamos un bit para indicar si es lider o no. Agregamos también el id de la pareja.
```C
typedef struct {
	int16_t selector;
	task_state_t state;
	partner_status_t partner_status;
	uint8_t leader : 1;
	int8_t id_pareja; 
} sched_entry_t;
```
Definimos el nuevo enum:
```C
typedef enum{
	TASK_MARRIED;
	TASK_SINGLE;
	TASK_SEARCHING_PARTNER;
}partner_status_t;
```
Modificamos, además, los estados de la tarea para denotar el estado en el que está buscando una pareja, bloqueada y cuando está pausada por abandono.
```C
typedef enum {
TASK_SLOT_FREE,
TASK_RUNNABLE,
TASK_PAUSED,
TASK_PAUSED_BY_PARTNER_SEARCH; //Nuevo estado
TASK_PAUSED_BY_ABANDON;
} task_state_t;
```

El estado `TASK_MARRIED` indica el estado en el que una tarea tiene una pareja ya conformada. El estado `TASK_SINGLE` denota el estado de una tarea cuando no está buscando pareja ni está en una. El último estado indica cuando la tarea creó una pareja, se hizo líder, pero aún ninguna tarea se le unió a la pareja.

Definamos la syscall `void crear_pareja()`. Primero implementemos la rutina de atención. 
```NASM
global _isr80

_isr80:
	pushad
	
	
	call crear_pareja
	call estoy_pausado_por_search
	
	cmp ax, 1
	jne .fin
	.nextTask:
		call sched_next_task
		str cx
		cmp ax, cx
		je .fin
		mov word [sched_task_selector], ax
		jmp far [sched_task_offset]
	.fin:
	popad
	iret
```
Ahora podemos definir la syscall en `C`.
```C
void crear_pareja(){
	sched_entry_t tarea = sched_tasks[current_task];
	if(tarea.partner_status == TASK_MARRIED){
		return;
	}
	else if(tarea.partner_status == TASK_SINGLE){
		tarea.partner_status = TASK_SEARCHING_PARTNER;
		tarea.state = TASK_PAUSED_BY_PARTNER_SEARCH; //Pasa a buscar pareja.
		tarea.leader = 1; //Lo seteamos como lider
	}
	return;
}
```
No hacemos el if chequeando si el estado es `TASK_SEARCHING_PARTNER` porque la tarea no debería poder llamar a la syscall si está en ese estado, porque debería estar pausada.
Tenemos que, una vez que salimos del call en la rutina de atención de ASM, chequear si quedamos pausados por búsqueda de pareja, y, en caso afirmativo, deberíamos saltear automáticamente a la próxima tarea con un `jmp far`. Si no, deberíamos seguir con la ejecución de la tarea sin cambios. Definamos `estoy_pausado_por_search`
```C
uint8_t estoy_pausado_por_search(){
	sched_entry_t tarea = sched_tasks[current_task];
	if(tarea.state == TASK_PAUSED_BY_PARTNER_SEARCH){
		return 1;
	}
	return 0;
}
```
## juntarse_con(id_tarea)
Vamos a definir la syscall `juntarse_con(id_tarea)`. Vamos a definir la rutina de atención. El `id_tarea` vamos a asumir que me lo pasan por el registro `EDI`
```NASM
global _isr81

_isr81:
	pushad
	;lo pusheamos a la pila para hacer el call en C
	push EDI
	call juntarse_con
	add esp, 4
	;Nos queda el resultado en EAX, ¿lo vamos a usar?
	mov [ESP+OFFSET_EAX], EAX; Cuando hago el popad, popea el valor del EAX actualizado (guardado en la pila de nivel 0) al registro.
	;Esto permite que la llamada a la syscall devuelva el valor correcto.  
	popad
	iret
```
Implementamos la función en C. Asumimos que una tarea en en estado MARRIED no puede llamar a juntarse_con. Además, si la tarea está pausada por ser lider y buscar pareja no debería poder llamar a esta syscall. Por tanto, las únicas tareas que deberían llamar a esta syscall son las que tienen su estado actual en TASK_SINGLE.
```C
int juntarse_con(int id_tarea){
	sched_entry_t tareaAJuntarse = sched_tasks[id_tarea];
	if(tareaAJuntarse.partner_status == TASK_MARRIED){
		return 1;
	}
	else if(tareaAJuntarse.partner_status == TASK_SINGLE){
		return 1;
	}
	else{
		//La volvemos a poner en ejecución a la tarea pausada
		tareaAJuntarse.state = TASK_RUNNABLE;
		//La ponemos como 'en pareja'
		tareaAJuntarse.partner_status = TASK_MARRIED;
		//La pareja de la tarea a juntarse somos nosotros.
		tareaAJuntarse.id_pareja = current_task;
		//Nos ponemos a nosotros como 'en pareja'
		sched_tasks[current_task].partner_status = TASK_MARRIED;
		//Nuestra pareja es tareaAJuntarse. 
		sched_tasks[current_task].id_pareja = id_tarea;
		return 0;
	}
}
```

Vamos a definir ahora la rutina de atención para la siguiente syscall, abandonar_pareja. 
```NASM
global _isr82

_isr82:
	pushad
	call abandonar_pareja
	call estoy_pausada_por_abandono
	cmp ax, 1
	jne .fin
	
	.nextTask:
		call sched_next_task
		str cx
		cmp ax, cx
		je .fin
		mov word [sched_task_selector], ax
		jmp far [sched_task_offset]
	.fin:
	popad
	iret
```
Vamos a definir la función en C
```C
void abandonar_pareja(){
	sched_entry_t tareaActual = sched_tasks[current_task];
	if(tareaActual.partner_status == TASK_SINGLE){
		return;
	}
	else if(tareaActual.partner_status == TASK_MARRIED){
		if(es_lider(current_task)){
			tareaActual.state = TASK_PAUSED_BY_ABANDON; //Podemos usar este estado
			romper_pareja(); //Limpiamos la pareja. 
			//No modificamos otra cosa porque lo único importante cuando un lider abandona es que no pueda seguirse ejecutando.
		}
		else{
			//Si abandona una no-lider me pongo como soltera
			tareaActual.partner_status = TASK_SINGLE;
			//Si mi pareja era líder, y estaba por abandonar, le desmapeo los 4MB
			if(sched_task[tareaActual.id_pareja].state = TASK_PAUSED_BY_ABANDON){
				for(int i = 0; i < 1024; i++){
					mmu_unmap_page(obtener_cr3(tareaActual.id_pareja), 0xC0C00000+i*PAGE_SIZE);
				}
				//Y a mi pareja la seteo como runnable
				sched_task[tareaActual.id_pareja].state = TASK_RUNNABLE;
			}
			for(int i = 0; i < 1024; i++){
					mmu_unmap_page(rcr3(), 0xC0C00000+i*PAGE_SIZE);
				}
			romper_pareja();
		}
	}
	return;
}
```
Definamos `obtener_cr3`
```C
uint32_t obtener_cr3(uint16_t selector){
    obtener_TSS(selector);
    return tss_task->cr3;
}
```
Definamos `obtener_TSS`
```C
tss_t* obtener_TSS(uint16_t selector){
    uint16_t idx = selector >> 3;
    return gdt[idx].base;
}
```
Definamos `estoy_pausada_por_abandono`
```C
uint16_t estoy_pausada_por_abandono(){
	return (sched_tasks[current_task].state == TASK_PAUSED_BY_ABANDON);
} 
```

Tenemos que modificar el page_fault para poder mapear la dirección on-demand `0xC0C00000`, tendríamos que modificar la función `page_fault_handler`. Vamos a tomar la decisión de que, cuando una de las dos tareas de una pareja pida esa dirección virtual, mapeamos las dos. Esto permite que no tengamos que mantener ningún chequeo de que el otro ya haya mapeado ni reservar sus páginas físicas. 
```C
bool page_fault_handler(vaddr_t virt) {
print("Atendiendo page fault...", 0, 0, C_FG_WHITE | C_BG_BLACK);
	// Chequeemos si el acceso fue dentro del area on-demand
	// En caso de que si, mapear la pagina
	if((virt >= ON_DEMAND_MEM_START_VIRTUAL) && (virt <= ON_DEMAND_MEM_END_VIRTUAL)){
	//Conseguimos el CR3
	uint32_t cr3 = rcr3();
	mmu_map_page(cr3, ON_DEMAND_MEM_START_VIRTUAL, ON_DEMAND_MEM_START_PHYSICAL, (MMU_P | MMU_U | MMU_W));
	return true;
	}
	//Tenemos que chequear a ver si fue una tarea en la dirección de memoria compartida para parejas. 
	else if(virt >= 0xC0C00000 && virt < 0xC0C00000+1024*PAGE_SIZE){
		sched_entry_t tareaActual = sched_tasks[current_task];
		if(tareaActual.partner_status == TASK_MARRIED){
			uint32_t cr3_pareja = obtener_cr3(tareaActual.id_pareja);
			uint32_t cr3_actual = rcr3();
			paddr_t phys = mmu_next_user_page();
			zero_page(phys); //Limpiamos la página física para que quede con todo ceros.
			if(es_lider(tareaActual.id_pareja)){
			mmu_map_page(cr3_pareja, virt, phys, MMU_P | MMU_W | MMU_U);
			mmu_map_page(cr3_actual, virt, phys, MMU_P | MMU_U);	
			}
			else{
			mmu_map_page(cr3_actual, virt, phys, MMU_P | MMU_W | MMU_U);
			mmu_map_page(cr3_pareja, virt, phys, MMU_P | MMU_U);
			}
		}
		else{
			return false;
		}
	}
	//Si no se pudo, false. return false;
	return false;
}
```
Si la tarea que intentó acceder está en pareja, y es líder, le mapeamos la página con permisos de escritura y a la pareja sin permisos de escritura. Si estamos en pareja y no somos el líder, hacemos al revés. Por último, si no estamos en pareja, no hacemos nada. 