/*
	Set the kernel entry point to the _start method.
	This method accepts the handoff arguments from the bootloader and initializes the kernel.
*/
ENTRY(_start)


/*
	Thornhill has a higher-half kernel.
	As the OS is designed for x86_64 architecture, the kernel's sections will be page-aligned to 4KiB pages.
*/
/*
kernel_base = 0xFFFFFFFF80000000;
kernel_base_phys = 2M;

kernel_start = kernel_base + kernel_base_phys;
*/

/*
	Set the program headers.

	This is a useful overview of how PHDRS works:
	https://doc.ecoscentric.com/gnutools/doc/ld/PHDRS.html
*/
PHDRS {
	headers PT_PHDR PHDRS;
	text PT_LOAD FILEHDR PHDRS;
	data PT_LOAD;
}

SECTIONS {
	/*
		Set the location counter to our kernel start address.

		When the output address is not specified by some other means, the address is set from
		the current value of the location counter, which is then incremented by the size of the
		outputted section.
	*/
	. = 2M;
	kernel_base = .;


	/*
		Program code.
	*/
	.text ALIGN(4K) : {
		*(.text*)
	} :text

	/*
		Read-only data.

		This section will be placed immediately after the previous section (.text).
	*/
	.rodata ALIGN(4K) : {
		*(.rodata*)
	} :text


	/* Move to a new page in memory. */
	. = . + 0x1000;

	/*
		Read-Write Initialized Data.

		This section will be placed immediately after the previous section (.rodata).
	*/
	.data ALIGN(4K) : {
		*(.data*)
	} :data

	/*
		Read-Write Zero-Initialized Data.

		'Block Started by Symbol' section (also known as the 'Better Save Space' section).
		This reserves space for any uninitialized objects declared at file scope.

		As with before, this will be placed immediately after the previous section (.data).
	*/
	.bss ALIGN(4K) : {
		bss_start = .;

		*(COMMON)
		*(.bss*)

		bss_end = .;
	}
	

	kernel_end = .;

	/*
		The output section name "/DISCARD/" is special; it is used to discard input sections.
		Any input sections assigned to "/DISCARD/" are not included in the output file.
	*/
	/DISCARD/ : { *(.eh_frame) *(.comment) }
}