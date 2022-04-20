ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    . = 0x00100000;
	_kernel_start = .;

    .multiboot.text :
    {
        *(.multiboot.text)
    }
	
	.text ALIGN(4K) :
	{
		*(.text)
	}

	.data ALIGN (4K) : 
	{
		*(.data)
	}

	.rodata ALIGN (4K) :
	{
		*(.rodata)
	}

	.bss :
	{
       *(COMMON)
       *(.bss)
	}

	_kernel_end = .;
}