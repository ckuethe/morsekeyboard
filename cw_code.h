#ifndef _CWCODE_H_
#define _CWCODE_H_

/* Includes: */
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/atomic.h>
	#include <limits.h>

    #include <avr/pgmspace.h>
	#include <MyUSB/Common/Common.h>

#define LA 0x4
#define LB 0x5
#define LC 0x6
#define LD 0x7
#define LE 0x8
#define LF 0x9
#define LG 0xa
#define LH 0xb
#define LI 0xc
#define LJ 0xd
#define LK 0xe
#define LL 0xf
#define LM 0x10
#define LN 0x11
#define LO 0x12
#define LP 0x13
#define LQ 0x14
#define LR 0x15
#define LS 0x16
#define LT 0x17
#define LU 0x18
#define LV 0x19
#define LW 0x1a
#define LX 0x1b
#define LY 0x1c
#define LZ 0x1d
#define N0 0x27
#define N1 0x1e
#define N2 0x1f
#define N3 0x20
#define N4 0x21
#define N5 0x22
#define N6 0x23
#define N7 0x24
#define N8 0x25
#define N9 0x26
#define BSLASH 0x31
#define FSLASH 0x38
#define RSQUARE 0x2f
#define LSQUARE 0x30
#define LTH 0x2036 //<--- need shift
#define GT 0x2037
#define RPAREN 0x2026
#define LPAREN 0x2027
#define RCURLY 0x202f
#define LCURLY 0x2030
#define PERIOD 0x37
#define COMMA 0x36
#define QUESTION 0x2038
#define SEMICOLON 0x33
#define COLON 0x2033
#define DASH 0x2d
#define APOS 0x34
#define SPACE 0x2c
#define ENTER 0x28
#define BSPACE 0x2a
#define SHFT 0x2000 //<-- need shift
#define CTRL 0x0100
#define ALT 0x0400
/*
(1 << 0) = Keyboard Left Control
(1 << 1) = Keyboard Left Shift
(1 << 2) = Keyboard Left Alt
(1 << 3) = Keyboard Left GUI (most commonly the Windows key)
(1 << 4) = Keyboard Right Control
(1 << 5) = Keyboard Right Shift
(1 << 6) = Keyboard Right Alt
(1 << 7) = Keyboard Right GUI (most commonly the Windows key) 
*/
#define F1 0x3a
#define F2 0x3b
#define F3 0x3c
#define F4 0x3d
#define F5 0x3e
#define F6 0x3f
#define F7 0x40
#define F8 0x41
#define F9 0x42
#define F10 0x43
#define F11 0x44
#define F12 0x45
#define TOGGLEGRP 0xfefe
#define UP 0x60
#define DN 0x5a
#define LFT 0x5c
#define RT 0x5e



extern uint16_t cw_table[] PROGMEM;

#endif
