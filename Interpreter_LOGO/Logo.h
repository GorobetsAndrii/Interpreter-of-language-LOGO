#include"Rysownik.h"
#include<vector>
#include<cstring>
#include<utility>

union Bag
{
	int num;
	float tab[3];
};

/*--------------------------- ZMIENIĆ WSZYSTKO NA UNION -----------------------------------------------*/
class Interpreter;

class Command
{
public:

	virtual bool Check(const char *str) = 0;
	virtual void Work(Interpreter &Logo,float tab[3]) = 0;
};

class Interpreter
{
	friend class Command;
public:
	Rysownik *paint;
	std::vector<Command *> lista;
	float R_angle;
	float L_angle;
	int X_Pos;
	int Y_Pos;
	unsigned char r_pen;
	unsigned char g_pen;
	unsigned char b_pen;
	unsigned char r_back;
	unsigned char g_back;
	unsigned char b_back;
	bool State_Pen;
	bool State_Erase;
//	bool Loop_X;
//	bool Loop_Y;

public:
	Interpreter(const std::vector<Command *> &temp, Rysownik &r)
	{
		lista = temp;
		R_angle = 0;
		L_angle = 0;
		r_pen = 0;
		g_pen = 0;
		b_pen = 0;
		r_back = 255;
		g_back = 255;
		b_back = 255;
		State_Pen = true;
		State_Erase = false;
		paint = &r;
//		Loop_X = false;
//		Loop_Y = false;

		if(paint->bmp.bitmapCoreHeader.bmpWidth % 2 == 0) X_Pos = paint->bmp.bitmapCoreHeader.bmpWidth / 2;
		else X_Pos = paint->bmp.bitmapCoreHeader.bmpWidth % 2 - 1 ;

		if(paint->bmp.bitmapCoreHeader.bmpHeight %2 == 0) Y_Pos = paint->bmp.bitmapCoreHeader.bmpHeight / 2;
		else Y_Pos = paint->bmp.bitmapCoreHeader.bmpHeight / 2 - 1;
	}

	void Read_File(char *name)
	{
		std::ifstream file;
		file.open(name);

		if(!file.is_open())
		{
			std::cout<<"File no found"<<std::endl;
			return;
		}

		float tab[3];
		std::string namestr;
		
		char kom[2] = "#";
		
		while(file)
		{
			std::getline(file,namestr);
			std::stringstream str(namestr);

			if(strcmp(namestr.c_str(),"#") == 0 || namestr[0] == kom[0]) continue;

			while(str>>namestr)
			{	
				
				if(strcmp(namestr.c_str(),"REPEAT") == 0)
				{
					int il;
					str>>il;
					str>>namestr;
					std::vector<std::pair<std::string,float>> vi;
					
					while(strcmp(namestr.c_str(),"]") != 0)
					{
							str>>namestr;
							float tfp;
							str>>tfp;
							vi.push_back(std::pair<std::string,float>(namestr,tfp));
					}
					vi.pop_back();
					
					for(int i = 0; i < il ; ++i)
					{
						for(auto vl = vi.begin() ; vl != vi.end(); vl++)
						{
							
							for(int j = 0; j<lista.size(); ++j)
							{
								if(lista[j]->Check(vl->first.c_str()) == 1)
								{
									tab[0] = vl->second;
									lista[j]->Work((*this),tab);
								}
							}
						}
					}
					continue;
				}				
				
				for(int i = 0 ; i < lista.size() ; ++i)
				{
					if(lista[i]->Check(namestr.c_str()) == 1)
					{
						int k = 0;
						while(str>>tab[k] && k<3)
						{
							k++;
						}
						lista[i]->Work((*this),tab);
					}	
				}
			}
		}
		
		file.close();

		std::ofstream outfile("test.bmp",std::ofstream::binary);
		outfile << paint->bmp;
	}
};


class Forward : public Command
{
public:
	
	char *name1;
	char *name2;

	Forward()
	{
		
		name1 = new char[16];
		name2 = new char[3];
		strcpy(name1,"FORWARD");
		strcpy(name2,"FD");
	}

	~Forward()
	{
		delete [] name1;
		delete [] name2;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name1) == 0 || strcmp(str,name2) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		int x1,y1;
		x1 = tab[0]*cos(((Logo.L_angle + Logo.R_angle)*M_PI)/180) + Logo.X_Pos;
		y1 = tab[0]*sin(((Logo.L_angle + Logo.R_angle)*M_PI)/180) + Logo.Y_Pos;

		if(Logo.State_Erase == true)
		{
			if(x1 <= Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 <= Logo.paint->bmp.bitmapCoreHeader.bmpHeight && x1 >= 0 && y1 >= 0)
			{
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
				return;
			}
			if(x1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 > 0)
			{
				int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
				int b = (y1 - a*x1);
				x1 = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
							
				y1 = a*x1 + b;
							
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
					
			}
			else if(x1 < 0)
			{
				int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
				int b = (y1 - a*x1);

				x1 = 1;
							
				y1 = a*x1 + b;
							
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;

			}
			if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 > 0)
			{
				int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
				int b = (y1 - a*x1);
				y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
				x1 = (y1 - b)/a;
							
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
					
			}
			else if(y1 < 0)
			{
				int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
				int b = (y1 - a*x1);
						
				y1 = 1;
							
				x1 = -b/a;
							
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
			}
		
		}
		else if(Logo.State_Pen == false)
		{
			if(x1 <= Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 <= Logo.paint->bmp.bitmapCoreHeader.bmpHeight && x1 >= 0 && y1 >= 0)
			{
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
				return;
			}
	
				if(x1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						x1 = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
							
						y1 = a*x1 + b;
							
					
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
						
					}
				else if(x1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);

						x1 = 1;
							
						y1 = a*x1 + b;
							
						
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
						
					}
				
			
				if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
						x1 = (y1 - b)/a;
							
						
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					
					}
					else if(y1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						
						y1 = 1;
							
						x1 = -b/a;
							
						
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					}
		}
		else if(Logo.State_Pen = true)
		{
			if(x1 <= Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 <= Logo.paint->bmp.bitmapCoreHeader.bmpHeight && x1 >= 0 && y1 >= 0)
			{
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
				return;
			}

		
				
					if(x1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						x1 = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
							
						y1 = a*x1 + b;
							
						Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					
					}
					else if(x1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);

						x1 = 1;
							
						y1 = a*x1 + b;
							
						Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;

					}
				
			
				if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
						x1 = (y1 - b)/a;
							
						Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					
					}
					else if(y1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						
						y1 = 1;
							
						x1 = -b/a;
							
						Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					}
				return;	
			
			/*
			int R = tab[0];
			while(R > 1)
			{
				x1 = R*cos(((Logo.L_angle + Logo.R_angle)*M_PI)/180) + Logo.X_Pos;
				y1 = R*sin(((Logo.L_angle + Logo.R_angle)*M_PI)/180) + Logo.Y_Pos;
				
				bool xl = false, xr = false , yl = false, yr = false; 
				if(Logo.Loop_X)
				{
					if(x1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && y1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						x1 = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
							
						y1 = a*x1 + b;
						xr = true;
					}
					else if(x1 < 0 && y1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						x1 = 1;
						y1 = a*x1 + b;
						xl = true;
					}
					else if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 > 0 )
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
						x1 = (y1 - b)/a;
						yr = true;
					}
					else if(y1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						
						y1 = 1;
							
						x1 = -b/a;
						yl = true;
					}

				}
				/*
				if(Logo.Loop_Y)
				{
					if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 >= 0 && y1 >= 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
						x1 = (y1 - b)/a;
						yr = true;
					}
					else if(y1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						
						y1 = 1;
							
						x1 = -b/a;
						yl = true;
					}
				}
				
				//std::cout<<x1<<"  "<<y1<<std::endl;
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
				R -= (x1 - Logo.X_Pos)/(cos(((Logo.L_angle + Logo.R_angle)*M_PI)/180));
				std::cout<<R<<std::endl;
				
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
				
				if(Logo.X_Pos == 1) Logo.X_Pos = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
				else if(Logo.X_Pos == Logo.paint->bmp.bitmapCoreHeader.bmpWidth) Logo.X_Pos = 1;
				
				if(Logo.Y_Pos == 1) Logo.Y_Pos = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
				else if(Logo.Y_Pos == Logo.paint->bmp.bitmapCoreHeader.bmpHeight) Logo.Y_Pos = 1;
				*/
			} 
		}
};

class Back : public Command
{
public:
	
	char *name1;
	char *name2;

	Back()
	{
		name1 = new char[16];
		name2 = new char[3];
		strcpy(name1,"BACK");
		strcpy(name2,"BK");
	}

	~Back()
	{
		delete [] name1;
		delete [] name2;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name1) == 0 || strcmp(str,name2) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		int x1,y1;
		x1 = tab[0]*cos(((Logo.L_angle + Logo.R_angle + 180)*M_PI)/180) + Logo.X_Pos;
		y1 = tab[0]*sin(((Logo.L_angle + Logo.R_angle + 180)*M_PI)/180) + Logo.Y_Pos;

		if(Logo.State_Erase == true)
		{
			if(x1 <= Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 <= Logo.paint->bmp.bitmapCoreHeader.bmpHeight && x1 >= 0 && y1 >= 0)
			{
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
				return;
			}
			if(x1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 > 0)
			{
				int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
				int b = (y1 - a*x1);
				x1 = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
							
				y1 = a*x1 + b;
							
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
					
			}
			else if(x1 < 0)
			{
				int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
				int b = (y1 - a*x1);

				x1 = 1;
							
				y1 = a*x1 + b;
							
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;

			}
			if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 > 0)
			{
				int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
				int b = (y1 - a*x1);
				y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
				x1 = (y1 - b)/a;
							
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
					
			}
			else if(y1 < 0)
			{
				int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
				int b = (y1 - a*x1);
						
				y1 = 1;
							
				x1 = -b/a;
							
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_back,Logo.g_back,Logo.b_back);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
			}
		
		}
		else if(Logo.State_Pen == false)
		{
			if(x1 <= Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 <= Logo.paint->bmp.bitmapCoreHeader.bmpHeight && x1 >= 0 && y1 >= 0)
			{
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
				return;
			}
	
				if(x1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						x1 = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
							
						y1 = a*x1 + b;
							
					
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
						
					}
				else if(x1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);

						x1 = 1;
							
						y1 = a*x1 + b;
							
						
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
						
					}
				
			
				if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
						x1 = (y1 - b)/a;
							
						
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					
					}
					else if(y1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						
						y1 = 1;
							
						x1 = -b/a;
							
						
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					}
		}
		else if(Logo.State_Pen = true)
		{
			if(x1 <= Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 <= Logo.paint->bmp.bitmapCoreHeader.bmpHeight && x1 >= 0 && y1 >= 0)
			{
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
				return;
			}

		
				
					if(x1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth && y1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						x1 = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
							
						y1 = a*x1 + b;
							
						Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					
					}
					else if(x1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);

						x1 = 1;
							
						y1 = a*x1 + b;
							
						Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;

					}
				
			
				if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
						x1 = (y1 - b)/a;
							
						Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					
					}
					else if(y1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						
						y1 = 1;
							
						x1 = -b/a;
							
						Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
						Logo.X_Pos = x1;
						Logo.Y_Pos = y1;
					}
				return;	
			
			/*
			int R = tab[0];
			while(R > 1)
			{
				x1 = R*cos(((Logo.L_angle + Logo.R_angle)*M_PI)/180) + Logo.X_Pos;
				y1 = R*sin(((Logo.L_angle + Logo.R_angle)*M_PI)/180) + Logo.Y_Pos;
				
				bool xl = false, xr = false , yl = false, yr = false; 
				if(Logo.Loop_X)
				{
					if(x1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && y1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						x1 = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
							
						y1 = a*x1 + b;
						xr = true;
					}
					else if(x1 < 0 && y1 > 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						x1 = 1;
						y1 = a*x1 + b;
						xl = true;
					}
					else if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 > 0 )
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
						x1 = (y1 - b)/a;
						yr = true;
					}
					else if(y1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						
						y1 = 1;
							
						x1 = -b/a;
						yl = true;
					}

				}
				/*
				if(Logo.Loop_Y)
				{
					if(y1 > Logo.paint->bmp.bitmapCoreHeader.bmpWidth  && x1 >= 0 && y1 >= 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						y1 = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
							
						x1 = (y1 - b)/a;
						yr = true;
					}
					else if(y1 < 0)
					{
						int a = (y1 - Logo.Y_Pos)/(x1-Logo.X_Pos);
						int b = (y1 - a*x1);
						
						y1 = 1;
							
						x1 = -b/a;
						yl = true;
					}
				}
				
				//std::cout<<x1<<"  "<<y1<<std::endl;
				Logo.paint->drawLine(Logo.X_Pos,Logo.Y_Pos,x1,y1,Logo.r_pen,Logo.g_pen,Logo.b_pen);
				R -= (x1 - Logo.X_Pos)/(cos(((Logo.L_angle + Logo.R_angle)*M_PI)/180));
				std::cout<<R<<std::endl;
				
				Logo.X_Pos = x1;
				Logo.Y_Pos = y1;
				
				if(Logo.X_Pos == 1) Logo.X_Pos = Logo.paint->bmp.bitmapCoreHeader.bmpWidth;
				else if(Logo.X_Pos == Logo.paint->bmp.bitmapCoreHeader.bmpWidth) Logo.X_Pos = 1;
				
				if(Logo.Y_Pos == 1) Logo.Y_Pos = Logo.paint->bmp.bitmapCoreHeader.bmpHeight;
				else if(Logo.Y_Pos == Logo.paint->bmp.bitmapCoreHeader.bmpHeight) Logo.Y_Pos = 1;
				*/
			} 
	}
};

class Left : public Command
{
public:
	
	char *name1;
	char *name2;

	Left()
	{
		name1 = new char[16];
		name2 = new char[3];
		strcpy(name1,"LEFT");
		strcpy(name2,"LT");
	}
	
	~Left()
	{
		delete [] name1;
		delete [] name2;
	}

	virtual bool Check(const char *str)
	{
		if(strcmp(str,name1) == 0 || strcmp(str,name2) == 0) return true;
		else return false;
	}
	
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		Logo.L_angle -= tab[0];
	}
};

class Right : public Command
{
public:
	char *name1;
	char *name2;

	Right()
	{
		name1 = new char[16];
		name2 = new char[3];
		strcpy(name1,"RIGHT");
		strcpy(name2,"RT");
	}

	~Right()
	{
		delete [] name1;
		delete [] name2;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name1) == 0 || strcmp(str,name2) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		Logo.R_angle += tab[0];
	}
};

class Clearscreen : public Command
{
public:
	char *name;

	Clearscreen()
	{
		name = new char[3];
		strcpy(name,"CS");
	}

	~Clearscreen()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		for(int i = 0; i < Logo.paint->bmp.bitmapCoreHeader.bmpWidth ; ++i)
		{
			for(int j = 1; j<=Logo.paint->bmp.bitmapCoreHeader.bmpHeight; ++j)
			{
				Logo.paint->bmp.setPixel(i,j,255,255,255);
			}
		}
		if(Logo.paint->bmp.bitmapCoreHeader.bmpWidth % 2 == 0) Logo.X_Pos = Logo.paint->bmp.bitmapCoreHeader.bmpWidth / 2;
		else Logo.X_Pos = Logo.paint->bmp.bitmapCoreHeader.bmpWidth % 2 - 1 ;

		if(Logo.paint->bmp.bitmapCoreHeader.bmpHeight %2 == 0) Logo.Y_Pos = Logo.paint->bmp.bitmapCoreHeader.bmpHeight / 2;
		else Logo.Y_Pos = Logo.paint->bmp.bitmapCoreHeader.bmpHeight / 2 - 1;
	}
};

class PenUp : public Command
{
public:
	char *name;

	PenUp()
	{
		name = new char[3];
		strcpy(name,"PU");
	}

	~PenUp()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		Logo.State_Pen = false;
	}
};

class PenDown : public Command
{
public:
	char *name;

	PenDown()
	{
		name = new char[3];
		strcpy(name,"PD");
	}

	~PenDown()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		Logo.State_Pen = true;
	}
};

class PenErase : public Command
{
public:
	char *name;

	PenErase()
	{
		name = new char[3];
		strcpy(name,"PE");
	}

	~PenErase()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		Logo.State_Erase = true;
	}
};

class PenPaint : public Command
{
public:
	char *name;

	PenPaint()
	{
		name = new char[4];
		strcpy(name,"PPT");
	}

	~PenPaint()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		Logo.State_Erase = false;
	}
};

class SetPenColor : public Command
{
public:
	char *name;

	SetPenColor()
	{
		name = new char[6];
		strcpy(name,"SETPC");
	}

	~SetPenColor()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		Logo.r_pen = tab[0];
		Logo.g_pen = tab[1];
		Logo.b_pen = tab[2];
	}
};

class SetBackGroundColor : public Command
{
public:
	char *name;

	SetBackGroundColor()
	{
		name = new char[6];
		strcpy(name,"SETSC");
	}

	~SetBackGroundColor()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		for(int i = 0 ; i<Logo.paint->bmp.bitmapCoreHeader.bmpWidth ; ++i)
		{
			for(int j = 1; j<=Logo.paint->bmp.bitmapCoreHeader.bmpHeight; ++j)
			{
				if(Logo.paint->getColor(i,j,Logo.r_back,Logo.g_back,Logo.b_back) == false)
				{
					Logo.paint->bmp.setPixel(i,j,tab[0],tab[1],tab[2]);
				}
			}
		}
		Logo.r_back = tab[0];
		Logo.g_back = tab[1];
		Logo.b_back = tab[2];
	}
};

class Print_Pen : public Command
{
public:
	char *name;

	Print_Pen()
	{
		name = new char[10];
		strcpy(name,"PRINT_PEN");
	}

	~Print_Pen()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		std::cout<<"Pisak opuszczony "<<Logo.State_Pen<<" ,gumka "<<Logo.State_Erase<<std::endl;
	}
};
/*
class LoopX : public Command
{
public:
	char *name;

	LoopX()
	{
		name = new char[4];
		strcpy(name,"LPX");
	}

	~LoopX()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		if(Logo.Loop_X) Logo.Loop_X = false;
		else Logo.Loop_X = true;
	}
};

class LoopY : public Command
{
public:
	char *name;

	LoopY()
	{
		name = new char[4];
		strcpy(name,"LPY");
	}

	~LoopY()
	{
		delete [] name;
	}
	
	virtual bool Check(const char *str)
	{
		if(strcmp(str,name) == 0) return true;
		else return false;
	}
	virtual void Work(Interpreter &Logo,float tab[3])
	{
		if(Logo.Loop_Y) Logo.Loop_Y = false;
		else Logo.Loop_Y = true;
	}
};
*/
class Builder
{
private:
	Forward fd;
	Back bk;
	Left lt;
	Right rt;
	Clearscreen cl;
	PenUp pu;
	PenDown pd;
	PenErase pe;
	PenPaint pp;
	Print_Pen ptp;
	SetPenColor spc;
	SetBackGroundColor sbgc;
//	LoopX lpx;
//	LoopY lpy;

	Rysownik *rys;

public:
	Builder(Rysownik &paint)
	{
		rys = &paint;
	}
	~Builder(){}
	
	std::vector<Command *> Build()
	{
		std::vector<Command *> lista;
		lista.push_back(&fd);
		lista.push_back(&bk);
		lista.push_back(&lt);
		lista.push_back(&rt);
		lista.push_back(&cl);
		lista.push_back(&pu);
		lista.push_back(&pd);
		lista.push_back(&pe);
		lista.push_back(&pp);
		lista.push_back(&ptp);
		lista.push_back(&spc);
		lista.push_back(&sbgc);
	//	lista.push_back(&lpx);
	//	lista.push_back(&lpy);

		
		return lista;
	}
};

