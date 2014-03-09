/*
 * MyFile.cpp
 *
 *  Created on: Mar 4, 2014
 *      Author: vert
 */

#include "../inc/MyFile.hpp"

MyFile::MyFile()
{
	// TODO Auto-generated constructor stub

}

MyFile::MyFile ( MyString path )
{
	this->path = path;
}

MyFile::~MyFile()
{
}

bool MyFile::isDirectory ( void )
{
	struct stat sb;

	return ( stat ( path.c_str(), &sb ) == 0 && S_ISDIR ( sb.st_mode ) );
}
