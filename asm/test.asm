DATAS SEGMENT
    ;�˴��������ݶδ���  
    a dd 4
    b dd 5
	message  db "boot"
DATAS ENDS

STACKS SEGMENT
    ;�˴������ջ�δ���
STACKS ENDS

CODES SEGMENT
    ASSUME CS:CODES,DS:DATAS,SS:STACKS
START:
    MOV AX,DATAS
    MOV DS,AX
    ;�˴��������δ���
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

