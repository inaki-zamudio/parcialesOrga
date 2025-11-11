```c
void buffer_dma(pd_entry_t* pd) {
  mmu_map_page(pd, 0xBABAB000, 0xF151C000, MMU_U | MMU_P);
}

void buffer_copy(pd_entry_t pd, paddr_t phyDir, vaddr_t copyDir) {
  mmu_map_page(pd, copyDir, phyDir, MMU_U | MMU_W | MMU_P);
  copy_page(phyDir, xF151C000);
}
```
