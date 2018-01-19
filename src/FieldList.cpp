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


#include "Field.h"
#include "FieldList.h"


FieldList::FieldList()
{
   first = last = NULL;
}

FieldList::~FieldList()
{
   clear();
}

int FieldList::isEmpty()
{
   return (first == NULL);
}

int FieldList::getCount()
{
   int cnt = 0;
   Field *fp = first;
   while (fp)
   {
      ++cnt;
      fp = fp->getNext();
   }
   return cnt;
}

Field * FieldList::getFirst()
{
   return first;
}

Field * FieldList::getLast()
{
   return last;
}

void FieldList::add(Field *fld)
{
   if (last == NULL)
      first = last = fld;
   else
   {
      last->setNext(fld);
      last = fld;
   }
}

void FieldList::clear()
{
   Field *fp,*fpn;
   fp = first;
   while (fp != NULL)
   {
      fpn = fp->getNext();
      delete fp;
      fp = fpn;
   }
   first = last = NULL;
}

