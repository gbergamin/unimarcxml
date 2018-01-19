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

#include <iostream>
#include <iomanip>
#include <cstring>

#include "Field.h"
#include "strutils.h"


SubField::SubField()
{
   id   = 255;
   data = NULL;
}


SubField::SubField(char ident, char *dp, int len)
{
   id = ident;
   setData(dp,len);
}


SubField::SubField(char *dp, int len)
{
   setRawData(dp,len);
}


SubField::~SubField()
{
   if (data != NULL)
   {
      delete[] data;
      data = NULL;
   }
}


char SubField::getId()
{
   return id;
}


void SubField::setId( char sfid )
{
   id = sfid;
}


char * SubField::getData()
{
   return data + 1;
}


char * SubField::getRawData()
{
   return data;
}



void SubField::setRawData(char *dp)
{
  if (dp == NULL)
     return;
  id = dp[0];

  int len = strlen(dp);
  data = new char[len+1];
  if (len > 0)
     memcpy(data, dp, len);
  data[len]='\0';
}


void SubField::setRawData(char *dp, int len)
{
  if (dp == NULL)
    return;
  if (len < 0)
     return;
  id = dp[0];
  data = new char[len+1];
  if (len > 0)
     memcpy(data, dp, len);
  data[len]='\0';
}


void SubField::setData(char *dp, int len)
{
  char *dtmp;
  if (dp == NULL)
    return;
  if (len < 0)
     return;

  dtmp = data; // save pointer to data //
  data = new char[len+2];   // consider field-id in first position + end of string //
  if (dtmp != NULL)
  {
     data[0] = dtmp[0];
     delete[] dtmp;
  }
  if (len > 0)
     memcpy(data+1, dp, len);
  data[len+1]='\0';
}


int SubField::getLength()
{
   return strlen(data); // id + data //
}


void SubField::print(std::ostream &os)
{
    os << id << ": "  << data+1 << '\n';
}


void SubField::printXML(std::ostream &os, int indent)
{
    char *cp;
    char space[] = "                                        "; // 40 spaces //
    int spc = (indent >= 40) ? 40 : indent;
    if (spc < 0) spc = 0;
    os.write(space,spc);
    os << "<sf c=\"" << id << "\">";
    cp = data+1;
    while (*cp)
    {
       switch(*cp)
       {
          case '<': os << "&lt;"; break;
          case '>': os << "&gt;"; break;
          case '&': os << "&amp;"; break;
          default :  os.put(*cp);
       }
       ++cp;
    }
    os << "</sf>";
    if (spc > 0) os << '\n';
}


void SubField::deleteControlCharacters()
{
   strutils::deleteControlCharacters(data);
}


