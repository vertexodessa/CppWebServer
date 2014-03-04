/*
 * MyString.hpp
 *
 *  Created on: Mar 4, 2014
 *      Author: vert
 */

#ifndef MYSTRING_H_
#define MYSTRING_H_

#include <string>
#include <algorithm>


using namespace std;

// Yeah, I know, not such a good idea, to extend base STL string class.. :(
// destructor in that class is not virtual so that could lead to some memory leak problems..
// will refactor it later (c) Vert

class MyString: public string{
public:
	MyString():string(){};
	MyString(char* s):string(s){};
	MyString(string s):string(s){};

	virtual ~MyString(){};

	bool endsWith(const string s){return find_last_of(s) == this->length() - s.length();};
	bool startsWith(const string s){return find(s) == 0;};

	// trim from start
	string &ltrim() {
	        this->erase(this->begin(), find_if(this->begin(), this->end(), not1(ptr_fun<int, int>(isspace))));
	        return *this;
	}

	// trim from end
	string &rtrim() {
	        this->erase(find_if(this->rbegin(), this->rend(), not1(ptr_fun<int, int>(isspace))).base(), this->end());
	        return *this;
	}

	// trim from both ends
	string &trim() {
			rtrim();
	        ltrim();
	        return *this;
	}

};

#endif /* MYSTRING_H_ */
