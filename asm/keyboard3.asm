DATAS SEGMENT
    ;�˴��������ݶδ���  
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
    
    mov ah,0
    int 16h   ;��ȡ���̻�����
    
    cmp al,20h
    jb nochar;����Ĳ����ַ�
	call strpush
	
	jmp start    
    
    


nochar:
	ret
    
    
    MOV AH,4CH
    INT 21H
CODES ENDS
    END START
