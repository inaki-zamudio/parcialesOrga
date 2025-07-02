# Aprendizajes que surgieron realizando este parcial:

1) Si queremos que una tarea ***ceda*** tiempo de su ejecución a otra, es simplemente hacer un ***context switch*** (con el jump far). 

    Tarea A llama a syscall que luego hace un context switch a la tarea B, entonces la tarea B comenzará a ejecutarse dentro del tiempo de ejecución de la tarea A. 

> Como la syscall se ejecuta en MODO KERNEL, está autorizada a realizar instrucciones privilegiadas como jmp far. Esto es, hacer un cambio manual de tarea (tal como lo hace la isr32). No es necesario ejecutar ni popad ni iret porque la tarea que cede CPU va a ser retomada más adelante por el scheduler. El jmp far se encarga de guardar el estado en la TSS automáticamente, asegurando que pueda continuar correctamente cuando el scheduler la elija. 

2) Nos podemos pasar datos a través de pushear cosas o bien a través de la **memoria compartida**. 