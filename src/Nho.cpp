#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <iostream>
#include "PEImage.h"
#include "ProfileManager.h"

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	ProfileManager profileMng;
	NhoProcessor processor;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN);
	cout << "******************** NhoVirtualizer 1.0 ********************\n";
	char input[MAX_PATH + 1];
	memset(input, 0, MAX_PATH + 1);

	
	cout << "Open profile (leave empty to create a new one): ";
	cin.get(input, MAX_PATH);
	cout << input;

	if (strlen(input) != 0) {
		profileMng.Load(input);
		cout << "Input file: " << profileMng.m_inFile << '\n';
		cout << "Output file: " << profileMng.m_outFile << '\n';
	}
	else {
		cout << "Enter Input File Name: ";
		cin.get(input, MAX_PATH);
		memcpy(profileMng.m_inFile, input, MAX_PATH);

		cout << "Enter Output File Name: ";
		cin.get(input, MAX_PATH);
		memcpy(profileMng.m_outFile, input, MAX_PATH);

		cout << "Save Profile To: ";
		cin.get(input, MAX_PATH);
		profileMng.Save(input);
	}

	while (true) {
		cout << "Start processing? (y/n)";
		int answer = _getche();

		if (answer == 'n')
			return 0;
		else if (answer == 'y')
			break;
	}
	
	profileMng.Execute(&processor);
	
	_getch();
	return 0;
}

