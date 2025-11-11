a)

```c
uint8_t lock = -1; // variable global que guarda la tarea que posee el lock

void get_lock(vaddr_t shared_page) {
  // si shared_page corresponde al área compartida:
  if (shared_page < TASK_LOCKABLE_PAGE_VIRT || shared_page > TASK_LOCKABLE_PAGE_VIRT + PAGE_SIZE) return;
  // modifica el flag lock:
  lock = current_task;
  // desmapear la página compartida de las demás tareas:
  for (uint32_t i = 0; i < MAX_TASKS; i++) {
    if (i == current_task) continue; // no quiero desmapearME la página
    uint32_t cr3_tarea = obtener_cr3(sched_tasks[i].selector);
    mmu_unmap_page(cr3_tarea, TASK_LOCKABLE_PAGE_VIRT);
  }
}

uint32_t obtener_cr3(uint16_t selector){
    obtener_TSS(selector);
    return tss_task->cr3;
}
```
