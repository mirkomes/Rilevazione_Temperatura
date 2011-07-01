OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_entry_rom)

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

        . = ALIGN(16);
        .bss : {
	      __bss_start = .;
	      *(.bss);
	      . = ALIGN(16);
	      __bss_end = .;

        }
}
