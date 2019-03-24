#include"Logo.h"
#include<string>

int main()
{
	int w = 800;
	int h = 800;
	/*
	std::cout<<"Width : ";
	std::cin>>w;

	std::cout<<"Height : ";
	std::cin>>h;
	*/
	Rysownik Paint(w,h);
	Builder B(Paint);
	std::vector<Command *> begin;
	begin = B.Build();
	Interpreter Inter(begin,Paint);

	Inter.Read_File("comand.txt");

	/*
	char str[16] = "PU"; 
	
	for(std::vector<Command *>::iterator it = Inter.lista.begin() ; it != Inter.lista.end(); it++)
	{
		std::cout<<(*it)->Check(str)<<std::endl;
	}
	*/

	std::cout<<"end."<<std::endl;
	return 0;
}