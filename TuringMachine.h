#ifndef _TURINGMACHINE_H_
#define _TURINGMACHINE_H_

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <stack>


struct Tuple {
	std::string current, next; // state[0]:現状態、state[1]:次状態
//	char alph[4];		// alph[0]:入力用テープの読み込み、alph[1]:作業用テープの読み込み、
			// alph[2]:書き込み、alph[3]:書き込み
	std::string read, write, headding; // hder[0]:入力用テープのヘッドの動き、hder[1]:作業用テープの

	Tuple(int tapes) {
		current = "";
		next = "";
		read = std::string(tapes, ' ');
		write = std::string(tapes, ' ');
		headding = std::string(tapes, ' ');
	}

	// input & output ;
	friend std::ostream & operator <<(std::ostream & stream, const Tuple & obj) {
		stream << obj.current << ", " << obj.read << ", " << obj.next << ", " << obj.write << ", " << obj.headding;
		return stream;
	}

};

struct TuringMachine {
public:
	static const char BLANK = '_';
	static const char SPECIAL_DONTCARE = '*';
	static const char SPECIAL_THESAME = '*';

private:
	std::string::iterator * head;
	std::string * tape;
	std::string state;

public:
	std::vector<Tuple> table;
	unsigned int tapes;
	std::set<std::string> acceptingStates;

	int step;

	TuringMachine() {
		tapes = 1;
		//step = 0;
		//answ = 0;
	}

public:
	void maketable(const std::string &, const bool);
	void simulate(std::string, std::string[]);
	void print(int); //string state);
	bool searchin(std::string state, char oninput, char onwork);

};


#endif
