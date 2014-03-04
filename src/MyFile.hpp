/*
 * MyFile.hpp
 *
 *  Created on: Mar 4, 2014
 *      Author: vert
 */

#ifndef MYFILE_HPP_
#define MYFILE_HPP_

class MyFile {
private:
//deny copy
	MyFile(){};
	MyFile(MyFile&){};
public:
	MyFile(MyString path);
	virtual ~MyFile();

	bool isDirectory(void);
};

#endif /* MYFILE_HPP_ */
