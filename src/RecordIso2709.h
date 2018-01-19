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

#ifndef _RECORDISO_H_
#define _RECORDISO_H_

#include	<iostream>

#include	"Field.h"
#include	"FieldList.h"
#include	"strutils.h"

#define LABELSIZE 24
#define MAXRECSIZE 100352

#define CTOI(c) isdigit(c) ? (int)((c) - '0') : (int) 0
#define ITOC(c) ((c)<10) ? ((c) + '0') : '0';


enum terminator {
        RT = 29, /* ^] record terminator;  printed \ in MARC specs */
        FT = 30, /* ^^ field terminator;   printed ^ or @ in MARC specs */
        DL = 31  /* ^_ subfield delimiter; printed $ in MARC specs */
};


class RecordIso2709
{
 private:

   char		label[LABELSIZE+1];	// record label, may be invalidated after record modification //
   char		buf[MAXRECSIZE];
   int		Dimpl_Flen;
   int		Dimpl_Foff;
   int		num_entries;
   char		fldterm;
   char		recterm;
   int		status;
   FieldList	dir;
   std::istream *inps;

 public:
   static const int OK			=  0;
   static const int BAD_LABEL		=  1;
   static const int BAD_DATA		=  2;
   static const int ILLEGAL_CHARACTERS	=  4;
   static const int ILLEGAL_FIELDSEP	=  8;
   static const int INVALID_RECORDLENGTH= 16;

   RecordIso2709();
   RecordIso2709( std::istream &inps );
   void init();
   void clear();
   void setInputStream( std::istream &inps );
   //int  read( std::istream &inps );
   int  read();
   int  getFieldCount();
   void print( std::ostream &outs );
   void printXML( std::ostream &outs, int indent );
   void write_iso( std::ostream &outs );
   void	deleteControlCharacters();
   int	getStatus();
   int	isValid();

   void old_write_iso( std::ostream &outs );
};


#endif /* _RECORDISO_H_ */

