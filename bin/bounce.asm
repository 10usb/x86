org 100h
start:
mov    ax, 13h
int    10h
push   0a000h
pop    es
and    word [20ch], 00h
inc    word [20ch]
fild   word [20ch]
mov    word [20eh], 1eh
fidiv  word [20eh]
fsin   
fabs   
mov    word [20eh], 0ffb0h
fimul  word [20eh]
fistp  word [208h]
add    word [208h], 4bh	
mov    word [202h], 80h
mov    word [200h], 80h
mov    ax, word [200h]
sub    ax, 40h
mov    word [204h], ax
imul   ax, ax
mov    bx, [202h]
sub    bx, 40h
mov    [206h], bx
imul   bx, bx
add    ax, bx
mov    cx, 1000h
sub    cx, 320h
cmp    ax, cx
ja     loc_000001c3
mov    word [20ah], 1000h
sub    [20ah], ax
fild   word [20ah]
fsqrt  
fistp  word [20ah]
mov    ax, word [204h]
shl    ax, 07h
cwd    
idiv   word [20ah]
mov    bx, 20h
sub    bx, [20ah]
imul   ax, bx
mov    cx, ax
mov    ax, word [206h]
shl    ax, 07h
cwd    
idiv   word [20ah]
imul   ax, bx
sar    ax, 07h
add    ax, [202h]
add    ax, [208h]
sar    cx, 07h
add    cx, [200h]
add    cx, [20ch]
xor    ax, cx
and    ax, 1fh
add    ax, 10h
jmp    loc_000001c5
;                                       XREFS First: 0010:0168 Number : 1
loc_000001c3:
xor    al, al
;                                       XREFS First: 0010:01c1 Number : 1
loc_000001c5:
mov    di, [202h]
add    di, [208h]
imul   di, di, 140h
add    di, [200h]
add    di, [20ch]
stosb  
dec    word [200h]
jnz    143h
dec    word [202h]
jnz    13dh
mov    dx, 3dah
in     al, dx
and    al, 08h
jz     1edh
in     al, 60h
dec    al
jnz    10eh
mov    ax, 03h
int    10h
ret    
