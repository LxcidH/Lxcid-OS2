extern c_interrupt_handler

%macro isr_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret
%endmacro

global isr_stub_table
isr_stub_table:
%assign i 0
%rep    32
    dd isr_stub_%+i
%assign i i+1
%endrep

; This is the core logic that all ISRs and IRQs jump to.
common_handler_stub:
    pusha           ; Save all general-purpose registers.

    mov ax, ds      ; Save the data segment selector.
    push eax

    mov ax, 0x10    ; Load the kernel data segment selector.
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp    ; Get a pointer to the registers struct on the stack
    push eax        ; Push pointer as an argument for the C handler

    call c_interrupt_handler ; Call our unified C handler.

    add esp, 4      ; Clean up the pointer argument from the stack.

    pop eax         ; Restore the original data segment.
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; Restore general-purpose registers.
    add esp, 8      ; Clean up the pushed error code and interrupt number.
    iret            ; Atomically restore state and return from interrupt.

; A safe default handler that just returns, in case we jump to an
; unassigned interrupt vector.
global default_handler
default_handler:
    cli
    hlt

extern exception_handler
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
