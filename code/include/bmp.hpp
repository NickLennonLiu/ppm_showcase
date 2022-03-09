#ifndef BMP_H
#define BMP_H

// Original Code from: https://blog.taoky.moe/attachments/bmp1.cpp

#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <vecmath.h>
using namespace std;

struct BITMAPFILEHEADER
{
  char bfType[2]; // must be "BM"
  char bfSize[4]; // the size of the bmp file
  char bfReserved1[2];
  char bfReserved2[2];
  char bfOffBits[4]; // the offset to the bitmap data
} ;

struct BITMAPINFOHEADER
{
  char biSize[4]; // the size of BITMAPINFOHEADER
  char biWidth[4]; // width (pixels)
  char biHeight[4]; // height (pixels)
  char biPlanes[2]; // color planes
  char biBitCount[2]; // bits per pixel
  char biCompression[4]; // type of compression (0 is no compression)
  char biSizeImage[4]; // the origin size of the bitmap data (before compression)
  char biXPelsPerMeter[4]; // horizontal pixels per meter
  char biYPelsPerMeter[4]; // vertical pixels per meter
  char biClrUsed[4]; // the number of colors used
  char biClrImportant[4]; // "important" colors, usually 0
} ;

struct Pixel
{
  unsigned char b;
  unsigned char g;
  unsigned char r;
};

inline int ToHumanRead(char *str, int size = 4);


void Output_FileHeader(BITMAPFILEHEADER& FileHeader);

void Output_InfoHeader(BITMAPINFOHEADER& InfoHeader);

void drop_alpha(ifstream& bmpfile, BITMAPINFOHEADER& InfoHeader);

vector <Pixel> * ReadBitmap(ifstream& bmpfile, BITMAPINFOHEADER& InfoHeader);

void PrintLocation(int x, int y, vector <Pixel> * vec);

vector<Vector3f> bmp(char *filename, BITMAPFILEHEADER& FileHeader, BITMAPINFOHEADER& InfoHeader);

#endif