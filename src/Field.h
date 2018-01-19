/**************************************************************************
 * 	extractISO2709
 * 	Program for data extraction from files conformant to format ISO-2709.
 * 	Output can be generated in different formats:
 *  	1. simple text format
 *  	2. xml according to UNIMARCXML Schema  
 *  	<http://www.bncf.firenze.sbn.it/progetti/unimarc/slim/documentation/unimarcslim.xsd> 
 *  	this Schema <info:srw/schema/8/unimarcxml-v0.1>  can convert UNIMARC records as specified in the MARC
 *  	documentation including the encoding of the so called embedded fields (4XX fields)
 *
 *
 * 	Copyright (C) 2011  WEBDEV <http://www.webdev.it>, BNCF <http://www.bncf.firenze.sbn.it>
 *
 *    	This program is free software: you can redistribute it and/or modify
 *    	it under the terms of the GNU General Public License as published by
 *    	the Free Software Foundation, either version 3 of the License, or
 *    	(at your option) any later version.
 *
 *    	This program is distributed in the hope that it will be useful,
 *    	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    	GNU General Public License for more details.
 *
 *    	You should have received a copy of the GNU General Public License
 *    	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

#ifndef _FIELD_H_
#define _FIELD_H_

#include <iostream>
#include <vector>

#define SF 31  /* ^_ subfield delimiter; printed $ in MARC specs */


class SubField
{
  public:
	SubField();
	SubField(char*,int);
	SubField(char,char*,int);
	~SubField();
	char getId();
	void setId(char);
	char* getRawData();
	char* getData();
	void  setData(char*,int);
	void  setRawData(char*);
	void  setRawData(char*,int);
	int   getLength();
	void	print(std::ostream& os);
	void	printXML(std::ostream& os, int indent);
	void	deleteControlCharacters();
  private:
	char	id;
	char	*data;
};


class Field
{
  public:
	Field();
	~Field();
	long   getLength();
	void   setLength(long len);
	long   getOffset();
	void   setOffset(long offs);
	char  *getData();
	void   setRawData(char *dp, int len = 0);
	Field *getNext();
	void   setNext(Field * fp);
	void   setTag(char *tp, int len = 3);
	char  *getTag();
	char	getInd1();
	char	getInd2();
	int	isControlField();
	void	print(std::ostream& os);
	void	printXML(std::ostream& os, int indent);
	void	deleteControlCharacters();
  private:
	char	ftag[4];
	long	flength;
	long	foffset;
	char	*fdata;
	int	fieldType;
	Field	*next;
	char	ind1;
	char	ind2;
	int	isValidLength;
	std::vector<SubField*>	subfields;
	void	parseSubFields(char*, int, char);
};

#endif /* _FIELD_H_ */

