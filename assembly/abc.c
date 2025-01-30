.thumb
.org 0x08070000  @ Replace existing rival assignment
bl ChooseRivalBasedOnGender
nop              @ Padding

.org 0x080F0000  @ Free space in the ROM
.global ChooseRivalBasedOnGender
ChooseRivalBasedOnGender:
    push {r0, r1, r2, lr}      @ Save registers
    
    ldr r0, =0x02024588        @ Load gSaveBlock2 base address
    ldrb r1, [r0, #0x0]        @ Load player's gender (0 = male, 1 = female)

    cmp r1, #0                 @ Compare gender
    beq MaleChosen             @ If male, branch to MaleChosen

FemaleChosen:
    ldr r2, =0x020386AE        @ Rival Trainer ID location
    mov r1, #0x1A3             @ Female rival ID (Leaf, new trainer)
    strh r1, [r2]              @ Store female rival ID
    b EndFunction

MaleChosen:
    ldr r2, =0x020386AE        @ Rival Trainer ID location
    mov r1, #0xB4              @ Default male rival ID (Blue)
    strh r1, [r2]              @ Store male rival ID

EndFunction:
    pop {r0, r1, r2, lr}       @ Restore registers
    bx lr                      @ Return
