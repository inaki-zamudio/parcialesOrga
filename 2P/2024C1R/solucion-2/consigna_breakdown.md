### Características del sistema que se pide:
- Las tareas tienen memoria compartida en TASK_LOCKABLE_PAGE_VIRT
- Si dicha dirección está mapeada, apunta a TASK_LOCKABLE_PAGE_PHY
- Se quiere que sólo una tarea pueda acceder a esa memoria a la vez
- *Procedimiento*:
  - Si la tarea quiere acceder a memoria compartida, llama a la syscall `lock`
  - Si la memoria no está siendo utilizada por otra tarea, la tarea _adquiere_ el lock
  - En cambio, si otra tarea tiene el lock, la tarea que llamó a la syscall deberá abandonar su ejecución, ser pausada hasta adquirir el `lock`, y comenzar inmediatamente a ejecutar la siguiente tarea
  - Cada vez que se ponga en ejecución la tarea, verificará el acceso a la memoria
  - Cuando ya no necesite el `lock`, la tarea deberá liberarlo con la syscall `release`
  - Una vez liberado, la tarea que primero haya pedido el `lock` y todavía no lo recibió podrá adquirirlo
  - No hay un sistema de turnos, simplemente se le dará acceso a la memoria compartida a la tarea que primero vuelva a solicitarlo, una vez que se libere el lock. 


> **ACLARACIÓN:**    
> "... si la tarea invocante de la syscall `lock` no puede acceder a la memoria compartida, debe ser bloqueada y el scheduler debe continuar con la siguiente tarea".