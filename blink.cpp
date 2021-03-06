#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <iostream>
#include "ftd2xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <thread>

#pragma comment(lib, "user32.lib")
using namespace std;

FT_HANDLE ftHandle;
FT_STATUS ftStatus;

HANDLE hIn;
HANDLE hOut;
INPUT_RECORD InRec;
DWORD NumRead;

int blinkedTotal = 0;

DWORD WINAPI myThreadFunc(LPVOID lpParameter)
{
	unsigned int &ton = *((unsigned int *)lpParameter);
	int ttotal = 300;

	while (1)
	{
		FT_ClrRts(ftHandle);
		this_thread::sleep_for(std::chrono::milliseconds(ton));
		FT_SetRts(ftHandle);
		this_thread::sleep_for(std::chrono::milliseconds((ttotal - ton)));
		blinkedTotal++;
	}
	return 0;
}

void blinkFreq()
{
	cout << "Wähle ein Zahl zwischen 1-29 für Anzeit der Led in ms. " << endl;
	unsigned int ton = 10;
	DWORD myThreadID;
	HANDLE myHandle = CreateThread(0, 0, myThreadFunc, &ton, 0, &myThreadID);
	int input = -1;
	while (input != 0)
	{
		if ((0 < input) && (input < 30))
		{
			ton = input * 10;
		}
		cout << "Aktuelle Anzeit: ";
		cout << ton << endl;
		cout << "Aktuelle Auszeit: ";
		cout << 300-ton << endl;
		cin >> input;
	}
}

void onOff(int milliseconds)
{
	blinkedTotal++;
	FT_ClrRts(ftHandle);
	Sleep(milliseconds);
	FT_SetRts(ftHandle);
	Sleep(milliseconds);
	return;
}

void ascii()
{

	char letter = 'a';
	while (1)
	{
		printf("Bitte Buchstaben eingeben. Die Lampe blinkt dann so oft, wie die zugehoerige Nummer in der ASCII-Tabelle (Abbruch mit 0):\n");
		cin >> letter;
		if (letter == '0')
			return;
		for (int i = 0; i < letter; i++)
		{
			onOff(150);
		}
		printf("\n");
	}
}

void password()
{
	srand(time(NULL));
	printf("Versuche das 3 stellige Passwort Stelle fuer Stelle zu erraten\n");
	int one = rand() % 10;
	int two = rand() % 10;
	int three = rand() % 10;
	int answer;
	int total = one * 100 + two * 10 + three;

	while (1)
	{
		scanf("%d", &answer);
		if (answer == one)
		{
			printf("Die erste Zahl hast du erraten: %d\n", one);
			break;
		}
		printf("Die Zahl stimmt noch nicht. Die Lampe zeigt die die Betragsdifferenz zum Wert an\n");
		Sleep(1000);
		int diff = abs(one - answer);
		for (int i = 0; i < diff; i++)
		{
			onOff(200);
		}
	}
	printf("Jetzt die zweite Zahl!\n");
	while (1)
	{
		scanf("%d", &answer);
		if (answer == two)
		{
			printf("Die zweite Zahl hast du erraten: %d\n", two);
			break;
		}
		printf("Die Zahl stimmt noch nicht. Die Lampe zeigt die die Betragsdifferenz zum Wert an\n");
		Sleep(1000);
		int diff = abs(two - answer);
		for (int i = 0; i < diff; i++)
		{
			onOff(200);
		}
	}
	printf("Jetzt die dritte Zahl!\n");
	while (1)
	{
		scanf("%d", &answer);
		if (answer == three)
		{
			printf("Die dritte Zahl hast du erraten: %d\n", three);
			break;
		}
		printf("Die Zahl stimmt noch nicht. Die Lampe zeigt die die Betragsdifferenz zum Wert an\n");
		Sleep(1000);
		int diff = abs(three - answer);
		for (int i = 0; i < diff; i++)
		{
			onOff(200);
		}
	}
	printf("Gib jetzt den ganzen Zahlencode ein!\n");
	while (1)
	{
		scanf("%d", &answer);
		if (answer == total)
		{
			printf("Das ist korrekt: %d\n", total);
			break;
		}
		printf("Das war leider falsch. Die Lampe zeigt die die Betragsdifferenz zum Wert an\n");
		Sleep(100);
		int diff = abs(total - answer);
		for (int i = 0; i < diff; i++)
		{
			onOff(200);
		}
	}
	printf("Super, das war richtig! :)");
	for (int yay = 0; yay < 50; yay++)
		onOff(30);
}

void stdinReaction()
{
	printf("Jede Tastenbewegung wird registriert und die Lampe leuchtet auf. Zum Abbrechen 0 eingeben\n");

	hIn = GetStdHandle(STD_INPUT_HANDLE);
	while (1)
	{
		ReadConsoleInput(hIn, &InRec, 1, &NumRead);

		if (InRec.EventType == KEY_EVENT)
		{
			printf("%c ", InRec.Event.KeyEvent.uChar.AsciiChar);
			onOff(2);
		}
		if (InRec.Event.KeyEvent.uChar.AsciiChar == '0')
			break;
	}
}

int main(int argc, char *argv[])
{

	ftStatus = FT_Open(0, &ftHandle);
	if (ftStatus != FT_OK)
	{
		printf("FT_Open failed\n");
		return 1;
	}
	printf("ftStatus : %i\n", ftStatus);
	FT_SetRts(ftHandle);

	int mode = 1;

	while (1)
	{
		printf("Bitte einen Modus waehlen:\n");
		printf("1: ASCII\n2: Passwort knacken\n3: stdin\n4: Blinkfrequenz aendern\n0: Beenden\n");
		scanf("%d", &mode);
		if (mode == 0)
			break;
		fflush(stdin);

		if (mode == 1)
		{
			ascii();
		}
		else if (mode == 2)
		{
			password();
		}
		else if (mode == 3)
		{
			stdinReaction();
		}
		else if (mode == 4)
		{
			blinkFreq();
		}
		else
		{
			printf("Das war keine gueltige Eingabe :c");
		}
	}

	FT_Close(ftHandle);

	printf("Insgesamt hat die Lampe %dx geblinkt :P\n", blinkedTotal);
	printf("Eine beliebige Taste druecken, um das Fenster zu schliessen...");
	scanf("%d");

	return 0;
}
