; Compiler instructions
use16      ; The bootloader start always in 16-bits mode ( Real Mode ) 
org 0x7C00 ; Startposition in memory

mov ax, 0x0000      ; Data Segment (ds) register klaarmaken voor gebruik 
mov ds, ax          ; Dit moet via ax omdat de ds register niet direct gevuld kan worden
again:
mov si, HelloWorld  ; De bekende zin in SourceIndex (si) laden
mov ah, 0x0E        ;
mov bh, 0x00        ; don't no what this is
mov bl, 0x47        ; bg=black,ft=white  altough it does't seem to have any affect
nextchar:           ; Begin schrijven letter
    lodsb           ; Zet [SI] in AL en SI++ 
    or al, al       ; Als de vorige opdracht mislukt ( einde zin ) dan... 
    jz return       ; Sprong naar label return 
    int 0x10        ; BIOS video interrupt 
jmp nextchar        ; Volgende letter in SI
return:
;jmp again
jmp $               ; Hier laten we de computer 'vastlopen' ( never ending loop )  

; De Hello World data die voor de fun eindigd met een enter en dan met een 0 om de loop te eindigen
HelloWorld db 'Hello World',13,10,'And good bye!',0

; Opvullen
times 510-($-$$) db 0 ; Opvullen tot 510 bytes met '0'-en
dw 0xAA55 ; Boot loader signature 2 bytes = 512 bytes!!! 
; Daarnaast is deze signature nodig anders weet de bios niet dat er uitvoerbare code in dit segment zit
times 1474560-($-$$) db 0
;