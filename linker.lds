OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_entry_rom)

	jiffies = 0x40018008;

SECTIONS
{
        . = 0x0;
        .vectors : { *(.vectors) }
        .text.boot : {
	    *(.text.romboot);
	    *(.text.secondboot);
        }
        .text : { *(.text); }	
        .rodata : { *(.rodata) }
        . = ALIGN(16);
	_erom = .;

	.data 0x10000400: AT(_erom) {
	      _sdata = .;
	      *(.data);
	      _edata = .;
        }

	__temp_start = .;
	.temp : {*(.temp); }
	__temp_end = .;

	__task_start = .;
	.task : {*(.task); }
	__task_end = .;

        . = ALIGN(16);
        .bss : {
	      __bss_start = .;
	      *(.bss);
	      . = ALIGN(16);
	      __bss_end = .;

        }
}
