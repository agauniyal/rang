#include "rang.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace rang;

void loadTxt(ifstream& inFile,string& strFile)
{
	if(!inFile.is_open())
		return;
	int fileSize=0;
	inFile.seekg(0,ios::end);
	fileSize= inFile.tellg();
	inFile.seekg(0,ios::beg);
	
	strFile.resize(fileSize);
	inFile.read(&strFile[0],fileSize);
	
}

void str_Change(ostream& os,int Style,int fg_color, int bg_color, int fg_bright, int bg_bright, string& strFile)
{
	switch(Style)
	{
	case 0: 
		break;
	case 1: os<<style::bold;
		break;
	case 2: os<<style::dim;
		break;
	case 3: os<<style::italic;
		break;
	case 4: os << style::underline;
		break;
	case 5: os << style::blink;
		break;
	case 6: os << style::rblink;
		break;
	case 7: os << style::crossed;
		break;
	}
	switch(fg_color)
	{
	case 0:
		break;
	case 1: os<<fg::black;
		break;
	case 2: os<<fg::red;
		break;
	case 3: os<<fg::green;
		break;
	case 4: os << fg::yellow;
		break;
	case 5: os << fg::blue;
		break;
	case 6: os << fg::magenta;
		break;
	case 7: os << fg::cyan;
		break;
	case 8: os << fg::gray;
		break;
	}
	switch (bg_color)
	{
	case 0:
		break;
	case 1: os << bg::black;
		break;
	case 2: os << bg::red;
		break;
	case 3: os << bg::green;
		break;
	case 4: os << bg::yellow;
		break;
	case 5: os << bg::blue;
		break;
	case 6: os << bg::magenta;
		break;
	case 7: os << bg::cyan;
		break;
	case 8: os << bg::gray;
		break;
	}
	switch (fg_bright)
	{
	case 0:
		break;
	case 1: os << fgB::black;
		break;
	case 2: os << fgB::red;
		break;
	case 3: os << fgB::green;
		break;
	case 4: os << fgB::yellow;
		break;
	case 5: os << fgB::blue;
		break;
	case 6: os << fgB::magenta;
		break;
	case 7: os << fgB::cyan;
		break;
	case 8: os << fgB::gray;
		break;
	}
	switch (bg_bright)
	{
	case 1: os << bgB::black;
		break;
	case 2: os << bgB::red;
		break;
	case 3: os << bgB::green;
		break;
	case 4: os << bgB::yellow;
		break;
	case 5: os << bgB::blue;
		break;
	case 6: os << bgB::magenta;
		break;
	case 7: os << bgB::cyan;
		break;
	case 8: os << bgB::gray;
		break;
	}
	os<<strFile;
}

void reset(ostream& os) {
	os << style::reset;
	os << fg::reset;
	os << bg::reset;
}

int main()
{
	string inFileName;
	string strFile;
	int Style,fg_color,bg_color,fg_bright,bg_bright,menu;
	
	cout<<"load file name: ";
	cin>>inFileName;
	ifstream inFile(inFileName+".txt");
	
	while (1) {
		cout << bg::red << "------Select Menu------" << bg::reset << endl;
		cout << "Result = 0		Text style = 1"<< endl; 
		cout << "color =  2		Background color = 3" << endl;
		cout << "Text bright_color = 4	Background bright_color = 5" << endl;
		cin >> menu;

		if (menu == 0)
		{
			loadTxt(inFile, strFile);
			str_Change(cout, Style, fg_color, bg_color, fg_bright, bg_bright, strFile);
			reset(cout);
			inFile.close();
			return 0;
		}
		else if (menu == 1)
		{
			cout << style::italic << "------Text style------" << style::reset << endl
				<< "0 = default	1 = bold	2 = dim"
				<< endl << "3 = italic	4 = underline	5 = blink"
				<< endl << "6 = rblink	7 = reversed	8 = conceal" << endl << "style:";
				cin >> Style;
				if (Style > 8 || Style < 0)
				{
					cout << "Wrong Select! Text style set default." << endl << endl;
					Style = 0;
				}
		}
		else if (menu == 2)
		{
			cout << fg::red << "------Text color------" << fg::reset << endl
				<< "0 = default	1 = black	2 = red"
				<< endl << "3 = green	4 = yellow	5 = blue"
				<< endl << "6 = magenta	7 = cyan	8 = gray" << endl << "color:";
				cin >> fg_color;
				if (fg_color > 8 || fg_color < 0)
				{
					cout << "Wrong Select! Text color set default." << endl << endl;
					fg_color = 0;
				}
		}
		else if (menu == 3)
		{
			cout << bg::blue << "------Background color------" << bg::reset << endl
				<< "0 = default	1 = black	2 = red"
				<< endl << "3 = green	4 = yellow	5 = blue"
				<< endl << "6 = magenta	7 = cyan	8 = gray" << endl << "color:";
				cin >> bg_color;
				if (bg_color > 8 || bg_color < 0)
				{
					cout << "Wrong Select! Background color set default." << endl << endl;
					bg_color = 0;
				}
		}
		else if (menu == 4)
		{
			cout << fgB::magenta << "------Text bright_color------" << fg::reset << endl
				<< "0 = default	1 = black	2 = red"
				<< endl << "3 = green	4 = yellow	5 = blue"
				<< endl << "6 = magenta	7 = cyan	8 = gray" << endl << "color:";
			cin >> fg_bright;
			if (fg_bright > 8 || fg_bright < 0)
			{
				cout << "Wrong Select! Text color set default." << endl << endl;
				fg_bright = 0;
			}
		}
		else if (menu == 5)
		{
			cout << bgB::cyan << "------Background bright_color------" << bg::reset << endl
				<< "0 = default	1 = black	2 = red"
				<< endl << "3 = green	4 = yellow	5 = blue"
				<< endl << "6 = magenta	7 = cyan	8 = gray" << endl << "color:";
			cin >> bg_bright;
			if (bg_bright > 8 || bg_bright < 0)
			{
				cout << "Wrong Select! Background color set default." << endl << endl;
				bg_bright = 0;
			}
		}
		else
		{
			cout << "Wrong Select!" << endl;
		}
	}
}
