global gdt_flush

gdt_flush:
    mov eax, [esp + 4]      ; Gets the ptr to the gdt from the stack
    lgdt [eax]              ; Load the GDT ptr into the GDTR register

    ; Once the gdt is loaded we need to update the segment registers to use
    ; the new segments we've created. The offsets are:
    ; 0x08 for the Code Segment (entry 1)
    ; 0x10 for the Data Segment (entry 2)

    mov ax, 0x10        ; Loads the offset of our data segment into ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to our code segment to update the CS register
    ; This will flush the cpus pre-fetched instruction pipeline
    jmp 0x08:.flush
.flush:
    ret                 ; Return to C code
