/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, R�tihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:         widgets.h (created by uTaskerFileCreate V1.9)

   Project:      uTasker Demonstration project
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2012
   *********************************************************************

This file is not linked directly in the project but is included by the GLCD task file

*/ 

static const unsigned char flame[] = {
0x00,0x00,0x4A,0x00,0x79,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x03,0x80,0x00,0x00,0x00,0x3F,
0x00,0x00,0x00,0x00,0x01,0xC0,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x70,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x3F,
0x00,0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x0F,0x80,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x0F,0xC0,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x07,0xC0,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,0x00,0x3F,
0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x01,0xFC,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x3F,
0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x80,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x80,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x80,0x00,0x3F,
0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xC0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xC0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xC0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE0,0x00,0x3F,
0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xF0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xF0,0x00,0x3F,
0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xF0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xF0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xF0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xF0,0x00,0x3F,0x00,0x00,0x00,0x00,0x04,0x00,0x1F,0xF0,0x00,0x3F,0x00,0x00,0x00,0x00,0x08,0x00,0x1F,0xF0,0x00,0x3F,
0x00,0x00,0x00,0x00,0x18,0x00,0x1F,0xF0,0x00,0x3F,0x00,0x00,0x00,0x00,0x30,0x00,0x3F,0xF0,0x00,0x3F,0x00,0x00,0x00,0x00,0xE0,0x00,0x3F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x01,0xC0,0x00,0x3F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x03,0x80,0x00,0x3F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x07,0x00,0x00,0x7F,0xE0,0x00,0x3F,
0x00,0x00,0x00,0x0E,0x00,0x00,0x7F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x1E,0x00,0x00,0x7F,0xE0,0x00,0x3F,0x00,0x00,0x00,0x3C,0x00,0x00,0xFF,0xE0,0x00,0x3F,0x00,0x00,0x00,0x78,0x00,0x00,0xFF,0xE0,0x00,0x3F,0x00,0x00,0x00,0xF8,0x00,0x01,0xFF,0xC0,0x00,0x3F,0x00,0x00,0x01,0xF0,0x00,0x03,0xFF,0xC0,0x00,0x3F,
0x00,0x00,0x03,0xF0,0x00,0x03,0xFF,0xC0,0x00,0x3F,0x00,0x00,0x07,0xE0,0x00,0x07,0xFF,0xC0,0x00,0x3F,0x00,0x00,0x07,0xE0,0x00,0x0F,0xFF,0xC0,0x00,0x3F,0x00,0x00,0x0F,0xC0,0x00,0x0F,0xFF,0x80,0x00,0x3F,0x00,0x00,0x1F,0xC0,0x00,0x1F,0xFF,0x80,0x00,0x3F,0x00,0x00,0x1F,0xC0,0x00,0x7F,0xFF,0x80,0x00,0x3F,
0x00,0x00,0x3F,0x80,0x00,0xFF,0xCF,0x80,0x40,0x3F,0x00,0x00,0x7F,0x80,0x01,0xFF,0x1F,0x80,0x40,0x3F,0x00,0x00,0x7F,0x80,0x03,0xFC,0x3F,0x80,0x60,0x3F,0x00,0x00,0xFF,0x80,0x0F,0xF0,0x7F,0x00,0x60,0x3F,0x00,0x00,0xFF,0x80,0x3F,0xE1,0xFF,0x00,0x70,0x3F,0x00,0x01,0xFF,0x80,0xFF,0xC3,0xFF,0x00,0x70,0x3F,
0x00,0x01,0xFF,0xC7,0xFF,0x03,0xFF,0x00,0x30,0x3F,0x00,0x03,0xFF,0xFF,0xFE,0x07,0xFF,0x00,0x38,0x3F,0x00,0x03,0xFF,0xFF,0xFC,0x0F,0xFE,0x00,0x38,0x3F,0x00,0x07,0xFF,0xFF,0xFC,0x1F,0xFE,0x00,0x3C,0x3F,0x00,0x07,0xFF,0xFF,0xF8,0x1F,0xFE,0x00,0x7C,0x3F,0x00,0x07,0xFF,0xFF,0xF0,0x3F,0xFE,0x00,0x7C,0x3F,
0x00,0x07,0xFF,0xFF,0xF0,0x7F,0xFE,0x00,0x7E,0x3F,0x00,0x07,0xFF,0xFF,0xE0,0x7F,0xFE,0x00,0x7E,0x3F,0x00,0x0F,0xFF,0xFF,0xE0,0x7F,0xFC,0x00,0x7E,0x3F,0x00,0x0F,0xFF,0xFF,0xC0,0xFF,0xFC,0x00,0x7E,0x3F,0x00,0x0F,0xFF,0xFF,0xC0,0xFF,0xFC,0x00,0xFE,0x3F,0x00,0x8F,0xFF,0xFF,0xC1,0xFF,0xFE,0x00,0xFF,0x3F,
0x01,0x8F,0xFF,0xFF,0x81,0xFF,0xFE,0x01,0xFF,0x3F,0x03,0x0F,0xFF,0xFF,0x81,0xFF,0xFE,0x03,0xFF,0x3F,0x07,0x0F,0xFF,0xFF,0x81,0xFF,0xFF,0x07,0xFF,0x3F,0x0F,0x0F,0xFF,0xFF,0x01,0xFF,0xFF,0x9F,0xFF,0x3F,0x0E,0x0F,0xFF,0xFF,0x01,0xFF,0xFF,0xFF,0xFF,0x3F,0x1E,0x0F,0xFF,0xFF,0x03,0xFF,0xFF,0xFF,0xFF,0x3F,
0x3E,0x0F,0xFF,0xFF,0x01,0xFF,0xFF,0xFF,0xFF,0x3F,0x3E,0x0F,0xFF,0xFF,0x01,0xFF,0xFF,0xFF,0xFF,0x3F,0x7E,0x0F,0xFF,0xFF,0x01,0xFF,0xFF,0xFF,0xFF,0x3F,0x7E,0x0F,0xFF,0xFF,0x01,0xFF,0xFF,0xFF,0xFF,0xBF,0x7F,0x0F,0xFF,0xFF,0x01,0xFF,0xFF,0xFF,0xFF,0xBF,0x7F,0x0F,0xFF,0xFF,0x01,0xFF,0xFF,0xFF,0xFF,0xBF,
0x7F,0x07,0xFF,0xFE,0x01,0xFF,0xBF,0xFF,0xFF,0x3F,0x7F,0x87,0xFF,0xFE,0x00,0xFF,0xBF,0xFF,0xFF,0x3F,0x7F,0x87,0xFF,0xFE,0x00,0xFF,0xDF,0xFF,0xFF,0x3F,0x7F,0x87,0xFF,0xFE,0x00,0xFF,0xCF,0xFF,0xFF,0x3F,0x7F,0xC7,0xFF,0xFE,0x00,0x7F,0xC7,0xFF,0xFF,0x3F,0x7F,0xE3,0xFF,0xFE,0x00,0x7F,0xE3,0xFF,0xFF,0x3F,
0x3F,0xE3,0xFF,0xFE,0x00,0x3F,0xE3,0xFF,0xFE,0x3F,0x3F,0xF3,0xFF,0xFE,0x00,0x3F,0xF1,0xFF,0xFE,0x3F,0x1F,0xF1,0xFF,0xFE,0x00,0x1F,0xF0,0xFF,0xFE,0x3F,0x1F,0xF1,0xFE,0xFE,0x00,0x1F,0xF0,0xFF,0xFC,0x3F,0x0F,0xB8,0xFC,0xFE,0x00,0x0F,0xF0,0x7F,0xFC,0x3F,0x0F,0xF8,0xF8,0xFE,0x00,0x07,0xF8,0x7F,0xFC,0x3F,
0x07,0xD8,0xF8,0xFE,0x00,0x07,0xF8,0x3F,0xF8,0x3F,0x03,0xD8,0x70,0xFE,0x00,0x03,0xF0,0x3F,0xF8,0x3F,0x03,0xCC,0x70,0xFE,0x00,0x01,0xF0,0x3F,0xF0,0x3F,0x01,0xCC,0x20,0xFE,0x00,0x00,0xF0,0x3F,0xE0,0x3F,0x00,0x84,0x20,0xFE,0x00,0x00,0x60,0x3F,0xE0,0x3F,0x00,0x00,0x00,0xFE,0x00,0x00,0x20,0x3F,0xC0,0x3F,
0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x3F,0x80,0x3F,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x3F,0x00,0x3F,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x7E,0x00,0x3F,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0xF8,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x80,0x00,0x3F,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,};

static const unsigned char Blaze[] = {
0x00,0x00,0x66,0x00,0x1E,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFC,0x03,0xF8,0x00,0x0F,0xFF,0x00,0xFF,0xFF,0xE3,0xFF,0xF3,0xFF,0xFF,0x03,0xF8,0x00,0x0F,0xFF,0x00,0xFF,0xFF,0xC3,0xFF,0xF3,0xFF,0xFF,0x83,0xF8,0x00,0x1F,0xFF,0x80,0xFF,0xFF,0xC3,0xFF,0xF3,0xFF,0xFF,0x83,0xF8,0x00,0x1F,0xFF,0x80,
0xFF,0xFF,0xC3,0xFF,0xF3,0xFF,0xFF,0xC3,0xF8,0x00,0x1F,0xFF,0x80,0xFF,0xFF,0x83,0xFF,0xF3,0xFF,0xFF,0xC3,0xF8,0x00,0x1F,0xFF,0x80,0xFF,0xFF,0x83,0xFF,0xF3,0xFE,0x7F,0xC3,0xF8,0x00,0x1F,0xFF,0x80,0xFF,0xFF,0x03,0xFF,0xF3,0xFE,0x3F,0xC3,0xF8,0x00,0x3F,0xFF,0xC0,0x00,0xFF,0x03,0xF8,0x03,0xFE,0x3F,0xC3,
0xF8,0x00,0x3F,0xFF,0xC0,0x01,0xFF,0x03,0xF8,0x03,0xFE,0x3F,0xC3,0xF8,0x00,0x3F,0xFF,0xC0,0x01,0xFE,0x03,0xF8,0x03,0xFE,0x7F,0xC3,0xF8,0x00,0x3F,0xFF,0xC0,0x03,0xFE,0x03,0xFF,0xC3,0xFF,0xFF,0x83,0xF8,0x00,0x3F,0xFF,0xC0,0x03,0xFC,0x03,0xFF,0xC3,0xFF,0xFF,0x83,0xF8,0x00,0x7F,0x9F,0xE0,0x07,0xFC,0x03,
0xFF,0xC3,0xFF,0xFF,0x03,0xF8,0x00,0x7F,0x9F,0xE0,0x07,0xF8,0x03,0xFF,0xC3,0xFF,0xFF,0x03,0xF8,0x00,0x7F,0x9F,0xE0,0x07,0xF8,0x03,0xFF,0xC3,0xFF,0xFF,0x83,0xF8,0x00,0x7F,0x9F,0xE0,0x0F,0xF8,0x03,0xFF,0xC3,0xFF,0xFF,0xC3,0xF8,0x00,0xFF,0x9F,0xF0,0x0F,0xF0,0x03,0xFF,0xC3,0xFE,0x3F,0xC3,0xF8,0x00,0xFF,
0xFF,0xF0,0x1F,0xF0,0x03,0xF8,0x03,0xFE,0x1F,0xC3,0xF8,0x00,0xFF,0xFF,0xF0,0x1F,0xE0,0x03,0xF8,0x03,0xFE,0x1F,0xC3,0xF8,0x00,0xFF,0xFF,0xF0,0x1F,0xE0,0x03,0xF8,0x03,0xFE,0x1F,0xC3,0xF8,0x00,0xFF,0xFF,0xF0,0x3F,0xE0,0x03,0xF8,0x03,0xFE,0x3F,0xC3,0xFF,0xF9,0xFF,0xFF,0xF8,0x3F,0xFF,0xE3,0xFF,0xF3,0xFF,
0xFF,0xC3,0xFF,0xF9,0xFF,0xFF,0xF8,0x7F,0xFF,0xE3,0xFF,0xF3,0xFF,0xFF,0xC3,0xFF,0xF9,0xFE,0x07,0xF8,0x7F,0xFF,0xE3,0xFF,0xF3,0xFF,0xFF,0x83,0xFF,0xF9,0xFE,0x07,0xF8,0xFF,0xFF,0xE3,0xFF,0xF3,0xFF,0xFF,0x83,0xFF,0xF9,0xFE,0x07,0xF8,0xFF,0xFF,0xE3,0xFF,0xF3,0xFF,0xFF,0x03,0xFF,0xFB,0xFC,0x03,0xFC,0xFF,
0xFF,0xE3,0xFF,0xF3,0xFF,0xFC,0x03,0xFF,0xFB,0xFC,0x03,0xFD,0xFF,0xFF,0xE3,0xFF,0xF3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,};

static const unsigned char Tasker[] = {
0x00,0x00,0x4C,0x00,0x13,
0xFF,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x00,0x0F,0xFF,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x00,0x0F,0xFF,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x00,0x0F,0x1F,0x00,0x00,0x00,0x01,0xF0,0x00,0x00,0x00,0x0F,0x1F,0x00,0x00,0x00,0x01,0xF0,0x00,0x00,0x00,0x0F,0x1F,0x00,0xFE,0x07,0xE1,0xF1,0xF0,0xFC,0x1F,0x7F,
0x1F,0x03,0xFF,0x0F,0xF1,0xF3,0xE1,0xFE,0x1F,0xFF,0x1F,0x03,0xFF,0x9F,0xF9,0xF7,0xC3,0xFF,0x1F,0xFF,0x1F,0x07,0xCF,0x9F,0x71,0xFF,0x83,0xEF,0x1F,0xFF,0x1F,0x07,0xCF,0x9F,0x01,0xFF,0x07,0xCF,0x9F,0x8F,0x1F,0x00,0x7F,0x9F,0xC1,0xFF,0x87,0xCF,0x9F,0x8F,0x1F,0x01,0xFF,0x8F,0xF1,0xFF,0x87,0xFF,0x9F,0x0F,
0x1F,0x03,0xEF,0x8F,0xF9,0xFF,0x87,0xFF,0x9F,0x0F,0x1F,0x07,0xCF,0x83,0xF9,0xFF,0xC7,0xC0,0x1F,0x0F,0x1F,0x07,0xCF,0x9E,0xF9,0xF7,0xC7,0xCF,0x9F,0x0F,0x1F,0x07,0xDF,0x9E,0xF9,0xF3,0xE3,0xCF,0x9F,0x0F,0x1F,0x07,0xFF,0x9F,0xF9,0xF3,0xE3,0xFF,0x1F,0x0F,0x1F,0x03,0xFF,0x8F,0xF1,0xF3,0xE1,0xFF,0x1F,0x0F,
0x1F,0x01,0xFF,0x87,0xE1,0xF1,0xF0,0xFC,0x1F,0x0F,};

static const unsigned char u_symbol[] = {
0x00,0x00,0x1E,0x00,0x20,
0x00,0x00,0x00,0x03,0x00,0x00,0x3E,0x3F,0x00,0x00,0x7E,0x7F,0x00,0x00,0xFC,0xFF,0x00,0x00,0xFC,0xFF,0x00,0x01,0xF9,0xFB,0x00,0x03,0xF3,0xF3,0x00,0x03,0xF3,0xF3,0x00,0x07,0xE7,0xE3,0x00,0x07,0xC7,0xC7,0x00,0x0F,0xCF,0xCF,0x00,0x1F,0x9F,0x8F,0x00,0x1F,0x9F,0x9F,0x00,0x3F,0x3F,0xBB,0x00,0x7F,0x7F,0xF3,
0x00,0x7F,0xFF,0xF3,0x00,0xFF,0xEF,0xC3,0x00,0xFB,0x03,0x83,0x01,0xF8,0x00,0x03,0x03,0xF0,0x00,0x03,0x03,0xE0,0x00,0x03,0x07,0xE0,0x00,0x03,0x07,0xC0,0x00,0x03,0x0F,0xC0,0x00,0x03,0x1F,0x80,0x00,0x03,0x1F,0x80,0x00,0x03,0x3F,0x00,0x00,0x03,0x3E,0x00,0x00,0x03,0x7E,0x00,0x00,0x03,0x7C,0x00,0x00,0x03,
0xFC,0x00,0x00,0x03,0xF8,0x00,0x00,0x03,};
