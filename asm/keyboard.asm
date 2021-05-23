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
    call init_reg
    call keyboard
    
    
    
    
    MOV AH,4CH
    INT 21H
    
init_reg:
	mov bx,0B800h
	mov es,bx
	mov bx,40h
	mov ds,bx
	ret    
show:
	push cx
	push dx
	push ds
	push es
	push si
	push di
	
	mov cx,8
testB:
	mov dx,0
	shl al,1
	adc dx,30h
	mov es:[di],dl
	add di,2
	loop testB
	
	
	pop di
	pop si
	pop es
	pop ds 
	pop dx
	pop cx
	ret
	
	    
keyboard: 
	mov si,17h

testA:		
	mov di,160*0+40*2
	mov al,ds:[si]
	call show	
	jmp testA
	ret
	

CODES ENDS
    END START
