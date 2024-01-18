
/* avr-libc memory allocation routines free list structure  */
struct __freelist {
	size_t sz;
	struct __freelist *nx;
};

inline unsigned int FreeSRAM(){
unsigned int free_memory = (unsigned int)&free_memory + sizeof(free_memory);
    // the last element in stack, so its address is current pointer to end of stack
	#if defined (__AVR__)

	extern unsigned int __heap_start;	// allocation heap head
	extern void *__brkval;				// heap tail/ 0 if no allocation
	extern struct __freelist *__flp;	// freelist tail

		if(__brkval == 0) free_memory -= (unsigned int)&__heap_start;
		  else {          free_memory -= (unsigned int)__brkval;

		/* add all free memory fragments if any */
			for(struct __freelist *current = __flp; current; current = current->nx) {
				free_memory += 2;		// memory blocks header bytes=2
				free_memory += (unsigned int)current->sz;
			}
		}

	#endif
	return(free_memory);
}