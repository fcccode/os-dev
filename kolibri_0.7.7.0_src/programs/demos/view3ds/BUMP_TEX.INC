
;CATMULL_SHIFT equ 8
;TEXTURE_SIZE EQU (TEX_X * TEX_Y)-1
;ROUND equ 8
;Ext = NON
;MMX = 1
;NON = 0
;------- Big thanks to Majuma (www.majuma.xt.pl) for absolutely great---
;------- DOS 13h mode demos --------------------------------------------
;------- Procedure draws bump triangle with texture, I use -------------
;--------Catmull Z-buffer algorithm- (Z coordinate interpolation)-------
;--------I calc texture pixel by this way: col1*col2/256 ---------------
bump_tex_triangle_z:
;------------------in - eax - x1 shl 16 + y1 -----------
;---------------------- ebx - x2 shl 16 + y2 -----------
;---------------------- ecx - x3 shl 16 + y3 -----------
;---------------------- edx - pointer to bump map-------
;---------------------- esi - pointer to env map--------
;---------------------- edi - pointer to screen buffer--
;---------------------- stack : bump coordinates--------
;----------------------         environment coordinates-
;----------------------         Z position coordinates--
;----------------------         pointer to Z buffer-----
;----------------------         pointer to texture------
;----------------------         texture coordinates-----
;-- Z-buffer - filled with coordinates as dword --------
;-- (Z coor. as word) shl CATMULL_SHIFT ----------------
.b_x1	equ ebp+4   ; procedure don't save registers !!!
.b_y1	equ ebp+6   ; each coordinate as word
.b_x2	equ ebp+8
.b_y2	equ ebp+10	 ; b - bump map coords
.b_x3	equ ebp+12	 ; e - env map coords
.b_y3	equ ebp+14
.e_x1	equ ebp+16
.e_y1	equ ebp+18
.e_x2	equ ebp+20
.e_y2	equ ebp+22
.e_x3	equ ebp+24
.e_y3	equ ebp+26
.z1	equ word[ebp+28]
.z2	equ word[ebp+30]
.z3	equ word[ebp+32]
.z_buff equ dword[ebp+34]	; pointer to Z-buffer
.tex_ptr equ dword[ebp+38]	; ptr to texture
.t_x1	equ ebp+42		; texture coords
.t_y1	equ ebp+44
.t_x2	equ ebp+46
.t_y2	equ ebp+48
.t_x3	equ ebp+50
.t_y3	equ ebp+52



.t_bmap equ dword[ebp-4]	; pointer to bump map
.t_emap equ dword[ebp-8]	; pointer to env map
.x1	equ word[ebp-10]
.y1	equ word[ebp-12]
.x2	equ word[ebp-14]
.y2	equ word[ebp-16]
.x3	equ word[ebp-18]
.y3	equ word[ebp-20]

.dx12	equ dword[ebp-24]
.dby12q equ	 [ebp-32]
.dbx12	equ dword[ebp-28]
.dby12	equ dword[ebp-32]
.dey12q equ	 [ebp-40]
.dex12	equ dword[ebp-36]
.dey12	equ dword[ebp-40]
.dz12	equ dword[ebp-44]
.dtx12	equ dword[ebp-48]
.dty12	equ dword[ebp-52]
.dty12q equ	 [ebp-52]

.dx13  equ dword[ebp-52-4*1]
.dbx13 equ dword[ebp-52-4*2]
.dby13 equ dword[ebp-52-4*3]
.dby13q equ	[ebp-52-4*3]
.dex13 equ dword[ebp-52-4*4]
.dey13 equ dword[ebp-52-4*5]
.dey13q equ	[ebp-52-4*5]
.dz13  equ dword[ebp-52-4*6]
.dtx13 equ dword[ebp-52-4*7]
.dty13 equ dword[ebp-52-4*8]


.dx23  equ dword[ebp-(52+4*9)]
.dbx23 equ dword[ebp-(52+4*10)]
.dby23 equ dword[ebp-(52+4*11)]
.dby23q equ	[ebp-(52+4*11)]
.dex23 equ dword[ebp-(52+4*12)]
.dey23 equ dword[ebp-(52+4*13)]
.dey23q equ	[ebp-(52+4*13)]
.dz23  equ dword[ebp-(52+4*14)]
.dtx23 equ dword[ebp-(52+4*15)]
.dty23 equ dword[ebp-(52+4*16)]

.cx1   equ dword[ebp-(52+4*17)] 	; current variables
.cx2   equ dword[ebp-(52+4*18)]
.cbx1  equ dword[ebp-(52+4*19)]
.cbx2  equ dword[ebp-(52+4*20)]
.cby1  equ dword[ebp-(52+4*21)]
.cby2  equ dword[ebp-(52+4*22)]
.cex1  equ dword[ebp-(52+4*23)]
.cex2  equ dword[ebp-(52+4*24)]
.cey1  equ dword[ebp-(52+4*25)]
.cey2  equ dword[ebp-(52+4*26)]
.cz1   equ dword[ebp-(52+4*27)]
.cz2   equ dword[ebp-(52+4*28)]

.ctx1  equ dword[ebp-(52+4*29)]
.ctx2  equ dword[ebp-(52+4*30)]
.cty1  equ dword[ebp-(52+4*31)]
.cty2  equ dword[ebp-(52+4*32)]

    if Ext >= MMX
       emms
    end if
       cld
       mov     ebp,esp
       push    edx	  ; store bump map
       push    esi	  ; store e. map
     ; sub     esp,120
 .sort3:		  ; sort triangle coordinates...
       cmp     ax,bx
       jle     .sort1
       xchg    eax,ebx
       mov     edx,dword[.b_x1]
       xchg    edx,dword[.b_x2]
       mov     dword[.b_x1],edx
       mov     edx,dword[.e_x1]
       xchg    edx,dword[.e_x2]
       mov     dword[.e_x1],edx
       mov     edx,dword[.t_x1]
       xchg    edx,dword[.t_x2]
       mov     dword[.t_x1],edx
       mov     dx,.z1
       xchg    dx,.z2
       mov     .z1,dx
 .sort1:
       cmp	bx,cx
       jle	.sort2
       xchg	ebx,ecx
       mov	edx,dword[.b_x2]
       xchg	edx,dword[.b_x3]
       mov	dword[.b_x2],edx
       mov	edx,dword[.e_x2]
       xchg	edx,dword[.e_x3]
       mov	dword[.e_x2],edx
       mov	edx,dword[.t_x2]
       xchg	edx,dword[.t_x3]
       mov	dword[.t_x2],edx
       mov     dx,.z2
       xchg    dx,.z3
       mov     .z2,dx
       jmp	.sort3
 .sort2:
       push	eax	; store triangle coords in variables
       push	ebx
       push	ecx
	 mov	  edx,80008000h  ; eax,ebx,ecx are ANDd together into edx which means that
	 and	  edx,ebx	 ; if *all* of them are negative a sign flag is raised
	 and	  edx,ecx
	 and	  edx,eax
	 test	  edx,80008000h  ; Check both X&Y at once
	 jne	  .loop23_done
    ;   mov     edx,eax         ; eax,ebx,ecx are ORd together into edx which means that
    ;   or      edx,ebx         ; if any *one* of them is negative a sign flag is raised
    ;   or      edx,ecx
    ;   test    edx,80000000h   ; Check only X
    ;   jne     .loop23_done

    ;   cmp     .x1,SIZE_X    ; {
    ;   jg      .loop23_done
    ;   cmp     .x2,SIZE_X     ; This can be optimized with effort
    ;   jg      .loop23_done
    ;   cmp     .x3,SIZE_X
    ;   jg      .loop23_done    ; {


       mov	bx,.y2	     ; calc delta 12
       sub	bx,.y1
       jnz	.bt_dx12_make
       mov	ecx,8
       xor	edx,edx
     @@:
       push	edx   ;dword 0
       loop	@b
       jmp	.bt_dx12_done
 .bt_dx12_make:

       mov	ax,.x2
       sub	ax,.x1
       cwde
       movsx	ebx,bx
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dx12,eax
       push	 eax

;if Ext>=SSE
;
;       sub       esp,16
;       mov       eax,255
;       cvtsi2ss  xmm4,eax
;       cvtsi2ss  xmm3,ebx            ;rcps
;       divss     xmm3,xmm4
;       shufps    xmm3,xmm3,0
;
;       movd      mm0,[.b_x1]
;       movd      mm1,[.b_x2]
;       movd      mm2,[.e_x1]
;       movd      mm3,[.e_x2]
;
;       pxor       mm4,mm4
;       punpcklwd  mm0,mm4
;       punpcklwd  mm1,mm4
;       punpcklwd  mm2,mm4
;       punpcklwd  mm3,mm4
;
;       cvtpi2ps  xmm0,mm0
;       movlhps   xmm0,xmm0
;       cvtpi2ps  xmm0,mm2
;       cvtpi2ps  xmm1,mm1
;       movlhps   xmm1,xmm1
;       cvtpi2ps  xmm1,mm3
;       subps     xmm1,xmm0
;
;       divps     xmm1,xmm3
;
;       shufps    xmm1,xmm1,10110001b
;       cvtps2pi  mm0,xmm1          ; mm0 -> 2 delta dwords
;       movhlps   xmm1,xmm1
;       cvtps2pi  mm1,xmm1
;       movq      .dey12q,mm0
;       movq      .dby12q,mm1
;
;;       mov       ax,.z1
;;       mov       bx,.z2
;;       shl       eax,16
;       shl       ebx,16
;       mov       ax,.x1
;       mov       bx,.x2
;       movd      mm2,[.t_x1]
;       movd      mm3,[.t_x2]
;       movd      mm0,eax
;       movd      mm1,ebx

;       pxor       mm4,mm4
;       punpcklwd  mm0,mm4
;       punpcklwd  mm1,mm4
;       punpcklwd  mm2,mm4
;       punpcklwd  mm3,mm4

;       cvtpi2ps  xmm0,mm0
;       movlhps   xmm0,xmm0
;       cvtpi2ps  xmm0,mm2
;       cvtpi2ps  xmm1,mm1
;       movlhps   xmm1,xmm1
;       cvtpi2ps  xmm1,mm3
;       subps     xmm1,xmm0

;       divps     xmm1,xmm3

;       shufps    xmm1,xmm1,10110001b
;       cvtps2pi  mm0,xmm1          ; mm0 -> 2 delta dwords
;       movhlps   xmm1,xmm1
;       cvtps2pi  mm1,xmm1
;       movq      .dty12q,mm0
;       movd      .dx12,mm1
;       psrlq     mm1,32
;       movd      .dz12,mm1


;
;       cvtpi2ps  xmm0,.tx1q ;mm0
;       movlhps   xmm0,xmm0
;       cvtpi2ps  xmm0,mm2
;       cvtpi2ps  xmm1,.tx2q ;mm1
;       movlhps   xmm1,xmm1
;       cvtpi2ps  xmm1,mm3
;       subps     xmm1,xmm0
;
;       divps     xmm1,xmm3
;
;       shufps    xmm1,xmm1,10110100b
;       cvtps2pi  mm0,xmm1          ; mm0 -> 2 delta dwords
;       movhlps   xmm1,xmm1
;       cvtps2pi  mm1,xmm1
;       movd      .dz,mm0
;       movq      .dtyq,mm1

;else

       mov	ax,word[.b_x2]
       sub	ax,word[.b_x1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dbx12,eax
       push	 eax

       mov	ax,word[.b_y2]
       sub	ax,word[.b_y1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dby12,eax
       push	 eax

       mov	ax,word[.e_x2]
       sub	ax,word[.e_x1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dex12,eax
       push	 eax

       mov	ax,word[.e_y2]
       sub	ax,word[.e_y1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dey12,eax
       push	 eax
;end if

       mov     ax,.z2
       sub     ax,.z1
       cwde
       shl     eax,CATMULL_SHIFT
       cdq
       idiv    ebx
       push    eax

       mov	ax,word[.t_x2]
       sub	ax,word[.t_x1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dtx12,eax
       push	 eax

       mov	ax,word[.t_y2]
       sub	ax,word[.t_y1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dty12,eax
       push	 eax


   .bt_dx12_done:

       mov	bx,.y3	     ; calc delta13
       sub	bx,.y1
       jnz	.bt_dx13_make
       mov	ecx,8
       xor	edx,edx
     @@:
       push	edx   ;dword 0
       loop	@b
       jmp	.bt_dx13_done
 .bt_dx13_make:
       mov	ax,.x3
       sub	ax,.x1
       cwde
       movsx	ebx,bx
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dx13,eax
       push	 eax
;if Ext>=SSE
;
;       sub       esp,16
;       mov       eax,255
;       cvtsi2ss  xmm4,eax
;       cvtsi2ss  xmm3,ebx            ;rcps
;       divss     xmm3,xmm4
;       shufps    xmm3,xmm3,0
;
;       movd      mm0,[.b_x1]
;       movd      mm1,[.b_x3]
;       movd      mm2,[.e_x1]
;       movd      mm3,[.e_x3]
;
;       pxor       mm4,mm4
;       punpcklwd  mm0,mm4
;       punpcklwd  mm1,mm4
;       punpcklwd  mm2,mm4
;       punpcklwd  mm3,mm4
;
;       cvtpi2ps  xmm0,mm0
;       movlhps   xmm0,xmm0
;       cvtpi2ps  xmm0,mm2
;       cvtpi2ps  xmm1,mm1
;       movlhps   xmm1,xmm1
;       cvtpi2ps  xmm1,mm3
;       subps     xmm1,xmm0
;
;       divps     xmm1,xmm3
;
;       shufps    xmm1,xmm1,10110001b
;       cvtps2pi  mm0,xmm1          ; mm0 -> 2 delta dwords
;       movhlps   xmm1,xmm1
;       cvtps2pi  mm1,xmm1
;       movq      .dey13q,mm0
;       movq      .dby13q,mm1
;else
       mov	ax,word[.b_x3]
       sub	ax,word[.b_x1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dbx13,eax
       push	 eax

       mov	ax,word[.b_y3]
       sub	ax,word[.b_y1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dby13,eax
       push	 eax

       mov	ax,word[.e_x3]
       sub	ax,word[.e_x1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dex13,eax
       push	 eax

       mov	ax,word[.e_y3]
       sub	ax,word[.e_y1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dey13,eax
       push	 eax
;end if

       mov     ax,.z3
       sub     ax,.z1
       cwde
       shl     eax,CATMULL_SHIFT
       cdq
       idiv    ebx
  ;    mov    .dz13,eax
       push    eax

       mov	ax,word[.t_x3]
       sub	ax,word[.t_x1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dtx13,eax
       push	 eax

       mov	ax,word[.t_y3]
       sub	ax,word[.t_y1]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dty13,eax
       push	 eax

   .bt_dx13_done:

       mov	bx,.y3	     ; calc delta23
       sub	bx,.y2
       jnz	.bt_dx23_make
       mov	ecx,8
       xor	edx,edx
     @@:
       push	edx   ;dword 0
       loop	@b
       jmp	.bt_dx23_done
 .bt_dx23_make:
       mov	ax,.x3
       sub	ax,.x2
       cwde
       movsx	ebx,bx
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dx23,eax
       push	 eax

;if Ext>=SSE
;
;       sub       esp,16
;       mov       eax,255
;       cvtsi2ss  xmm4,eax
;       cvtsi2ss  xmm3,ebx            ;rcps
;       divss     xmm3,xmm4
;       shufps    xmm3,xmm3,0
;
;       movd      mm0,[.b_x2]
;       movd      mm1,[.b_x3]
;       movd      mm2,[.e_x2]
;       movd      mm3,[.e_x3]
;
;       pxor       mm4,mm4
;       punpcklwd  mm0,mm4
;       punpcklwd  mm1,mm4
;       punpcklwd  mm2,mm4
;       punpcklwd  mm3,mm4
;
;       cvtpi2ps  xmm0,mm0
;       movlhps   xmm0,xmm0
;       cvtpi2ps  xmm0,mm2
;       cvtpi2ps  xmm1,mm1
;       movlhps   xmm1,xmm1
;       cvtpi2ps  xmm1,mm3
;       subps     xmm1,xmm0
;
;       divps     xmm1,xmm3
;
;       shufps    xmm1,xmm1,10110001b
;       cvtps2pi  mm0,xmm1          ; mm0 -> 2 delta dwords
;       movhlps   xmm1,xmm1
;       cvtps2pi  mm1,xmm1
;       movq      .dey23q,mm0
;       movq      .dby23q,mm1
;else
       mov	ax,word[.b_x3]
       sub	ax,word[.b_x2]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dbx23,eax
       push	 eax

       mov	ax,word[.b_y3]
       sub	ax,word[.b_y2]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dby23,eax
       push	 eax

       mov	ax,word[.e_x3]
       sub	ax,word[.e_x2]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dex23,eax
       push	 eax

       mov	ax,word[.e_y3]
       sub	ax,word[.e_y2]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dey23,eax
       push	 eax
;end if

       mov     ax,.z3
       sub     ax,.z2
       cwde
       shl     eax,CATMULL_SHIFT
       cdq
       idiv    ebx
     ; mov     .dz23,eax
       push    eax

       mov	ax,word[.t_x3]
       sub	ax,word[.t_x2]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dtx23,eax
       push	 eax

       mov	ax,word[.t_y3]
       sub	ax,word[.t_y2]
       cwde
       shl	eax,ROUND
       cdq
       idiv	ebx
 ;     mov      .dty23,eax
       push	 eax

      ;  sub     esp,40
   .bt_dx23_done:
       movsx	eax,.x1
       shl	eax,ROUND
     ; mov      .cx1,eax
     ; mov      .cx2,eax
       push	eax
       push	eax
     ;  sub       esp,8

       movsx	ebx,word[.b_x1]
       shl	ebx,ROUND
     ; mov      .cbx1,eax
     ; mov      .cbx2,eax
       push	ebx
       push	ebx

       movsx	ecx,word[.b_y1]
       shl	ecx,ROUND
     ; mov      .cby1,eax
     ; mov      .cby2,eax
       push	ecx
       push	ecx

       movsx	edx,word[.e_x1]
       shl	edx,ROUND
      ;mov      .cex1,eax
      ;mov      .cex2,eax
       push	edx
       push	edx

       movsx	eax,word[.e_y1]
       shl	eax,ROUND
      ;mov      .cey1,eax
      ;mov      .cey2,eax
       push	eax
       push	eax

       movsx	ebx,.z1
       shl	ebx,CATMULL_SHIFT
      ;mov      .cz1,eax
      ;mov      .cz2,eax
       push	ebx
       push	ebx

       movsx	ecx,word[.t_x1]
       shl	ecx,ROUND
      ;mov      .ctx1,eax
      ;mov      .ctx2,eax
       push	ecx
       push	ecx

       movsx	edx,word[.t_y1]
       shl	edx,ROUND
      ;mov      .cty1,eax
      ;mov      .cty2,eax
       push	edx
       push	edx


       movsx	ecx,.y1
       cmp	cx,.y2
       jge	.loop12_done
  .loop12:
       call	.call_line

       mov	eax,.dx13
       add	.cx1,eax
       mov	ebx,.dx12
       add	.cx2,ebx


       mov	edx,.dbx13
       add	.cbx1,edx
       mov	eax,.dbx12
       add	.cbx2,eax
       mov	ebx,.dby13
       add	.cby1,ebx
       mov	edx,.dby12
       add	.cby2,edx

       mov	eax,.dex13
       add	.cex1,eax
       mov	ebx,.dex12
       add	.cex2,ebx
       mov	edx,.dey13
       add	.cey1,edx
       mov	eax,.dey12
       add	.cey2,eax

       mov	ebx,.dz13
       add	.cz1,ebx
       mov	edx,.dz12
       add	.cz2,edx

       mov	eax,.dtx13
       add	.ctx1,eax
       mov	ebx,.dtx12
       add	.ctx2,ebx
       mov	edx,.dty13
       add	.cty1,edx
       mov	eax,.dty12
       add	.cty2,eax

       inc	ecx
       cmp	cx,.y2
       jl	.loop12
    .loop12_done:

       movsx	ecx,.y2
       cmp	cx,.y3
       jge	.loop23_done

       movsx	eax,.z2
       shl	eax,CATMULL_SHIFT
       mov	.cz2,eax

       movsx	ebx,.x2
       shl	ebx,ROUND
       mov	.cx2,ebx

       movzx	edx,word[.b_x2]
       shl	edx,ROUND
       mov	.cbx2,edx

       movzx	eax,word[.b_y2]
       shl	eax,ROUND
       mov	.cby2,eax

       movzx	ebx,word[.e_x2]
       shl	ebx,ROUND
       mov	.cex2,ebx

       movzx	edx,word[.e_y2]
       shl	edx,ROUND
       mov	.cey2,edx

       movzx	eax,word[.t_x2]
       shl	eax,ROUND
       mov	.ctx2,eax

       movzx	ebx,word[.t_y2]
       shl	ebx,ROUND
       mov	.cty2,ebx

     .loop23:
       call	.call_line

       mov	eax,.dx13
       add	.cx1,eax
       mov	ebx,.dx23
       add	.cx2,ebx

       mov	edx,.dbx13
       add	.cbx1,edx
       mov	eax,.dbx23
       add	.cbx2,eax
       mov	ebx,.dby13
       add	.cby1,ebx
       mov	edx,.dby23
       add	.cby2,edx

       mov	eax,.dex13
       add	.cex1,eax
       mov	ebx,.dex23
       add	.cex2,ebx
       mov	edx,.dey13
       add	.cey1,edx
       mov	eax,.dey23
       add	.cey2,eax

       mov	ebx,.dz13
       add	.cz1,ebx
       mov	edx,.dz23
       add	.cz2,edx

       mov	eax,.dtx13
       add	.ctx1,eax
       mov	ebx,.dtx23
       add	.ctx2,ebx
       mov	edx,.dty13
       add	.cty1,edx
       mov	eax,.dty23
       add	.cty2,eax

       inc	ecx
       cmp	cx,.y3
       jl	.loop23
    .loop23_done:

       mov	esp,ebp
ret   50

.call_line:

       pushad
       push	.tex_ptr
       push	.cty2
       push	.ctx2
       push	.cty1
       push	.ctx1
       push	.cz1
       push	.cz2
       push	.z_buff
       push	.t_emap
       push	.t_bmap
       push	.cey2
       push	.cex2
       push	.cey1
       push	.cex1
       push	.cby2
       push	.cbx2
       push	.cby1
       push	.cbx1
       push	ecx

       mov	eax,.cx1
       sar	eax,ROUND
       mov	ebx,.cx2
       sar	ebx,ROUND

       call	bump_tex_line_z

       popad
ret
bump_tex_line_z:
;--------------in: eax - x1
;--------------    ebx - x2
;--------------    edi - pointer to screen buffer
;stack - another parameters :
.y	equ dword [ebp+4]
.bx1q	equ	  [ebp+8]
.bx2q	equ	  [ebp+16]
.ex1q	equ	  [ebp+24]
.ex2q	equ	  [ebp+32]
.tx1q	equ	  [ebp+60]
.tx2q	equ	  [ebp+68]
;.bx1q   equ       [ebp+8]
;.bx2q   equ       [ebp+16]
;.ex1q   equ       [ebp+24]
;.exyq   equ       [ebp+32]
.bx1	equ dword [ebp+8]   ;   ---
.by1	equ dword [ebp+12]  ;       |
.bx2	equ dword [ebp+16]  ;       |
.by2	equ dword [ebp+20]  ;       |>   b. map and e. map coords
.ex1	equ dword [ebp+24]  ;       |>   shifted shl ROUND
.ey1	equ dword [ebp+28]  ;       |
.ex2	equ dword [ebp+32]  ;       |
.ey2	equ dword [ebp+36]  ;   ---
.bmap	equ dword [ebp+40]  ; bump map offset
.emap	equ dword [ebp+44]  ; env map offset
.z_buff equ dword [ebp+48]
.z2	equ dword [ebp+52]  ;   -- |>   z coords shifted
.z1	equ dword [ebp+56]  ;   --       shl  CATMULL_SHIFT

.tx1	equ dword [ebp+60]  ;  -----
.ty1	equ dword [ebp+64]  ;       |>   shifted shl ROUND
.tx2	equ dword [ebp+68]  ;       |
.ty2	equ dword [ebp+72]  ;   ---
.tex_map equ dword [ebp+76]  ; texture offset  ( pointer )


.x1	equ dword [ebp-4]
.x2	equ dword [ebp-8]
.dbx	equ dword [ebp-12]
.dby	equ dword [ebp-16]
.dbyq	equ qword [ebp-16]  ; - new
.dex	equ dword [ebp-20]
.dey	equ dword [ebp-24]
.deyq	equ qword [ebp-24]  ; - new
.dz	equ dword [ebp-28]
.dtx	equ dword [ebp-32]
.dty	equ dword [ebp-36]
.dtyq	equ qword [ebp-36]

.cbx	equ dword [ebp-40]
.cby	equ dword [ebp-44]
.cbyq	equ qword [ebp-44]  ; - new
.cex	equ dword [ebp-48]
.cey	equ dword [ebp-52]
.ceyq	equ qword [ebp-52]  ; - new
.cz	equ dword [ebp-56]
.czbuff equ dword [ebp-60]
.ctx	equ dword [ebp-64]
.cty	equ dword [ebp-68]
.ctyq	equ qword [ebp-68]
.c_scr	equ dword [ebp-72]

.temp1	equ	   ebp-80
.temp2	equ	   ebp-88
.temp3	equ	   ebp-76
.temp4	equ	   ebp-84
.temp5	equ	   ebp-92

	mov	ebp,esp



	mov	ecx,.y
	or	ecx,ecx
	jl	.bl_end
	cmp	ecx,SIZE_Y
	jge	.bl_end

	cmp	eax,ebx
	jl	.bl_ok
	je	.bl_end

	xchg	eax,ebx
if Ext=NON
	mov	edx,.bx1
	xchg	edx,.bx2
	mov	.bx1,edx
	mov	edx,.by1
	xchg	edx,.by2
	mov	.by1,edx

	mov	edx,.ex1
	xchg	edx,.ex2
	mov	.ex1,edx
	mov	edx,.ey1
	xchg	edx,.ey2
	mov	.ey1,edx

	mov	edx,.tx1
	xchg	edx,.tx2
	mov	.tx1,edx
	mov	edx,.ty1
	xchg	edx,.ty2
	mov	.ty1,edx
else
	movq	mm0,.bx1q
	movq	mm1,.bx2q
	movq	mm2,.ex1q
	movq	mm3,.ex2q
	movq	mm4,.tx1q
	movq	mm5,.tx2q
	movq	.bx2q,mm0
	movq	.bx1q,mm1
	movq	.ex1q,mm3
	movq	.ex2q,mm2
	movq	.tx1q,mm5
	movq	.tx2q,mm4
end if

	mov	edx,.z1
	xchg	edx,.z2
	mov	.z1,edx
  .bl_ok:
	push	eax
	push	ebx	      ;store x1, x2
	cmp	.x1,SIZE_X
	jge	.bl_end
	cmp	.x2,0
	jle	.bl_end

	mov	ebx,.x2
	sub	ebx,.x1

if Ext>=SSE

       sub	 esp,28
       cvtsi2ss  xmm3,ebx	     ;rcps
       shufps	 xmm3,xmm3,0

       cvtpi2ps  xmm0,.bx1q ;mm0
       movlhps	 xmm0,xmm0
       cvtpi2ps  xmm0,.ex1q ;mm2
       cvtpi2ps  xmm1,.bx2q ;mm1
       movlhps	 xmm1,xmm1
       cvtpi2ps  xmm1,.ex2q ;mm3
       subps	 xmm1,xmm0

       divps	 xmm1,xmm3

       shufps	 xmm1,xmm1,10110001b
       cvtps2pi  mm0,xmm1	   ; mm0 -> 2 delta dwords
       movhlps	 xmm1,xmm1
       cvtps2pi  mm1,xmm1
       movq	 .deyq,mm0
       movq	 .dbyq,mm1

       movd	 mm2,.z1
       movd	 mm3,.z2

       cvtpi2ps  xmm0,.tx1q ;mm0
       movlhps	 xmm0,xmm0
       cvtpi2ps  xmm0,mm2
       cvtpi2ps  xmm1,.tx2q ;mm1
       movlhps	 xmm1,xmm1
       cvtpi2ps  xmm1,mm3
       subps	 xmm1,xmm0

       divps	 xmm1,xmm3

       shufps	 xmm1,xmm1,10110100b
       cvtps2pi  mm0,xmm1	   ; mm0 -> 2 delta dwords
       movhlps	 xmm1,xmm1
       cvtps2pi  mm1,xmm1
       movd	 .dz,mm0
       movq	 .dtyq,mm1

else

	mov	eax,.bx2       ; calc .dbx
	sub	eax,.bx1
	cdq
	idiv	ebx
	push	eax

	mov	eax,.by2       ; calc .dby
	sub	eax,.by1
	cdq
	idiv	ebx
	push	eax

	mov	eax,.ex2       ; calc .dex
	sub	eax,.ex1
	cdq
	idiv	ebx
	push	eax

	mov	eax,.ey2       ; calc .dey
	sub	eax,.ey1
	cdq
	idiv	ebx
	push	eax


	mov	eax,.z2        ; calc .dz
	sub	eax,.z1
	cdq
	idiv	ebx
	push	eax

	mov	eax,.tx2       ; calc .dtx
	sub	eax,.tx1
	cdq
	idiv	ebx
	push	eax

	mov	eax,.ty2       ; calc .dty
	sub	eax,.ty1
	cdq
	idiv	ebx
	push	eax

end if
	cmp	.x1,0	      ; set correctly begin variable
	jge	@f	      ; CLIPPING ON FUNCTION
			      ; cutting triangle exceedes screen
	mov	ebx,.x1
	neg	ebx
	imul	ebx	      ; eax = .dz * abs(.x1)
	add	.z1,eax
	mov	.x1,0

	mov	eax,.dbx
	imul	ebx
	add    .bx1,eax

	mov	eax,.dby
	imul	ebx
	add	.by1,eax

	mov	eax,.dex
	imul	ebx
	add	.ex1,eax

	mov	eax,.dey
	imul	ebx
	add	.ey1,eax

	mov	eax,.dtx
	imul	ebx
	add	.tx1,eax

	mov	eax,.dty
	imul	ebx
	add	.ty1,eax

      @@:
	cmp	.x2,SIZE_X
	jl	@f
	mov	.x2,SIZE_X
      @@:
	mov	eax,SIZE_X	 ;calc memory begin in buffers
	mul	.y
	add	eax,.x1
	lea	esi,[4*eax]
	add	esi,.z_buff	  ; z-buffer filled with dd variables
	lea	eax,[eax*3]
	add	edi,eax


	mov	ecx,.x2
	sub	ecx,.x1
	; init current variables
	push	.bx1   ; current b, e and t shifted shl ROUND   .cbx
	push	.by1					     ;  .cby
	push	.ex1					     ;  .cex
	push	.ey1					     ;  .cey

	push	.z1    ; current z shl CATMULL_SHIFT         ; .cz
	push	esi					     ; .czbuff

	push	.tx1	  ;         .ctx
	push	.ty1	  ;         .cty
	push	edi	  ;         .c_scr
if Ext>=MMX
	movq	mm7,.ctyq
	movq	mm6,.cbyq
	movq	mm5,.ceyq
;        movq    mm4,.dtyq
;        movq    mm3,.dbyq
end if

     .draw:
    ; if TEX = SHIFTING   ;bump drawing only in shifting mode
	mov	esi,.czbuff	 ; .czbuff current address in buffer
	mov	ebx,.cz 	 ; .cz - cur z position
	cmp	ebx,dword[esi]
	jge	.skip
if Ext=NON
	mov	eax,.cby
	shr	eax,ROUND
	mov	esi,.cbx
	shr	esi,ROUND
else
	movq	mm1,mm6
	psrld	mm1,ROUND
	movd	eax,mm1
	psrlq	mm1,32
	movd	esi,mm1
end if



	shl	eax,TEX_SHIFT
	add	esi,eax 	;-  ; esi - current bump map index

	mov	ebx,esi
	dec	ebx
	and	ebx,TEXTURE_SIZE
	add	ebx,.bmap
	movzx	eax,byte [ebx]

	mov	ebx,esi
	inc	ebx
	and	ebx,TEXTURE_SIZE
	add	ebx,.bmap
	movzx	ebx,byte [ebx]
	sub	eax,ebx

	mov	ebx,esi
	sub	ebx,TEX_X
	and	ebx,TEXTURE_SIZE
	add	ebx,.bmap
	movzx	edx,byte [ebx]

	mov	ebx,esi
	add	ebx,TEX_X
	and	ebx,TEXTURE_SIZE
	add	ebx,.bmap
	movzx	ebx,byte [ebx]
	sub	edx,ebx

     ;  eax - horizontal sub    modificated x coord
     ;  edx - vertical   sub    modificated y coord
if Ext=NON
	mov	ebx,.cex       ;.cex - current env map X
	shr	ebx,ROUND
	add	eax,ebx


	mov	ebx,.cey       ;.cey - current  env map y
	shr	ebx,ROUND
	add	edx,ebx

else
	movq	mm1,mm5        ; mm5 - copy of cur env coords
	psrld	mm1,ROUND
	movd	ebx,mm1
	psrlq	mm1,32
	add	eax,ebx
	movd	ebx,mm1
	add	edx,ebx
;        movq    qword[.temp1],mm3
;        add     eax,dword [.temp1]
;        add     edx,dword [.temp1+4]
end if

	or	eax,eax
	jl	.black
	cmp	eax,TEX_X
	jg	.black
	or	edx,edx
	jl	.black
	cmp	edx,TEX_Y
	jg	.black

	shl	edx,TEX_SHIFT	; zaburzenie w emapie = zaburzenie w teksturze
	add	edx,eax 	; proponuje nie stawiac czarnego pixela tylko
	lea	esi,[edx*3]	; niezaburzony.
	add	esi,.emap	;
	lodsd

if Ext=NON
	mov	edx,.cty
	shr	edx,ROUND  ; sar

	mov	edi,.ctx
	shr	edi,ROUND  ; sar
else
	movq	mm1,mm7
	psrld	mm1,ROUND
	movd	edx,mm1
	psrlq	mm1,32
	movd	edi,mm1

end if

	shl	edx,TEX_SHIFT
	add	edi,edx
	and	edi,TEXTURE_SIZE
	lea	esi,[edi*3]
	add	esi,.tex_map

if Ext=NON
	mov	edx,eax
	lodsd
	push	ax
	mul	dl
	mov	dl,ah
	pop	ax
	shr	ax,8
	mul	dh
	mov	al,dl
	mov	edi,.c_scr
	stosw
	shr	edx,16
	shr	eax,16
	mul	dl
	shr	ax,8
	stosb
else
	movd	   mm0,eax
	pxor	   mm1,mm1
	punpcklbw  mm0,mm1
	movd	   mm2,[esi]
	punpcklbw  mm2,mm1
	pmullw	   mm0,mm2
	psrlw	   mm0,8
	packuswb   mm0,mm1
	mov	   edi,.c_scr
	movd	   [edi],mm0

end if

	jmp	.actual_zbuff	; actualize z buffer
     @@:
     .black:
	xor	eax,eax
	mov	edi,.c_scr
	stosd
     .actual_zbuff:
	mov	eax,.cz
	mov	edi,.czbuff
	stosd

      .skip:
	add	.czbuff,4
	add	.c_scr,3

if Ext=NON
	mov	eax,.dbx
	add	.cbx,eax
	mov	ebx,.dby
	add	.cby,ebx

	mov	edx,.dex
	add	.cex,edx
	mov	eax,.dey
	add	.cey,eax

	mov	ebx,.dtx
	add	.ctx,ebx
	mov	edx,.dty
	add	.cty,edx

else
	paddd	mm7,.dtyq
	paddd	mm6,.dbyq
	paddd	mm5,.deyq
end if
	mov	eax,.dz
	add	.cz,eax

	dec	ecx
	jnz	.draw

  .bl_end:
	mov	esp,ebp
ret 76
;Ext = MMX

;     else
;        movq    mm5, qword[.temp1]  ;-
;        paddd   mm5, qword[.temp5]  ; .temp5 == low dword = TEX_X, high dword = -TEX_X
;        pand    mm5, qword[.temp3]  ; .temp3 == low = high dword = TEX_SIZE
;        paddd   mm5, qword[.temp4]  ; .temp4 == low = high dword = offset .bmap
;        movd    ebx,mm5
;        psrlq   mm5,32
;     end if
