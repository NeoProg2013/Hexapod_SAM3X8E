//  ***************************************************************************
/// @file    oled_gl_font_6x8.h
/// @author  NeoProg
/// @brief   Font 6x8 for OLED display
//  ***************************************************************************
#ifndef OLED_GL_FONT_6X8_H_
#define OLED_GL_FONT_6X8_H_

static const uint8_t font_6x8[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char
    0x00, 0x00, 0x00, 0xBF, 0x00, 0x00,      // Code for char !
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char "
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char #
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char $
    0x47, 0x25, 0x17, 0xE8, 0xA4, 0xE2,      // Code for char %
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char &
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char '
    0x00, 0x00, 0x00, 0x7E, 0x81, 0x00,      // Code for char (
    0x00, 0x00, 0x81, 0x7E, 0x00, 0x00,      // Code for char )
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char *
    0x00, 0x10, 0x10, 0x7C, 0x10, 0x10,      // Code for char +
    0x00, 0x00, 0xC0, 0x00, 0x00, 0x00,      // Code for char ,
    0x00, 0x10, 0x10, 0x10, 0x10, 0x00,      // Code for char -
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00,      // Code for char .
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char /
    0x00, 0x7E, 0x81, 0x81, 0x81, 0x7E,      // Code for char 0
    0x00, 0x84, 0x82, 0xFF, 0x80, 0x80,      // Code for char 1
    0x00, 0xC2, 0xA1, 0x91, 0x91, 0x8E,      // Code for char 2
    0x00, 0xC3, 0x89, 0x89, 0xF7, 0x00,      // Code for char 3
    0x00, 0x1F, 0x10, 0x10, 0xFF, 0x10,      // Code for char 4
    0x00, 0x4F, 0x89, 0x89, 0x89, 0x71,      // Code for char 5
    0x00, 0x7E, 0x89, 0x89, 0x89, 0x72,      // Code for char 6
    0x00, 0x01, 0xC1, 0x31, 0x0D, 0x03,      // Code for char 7
    0x00, 0x76, 0x89, 0x89, 0x89, 0x76,      // Code for char 8
    0x00, 0x4E, 0x91, 0x91, 0x91, 0x7E,      // Code for char 9
    0x00, 0x00, 0x00, 0x22, 0x00, 0x00,      // Code for char :
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char ;
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char <
    0x00, 0x28, 0x28, 0x28, 0x28, 0x28,      // Code for char =
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char >
    0x00, 0x02, 0x01, 0xA1, 0x11, 0x0E,      // Code for char ?
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char @
    0x00, 0xF0, 0x2E, 0x23, 0x3C, 0xC0,      // Code for char A
    0x00, 0xFF, 0x89, 0x89, 0x89, 0x76,      // Code for char B
    0x00, 0x7E, 0x81, 0x81, 0x81, 0x42,      // Code for char C
    0x00, 0xFF, 0x81, 0x81, 0x81, 0x7E,      // Code for char D
    0x00, 0xFF, 0x89, 0x89, 0x89, 0x81,      // Code for char E
    0x00, 0xFF, 0x09, 0x09, 0x09, 0x01,      // Code for char F
    0x00, 0x7E, 0x81, 0x91, 0x91, 0xF2,      // Code for char G
    0x00, 0xFF, 0x08, 0x08, 0x08, 0xFF,      // Code for char H
    0x00, 0x81, 0x81, 0xFF, 0x81, 0x81,      // Code for char I
    0x00, 0x40, 0x80, 0x80, 0x7F, 0x00,      // Code for char J
    0x00, 0xFF, 0x08, 0x14, 0x62, 0x81,      // Code for char K
    0x00, 0xFF, 0x80, 0x80, 0x80, 0x80,      // Code for char L
    0x00, 0xFF, 0x0C, 0x10, 0x0C, 0xFF,      // Code for char M
    0x00, 0xFF, 0x04, 0x18, 0x20, 0xFF,      // Code for char N
    0x00, 0x7E, 0x81, 0x81, 0x81, 0x7E,      // Code for char O
    0x00, 0xFF, 0x11, 0x11, 0x11, 0x0E,      // Code for char P
    0x00, 0x3E, 0x41, 0x41, 0x41, 0xBE,      // Code for char Q
    0x00, 0xFF, 0x11, 0x31, 0x51, 0x8E,      // Code for char R
    0x00, 0x46, 0x89, 0x89, 0x91, 0x62,      // Code for char S
    0x00, 0x01, 0x01, 0xFF, 0x01, 0x01,      // Code for char T
    0x00, 0x7F, 0x80, 0x80, 0x80, 0x7F,      // Code for char U
    0x00, 0x07, 0x38, 0xC0, 0x38, 0x07,      // Code for char V
    0x00, 0xFF, 0x38, 0x0F, 0x38, 0xFF,      // Code for char W
    0x00, 0xC3, 0x24, 0x18, 0x24, 0xC3,      // Code for char X
    0x00, 0x03, 0x0C, 0xF0, 0x0C, 0x03,      // Code for char Y
    0x00, 0xC1, 0xB1, 0x8D, 0x83, 0x81,      // Code for char Z
    0x00, 0x00, 0x00, 0xFF, 0x81, 0x00,      // Code for char [
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char BackSlash
    0x00, 0x00, 0x81, 0xFF, 0x00, 0x00,      // Code for char ]
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char ^
    0x00, 0x80, 0x80, 0x80, 0x80, 0x80,      // Code for char _
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char `
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char a
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char b
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char c
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char d
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char e
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char g
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char h
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char i
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char j
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char k
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char l
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char m
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char n
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char o
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char p
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char q
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char r
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char s
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char t
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char u
    0x00, 0x0C, 0x30, 0xC0, 0x30, 0x0C,      // Code for char v
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char w
    0x00, 0x84, 0x48, 0x30, 0x48, 0x84,      // Code for char x
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char y
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char z
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char {
    0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,      // Code for char |
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char }
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char ~
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00       // Code for char 
};

#endif /* OLED_GL_FONT_6X8_H_ */