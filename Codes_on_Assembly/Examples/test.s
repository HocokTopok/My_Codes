.global _main

.section __TEXT,__text
_main:
    mov x0, #1              // stdout
    adrp x1, message@PAGE
    add  x1, x1, message@PAGEOFF
    mov x2, #13             // длина строки
    mov x16, #4             // write
    svc #0x80

    mov x0, #0
    mov x16, #1             // exit
    svc #0x80

.section __TEXT,__cstring
message:
    .asciz "Hello, world!\n"