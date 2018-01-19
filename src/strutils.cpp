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

#include        <iostream>
#include        <cstring>
#include        <cstdio>
#include        <cstdlib>

#include "strutils.h"


using namespace std;

extern	void 	error(int , const char*);

namespace strutils
{



long	strntolong( char *src , int len )
{
	char ts[21];

	if (len == 0)
	  return atol(src);

	if (len > 20)
		error(1,"parameter format error");

	memcpy( ts , src , len );
	ts[len] = '\0';

	return atol(ts);
}


char * longtostrn( char* offs , long lv , int len )
{
	int j;
	char cnvl[12];
	sprintf(cnvl,"%0*ld",len,lv);
	if (strlen(cnvl) > len)
	{
		char msg[100];
		sprintf(msg,"format error in call to longtostr(%ld, %d)\n",lv,len);
		error(1,msg);
	}
	else
	   for ( j = 0 ; j < len ; ++j )
		offs[j] = cnvl[j]; // doesn't copy the final '\0' //
	     
	return offs;
}



//---------------------------------------------------------------------------------
// deleteControlCharacters(char*)
//
// elimina i caratteri con valore decimale < 32
//---------------------------------------------------------------------------------

char * deleteControlCharacters(char* data)
{
  int k;
  unsigned char sc;
  char * rs = data;

  if (data == NULL)
     return NULL;

  k = 0;
  while (sc = (unsigned char)data[k++])
     if (sc > 31)
        *rs++ = sc;
  *rs = '\0';

  return data;
}


//---------------------------------------------------------------------------------
// hasIllegalCharacters(char*)
//
// esamina buffer per la presenza di caratteri con le seguenti caratteristiche:
//  valore decimale > 127
//  valore decimale < 32 e diverso da Tab
//---------------------------------------------------------------------------------

int hasIllegalCharacters(char* data)
{
  int k = 0;
  unsigned char sc;
  int yes = 0;

  if (data == NULL)
     return yes;

  while (sc = (unsigned char)data[k])
  {
     if  (sc > 127)
     {
         if  (sc < 0xC0)
         {
            yes = 1;
            std::cerr << "Bad char: " << std::hex << (short) sc <<  '\n';
         }
         else
            ++k;
     }
     else
     if ((sc < 32) && (sc != 9))
     {
        yes = 1;
        std::cerr << "bad char: " << std::hex << sc <<  '\n';
     }
     ++k;
  }
  return yes;
}


}//namespace//


