data segment 
    A db 1,3,5
    len equ $ - A
data ends

code segment 
    ASSUME CS:CODE, DS:DATA
start:
    mov cx, 100
    mov bx, 0

next:
    add bx, cx
    loop next

    jmp $
 

nextr:
    mov ax ,data
    mov ds, ax

    lea si, A
    mov cx, len
    dec  cx
    mov dl,[si]
    inc si
m0:
   cmp dl,[si]
   jge m1
   mov dl,[si]
m1:
   inc si
   loop m0

   ; jb swap
  
  
    jmp $



 mov ah, 4cH
  int 21H
code ends
    end start