a)
```c
void buffer_dma(pd_entry_t* pd) {
	mmu_map_page(pd, 0xBABAB000, 0xF151C000, MMU_U | MMU_P);
}
```

b)
```c
void buffer_copy(pd_entry_t* pd, paddr_t phys) {
	copy_page(phys, 0xF151C000);
	mmu_map_page(pd, 0xBABAB000, phys, MMU_U | MMU_P);
}
```
