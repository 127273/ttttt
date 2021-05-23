DATAS SEGMENT
    ;此处输入数据段代码  
    a dd 4
    b dd 5
	message  db "boot"
DATAS ENDS

STACKS SEGMENT
    ;此处输入堆栈段代码
STACKS ENDS

CODES SEGMENT
    ASSUME CS:CODES,DS:DATAS,SS:STACKS
START:
    MOV AX,DATAS
    MOV DS,AX
    ;此处输入代码段代码
    mov ax, 9
    cmp ax,6
    ja FunA

    
    MOV AH,4CH
    INT 21H
FunA:
	mov ax,"bo8t"
	mov bp,ax
	mov cx,4
	mov ax, 0B800h
	mov bx,000fh
	mov dx,000fh
	int 10h
	jmp $

CODES ENDS
    END START

