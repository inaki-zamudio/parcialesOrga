```c
vaddr_t* paginas_modificadas(int32_t cr3) {
    vaddr_t arr[];
    pd_entry_t* pd = CR3_TO_PAGE_DIR(cr3);
    for (uint32_t offset = 0; offset < 1024*4; offset+=4) {
        pd_entry_t* pde = pd+offset;
        pt_entry_t* pt = *(pde) >> 20;
        for (uint32_t offset_2 = 0; offset_2 < 1024*4; offset+=4) {
            paddr_t* pte = pt + offset_2;
            
        }
    }
}
```
