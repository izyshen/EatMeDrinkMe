/*
 * ********** TAKEN FROM https://gist.github.com/niw/5963798 ***************
 *
 * A simple libpng example program
 * http://zarb.org/~gc/html/libpng.html
 *
 * Modified by Yoshimasa Niwa to make it much simpler
 * and support all defined color_type.
 *
 * To build, use the next instruction on OS X.
 * $ brew install libpng
 * $ clang -lz -lpng15 libpng_test.c
 *
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */
#pragma once 

#include <stdlib.h>
#include <stdio.h>
#include <png.h>

extern int png_width, png_height;
extern png_byte color_type;
extern png_byte bit_depth;
extern png_bytep *row_pointers;

void read_png_file(const std::string &filename);
