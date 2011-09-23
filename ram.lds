OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_entry_ram)

	jiffies = 0x40018008;

SECTIONS
{
	. = 0x10000400;
	.text.boot : {
		    *(.text.ramboot);
		    *(.text.anyboot);
	}
	.text : { *(.text)}
	.rodata : { *(.rodata) }
	. = ALIGN(16);
	_erom = .;

	.data : {
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

	/DISCARD/ : { *(.text.romboot); *(.vectors); }
}

