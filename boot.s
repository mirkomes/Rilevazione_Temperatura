/* RAM boot: the first kB of RAM is unused, place the stack here */
.section .text.ramboot
.global _entry_ram
_entry_ram:
	mov	r0, pc
	sub	r0, #12
	mov	sp, r0
	b	_entry

.section .text.romboot
.global _entry_rom
_entry_rom:
	
	ldr	r0, =_sdata
	ldr	r1, =_edata
	ldr	r2, =_erom
	/* stack is before data */
	mov	r3, r0
	sub	r3, #4
	mov	sp, r3
	
	/* copy from r2 to r0, until r1. We know it's aligned at 16b */
10:
	cmp	r0, r1
	bge	_entry
	/*copia le celle di memoria puntante da r2 che parte dall'indirizzo _erom e le salva
	nella sezione puntata da r0 che parte da _sdata fino a quando la zona .data non viene saturata*/
	ldmia	r2!, {r4, r5, r6, r7}
	stmia	r0!, {r4, r5, r6, r7}
	b	10b


.section .text.secondboot
_entry:
	ldr	r1, =__bss_start
	ldr	r2, =__bss_end
	mov	r0, #0
0:
	cmp	r1, r2
	bge	1f
	str	r0, [r1]
	add	r1, #4
	b	0b

1:	
	bl timer_setup
	bl gpio_setup
          bl i2c_setup
          bl serial_setup

	mov	r0, #0
	bl main
2:	b 2b

