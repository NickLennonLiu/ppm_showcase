#include "bmp.hpp"
#include <vecmath.h>
#include "ray.hpp"
// Original Code from: https://blog.taoky.moe/attachments/bmp1.cpp

inline int ToHumanRead(char *str, int size)
{
  // Convert to Big Endian
  long l = 0;
  std::memcpy(&l, str, size);
  return (signed int)l;
}


void Output_FileHeader(BITMAPFILEHEADER& FileHeader)
{
  cout << "File Size (BITMAPFILEHEADER): " << ToHumanRead(FileHeader.bfSize) << endl;
  cout << "Reserved 1 (usually 0, BITMAPFILEHEADER): " << ToHumanRead(FileHeader.bfReserved1, 2) << endl;
  cout << "Reserved 2 (usually 0, BITMAPFILEHEADER): " << ToHumanRead(FileHeader.bfReserved2, 2) << endl;
  cout << "Bitmap Data Offset (BITMAPFILEHEADER): " << ToHumanRead(FileHeader.bfOffBits) << endl;
}

void Output_InfoHeader(BITMAPINFOHEADER& InfoHeader)
{
  cout << "BITMAPINFOHEADER Size: " << ToHumanRead(InfoHeader.biSize) << endl;
  cout << "Width (BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biWidth) << endl;
  cout << "Height (BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biHeight) << endl;
  cout << "Number of Color Planes (BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biPlanes, 2) << endl;
  cout << "Number of Bits per Pixel (BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biBitCount) << endl;
  cout << "Compression Type (0 is none, BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biCompression) << endl;
  cout << "Original Size of Bitmap (0 usually if no compression, BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biSizeImage) << endl;
  cout << "Number of Horizontal Pixels per Meter (0 usually, BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biXPelsPerMeter) << endl;
  cout << "Number of Vertical Pixels per Meter (0 usually, BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biYPelsPerMeter) << endl;
  cout << "Number of Color Used (0 sometimes, BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biClrUsed) << endl;
  cout << "Number of Important Color (0 means all is important, BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biClrImportant) << endl;
}

void drop_alpha(ifstream& bmpfile, BITMAPINFOHEADER& InfoHeader)
{
  if (ToHumanRead(InfoHeader.biBitCount) == 32)
  {

    unsigned char null;
    bmpfile.read((char *)&null, sizeof(null));
  }
}

vector <Pixel> * ReadBitmap(ifstream& bmpfile, BITMAPINFOHEADER& InfoHeader)
{
  int offset = 0;
  int height = ToHumanRead(InfoHeader.biHeight);
  int width = ToHumanRead(InfoHeader.biWidth);
  Pixel p;
  int linebyte = width * ToHumanRead(InfoHeader.biBitCount) / 8;
  offset = linebyte % 4;
  if (offset != 0) offset = 4 - offset; // "linebyte" should be the multiple of 4
  cout << "Offset: " << offset << endl;
  vector <Pixel> *vec = new vector <Pixel> [abs(height)];
  // Height could be a negative number.
  bool isBottom = false;
  if (ToHumanRead(InfoHeader.biHeight) > 0) // read from bottom
  {
    isBottom = true;
  }
  for (int i = 0; i < abs(height); i++)
  {
    for (int j = 0; j < width; j++)
    {
      bmpfile.read((char *)&p, sizeof(p));
      if (!isBottom) vec[i].push_back(p);
      else vec[height - i - 1].push_back(p);
      drop_alpha(bmpfile, InfoHeader);
    }
    for (int j = 0; j < offset; j++)
    {
      unsigned char null;
      bmpfile.read((char *)&null, sizeof(null));
    }
  }
  return vec;
}

void PrintLocation(int x, int y, vector <Pixel> * vec)
{
  cout << (int)vec[x][y].r << " " << (int)vec[x][y].g << " " << (int)vec[x][y].b << endl;
}

Vector3f ToColor(Pixel p)
{
    int r = (int)p.r;
    int g = (int)p.g;
    int b = (int)p.b;
    return Vector3f(r/255.0, g/255.0, b/255.0);
}

vector<Vector3f> bmp(char *filename, BITMAPFILEHEADER& FileHeader, BITMAPINFOHEADER& InfoHeader)
{
  vector<Pixel>* vec;
  vector<Vector3f> texture;
  ifstream bmpfile;
  bmpfile.open(filename, ios::in | ios::binary); // open the file
  if (bmpfile.is_open())
  {
    bmpfile.read((char *)&FileHeader, sizeof(FileHeader)); // Read BITMAPFILEHEADER
    if (strncmp(FileHeader.bfType, "BM", 2) != 0) // Judge whether a BMP.
    {
      cout << "Not a BMP File, or an Unsupported OS/2 BMP File." << endl;
      return vector<Vector3f>();
    }
    bmpfile.read((char *)&InfoHeader, sizeof(InfoHeader)); // Read BITMAPINFOHEADER
    /* Information Output */
    cout << "=============Texture Information===============" << endl;
    Output_FileHeader(FileHeader);
    Output_InfoHeader(InfoHeader);
    /* Output End */
    vec = ReadBitmap(bmpfile, InfoHeader); // Read the data
    cout << "Finished reading bmp" << endl;
    cout << "===============================================" << endl;
    for(int i = 0; i < 500; ++i)
      for(int j = 0; j < 500; ++j)
        texture.push_back(ToColor(vec[i][j]));
        
    bmpfile.close();
  }
  else
  {
    cout << "Open File Error." << endl;
    return vector<Vector3f>();
  }
  return texture;
}