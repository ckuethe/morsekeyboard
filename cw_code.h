
#ifndef _CWCODE_H_
#define _CWCODE_H_

/* Includes: */
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/atomic.h>
	#include <limits.h>

    #include <avr/pgmspace.h>
	#include <MyUSB/Common/Common.h>

#define LE 0x8
#define LT 0x17
#define LI 0xc
#define LA 0x4
#define LN 0x11
#define LM 0x10
#define LS 0x16
#define LU 0x18
#define LR 0x15
#define LW 0x1a
#define LD 0x7
#define LK 0xe
#define LG 0xa
#define LO 0x12
#define LH 0xb
#define LV 0x19
#define LF 0x9
#define SPACE 0x2c
#define LL 0xf
#define ENTER 0x28
#define LP 0x13
#define LJ 0xd
#define LB 0x5
#define LX 0x1b
#define LC 0x6
#define LY 0x1c
#define LZ 0x1d
#define LQ 0x14
#define CTRL 0x0100
#define BSPACE 0x2a
#define N5 0x22
#define N4 0x21
#define TOGGLEGRP 0xfefe
#define N3 0x20
#define REPEAT 0xfefd
#define LCURLY 0x2030
#define N2 0x1f
#define UP 0x60
#define RT 0x5e
#define DN 0x5a
#define N1 0x1e
#define N6 0x23
#define FSLASH 0x38
#define SEMICOLON 0x33
#define RPAREN 0x2026
#define N7 0x24
#define RCURLY 0x202f
#define ALT 0x0400
#define N8 0x25
#define N9 0x26
#define N0 0x27
#define QUESTION 0x2038
#define UNDERSCORE 0x202D
#define LFT 0x5c
#define PERIOD 0x37
#define RSQUARE 0x2f
#define APOS 0x34
#define DASH 0x2d
#define LSQUARE 0x30
#define LPAREN 0x2027
#define ESC 0x29
#define SHFT 0x2000
#define COMMA 0x36
#define COLON 0x2033
#define BSLASH 0x31
#define DOLLAR 0x2021
#define F12 0x45
#define F11 0x44
#define F5 0x3e
#define F4 0x3d
#define F3 0x3c
#define F2 0x3b
#define F1 0x3a
#define F6 0x3f
#define F7 0x40
#define F8 0x41
#define F9 0x42
#define F10 0x43

extern uint16_t cw_table[] PROGMEM;

#endif

