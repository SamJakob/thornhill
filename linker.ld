OUTPUT_FORMAT("elf32-i386")
ENTRY(boot)

SECTIONS {
	. = 0x1000;

	.text BLOCK(4K) : ALIGN(4K) {
		_text_start = .;
        *(.boot)
        *(.text)
		_text_end = .;
	}

    .rodata BLOCK(4K) : ALIGN(4K) {
		__rodata_start = .;
		*(.rodata)
		__rodata_end = .;
	}

    .data BLOCK(4K) : ALIGN(4K) {
		__data_start = .;
		*(.data)
		__data_end = .;
	}

	.bss BLOCK(4K) : ALIGN(4K) {
		__bss_start = .;
        *(COMMON)
		*(.bss)
		__bss_end = .;
	}

	__kernel_end = .;
}