;compile with
;yasm-0.6.0-win64.exe -f win32 test.asm
;link test.obj /ENTRY:start /SUBSYSTEM:CONSOLE

bits 32

section .data
orgSelector: dd  0 

section .text
global _start


_start:
  mov [orgSelector],cs
  
  int 3
  
  mov eax,[orgSelector]
  and eax,7 ; mask off index keeping ring and table selector
  push ebx
  mov ebx,[orgSelector]
  shr ebx, 3 ;extract index
  add ebx, 2 ; add two to get to 64-bit entry
  shl ebx,3
  or eax,ebx ; recombine
  pop ebx
  
  push eax ;setup for retfz
  push code64
  
  mov eax,0
  mov edx,0x32
  
  retf ;do jmp far but use retf so we can dynamically choose the selector
  
  
bits 64
 code64:
 ;this code would crash with invalid opcode in 32-bit mode
  mov rax,0xFF
  int 3
  mov r8,0x123456789ABCDEF0 ;load some random crap, using 64-bit only registers 
  push r8 ; push 64x2 bytes onto the stack of easy to find crap
  push r8
  mov rdx,0x64 ;change the 32 in edx to 64 to show we were in 64 bit mode
               ;(the 32-bit debugger cant figure out what is going 
               ;on so we cant step though and look)
  xor rax,rax ;jump back to old selector using same trick
  mov eax,[orgSelector]
  push rax
  push code32
  retf
bits 32
code32:
  mov ecx,eax
  mov ecx,edx ;load new edx value to eax
  nop
  pop eax ;reballance stack by poping 32x4
  pop eax
  pop eax
  pop eax
  xor eax,eax
  
  ret ;return to windows