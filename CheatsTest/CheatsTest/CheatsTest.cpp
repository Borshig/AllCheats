// CheatsTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>

int main()
{
	int value = 0;
	int prevValue = 0;
	while (value != 1111)
	{
		std::cout << "Current value: " << value << std::endl;
		std::cout << "Enter new value: ";
		std::cin >> value;				
		std::cout << std::endl;
		prevValue = value;
		Sleep(20);
	}
	return 0;
}
