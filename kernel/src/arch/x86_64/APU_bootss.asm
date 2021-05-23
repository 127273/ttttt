


[BITS 16]
TRAMPOLINE_BASE equ 0x20000



global boot_end
global trampoline32
extern slave_pro
extern slave_long_mode_start

global boot_start

boot_start:
 
  cld
      mov ax, cs
      mov ds, ax
      mov es, ax
      mov fs, ax
      mov gs, ax
      mov ss, ax
    
  
      mov esp, eax
      mov eax, cr0
       or al, 0x1
       mov cr0, eax
     
      
 ; 
    lgdt [0x330]
    ;jmp   0x8:(trampoline32)
     jmp    0x8:(trampoline32 - boot_start + TRAMPOLINE_BASE)
      ;hlt
[BITS 32] 
section .text

trampoline32:
   hlt
  
	mov	ax,	0x10
	mov	ds,	ax
	mov	es,	ax
	mov	fs,	ax
	mov	gs,	ax
	mov	ss,	ax
    hlt
	;mov	esp,	7E00h
    mov eax, (sp4_table -  boot_start)
    mov cr3, eax

    ; enable Physical Address Extension (PAE-flag) in cr4
    mov eax, cr4
    or eax, 1 << 5
    or eax, 1 << 7
    mov cr4, eax

    ; set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
  hlt
    lgdt	[(GdtLen64 - 1)]  
    
    jmp   0x8:(trampoline64 - boot_start + TRAMPOLINE_BASE)
    hlt

[BITS 64]
; ---- 64 BITS ----
trampoline64:

    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov ax, 0x10
    mov fs, ax
    mov gs, ax
   hlt
    ;jmp 0x8:( 0x30000)
 mov rax, qword vcode64
    jmp vcode64

vcode64:


    push rbp
    ; set up sse as higher half use it
    mov rax, cr0

    btr eax, 2
    bts eax, 1
    mov cr0, rax

    mov rax, cr4
    bts eax, 9
    bts eax, 10
    mov cr4, rax

  
    xor rbp, rbp

    mov rax, qword trampoline_ext
    call rax
    cli
    hlt


    
trampoline_ext:
    call slave_pro
boot_end :


align 16
  lm_gdtr:
    dw lm_gdt_end - lm_gdt_start - 1
    dq lm_gdt_start - boot_start + TRAMPOLINE_BASE

align 16
  lm_gdt_start:
    ; null selector
    dq 0
    ; 64bit cs selector
    dq 0x00AF98000000FFFF
    ; 64bit ds selector
    dq 0x00CF92000000FFFF
  lm_gdt_end:


align 16
  pm_gdtr:
    dw pm_gdt_end - pm_gdt_start - 1
    dd pm_gdt_start - boot_start + TRAMPOLINE_BASE

align 16
  pm_gdt_start:
    ; null selector
    dq 0
    ; cs selector
    dq 0x00CF9A000000FFFF
    ; ds selector
    dq 0x00CF92000000FFFF
  pm_gdt_end:





section .data

align 4096

global sp4_table
sp4_table:
    times 512 dq 0 ; 0 temporary to p3_table



LABEL_GDT64:		dq	0x0000000000000000
LABEL_DESC_CODE64:	dq	0x0020980000000000
LABEL_DESC_DATA64:	dq	0x0000920000000000

GdtLen64	equ	$ - LABEL_GDT64
GdtPtr6433	dw GdtLen64 - 1
    

SelectorCode64	equ	LABEL_DESC_CODE64 - LABEL_GDT64
SelectorData64	equ	LABEL_DESC_DATA64 - LABEL_GDT64
