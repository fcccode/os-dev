;
;   PANEL SETUP
;
;   Compile with FASM for Menuet
;

use32

               org    0x0

               db     'MENUET01'              ; 8 byte id
               dd     0x01                    ; header version
               dd     START                   ; start of code
               dd     I_END                   ; size of image
               dd     0x100000                ; memory for app
               dd     0x7fff0                 ; esp
               dd     0x0 , 0x0               ; I_Param , I_Icon

START:                          ; start of execution

     call draw_window

still:

    mov  eax,10                 ; wait here for event
    int  0x40

    cmp  eax,1                  ; redraw request ?
    je   red
    cmp  eax,2                  ; key in buffer ?
    je   key
    cmp  eax,3                  ; button in buffer ?
    je   button

    jmp  still

  red:                          ; redraw
    call draw_window
    jmp  still

  key:                          ; key
    mov  eax,2                  ; just read it and ignore
    int  0x40

    shr  eax,8
    cmp  eax,'0'
    jb   still
    cmp  eax,'9'
    jg   still

    mov  edi,[ent]
    add  edi,text
    mov  esi,edi
    inc  esi
    mov  ecx,3
    cld
    rep  movsb

    mov  [edi],al

    call draw_window
    jmp  still

  button:                       ; button
    mov  eax,17                 ; get id
    int  0x40

    cmp  ah,1                   ; button id=1 ?
    jne  noclose
    mov  eax,-1                 ; close this program
    int  0x40
  noclose:

    cmp  ah,10
    jne  no_apply

    mov  esi,text+17
    mov  edi,I_END+10
    mov  ecx,13
   newfe:
    mov  ebx,[esi]
    mov  [edi],ebx
    mov  [edi+4],byte ';'
    add  edi,5
    add  esi,55
    loop newfe
    mov  [edi],byte 'x'

    mov  eax,33
    mov  ebx,filename
    mov  ecx,I_END+10
    mov  edx,5*13+1
    mov  esi,0
    int  0x40


    mov  esi,1
   newread:
    inc  esi
    mov  eax,9
    mov  ebx,I_END
    mov  ecx,esi
    int  0x40
    cmp  esi,eax
    jg   all_terminated

    cmp  [I_END+10],dword 'PANE'
    jne  newread
    cmp  [I_END+14],dword 'L   '
    jne  newread

    mov  eax,18
    mov  ebx,2
    mov  ecx,esi
    int  0x40

    mov  eax,5
    mov  ebx,5
    int  0x40

    mov  esi,1

    jmp  newread

   all_terminated:

    mov  eax,5
    mov  ebx,25
    int  0x40

    mov  eax,19
    mov  ebx,panel
    mov  ecx,0
    int  0x40


  no_apply:

    cmp  ah,11
    jb   no_entry
    shr  eax,8
    sub  eax,11
    imul eax,55
    add  eax,17
    mov  [ent],eax
    mov  [text+eax],dword '0000'
    call draw_window
    jmp  still
  no_entry:


    jmp  still


ent       dd  17
filename  db  'PANEL   DAT'
panel     db  'PANEL      '

;   *********************************************
;   *******  WINDOW DEFINITIONS AND DRAW ********
;   *********************************************


draw_window:


    mov  eax,12                    ; function 12:tell os about windowdraw
    mov  ebx,1                     ; 1, start of draw
    int  0x40

                                   ; DRAW WINDOW
    mov  eax,0                     ; function 0 : define and draw window
    mov  ebx,100*65536+400         ; [x start] *65536 + [x size]
    mov  ecx,100*65536+200         ; [y start] *65536 + [y size]
    mov  edx,0x03ffffff            ; color of work area RRGGBB,8->color gl
    mov  esi,0x805080d0            ; color of grab bar  RRGGBB,8->color gl
    mov  edi,0x005080d0            ; color of frames    RRGGBB
    int  0x40

                                   ; WINDOW LABEL
    mov  eax,4                     ; function 4 : write text to window
    mov  ebx,8*65536+8             ; [x start] *65536 + [y start]
    mov  ecx,0x00ddeeff            ; color of text RRGGBB
    mov  edx,labelt                ; pointer to text beginning
    mov  esi,labellen-labelt       ; text length
    int  0x40

    mov  eax,8
    mov  ebx,25*65536+160
    mov  ecx,172*65536+12
    mov  edx,10
    mov  esi,0x6677cc
    int  0x40

    mov  eax,8
    mov  ebx,340*65536+20
    mov  ecx,34*65536+10
    mov  edx,11
  newb:
    int  0x40
    add  ecx,10*65536
    inc  edx
    cmp  edx,24
    jb   newb

    mov  ebx,25*65536+35           ; draw info text with function 4
    mov  ecx,0x224466
    mov  edx,text
    mov  esi,55
  newline:
    mov  eax,4
    int  0x40
    add  ebx,10
    add  edx,55
    cmp  [edx],byte 'x'
    jne  newline

    mov  eax,12                    ; function 12:tell os about windowdraw
    mov  ebx,2                     ; 2, end of draw
    int  0x40

    ret


; DATA AREA


text:

db  'width            0000  :  0 for full screen width     <'
db  'buttons          0000  :  0 no frames  , 1 frames     <'
db  'soften_up        0001  :  0 no         , 1 yes        <'
db  'soften_down      0001  :  0 no         , 1 yes        <'
db  'minimize_left    0001  :  0 no         , 1 yes        <'
db  'minimize_right   0001  :  0 no         , 1 yes        <'
db  'icons_position   0130  :  position in pixels          <'
db  'menu_enable      0001  :  0 no         , 1 yes        <'
db  'setup_enable     0001  :  0 no         , 1 yes        <'
db  'graph_text       0001  :  0 graphics   , 1 text       <'
db  'soften_middle    0001  :  0 no         , 1 yes        <'
db  'icons            0001  :  0 start      , 1 activate   <'
db  'cpu_usage        0000  :  0 no         , 1 yes        <'
db  '                                                       '
db  '         APPLY                                         '
db  'x'


labelt:
     db   'PANEL SETUP'
labellen:

I_END:



