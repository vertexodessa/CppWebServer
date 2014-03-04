/*
 * MyString.hpp
 *
 *  Created on: Mar 4, 2014
 *      Author: vert
 */

#ifndef MYSTRING_H_
#define MYSTRING_H_

using namespace std;

class MyString: public string{
public:
	MyString():string(){};
	virtual ~MyString();

	bool endsWith(const string s){return find_last_of(s) == this->length() - s.length();};
	bool startsWith(const string s){return find(s) == 0;};

	// trim from start
	static inline std::string &ltrim() {
	        this->erase(this->begin(), find_if(this->begin(), this->end(), not1(ptr_fun<int, int>(isspace))));
	        return *this;
	}

	// trim from end
	static inline std::string &rtrim() {
	        this->erase(find_if(this->rbegin(), this->rend(), not1(ptr_fun<int, int>(isspace))).base(), this->end());
	        return *this;
	}

	// trim from both ends
	static inline std::string &trim() {
			rtrim();
	        ltrim();
	        return *this;
	}

};

#endif /* MYSTRING_H_ */
