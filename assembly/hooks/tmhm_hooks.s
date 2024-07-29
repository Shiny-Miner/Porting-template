.text
.align 2
.thumb
.thumb_func

.include "../asm_defines.s"




@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ TM/HM Expansion - Party Pokemon sprites
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.pool
@0x8131B94 with r0
CreateTMCaseSpriteHook:
	bl CreateTMCaseSprite
	ldr r1, =0x8131BE2 | 1
bxr1:
	bx r1

.pool
@0x8131E5C with r0
TMCaseMonIconPalChangeHook:
	mov r0, r5 @Item Id
	bl ChangeMonIconPalsInTMCase
	pop {r4-r6, pc}
