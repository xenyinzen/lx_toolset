#ifndef _KEYBOARD_TEST_H_
#define _KEYBOARD_TEST_H_

#define SCANCODE_A	0x1E
#define SCANCODE_B 	0x30	
#define SCANCODE_C 	0x2E	
#define SCANCODE_D 	0x20
#define SCANCODE_E 	0x12
#define SCANCODE_F 	0x21
#define SCANCODE_G 	0x22
#define SCANCODE_H 	0x23
#define SCANCODE_I 	0x17
#define SCANCODE_J 	0x24
#define SCANCODE_K 	0x25
#define SCANCODE_L 	0x26
#define SCANCODE_M 	0x32
#define SCANCODE_N 	0x31
#define SCANCODE_O 	0x18
#define SCANCODE_P 	0x19
#define SCANCODE_Q 	0x10
#define SCANCODE_R 	0x13
#define SCANCODE_S 	0x1F
#define SCANCODE_T 	0x14
#define SCANCODE_U 	0x16
#define SCANCODE_V 	0x2F
#define SCANCODE_W 	0x11
#define SCANCODE_X 	0x2D
#define SCANCODE_Y 	0x15
#define SCANCODE_Z 	0x2C
#define SCANCODE_0 	0xB 
#define SCANCODE_1 	0x2 
#define SCANCODE_2 	0x3 
#define SCANCODE_3 	0x4 
#define SCANCODE_4 	0x5 
#define SCANCODE_5 	0x6 
#define SCANCODE_6 	0x7 
#define SCANCODE_7 	0x8 
#define SCANCODE_8 	0x9 
#define SCANCODE_9 	0xA 
//#define SCANCODE_BACKSPACE 	0xE 
#define SCANCODE_SPACE 	0x39 
#define SCANCODE_TAB 	0xF 
#define SCANCODE_ENTER 	0x1C 
#define SCANCODE_ESC 	0x1 
#define SCANCODE_DEL 	0x53 
#define SCANCODE_CAPSLOCK 	0x3A 
#define SCANCODE_LEFTSHIFT 	0x2A 
#define SCANCODE_LEFTCTRL 	0x1D 
#define SCANCODE_LEFTALT 	0x38
#define SCANCODE_RIGHTSHIFT 	0x36 
//#define SCANCODE_RIGHTCTRL 	0x1D 
//#define SCANCODE_RIGHTALT 	0x38 
#define SCANCODE_F1 	0x3B 
#define SCANCODE_F2 	0x3C 
#define SCANCODE_F3 	0x3D 
#define SCANCODE_F4 	0x3E 	
#define SCANCODE_F5 	0x3F 	
#define SCANCODE_F6 	0x40 	
#define SCANCODE_F7 	0x41 	
#define SCANCODE_F8 	0x42 	
#define SCANCODE_F9 	0x43 	
#define SCANCODE_F10 	0x44 		
#define SCANCODE_F11 	0x57 	
#define SCANCODE_F12 	0x58 	
#define SCANCODE_INSERT 	0x52 
#define SCANCODE_HOME 	0x47 	
#define SCANCODE_END 	0x4F 	
#define SCANCODE_PAGEUP 	0x49 	
#define SCANCODE_PAGEDOWN 	0x51 	
#define SCANCODE_UPARROW 	0x48 	
#define SCANCODE_LEFTARROW 	0x4B 
#define SCANCODE_DOWNARROW 	0x50 
#define SCANCODE_RIGHTARROW 0x4D 	
#define SCANCODE_GRAVE 	0x29 	
#define SCANCODE_MINUS 	0xC 	
#define SCANCODE_EQUALS 	0xD 	
#define SCANCODE_BACKSLASH 	0x2B 	
#define SCANCODE_LEFTBRACKETS 	0x1A 
#define SCANCODE_RIGHTBRACKETS 	0x1B 	
#define SCANCODE_SEMICOLON 	0x27 
#define SCANCODE_APOSTROPHE 	0x28 	
#define SCANCODE_COMMA 	0x33 	
#define SCANCODE_DOT 	0x34 	
#define SCANCODE_SLASH 	0x35 		
#define SCANCODE_NUM 	0x45 	

#define LINE_WITDH 80

struct KeyPoint {
	char *keyname;
	int row, col;
	int is_pressed;
	int is_valid;
};

struct KeyPoint keys[128] = {
	"", 0, 0, 0, 0,
	"ESC", 8, 1, 0, 1,	//1
	"1", 10, 1, 0, 1,
	"2", 10, 9, 0, 1,
	"3", 10, 15, 0, 1,
	"4", 10, 21, 0, 1,
	"5", 10, 27, 0, 1,
	"6", 10, 33, 0, 1,
	"7", 10, 39, 0, 1,
	"8", 10, 45, 0, 1,
	"9", 10, 51, 0, 1,
	"0", 10, 57, 0, 1,	//11
	"-", 10, 63, 0, 1,
	"=", 10, 69, 0, 1,
	"BS", 10, 74, 0, 1,
	"Tab", 12, 1, 0, 1,
	"Q", 12, 8, 0, 1,	//16
	"W", 12, 14, 0, 1,
	"E", 12, 20, 0, 1,
	"R", 12, 26, 0, 1,
	"T", 12, 32, 0, 1,
	"Y", 12, 38, 0, 1,
	"U", 12, 44, 0, 1,
	"I", 12, 50, 0, 1,
	"O", 12, 56, 0, 1,
	"P", 12, 62, 0, 1,	//26
	"[", 12, 67, 0, 1,
	"]", 12, 72, 0, 1,
	"Enter", 14, 73, 0, 1,
	"LCtrl", 18, 6, 0, 1,
	"A", 14, 9, 0, 1,	//1
	"S", 14, 15, 0, 1,	//31
	"D", 14, 21, 0, 1,
	"F", 14, 27, 0, 1,
	"G", 14, 33, 0, 1,
	"H", 14, 39, 0, 1,
	"J", 14, 45, 0, 1,
	"K", 14, 51, 0, 1,
	"L", 14, 57, 0, 1,	//38
	";", 14, 62, 0, 1,
	"'", 14, 67, 0, 1,
	"`", 8, 6, 0, 1,
	"LShift", 16, 1, 0, 1,
	"\\", 12, 77, 0, 1,
	"Z", 16, 10, 0, 1,	//44
	"X", 16, 16, 0, 1,
	"C", 16, 22, 0, 1,
	"V", 16, 28, 0, 1,
	"B", 16, 34, 0, 1,
	"N", 16, 40, 0, 1,
	"M", 16, 46, 0, 1,	//50
	",", 16, 52, 0, 1,
	".", 16, 57, 0, 1,
	"/", 16, 62, 0, 1,
	"RShift", 16, 73, 0, 1,
	"", 0, 0, 0, 0,
	"LAlt", 18, 18, 0, 1,
	"Space", 18, 34, 0, 1,
	"Caps", 14, 1, 0, 1,
	"F1", 8, 10, 0, 1,	//59
	"F2", 8, 14, 0, 1,
	"F3", 8, 19, 0, 1,
	"F4", 8, 24, 0, 1,
	"F5", 8, 28, 0, 1,
	"F6", 8, 33, 0, 1,
	"F7", 8, 37, 0, 1,
	"F8", 8, 42, 0, 1,
	"F9", 8, 47, 0, 1,
	"F10", 8, 52, 0, 1,	//68
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,	//72
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,	//75
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,	//77
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,	//80
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,	//82
	"", 0, 0, 0, 0,	//83
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"F11", 8, 56, 0, 1,	//87
	"F12", 0, 0, 0, 0,	//88
//	"F12", 8, 61, 0, 1,	//88
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0,
	"", 0, 0, 0, 0
};


struct KeyPoint e0_keys[128] = {
	"", 0, 0, 0, 0,	//0
	"", 0, 0, 0, 0,	//1
	"", 0, 0, 0, 0,	//2
	"", 0, 0, 0, 0,	//3
	"", 0, 0, 0, 0,	//4
	"", 0, 0, 0, 0,	//5
	"", 0, 0, 0, 0,	//6
	"", 0, 0, 0, 0,	//7
	"", 0, 0, 0, 0,	//8
	"", 0, 0, 0, 0,	//9
	"", 0, 0, 0, 0,	//10
	"", 0, 0, 0, 0,	//11
	"", 0, 0, 0, 0,	//12
	"", 0, 0, 0, 0,	//13
	"", 0, 0, 0, 0,	//14
	"", 0, 0, 0, 0,	//15
	"", 0, 0, 0, 0,	//16
	"", 0, 0, 0, 0,	//17
	"", 0, 0, 0, 0,	//18
	"", 0, 0, 0, 0,	//19
	"", 0, 0, 0, 0,	//20
	"", 0, 0, 0, 0,	//21
	"", 0, 0, 0, 0,	//22
	"", 0, 0, 0, 0,	//23
	"", 0, 0, 0, 0,	//24
	"", 0, 0, 0, 0,	//25
	"", 0, 0, 0, 0,	//26
	"", 0, 0, 0, 0,	//27
	"", 0, 0, 0, 0,	//28
//	"Pause", 8, 66, 0, 1,	//29:0xe1 0x1D (Make:e1,1d,45,e1,9d,c5; Break:none)
	"Pause", 0, 0, 0, 0,	//29:0xe1 0x1D (Make:e1,1d,45,e1,9d,c5; Break:none)
	"", 0, 0, 0, 0,	//29:0x1D (RCtrl:Make e1,1d;Break e1,9d)
	"", 0, 0, 0, 0,	//31
	"", 0, 0, 0, 0,	//32
	"", 0, 0, 0, 0,	//33
	"", 0, 0, 0, 0,	//34
	"", 0, 0, 0, 0,	//35
	"", 0, 0, 0, 0,	//36
	"", 0, 0, 0, 0,	//37
	"", 0, 0, 0, 0,	//38
	"", 0, 0, 0, 0,	//39
	"", 0, 0, 0, 0,	//40
	"", 0, 0, 0, 0,	//41
	"PrintScreen", 0, 0, 0, 0,	//42:0x2A (Make:e0,2a,e0,37; Break:e0,b7,e0,aa)
	"", 0, 0, 0, 0,	//43
	"", 0, 0, 0, 0,	//44
	"", 0, 0, 0, 0,	//45
	"", 0, 0, 0, 0,	//46
	"", 0, 0, 0, 0,	//47
	"", 0, 0, 0, 0,	//48
	"", 0, 0, 0, 0,	//49
	"", 0, 0, 0, 0,	//50
	"", 0, 0, 0, 0,	//51
	"", 0, 0, 0, 0,	//52
	"", 0, 0, 0, 0,	//53
	"", 0, 0, 0, 0,	//54
	"", 0, 0, 0, 0,	//55
	"RAlt", 18, 50, 0, 1,	//56:0x38
	"", 0, 0, 0, 0,	//57
	"", 0, 0, 0, 0,	//58
	"", 0, 0, 0, 0,	//59
	"", 0, 0, 0, 0,	//60
	"", 0, 0, 0, 0,	//61
	"", 0, 0, 0, 0,	//62
	"", 0, 0, 0, 0,	//63
	"", 0, 0, 0, 0,	//64
	"", 0, 0, 0, 0,	//65
	"", 0, 0, 0, 0,	//66
	"", 0, 0, 0, 0,	//67
	"", 0, 0, 0, 0,	//68
	"", 0, 0, 0, 0,	//69
	"", 0, 0, 0, 0,	//70
	"Home", 0, 0, 0, 0,	//71:0x47 (e0,47; e0,c7)
	"Up", 16, 68, 0, 1,	//72:0x48 (e0,48; e0,c8)
	"PgUp", 8, 61, 0, 1,	//73:0x49 (e0,49; e0,c9)
//	"PageUp", 0, 0, 0, 0,	//73:0x49 (e0,49; e0,c9)
	"", 0, 0, 0, 0,	//74
	"Left", 18, 62, 0, 1,	//75:0x4b (e0,4b; e0,cb)
	"", 0, 0, 0, 0,	//76
	"Right", 18, 74, 0, 1,	//77:0x4d (e0,4d; e0,cd)
	"", 0, 0, 0, 0,	//78
	"End", 0, 0, 0, 0,	//79:0x4f (e0,4f; e0,cf)
	"Down", 18, 68, 0, 1,	//80:0x50 (e0,50; e0,d0)
	"PgDn", 8, 66, 0, 1,	//81
//	"PageDown", 0, 0, 0, 0,	//81
	"Ins", 8, 72, 0, 1,	//82:0x52 (e0,52; e0,d2)
	"Del", 8, 76, 0, 1,	//83:0x53 (e0,53; e0,d3)
	"", 0, 0, 0, 0,	//84
	"", 0, 0, 0, 0,	//85
	"", 0, 0, 0, 0,	//86
	"", 0, 0, 0, 0,	//87
	"", 0, 0, 0, 0,	//88
	"", 0, 0, 0, 0,	//89
	"", 0, 0, 0, 0,	//90
	"Menu", 18, 12, 0, 1,	//91:0x5B (LGui)
	"RGui", 0, 0, 0, 0,	//92:0x5C
	"Paste", 18, 56, 0, 1,	//93:0x5D (APPS)
	"", 0, 0, 0, 0,	//94
	"", 0, 0, 0, 0,	//95
	"", 0, 0, 0, 0,	//96
	"", 0, 0, 0, 0,	//97
	"", 0, 0, 0, 0,	//98
	"", 0, 0, 0, 0,	//99
	"", 0, 0, 0, 0,	//100
	"", 0, 0, 0, 0,	//101
	"", 0, 0, 0, 0,	//102
	"", 0, 0, 0, 0,	//103
	"", 0, 0, 0, 0,	//104
	"", 0, 0, 0, 0,	//105
	"Lctrl", 18, 8, 0, 0,//106
	"", 0, 0, 0, 0,	//107
	"", 0, 0, 0, 0,	//108
	"", 0, 0, 0, 0,	//109
	"", 0, 0, 0, 0,	//110
	"", 0, 0, 0, 0,	//111
	"", 0, 0, 0, 0,	//112
	"", 0, 0, 0, 0,	//113
	"", 0, 0, 0, 0,	//114
	"", 0, 0, 0, 0,	//115
	"", 0, 0, 0, 0,	//116
	"", 0, 0, 0, 0,	//117
	"", 0, 0, 0, 0,	//118
	"", 0, 0, 0, 0,	//119
	"", 0, 0, 0, 0,	//120
	"", 0, 0, 0, 0,	//121
	"", 0, 0, 0, 0,	//122
	"", 0, 0, 0, 0,	//123
	"", 0, 0, 0, 0,	//124
	"", 0, 0, 0, 0,	//125
	"", 0, 0, 0, 0,	//126
	"", 0, 0, 0, 0,	//127
};


static int is_a_console(int fd);
static int open_a_console(const char *fnam);
int getfd(const char *fnam);
static void get_mode(void);
static void clean_up(void);
static void die(int x);
static void watch_dog(int x);
int count_keys( unsigned char *buf, int n );

#endif /* _KEYBOARD_TEST_H_ */

