.thumb
.text
.align 2

.include "../asm_defines.s"
.global StartCB_HandleInput_Hook

StartCB_HandleInput_Hook:
    ldr r3, =0x0806F280 |1
    bx r3