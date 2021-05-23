DATAS SEGMENT
    char db ?
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

    int 16h
    mov char,al
   ; mov ah,1
    call strpush
    cmp ah,0Eh
    je  str_pop
    cmp al,'r'
    je red
    cmp al,'g'
    je green
   
    
    
strpush:
	mov ds:[si+bx],al  ;保留输入的字符ascii
	jmp show
	jmp start    
   
   
red :
	shl ah,1
	
green:
	shl ah,1
	
show:
	mov ax,0B800h
    mov es,ax
    mov bx,1
    mov cx,20;控制loop循环的次数   chang color 距离
    s:
    	and byte ptr es:[bx],1111000b
    	or es:[bx],ah
    	add bx,2
    	loop s
    
  ;  mov ax,1301h
    mov al,160
    mov ah,0
    mul dh
    mov di,ax
    add dl,dl
    mov dh,0
    add di,dx
    
    
    ;mov bx,0 
    ;mov di,ax
   ; mov dx,0000h	
  ;  mov byte ptr es:[di],' '
   ; mov ah,2
  	;mov dl, es:[si+dx-1]; 21h dl=要显示字符的ascii
    int 21h
    jmp start ;
  
str_pop:
	mov ax,0B800h
    mov es,ax
    mov bx,1
 	mov cx,200;控制loop循环的次数   chang color 距离
   z:
    	and byte ptr es:[bx],1111000b
    	or es:[bx],ah
    	add bx,2
    	loop z

	;dec bx
	;mov byte ptr ds:[si+bx],0
	jmp start     
    MOV AH,4CH
    INT 21H
CODES ENDS
    END START




