#include"BMP.h"

class Rysownik{
	public:
	
	BMP bmp;
	
	Rysownik(uint16_t width = 800, uint16_t height = 600) : bmp(width, height){}
	//////////////////////////////////////////////////////////////////////////////////////
	Rysownik & operator=(const Rysownik & r)
	{
		return (*this);
	}
	//////////////////////////////////////////////////////////////////////////////////////
	bool getColor(uint16_t x , uint16_t y,unsigned char r, unsigned char g,
		unsigned char b);
	virtual void drawLine(int x1 , int y1 , int x2 , int y2 ,unsigned char r, unsigned char g,
			unsigned char b );
	
};


void Rysownik::drawLine(int x1 , int y1 , int x2 , int y2 ,unsigned char r, unsigned char g,
			unsigned char b ) 
{
	int d, dx, dy, ai, bi, xi, yi;
    int x = x1, y = y1;
    
    if(x1<x2)
	{
		xi = 1;
		dx = x2 - x1;	
	}
	else
	{
		xi = -1;
		dx = x1 - x2;
	}
	
	if(y1<y2)
	{
		yi = 1;
		dy = y2 - y1;
	} 
	else
	{
		yi = -1;
		dy = y1 - y2;
	}
	
	if(x<this->bmp.bitmapCoreHeader.bmpWidth && y<this->bmp.bitmapCoreHeader.bmpHeight)
	{
		this->bmp.setPixel(x,y,r,g,b);
	}
	if(dx>dy)
	{
		ai = (dy - dx)*2;
		bi = dy*2;
		d = bi - dx;
		
		while(x != x2)
		{
			if(d >= 0)
			{
				x += xi;
				y += yi;
				d += ai;
			}
			else
			{
				d += bi;
				x += xi;
			}
			if(x<this->bmp.bitmapCoreHeader.bmpWidth && y<this->bmp.bitmapCoreHeader.bmpHeight)
			{
				this->bmp.setPixel(x,y,r,g,b);
			}
		}	
	}
	else
	{
		ai = (dx - dy)*2;
		bi = dx*2;
		d = bi - dy;
		
		while(y != y2)
		{
			if(d >= 0)
			{
				x += xi;
				y += yi;
				d += ai;	
			}
			else
			{
				d += bi;
				y += yi;
			}
			if(x<this->bmp.bitmapCoreHeader.bmpWidth && y<this->bmp.bitmapCoreHeader.bmpHeight)
			{
				this->bmp.setPixel(x,y,r,g,b);
			}	
		}	
	}
	
}

bool Rysownik::getColor(uint16_t x , uint16_t y,unsigned char r, unsigned char g,
		unsigned char b)
{
	assert(this->bmp.bitmapCoreHeader.bitsPerPixel == 24);

	const size_t rowSize = ((this->bmp.bitmapCoreHeader.bitsPerPixel
			* this->bmp.bitmapCoreHeader.bmpWidth + 31) / 32) * 4;
	const size_t offset = rowSize * (this->bmp.bitmapCoreHeader.bmpHeight - y)
			+ x * (this->bmp.bitmapCoreHeader.bitsPerPixel / 8);

	if(this->bmp.pixelData[offset + 0] == b && this->bmp.pixelData[offset + 1] == g && this->bmp.pixelData[offset + 2] == r)
	{
		return false;
	}
	return true;
}