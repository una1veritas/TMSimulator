/*
 * TuringMachine.cpp
 *
 *  Created on: 2015/05/18
 *      Author: sin
 */

#include <unistd.h>
#include <cstdlib>
#include <ctime>

#include "TuringMachine.h"

// 状態遷移表のチェックと行数の確認
void TuringMachine::program(std::istream & file, const bool inputIsReadOnly) {

	std::istringstream strin;
	std::string buff;
	//int v;
	//int k = 0;

	// inspecting the number of tapes
	std::string dummy;
//	char tapealphabet;
	unsigned int c = 0;
	std::set<std::string> states;

	// inspect a line to determin the number of tapes.

	while (!file.eof()) {
		getline(file, buff, '\n');
		strin.str(buff);
		strin.clear();
		strin >> dummy;
		if (dummy[0] == '#') {
			continue;
		} else {
			for (c = 1; !strin.eof(); c++) {
				strin >> dummy;
			}
			break;
		}
	}
	std::cerr << "columns " << c << std::endl;
	if ( inputIsReadOnly ) {
		noOfTapes = (c - 4) / 3 + 1;
	} else {
		noOfTapes = (c - 2) / 3;
	}
	std::cerr << noOfTapes << " tapes." << std::endl;

	// Now re-read to define the transition table.

	file.clear();
	file.seekg(0, std::ios::beg);
	//v = 0;
	bool skipremaining = false;
	while (!file.eof()) {
		if ( skipremaining ) break;
		getline(file, buff, '\n');
		strin.str(buff);
		strin.clear();

		if (std::string(buff).empty())
			continue;
		if (buff[0] == '#')
			continue;

		table.push_back(Tuple(noOfTapes));
		strin >> table.back().current;
		for (c = 0; c < noOfTapes; c++)
			strin >> table.back().read[c];
		strin >> table.back().next;
		if (table.back().next[0] == '!') {
			table.back().next = (std::string) table.back().next.substr(1, table.back().next.size());
			acceptingStates.insert(table.back().next);
		}
		for (c = 0; c < noOfTapes; c++) {
			if ( c == 0 && inputIsReadOnly ) {
				table.back().write[c] = table.back().read[c];
			} else {
				strin >> table.back().write[c];
			}
			strin >> table.back().headding[c];
		}
		//cerr << table[table.size()].read[0] << ", " << table[table.size()].read[1] << endl;
		if (file.eof())
			break;
#ifdef FALSE
#error ???
		if (states.count(table.back().current) == 0)
			std::cerr << "Error!!" << std::endl << std::flush;
#endif
		// テープ記号がアルファベットもしくは数字かをチェック。
		for (c = 0; c < noOfTapes; c++) {
			if (!(isgraph(table.back().read[c])
					&& isgraph(table.back().write[c]))) {
				std::cout << "table-" << table.size() << ":Improper format." << std::endl;
				skipremaining = true;
				break; //exit(1);
			}
			if (!(table.back().headding[c] == 'R'
					|| table.back().headding[c] == 'L'
					|| table.back().headding[c] == 'N')) {
				std::cout << "table-" << table.size()
						<< ":Improper head motion specification." << std::endl;
				exit(1);
			}
		}
		// 遷移がR,L,Nのいずれかになっているかをチェック。
//		table.size()++;
	}

	return;
}

void TuringMachine::initialize(const std::string inputTape) {

	tapes = new Tape[noOfTapes];
	tapes[0].set(inputTape);
	if ( inputTape.length() == 0 )
		tapes[0].content += SPECIAL_BLANK;
	//cerr << head[0] << "," << input.begin()<< endl;
	for(unsigned int i = 1; i < noOfTapes; i++) {
		tapes[i].content += SPECIAL_BLANK;
		tapes[i].headpos = 0;
	}
	state = table[0].current; //the initial state

	std::srand(std::time(0)); // seed for nondeterministic transitions
}

// tapeを読み状態遷移を実行する関数
void TuringMachine::simulate(void) {
	// adrs:テープのヘッダの位置、s:現状態、step:ステップ数
	unsigned int i;
	int searchOffset, /* adrs[2],*//* s=0, */step = 0; //,undo;
	char c = 'n';
	std::string acc;
//	time_t t1,t2;
	std::map<std::string, int>::iterator sitr;
	std::map<char, int>::iterator hitr;

	int headd;

	// 初期状態の印字
	std::cout << std::endl << "Step: " << step << " ";
	print();

	// 状態遷移を行う
	while (true) {
		searchOffset = rand() % table.size();
		for (i = 0; i < table.size(); i++) {
			Tuple & currentTuple = table[(searchOffset + i) % table.size()];
			if (currentTuple.current == state) {
				unsigned int tn;
				//cerr << currentTuple << endl;
				std::string expectstr(""), headstr("");
				for (tn = 0; tn < noOfTapes; tn++) {
					if ( currentTuple.read[tn] == SPECIAL_DONTCARE ) {
						expectstr += tapes[tn].head();
					} else {
						expectstr += currentTuple.read[tn];
					}
					headstr += tapes[tn].head();
				}
				//cerr << expectstr << " - " << headstr << endl;
				if ( headstr == expectstr)
					break;
			}
		}
		if ( (unsigned ) i == table.size())
			break;  // halt
		// preserve the row-in-table number of the tuple
		i = (searchOffset + i) % table.size();
		if (c == 'r') {
			c = 'n';
		} else if (c != 's' && c != 'c') {
			std::cout << ">>" << std::endl;
			c = (char) getchar();
		}
		switch (c) {
		case 'e':
			exit(0);
		case 'c':
			//Sleep(750);
			usleep(250000);
			break;
		}

		// データの書き換え
		for (unsigned int k = 0; k < noOfTapes; k++) {
			if (table[i].write[k] == SPECIAL_THESAME) {
				//*head[k] = *head[k];
				// implements this by don't touch
			} else {
				tapes[k].head() = table[i].write[k] ;
			}
			switch (table[i].headding[k]) {
			case 'R':
			case 'r':
				headd = +1;
				break;
			case 'L':
			case 'l':
				headd = -1;
				break;
			default: // 'N' or 'n'
				headd = 0;
				break;
			}
			tapes[k].move(headd);
			state = table[i].next;
		}
		step++;
		std::cout << std::endl << "Step: " << step << " ";
		print();
		// undo,redo
	}
	std::cout << std::endl << "The Machine has halted at the state '" << state << "' and " << std::endl;
	if (acceptingStates.find(state) != acceptingStates.end()) {
		std::cout << "accepted ";
	} else {
		std::cout << "rejected ";
	}
	std::cout << "input '" << inputTape() << "'." << std::endl << std::endl;

	if (step == 0)
		exit(0);
}


// 状態遷移を実行する関数
bool TuringMachine::nextConfiguration(void) {
	unsigned int i;
	int searchOffset, /* adrs[2],*//* s=0, */step = 0; //,undo;
	std::string acc;
	std::map<std::string, int>::iterator sitr;
	std::map<char, int>::iterator hitr;

	int headd;

	// 状態遷移を行う

	searchOffset = rand() % table.size();
	for (i = 0; i < table.size(); i++) {
		Tuple & currentTuple = table[(searchOffset + i) % table.size()];
		if (currentTuple.current == state) {
			unsigned int tn;
			//cerr << currentTuple << endl;
			std::string expectstr(""), headstr("");
			for (tn = 0; tn < noOfTapes; tn++) {
				if ( currentTuple.read[tn] == SPECIAL_DONTCARE ) {
					expectstr += tapes[tn].head();
				} else {
					expectstr += currentTuple.read[tn];
				}
				headstr += tapes[tn].head();
			}
			//cerr << expectstr << " - " << headstr << endl;
			if ( headstr == expectstr)
				break;
		}
	}
	if ( (unsigned ) i == table.size())
		return false;  // halt
	// preserve the row-in-table number of the tuple
	i = (searchOffset + i) % table.size();

	// データの書き換え
	for (unsigned int k = 0; k < noOfTapes; k++) {
		if (table[i].write[k] == SPECIAL_THESAME) {
			//*head[k] = *head[k];
			// implements this by don't touch
		} else {
			tapes[k].head() = table[i].write[k] ;
		}
		switch (table[i].headding[k]) {
		case 'R':
		case 'r':
			headd = +1;
			break;
		case 'L':
		case 'l':
			headd = -1;
			break;
		default: // 'N' or 'n'
			headd = 0;
			break;
		}
		tapes[k].move(headd);
		state = table[i].next;
	}
	step++;

	return true;
}

// ステップ毎の状態を表示する関数
void TuringMachine::print(void) { //string state){

	// 状態の表示
	if ( acceptingStates.find(state) != acceptingStates.end() )
		std::cout << "Accepting ";
	std::cout << std::endl;
	std::cout << "State: " << state << std::endl;

	// 入力用テープの表示
	std::cout << "Input tape: " << std::endl;
	std::cout << tapes[0] << std::endl;

	// 作業用テープの表示
	std::cout << "Working tape:";
	for (unsigned int tn = 1; tn < noOfTapes; tn++) {
		std::cout << std::endl << tapes[tn];
	}
	std::cout << std::endl;

}

/*
bool TuringMachine::searchin(std::string s, char in, char wk) {

	for (unsigned int i = 0; i < table.size(); i++)
		if (table[i].current == s && table[i].read[0] == in
				&& table[i].read[1] == wk)
			return true;
	return false;
}
*/

void TuringMachine::show(std::ostream & stream) {
	stream << "---Transition table---" << std::endl;
	for (unsigned int i = 0; i < table.size(); i++)
		stream << table[i].current << ' ' << table[i].read[0] << ' '
				<< table[i].read[1] << " -> " << table[i].next << " ("
				<< table[i].write[0] << ", " << table[i].headding[0]
				<< "), (" << table[i].write[1] << ", "
				<< table[i].headding[1] << ") " << std::endl;
	stream << "---Table end---" << std::endl;
	stream << "Accepting states: ";
	for (std::set<std::string>::iterator ep = acceptingStates.begin();
			ep != acceptingStates.end(); ep++) {
		stream << *ep << ", ";
	}
	stream << std::endl;
}
