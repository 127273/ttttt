DATAS SEGMENT
    ;此处输入数据段代码  
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
    
    mov ah,0
    int 16h   ;读取键盘缓冲区
    
    cmp al,20h
    jb nochar;输入的不是字符
	call strpush
	
	jmp start    
    
    


nochar:
	ret
    
    
    MOV AH,4CH
    INT 21H
CODES ENDS
    END START
