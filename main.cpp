/*
    Bắt đầu phần khai báo các thư viện, header file trong C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>





/*
    bắt đầu phần define tên kiểu dữ liệu thành tên khác.
        thực ra chẳng define cũng đc, define để viết cho ngắn và thân thiện hơn thôi
        unsigned là gì?
            bỏ miền giá trị âm => max được tăng lên gấp đôi
            EX: nếu char range [-128, 127] thì unsigned char có range từ [0, 255]
 */
#ifndef WORD
#define WORD unsigned short

#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef LONG
#define LONG long
#endif

#define minGW 1

#ifdef minGW
#pragma pack(push, 2)
#endif




/*
    kiểu dữ liệu người dùng định nghĩa: struct
    thằng này chứa các thông tin header của image
 */
typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#ifdef minGW
#pragma pack(pop)
#endif




/*
    kiểu dữ liệu người dùng định nghĩa: struct
    thằng này chứa các thông tin header của image
 */

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;




/*
    kiểu dữ liệu người dùng định nghĩa: struct
    thằng này chứa thông tin màu sắc của từng pixel image
 */

typedef struct tagRGBQUAD
{
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} RGBQUAD, *LPRGBQUAD;




/*
    kiểu dữ liệu người dùng định nghĩa: struct
    Đối tượng lưu thông tin của image
 */

typedef struct tagBITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;



/*
    đối tượng image
 */

typedef struct tagBITMAP
{
    BITMAPINFOHEADER bmInfo;
    unsigned char *pBits;
} BITMAP, *PBITMAP;


/*
    Đọc ảnh từ file có đường dẫn là `filename`
 */
PBITMAP readBitmapFile(const char *filename)
{
    FILE *fp;
    PBITMAP result;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFO bmpInfo;

    fp = fopen(filename, "rb");

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    fread(&bmpInfo, sizeof(BITMAPINFO), 1, fp);

    fseek(fp, fileHeader.bfOffBits, SEEK_SET);
    unsigned char *pBits = (unsigned char *)calloc(bmpInfo.bmiHeader.biSizeImage, sizeof(unsigned char));
    fread(pBits, sizeof(char), bmpInfo.bmiHeader.biSizeImage, fp);
    fclose(fp);

    result = (PBITMAP)calloc(1, sizeof(*result));
    memcpy(&result->bmInfo, &bmpInfo, sizeof(bmpInfo));
    result->pBits = pBits;
    return result;
}


/*
    Lưu ảnh `img` vào file có đường dẫn là `filename`
 */

void saveBitmapFile(const char *filename, PBITMAP img)
{
    FILE *fp;
    BITMAPFILEHEADER fileHeader;
    memset(&fileHeader, 0, sizeof(fileHeader));
    fileHeader.bfType = 0x4d42; //'BM'
    fileHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    fileHeader.bfSize = fileHeader.bfOffBits + (img->bmInfo.biSizeImage);

    fp = fopen(filename, "wb");
    fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
    fwrite(&img->bmInfo, sizeof(img->bmInfo), 1, fp);
    fwrite(img->pBits, 1, img->bmInfo.biSizeImage, fp);
    fclose(fp);
}


/*
    Set giá trị màu cho pixel ở tọa độ (x, y)
    Một pixel sẽ có 3 giá trị màu red, green, blue
 */

void setColor(int x, int y, BYTE r, BYTE g, BYTE b, PBITMAP image)
{
    unsigned char *ptrToPixel;
    int rowSize = ((image->bmInfo.biBitCount * image->bmInfo.biWidth + 31) / 32) * 4;
    int curRow, bytesPerPixel;
    if (image->bmInfo.biHeight < 0)
    {
        curRow = y;
    }
    else if (image->bmInfo.biHeight > 0)
    {
        curRow = (image->bmInfo.biHeight - 1) - y;
    }
    bytesPerPixel = image->bmInfo.biBitCount / 8;
    ptrToPixel = (curRow * rowSize) + (x * bytesPerPixel) + image->pBits;

    ptrToPixel[0] = b;
    ptrToPixel[1] = g;
    ptrToPixel[2] = r;
}


/*
    Get giá trị màu cho pixel ở tọa độ (x, y)
    Một pixel sẽ có 3 giá trị màu red, green, blue
 */
void getColor(int x, int y, BYTE &r, BYTE &g, BYTE &b, PBITMAP image)
{
    unsigned char *ptrToPixel;
    int rowSize = ((image->bmInfo.biBitCount * image->bmInfo.biWidth + 31) / 32) * 4;
    int curRow, bytesPerPixel;
    if (image->bmInfo.biHeight < 0)
    {
        curRow = y;
    }
    else if (image->bmInfo.biHeight > 0)
    {
        curRow = (image->bmInfo.biHeight - 1) - y;
    }
    bytesPerPixel = image->bmInfo.biBitCount / 8;
    ptrToPixel = (curRow * rowSize) + (x * bytesPerPixel) + image->pBits;

    b = ptrToPixel[0] > 255 ? 255 : ptrToPixel[0];
    g = ptrToPixel[1] > 255 ? 255 : ptrToPixel[1];
    r = ptrToPixel[2] > 255 ? 255 : ptrToPixel[2];
}

int main()
{

    // đọc ảnh
    PBITMAP inImage = readBitmapFile("inp.bmp");
    // kiểm tra có phải bit depth là 24 không,
    // nếu khác 24 => thoát ct
    if (inImage->bmInfo.biBitCount != 24)
    {
        printf("Input image must be 24 bits depth!");
        exit(0);
    }
    // Lấy width, height của ảnh
    int w = inImage->bmInfo.biWidth;
    int h = inImage->bmInfo.biHeight;

    BYTE r, g, b;
    // duyệt qua từng pixal
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            // lấy màu của pixel (i, j)
            getColor(i, j, r, g, b, inImage);
            // nếu pixel có màu trắng (r,g,b = 255,255,255 là trắng tuyệt đối) => nền
            if (r >= 250 && g >= 250 && b >= 250)
            {
                continue;
            }
            // nếu pixel có màu đỏ (r,g,b = 255,0,0 là đỏ tuyệt đối) => hình tròn
            else if (r >= 250 && g + b <= 10)
            {
                continue;
            }
            // nếu không phải 2 trường hợp trên => đổi nó thành màu đen
            // màu đen có (r,g,b) = (0,0,0)
            else
            {
                setColor(i, j, 0, 0, 0, inImage);
            }
        }
    }
    // lưu ảnh
    saveBitmapFile("out.bmp", inImage);
}