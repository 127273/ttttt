data segment 
    A db 1,3,5
    len equ $ - A
data ends
class Mintor
{
    int serverID;
    int IDkey;
    String serverAddress;
    long MAC;
    int type;
    double value;
    double valueRe;
    Mintor(int type)
    {
        this.type = type;
    }
};

class MintorManager
{
    Mintor mintor1[20];
    Mintor mintor2[20];
    Mintor mintor3[20];

    long GetMAC(int type, int pos)
    {
        return 
    }
};
code segment 
    ASSUME CS:CODE, DS:DATA
start:
    mov cx, 100
    mov bx, 0
    jmp nextr 
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