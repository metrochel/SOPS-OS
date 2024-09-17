ENTRY(main)

SECTIONS {
	. = 0xC0000000;

	.symtab : ALIGN(4096) {
		*(.symtab)
	}

	.strtab : ALIGN(4096) {
		*(.strtab)
	}

	.shstrtab : ALIGN(4096) {
		*(.shstrtab)
	}

	.text : ALIGN(4096) {
		*(.text)
	}

	.text.startup : ALIGN(4096) {
		*(.text.startup)
	}

	.data : ALIGN(4096) {
		*(.data)
	}

	.bss : ALIGN(4096) {
		*(.bss)
	}

	.rodata : ALIGN(4096) {
		*(.rodata)
	}

	.comment : ALIGN(4096) {
		*(.comment)
	}
}
