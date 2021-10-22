/*
	Set the kernel entry point to the _start method.
	This method accepts the handoff arguments from the bootloader and initializes the kernel.
*/
ENTRY(_start)


/*
	Thornhill is written for UEFI systems, which means we boot up in long mode.
	Thus, the kernel base address is at 2M to ensure it's not accessed in real
	mode. This also removes any size limits for the kernel. The virtual memory
	base address is set to 64GiB.
	
	As the OS is designed for x86_64 architecture, the kernel's sections are
	page-aligned to 4KiB pages.
*/

KERNEL_VIRTUAL_BASE = 0x1000000000; /* 64 GiB */

SECTIONS {
	/*
		Set the location counter to our kernel start address.

		When the output address is not specified by some other means, the address is set from
		the current value of the location counter, which is then incremented by the size of the
		outputted section.
	*/
	. = KERNEL_VIRTUAL_BASE;
	kernel_virtual_base = .;

	. = KERNEL_VIRTUAL_BASE + 2M;
	kernel_start = .;
	kernel_code_start = .;


	/*
		Program code.
	*/
	.text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VIRTUAL_BASE) {
		*(.text*)
	}

	/*
		Read-only data.

		This section will be placed immediately after the previous section (.text).
	*/
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VIRTUAL_BASE) {
		*(.rodata*)
	}

	kernel_code_end = .;

	/* Move to the data boundary. */
	. = . + 2M;
	kernel_data_start = .;

	/*
		Read-Write Initialized Data.

		This section will be placed immediately after the start of the kernel data boundary
		(kernel_data_start).
	*/
	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE) {
		*(.data*)
	}

	/*
		Read-Write Zero-Initialized Data.

		'Block Started by Symbol' section (also known as the 'Better Save Space' section).
		This reserves space for any uninitialized objects declared at file scope.

		As with before, this will be placed immediately after the previous section (.data).
	*/
	.bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE) {
		bss_start = .;

		*(COMMON)
		*(.bss*)

		bss_end = .;
	}
	

	kernel_data_end = .;
	kernel_end = .;

	/*
		The output section name "/DISCARD/" is special; it is used to discard input sections.
		Any input sections assigned to "/DISCARD/" are not included in the output file.
	*/
	/DISCARD/ : { *(.eh_frame) *(.comment) }
}
