/*
 * LCD_LOGO.h
 *
 *  Created on: Jul 25, 2012
 *      Author: danijacm
 */

#ifndef LCD_LOGO_H_
#define LCD_LOGO_H_

unsigned char logoPichincha[]={
0x47,0x49,0x46,0x38,0x37,0x61,0x40,0x01,
0xD8,0x00,0xE7,0xE1,0x00,0x01,0x01,0x00,
0x02,0x02,0x00,0x11,0x0F,0x07,0x11,0x0F,
0x08,0x12,0x10,0x07,0x12,0x10,0x08,0x12,
0x10,0x09,0x12,0x10,0x0A,0x13,0x10,0x08,
0x13,0x11,0x07,0x13,0x11,0x08,0x13,0x11,
0x09,0x14,0x11,0x07,0x13,0x11,0x0A,0x14,
0x11,0x0A,0x14,0x12,0x07,0x14,0x12,0x0A,
0x15,0x12,0x08,0x15,0x12,0x0A,0x15,0x13,
0x0B,0x17,0x14,0x08,0x16,0x14,0x0C,0x16,
0x14,0x0D,0x19,0x16,0x08,0x18,0x16,0x0F,
0x19,0x16,0x0F,0x1B,0x17,0x08,0x19,0x17,
0x10,0x1A,0x17,0x10,0x1A,0x18,0x11,0x1D,
0x19,0x09,0x1D,0x19,0x0A,0x1B,0x19,0x12,
0x1E,0x1A,0x09,0x1C,0x1A,0x13,0x1F,0x1B,
0x09,0x1E,0x1C,0x15,0x1F,0x1D,0x17,0x23,

0x1E,0x09,0x20,0x1E,0x14,0x21,0x1E,0x18,
0x21,0x1F,0x18,0x25,0x20,0x0A,0x22,0x20,
0x1A,0x23,0x21,0x1A,0x24,0x21,0x1C,0x24,
0x22,0x1B,0x29,0x23,0x0C,0x2A,0x24,0x09,
0x2A,0x24,0x0A,0x2A,0x24,0x0B,0x29,0x24,
0x0E,0x29,0x24,0x0F,0x26,0x24,0x1E,0x26,
0x24,0x1F,0x27,0x25,0x1F,0x2D,0x26,0x0B,
0x2D,0x26,0x0D,0x29,0x27,0x20,0x29,0x27,
0x21,0x29,0x27,0x22,0x2A,0x27,0x22,0x2A,
0x28,0x22,0x2A,0x28,0x23,0x2B,0x2A,0x23,
0x2D,0x2A,0x26,0x2C,0x2B,0x25,0x2D,0x2B,
0x26,0x30,0x2D,0x29,0x30,0x2E,0x29,0x31,
0x2E,0x29,0x30,0x2F,0x28,0x33,0x31,0x2D,
0x34,0x32,0x2C,0x34,0x32,0x2D,0x3B,0x33,
0x0F,0x3C,0x33,0x0D,0x3B,0x34,0x10,0x3B,
0x34,0x12,0x36,0x33,0x2F,0x36,0x34,0x2F,

0x3A,0x38,0x34,0x3B,0x39,0x34,0x3D,0x3A,
0x38,0x3D,0x3B,0x38,0x47,0x3C,0x11,0x3D,
0x3C,0x38,0x47,0x3D,0x12,0x47,0x3D,0x13,
0x3E,0x3C,0x38,0x47,0x3D,0x16,0x40,0x3D,
0x3B,0x40,0x3E,0x3B,0x4B,0x40,0x0E,0x42,
0x3F,0x3B,0x42,0x40,0x3D,0x47,0x41,0x30,
0x4E,0x43,0x11,0x4E,0x43,0x12,0x45,0x43,
0x3D,0x45,0x44,0x3D,0x46,0x44,0x41,0x52,
0x46,0x14,0x4F,0x4C,0x3B,0x5C,0x4E,0x17,
0x5C,0x4F,0x15,0x5C,0x4F,0x16,0x5F,0x51,
0x14,0x60,0x52,0x12,0x54,0x51,0x42,0x67,
0x58,0x19,0x67,0x58,0x1B,0x6A,0x59,0x15,
0x5B,0x5A,0x55,0x5D,0x5B,0x59,0x71,0x60,
0x17,0x71,0x60,0x18,0x71,0x60,0x19,0x71,
0x61,0x19,0x7A,0x68,0x17,0x6A,0x68,0x60,
0x7E,0x6B,0x1B,0x7E,0x6C,0x21,0x81,0x6D,

0x1A,0x81,0x6D,0x1B,0x81,0x6D,0x1C,0x6F,
0x6E,0x6C,0x88,0x73,0x1D,0x88,0x74,0x21,
0x91,0x7B,0x1D,0x91,0x7C,0x1F,0x91,0x7C,
0x20,0x91,0x7D,0x24,0x7D,0x7C,0x7A,0x9D,
0x86,0x20,0x9D,0x86,0x21,0x9D,0x86,0x24,
0x9D,0x87,0x26,0xA0,0x88,0x1F,0xA0,0x88,
0x22,0xA6,0x8D,0x22,0xA6,0x8D,0x23,0xA6,
0x8D,0x24,0xA6,0x8E,0x27,0x8E,0x8C,0x8B,
0xAE,0x95,0x22,0xAE,0x95,0x25,0xB1,0x97,
0x24,0xB1,0x97,0x25,0xB1,0x97,0x26,0x9B,
0x9A,0x99,0xB9,0x9E,0x25,0xB9,0x9E,0x27,
0xB9,0x9F,0x2A,0xBC,0xA0,0x25,0xC4,0xA7,
0x28,0xAA,0xA9,0xA8,0xB5,0xAD,0x8F,0xCE,
0xB0,0x29,0xB5,0xAD,0x91,0xCE,0xB0,0x2B,
0xB3,0xB2,0xB2,0xD8,0xB8,0x2B,0xCA,0xB8,
0x65,0xB9,0xB8,0xB8,0xE2,0xC0,0x2C,0xE2,

0xC1,0x2F,0xE4,0xC2,0x2B,0xE4,0xC2,0x2D,
0xC2,0xC1,0xC1,0xC4,0xC3,0xC3,0xEA,0xC8,
0x2D,0xEA,0xC8,0x2E,0xC7,0xC7,0xC5,0xF1,
0xCD,0x2E,0xF3,0xCF,0x2F,0xDD,0xD0,0x99,
0xCF,0xCF,0xCD,0xFC,0xD5,0x29,0xFC,0xD5,
0x2A,0xD2,0xD1,0xD1,0xFC,0xD6,0x2C,0xFB,
0xD6,0x30,0xE1,0xD5,0xA1,0xFC,0xD8,0x36,
0xD4,0xD4,0xD4,0xE4,0xD8,0xA8,0xD9,0xD9,
0xD9,0xDE,0xDB,0xCC,0xDD,0xDB,0xD2,0xDD,
0xDB,0xD8,0xDE,0xDE,0xDD,0xDE,0xDE,0xDE,
0xE1,0xDF,0xD8,0xDF,0xDF,0xDE,0xE3,0xE3,
0xE2,0xE3,0xE3,0xE3,0xE7,0xE5,0xDD,0xE7,
0xE6,0xE0,0xE7,0xE7,0xE6,0xEA,0xE7,0xDE,
0xE7,0xE7,0xE7,0xE8,0xE8,0xE8,0xEC,0xEA,
0xE0,0xEA,0xEA,0xEA,0xEB,0xEB,0xEA,0xEF,
0xED,0xE6,0xF0,0xEE,0xE6,0xEF,0xEE,0xEE,

0xF1,0xEF,0xE7,0xF1,0xEF,0xE8,0xEF,0xEF,
0xEF,0xF1,0xF0,0xEC,0xF5,0xF1,0xDF,0xF1,
0xF1,0xF1,0xF3,0xF3,0xF3,0xF4,0xF4,0xF3,
0xF4,0xF4,0xF4,0xF5,0xF5,0xF5,0xF7,0xF7,
0xF7,0xF8,0xF8,0xF8,0xF9,0xF9,0xF9,0xFB,
0xFB,0xFB,0xFC,0xFC,0xFC,0xFD,0xFD,0xFD,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x2C,0x00,0x00,
0x00,0x00,0x40,0x01,0xD8,0x00,0x00,0x08,
0xFE,0x00,0xC3,0x09,0x1C,0x48,0xB0,0xA0,
0xC1,0x83,0x08,0x13,0x2A,0x5C,0xC8,0xB0,
0xA1,0xC3,0x87,0x10,0x23,0x4A,0x9C,0x48,
0xB1,0xA2,0xC5,0x8B,0x18,0x33,0x6A,0xDC,
0xC8,0xB1,0xA3,0xC7,0x8F,0x20,0x43,0x8A,
0x1C,0x49,0xB2,0xA4,0xC9,0x93,0x28,0x53,
0xAA,0x5C,0xC9,0xB2,0xA5,0xCB,0x97,0x30,
0x63,0xCA,0x9C,0x49,0xB3,0xA6,0xCD,0x9B,
0x38,0x73,0xEA,0xDC,0xC9,0xB3,0xA7,0xCF,
0x9F,0x40,0x83,0x0A,0x1D,0x4A,0xB4,0xA8,
0xD1,0xA3,0x48,0x93,0x2A,0x5D,0xCA,0xB4,
0xA9,0xD3,0xA7,0x50,0xA3,0x4A,0x9D,0x4A,
0xB5,0xAA,0xD5,0xAB,0x58,0xB3,0x6A,0xDD,

0xCA,0x95,0xA8,0xB5,0x58,0x5D,0xC3,0x8A,
0x25,0x89,0x2D,0x56,0xAB,0x56,0x63,0xD3,
0xAA,0xCD,0xE8,0xAD,0xD9,0xB1,0x61,0xBF,
0x84,0xAD,0x9D,0x4B,0xB7,0xAE,0xDD,0xBB,
0x78,0xF3,0xEA,0xDD,0xCB,0xB7,0xAF,0xDF,
0xBF,0x80,0x03,0x0B,0x1E,0x4C,0xB8,0xB0,
0xE1,0xC3,0x88,0x13,0x2B,0x5E,0xCC,0xB8,
0xB1,0xE3,0xC7,0x90,0x23,0x4B,0x4E,0x28,
0xA0,0xB2,0xE5,0xCA,0x93,0x33,0x9B,0xAC,
0x5C,0xE4,0xB2,0xE7,0xCF,0x9E,0x6D,0x60,
0xD6,0x4C,0x7A,0xA2,0x80,0xCE,0xA0,0x53,
0xAB,0x06,0x9D,0x45,0x40,0xE9,0xD7,0x07,
0x05,0x44,0x59,0xFD,0xC7,0x96,0xED,0xDB,
0xB8,0x73,0xE3,0x56,0xA0,0x1A,0x85,0x6B,
0xD8,0x9A,0x55,0x1F,0xD2,0x4D,0xBC,0xB8,
0x71,0x5B,0xAA,0x81,0x3F,0x4E,0xFD,0xEA,

0xB8,0xF3,0xE7,0xBA,0x53,0x2B,0x4F,0xBC,
0xE5,0x33,0xF4,0xEB,0xD8,0x71,0x6F,0xF2,
0x5C,0xE1,0xF7,0x74,0xC0,0xA0,0xB3,0xFE,
0x8B,0x1F,0x6F,0x0B,0xD3,0xE7,0xEF,0x7D,
0xAD,0x93,0x5F,0x3F,0xFE,0x3C,0xFA,0xBB,
0xEA,0xD9,0xCB,0x17,0xEF,0xC6,0xF3,0xFB,
0xB9,0xF1,0xE7,0xEB,0x17,0x6F,0xFF,0xBE,
0x58,0xCF,0xFB,0x05,0xB8,0x5E,0x7F,0xFE,
0x65,0x05,0xA0,0x80,0x08,0x8E,0xE7,0x88,
0x65,0x29,0x14,0x78,0xD5,0x81,0x09,0x46,
0xC8,0xDF,0x65,0x0E,0x4E,0x65,0x85,0x65,
0xC3,0x21,0xC8,0xC7,0x6A,0x1C,0x46,0x68,
0x19,0x06,0x15,0x3E,0x05,0xE1,0x7C,0x75,
0x70,0x68,0x62,0x6A,0x01,0x56,0x42,0x61,
0x88,0x4A,0x5D,0x36,0x89,0x7E,0xAB,0x09,
0x01,0x8D,0x43,0xDD,0x48,0xD1,0xE1,0x7C,

0x96,0x51,0xC1,0x22,0x52,0x97,0xE1,0x08,
0x1A,0x2D,0x1A,0x75,0xA3,0x9A,0x7C,0x2B,
0xEE,0x38,0x54,0x8F,0x03,0x7E,0xC6,0xC9,
0x48,0xE1,0xB1,0x57,0xA4,0x91,0x3D,0x21,
0x49,0xDE,0x67,0xE0,0xA4,0xD4,0x64,0x7B,
0x96,0x41,0x19,0xA5,0x65,0x49,0x5A,0x46,
0xCA,0x4B,0xF9,0x61,0xF7,0xA4,0x96,0x36,
0x49,0x99,0x1D,0x81,0x31,0x85,0x09,0x1D,
0x22,0x95,0x11,0x41,0xE6,0x4D,0x2E,0x70,
0x39,0x61,0x96,0x37,0x89,0x50,0x59,0x01,
0x02,0x28,0x82,0xA5,0x77,0x6F,0xC6,0x84,
0x84,0x9C,0x62,0x8E,0x79,0xD3,0x88,0xD7,
0xD1,0xD9,0xE7,0x4B,0x5E,0x00,0x5A,0xA8,
0xA1,0x5B,0x2A,0x0A,0x1D,0xA3,0x87,0xAA,
0xC4,0x45,0x65,0x1E,0x9C,0x69,0x59,0x21,
0x47,0x0A,0x30,0x80,0x00,0x73,0x46,0xBA,

0x92,0x99,0xCF,0x09,0x0A,0x14,0xA1,0xCE,
0x55,0xD6,0xA0,0xA7,0x27,0x81,0x7A,0x5C,
0x1F,0x96,0xA9,0x92,0x94,0xAA,0xFE,0xC7,
0x55,0x16,0x05,0xAA,0x25,0xC1,0x5A,0x9C,
0xA8,0x45,0xD9,0x7A,0xEB,0x68,0xB4,0x86,
0xE4,0x68,0xAC,0x90,0x26,0x55,0xC6,0xAF,
0xC0,0xF2,0xD9,0x2B,0x47,0xC4,0xEE,0x2A,
0x40,0x32,0x51,0xE9,0x9A,0xDB,0x07,0xC1,
0x1E,0x7B,0x51,0xB2,0xC4,0x45,0xDB,0x94,
0xB3,0xB8,0x59,0x2B,0xAD,0x69,0x95,0x05,
0x6A,0x6C,0xB3,0x9A,0x72,0xFA,0x28,0xAF,
0xDB,0x56,0x64,0xD9,0x0B,0x8B,0x7E,0x9B,
0xD1,0x89,0xEA,0x92,0x84,0xED,0x6D,0xDA,
0x96,0xDB,0x10,0xB5,0xB9,0xC5,0x1B,0x11,
0xBB,0xC9,0xA5,0x4A,0x6F,0xB6,0xE4,0xCA,
0xFB,0xD0,0x02,0xDD,0x8E,0xDB,0xEE,0xBD,

0x27,0x12,0x20,0x83,0x06,0x0F,0x70,0xB8,
0x99,0x00,0x78,0x0A,0xEC,0x2F,0xC1,0xE2,
0x86,0xDA,0x2F,0xB7,0x9E,0x31,0x21,0xCB,
0xC5,0x18,0x67,0xAC,0x31,0xC6,0x09,0xB8,
0xC7,0xE4,0xBE,0xB6,0xD9,0xFB,0x30,0x41,
0x20,0x23,0x37,0x31,0xC4,0x96,0x6D,0xAC,
0xF2,0xCA,0x19,0xDF,0x81,0x26,0x48,0xE1,
0xA6,0x3B,0x32,0x43,0x20,0x04,0x5C,0xEA,
0xC9,0x10,0x5D,0x16,0x02,0xCB,0x3C,0xF7,
0xFC,0xB2,0x47,0x25,0x8B,0xFC,0x70,0xD0,
0x38,0x3F,0x74,0x59,0xCF,0x48,0xFB,0x2C,
0xB4,0x45,0x44,0x0F,0x3C,0x33,0xD1,0x65,
0x30,0x5D,0x99,0x0A,0x2C,0xEB,0x32,0x82,
0x09,0x4A,0x0B,0xC0,0x33,0xAE,0x1B,0x19,
0x60,0x33,0xB0,0x33,0x23,0xD4,0xB4,0xD4,
0x02,0x74,0xA1,0xF2,0x67,0x59,0x57,0xB6,

0x75,0xD1,0x19,0xFD,0x29,0xC0,0x1C,0x12,
0x3F,0x11,0xB6,0x41,0x20,0x2F,0x4D,0x99,
0xDA,0x1A,0xA7,0x96,0x36,0xFE,0xDE,0x2B,
0xDB,0x4D,0xB1,0xC3,0x73,0x0B,0x04,0x32,
0x2A,0x6C,0xCF,0xCB,0x37,0xC6,0x45,0x1E,
0x7E,0xB6,0x6B,0x29,0xAF,0xED,0x34,0x46,
0x75,0x0B,0x70,0x6A,0xE0,0x95,0xA9,0x21,
0xF1,0x17,0xD3,0x2A,0x2E,0x0B,0xA3,0x9A,
0xE7,0xED,0x5D,0xE3,0x7D,0x17,0x6E,0x91,
0x37,0x91,0x3F,0x7E,0x6C,0xE9,0x90,0x0B,
0x70,0x81,0xE7,0xC6,0x76,0x9E,0x31,0xB9,
0xA0,0x2F,0x6E,0xBA,0xB9,0x5F,0x17,0xE7,
0x84,0x00,0x0B,0x50,0x2E,0x40,0x1E,0x12,
0xAF,0xAB,0xB5,0xE7,0x74,0xFF,0xCE,0x72,
0xBF,0xB1,0x6F,0xEC,0x37,0xC4,0x12,0xCF,
0xEE,0x69,0xE4,0x25,0xF8,0x6E,0xFC,0xB7,

0xAE,0x23,0xDE,0x7A,0xF4,0x17,0x1F,0xFF,
0x10,0x29,0xB5,0x13,0xC7,0x5B,0x0B,0x61,
0x57,0x56,0x85,0x73,0x83,0x88,0xEE,0x90,
0xE6,0x13,0x53,0xBF,0x39,0xF4,0xE6,0x8B,
0x4F,0x51,0xE4,0x4F,0x67,0x1F,0x9D,0x00,
0x5F,0xFA,0x1E,0x03,0xF0,0x05,0xA5,0xAF,
0x6E,0x65,0xA3,0x38,0xEE,0xAB,0x00,0x81,
0xDC,0xAC,0x7C,0x9F,0xFB,0xB2,0xDE,0xDD,
0x84,0x77,0x31,0x38,0x70,0x8E,0x80,0x8B,
0x8B,0x8D,0xFD,0xFE,0x87,0xBC,0x9B,0x3D,
0x6C,0x0C,0xEE,0xAB,0x97,0x00,0xB6,0xB1,
0x91,0xE2,0x9D,0x8F,0x4E,0xF6,0x13,0x1B,
0x02,0x13,0x08,0x92,0x02,0x1C,0x60,0x0F,
0xFE,0xF3,0xD7,0x6C,0x22,0x56,0x9C,0x26,
0xA8,0x6F,0x7C,0x16,0x5C,0x51,0x06,0x15,
0x88,0x34,0x01,0xCE,0x6B,0x53,0xA1,0xAA,

0x80,0xBF,0x2A,0xB3,0x8A,0x9B,0x01,0xC3,
0x70,0xF8,0xB2,0x60,0xF5,0x8E,0xE6,0xB8,
0x1C,0xFA,0xD0,0x63,0xFE,0x46,0x23,0xE1,
0xAE,0xE4,0xC5,0x3E,0x1C,0xFE,0xB0,0x0D,
0x1A,0x0B,0x86,0x67,0xF6,0xF6,0xC3,0x26,
0xDA,0x2D,0x80,0x27,0x34,0x12,0x14,0x95,
0x80,0xC2,0x33,0xF4,0x22,0x69,0x3B,0xD4,
0xDB,0xF9,0x94,0x86,0xC5,0x2E,0xF6,0xF0,
0x7F,0x30,0x74,0xE0,0xB6,0x22,0xC8,0xAF,
0x20,0x2E,0xB1,0x8B,0xC6,0x08,0x07,0x1E,
0xF8,0x66,0x3F,0x2F,0xBA,0x71,0x65,0xCB,
0xE0,0x1A,0xC9,0xC8,0x18,0xB2,0x28,0x86,
0x08,0x8A,0xFF,0x4B,0x4D,0x34,0xDE,0xF8,
0xBA,0x0D,0x7A,0x8E,0x8F,0x80,0xEC,0xA3,
0xE8,0x58,0xD0,0xB0,0x62,0x49,0x6B,0x0A,
0x74,0x34,0x19,0x37,0xD6,0xA7,0xC3,0x2E,

0xB6,0x31,0x90,0x90,0x94,0xC0,0x9D,0x18,
0x92,0x48,0x93,0x49,0xAB,0x32,0x9E,0x10,
0x23,0xED,0xFC,0x88,0xC5,0x47,0x42,0x32,
0x90,0x22,0x83,0xE2,0x25,0x85,0xF8,0x3E,
0x06,0x6A,0x90,0x93,0x2D,0x44,0xE5,0x0E,
0x3F,0xF9,0x49,0x7B,0x59,0x22,0x91,0x2E,
0x7C,0x8F,0x28,0x53,0xA7,0xCA,0x8B,0x01,
0xE0,0x96,0xB8,0xBC,0xE5,0x02,0x9D,0x08,
0x1A,0x3E,0x8A,0x4E,0x53,0xCE,0x79,0x84,
0x1D,0x0B,0xD4,0x1D,0x52,0x4A,0x50,0x23,
0xE6,0x93,0x05,0x00,0x58,0xF8,0x45,0x5E,
0xA2,0xCD,0x8B,0xF6,0x9A,0x25,0xAA,0x60,
0x39,0x4C,0x0D,0x22,0x6D,0x99,0xC1,0xCB,
0xDA,0x29,0x93,0xD6,0x4B,0x47,0x3E,0xAE,
0x06,0xD4,0xEC,0x55,0x38,0x91,0x59,0x4B,
0x65,0x32,0x73,0x78,0x4E,0x4B,0xE6,0xF9,

0x04,0xA7,0x4E,0xE9,0x51,0xD2,0x98,0xFC,
0x32,0xE5,0x7D,0xA4,0x99,0xBA,0x6B,0x9E,
0x33,0x74,0x0A,0xFE,0x51,0x27,0xAF,0x1A,
0xC9,0xC1,0x7C,0x16,0x20,0x0C,0xC7,0xD1,
0x42,0x35,0xD1,0x43,0xCF,0xCC,0xD9,0x33,
0x9B,0x6B,0xCB,0x67,0x39,0xD7,0x39,0x90,
0x76,0x56,0x8F,0x21,0xD9,0x08,0xA3,0x71,
0x04,0xA0,0xA3,0xE5,0xD1,0x51,0x12,0x03,
0xBD,0xE7,0xCA,0xB0,0x59,0xBF,0x5A,0xB2,
0x4D,0x9F,0x7C,0x72,0xE8,0xE6,0x0C,0xA7,
0xC9,0x43,0x55,0xA6,0x39,0x13,0x95,0xA7,
0x42,0x91,0x16,0x00,0x8D,0x3E,0x6F,0xA5,
0xA9,0xEC,0xA8,0x23,0x49,0x0A,0x36,0x8B,
0x96,0xB4,0x9E,0x3D,0x6B,0x29,0x42,0xD1,
0x09,0x53,0x9E,0x31,0x80,0x78,0x0B,0x7D,
0xE8,0x3B,0x43,0x18,0xA9,0x71,0x92,0x93,

0xA5,0x2E,0xA5,0x9F,0x35,0xB5,0x39,0xC7,
0x99,0x52,0xB2,0x00,0xA7,0x30,0xA4,0x49,
0x8D,0xEA,0xBC,0x9C,0x26,0xB5,0x8F,0xF9,
0xE4,0x40,0x2A,0xA7,0xE7,0xD4,0x85,0xD0,
0x8B,0x06,0x19,0x05,0x4E,0x41,0x0D,0x2A,
0x8B,0x5A,0xCC,0x62,0x65,0x3A,0x95,0x69,
0x42,0x29,0xE3,0x8B,0x98,0x12,0x84,0x18,
0xED,0x54,0x1F,0x1D,0x4D,0x11,0x56,0xD8,
0x8C,0x55,0x6A,0x17,0xC3,0x45,0x2D,0xD0,
0x7A,0x55,0x77,0x52,0x86,0x9B,0xD3,0x0B,
0xAA,0xFA,0x0A,0x39,0x51,0x17,0x14,0x95,
0x8E,0x95,0x71,0x00,0xB2,0xCA,0x99,0xD6,
0xA6,0xAE,0x55,0x6C,0x80,0x55,0x6B,0x64,
0x1B,0x42,0xD8,0x5B,0x91,0x20,0x52,0x3D,
0x40,0xAC,0x6C,0x16,0x8B,0xD4,0x9D,0xE2,
0x73,0xA9,0xFA,0x1B,0xC8,0x2E,0xE2,0x2A,

0x57,0x78,0xC2,0x4B,0x86,0x53,0x55,0x40,
0x4D,0x2B,0xC8,0xD8,0xBE,0xEE,0xF0,0x6E,
0x5B,0x75,0xFE,0x6C,0x6C,0x1D,0x22,0xD1,
0x21,0x4E,0xD5,0x0C,0xAB,0x3D,0xAA,0x55,
0x3D,0xDB,0x4F,0xDE,0xF6,0x93,0x87,0x59,
0x03,0x42,0x10,0x6F,0xAA,0xA5,0xCA,0x08,
0x22,0xB7,0x55,0xE5,0x59,0x00,0x94,0xE1,
0x5B,0xD6,0xAD,0xA6,0x67,0xD5,0xD0,0x62,
0xDA,0x72,0x66,0xDA,0x3A,0x1E,0x16,0xA5,
0xB6,0x65,0xED,0x56,0xBB,0x89,0x4E,0x0E,
0x31,0x71,0xA1,0x3F,0xF3,0x6A,0x75,0x2D,
0x39,0xD5,0x52,0x20,0x17,0xA7,0x6F,0xDC,
0xA5,0x6A,0xB2,0xE6,0x4D,0xCB,0xE8,0x40,
0x22,0x54,0x25,0x53,0x65,0x2E,0x71,0x5E,
0xB2,0xA6,0xD7,0xA3,0x9C,0xE4,0xE7,0x05,
0xDB,0x5B,0x19,0x08,0x70,0x8B,0xB8,0x50,

0xA2,0x61,0x7D,0xF1,0xEA,0x4B,0xFC,0x86,
0x4E,0x95,0x22,0xFD,0x8C,0x7F,0x09,0x06,
0x60,0x29,0x0A,0xE0,0x45,0x29,0xED,0x88,
0x48,0xFB,0xC8,0xC5,0xC5,0x39,0xAE,0xC0,
0xC7,0x8B,0x6F,0x71,0x05,0x60,0x87,0x01,
0xD3,0x0E,0x90,0x9E,0xCC,0x18,0x2C,0xA8,
0x57,0x99,0x4E,0x80,0xB2,0x32,0x13,0xA0,
0x6E,0x83,0x77,0x44,0x29,0x0F,0x33,0x72,
0xBB,0xCF,0xEC,0xEE,0x7A,0x0F,0xEC,0x19,
0x5E,0xB8,0xD1,0x6F,0x1A,0x86,0x52,0x0A,
0x34,0x2B,0xE1,0x72,0x3E,0xB7,0x99,0x31,
0x7E,0xDE,0x8C,0x3B,0x79,0xC2,0xDA,0x56,
0x4B,0xA5,0xD3,0x41,0x0D,0xB0,0x16,0xAC,
0xDB,0xC7,0x36,0x14,0xBF,0x7D,0x8D,0x96,
0x65,0x9E,0xE1,0xCD,0xE1,0xC6,0x8A,0x07,
0x87,0x1D,0x6F,0x5D,0x25,0x3B,0x3C,0xD7,

0x32,0x74,0x8E,0xF9,0x2D,0x9A,0x7E,0x59,
0xE7,0x10,0xAF,0x95,0xEA,0x07,0x59,0x5E,
0x31,0xFE,0xB7,0x5A,0xE8,0xE5,0xF2,0x31,
0x4C,0x65,0xB7,0x28,0xDC,0x98,0xB1,0x4A,
0xD3,0x08,0x4F,0x55,0xCB,0x48,0x06,0x6D,
0x97,0x9B,0x4B,0xE7,0x27,0x87,0xD9,0x70,
0x55,0x7E,0x6A,0x75,0x67,0xB0,0xE5,0xD7,
0xE4,0xF8,0xC3,0x3E,0x6B,0xF3,0xF4,0x28,
0x20,0x3B,0xC3,0xED,0x11,0xB0,0xA6,0x13,
0xC0,0x01,0xD6,0x20,0xD5,0xA9,0xAA,0x19,
0xBE,0x3E,0x56,0x34,0x98,0x1B,0x8D,0x43,
0x22,0x47,0xDA,0x00,0x44,0xB5,0x34,0xB0,
0xA2,0xD6,0xE4,0x3D,0x73,0xF9,0xB7,0xE4,
0xB3,0x5B,0x2E,0x48,0x1B,0x69,0x3C,0x4F,
0x93,0x8E,0x39,0x28,0x34,0x3B,0xD9,0xCC,
0x67,0xBF,0xA6,0x30,0xC3,0x41,0xFD,0x32,

0x42,0xA4,0x60,0x66,0x17,0xB3,0xF8,0xD0,
0x6B,0x4E,0x74,0xAD,0xB3,0x68,0x19,0x18,
0x38,0x37,0x22,0xD3,0x68,0x27,0x32,0xE4,
0xDC,0xEB,0x89,0x56,0x34,0xCD,0xBE,0xA6,
0x2E,0xAD,0x4F,0x2D,0x64,0xC5,0xC9,0x31,
0x21,0xA1,0x68,0x27,0x18,0xE4,0x2C,0x00,
0x1C,0x04,0x54,0xD6,0x86,0xC6,0xB3,0x36,
0x4A,0xDD,0x37,0x2F,0xEF,0x57,0x7A,0x95,
0x89,0x03,0x23,0x05,0x5B,0x38,0x23,0x97,
0x92,0x56,0xD4,0xCC,0x33,0xB5,0x8D,0x67,
0x6E,0x41,0xC6,0x32,0x1C,0x0E,0x8D,0x97,
0x1C,0x9A,0x9D,0x5D,0x8B,0xBA,0x3A,0xB9,
0xA6,0x96,0xED,0xCA,0xD8,0x20,0x9D,0xD1,
0xE5,0xFB,0xA3,0xC0,0xDE,0xF0,0xBF,0xD1,
0x1B,0xF0,0x27,0x33,0x91,0x9C,0xB9,0x2E,
0x1C,0x9E,0x22,0x71,0x69,0x23,0x41,0x10,

0xCF,0x1B,0x00,0x78,0xB9,0x87,0xAD,0x6B,
0x8C,0xA4,0x62,0xCE,0x7D,0x3E,0x08,0x38,
0xFE,0xDC,0x2D,0x41,0x79,0x13,0x74,0xBC,
0xE4,0xB5,0x2F,0x4F,0x07,0xF2,0x8D,0x1C,
0xB2,0x36,0xC1,0x08,0x5F,0x38,0xAD,0x32,
0x90,0xF0,0x20,0xC2,0xD8,0x99,0x38,0x9F,
0xF0,0xF9,0x0A,0xA0,0x90,0x01,0x0C,0x40,
0x4F,0xD1,0x0E,0x70,0xBC,0x19,0x4E,0x63,
0x0E,0x9D,0x20,0xE7,0x43,0xBE,0x31,0xDB,
0xFC,0x90,0x48,0xDE,0x98,0x5C,0x96,0x28,
0xDF,0xB2,0x00,0x10,0xC0,0xCA,0xAA,0x5B,
0x5D,0x63,0xCE,0x98,0x24,0x65,0x48,0x1E,
0xCF,0x31,0x52,0x13,0x53,0x88,0xBE,0xBA,
0xD8,0x59,0x69,0xAF,0x02,0x0C,0xA0,0x77,
0x5E,0x1F,0xEF,0x22,0x32,0x0A,0xF2,0xB1,
0xBB,0xFD,0xC0,0x5E,0xE5,0xF7,0x91,0x9F,

0x3E,0xCF,0xA8,0x0B,0xC0,0x1B,0x14,0x7B,
0xBB,0xDE,0xBD,0x28,0x8D,0x50,0x0E,0xBD,
0x5C,0x00,0xC3,0xF3,0xEC,0xA4,0xBB,0xF7,
0xC2,0xCB,0x98,0x0C,0x5E,0xE5,0x7A,0x19,
0x89,0x58,0x49,0xF1,0x71,0xD7,0xF0,0x90,
0x3F,0xF6,0x3B,0x05,0x3F,0xB4,0xBF,0x2B,
0x54,0x00,0xD4,0x28,0x46,0xE4,0x37,0x8F,
0xB1,0x4F,0x84,0x37,0x78,0x76,0x1F,0x59,
0xE4,0xAE,0x21,0x5E,0xA4,0x9B,0xFE,0xF4,
0xA0,0x39,0x02,0x0A,0x29,0x2F,0x7A,0x01,
0x30,0x22,0xD4,0xA7,0x44,0xBD,0xEC,0x91,
0x9E,0x04,0xA3,0x29,0x1E,0x5E,0xE0,0xF6,
0x8F,0x92,0x6F,0x56,0x03,0x2B,0x41,0xD9,
0x26,0x03,0x28,0x00,0x08,0x8F,0x03,0x08,
0x01,0x5C,0xB6,0x7B,0x28,0x4F,0xF9,0xC2,
0x94,0x56,0xA6,0xC6,0xE7,0xBE,0x40,0x3C,

0x70,0x3E,0xDD,0x6D,0xEE,0xE4,0x34,0x55,
0x32,0x13,0xCF,0x77,0xD0,0x49,0x25,0xFE,
0xE6,0xAA,0xE5,0x57,0x1F,0x26,0xA8,0x0B,
0xDC,0x0D,0xA4,0xAF,0x2F,0xE6,0xCF,0x44,
0x00,0x72,0x27,0x4E,0x22,0xB2,0x5F,0xA1,
0xCA,0xA4,0x21,0x79,0xDE,0x5F,0xB9,0xF5,
0x93,0x7F,0xEF,0x37,0x8D,0xCD,0x5D,0xBF,
0x87,0x89,0x37,0x82,0x6F,0xF7,0x21,0x04,
0xAE,0xA3,0xF4,0x27,0x00,0xCC,0x80,0x7F,
0xE6,0x07,0x7E,0xB7,0x87,0x7B,0xD3,0xA7,
0x70,0x80,0xB3,0x3F,0x3E,0x96,0x80,0xE6,
0x72,0x80,0x75,0xE4,0x80,0x0A,0x08,0x7F,
0xFB,0x03,0x69,0x2E,0x51,0x32,0x84,0xC0,
0x7E,0x42,0x17,0x80,0x12,0xE8,0x67,0x9E,
0xC6,0x12,0x92,0x26,0x00,0xC3,0x07,0x7B,
0xFF,0x07,0x66,0x32,0xB3,0x58,0x0E,0x15,

0x01,0x96,0x71,0x7C,0xFA,0x52,0x59,0x29,
0x95,0x05,0x25,0x38,0x40,0x27,0xB8,0x49,
0x67,0xC4,0x5F,0x05,0xE7,0x11,0x9D,0xE1,
0x82,0xC5,0xA1,0x07,0x1A,0x68,0x7F,0x95,
0x14,0x81,0x64,0x33,0x7B,0xD7,0x46,0x11,
0xA8,0x91,0x7C,0x26,0xD3,0x81,0x77,0xE6,
0x2D,0x34,0x28,0x84,0xD9,0x07,0x01,0xE8,
0x67,0x84,0xF5,0x57,0x2E,0x35,0x63,0x84,
0x47,0x68,0x15,0x0C,0xF3,0x83,0x55,0x18,
0x83,0x38,0xA4,0x84,0x53,0x51,0x19,0xE9,
0xA7,0x2C,0x5A,0x68,0x46,0x5C,0x08,0x15,
0x4F,0x48,0x85,0x51,0xD8,0x7A,0x66,0x58,
0x19,0x7E,0x20,0x22,0x25,0x03,0x84,0x61,
0x48,0x5D,0x69,0xD8,0x83,0x2B,0xB1,0x03,
0x6D,0x58,0x47,0x72,0xF3,0x86,0x10,0x01,
0x05,0x95,0x41,0x09,0xDE,0x82,0x84,0x9F,

0x52,0x87,0x26,0x63,0x03,0x78,0x28,0x11,
0x53,0x28,0x0A,0x96,0x22,0x00,0xFE,0x16,
0x60,0x14,0xC0,0xF0,0x2E,0x5D,0x37,0x88,
0x98,0x46,0x85,0x11,0xE8,0x87,0xF8,0x57,
0x19,0xAE,0x70,0x88,0x8E,0xB8,0x6E,0x86,
0x90,0x1D,0x4B,0x30,0x84,0x34,0xB1,0x02,
0x8C,0xD8,0x88,0x97,0x48,0x31,0xB5,0x71,
0x88,0x02,0xB0,0x03,0x3C,0xB1,0x88,0x57,
0x58,0x89,0x96,0x18,0x8A,0x34,0xB8,0x27,
0x02,0xC0,0x02,0x38,0xA1,0x0D,0xA4,0x42,
0x81,0xAC,0x58,0x11,0x3B,0x86,0x85,0xB8,
0x21,0x06,0x97,0xD1,0x3C,0x33,0xC1,0x09,
0xB3,0x48,0x8B,0xB5,0x58,0x11,0x45,0x38,
0x25,0x9F,0x87,0x12,0x6A,0x02,0x1D,0x6F,
0x50,0x19,0x3E,0x10,0x8C,0xBE,0x83,0x8B,
0xB9,0xB1,0x21,0x97,0xD1,0x7B,0x29,0x01,

0x09,0x57,0x08,0x88,0xF1,0x24,0x89,0x83,
0x78,0x19,0x86,0xD8,0x25,0x96,0xD1,0x00,
0x24,0x21,0x07,0x57,0xE2,0x8A,0xCC,0xD8,
0x63,0xCE,0x58,0x4A,0x9E,0x01,0x76,0x1A,
0x61,0x0D,0x28,0x42,0x24,0x67,0x38,0x8E,
0x05,0xE1,0x36,0x90,0xA8,0x2C,0xA0,0xB1,
0x03,0xDD,0x10,0x11,0xC3,0x32,0x24,0x3E,
0x22,0x00,0x19,0xE0,0x8E,0x30,0xF3,0x89,
0xC9,0x33,0x7B,0x01,0xC2,0x89,0xFC,0xD8,
0x8A,0x08,0xA2,0x09,0x39,0x84,0x27,0xBF,
0xE8,0x24,0x02,0x39,0x90,0x4B,0x28,0x21,
0xB9,0xA1,0x09,0x38,0x50,0x19,0x74,0xD0,
0x3F,0x0E,0x79,0x1B,0xAC,0xB0,0x90,0x0C,
0x99,0x39,0xD6,0x58,0x91,0xFB,0x51,0x8C,
0x19,0x09,0x34,0x97,0xD1,0x08,0x1C,0x39,
0x92,0x91,0x88,0x8D,0x1F,0x19,0x7B,0x02,

0x60,0x5E,0x24,0xE9,0x21,0x18,0x79,0x92,
0x1D,0x11,0x27,0x96,0x71,0xA0,0x05,0x2B,
0x19,0x90,0x96,0x61,0x04,0x2E,0x09,0x13,
0x1D,0x90,0x90,0x33,0xE9,0x1C,0xA0,0x70,
0x19,0x7F,0x72,0x93,0x32,0xF1,0x19,0x22,
0xB9,0x93,0x02,0xD3,0x8E,0x40,0x89,0x82,
0xFE,0x58,0x91,0xA0,0x71,0x94,0x3A,0x01,
0x1A,0x68,0x40,0x94,0x47,0x98,0x23,0x26,
0xC9,0x94,0xC8,0x34,0x29,0x9F,0x81,0x2E,
0x12,0xF2,0x0A,0xA0,0xD1,0x02,0x53,0x49,
0x95,0x20,0xB9,0x1A,0xD8,0x35,0x1E,0x58,
0xA0,0x30,0x5E,0x79,0x14,0x4C,0x78,0x19,
0x9D,0x51,0x96,0x64,0x28,0x00,0x41,0xE0,
0x4C,0x34,0x27,0x87,0x6A,0x89,0x13,0xEC,
0x32,0x06,0x71,0x59,0x97,0x76,0x79,0x97,
0x78,0x99,0x97,0x7A,0xB9,0x97,0x7C,0xD9,

0x97,0x7E,0xF9,0x97,0x80,0x19,0x98,0x82,
0x39,0x98,0x84,0x59,0x98,0x86,0x79,0x98,
0x88,0x99,0x98,0x8A,0xB9,0x98,0x8C,0xD9,
0x98,0x8E,0xF9,0x98,0x90,0x19,0x99,0x92,
0x39,0x99,0x94,0x59,0x99,0x96,0x79,0x99,
0x98,0x99,0x99,0x05,0x12,0x10,0x00,0x3B};


#endif /* LCD_LOGO_H_ */
