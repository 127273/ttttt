int main()
{
	__asm{


	mov ax,cs
	mov ds,ax
	mov es,ax
	call pri
   ; jmp $ 897huibin
	pri:
		mov ax,4567
		mov bp,ax
		mov cx,4
		mov ax,1310h
		mov bx,0000h
		mov dx,0004h
		int 10h
		ret


	}
}