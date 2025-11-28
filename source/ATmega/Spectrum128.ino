#include <PS2KeyRaw.h>

#define AX0 4
#define AX1 5
#define AX2 6
#define AX3 7
#define AY0 8
#define AY1 9
#define AY2 10
#define RSTMT 14
#define CSMT  15
#define DATMT 16
#define STBMT 17
#define DATAPIN 2
#define IRQPIN  3

/*
ZX Spectrum 40-key keyboard matrix:
         ┌─────────────────────────────────────────────────────┐
         │     ┌─────────────────────────────────────────┐     │
         │     │     ┌─────────────────────────────┐     │     │
         │     │     │     ┌─────────────────┐     │     │     │
         │     │     │     │     ┌─────┐     │     │     │     │
       ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐
 A11/ ─┤ 1 ├─┤ 2 ├─┤ 3 ├─┤ 4 ├─┤ 5 │ │ 6 ├─┤ 7 ├─┤ 8 ├─┤ 9 ├─┤ 0 ├─ A12/
       └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘
       ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐
 A10/ ─┤ Q ├─┤ W ├─┤ E ├─┤ R ├─┤ T │ │ Y ├─┤ U ├─┤ I ├─┤ O ├─┤ P ├─ A13/
       └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘
       ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐
  A9/ ─┤ A ├─┤ S ├─┤ D ├─┤ F ├─┤ G │ │ H ├─┤ J ├─┤ K ├─┤ L ├─┤Ent├─ A14/
       └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘
       ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐
  A8/ ─┤ CS├─┤ Z ├─┤ X ├─┤ C ├─┤ V │ │ B ├─┤ N ├─┤ M ├─┤ SS├─┤ SP├─ A15/
       └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘
         D0    D1    D2    D3    D4    D4    D3    D2    D1    D0

ZX Spectrum+ extended 58 keys keyboard:
 CS + 1 "EDIT"
 CS + 2 "CAPS LOCK"
 CS + 3 "TRUE VIDEO"
 CS + 4 "INV VIDEO"
 CS + 5 "LEFT"
 CS + 6 "DOWN"
 CS + 7 "UP"
 CS + 8 "RIGHT"
 CS + 9 "GRAPH"
 CS + 0 "DELETE"

 CS + SP "BREAK"
 CS + SS "EXTENDED MODE"

 SS + N ","
 SS + M "."
 SS + O ";"
 SS + P """
*/

/*
8x16 Analog Switch Array MT8816

The following table is the decoding truth table of the 7-bit address for MT8816 chip and the address table of 128 analog switches.

Intersection
Point                                                    Address
Port Y -        AY2   AY1   AY0   AX3   AX2   AX1   AX0    No.
Port X
Y0 - X0          0     0     0     0     0     0     0     00H
Y0 - X1          0     0     0     0     0     0     1     01H
Y0 - X2          0     0     0     0     0     1     0     02H
······
Y7 - X14         1     1     1     1     1     1     0     7EH
Y7 - X15         1     1     1     1     1     1     1     7FH

ZX Spectrum A8..A15 bus connected to MT8816 Y0..Y7 pins; D0..D4 bus connected to MT8816 X0..X4 pins.
*/
enum MT8816_AX
{
	D0 = 0,
	D1 = 1,
	D2 = 2,
	D3 = 3,
	D4 = 4
};

enum MT8816_AY
{
	A_8  = 0 << 4,
	A_9  = 1 << 4,
	A_10 = 2 << 4,
	A_11 = 3 << 4,
	A_12 = 4 << 4,
	A_13 = 5 << 4,
	A_14 = 6 << 4,
	A_15 = 7 << 4
};

enum zx_keys
{
	ZX_KEY_CS  = D0 | A_8,
	ZX_KEY_A   = D0 | A_9,
	ZX_KEY_Q   = D0 | A_10,
	ZX_KEY_1   = D0 | A_11,
	ZX_KEY_0   = D0 | A_12,
	ZX_KEY_P   = D0 | A_13,
	ZX_KEY_ENT = D0 | A_14,
	ZX_KEY_SP  = D0 | A_15,
	ZX_KEY_Z   = D1 | A_8,
	ZX_KEY_S   = D1 | A_9,
	ZX_KEY_W   = D1 | A_10,
	ZX_KEY_2   = D1 | A_11,
	ZX_KEY_9   = D1 | A_12,
	ZX_KEY_O   = D1 | A_13,
	ZX_KEY_L   = D1 | A_14,
	ZX_KEY_SS  = D1 | A_15,
	ZX_KEY_X   = D2 | A_8,
	ZX_KEY_D   = D2 | A_9,
	ZX_KEY_E   = D2 | A_10,
	ZX_KEY_3   = D2 | A_11,
	ZX_KEY_8   = D2 | A_12,
	ZX_KEY_I   = D2 | A_13,
	ZX_KEY_K   = D2 | A_14,
	ZX_KEY_M   = D2 | A_15,
	ZX_KEY_C   = D3 | A_8,
	ZX_KEY_F   = D3 | A_9,
	ZX_KEY_R   = D3 | A_10,
	ZX_KEY_4   = D3 | A_11,
	ZX_KEY_7   = D3 | A_12,
	ZX_KEY_U   = D3 | A_13,
	ZX_KEY_J   = D3 | A_14,
	ZX_KEY_N   = D3 | A_15,
	ZX_KEY_V   = D4 | A_8,
	ZX_KEY_G   = D4 | A_9,
	ZX_KEY_T   = D4 | A_10,
	ZX_KEY_5   = D4 | A_11,
	ZX_KEY_6   = D4 | A_12,
	ZX_KEY_Y   = D4 | A_13,
	ZX_KEY_H   = D4 | A_14,
	ZX_KEY_B   = D4 | A_15,

	NONE= 0x7F
};

uint8_t ps2_to_zx[128] = // PS/2 set 2 scancode to MTX8816 / ZX key matrix address mapping
{
	NONE,		// 0x0
	NONE,		// 0x1
	NONE,		// 0x2
	NONE,		// 0x3
	NONE,		// 0x4
	NONE,		// 0x5
	NONE,		// 0x6
	NONE,		// 0x7
	NONE,		// 0x8
	NONE,		// 0x9
	NONE,		// 0xA
	NONE,		// 0xB
	NONE,		// 0xC
	NONE,		// 0xD
	NONE,		// 0xE
	NONE,		// 0xF
	NONE,		// 0x10
	NONE,		// 0x11
	ZX_KEY_CS,	// 0x12 PS2_KEY_LSHIFT
	NONE,		// 0x13
	NONE,		// 0x14
	ZX_KEY_Q,	// 0x15 PS2_KEY_Q
	ZX_KEY_1,	// 0x16 PS2_KEY_1
	NONE,		// 0x17
	NONE,		// 0x18
	NONE,		// 0x19
	ZX_KEY_Z,	// 0x1A PS2_KEY_Z
	ZX_KEY_S,	// 0x1B PS2_KEY_S
	ZX_KEY_A,	// 0x1C PS2_KEY_A
	ZX_KEY_W,	// 0x1D PS2_KEY_W
	ZX_KEY_2,	// 0x1E PS2_KEY_2
	NONE,		// 0x1F
	NONE,		// 0x20
	ZX_KEY_C,	// 0x21 PS2_KEY_C
	ZX_KEY_X,	// 0x22 PS2_KEY_X
	ZX_KEY_D,	// 0x23 PS2_KEY_D
	ZX_KEY_E,	// 0x24 PS2_KEY_E
	ZX_KEY_4,	// 0x25 PS2_KEY_4
	ZX_KEY_3,	// 0x26 PS2_KEY_3
	NONE,		// 0x27
	NONE,		// 0x28
	ZX_KEY_SP,	// 0x29 PS2_KEY_SPACE
	ZX_KEY_V,	// 0x2A PS2_KEY_V
	ZX_KEY_F,	// 0x2B PS2_KEY_F
	ZX_KEY_T,	// 0x2C PS2_KEY_T
	ZX_KEY_R,	// 0x2D PS2_KEY_R
	ZX_KEY_5,	// 0x2E PS2_KEY_5
	NONE,		// 0x2F
	NONE,		// 0x30
	ZX_KEY_N,	// 0x31 PS2_KEY_N
	ZX_KEY_B,	// 0x32 PS2_KEY_B
	ZX_KEY_H,	// 0x33 PS2_KEY_H
	ZX_KEY_G,	// 0x34 PS2_KEY_G
	ZX_KEY_Y,	// 0x35 PS2_KEY_Y
	ZX_KEY_6,	// 0x36 PS2_KEY_6
	NONE,		// 0x37
	NONE,		// 0x38
	NONE,		// 0x39
	ZX_KEY_M,	// 0x3A PS2_KEY_M
	ZX_KEY_J,	// 0x3B PS2_KEY_J
	ZX_KEY_U,	// 0x3C PS2_KEY_U
	ZX_KEY_7,	// 0x3D PS2_KEY_7
	ZX_KEY_8,	// 0x3E PS2_KEY_8
	NONE,		// 0x3F
	NONE,		// 0x40
	NONE,		// 0x41
	ZX_KEY_K,	// 0x42 PS2_KEY_K
	ZX_KEY_I,	// 0x43 PS2_KEY_I
	ZX_KEY_O,	// 0x44 PS2_KEY_O
	ZX_KEY_0,	// 0x45 PS2_KEY_0
	ZX_KEY_9,	// 0x46 PS2_KEY_9
	NONE,		// 0x47
	NONE,		// 0x48
	NONE,		// 0x49
	NONE,		// 0x4A
	ZX_KEY_L,	// 0x4B PS2_KEY_L
	NONE,		// 0x4C
	ZX_KEY_P,	// 0x4D PS2_KEY_P
	NONE,		// 0x4E
	NONE,		// 0x4F
	NONE,		// 0x50
	NONE,		// 0x51
	NONE,		// 0x52
	NONE,		// 0x53
	NONE,		// 0x54
	NONE,		// 0x55
	NONE,		// 0x56
	NONE,		// 0x57
	NONE,		// 0x58
	ZX_KEY_SS,	// 0x59 PS2_KEY_RSHIFT
	ZX_KEY_ENT,	// 0x5A PS2_KEY_RETURN / Enter on main keyboard
	NONE,		// 0x5B
	NONE,		// 0x5C
	NONE,		// 0x5D
	NONE,		// 0x5E
	NONE,		// 0x5F
	NONE,		// 0x60
	NONE,		// 0x61
	NONE,		// 0x62
	NONE,		// 0x63
	NONE,		// 0x64
	NONE,		// 0x65
	ZX_KEY_0,	// 0x66 PS2_KEY_BACK / Backspace
	NONE,		// 0x67
	NONE,		// 0x68
	NONE,		// 0x69
	NONE,		// 0x6A
	ZX_KEY_5,	// 0x6B PS2_KEY_NUMPAD4
	NONE,		// 0x6C
	NONE,		// 0x6D
	NONE,		// 0x6E
	NONE,		// 0x6F
	NONE,		// 0x70
	NONE,		// 0x71
	NONE,		// 0x72
	NONE,		// 0x73
	NONE,		// 0x74
	NONE,		// 0x75
	NONE,		// 0x76
	NONE,		// 0x77
	NONE,		// 0x78
	NONE,		// 0x79
	NONE,		// 0x7A
	NONE,		// 0x7B
	NONE,		// 0x7C
	NONE,		// 0x7D
	NONE,		// 0x7E
	NONE,		// 0x7F
};

/*
 Reference:
 The PS/2 Keyboard Interface
 Source: http://www.Computer-Engineering.org/ps2keyboard/
 Author: Adam Chapweske
 Keyboard Scan Codes: Set 2

 Notes for PS/2 keyboard scan codes set 2 (default):

 - Following keys hame multi-byte scan codes:
  Key         Make Code   Break Code
  L GUI       E0,1F       E0,F0,1F
  R CTRL      E0,14       E0,F0,14
  R GUI       E0,27       E0,F0,27
  R ALT       E0,11       E0,F0,11
  APPS        E0,2F       E0,F0,2F
  PRNT SCRN   E0,12,      E0,F0,7C,
              E0,7C       E0,F0,12
  PAUSE       E1,14,77,
              E1,F0,14,   -NONE-
              F0,77
  INSERT      E0,70       E0,F0,70
  HOME        E0,6C       E0,F0,6C
  PG UP       E0,7D       E0,F0,7D
  DELETE      E0,71       E0,F0,71
  END         E0,69       E0,F0,69
  PG DN       E0,7A       E0,F0,7A
  U ARROW     E0,75       E0,F0,75
  L ARROW     E0,6B       E0,F0,6B
  D ARROW     E0,72       E0,F0,72
  R ARROW     E0,74       E0,F0,74
  KP /        E0,4A       E0,F0,4A
  KP EN       E0,5A       E0,F0,5A

 - Pause/Break key does not have a break code

 ACPI Scan Codes:
  Key   Make   Break
  Power E0,37  E0,F0,37
  Sleep E0,3F  E0,F0,3F
  Wake  E0,5E  E0,F0,5E

 Windows Multimedia Scan Codes:
  Key            Make   Break
  Next Track     E0,4D  E0,F0,4D
  Previous Track E0,15  E0,F0,15
  Stop           E0,3B  E0,F0,3B
  Play/Pause     E0,34  E0,F0,34
  Mute           E0,23  E0,F0,23
  Volume Up      E0,32  E0,F0,32
  Volume Down    E0,21  E0,F0,21
  Media Select   E0,50  E0,F0,50
  E-Mail         E0,48  E0,F0,48
  Calculator     E0,2B  E0,F0,2B
  My Computer    E0,40  E0,F0,40
  WWW Search     E0,10  E0,F0,10
  WWW Home       E0,3A  E0,F0,3A
  WWW Back       E0,38  E0,F0,38
  WWW Forward    E0,30  E0,F0,30
  WWW Stop       E0,28  E0,F0,28
  WWW Refresh    E0,20  E0,F0,20
  WWW Favorites  E0,18  E0,F0,18
*/

enum ps2_scan_codes_set_2
{
//                     PS/2 SET 2, PS/2 SET 1 (XT/BIOS PC)
	PS2_KEY_ESCAPE       = 0x76 /*0x01*/,
	PS2_KEY_1            = 0x16 /*0x02*/,
	PS2_KEY_2            = 0x1E /*0x03*/,
	PS2_KEY_3            = 0x26 /*0x04*/,
	PS2_KEY_4            = 0x25 /*0x05*/,
	PS2_KEY_5            = 0x2E /*0x06*/,
	PS2_KEY_6            = 0x36 /*0x07*/,
	PS2_KEY_7            = 0x3D /*0x08*/,
	PS2_KEY_8            = 0x3E /*0x09*/,
	PS2_KEY_9            = 0x46 /*0x0A*/,
	PS2_KEY_0            = 0x45 /*0x0B*/,
	PS2_KEY_MINUS        = 0x4E /*0x0C*/, // - on main keyboard
	PS2_KEY_EQUALS       = 0x55 /*0x0D*/,
	PS2_KEY_BACK         = 0x66 /*0x0E*/, // backspace
	PS2_KEY_TAB          = 0x0D /*0x0F*/,
	PS2_KEY_Q            = 0x15 /*0x10*/,
	PS2_KEY_W            = 0x1D /*0x11*/,
	PS2_KEY_E            = 0x24 /*0x12*/,
	PS2_KEY_R            = 0x2D /*0x13*/,
	PS2_KEY_T            = 0x2C /*0x14*/,
	PS2_KEY_Y            = 0x35 /*0x15*/,
	PS2_KEY_U            = 0x3C /*0x16*/,
	PS2_KEY_I            = 0x43 /*0x17*/,
	PS2_KEY_O            = 0x44 /*0x18*/,
	PS2_KEY_P            = 0x4D /*0x19*/,
	PS2_KEY_LBRACKET     = 0x54 /*0x1A*/,
	PS2_KEY_RBRACKET     = 0x5B /*0x1B*/,
	PS2_KEY_RETURN       = 0x5A /*0x1C*/, // Enter on main keyboard
	PS2_KEY_LCONTROL     = 0x14 /*0x1D*/,
	PS2_KEY_A            = 0x1C /*0x1E*/,
	PS2_KEY_S            = 0x1B /*0x1F*/,
	PS2_KEY_D            = 0x23 /*0x20*/,
	PS2_KEY_F            = 0x2B /*0x21*/,
	PS2_KEY_G            = 0x34 /*0x22*/,
	PS2_KEY_H            = 0x33 /*0x23*/,
	PS2_KEY_J            = 0x3B /*0x24*/,
	PS2_KEY_K            = 0x42 /*0x25*/,
	PS2_KEY_L            = 0x4B /*0x26*/,
	PS2_KEY_SEMICOLON    = 0x4C /*0x27*/,
	PS2_KEY_APOSTROPHE   = 0x52 /*0x28*/,
	PS2_KEY_GRAVE        = 0x0E /*0x29*/, // accent grave
	PS2_KEY_LSHIFT       = 0x12 /*0x2A*/,
	PS2_KEY_BACKSLASH    = 0x5D /*0x2B*/,
	PS2_KEY_Z            = 0x1A /*0x2C*/,
	PS2_KEY_X            = 0x22 /*0x2D*/,
	PS2_KEY_C            = 0x21 /*0x2E*/,
	PS2_KEY_V            = 0x2A /*0x2F*/,
	PS2_KEY_B            = 0x32 /*0x30*/,
	PS2_KEY_N            = 0x31 /*0x31*/,
	PS2_KEY_M            = 0x3A /*0x32*/,
	PS2_KEY_COMMA        = 0x41 /*0x33*/,
	PS2_KEY_PERIOD       = 0x49 /*0x34*/, // . on main keyboard
	PS2_KEY_SLASH        = 0x4A /*0x35*/, // / on main keyboard
	PS2_KEY_RSHIFT       = 0x59 /*0x36*/,
	PS2_KEY_MULTIPLY     = 0x7C /*0x37*/, // * on numeric keypad
	PS2_KEY_LMENU        = 0x11 /*0x38*/, // left Alt
	PS2_KEY_SPACE        = 0x29 /*0x39*/,
	PS2_KEY_CAPITAL      = 0x58 /*0x3A*/,
	PS2_KEY_F1           = 0x05 /*0x3B*/,
	PS2_KEY_F2           = 0x06 /*0x3C*/,
	PS2_KEY_F3           = 0x04 /*0x3D*/,
	PS2_KEY_F4           = 0x0C /*0x3E*/,
	PS2_KEY_F5           = 0x03 /*0x3F*/,
	PS2_KEY_F6           = 0x0B /*0x40*/,
	PS2_KEY_F7           = 0x83 /*0x41*/,
	PS2_KEY_F8           = 0x0A /*0x42*/,
	PS2_KEY_F9           = 0x01 /*0x43*/,
	PS2_KEY_F10          = 0x09 /*0x44*/,
	PS2_KEY_NUMLOCK      = 0x77 /*0x45*/,
	PS2_KEY_SCROLL       = 0x7E /*0x46*/, // Scroll Lock
	PS2_KEY_NUMPAD7      = 0x6C /*0x47*/,
	PS2_KEY_NUMPAD8      = 0x75 /*0x48*/,
	PS2_KEY_NUMPAD9      = 0x7D /*0x49*/,
	PS2_KEY_SUBTRACT     = 0x7B /*0x4A*/, // - on numeric keypad
	PS2_KEY_NUMPAD4      = 0x6B /*0x4B*/,
	PS2_KEY_NUMPAD5      = 0x73 /*0x4C*/,
	PS2_KEY_NUMPAD6      = 0x74 /*0x4D*/,
	PS2_KEY_ADD          = 0x79 /*0x4E*/, // + on numeric keypad
	PS2_KEY_NUMPAD1      = 0x69 /*0x4F*/,
	PS2_KEY_NUMPAD2      = 0x72 /*0x50*/,
	PS2_KEY_NUMPAD3      = 0x7A /*0x51*/,
	PS2_KEY_NUMPAD0      = 0x70 /*0x52*/,
	PS2_KEY_DECIMAL      = 0x71 /*0x53*/, // . on numeric keypad
	PS2_KEY_F11          = 0x78 /*0x57*/,
	PS2_KEY_F12          = 0x07 /*0x58*/
#if 0
	PS2_KEY_NEXTTRACK    = 0xE0, 0x4D /*0x99*/, // Next Track
	PS2_KEY_NUMPADENTER  = 0xE0, 0x5A /*0x9C*/, // Enter on numeric keypad
	PS2_KEY_RCONTROL     = 0xE0, 0x14 /*0x9D*/,
	PS2_KEY_MUTE         = 0xE0, 0x23 /*0xA0*/, // Mute
	PS2_KEY_CALCULATOR   = 0xE0, 0x2B /*0xA1*/, // Calculator
	PS2_KEY_PLAYPAUSE    = 0xE0, 0x34 /*0xA2*/, // Play / Pause
	PS2_KEY_MEDIASTOP    = 0xE0, 0x3B /*0xA4*/, // Media Stop
	PS2_KEY_VOLUMEDOWN   = 0xE0, 0x21 /*0xAE*/, // Volume -
	PS2_KEY_VOLUMEUP     = 0xE0, 0x32 /*0xB0*/, // Volume +
	PS2_KEY_WEBHOME      = 0xE0, 0x3A /*0xB2*/, // Web home
	PS2_KEY_DIVIDE       = 0xE0, 0x4A /*0xB5*/, // / on numeric keypad
	PS2_KEY_SYSRQ        = 0xE0, 0x12, 0xE0, 0x7C /*0xB7*/,
	PS2_KEY_RMENU        = 0xE0, 0x11 /*0xB8*/, // Right Alt
	PS2_KEY_PAUSE        = 0xE1, 0x14, 77, E1, F0, 14, F0, 77 /*0xC5*/, // Pause
	PS2_KEY_HOME         = 0xE0, 0x6C /*0xC7*/, // Home on arrow keypad
	PS2_KEY_UP           = 0xE0, 0x75 /*0xC8*/, // UpArrow on arrow keypad
	PS2_KEY_PRIOR        = 0xE0, 0x7D /*0xC9*/, // PgUp on arrow keypad
	PS2_KEY_LEFT         = 0xE0, 0x6B /*0xCB*/, // LeftArrow on arrow keypad
	PS2_KEY_RIGHT        = 0xE0, 0x74 /*0xCD*/, // RightArrow on arrow keypad
	PS2_KEY_END          = 0xE0, 0x69 /*0xCF*/, // End on arrow keypad
	PS2_KEY_DOWN         = 0xE0, 0x72 /*0xD0*/, // DownArrow on arrow keypad
	PS2_KEY_NEXT         = 0xE0, 0x7A /*0xD1*/, // PgDn on arrow keypad
	PS2_KEY_INSERT       = 0xE0, 0x70 /*0xD2*/, // Insert on arrow keypad
	PS2_KEY_DELETE       = 0xE0, 0x71 /*0xD3*/, // Delete on arrow keypad
	PS2_KEY_LWIN         = 0xE0, 0x1F /*0xDB*/, // Left Windows key
	PS2_KEY_RWIN         = 0xE0, 0x27 /*0xDC*/, // Right Windows key
	PS2_KEY_APPS         = 0xE0, 0x2F /*0xDD*/, // AppMenu key
	PS2_KEY_POWER        = 0xE0, 0x37 /*0xDE*/, // System Power
	PS2_KEY_SLEEP        = 0xE0, 0x3F /*0xDF*/, // System Sleep
	PS2_KEY_WAKE         = 0xE0, 0x5E /*0xE3*/, // System Wake
	PS2_KEY_WEBSEARCH    = 0xE0, 0x10 /*0xE5*/, // Web Search
	PS2_KEY_WEBFAVORITES = 0xE0, 0x18 /*0xE6*/, // Web Favorites
	PS2_KEY_WEBREFRESH   = 0xE0, 0x20 /*0xE7*/, // Web Refresh
	PS2_KEY_WEBSTOP      = 0xE0, 0x28 /*0xE8*/, // Web Stop
	PS2_KEY_WEBFORWARD   = 0xE0, 0x30 /*0xE9*/, // Web Forward
	PS2_KEY_WEBBACK      = 0xE0, 0x38 /*0xEA*/, // Web Back
	PS2_KEY_MYCOMPUTER   = 0xE0, 0x40 /*0xEB*/, // My Computer
	PS2_KEY_MAIL         = 0xE0, 0x48 /*0xEC*/, // Mail
	PS2_KEY_MEDIASELECT  = 0xE0, 0x50 /*0xED*/   // Media Select
#endif
};

/*
 https://zx-pk.ru/threads/33211-universalnyj-kontroller-ps-2-klaviatury-dlya-kompyuterov-s-matrichnymi-klaviaturami.html
 Xrust PS/2 Keyboard Controller mappings:
  Code    Key   ZX Key
  66      BKSP  [BS]
  (E0),6B LEFT  [Left]
  (E0),72 DOWN  [Down]
  (E0),75 UP    [Up]
  (E0),74 RIGHT [Right]
  76      ESC   [Edit]
  58      CAPS  [Caps lock]
  0D      TAB   [Ext mode]
  41      ,     [,]
  49      .     [.]
  4C      ;     [;]
  52      '     ["]

 Note:
  With Xrust controller firmware, keys with multi-byte scan codes
  handled as keys with single-byte scan codes because of 0xE0 leading byte ignored:

  PS2_KEY_RCONTROL 0xE0, 0x14  "Right Control"              PS2_KEY_LCONTROL 0x14 "Left Control"
  PS2_KEY_DIVIDE   0xE0, 0x4A  "/ on numeric keypad"        PS2_KEY_SLASH    0x4A "/ on main keyboard"
  PS2_KEY_SYSRQ    0xE0, 0x12, "PrintScrn/SysRq"            PS2_KEY_LSHIFT   0x12 "Left Shift"
                   0xE0, 0x7C                               PS2_KEY_MULTIPLY 0x7C "* on numeric keypad"
  PS2_KEY_RMENU    0xE0, 0x11  "Right Alt"                  PS2_KEY_LMENU    0x11 "Left Alt"
  PS2_KEY_UP       0xE0, 0x75  "UpArrow on arrow keypad"    PS2_KEY_NUMPAD8  0x75
  PS2_KEY_LEFT     0xE0, 0x6B  "LeftArrow on arrow keypad"  PS2_KEY_NUMPAD4  0x6B
  PS2_KEY_RIGHT    0xE0, 0x74  "RightArrow on arrow keypad" PS2_KEY_NUMPAD6  0x74
  PS2_KEY_DOWN     0xE0, 0x72  "DownArrow on arrow keypad"  PS2_KEY_NUMPAD2  0x72
  PS2_KEY_DELETE   0xE0, 0x71  "Delete on arrow keypad"     PS2_KEY_DECIMAL  0x71 ". on numeric keypad"
*/

PS2KeyRaw keyboard;

void setup()
{
	//Инициализация портов
	pinMode(AX0, OUTPUT);   //AX0
	pinMode(AX1, OUTPUT);   //AX1
	pinMode(AX2, OUTPUT);   //AX2
	pinMode(AX3, OUTPUT);   //AX3
	pinMode(AY0, OUTPUT);   //AY0
	pinMode(AY1, OUTPUT);   //AY1
	pinMode(AY2, OUTPUT);   //AY2
	pinMode(RSTMT, OUTPUT); //RES
	pinMode(CSMT, OUTPUT);  //CS
	pinMode(DATMT, OUTPUT); //DAT
	pinMode(STBMT, OUTPUT); //STB

	//Инициализация клавиатуры
	keyboard.begin( DATAPIN, IRQPIN );

	//Инициализация MT8816
	SetAddr(0);
	digitalWrite(RSTMT, LOW);
	digitalWrite(CSMT, LOW);
	digitalWrite(DATMT, LOW);
	digitalWrite(STBMT, LOW); //инициализация
	digitalWrite(CSMT, HIGH); //выбор чипа
	digitalWrite(RSTMT, HIGH);
	digitalWrite(RSTMT, LOW); //сброс
	digitalWrite(CSMT, LOW);

	SetAddr(ZX_KEY_CS);
	SetKey(LOW);
}

void SetAddr(uint8_t addr)
{
	digitalWrite(AX0,bitRead(addr,0));
	digitalWrite(AX1,bitRead(addr,1));
	digitalWrite(AX2,bitRead(addr,2));
	digitalWrite(AX3,bitRead(addr,3));
	digitalWrite(AY0,bitRead(addr,4));
	digitalWrite(AY1,bitRead(addr,5));
	digitalWrite(AY2,bitRead(addr,6));
}

void SetKey(boolean data){
	digitalWrite(CSMT, HIGH); //выбор чипа
	digitalWrite(STBMT, HIGH); //строб on
	digitalWrite(DATMT, data); //данные
	digitalWrite(STBMT, LOW); //строб off
	digitalWrite(CSMT, LOW);
}

void loop()
{
	boolean state = HIGH;

	if( keyboard.available() )
	{
		int c = keyboard.read(); //чтение кода
		switch (c)
		{
			case 0xE0:				//если считался префикс 0xE0 (2-byte extended key code prefix)
				break;
			case 0xF0:				//если считался префикс 0xF0 (отпускание клавиши)
				state = LOW;
				break;
			case PS2_KEY_BACK:		//если считался код 0x66 [BS]
				SetAddr(ZX_KEY_CS);
				SetKey(state);
				SetAddr(ZX_KEY_0);
				SetKey(state);
				state = HIGH;
				break;
			case 0x6B:				//если считался код 0x6B [Left] PS2_KEY_NUMPAD4 0x6B / PS2_KEY_LEFT 0xE0 0x6B
				SetAddr(ZX_KEY_CS);
				SetKey(state);
				SetAddr(ZX_KEY_5);
				SetKey(state);
				state = HIGH;
				break;
			case 0x72:				//если считался код 0x72 [Down] PS2_KEY_NUMPAD2 0x72 / PS2_KEY_DOWN 0xE0 0x72
				SetAddr(ZX_KEY_CS);
				SetKey(state);
				SetAddr(ZX_KEY_6);
				SetKey(state);
				state = HIGH;
				break;
			case 0x75:				//если считался код 0x75 [Up] PS2_KEY_NUMPAD8 0x75 / PS2_KEY_UP 0xE0 0x75
				SetAddr(ZX_KEY_CS);
				SetKey(state);
				SetAddr(ZX_KEY_7);
				SetKey(state);
				state = HIGH;
				break;
			case 0x74:				//если считался код 0x74 [Right] PS2_KEY_NUMPAD6 0x74 / PS2_KEY_RIGHT 0xE0 0x74
				SetAddr(ZX_KEY_CS);
				SetKey(state);
				SetAddr(ZX_KEY_8);
				SetKey(state);
				state = HIGH;
				break;
			case PS2_KEY_ESCAPE:	//если считался код 0x76 [Edit]
				SetAddr(ZX_KEY_CS);
				SetKey(state);
				SetAddr(ZX_KEY_1);
				SetKey(state);
				state = HIGH;
				break;
			case PS2_KEY_CAPITAL:	//если считался код 0x58 [Caps lock]
				SetAddr(ZX_KEY_CS);
				SetKey(state);
				SetAddr(ZX_KEY_2);
				SetKey(state);
				state = HIGH;
				break;
			case PS2_KEY_TAB:		//если считался код 0x0D [Ext mode]
				SetAddr(ZX_KEY_CS);
				SetKey(state);
				SetAddr(ZX_KEY_SS);
				SetKey(state);
				state = HIGH;
				break;
			case PS2_KEY_COMMA:		//если считался код 0x41 [,]
				SetAddr(ZX_KEY_SS);
				SetKey(state);
				SetAddr(ZX_KEY_N);
				SetKey(state);
				state = HIGH;
				break;
			case PS2_KEY_PERIOD:	//если считался код 0x49 [.]
				SetAddr(ZX_KEY_SS);
				SetKey(state);
				SetAddr(ZX_KEY_M);
				SetKey(state);
				state = HIGH;
				break;
			case PS2_KEY_SEMICOLON: //если считался код 0x4C [;]
				SetAddr(ZX_KEY_SS);
				SetKey(state);
				SetAddr(ZX_KEY_O);
				SetKey(state);
				state = HIGH;
				break;
			case PS2_KEY_APOSTROPHE: //если считался код 0x52 ["]
				SetAddr(ZX_KEY_SS);
				SetKey(state);
				SetAddr(ZX_KEY_P);
				SetKey(state);
				state = HIGH;
				break;
			default:
				SetAddr(ps2_to_zx[c]);
				SetKey(state);
				state = HIGH;
		}
	}
}
