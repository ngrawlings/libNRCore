//
//  Bitmap.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 23/07/2014.
//  Copyright (c) 2014 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__Bitmap__
#define __libNRCore__Bitmap__

class Bitmap {
public:
    Bitmap(unsigned char data, unsigned int width, unsigned int height, unsigned int bits_per_pixel, unsigned int bytes_per_scanline);
    ~Bitmap();
    
    unsigned char* pixel(unsigned int row, unsigned int col);
    unsigned char* scanline(unsigned int row);
    
    Bitmap getRect(unsigned int row, unsigned int col, unsigned int width, unsigned int height);
    Bitmap rotate(double degrees);
    
protected:
    unsigned int width;
    unsigned int height;
    unsigned int bits_per_pixel;
    unsigned int bytes_per_scanline;
    
    unsigned char data;
};

#endif /* defined(__libNRCore__Bitmap__) */
