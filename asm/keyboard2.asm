DATAS SEGMENT
    char db ?
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
	mov ds:[si+bx],al  ;����������ַ�ascii
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
    mov cx,20;����loopѭ���Ĵ���   chang color ����
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
  	;mov dl, es:[si+dx-1]; 21h dl=Ҫ��ʾ�ַ���ascii
    int 21h
    jmp start ;
  
str_pop:
	mov ax,0B800h
    mov es,ax
    mov bx,1
 	mov cx,200;����loopѭ���Ĵ���   chang color ����
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




