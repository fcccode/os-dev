; <--- description --->
; compiler:     FASM 1.50
; name:         Display Test
; version:      0.4
; author:       barsuk


; <--- include all MeOS stuff --->
include "lang.inc"
include "..\..\MACROS.INC"

; <--- start of MenuetOS application --->
MEOS_APP_START

include "debug.inc"


; <--- start of code --->
CODE
	mov     eax, 37
	mov     ebx, 4
	mov     edx, 2
	mov     ecx, cursor
	int     0x40
	or      eax, eax
	jz      exit
	mov     [cursorID], eax

	call	draw_window		  ; at first create and draw the window

  wait_event:				  ; main cycle
	xor	ebx, ebx
	mov	eax, 10
	int	0x40

	cmp	eax, 1			  ;   if event == 1
	je	 redraw 		  ;     jump to redraw handler
	cmp	eax, 2			  ;   else if event == 2
	je	 key				;       jump to key handler
	cmp	eax, 3			  ;   else if event == 3
	je	 button 		  ;     jump to button handler

	jmp	wait_event		   ;   else return to the start of main cycle


  redraw:					 ; redraw event handler
	call	draw_window
	jmp	wait_event


  key:						; key event handler
	mov	eax, 2			  ;   get key code
	int	0x40

	cmp	ah, 27
	jz	exit

	cmp	ah, 0x20
	jz	next_test

	cmp	ah, 179 		; ->
	jz	next_test

	cmp	ah, 176 		; <-
	jz	prev_test

	cmp	ah, 'i'
	jz	toggle_info

	cmp	ah, 'I' 		; � ���� � 祫� ������� ))
	jz	toggle_info

	cmp	ah, 'c'
	jz	toggle_cursor

	cmp	ah, 'C'
	jz	toggle_cursor

	cmp	ah, 'd'
	jz	redraw

	cmp	ah, 'D'
	jz	redraw

	jmp	wait_event

next_test:
	cmp	dword [test_done], 1
	jz	wait_event

	inc	dword [test_num]
	call	draw_window
	jmp	wait_event

prev_test:
	cmp	dword [test_num], ebx
	jz	wait_event

	dec	dword [test_num]
	mov	dword [test_done], ebx
	call	draw_window
	jmp	wait_event


toggle_info:
	xor	dword [show_info], 1
	call	draw_window
	jmp	wait_event

toggle_cursor:
	mov	eax, cursorVisible
	cmp	dword [eax], 0
	jz	.no_cursor

	mov	dword [eax], 0
	mov	ecx, [cursorID]
	jmp	.set
.no_cursor:
	mov	dword [eax], 1
	xor	ecx, ecx
.set:
	mov	eax, 37
	mov	ebx, 5
	int	0x40
	mov	eax, 18
	mov	ebx, 15
	int	0x40			; �⮡� ���������
	jmp	wait_event

button: 				 ; button event handler
	mov	eax, 17 		 ;   get button identifier
	int	0x40

	cmp	ah, 1
	jne	wait_event		   ;   return if button id != 1

exit:
	or	 eax, -1			 ;   exit application
	int	0x40


draw_window:
	mov	eax, 12 		 ; start drawing
	mov	ebx, 1
	int	0x40

	; �����
	;mov     eax, 37
	;mov     ebx, 5
	;mov     ecx, cursorID
	;int     0x40

	mov	eax, 14 		; screen size
	int	0x40
	mov	ebx, eax
	shr	ebx, 16
	mov	ecx, eax
	and	ecx, 0xffff
	mov	[screenx], ebx
	mov	[screeny], ecx

	inc	ebx
	inc	ecx
	mov	eax, 0			  ; create and draw the window
	mov	edx, 0x01000000
	mov	esi, edx
	int	0x40

	; ����� �࠭��� ����

	mov	eax, 13 		; ��㡮 ⠪
	xor	edx, edx
	int	0x40
	dec	ebx
	dec	ecx

	mov	eax, [test_num]
	mov	eax, [test_proc + eax*4]
	or	eax, eax
	jz	end_of_test
	call	eax
	jmp	exit_draw
end_of_test:
	mov	eax, 4
	mov	ebx, 8 * 65536 + 8
	mov	ecx, 0xffffff
	mov	edx, test_finish
	mov	esi, test_finish.size
	int	0x40
	mov	dword [test_done], 1
	jmp	no_info

exit_draw:

	cmp	dword [show_info], 1
	jnz	no_info

; ᭮�� ࠧ���� ��࠭�
	mov	ebx, [screenx]
	mov	ecx, [screeny]

; ᣥ����� ����� � ࠧ�襭��� ��࠭�. �� �㦭�, ��⮬� �� ���

	; ��אַ㣮�쭨� 200�40 � ��䮩
	mov	edx, 200
	mov	eax, 13
	sub	ebx, edx
	shl	ebx, 15
	mov	bx, dx
	mov	edx, 40
	sub	ecx, edx
	shl	ecx, 15
	mov	cx, dx
	mov	edx, 0xffffff
	int	0x40
	xor	edx, edx
	add	ebx, 0x0000fffe 	; �祭� 㤮��� :))))
	add	ecx, 0x0000fffe
	int	0x40


; ⥪��
	mov	eax, 4
	shr	ecx, 16
	mov	bx, cx
	add	ebx, 0x00010001
	mov	ecx, 0x80ffffff
	mov	edx, [test_num]
	mov	edx, [test_info + edx*4]
	int	0x40
	add	ebx, 8
	mov	edx, press_space
	int	0x40
	add	ebx, 8
	mov	edx, press_i
	int	0x40
	add	ebx, 8
	mov	edx, press_c
	int	0x40

no_info:
	mov	eax, 12 		 ; finish drawing
	mov	ebx, 2
	int	0x40

  ret

; <---- procedures for various tests of display ----->
; in: ebx = screen_width, ecx = screen_height

lsz i_image_size, ru, "Image Size and Placement"
db	0
t_image_size:
	mov	eax, 38
	mov	esi, ebx
	mov	edi, ecx
	mov	edx, 0xffffff
; 6 ��१���
	xor	ecx, ecx
	int	0x40
	mov	ecx, edi
	shl	ecx, 16
	xor	ebx, ebx
	int	0x40
	mov	ebx, esi
	shl	ebx, 16
	add	ecx, edi
	int	0x40
	sub	ecx, edi
	add	ebx, esi
	int	0x40
; ࠬ�� ��⮢�
	mov	ebx, esi
	shl	ebx, 16
	mov	ecx, edi
	shl	ecx, 15
	mov	cx, di
	shr	cx, 1
	int	0x40
	shr	ebx, 1
	mov	bx, si
	shr	bx, 1
	mov	ecx, edi
	shl	ecx, 16
	int	0x40

	ret

lsz i_grid, ru, "Grid"
db	0
t_grid:
;       �⪠ ࠧ��஬ � 64 ���ᥫ�
	mov	eax, 38
	inc	ebx
	inc	ecx
	mov	esi, ebx
	mov	edi, ecx
	mov	edx, 0xffffff
	mov	ebp, 0x00400040
;       ��ਧ��⠫�� �����
	shl	ebx, 16
	xor	ecx, ecx
grid_next_y:
	int	0x40
	add	ecx, ebp
	cmp	cx, di
	jnae	grid_next_y
	sub	ecx, 0x00010001
	int	0x40
;       ���⨪���� �����
	mov	ecx, edi
	shl	ecx, 16
	xor	ebx, ebx
grid_next_x:
	int	0x40
	add	ebx, ebp
	cmp	bx, si
	jnae	grid_next_x
	sub	ebx, 0x00010001
	int	0x40

	ret

lsz i_horstr, ru, "Horizontal Straightness"
db	0
t_horstr:
	mov	eax, 38
	mov	edi, ecx
	mov	edx, 0xffffff
	mov	esi, ecx
	inc	esi
	shr	esi, 3
	mov	ebp, esi
	shl	ebp, 16
	mov	bp, si
;       ��ਧ��⠫�� �����
	shl	ebx, 16
	mov	ecx, ebp
	shr	ecx, 1
	mov	cx, bp
	shr	cx, 1
hor_next_y:
	int	0x40
	add	ecx, ebp
	cmp	cx, di
	jnae	hor_next_y
	ret
lsz i_vertstr, ru, "Vertical Straightness",0
db	0
t_vertstr:
	mov	eax, 38
	mov	edx, 0xffffff
	mov	esi, ebx
	shl	ecx, 16
	mov	edi, esi
	shr	edi, 3
	mov	ebp, edi
	shl	ebp, 16
	mov	bp, di
	mov	ebx, ebp
	shr	ebx, 1
	mov	bx, bp
	shr	bx, 1
vert_next_x:
	int	0x40
	add	ebx, ebp
	cmp	bx, si
	jnae	vert_next_x

	ret

lsz i_distort, ru, "Distortion",0
db	0
t_distort:
	mov	edx, 0xffffff
	mov	esi, ebx
	mov	edi, ecx
	mov	ebp, 3

	xor	ebx, ebx
dist_next:
	mov	eax, 38
	push	ebp
	mov	ebp, ebx
	shl	ebx, 16
	or	ebx, ebp

	mov	ecx, edi
	shl	ecx, 16
	or	ecx, ebp
	int	0x40

	mov	ebx, esi
	shl	ebx, 16
	mov	bx, si
	int	0x40

	mov	bx, bp
	mov	ecx, ebp
	shl	ecx, 16
	or	ecx, ebp
	int	0x40

	mov	ecx, edi
	shl	ecx, 16
	mov	cx, di
	int	0x40

	mov	eax, 30
	sub	esi, eax
	sub	edi, eax
	mov	ebx, ebp
	add	ebx, eax
	pop	ebp
	dec	ebp
	jnz	dist_next

	ret

lsz i_horres, ru, "Horizontal Resolution",0
db	0
t_horres:
	mov	eax, 38
	mov	edx, 0xffffff
	mov	edi, ecx
	shl	ecx, 16
	mov	esi, ebx
	xor	ebx, ebx
	mov	edi, 0x003A003A
	mov	ebp, 0x00030003

horres_next:
	add	ebx, edi
	int	0x40
	add	ebx, ebp
	int	0x40
	add	ebx, ebp
	int	0x40
	add	ebx, ebp
	int	0x40
	add	ebx, ebp
	int	0x40
	cmp	bx, si
	jna	horres_next

	ret

lsz i_vertres, ru, "Vertical Resolution",0
db	0
t_vertres:
	mov	eax, 38
	mov	edx, 0xffffff
;       mov     esi, ebx
	shl	ebx, 16
	mov	edi, ecx
	xor	ecx, ecx
	mov	ebp, 0x00030003
	mov	esi, 0x002A002A

vertres_next:
	add	ecx, esi
	int	0x40
	add	ecx, ebp
	int	0x40
	add	ecx, ebp
	int	0x40
	add	ecx, ebp
	int	0x40
	add	ecx, ebp
	int	0x40
	add	ecx, 0x00540054
	cmp	cx, di
	jna	vertres_next

	ret

lsz i_moire, ru, "Moire Patterns",0
db	0
t_moire:
	mov	eax, 38
	mov	edx, 0xffffff
	mov	edi, ecx
	shl	ecx, 16
	mov	esi, ebx
	xor	ebx, ebx
	mov	ebp, 0x00030003
moire_next:
	int	0x40
	add	ebx, ebp
	cmp	bx, si
	jna	moire_next

	ret

lsz i_revsharp, ru, "Reverse Video Sharpness",0
db	0
t_revsharp:
	mov	esi, ebx
	mov	edi, ecx
	shr	ecx, 1
	mov	eax, 13
	mov	edx, 0xffffff
	int	0x40
; � ⥯��� - ������� �����
	mov	eax, 38
	mov	ecx, edi
	mov	edx, 0x01000000
	xor	ebx, ebx
	mov	ebp, 0x00010001
	mov	edi, 0x003F003F
revsharp_next:
	add	ebx, edi
	int	0x40
	add	ebx, ebp
	int	0x40
	add	ebx, ebp
	int	0x40
	add	ebx, edi
	sub	ebx, ebp
	int	0x40
	cmp	bx, si
	jna	revsharp_next

	ret

lsz i_flicker, ru, "Flicker Severity",0
db	0
t_flicker:
	mov	eax, 13
	mov	edx, 0xffffff
	int	0x40
	ret

lsz i_glare, ru, "Glare Severity",0
db	0
t_glare:		; ��⨬���஢��� ��祣�
	ret

lsz i_interlace, ru, "Interlacing Detection",0
db	0
t_interlace:
	mov	edi, ecx
	mov	eax, 38
	mov	edx, 0xffffff
	xor	ecx, ecx
	mov	ebp, 0x00020002
interlace_next:
	add	ecx, ebp
	int	0x40
	cmp	cx, di
	jna	interlace_next

	ret

lsz i_scrreg, ru, "Screen Regulation",0
db	0
t_scrreg:
	mov	eax, 13
	mov	edx, 0xffffff
	add	ebx, 0x0018FFCD ; +25 � ��砫� � -50 �� �����
	shr	ecx, 1
	add	ecx, 0x0013FFEC ; +19 � ��砫� � -19 �� �����
	int	0x40
	ret

lsz i_pricol, ru, "Primary Color Purity"
db	0
t_pricol:

	mov	esi, ebx
	mov	edi, ecx
	mov	eax, 13

	shr	ebx, 4	; /16
	mov	ebp, ebx
	shl	ebx, 16
	mov	bx, bp
	shl	ebp, 16
	lea	ebp, [ebp + ebp * 4]		; ebp *= 5

	mov	ecx, 0x00280000
	mov	cx, di
	sub	cx, 80
	;shr     cx, 1

	shl	bx, 2
	mov	edx, 0xff0000
	int	0x40

	add	ebx, ebp
	shr	edx, 8
	int	0x40

	add	ebx, ebp
	shr	edx, 8
	int	0x40

	ret

lsz i_colint, ru, "Color Intensity Gradient"
db	0
t_colint:

	mov	esi, ebx
	mov	edi, ecx

;        mov     eax, ecx
;        shr     ecx, 2          ; end y coord
;        and     ecx, 0xffffff80         ; �� not 7F
;        shr     eax, 7                  ; / 128
;        mov     ebp, eax
;        mov     edx, eax
;        lea     eax, [eax + eax * 2]    ; eax *= 5
;        shl     ebp, 4
;        add     eax, ebp

;        shl     eax, 16
;        add     ecx, eax
;        mov     edx, ebp
;        shl     ebp, 16
;        mov     bp, dx          ; �� ���쭮�

	; � �� ����, �� ⠬ ��������, � �訫 ������� ᭮�� �_�

	; ���� ����� ᣥ����� ecx (��砫�� ᤢ��) � ebp (蠣 �� �)

	mov	eax, edi
	lea	eax, [eax + 2 * eax]
	shr	eax, 5			; eax = 3/32 �����
	mov	ebp, eax
	shl	ebp, 16
	mov	bp, ax			; ebp = ax � ����� ᫮���

	mov	ebx, eax		; ��࠭�� �� ���祭��

	mov	eax, edi
	inc	eax
	shr	eax, 4		; 3/16 ����� - ��砫쭮� ���祭��
				; �ᥣ� ������ ������ 3/4 �����, �⮣� �� 3/32 ����� �� ������ (��� ஢���� ���)
	lea	eax, [eax + eax * 2]
	mov	ecx, eax
	shl	ecx, 16
	shr	ebx, 2
	lea	ebx, [ebx + ebx * 2]	; ebx = 3/4 ebx, �.�. 3/4 ����� ������
	add	eax, ebx
	mov	cx, ax

	xor	edx, edx
	mov	eax, 0xffff
	div	esi
	mov	edi, eax	; edi = 64K/width

	mov	dword [color_index], 0
	jmp	colint_next

color_table	dd	0x00ff0000, 0x0000ff00, 0x00ffff00, \
			0x000000ff, 0x00ff00ff, 0x0000ffff, 0x00ffffff
color_index	dd	0

colint_next:
	xor	edx, edx
	xor	ebx, ebx
	mov	eax, 38
colint_next_line:
	push	edx
	push	eax
	movzx	eax, dh
	shl	eax, 16
	mov	dl, dh
	or	edx, eax
	mov	eax, [color_index]
	mov	eax, [color_table + eax * 4]
	and	edx, eax
	pop	eax
	int	0x40
	pop	edx
	add	ebx, 0x00010001
	add	edx, edi
	cmp	bx, si
	jna	colint_next_line

	add	ecx, ebp
	inc	dword [color_index]
	cmp	dword [color_index], 7
	jb	colint_next

	ret

lsz i_colalign, ru, "Color Alignment Grid"
db	0
t_colalign:

; ��᭠� �⪠
	inc	ebx		; ⠪ �㦭�
	inc	ecx
	mov	esi, ebx
	mov	edi, ecx
	mov	edx, 0xff0000
;       ��ਧ��⠫�� �����
	shl	ebx, 16
	xor	ecx, ecx
	push	edi
	shr	edi, 3
	mov	ebp, edi
	shl	ebp, 16
	mov	bp, di
	pop	edi
	mov	[yshift], ebp
	mov	eax, 38
cgrid_next_y:
	int	0x40
	add	ecx, ebp
	cmp	cx, di
	jnae	cgrid_next_y
	; ��᫥���� �����:
	sub	ecx, 0x00010001
	int	0x40

;       ���⨪���� �����
	mov	ecx, edi
	shl	ecx, 16
	xor	ebx, ebx
	push	esi
	shr	esi, 3
	mov	ebp, esi
	shl	ebp, 16
	mov	bp, si
	mov	[xshift], ebp
	pop	esi
	mov	eax, 38
cgrid_next_x:
	int	0x40
	add	ebx, ebp
	cmp	bx, si
	jnae	cgrid_next_x
	; ��᫥���� �����
	sub	ebx, 0x00010001
	int	0x40
	jmp	cgrid_green

	xshift	dd	0
	yshift	dd	0
	shift	dd	0
cgrid_green:
; ������ �����: ��ਧ��⠫��
	mov	edx, esi
	shr	edx, 5
	lea	eax, [edx + edx * 2]
	shl	edx, 16
	or	edx, eax
	mov	[shift], edx
	mov	eax, 38
	mov	edx, 0x00ff00
	xor	ecx, ecx
	mov	ebp, [xshift]
ggrid_next_yy:
	mov	ebx, [shift]
ggrid_next_yx:
	int	0x40
	add	ebx, ebp
	cmp	bx, si
	jnae	ggrid_next_yx
	sub	ebx, 0x00010001
	int	0x40

	add	ecx, [yshift]
	cmp	cx, di
	jnae	ggrid_next_yy
	; last row of lines
	mov	ebx, [shift]
	dec	ecx
ggrid_last_yx:
	int	0x40
	add	ebx, ebp
	cmp	bx, si
	jnae	ggrid_last_yx

; � ���⨪����
	mov	edx, edi
	shr	edx, 5
	lea	eax, [edx + edx * 2]
	shl	edx, 16
	or	edx, eax
	mov	[shift], edx

	mov	eax, 38
	mov	edx, 0x00ff00
	mov	ecx, [shift]
	mov	ebp, [xshift]
ggrid_next_xy:
	xor	ebx, ebx
ggrid_next_xx:
	int	0x40
	add	ebx, ebp
	cmp	bx, si
	jnae	ggrid_next_xx
	sub	ebx, 0x00010001
	int	0x40

	add	ecx, [yshift]
	cmp	cx, di
	jnae	ggrid_next_xy
	xor	ebx, ebx
	dec	ecx
ggrid_last_xy:
	;int     0x40
	;add     ebx, ebp
	;cmp     bx, si
	;jnae    ggrid_last_xy



	ret

lsz i_colsyn, ru, "Color Synchronization"
db	0
t_colsyn:

	inc	ebx
	inc	ecx
	mov	esi, ebx
	mov	edi, ecx

	shr	ebx, 5
	mov	eax, ebx
	lea	ebx, [ebx + ebx * 4]
	shl	ebx, 1			; 10/32
	mov	ebp, ebx
	shl	eax, 16
	or	ebx, eax
	shl	ebp, 16

	mov	edi, 0x0000ffff
	add	ecx, 0x003FFF7F
	mov	edx, edi
	mov	eax, 13
	int	0x40

	mov	edx, 0x00ff0000
	add	ebx, ebp
	int	0x40

	mov	edx, edi
	add	ebx, ebp
	int	0x40

	ret

; <--- initialised data --->
DATA

	screenx 	dd	0
	screeny 	dd	0

	test_num	dd	0
	test_done	dd	0
	show_info	dd	1
	test_proc	dd	t_image_size, t_grid, t_horstr, t_vertstr,\
		t_distort, t_horres, t_vertres, t_moire, t_revsharp, \
		t_flicker, t_glare, t_interlace, t_scrreg, t_pricol, \
		t_colint, t_colalign, t_colsyn, 0
	test_info	dd	i_image_size, i_grid, i_horstr, i_vertstr, \
		i_distort, i_horres, i_vertres, i_moire, i_revsharp, \
		i_flicker, i_glare, i_interlace, i_scrreg, i_pricol, \
		i_colint, i_colalign, i_colsyn, 0

	lsz press_space, ru, "������ �஡�� ��� �த�������,"
	db	0
	lsz press_i, ru, "I ��� ��४��祭�� ᢥ�����,"
	db	0
	lsz press_c, ru, "� C ��� ��४��祭�� �����"
	db	0
	lsz header, ru, "���� ������"
	lsz test_finish, ru, "����� ���. ������ ESC."

	cursor  dd 32*32 dup(0x00000000)	; �� ࠢ�� ᮦ�����

	cursorVisible	dd	1
	cursorID	dd	0


; <--- uninitialised data --->
UDATA


MEOS_APP_END
; <--- end of MenuetOS application --->