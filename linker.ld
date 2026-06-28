ENTRY(_start)

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

    .init : ALIGN(4096) {
        *(.init)
    }

    .fini : ALIGN(4096) {
        *(.fini)
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
