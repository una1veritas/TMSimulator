//============================================================================
// Name        : tm.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include <stdio.h>
//#include <conio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
//#include <windows.h>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <stack>

#include "TuringMachine.h"

using namespace std;

// メイン関数
int main(int argc, char * argv[]) {

	TuringMachine tm;

	unsigned int i; //,v;			// v:mapのキー
	string fname;
	string inputTape;
	bool readOnlyInput = true;

	unsigned int c;
	for(c = 1; c < argc; c++) {
		if (c == 1 ) {
			if (std::string(argv[c]) == "-rw") {
				readOnlyInput = false;
				continue;
			} if  (std::string(argv[c]) == "-ro") {
				readOnlyInput = true;
				continue;
			}
			// not an option.
		}
		if ( fname == "" ) {
			fname = argv[c];
			continue;
		}
		if ( fname != "" ) {
			inputTape = argv[c];
			break;
		}
	}

	cout << "Version 12.0419" << endl << endl;

	// TMファイルを読み込み、状態遷移表を作成する
	tm.maketable(fname, readOnlyInput);

// 遷移関数の表示
	cout << "---Transition table---" << endl;
	for (i = 0; i < tm.table.size(); i++)
		cout << tm.table[i].current << ' ' << tm.table[i].read[0] << ' '
				<< tm.table[i].read[1] << " -> " << tm.table[i].next << " ("
				<< tm.table[i].write[0] << ", " << tm.table[i].headding[0]
				<< "), (" << tm.table[i].write[1] << ", "
				<< tm.table[i].headding[1] << ") " << endl;
	cout << "---Table end---" << endl;
	cout << "Accepting states: ";
	for (set<string>::iterator ep = tm.acceptingStates.begin();
			ep != tm.acceptingStates.end(); ep++) {
		cout << *ep << ", ";
	}
	cout << endl;
	// シミュレート実行
	cout
	//		<< " Redo      -> 'r'" << endl
	<< " Go next step         -> '[return]'" << endl
//		<< " Undo      -> 'u'" << endl
			<< " Continue until halt  -> 'c'" << endl
//		<< " Traverse  -> 't'" << endl
			<< " Exit                 -> 'e'" << endl << endl;

	string * workingTapes = new string[tm.tapes];
	for(unsigned int i = 1; i < tm.tapes; i++) {
		workingTapes[i] += tm.BLANK;
	}
	tm.simulate(inputTape, workingTapes);

	delete[] workingTapes;
	return 0;
}

