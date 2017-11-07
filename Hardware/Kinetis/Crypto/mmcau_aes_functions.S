#*******************************************************************************
#
# Copyright (c) Freescale Semiconductor, Inc 2011.
#
# FILE NAME      : mmcau_aes_functions.s
# VERSION        : $Id: mmcau_aes_functions.s.rca 1.4 Thu Nov 21 14:17:01 2013 b40907 Experimental $
# TYPE           : Source Cortex-Mx assembly library code
# DEPARTMENT     : MSG R&D Core and Platforms
# AUTHOR         : David Schimke
# AUTHOR'S EMAIL : David.Schimke@freescale.com
# AUTHOR         : Anthony (Teejay) Ciancio
# AUTHOR'S EMAIL : teejay.ciancio@freescale.com
# -----------------------------------------------------------------------------
# Release history
# VERSION  Date       AUTHOR           DESCRIPTION
#          08-2010    David Schimke    Initial Release
#          12-2010    David Schimke    Remove "global" on data objects
#          01-2011    David Schimke    Add byte reverse to correct double word
#                                      read of byte arrays for little endian,
#                                      header added
#          11-2013    Teejay Ciancio   Small performance improvements to
#                                      set_key and decrypt; also, some cleanup
#          20.01.2017 Removed cau2_defines.hdr and inserted defines locally {uTasker}
#                     Note also that this will not assemble for cortex-m0 core
#
#*******************************************************************************
#*******************************************************************************


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
# Copyright (c) Freescale Semiconductor, Inc 2013.
#
# FILE NAME         : mmcau_aes_functions_M0.S
# VERSION           : $Id:  $
# TYPE              : Source Cortex-M0+ assembly library code
# DEPARTMENT        : MCG R&D Core and Platforms
# AUTHOR            : Anthony (Teejay) Ciancio
# AUTHOR EMAIL      : teejay.ciancio@freescale.com
#
#  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #
#
# VERSION   DATE        AUTHOR          DESCRIPTION
# *******   ****        ******          ***********
# 1.0       2013-11     Ciancio         initial release, using the ARMv6-M ISA
#           20.01.2017  Removed cau2_defines.hdr and inserted defines locally {uTasker}
#                       Note also that this will not assemble for cortex-m4 core
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#if !defined (ARM_MATH_CM0PLUS)
    #.include "cau2_defines.hdr"                                         {uTasker} Cortex-M4 code
    .equ    MMCAU_PPB_DIRECT,  0xe0081000
    .equ    MMCAU_PPB_INDIRECT,0xe0081800
    .equ    MMCAU_1_CMD,       0x80000000
    .equ    MMCAU_2_CMDS,      0x80100000
    .equ    MMCAU_3_CMDS,      0x80100200

    .equ    TL,0
    .equ    TS,0
    .equ    CASR,0
    .equ    CAA,1
    .equ    CA0,2
    .equ    CA1,3
    .equ    CA2,4
    .equ    CA3,5
    .equ    CA4,6
    .equ    CA5,7
    .equ    CA6,8
    .equ    CA7,9
    .equ    CA8,10
    .equ    CNOP,0x000
    .equ    LDR,0x010
    .equ    STR,0x020
    .equ    ADR,0x030
    .equ    RADR,0x040
    .equ    ADRA,0x050
    .equ    XOR,0x060
    .equ    ROTL,0x070
    .equ    MVRA,0x080
    .equ    MVAR,0x090
    .equ    AESS,0x0a0
    .equ    AESIS,0x0b0
    .equ    AESC,0x0c0
    .equ    AESIC,0x0d0
    .equ    AESR,0x0e0
    .equ    AESIR,0x0f0
    .equ    DESR,0x100
    .equ    DESK,0x110
    .equ    HASH,0x120
    .equ    SHS,0x130
    .equ    MDS,0x140
    .equ    SHS2,0x150
    .equ    ILL,0x1f0
    .equ    IP,8
    .equ    FP,4
    .equ    DC,1
    .equ    CP,2
    .equ    KSL1,0
    .equ    KSL2,1
    .equ    KSR1,2
    .equ    KSR2,3
    .equ    HFF,0
    .equ    HFG,1
    .equ    HFH,2
    .equ    HFI,3
    .equ    HFP,2
    .equ    HFC,4
    .equ    HFM,5
    .equ    HF2C,6
    .equ    HF2M,7
    .equ    HF2S,8
    .equ    HF2T,9
    .equ    HF2U,10
    .equ    HF2V,11

    .syntax unified

#*******************************************************************************
#*******************************************************************************
#
# AES: Performs an AES key expansion
#   arguments
#           *key        pointer to input key (128, 192, 256 bits in length)
#           key_size    key_size in bits (128, 192, 256)
#           *key_sch    pointer to key schedule output (44, 52, 60 longwords)
#
#   calling convention
#   void    mmcau_aes_set_key (const unsigned char *key,
#                              const int            key_size,
#                              unsigned char       *key_sch)

#  register allocation
# --------------------
#  r0      = scratch / input  *key (arg0)
#  r1      = scratch / input  size (arg1)
#  r2      = scratch / output *key_sch (arg2)
#  r3      = scratch
#  r4      = scratch
#  r5      = scratch
#  r6      = scratch
#  r7      = scratch
#  r8      = scratch
#  r9      = scratch
# r10 (sl) = scratch / pointer to rcon
# r11 (fp) = scratch / mmcau_1_cmd(AESS+CAA)
# r12 (ip) = scratch / MMCAU_PPB_DIRECT
# r13 (sp) = stack pointer
# r14 (lr) = scratch / link register

    .global _mmcau_aes_set_key
    .global mmcau_aes_set_key
    .type   mmcau_aes_set_key, %function
    .align  4

_mmcau_aes_set_key:
mmcau_aes_set_key:

    stmdb   sp!, {r4-ip,lr}                         @ save registers on stack

# prepare for AES operations register load
    movw    r8, #:lower16:setkey_reg_data
    movt    r8, #:upper16:setkey_reg_data

    ldmia   r0!, {r4-r7}                            @ copy key[0-3]
    rev     r4, r4                                  @ byte reverse
    rev     r5, r5                                  @ byte reverse
    rev     r6, r6                                  @ byte reverse
    rev     r7, r7                                  @ byte reverse
    stmia   r2!, {r4-r7}                            @ to key_sch[0-3]

# load registers needed for mmcau commands from setkey_reg_data:
    ldmia   r8, {sl-ip}                             @ setup AES operations

    cmp     r1, #128                                @ if key_size = 128
    beq     expand_128                              @ then go expand_128
    cmp     r1, #192                                @ else if size = 192
    beq     expand_192                              @ then go expand_192

expand_256:
    ldmia   r0, {r1,r3,r8-r9}                       @ copy key[4-7]
    rev     r1, r1                                  @ byte reverse
    rev     r3, r3                                  @ byte reverse
    rev     r8, r8                                  @ byte reverse
    rev     r9, r9                                  @ byte reverse
    stmia   r2!, {r1,r3,r8-r9}                      @ to key_sch[4-7]


    ldr     r0, [sl], $4                            @ get rcon[0]; sl++
    ror     lr, r9, $24                             @ rotate left by 8
    str     lr, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[7])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     lr, r0                                  @ XOR rcon[0]

# calculation for key_sch[8-11]
    eor     r4, lr                                  @ XOR key_sch[0]
    eor     r5, r4                                  @ key_sch[1]^key_sch[8]
    eor     r6, r5                                  @ key_sch[2]^key_sch[9]
    eor     r7, r6                                  @ key_sch[3]^key_sch[10]
    stmia   r2!, {r4-r7}                            @ store key_sch[8-11]

    str     r7, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[11])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA

# calculation for key_sch[12-15]
    eor     r1, lr                                  @ XOR key_sch[4]
    eor     r3, r1                                  @ key_sch[5]^key_sch[12]
    eor     r8, r3                                  @ key_sch[6]^key_sch[13]
    eor     r9, r8                                  @ key_sch[7]^key_sch[14]
    stmia   r2!, {r1,r3,r8-r9}                      @ store key_sch[12-15]

    ldr     r0, [sl], $4                            @ get rcon[1]; sl++
    ror     lr, r9, $24                             @ rotate left by 8
    str     lr, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[15])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     lr, r0                                  @ XOR rcon[1]

# calculation for key_sch[16-19]
    eor     r4, lr                                  @ XOR key_sch[8]
    eor     r5, r4                                  @ key_sch[9]^key_sch[16]
    eor     r6, r5                                  @ key_sch[10]^key_sch[17]
    eor     r7, r6                                  @ key_sch[11]^key_sch[18]
    stmia   r2!, {r4-r7}                            @ store key_sch[16-19]

    str     r7, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[19])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA

# calculation for key_sch[20-23]
    eor     r1, lr                                  @ XOR key_sch[12]
    eor     r3, r1                                  @ key_sch[13]^key_sch[20]
    eor     r8, r3                                  @ key_sch[14]^key_sch[21]
    eor     r9, r8                                  @ key_sch[15]^key_sch[22]
    stmia   r2!, {r1,r3,r8-r9}                      @ store key_sch[20-23]

    ldr     r0, [sl], $4                            @ get rcon[2]; sl++
    ror     lr, r9, $24                             @ rotate left by 8
    str     lr, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[23])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     lr, r0                                  @ XOR rcon[2]

# calculation for key_sch[24-27]
    eor     r4, lr                                  @ XOR key_sch[16]
    eor     r5, r4                                  @ key_sch[17]^key_sch[24]
    eor     r6, r5                                  @ key_sch[18]^key_sch[25]
    eor     r7, r6                                  @ key_sch[19]^key_sch[26]
    stmia   r2!, {r4-r7}                            @ store key_sch[24-27]

    str     r7, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[27])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA

# calculation for key_sch[28-31]
    eor     r1, lr                                  @ XOR key_sch[20]
    eor     r3, r1                                  @ key_sch[21]^key_sch[28]
    eor     r8, r3                                  @ key_sch[22]^key_sch[29]
    eor     r9, r8                                  @ key_sch[23]^key_sch[30]
    stmia   r2!, {r1,r3,r8-r9}                      @ store key_sch[28-31]

    ldr     r0, [sl], $4                            @ get rcon[3]; sl++
    ror     lr, r9, $24                             @ rotate left by 8
    str     lr, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[31])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     lr, r0                                  @ XOR rcon[3]

# calculation for key_sch[32-35]
    eor     r4, lr                                  @ XOR key_sch[24]
    eor     r5, r4                                  @ key_sch[25]^key_sch[32]
    eor     r6, r5                                  @ key_sch[26]^key_sch[33]
    eor     r7, r6                                  @ key_sch[27]^key_sch[34]
    stmia   r2!, {r4-r7}                            @ store key_sch[32-35]

    str     r7, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[35])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA

# calculation for key_sch[36-39]
    eor     r1, lr                                  @ XOR key_sch[28]
    eor     r3, r1                                  @ key_sch[29]^key_sch[36]
    eor     r8, r3                                  @ key_sch[30]^key_sch[37]
    eor     r9, r8                                  @ key_sch[31]^key_sch[38]
    stmia   r2!, {r1,r3,r8-r9}                      @ store key_sch[36-39]

    ldr     r0, [sl], $4                            @ get rcon[4]; sl++
    ror     lr, r9, $24                             @ rotate left by 8
    str     lr, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[39])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     lr, r0                                  @ XOR rcon[4]

# calculation for key_sch[40-43]
    eor     r4, lr                                  @ XOR key_sch[32]
    eor     r5, r4                                  @ key_sch[33]^key_sch[40]
    eor     r6, r5                                  @ key_sch[34]^key_sch[41]
    eor     r7, r6                                  @ key_sch[35]^key_sch[42]
    stmia   r2!, {r4-r7}                            @ store key_sch[40-43]

    str     r7, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[43])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA

# calculation for key_sch[44-47]
    eor     r1, lr                                  @ XOR key_sch[36]
    eor     r3, r1                                  @ key_sch[37]^key_sch[44]
    eor     r8, r3                                  @ key_sch[38]^key_sch[45]
    eor     r9, r8                                  @ key_sch[39]^key_sch[46]
    stmia   r2!, {r1,r3,r8-r9}                      @ store key_sch[44-47]

    ldr     r0, [sl], $4                            @ get rcon[5]; sl++
    ror     lr, r9, $24                             @ rotate left by 8
    str     lr, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[47])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     lr, r0                                  @ XOR rcon[5]

# calculation for key_sch[48-51]
    eor     r4, lr                                  @ XOR key_sch[40]
    eor     r5, r4                                  @ key_sch[41]^key_sch[48]
    eor     r6, r5                                  @ key_sch[42]^key_sch[49]
    eor     r7, r6                                  @ key_sch[43]^key_sch[50]
    stmia   r2!, {r4-r7}                            @ store key_sch[48-51]

    str     r7, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[51])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA

# calculation for key_sch[52-55]
    eor     r1, lr                                  @ XOR key_sch[44]
    eor     r3, r1                                  @ key_sch[45]^key_sch[52]
    eor     r8, r3                                  @ key_sch[46]^key_sch[53]
    eor     r9, r8                                  @ key_sch[47]^key_sch[54]
    stmia   r2!, {r1,r3,r8-r9}                      @ store key_sch[52-55]

    ldr     r0, [sl], $4                            @ get rcon[6]; sl++
    ror     lr, r9, $24                             @ rotate left by 8
    str     lr, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[55])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     lr, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     lr, r0                                  @ XOR rcon[6]

# calculation for key_sch[56-59]
    eor     r4, lr                                  @ XOR key_sch[48]
    eor     r5, r4                                  @ key_sch[49]^key_sch[56]
    eor     r6, r5                                  @ key_sch[50]^key_sch[57]
    eor     r7, r6                                  @ key_sch[51]^key_sch[58]
    stmia   r2!, {r4-r7}                            @ store key_sch[56-59]

    ldmia   sp!, {r4-ip,pc}                         @ restore regs and return

expand_192:
    ldmia   r0, {r8-r9}                             @ copy key[4-5]
    rev     r8, r8                                  @ byte reverse
    rev     r9, r9                                  @ byte reverse
    stmia   r2!, {r8-r9}                            @ to key_sch[4-5]

    ldr     r0, [sl], $4                            @ get rcon[0]; sl++
    ror     r3, r9, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[5])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[0]

# calculation for key_sch[6-11]
    eor     r4, r3                                  @ XOR key_sch[0]
    eor     r5, r4                                  @ key_sch[1]^key_sch[6]
    eor     r6, r5                                  @ key_sch[2]^key_sch[7]
    eor     r7, r6                                  @ key_sch[3]^key_sch[8]
    eor     r8, r7                                  @ key_sch[4]^key_sch[9]
    eor     r9, r8                                  @ key_sch[5]^key_sch[10]
    stmia   r2!, {r4-r9}                            @ store key_sch[6-11]

    ldr     r0, [sl], $4                            @ get rcon[1]; sl++
    ror     r3, r9, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[11])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[1]

# calculation for key_sch[12-17]
    eor     r4, r3                                  @ XOR key_sch[6]
    eor     r5, r4                                  @ key_sch[7]^key_sch[12]
    eor     r6, r5                                  @ key_sch[8]^key_sch[13]
    eor     r7, r6                                  @ key_sch[9]^key_sch[14]
    eor     r8, r7                                  @ key_sch[10]^key_sch[15]
    eor     r9, r8                                  @ key_sch[11]^key_sch[16]
    stmia   r2!, {r4-r9}                            @ store key_sch[12-17]

    ldr     r0, [sl], $4                            @ get rcon[2]; sl++
    ror     r3, r9, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[17])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[2]

# calculation for key_sch[18-23]
    eor     r4, r3                                  @ XOR key_sch[12]
    eor     r5, r4                                  @ key_sch[13]^key_sch[18]
    eor     r6, r5                                  @ key_sch[14]^key_sch[19]
    eor     r7, r6                                  @ key_sch[15]^key_sch[20]
    eor     r8, r7                                  @ key_sch[16]^key_sch[21]
    eor     r9, r8                                  @ key_sch[17]^key_sch[22]
    stmia   r2!, {r4-r9}                            @ store key_sch[18-23]

    ldr     r0, [sl], $4                            @ get rcon[3]; sl++
    ror     r3, r9, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[23])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[3]

# calculation for key_sch[24-29]
    eor     r4, r3                                  @ XOR key_sch[18]
    eor     r5, r4                                  @ key_sch[19]^key_sch[24]
    eor     r6, r5                                  @ key_sch[20]^key_sch[25]
    eor     r7, r6                                  @ key_sch[21]^key_sch[26]
    eor     r8, r7                                  @ key_sch[22]^key_sch[27]
    eor     r9, r8                                  @ key_sch[23]^key_sch[28]
    stmia   r2!, {r4-r9}                            @ store key_sch[24-29]

    ldr     r0, [sl], $4                            @ get rcon[4]; sl++
    ror     r3, r9, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[29])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[4]

# calculation for key_sch[30-35]
    eor     r4, r3                                  @ XOR key_sch[24]
    eor     r5, r4                                  @ key_sch[25]^key_sch[30]
    eor     r6, r5                                  @ key_sch[26]^key_sch[31]
    eor     r7, r6                                  @ key_sch[27]^key_sch[32]
    eor     r8, r7                                  @ key_sch[28]^key_sch[33]
    eor     r9, r8                                  @ key_sch[29]^key_sch[34]
    stmia   r2!, {r4-r9}                            @ store key_sch[30-35]

    ldr     r0, [sl], $4                            @ get rcon[5]; sl++
    ror     r3, r9, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[35])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[5]

# calculation for key_sch[36-41]
    eor     r4, r3                                  @ XOR key_sch[30]
    eor     r5, r4                                  @ key_sch[31]^key_sch[36]
    eor     r6, r5                                  @ key_sch[32]^key_sch[37]
    eor     r7, r6                                  @ key_sch[33]^key_sch[38]
    eor     r8, r7                                  @ key_sch[34]^key_sch[39]
    eor     r9, r8                                  @ key_sch[35]^key_sch[40]
    stmia   r2!, {r4-r9}                            @ store key_sch[36-41]

    ldr     r0, [sl], $4                            @ get rcon[6]; sl++
    ror     r3, r9, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[41])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[6]

# calculation for key_sch[42-47]
    eor     r4, r3                                  @ XOR key_sch[36]
    eor     r5, r4                                  @ key_sch[37]^key_sch[42]
    eor     r6, r5                                  @ key_sch[38]^key_sch[43]
    eor     r7, r6                                  @ key_sch[39]^key_sch[44]
    eor     r8, r7                                  @ key_sch[40]^key_sch[45]
    eor     r9, r8                                  @ key_sch[41]^key_sch[46]
    stmia   r2!, {r4-r9}                            @ store key_sch[42-47]

    ldr     r0, [sl], $4                            @ get rcon[7]; sl++
    ror     r3, r9, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[47])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[7]

# calculation for key_sch[48-51]
    eor     r4, r3                                  @ XOR key_sch[42]
    eor     r5, r4                                  @ key_sch[43]^key_sch[48]
    eor     r6, r5                                  @ key_sch[44]^key_sch[49]
    eor     r7, r6                                  @ key_sch[45]^key_sch[50]
    stmia   r2!, {r4-r7}                            @ store key_sch[48-51]

    ldmia   sp!, {r4-ip,pc}                         @ restore regs and return

expand_128:
    ldr     r0, [sl]                                @ get rcon[0]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[3])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[0]

# calculation for key_sch[4-7]
    eor     r4, r3                                  @ XOR key_sch[0]
    eor     r5, r4                                  @ key_sch[1]^key_sch[4]
    eor     r6, r5                                  @ key_sch[2]^key_sch[5]
    eor     r7, r6                                  @ key_sch[3]^key_sch[6]
    stmia   r2!, {r4-r7}                            @ store key_sch[4-7]

    ldr     r0, [sl, $1<<2]                         @ get rcon[1]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[7])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[1]

# calculation for key_sch[8-11]
    eor     r4, r3                                  @ XOR key_sch[4]
    eor     r5, r4                                  @ key_sch[5]^key_sch[8]
    eor     r6, r5                                  @ key_sch[6]^key_sch[9]
    eor     r7, r6                                  @ key_sch[7]^key_sch[10]
    stmia   r2!, {r4-r7}                            @ store key_sch[8-11]

    ldr     r0, [sl, $2<<2]                         @ get rcon[2]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[11])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[2]

# calculation for key_sch[12-15]
    eor     r4, r3                                  @ XOR key_sch[8]
    eor     r5, r4                                  @ key_sch[9]^key_sch[12]
    eor     r6, r5                                  @ key_sch[10]^key_sch[13]
    eor     r7, r6                                  @ key_sch[11]^key_sch[14]
    stmia   r2!, {r4-r7}                            @ store key_sch[12-15]

    ldr     r0, [sl, $3<<2]                         @ get rcon[3]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[15])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[3]

# calculation for key_sch[16-19]
    eor     r4, r3                                  @ XOR key_sch[12]
    eor     r5, r4                                  @ key_sch[13]^key_sch[16]
    eor     r6, r5                                  @ key_sch[14]^key_sch[17]
    eor     r7, r6                                  @ key_sch[15]^key_sch[18]
    stmia   r2!, {r4-r7}                            @ store key_sch[16-19]

    ldr     r0, [sl, $4<<2]                         @ get rcon[4]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[19])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[4]

# calculation for key_sch[20-23]
    eor     r4, r3                                  @ XOR key_sch[16]
    eor     r5, r4                                  @ key_sch[17]^key_sch[20]
    eor     r6, r5                                  @ key_sch[18]^key_sch[21]
    eor     r7, r6                                  @ key_sch[19]^key_sch[22]
    stmia   r2!, {r4-r7}                            @ store key_sch[20-23]

    ldr     r0, [sl, $5<<2]                         @ get rcon[5]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[23])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[5]

# calculation for key_sch[24-27]
    eor     r4, r3                                  @ XOR key_sch[20]
    eor     r5, r4                                  @ key_sch[21]^key_sch[24]
    eor     r6, r5                                  @ key_sch[22]^key_sch[25]
    eor     r7, r6                                  @ key_sch[23]^key_sch[26]
    stmia   r2!, {r4-r7}                            @ store key_sch[24-27]

    ldr     r0, [sl, $6<<2]                         @ get rcon[6]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[27])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[6]

# calculation for key_sch[28-31]
    eor     r4, r3                                  @ XOR key_sch[24]
    eor     r5, r4                                  @ key_sch[25]^key_sch[28]
    eor     r6, r5                                  @ key_sch[26]^key_sch[29]
    eor     r7, r6                                  @ key_sch[27]^key_sch[30]
    stmia   r2!, {r4-r7}                            @ store key_sch[28-31]

    ldr     r0, [sl, $7<<2]                         @ get rcon[7]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[31])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[7]

# calculation for key_sch[32-35]
    eor     r4, r3                                  @ XOR key_sch[28]
    eor     r5, r4                                  @ key_sch[29]^key_sch[32]
    eor     r6, r5                                  @ key_sch[30]^key_sch[33]
    eor     r7, r6                                  @ key_sch[31]^key_sch[34]
    stmia   r2!, {r4-r7}                            @ store key_sch[32-35]

    ldr     r0, [sl, $8<<2]                         @ get rcon[8]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[35])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[8]

# calculation for key_sch[36-39]
    eor     r4, r3                                  @ XOR key_sch[32]
    eor     r5, r4                                  @ key_sch[33]^key_sch[36]
    eor     r6, r5                                  @ key_sch[34]^key_sch[37]
    eor     r7, r6                                  @ key_sch[35]^key_sch[38]
    stmia   r2!, {r4-r7}                            @ store key_sch[36-39]

    ldr     r0, [sl, $9<<2]                         @ get rcon[9]
    ror     r3, r7, $24                             @ rotate left by 8
    str     r3, [ip, $0x800+(LDR+CAA)<<2]           @ ROTL(key_sch[39])-> acc
    str     fp, [ip]                                @ AES SubBytes
    ldr     r3, [ip, $0x800+(STR+CAA)<<2]           @ get CAA
    eor     r3, r0                                  @ XOR rcon[9]

# calculation for key_sch[40-43]
    eor     r4, r3                                  @ XOR key_sch[36]
    eor     r5, r4                                  @ key_sch[37]^key_sch[40]
    eor     r6, r5                                  @ key_sch[38]^key_sch[41]
    eor     r7, r6                                  @ key_sch[39]^key_sch[42]
    stmia   r2!, {r4-r7}                            @ store key_sch[40-43]

    ldmia   sp!, {r4-ip,pc}                         @ restore regs and return


#*******************************************************************************
#*******************************************************************************
#
# AES: Encrypts a single 16-byte block
#   arguments
#           *in         pointer to 16-byte block of input plaintext
#           *key_sch    pointer to key schedule (44, 52, 60 longwords)
#           nr          number of AES rounds (10, 12, 14 = f(key_schedule))
#           *out        pointer to 16-byte block of output ciphertext
#
#
#   calling convention
#   void    mmcau_aes_encrypt (const unsigned char *in,
#                              const unsigned char *key_sch,
#                              const int            nr,
#                              unsigned char       *out)

    .global _mmcau_aes_encrypt
    .global mmcau_aes_encrypt
    .type   mmcau_aes_encrypt, %function
    .align  4

_mmcau_aes_encrypt:
mmcau_aes_encrypt:

#  register allocation
# --------------------
#  r0      = scratch / input  *in  / mmcau_3_cmds(AESS+CA0,AESS+CA1,AESS+CA2)
#  r1      = scratch / input  *key_sch
#  r2      = scratch / input   nr
#  r3      = scratch / output *out
#  r4      = scratch
#  r5      = scratch
#  r6      = scratch
#  r7      = scratch
#  r8      = scratch / mmcau_2_cmds(AESS+CA3,AESR)
#  r9      = scratch / mmcau_indirect_cmd(AESC+CA0)
# r10 (sl) = scratch / mmcau_indirect_cmd(STR+CA0)
# r11 (fp) = scratch / mmcau_indirect_cmd(LDR+CA0)
# r12 (ip) = scratch / pointer to MMCAU_PPB_DIRECT
# r13 (sp) = stack pointer
# r14 (lr) = link register

    stmdb   sp!, {r4-ip}                            @ save registers on stack

# load the 16 plain text bytes (4 words) into r4-r7
    ldmia   r0, {r4-r7}                             @ get plaintext[0-3]
    rev     r4, r4                                  @ byte reverse
    rev     r5, r5                                  @ byte reverse
    rev     r6, r6                                  @ byte reverse
    rev     r7, r7                                  @ byte reverse

# prepare for AES operations register load
    movw    r0, #:lower16:encrypt_reg_data
    movt    r0, #:upper16:encrypt_reg_data

# XOR the first 4 keys into the 16 plain text bytes
    ldmia   r1!, {r8-fp}                            @ get key_sch[0-3]; r1++
    eor     r4, r8
    eor     r5, r9
    eor     r6, sl
    eor     r7, fp

# load registers needed for mmcau commands from encrypt_reg_data:
    ldmia   r0, {r0,r8-ip}                          @ setup AES operations

# load the XOR results into the CAU's CA0 - CA3 registers
    stmia   fp, {r4-r7}                             @ load CA0-CA3

# send a series of cau commands to perform the encryption
    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    cmp     r2, $10                                 @ if aes128, finish
    beq     encrypt_end

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    cmp     r2, $12                                 @ if aes192, finish
    beq     encrypt_end

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                            @ get next 4 keys; r1++
    stmia   r9, {r4-r7}                             @ MixColumns

encrypt_end:
    str     r0, [ip]                                @ SubBytes
    str     r8, [ip]                                @ SubBytes, ShiftRows

# XOR the last 4 keys into CAO - CA3 ciphertext output
    ldmia   sl, {r4-r7}                             @ get CA0 - CA3
    ldmia   r1, {r8-fp}                             @ get key_sch[j]-[j+3]
    eor     r4, r8
    eor     r5, r9
    eor     r6, sl
    eor     r7, fp

# store the 16-byte ciphertext output block into memory
    rev     r4, r4                                  @ byte reverse
    rev     r5, r5                                  @ byte reverse
    rev     r6, r6                                  @ byte reverse
    rev     r7, r7                                  @ byte reverse
    stmia   r3, {r4-r7}                             @ save to output[0-3]

    ldmia   sp!, {r4-ip}                            @ restore regs and return
    bx      lr

#*******************************************************************************
#*******************************************************************************
#
# AES: Decrypts a single 16-byte block
#   arguments
#           *in         pointer to 16-byte block of input chiphertext
#           *key_sch    pointer to key schedule (44, 52, 60 longwords)
#           nr          number of AES rounds (10, 12, 14 = f(key_schedule))
#           *out        pointer to 16-byte block of output plaintext
#
#
#   calling convention
#   void    mmcau_aes_decrypt (const unsigned char *in,
#                              const unsigned char *key_sch,
#                              const int            nr,
#                              unsigned char       *out)

    .global _mmcau_aes_decrypt
    .global mmcau_aes_decrypt
    .type   mmcau_aes_decrypt, %function
    .align  4

_mmcau_aes_decrypt:
mmcau_aes_decrypt:

#  register allocation
# --------------------
#  r0      = scratch / input  *in  / mmcau_3_cmds(AESIR,AESIS+CA3,AESIS+CA2)
#  r1      = scratch / input  *key_sch
#  r2      = scratch / input   nr
#  r3      = scratch / output *out
#  r4      = scratch
#  r5      = scratch
#  r6      = scratch
#  r7      = scratch
#  r8      = scratch / mmcau_2_cmds(AESIS+CA1,AESIS+CA0)
#  r9      = scratch / mmcau_indirect_cmd(AESIC+CA0)
# r10 (sl) = scratch / mmcau_indirect_cmd(STR+CA0)
# r11 (fp) = scratch / mmcau_indirect_cmd(LDR+CA0)
# r12 (ip) = scratch / pointer to MMCAU_PPB_DIRECT
# r13 (sp) = stack pointer
# r14 (lr) = link register

    stmdb   sp!, {r4-ip}                            @ save registers on stack

# load the 16 cipher bytes (4 words) into r4-r7
    ldmia   r0, {r4-r7}                             @ get in[0-3]
    rev     r4, r4                                  @ byte reverse
    rev     r5, r5                                  @ byte reverse
    rev     r6, r6                                  @ byte reverse
    rev     r7, r7                                  @ byte reverse

# prepare for AES operations register load
    movw    r0, #:lower16:decrypt_reg_data
    movt    r0, #:upper16:decrypt_reg_data

# the key_sch pointer (r1) is adjusted to define the end of the elements
# the adjustment factor = f(nr) is defined by the expression:
#   end of key_sch = 4 x (nr + 1) for nr = {10, 12, 14}
    add     r1, r1, r2, LSL $4

# XOR the last 4 keys into the 4 cipher words
    ldmia   r1, {r8-fp}                             @ get last 4 keys
    eor     r4, r8
    eor     r5, r9
    eor     r6, sl
    eor     r7, fp

# load registers needed for mmcau commands from decrypt_reg_data:
    ldmia   r0, {r0,r8-ip}                          @ setup AES operations

# load the 16 cipher bytes (4 words) into the CAU's CA0 - CA3 registers
    stmia   fp, {r4-r7}                             @ load CA0-CA3

# send a series of cau commands to perform the decryption

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    cmp     r2, $10                                 @ if aes128, finish
    beq     decrypt_end

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    cmp     r2, $12                                 @ if aes192, finish
    beq     decrypt_end

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

    ldmdb   r1!, {r4-r7}                            @ key_sch[i] to [i+4]; r1--
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes
    stmia   r9, {r4-r7}                             @ MixColumns

decrypt_end:
    str     r0, [ip]                                @ InvShiftRows,InvSubBytes
    str     r8, [ip]                                @ InvSubBytes

# XOR the first 4 keys into CAO - CA3 plaintext output
    ldmia   sl, {r4-r7}                             @ get CA0 - CA3
    ldmdb   r1!, {r8-fp}                            @ key_sch[i] to [i+4]; r1--
    eor     r4, r8
    eor     r5, r9
    eor     r6, sl
    eor     r7, fp

# store the 16-byte plain text output block into memory
    rev     r4, r4                                  @ byte reverse
    rev     r5, r5                                  @ byte reverse
    rev     r6, r6                                  @ byte reverse
    rev     r7, r7                                  @ byte reverse
    stmia   r3, {r4-r7}                             @ save to output[0-3]

    ldmia   sp!, {r4-ip}                            @ restore regs and return
    bx      lr

#*******************************************************************************

    .data
    .type   setkey_reg_data, %object
    .align  4

setkey_reg_data:
    .word   rcon                                                @ sl
    .word   MMCAU_1_CMD+(AESS+CAA)<<22                          @ fp
    .word   MMCAU_PPB_DIRECT                                    @ ip

    .type   encrypt_reg_data, %object
    .align  4

encrypt_reg_data:
    .word   MMCAU_3_CMDS+(AESS+CA0)<<22+(AESS+CA1)<<11+AESS+CA2 @ r0
    .word   MMCAU_2_CMDS+(AESS+CA3)<<22+(AESR)<<11              @ r8
    .word   MMCAU_PPB_INDIRECT+(AESC+CA0)<<2                    @ r9
    .word   MMCAU_PPB_INDIRECT+(STR+CA0)<<2                     @ sl
    .word   MMCAU_PPB_INDIRECT+(LDR+CA0)<<2                     @ fp
    .word   MMCAU_PPB_DIRECT                                    @ ip

    .type   decrypt_reg_data, %object
    .align  4

decrypt_reg_data:
    .word   MMCAU_3_CMDS+(AESIR)<<22+(AESIS+CA3)<<11+AESIS+CA2  @ r0
    .word   MMCAU_2_CMDS+(AESIS+CA1)<<22+(AESIS+CA0)<<11        @ r8
    .word   MMCAU_PPB_INDIRECT+(AESIC+CA0)<<2                   @ r9
    .word   MMCAU_PPB_INDIRECT+(STR+CA0)<<2                     @ sl
    .word   MMCAU_PPB_INDIRECT+(LDR+CA0)<<2                     @ fp
    .word   MMCAU_PPB_DIRECT                                    @ ip

    .type   rcon, %object
    .align  4
#else
   #.include "cau2_defines.hdr"                                          {uTasker} Cortex-M0+ code
    .syntax unified


    .equ      MMCAU_PPB_DIRECT, 0xf0005000
    .equ    MMCAU_PPB_INDIRECT, 0xf0005800
    .equ           MMCAU_1_CMD, 0x80000000
    .equ          MMCAU_2_CMDS, 0x80100000
    .equ          MMCAU_3_CMDS, 0x80100200

        .equ    TL,0
    .equ    TS,0
    .equ    CASR,0
    .equ    CAA,1
    .equ    CA0,2
    .equ    CA1,3
    .equ    CA2,4
    .equ    CA3,5
    .equ    CA4,6
    .equ    CA5,7
    .equ    CA6,8
    .equ    CA7,9
    .equ    CA8,10
    .equ    CNOP,0x000
    .equ    LDR,0x010
    .equ    STR,0x020
    .equ    ADR,0x030
    .equ    RADR,0x040
    .equ    ADRA,0x050
    .equ    XOR,0x060
    .equ    ROTL,0x070
    .equ    MVRA,0x080
    .equ    MVAR,0x090
    .equ    AESS,0x0a0
    .equ    AESIS,0x0b0
    .equ    AESC,0x0c0
    .equ    AESIC,0x0d0
    .equ    AESR,0x0e0
    .equ    AESIR,0x0f0
    .equ    DESR,0x100
    .equ    DESK,0x110
    .equ    HASH,0x120
    .equ    SHS,0x130
    .equ    MDS,0x140
    .equ    SHS2,0x150
    .equ    ILL,0x1f0
    .equ    IP,8
    .equ    FP,4
    .equ    DC,1
    .equ    CP,2
    .equ    KSL1,0
    .equ    KSL2,1
    .equ    KSR1,2
    .equ    KSR2,3
    .equ    HFF,0
    .equ    HFG,1
    .equ    HFH,2
    .equ    HFI,3
    .equ    HFP,2
    .equ    HFC,4
    .equ    HFM,5
    .equ    HF2C,6
    .equ    HF2M,7
    .equ    HF2S,8
    .equ    HF2T,9
    .equ    HF2U,10
    .equ    HF2V,11

    .syntax unified

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   MMCAU_AES_SET_KEY
#   Performs an AES key expansion
#
#   ARGUMENTS
#   *key            pointer to input key (128, 192, 256 bits in length)
#   key_size        key_size in bits (128, 192, 256)
#   *key_sch        pointer to key schedule output (44, 52, 60 longwords)
#
#   CALLING CONVENTION
#   void mmcau_aes_set_key     (const unsigned char     *key,
#                               const int               key_size,
#                               unsigned char           *key_sch)
#
#  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #
#
#    REGISTER  |  ALLOCATION (at the start of mmcau_aes_set_key)
#   -----------+------------------------------------------------------------
#          r0  |  *key          (arg0)
#          r1  |  key_size      (arg1)
#          r2  |  *key_sch      (arg2)
#              |
#        > r2  |  irrelevant
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

    .global _mmcau_aes_set_key
    .global mmcau_aes_set_key
    .type   mmcau_aes_set_key, %function
    .align  4

_mmcau_aes_set_key:
mmcau_aes_set_key:

# store regs r4-r12 and r14, we need to restore them at the end of the routine
    push    {r4-r7, lr}                     @ store low regs and link reg
    mov     r3, r8
    mov     r4, r9
    mov     r5, sl
    mov     r6, fp
    mov     r7, ip
    push    {r3-r7}                         @ store high regs

    ldr     r3, =set_key_reg_data            @ prepare for set_key reg load


set_key_check_size:
    cmp     r1, #128                        @ if key_size != 128,
    bne     set_key_check_size_again        @ then = 192 or 256, so check again
    b       set_key_128                     @ else = 128, so do set_key_128


set_key_check_size_again:
    cmp     r1, #192                        @ if key_size != 192,
    bne     set_key_256                     @ then = 256, so do set_key_256
    b       set_key_192                     @ else = 192, so do set_key_192
    .ltorg


set_key_256:

#    REGISTER  |  ALLOCATION (throughout set_key_256)
#   -----------+------------------------------------------------------------
#          r0  |  scratch
#          r1  |  scratch
#          r2  |  *key_sch
#          r3  |  key_sch[0+8i] / scratch
#          r4  |  key_sch[1+8i] / scratch
#          r5  |  key_sch[2+8i] / scratch
#          r6  |  key_sch[3+8i] / scratch
#          r7  |  scratch
#          r8  |  *rcon
#          r9  |  mmcau_1_cmd(AESS+CAA)
#    (sl) r10  |  *mmcau_direct_cmd()
#    (fp) r11  |  mmcau_indirect_cmd(LDR+CAA)
#    (ip) r12  |  mmcau_indirect_cmd(STR+CAA)
#    (sp) r13  |  stack pointer
#    (lr) r14  |  link register

# load some of the regs in preperation of the AES-256 set key calculations
    ldmia   r3, {r3-r7}
    mov     r8, r3                          @ r8 = *rcon
    mov     r9, r4                          @ r9 = mmcau_1_cmd(AESS+CAA)
    mov     sl, r5                          @ sl = *mmcau_direct_cmd()
    mov     fp, r6                          @ fp = mmcau_indirect_cmd(LDR+CAA)
    mov     ip, r7                          @ ip = mmcau_indirect_cmd(STR+CAA)

# calculate key_sch[0-4]
    ldmia   r0!, {r3-r7}                    @ load key[0-4]; *key++
    rev     r3, r3                          @ byterev(key[0]) = key_sch[0]
    rev     r4, r4                          @ byterev(key[1]) = key_sch[1]
    rev     r5, r5                          @ byterev(key[2]) = key_sch[2]
    rev     r6, r6                          @ byterev(key[3]) = key_sch[3]
    rev     r7, r7                          @ byterev(key[4]) = key_sch[4]
    stmia   r2!, {r3-r7}                    @ store key_sch[0-4], key_sch++

# calculate key_sch[5-7]
    ldmia   r0, {r0-r1,r7}                  @ load key[5-7]
    rev     r0, r0                          @ byterev(key[5]) = key_sch[5]
    rev     r1, r1                          @ byterev(key[6]) = key_sch[6]
    rev     r7, r7                          @ byterev(key[7]) = key_sch[7]
    stmia   r2!, {r0-r1, r7}                @ store key_sch[5-7], key_sch++

# calculate key_sch[8-11]
    mov     r0, r8
    ldr     r1, [r0]                        @ load rcon[0]
    movs    r0, #24
    rors    r7, r0                          @ ROTL(key_sch[7],8)
    mov     r0, fp
    str     r7, [r0]                        @ ROTL(key_sch[7]) -> acc
    mov     r7, r9
    mov     r0, sl
    str     r7, [r0]                        @ AES SubBytes
    mov     r0, ip
    ldr     r7, [r0]                        @ load CAA
    eors    r1, r7                          @ XOR rcon[0]
    eors    r3, r1                          @ XOR key_sch[0]  = key_sch[8]
    eors    r4, r3                          @ XOR key_sch[1]  = key_sch[9]
    eors    r5, r4                          @ XOR key_sch[2]  = key_sch[10]
    eors    r6, r5                          @ XOR key_sch[3]  = key_sch[11]
    stmia   r2!, {r3-r6}                    @ store key_sch[8-11], *key_sch++

# calculate key_sch[12-15]
    mov     r5, fp
    str     r6, [r5]                        @ ROTL(key_sch[11]) -> acc
    mov     r3, r9
    mov     r4, sl
    str     r3, [r4]                        @ AES SubBytes
    mov     r7, ip
    ldr     r1, [r7]                        @ load CAA
    subs    r2, #8<<2                       @ set *key_sch[4]
    ldmia   r2!, {r3-r6}                    @ load key_sch[4-7], *key_sch++
    eors    r3, r1                          @ XOR key_sch[4]  = key_sch[12]
    eors    r4, r3                          @ XOR key_sch[5]  = key_sch[13]
    eors    r5, r4                          @ XOR key_sch[6]  = key_sch[14]
    eors    r6, r5                          @ XOR key_sch[7]  = key_sch[15]
    adds    r2, #4<<2                       @ set *key_sch[12]
    stmia   r2!, {r3-r6}                    @ store key_sch[12-15], *key_sch++

# calculate key_sch[16-19]
    mov     r0, r8
    ldr     r7, [r0, #1<<2]                 @ load rcon[1]
    mov     r5, fp
    movs    r0, #24
    mov     r3, r9
    rors    r6, r0                          @ ROTL(key_sch[15],8)
    mov     r4, sl
    str     r6, [r5]                        @ ROTL(key_sch[15]) -> acc
    mov     r0, ip
    str     r3, [r4]                        @ AES SubBytes
    ldr     r1, [r0]                        @ load CAA
    eors    r1, r7                          @ XOR rcon[1]
    subs    r2, #8<<2                       @ set *key_sch[8]
    ldmia   r2!, {r3-r6}                    @ load key_sch[8-11], *key_sch++
    eors    r3, r1                          @ XOR key_sch[8]  = key_sch[16]
    eors    r4, r3                          @ XOR key_sch[9]  = key_sch[17]
    eors    r5, r4                          @ XOR key_sch[10] = key_sch[18]
    eors    r6, r5                          @ XOR key_sch[11] = key_sch[19]
    adds    r2, #4<<2                       @ set *key_sch[16]
    stmia   r2!, {r3-r6}                    @ store key_sch[16-19], *key_sch++

# calculate key_sch[20-23]
    mov     r5, fp
    str     r6, [r5]                        @ ROTL(key_sch[19]) -> acc
    mov     r3, r9
    mov     r4, sl
    str     r3, [r4]                        @ AES SubBytes
    mov     r7, ip
    ldr     r1, [r7]                        @ load CAA
    subs    r2, #8<<2                       @ set *key_sch[12]
    ldmia   r2!, {r3-r6}                    @ load key_sch[12-15], *key_sch++
    eors    r3, r1                          @ XOR key_sch[12] = key_sch[20]
    eors    r4, r3                          @ XOR key_sch[13] = key_sch[21]
    eors    r5, r4                          @ XOR key_sch[14] = key_sch[22]
    eors    r6, r5                          @ XOR key_sch[15] = key_sch[23]
    adds    r2, #4<<2                       @ set *key_sch[20]
    stmia   r2!, {r3-r6}                    @ store key_sch[20-23], *key_sch++

# calculate key_sch[24-27]
    mov     r0, r8
    ldr     r7, [r0, #2<<2]                 @ load rcon[2]
    mov     r5, fp
    movs    r0, #24
    mov     r3, r9
    rors    r6, r0                          @ ROTL(key_sch[23],8)
    mov     r4, sl
    str     r6, [r5]                        @ ROTL(key_sch[23]) -> acc
    mov     r0, ip
    str     r3, [r4]                        @ AES SubBytes
    ldr     r1, [r0]                        @ load CAA
    eors    r1, r7                          @ XOR rcon[2]
    subs    r2, #8<<2                       @ set *key_sch[16]
    ldmia   r2!, {r3-r6}                    @ load key_sch[16-19], *key_sch++
    eors    r3, r1                          @ XOR key_sch[16] = key_sch[24]
    eors    r4, r3                          @ XOR key_sch[17] = key_sch[25]
    eors    r5, r4                          @ XOR key_sch[18] = key_sch[26]
    eors    r6, r5                          @ XOR key_sch[19] = key_sch[27]
    adds    r2, #4<<2                       @ set *key_sch[24]
    stmia   r2!, {r3-r6}                    @ store key_sch[24-27], *key_sch++

# calculate key_sch[28-31]
    mov     r5, fp
    str     r6, [r5]                        @ ROTL(key_sch[27]) -> acc
    mov     r3, r9
    mov     r4, sl
    str     r3, [r4]                        @ AES SubBytes
    mov     r7, ip
    ldr     r1, [r7]                        @ load CAA
    subs    r2, #8<<2                       @ set *key_sch[20]
    ldmia   r2!, {r3-r6}                    @ load key_sch[20-23], *key_sch++
    eors    r3, r1                          @ XOR key_sch[20] = key_sch[28]
    eors    r4, r3                          @ XOR key_sch[21] = key_sch[29]
    eors    r5, r4                          @ XOR key_sch[22] = key_sch[30]
    eors    r6, r5                          @ XOR key_sch[23] = key_sch[31]
    adds    r2, #4<<2                       @ set *key_sch[28]
    stmia   r2!, {r3-r6}                    @ store key_sch[28-31], *key_sch++

# calculate key_sch[32-35]
    mov     r0, r8
    ldr     r7, [r0, #3<<2]                 @ load rcon[3]
    mov     r5, fp
    movs    r0, #24
    mov     r3, r9
    rors    r6, r0                          @ ROTL(key_sch[31],8)
    mov     r4, sl
    str     r6, [r5]                        @ ROTL(key_sch[31]) -> acc
    mov     r0, ip
    str     r3, [r4]                        @ AES SubBytes
    ldr     r1, [r0]                        @ load CAA
    eors    r1, r7                          @ XOR rcon[3]
    subs    r2, #8<<2                       @ set *key_sch[24]
    ldmia   r2!, {r3-r6}                    @ load key_sch[24-27], *key_sch++
    eors    r3, r1                          @ XOR key_sch[24] = key_sch[32]
    eors    r4, r3                          @ XOR key_sch[25] = key_sch[33]
    eors    r5, r4                          @ XOR key_sch[26] = key_sch[34]
    eors    r6, r5                          @ XOR key_sch[27] = key_sch[35]
    adds    r2, #4<<2                       @ set *key_sch[32]
    stmia   r2!, {r3-r6}                    @ store key_sch[32-35], *key_sch++

# calculate key_sch[36-39]
    mov     r5, fp
    str     r6, [r5]                        @ ROTL(key_sch[35]) -> acc
    mov     r3, r9
    mov     r4, sl
    str     r3, [r4]                        @ AES SubBytes
    mov     r7, ip
    ldr     r1, [r7]                        @ load CAA
    subs    r2, #8<<2                       @ set *key_sch[28]
    ldmia   r2!, {r3-r6}                    @ load key_sch[28-31], *key_sch++
    eors    r3, r1                          @ XOR key_sch[28] = key_sch[36]
    eors    r4, r3                          @ XOR key_sch[29] = key_sch[37]
    eors    r5, r4                          @ XOR key_sch[30] = key_sch[38]
    eors    r6, r5                          @ XOR key_sch[31] = key_sch[39]
    adds    r2, #4<<2                       @ set *key_sch[36]
    stmia   r2!, {r3-r6}                    @ store key_sch[36-39], *key_sch++

# calculate key_sch[40-43]
    mov     r0, r8
    ldr     r7, [r0, #4<<2]                 @ load rcon[4]
    mov     r5, fp
    movs    r0, #24
    mov     r3, r9
    rors    r6, r0                          @ ROTL(key_sch[39],8)
    mov     r4, sl
    str     r6, [r5]                        @ ROTL(key_sch[39]) -> acc
    mov     r0, ip
    str     r3, [r4]                        @ AES SubBytes
    ldr     r1, [r0]                        @ load CAA
    eors    r1, r7                          @ XOR rcon[4]
    subs    r2, #8<<2                       @ set *key_sch[32]
    ldmia   r2!, {r3-r6}                    @ load key_sch[32-35], *key_sch++
    eors    r3, r1                          @ XOR key_sch[32] = key_sch[40]
    eors    r4, r3                          @ XOR key_sch[33] = key_sch[41]
    eors    r5, r4                          @ XOR key_sch[34] = key_sch[42]
    eors    r6, r5                          @ XOR key_sch[35] = key_sch[43]
    adds    r2, #4<<2                       @ set *key_sch[40]
    stmia   r2!, {r3-r6}                    @ store key_sch[40-43], *key_sch++

# calculate key_sch[44-47]
    mov     r5, fp
    str     r6, [r5]                        @ ROTL(key_sch[43]) -> acc
    mov     r3, r9
    mov     r4, sl
    str     r3, [r4]                        @ AES SubBytes
    mov     r7, ip
    ldr     r1, [r7]                        @ load CAA
    subs    r2, #8<<2                       @ set *key_sch[36]
    ldmia   r2!, {r3-r6}                    @ load key_sch[36-39], *key_sch++
    eors    r3, r1                          @ XOR key_sch[36] = key_sch[44]
    eors    r4, r3                          @ XOR key_sch[37] = key_sch[45]
    eors    r5, r4                          @ XOR key_sch[38] = key_sch[46]
    eors    r6, r5                          @ XOR key_sch[39] = key_sch[47]
    adds    r2, #4<<2                       @ set *key_sch[44]
    stmia   r2!, {r3-r6}                    @ store key_sch[44-47], *key_sch++

# calculate key_sch[48-51]
    mov     r0, r8
    ldr     r7, [r0, #5<<2]                 @ load rcon[5]
    mov     r5, fp
    movs    r0, #24
    mov     r3, r9
    rors    r6, r0                          @ ROTL(key_sch[47],8)
    mov     r4, sl
    str     r6, [r5]                        @ ROTL(key_sch[47]) -> acc
    mov     r0, ip
    str     r3, [r4]                        @ AES SubBytes
    ldr     r1, [r0]                        @ load CAA
    eors    r1, r7                          @ XOR rcon[5]
    subs    r2, #8<<2                       @ set *key_sch[40]
    ldmia   r2!, {r3-r6}                    @ load key_sch[40-43], *key_sch++
    eors    r3, r1                          @ XOR key_sch[40] = key_sch[48]
    eors    r4, r3                          @ XOR key_sch[41] = key_sch[49]
    eors    r5, r4                          @ XOR key_sch[42] = key_sch[50]
    eors    r6, r5                          @ XOR key_sch[43] = key_sch[51]
    adds    r2, #4<<2                       @ set *key_sch[48]
    stmia   r2!, {r3-r6}                    @ store key_sch[48-51], *key_sch++

# calculate key_sch[52-55]
    mov     r5, fp
    str     r6, [r5]                        @ ROTL(key_sch[51]) -> acc
    mov     r3, r9
    mov     r4, sl
    str     r3, [r4]                        @ AES SubBytes
    mov     r7, ip
    ldr     r1, [r7]                        @ load CAA
    subs    r2, #8<<2                       @ set *key_sch[44]
    ldmia   r2!, {r3-r6}                    @ load key_sch[44-47], *key_sch++
    eors    r3, r1                          @ XOR key_sch[44] = key_sch[52]
    eors    r4, r3                          @ XOR key_sch[45] = key_sch[53]
    eors    r5, r4                          @ XOR key_sch[46] = key_sch[54]
    eors    r6, r5                          @ XOR key_sch[47] = key_sch[55]
    adds    r2, #4<<2                       @ set *key_sch[52]
    stmia   r2!, {r3-r6}                    @ store key_sch[52-55], *key_sch++

# calculate key_sch[56-59]
    mov     r0, r8
    ldr     r7, [r0, #6<<2]                 @ load rcon[6]
    mov     r5, fp
    movs    r0, #24
    mov     r3, r9
    rors    r6, r0                          @ ROTL(key_sch[55],8)
    mov     r4, sl
    str     r6, [r5]                        @ ROTL(key_sch[55]) -> acc
    mov     r0, ip
    str     r3, [r4]                        @ AES SubBytes
    ldr     r1, [r0]                        @ load CAA
    eors    r1, r7                          @ XOR rcon[6]
    subs    r2, #8<<2                       @ set *key_sch[48]
    ldmia   r2!, {r3-r6}                    @ load key_sch[48-51], *key_sch++
    eors    r3, r1                          @ XOR key_sch[48] = key_sch[56]
    eors    r4, r3                          @ XOR key_sch[49] = key_sch[57]
    eors    r5, r4                          @ XOR key_sch[50] = key_sch[58]
    eors    r6, r5                          @ XOR key_sch[51] = key_sch[59]
    adds    r2, #4<<2                       @ set *key_sch[56]
    stmia   r2!, {r3-r6}                    @ store key_sch[56-59], *key_sch++

    b       set_key_end                     @ end routine


set_key_192:

#    REGISTER  |  ALLOCATION (throughout set_key_192)
#   -----------+------------------------------------------------------------
#          r0  |  key_sch[0+6i]
#          r1  |  key_sch[1+6i]
#          r2  |  *key_sch
#          r3  |  key_sch[2+6i]
#          r4  |  key_sch[3+6i]
#          r5  |  key_sch[4+6i] / rcon[i]
#          r6  |  key_sch[5+6i] / scratch
#          r7  |  scratch
#          r8  |  *rcon
#          r9  |  mmcau_1_cmd(AESS+CAA)
#    (sl) r10  |  *mmcau_direct_cmd()
#    (fp) r11  |  mmcau_indirect_cmd(LDR+CAA)
#        NOTE  |  mmcau_indirect_cmd(STR+CAA) = mmcau_indirect_cmd(LDR+CAA)+64
#    (ip) r12  |  temporary storage for key_sch[4+6i]
#    (sp) r13  |  stack pointer
#    (lr) r14  |  temporary storage for key_sch[5+6i]

# load some of the regs in preperation of the AES-192 set key calculations
    ldmia   r3, {r3-r6}    
    mov     r8, r3                          @ r8 = *rcon
    mov     r9, r4                          @ r9 = mmcau_1_cmd(AESS+CAA)
    mov     sl, r5                          @ sl = *mmcau_direct_cmd()
    mov     fp, r6                          @ fp = mmcau_indirect_cmd(LDR+CAA)

# calculate key_sch[0-5]
    ldmia   r0, {r0-r1, r3-r6}              @ load key[0-5]
    rev     r0, r0                          @ byterev(key[0]) = key_sch[0]
    rev     r1, r1                          @ byterev(key[1]) = key_sch[1]
    rev     r3, r3                          @ byterev(key[2]) = key_sch[2]
    rev     r4, r4                          @ byterev(key[3]) = key_sch[3]
    rev     r5, r5                          @ byterev(key[4]) = key_sch[4]
    rev     r6, r6                          @ byterev(key[5]) = key_sch[5]
    stmia   r2!, {r0-r1, r3-r6}             @ store key_sch[0-5]

# calculate key_sch[6-11]
    mov     ip, r5                          @ temporarily store key_sch[4]
    mov     lr, r6                          @ temporarily store key_sch[5]
    mov     r7, r8
    ldr     r5, [r7, #0<<2]                 @ load rcon[0]
    movs    r7, #24
    rors    r6, r7                          @ ROTL(key_sch[5],8)
    mov     r7, fp
    str     r6, [r7]                        @ ROTL(key_sch[5],8) -> acc
    mov     r6, r9
    mov     r7, sl
    str     r6, [r7]                        @ AES SubBytes
    mov     r6, fp
    adds    r6, #64
    ldr     r7, [r6]                        @ load CAA
    eors    r7, r5                          @ XOR rcon[0]
    mov     r5, ip                          @ restore key_sch[4]
    mov     r6, lr                          @ restore key_sch[5]
    eors    r0, r7                          @ XOR key_sch[0]  = key_sch[6]
    eors    r1, r0                          @ XOR key_sch[1]  = key_sch[7]
    eors    r3, r1                          @ XOR key_sch[2]  = key_sch[8]
    eors    r4, r3                          @ XOR key_sch[3]  = key_sch[9]
    eors    r5, r4                          @ XOR key_sch[4]  = key_sch[10]
    eors    r6, r5                          @ XOR key_sch[5]  = key_sch[11]
    stmia   r2!, {r0-r1, r3-r6}             @ store key_sch[6-11], *key_sch++

# calculate key_sch[12-17]
    mov     ip, r5                          @ temporarily store key_sch[10]
    mov     lr, r6                          @ temporarily store key_sch[11]
    mov     r7, r8
    ldr     r5, [r7, #1<<2]                 @ load rcon[1]
    movs    r7, #24
    rors    r6, r7                          @ ROTL(key_sch[11],8)
    mov     r7, fp
    str     r6, [r7]                        @ ROTL(key_sch[11],8) -> acc
    mov     r6, r9
    mov     r7, sl
    str     r6, [r7]                        @ AES SubBytes
    mov     r6, fp
    adds    r6, #64
    ldr     r7, [r6]                        @ load CAA
    eors    r7, r5                          @ XOR rcon[1]
    mov     r5, ip                          @ restore key_sch[10]
    mov     r6, lr                          @ restore key_sch[11]
    eors    r0, r7                          @ XOR key_sch[6]  = key_sch[12]
    eors    r1, r0                          @ XOR key_sch[7]  = key_sch[13]
    eors    r3, r1                          @ XOR key_sch[8]  = key_sch[14]
    eors    r4, r3                          @ XOR key_sch[9]  = key_sch[15]
    eors    r5, r4                          @ XOR key_sch[10] = key_sch[16]
    eors    r6, r5                          @ XOR key_sch[11] = key_sch[17]
    stmia   r2!, {r0-r1, r3-r6}             @ store key_sch[12-17], *key_sch++

# calculate key_sch[18-23]
    mov     ip, r5                          @ temporarily store key_sch[16]
    mov     lr, r6                          @ temporarily store key_sch[17]
    mov     r7, r8
    ldr     r5, [r7, #2<<2]                 @ load rcon[2]
    movs    r7, #24
    rors    r6, r7                          @ ROTL(key_sch[17],8)
    mov     r7, fp
    str     r6, [r7]                        @ ROTL(key_sch[17],8) -> acc
    mov     r6, r9
    mov     r7, sl
    str     r6, [r7]                        @ AES SubBytes
    mov     r6, fp
    adds    r6, #64
    ldr     r7, [r6]                        @ load CAA
    eors    r7, r5                          @ XOR rcon[2]
    mov     r5, ip                          @ restore key_sch[16]
    mov     r6, lr                          @ restore key_sch[17]
    eors    r0, r7                          @ XOR key_sch[12] = key_sch[18]
    eors    r1, r0                          @ XOR key_sch[13] = key_sch[19]
    eors    r3, r1                          @ XOR key_sch[14] = key_sch[20]
    eors    r4, r3                          @ XOR key_sch[15] = key_sch[21]
    eors    r5, r4                          @ XOR key_sch[16] = key_sch[22]
    eors    r6, r5                          @ XOR key_sch[17] = key_sch[23]
    stmia   r2!, {r0-r1, r3-r6}             @ store key_sch[18-23], *key_sch++

# calculate key_sch[24-29]
    mov     ip, r5                          @ temporarily store key_sch[22]
    mov     lr, r6                          @ temporarily store key_sch[23]
    mov     r7, r8
    ldr     r5, [r7, #3<<2]                 @ load rcon[3]
    movs    r7, #24
    rors    r6, r7                          @ ROTL(key_sch[23],8)
    mov     r7, fp
    str     r6, [r7]                        @ ROTL(key_sch[23],8) -> acc
    mov     r6, r9
    mov     r7, sl
    str     r6, [r7]                        @ AES SubBytes
    mov     r6, fp
    adds    r6, #64
    ldr     r7, [r6]                        @ load CAA
    eors    r7, r5                          @ XOR rcon[3]
    mov     r5, ip                          @ restore key_sch[22]
    mov     r6, lr                          @ restore key_sch[23]
    eors    r0, r7                          @ XOR key_sch[18] = key_sch[24]
    eors    r1, r0                          @ XOR key_sch[19] = key_sch[25]
    eors    r3, r1                          @ XOR key_sch[20] = key_sch[26]
    eors    r4, r3                          @ XOR key_sch[21] = key_sch[27]
    eors    r5, r4                          @ XOR key_sch[22] = key_sch[28]
    eors    r6, r5                          @ XOR key_sch[23] = key_sch[29]
    stmia   r2!, {r0-r1, r3-r6}             @ store key_sch[24-29], *key_sch++

# calculate key_sch[30-35]
    mov     ip, r5                          @ temporarily store key_sch[28]
    mov     lr, r6                          @ temporarily store key_sch[29]
    mov     r7, r8
    ldr     r5, [r7, #4<<2]                 @ load rcon[4]
    movs    r7, #24
    rors    r6, r7                          @ ROTL(key_sch[29],8)
    mov     r7, fp
    str     r6, [r7]                        @ ROTL(key_sch[29],8) -> acc
    mov     r6, r9
    mov     r7, sl
    str     r6, [r7]                        @ AES SubBytes
    mov     r6, fp
    adds    r6, #64
    ldr     r7, [r6]                        @ load CAA
    eors    r7, r5                          @ XOR rcon[4]
    mov     r5, ip                          @ restore key_sch[28]
    mov     r6, lr                          @ restore key_sch[29]
    eors    r0, r7                          @ XOR key_sch[24] = key_sch[30]
    eors    r1, r0                          @ XOR key_sch[25] = key_sch[31]
    eors    r3, r1                          @ XOR key_sch[26] = key_sch[32]
    eors    r4, r3                          @ XOR key_sch[27] = key_sch[33]
    eors    r5, r4                          @ XOR key_sch[28] = key_sch[34]
    eors    r6, r5                          @ XOR key_sch[29] = key_sch[35]
    stmia   r2!, {r0-r1, r3-r6}             @ store key_sch[30-35], *key_sch++

# calculate key_sch[36-41]
    mov     ip, r5                          @ temporarily store key_sch[34]
    mov     lr, r6                          @ temporarily store key_sch[35]
    mov     r7, r8
    ldr     r5, [r7, #5<<2]                 @ load rcon[5]
    movs    r7, #24
    rors    r6, r7                          @ ROTL(key_sch[35],8)
    mov     r7, fp
    str     r6, [r7]                        @ ROTL(key_sch[35],8) -> acc
    mov     r6, r9
    mov     r7, sl
    str     r6, [r7]                        @ AES SubBytes
    mov     r6, fp
    adds    r6, #64
    ldr     r7, [r6]                        @ load CAA
    eors    r7, r5                          @ XOR rcon[5]
    mov     r5, ip                          @ restore key_sch[34]
    mov     r6, lr                          @ restore key_sch[35]
    eors    r0, r7                          @ XOR key_sch[30] = key_sch[36]
    eors    r1, r0                          @ XOR key_sch[31] = key_sch[37]
    eors    r3, r1                          @ XOR key_sch[32] = key_sch[38]
    eors    r4, r3                          @ XOR key_sch[33] = key_sch[39]
    eors    r5, r4                          @ XOR key_sch[34] = key_sch[40]
    eors    r6, r5                          @ XOR key_sch[35] = key_sch[41]
    stmia   r2!, {r0-r1, r3-r6}             @ store key_sch[35-41], *key_sch++

# calculate key_sch[42-47]
    mov     ip, r5                          @ temporarily store key_sch[40]
    mov     lr, r6                          @ temporarily store key_sch[41]
    mov     r7, r8
    ldr     r5, [r7, #6<<2]                 @ load rcon[6]
    movs    r7, #24
    rors    r6, r7                          @ ROTL(key_sch[41],8)
    mov     r7, fp
    str     r6, [r7]                        @ ROTL(key_sch[41],8) -> acc
    mov     r6, r9
    mov     r7, sl
    str     r6, [r7]                        @ AES SubBytes
    mov     r6, fp
    adds    r6, #64
    ldr     r7, [r6]                        @ load CAA
    eors    r7, r5                          @ XOR rcon[6]
    mov     r5, ip                          @ restore key_sch[40]
    mov     r6, lr                          @ restore key_sch[41]
    eors    r0, r7                          @ XOR key_sch[36] = key_sch[42]
    eors    r1, r0                          @ XOR key_sch[37] = key_sch[43]
    eors    r3, r1                          @ XOR key_sch[38] = key_sch[44]
    eors    r4, r3                          @ XOR key_sch[39] = key_sch[45]
    eors    r5, r4                          @ XOR key_sch[40] = key_sch[46]
    eors    r6, r5                          @ XOR key_sch[41] = key_sch[47]
    stmia   r2!, {r0-r1, r3-r6}             @ store key_sch[42-47], *key_sch++

# calculate key_sch[48-51]
    mov     r7, r8
    ldr     r5, [r7, #7<<2]                 @ load rcon[7]
    movs    r7, #24
    rors    r6, r7                          @ ROTL(key_sch[47],8)
    mov     r7, fp
    str     r6, [r7]                        @ ROTL(key_sch[47],8) -> acc
    mov     r6, r9
    mov     r7, sl
    str     r6, [r7]                        @ AES SubBytes
    mov     r6, fp
    adds    r6, #64
    ldr     r7, [r6]                        @ load CAA
    eors    r7, r5                          @ XOR rcon[7]
    eors    r0, r7                          @ XOR key_sch[42] = key_sch[48]
    eors    r1, r0                          @ XOR key_sch[43] = key_sch[49]
    eors    r3, r1                          @ XOR key_sch[44] = key_sch[50]
    eors    r4, r3                          @ XOR key_sch[45] = key_sch[51]
    stmia   r2!, {r0-r1, r3-r4}             @ store key_sch[48-51], *key_sch++

    b       set_key_end                     @ end routine


set_key_128:

#    REGISTER  |  ALLOCATION (throughout set_key_128)
#   -----------+------------------------------------------------------------
#          r0  |  rcon[i]
#          r1  |  scratch
#          r2  |  *key_sch
#          r3  |  key_sch[0+4i]
#          r4  |  key_sch[1+4i]
#          r5  |  key_sch[2+4i]
#          r6  |  key_sch[3+4i]
#          r7  |  scratch
#          r8  |  *rcon
#          r9  |  mmcau_1_cmd(AESS+CAA)
#    (sl) r10  |  *mmcau_direct_cmd()
#    (fp) r11  |  mmcau_indirect_cmd(LDR+CAA)
#    (ip) r12  |  mmcau_indirect_cmd(STR+CAA)
#    (sp) r13  |  stack pointer
#    (lr) r14  |  link register

# load some of the regs in preperation of the AES-128 set key calculations
    ldmia   r3, {r3-r7}    
    mov     r8, r3                          @ r8 = *rcon
    mov     r9, r4                          @ r9 = mmcau_1_cmd(AESS+CAA)
    mov     sl, r5                          @ sl = *mmcau_direct_cmd()
    mov     fp, r6                          @ fp = mmcau_indirect_cmd(LDR+CAA)
    mov     ip, r7                          @ ip = mmcau_indirect_cmd(STR+CAA)

# calculate key_sch[0-3]
    ldmia   r0!, {r3-r6}                    @ load key[0-3]
    rev     r3, r3                          @ byterev(key[0]) = key_sch[0]
    rev     r4, r4                          @ byterev(key[1]) = key_sch[1]
    rev     r5, r5                          @ byterev(key[2]) = key_sch[2]
    rev     r6, r6                          @ byterev(key[3]) = key_sch[3]
    stmia   r2!, {r3-r6}                    @ store key_sch[0-3], *key_sch++

# calculate key_sch[4-7]
    mov     r7, r8
    ldr     r0, [r7, #0<<2]                 @ load rcon[0]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[3]
    rors    r1, r7                          @ ROTL(key_sch[3],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[3],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[0]
    eors    r3, r7                          @ XOR key_sch[0]  = key_sch[4]
    eors    r4, r3                          @ XOR key_sch[1]  = key_sch[5]
    eors    r5, r4                          @ XOR key_sch[2]  = key_sch[6]
    eors    r6, r5                          @ XOR key_sch[3]  = key_sch[7]
    stmia   r2!, {r3-r6}                    @ store key_sch[4-7], *key_sch++

# calculate key_sch[8-11]
    mov     r7, r8
    ldr     r0, [r7, #1<<2]                 @ load rcon[1]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[7]
    rors    r1, r7                          @ ROTL(key_sch[7],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[7],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[1]
    eors    r3, r7                          @ XOR key_sch[4]  = key_sch[8]
    eors    r4, r3                          @ XOR key_sch[5]  = key_sch[9]
    eors    r5, r4                          @ XOR key_sch[6]  = key_sch[10]
    eors    r6, r5                          @ XOR key_sch[7]  = key_sch[11]
    stmia   r2!, {r3-r6}                    @ store key_sch[8-11], *key_sch++

# calculate key_sch[12-15]
    mov     r7, r8
    ldr     r0, [r7, #2<<2]                 @ load rcon[2]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[11]
    rors    r1, r7                          @ ROTL(key_sch[11],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[11],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[2]
    eors    r3, r7                          @ XOR key_sch[8]  = key_sch[12]
    eors    r4, r3                          @ XOR key_sch[9]  = key_sch[13]
    eors    r5, r4                          @ XOR key_sch[10] = key_sch[14]
    eors    r6, r5                          @ XOR key_sch[11] = key_sch[15]
    stmia   r2!, {r3-r6}                    @ store key_sch[12-15], *key_sch++

# calculate key_sch[16-19]
    mov     r7, r8
    ldr     r0, [r7, #3<<2]                 @ load rcon[3]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[15]
    rors    r1, r7                          @ ROTL(key_sch[15],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[15],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[3]
    eors    r3, r7                          @ XOR key_sch[12] = key_sch[16]
    eors    r4, r3                          @ XOR key_sch[13] = key_sch[17]
    eors    r5, r4                          @ XOR key_sch[14] = key_sch[18]
    eors    r6, r5                          @ XOR key_sch[15] = key_sch[19]
    stmia   r2!, {r3-r6}                    @ store key_sch[16-19], *key_sch++

# calculate key_sch[20-23]
    mov     r7, r8
    ldr     r0, [r7, #4<<2]                 @ load rcon[4]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[19]
    rors    r1, r7                          @ ROTL(key_sch[19],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[19],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[4]
    eors    r3, r7                          @ XOR key_sch[16] = key_sch[20]
    eors    r4, r3                          @ XOR key_sch[17] = key_sch[21]
    eors    r5, r4                          @ XOR key_sch[18] = key_sch[22]
    eors    r6, r5                          @ XOR key_sch[19] = key_sch[23]
    stmia   r2!, {r3-r6}                    @ store key_sch[20-23], *key_sch++

# calculate key_sch[24-27]
    mov     r7, r8
    ldr     r0, [r7, #5<<2]                 @ load rcon[5]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[23]
    rors    r1, r7                          @ ROTL(key_sch[23],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[23],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[5]
    eors    r3, r7                          @ XOR key_sch[20] = key_sch[24]
    eors    r4, r3                          @ XOR key_sch[21] = key_sch[25]
    eors    r5, r4                          @ XOR key_sch[22] = key_sch[26]
    eors    r6, r5                          @ XOR key_sch[23] = key_sch[27]
    stmia   r2!, {r3-r6}                    @ store key_sch[24-27], *key_sch++

# calculate key_sch[28-31]
    mov     r7, r8
    ldr     r0, [r7, #6<<2]                 @ load rcon[6]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[27]
    rors    r1, r7                          @ ROTL(key_sch[27],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[27],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[6]
    eors    r3, r7                          @ XOR key_sch[24] = key_sch[28]
    eors    r4, r3                          @ XOR key_sch[25] = key_sch[29]
    eors    r5, r4                          @ XOR key_sch[26] = key_sch[30]
    eors    r6, r5                          @ XOR key_sch[27] = key_sch[31]
    stmia   r2!, {r3-r6}                    @ store key_sch[28-31], *key_sch++

# calculate key_sch[32-35]
    mov     r7, r8
    ldr     r0, [r7, #7<<2]                 @ load rcon[7]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[31]
    rors    r1, r7                          @ ROTL(key_sch[31],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[31],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[7]
    eors    r3, r7                          @ XOR key_sch[28] = key_sch[32]
    eors    r4, r3                          @ XOR key_sch[29] = key_sch[33]
    eors    r5, r4                          @ XOR key_sch[30] = key_sch[34]
    eors    r6, r5                          @ XOR key_sch[31] = key_sch[35]
    stmia   r2!, {r3-r6}                    @ store key_sch[32-35], *key_sch++

# calculate key_sch[36-39]
    mov     r7, r8
    ldr     r0, [r7, #8<<2]                 @ load rcon[8]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[35]
    rors    r1, r7                          @ ROTL(key_sch[35],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[35],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[8]
    eors    r3, r7                          @ XOR key_sch[32] = key_sch[36]
    eors    r4, r3                          @ XOR key_sch[33] = key_sch[37]
    eors    r5, r4                          @ XOR key_sch[34] = key_sch[38]
    eors    r6, r5                          @ XOR key_sch[35] = key_sch[39]
    stmia   r2!, {r3-r6}                    @ store key_sch[36-39], *key_sch++

# calculate key_sch[40-43]
    mov     r7, r8
    ldr     r0, [r7, #9<<2]                 @ load rcon[9]
    movs    r7, #24
    mov     r1, r6                          @ copy key_sch[39]
    rors    r1, r7                          @ ROTL(key_sch[39],8)
    mov     r7, fp
    str     r1, [r7]                        @ ROTL(key_sch[39],8) -> acc
    mov     r1, r9
    mov     r7, sl
    str     r1, [r7]                        @ AES SubBytes
    mov     r1, ip
    ldr     r7, [r1]                        @ load CAA
    eors    r7, r0                          @ XOR rcon[9]
    eors    r3, r7                          @ XOR key_sch[36] = key_sch[40]
    eors    r4, r3                          @ XOR key_sch[37] = key_sch[41]
    eors    r5, r4                          @ XOR key_sch[38] = key_sch[42]
    eors    r6, r5                          @ XOR key_sch[39] = key_sch[43]
    stmia   r2!, {r3-r6}                    @ store key_sch[40-43], *key_sch++


set_key_end:

    pop     {r3-r7}                         @ restore high regs
    mov     r8, r3
    mov     r9, r4
    mov     sl, r5
    mov     fp, r6
    mov     ip, r7
    pop     {r4-r7, pc}                     @ restore low regs, exit routine


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   MMCAU_AES_ENCRYPT
#   Encrypts a single 16-byte block
#
#   ARGUMENTS
#   *in             pointer to 16-byte block of input plaintext
#   *key_sch        pointer to key schedule (44, 52, 60 longwords)
#   nr              number of AES rounds (10, 12, 14 = f(key_schedule))
#   *out            pointer to 16-byte block of output ciphertext
#
#
#   CALLING CONVENTION
#   void mmcau_aes_encrypt     (const unsigned char     *in,
#                               const unsigned char     *key_sch,
#                               const int               nr,
#                               unsigned char           *out)
#
#  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #
#
#    REGISTER  |  ALLOCATION (at the start of mmcau_aes_encrypt)
#   -----------+------------------------------------------------------------
#          r0  |  *in           (arg0)
#          r1  |  *key_sch      (arg1)
#          r2  |  nr            (arg2)
#          r3  |  *out          (arg3)
#              |
#        > r3  |  irrelevant
#
#
#    REGISTER  |  ALLOCATION (throughout mmcau_aes_encrypt)
#   -----------+------------------------------------------------------------
#          r0  |  mmcau_3_cmds(AESS+CA0,AESS+CA1,AESS+CA2)
#          r1  |  *key_sch
#          r2  |  *mmcau_direct_cmd()
#          r3  |  scratch
#          r4  |  key_sch[0+4i]
#          r5  |  key_sch[1+4i]
#          r6  |  key_sch[2+4i]
#          r7  |  key_sch[3+4i]
#          r8  |  mmcau_indirect_cmd(AESC+CA0)
#          r9  |  not used
#    (sl) r10  |  not used
#    (fp) r11  |  not used
#    (ip) r12  |  not used
#    (sp) r13  |  stack pointer
#    (lr) r14  |  mmcau_2_cmds(AESS+CA3,AESR)
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

    .global _mmcau_aes_encrypt
    .global mmcau_aes_encrypt
    .type   mmcau_aes_encrypt, %function
    .align  4

_mmcau_aes_encrypt:
mmcau_aes_encrypt:

# store nr and *out, we need them later in the routine
# store regs r4-r8, we need to restore them at the end of the routine
    push    {r2-r7, lr}                     @ store nr, *out, low regs, and lr
    mov     r4, r8
    push    {r4}                            @ store high reg

# XOR the first 4 keys into the 4 words of plaintext
    ldmia   r1!, {r4-r7}                    @ load first 4 keys, *key_sch++
    mov     lr, r1                          @ temporarily store *key_sch[4]
    ldmia   r0, {r0-r3}                     @ load plaintext
    rev     r0, r0
    rev     r1, r1
    rev     r2, r2
    rev     r3, r3
    eors    r4, r0
    eors    r5, r1
    eors    r6, r2
    eors    r7, r3
    ldr     r1, =MMCAU_PPB_INDIRECT+(LDR+CA0)<<2
    stmia   r1!, {r4-r7}                    @ store XOR results in CA[0-3]

# load some of the regs in preperation of the encryption
    ldr     r0, =encrypt_reg_data
    ldmia   r0, {r0-r3}
    mov     r8, r1                          @ r8 = mmcau_indirect_cmd(AESC+CA0)
    mov     r1, lr                          @ restore r1 = *key_sch[4]
    mov     lr, r3                          @ lr = mmcau_2_cmds(AESS+CA3,AESR)

# send a series of cau commands to perform the encryption
    str     r0, [r2]                        @ SubBytes
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns


    ldr     r3, [sp, #1<<2]                 @ load nr
    cmp     r3, #10                         @ check nr
    beq     encrypt_end                     @ if aes128, end routine
                                            @ else, continue on

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns


    ldr     r3, [sp, #1<<2]                 @ load nr
    cmp     r3, #12                         @ check nr
    beq     encrypt_end                     @ if aes192, end routine
                                            @ else, continue on

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows
    ldmia   r1!, {r4-r7}                    @ load next 4 keys, *key_sch++
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns


encrypt_end:

    str     r0, [r2]                        @ SubBytes
    mov     r3, lr
    str     r3, [r2]                        @ SubBytes, ShiftRows

# XOR the last 4 keys with the 4 words of ciphertext
    ldr     r0, =MMCAU_PPB_INDIRECT+(STR+CA0)<<2
    ldmia   r1!, {r4-r7}                    @ load last 4 keys
    ldmia   r0, {r0-r3}                     @ load ciphertext
    eors    r4, r0
    eors    r5, r1
    eors    r6, r2
    eors    r7, r3
    rev     r4, r4
    rev     r5, r5
    rev     r6, r6
    rev     r7, r7
    ldr     r1, [sp, #2<<2]                 @ get *out
    stmia   r1!, {r4-r7}                    @ store XOR results in out[0-3]

    pop     {r4}                            @ restore high reg
    mov     r8, r4
    add     sp, #2<<2                       @ set sp = *{r4-r7}
    pop     {r4-r7, pc}                     @ restore low regs, exit routine


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   MMCAU_AES_DECRYPT
#   Decrypts a single 16-byte block
#
#   ARGUMENTS
#   *in             pointer to 16-byte block of input chiphertext
#   *key_sch        pointer to key schedule (44, 52, 60 longwords)
#   nr              number of AES rounds (10, 12, 14 = f(key_schedule))
#   *out            pointer to 16-byte block of output plaintext
#
#
#   CALLING CONVENTION
#   void mmcau_aes_decrypt     (const unsigned char     *in,
#                               const unsigned char     *key_sch,
#                               const int               nr,
#                               unsigned char           *out)
#
#  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #
#
#    REGISTER  |  ALLOCATION (at the start of mmcau_aes_decrypt)
#   -----------+------------------------------------------------------------
#          r0  |  *in           (arg0)
#          r1  |  *key_sch      (arg1)
#          r2  |  nr            (arg2)
#          r3  |  *out          (arg3)
#              |
#        > r3  |  irrelevant
#
#
#    REGISTER  |  ALLOCATION (throughout mmcau_aes_decrypt)
#   -----------+------------------------------------------------------------
#          r0  |  mmcau_3_cmds(AESIR,AESIS+CA3,AESIS+CA)
#          r1  |  *key_sch
#          r2  |  *mmcau_direct_cmd()
#          r3  |  scratch
#          r4  |  *key_sch[0-4i]
#          r5  |  *key_sch[1-4i]
#          r6  |  *key_sch[2-4i]
#          r7  |  *key_sch[3-4i]
#          r8  |  mmcau_indirect_cmd(AESIC+CA0)
#          r9  |  not used
#    (sl) r10  |  not used
#    (fp) r11  |  not used
#    (ip) r12  |  not used
#    (sp) r13  |  stack pointer
#    (lr) r14  |  mmcau_2_cmds(AESIS+CA1,AESIS+CA0)
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

    .global _mmcau_aes_decrypt
    .global mmcau_aes_decrypt
    .type   mmcau_aes_decrypt, %function
    .align  4

_mmcau_aes_decrypt:
mmcau_aes_decrypt:

# store nr and *out, we need them later in the routine
# store regs r4-r8, we need to restore them at the end of the routine
    push    {r2-r7, lr}                     @ store nr, *out, low regs, and lr
    mov     r4, r8
    push    {r4}                            @ store high reg

# *key_sch is adjusted to define the end of the elements, such that
# the adjustment factor = f(nr) is defined by the expression:
#   end of key_sch = 4 * (nr + 1), where nr = {10, 12, 14}
    movs    r3, #28
    rors    r2, r3
    add     r1, r2                          @ calculate end of key_sch
    mov     lr, r1                          @ temporarily store end of key_sch

# XOR the last 4 keys into the 4 words of ciphertext
    ldmia   r1!, {r4-r7}                    @ load last 4 keys
    ldmia   r0, {r0-r3}                     @ load ciphertext
    rev     r0, r0
    rev     r1, r1
    rev     r2, r2
    rev     r3, r3
    eors    r4, r0
    eors    r5, r1
    eors    r6, r2
    eors    r7, r3
    ldr     r1, =MMCAU_PPB_INDIRECT+(LDR+CA0)<<2
    stmia   r1!, {r4-r7}                    @ store XOR results in CA[0-3]

# load some of the regs in preperation of the decryption
    ldr     r0, =decrypt_reg_data
    ldmia   r0, {r0-r3}
    mov     r8, r1                          @ r8 = mmcau_indirect_cmd(AESC+CA0)
    mov     r1, lr                          @ restore end of key_sch
    subs    r1, #4<<2                       @ *key_sch--
    mov     lr, r3                          @ lr = mmcau_2_cmds(AESS+CA3,AESR)

# send a series of cau commands to perform the decryption
    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns


    ldr     r3, [sp, #1<<2]                 @ restore nr
    cmp     r3, #10                         @ check nr
    beq     decrypt_end                     @ if aes128, end routine
                                            @ else, continue on

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns


    ldr     r3, [sp, #1<<2]                 @ restore nr
    cmp     r3, #12                         @ check nr
    beq     decrypt_end                     @ if aes192, end routine
                                            @ else, continue on

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns

    ldmia   r1!, {r4-r7}                    @ load previous 4 keys
    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    subs    r1, #8<<2                       @ *key_sch--
    str     r3, [r2]                        @ InvSubBytes
    mov     r3, r8
    stmia   r3!, {r4-r7}                    @ MixColumns


decrypt_end:

    str     r0, [r2]                        @ InvShiftRows, InvSubBytes
    mov     r3, lr
    str     r3, [r2]                        @ InvSubBytes

# XOR the first 4 keys with the 4 words of plaintext
    ldr     r0, =MMCAU_PPB_INDIRECT+(STR+CA0)<<2
    ldmia   r1!, {r4-r7}                    @ load first 4 keys
    ldmia   r0, {r0-r3}                     @ load plaintext
    eors    r4, r0
    eors    r5, r1
    eors    r6, r2
    eors    r7, r3
    rev     r4, r4
    rev     r5, r5
    rev     r6, r6
    rev     r7, r7
    ldr     r1, [sp, #2<<2]                 @ get *out
    stmia   r1!, {r4-r7}                    @ store XOR results in out[0-3]

    pop     {r4}                            @ restore high reg
    mov     r8, r4
    add     sp, #2<<2                       @ set sp = *{r4-r7}
    pop     {r4-r7, pc}                     @ restore low regs, exit routine


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

    .data


    .type   set_key_reg_data, %object
    .align  4

set_key_reg_data:
    .word   rcon                            @ r8
    .word   MMCAU_1_CMD+(AESS+CAA)<<22      @ r9
    .word   MMCAU_PPB_DIRECT                @ sl
    .word   MMCAU_PPB_INDIRECT+(LDR+CAA)<<2 @ fp
    .word   MMCAU_PPB_INDIRECT+(STR+CAA)<<2 @ ip


    .type   encrypt_reg_data, %object
    .align  4

encrypt_reg_data:
    .word   MMCAU_3_CMDS+(AESS+CA0)<<22+(AESS+CA1)<<11+AESS+CA2 @ r0
    .word   MMCAU_PPB_INDIRECT+(AESC+CA0)<<2                    @ r8
    .word   MMCAU_PPB_DIRECT                                    @ r2
    .word   MMCAU_2_CMDS+(AESS+CA3)<<22+(AESR)<<11              @ lr


    .type   decrypt_reg_data, %object
    .align  4

decrypt_reg_data:
    .word   MMCAU_3_CMDS+(AESIR)<<22+(AESIS+CA3)<<11+AESIS+CA2  @ r0
    .word   MMCAU_PPB_INDIRECT+(AESIC+CA0)<<2                   @ r8
    .word   MMCAU_PPB_DIRECT                                    @ r2
    .word   MMCAU_2_CMDS+(AESIS+CA1)<<22+(AESIS+CA0)<<11        @ lr


    .type   rcon, %object
    .align  4

#endif

rcon:
    .word 0x01000000
    .word 0x02000000
    .word 0x04000000
    .word 0x08000000
    .word 0x10000000
    .word 0x20000000
    .word 0x40000000
    .word 0x80000000
    .word 0x1b000000
    .word 0x36000000
