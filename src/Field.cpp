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

#include	<iostream>
#include	<cstdlib>
#include        <cstring>

#include "Field.h"
#include "strutils.h"


Field::Field()
{
   next  = NULL;
   fdata = NULL;
   ind1  = '\0';
   ind2  = '\0';
   ftag[0]= '\0';
   fieldType = 0; // 0: undefined, 1: data-field, 2: control-field
   isValidLength = 0; // length must be calculated //
}


Field::~Field()
{
   if (fdata != NULL)
   {
	delete[] fdata;
	fdata = NULL;
   }
                
   while (! subfields.empty() )
   {
      SubField *sf = subfields.back();
      subfields.pop_back();
      if (sf != NULL)
      {
	      delete sf;
	      sf = NULL;
      }
   }
}


void Field::setTag(char *tp, int len )
{
   memcpy(ftag, tp, 3);
   ftag[3]='\0';
   fieldType = ((ftag[0] == '0') && (ftag[1] == '0')) ? 2 : 1;
}


char* Field::getTag()
{
	return ftag;
}


char * Field::getData()
{
  int sz, offs;
  int len = 0;

  switch(fieldType)
  {
    case 2:	// control field
            return fdata;
	    break;
    case 1:	// data field
	    len = 2;
            sz = subfields.size();
            for (int j = 0 ; j < sz ; ++j)
            {
               SubField *sf = subfields[j];
               len += sf->getLength() + 1;
            }
            fdata = new char[len+1];
        // std::cerr << "tag: " << ftag << " len=" << len << '\n';
	    offs = 0;
	    fdata[offs++] = ind1;
	    fdata[offs++] = ind2;
            for (int j = 0 ; j < sz ; ++j)
            {
	       fdata[offs++] = SF;
               SubField *sf = subfields[j];
	       //fdata[offs++] = sf->getId();
               len = sf->getLength();
	       memcpy(fdata+offs,sf->getRawData(),len);
	       offs += len;
            }
	    fdata[offs] = '\0';
	    flength = offs;
	    isValidLength = 1;
            return fdata;
	    break;
    case 0:	// undefined
	    return NULL;
	    break;
  }
}



void Field::parseSubFields(char *str, int sl, char delim )
{
   char *p1, *p2, *eofs;
   if (str == NULL)
      return;

   eofs = str + sl; // pointer to end of string //
   p1   = p2 = (str[0] == delim) ? str + 1 : str;

   while (p2 < eofs)
   {
      // find next position of delimiter //
      while ( (*p2 != delim) && (p2 < eofs) )
            ++p2;

      int l = p2 - p1;
      SubField *subfld = new SubField(p1,l);
      subfields.push_back(subfld);
      p1 = ++p2;
   }
}


void Field::setRawData(char *dp, int len)
{
  int l = (len) ? len : strlen(dp);
  if (l == 0)
	  return;
    
  flength = l;
  isValidLength = 1; 
  switch(fieldType)
  {
    case 2:	// control field
            fdata = new char[l+1];
            memcpy(fdata, dp, l);
            fdata[l]='\0';
	    break;
    case 1:	// data field
	    ind1 = *dp++;
	    ind2 = *dp++;
	    l -= 2;
            parseSubFields(dp,l,SF);
	    break;
    case 0:	// undefined
            std::cerr << "Error: Tag not specified.\n";
	    return;
	    break;
  }
}


long Field::getLength()
{
  int sz;
  int len = 0;

  if (isValidLength)
	  return flength;

  switch(fieldType)
  {
    case 2:	// control field
	    if (fdata != NULL)
	       len = strlen(fdata);
	    break;
    case 1:	// data field
	    len = 2;
            sz = subfields.size();
            for (int j = 0 ; j < sz ; ++j)
            {
               SubField *sf = subfields[j];
               len += sf->getLength() + 1;
            }
	    break;
    case 0:	// undefined
	    return 0;
	    break;
  }
  return(len + 1);
}


void Field::setLength(long len)
{
	flength = len;
	isValidLength = 1;
}

long Field::getOffset()
{
	return foffset;
}

void Field::setOffset(long offs)
{
	foffset = offs;
}

Field * Field::getNext()
{
	return next;
}

void Field::setNext(Field *fp)
{
	next = fp;
}
int Field::isControlField()
{
 	return (fieldType == 2);
}

char Field::getInd1()
{
	return ind1;
}

char Field::getInd2()
{
	return ind2;
}

void Field::print(std::ostream &os)
{
  int sz;
   switch (fieldType)
   {
    case 2:
	    os << ftag << ": " << fdata << '\n';
	    break;
    case 1:
	    os << ftag << ":[" << ind1 << ind2 << "] ";
            sz = subfields.size();
            for (int j = 0 ; j < sz ; ++j)
            {
               SubField *sf = subfields[j];
	       if (j > 0)
		  os << "\t " ;
               sf->print(os);
            }
            break;
    case 0:
	    break;
   }
}


void	Field::printXML(std::ostream& os, int indent)
{
  int sz;
  int inS1 = 0;
  int inDF = 0;
  char space[] = "                                        "; // 40 spaces //
  int  spc;
  char *cp;

  if (indent < 0)
    spc = 0;
  else
    spc = (indent >= 40) ? 40 : indent;

   switch (fieldType)
   {
    case 2:
	    os.write(space,spc);
	    os << "<cf t=\"" << ftag << "\">";
	    cp = fdata;
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
	    os << "</cf>";
	    if (spc > 0) os << '\n';
	    break;
    case 1:
	    os.write(space,spc);
	    os << "<df t=\"" << ftag << "\" i1=\""
		    << ind1 << "\" i2=\"" << ind2 << "\">";
	    if (spc > 0) os << '\n';
            sz = subfields.size();
            for (int j = 0 ; j < sz ; ++j)
            {
	        // patch per unimarc: gestione link fields (4xx) //
		char * data;
		SubField *sf = subfields[j];
		if (sf->getId() == '1')
		{
		  if (inS1)
		  {
		    if (inDF)
		    {
	              os.write(space,2*spc);
		      os << "</df>";
	              if (spc > 0) os << '\n';
		      inDF = 0;
		    }
	            os.write(space,spc);
		    os << "</s1>";
	            if (spc > 0) os << '\n';
		  }
	          os.write(space,spc);
		  os << "<s1>";
	          if (spc > 0) os << '\n';
		  inS1 = 1;
		  data = sf->getData();
		  if ((data[0] == '0') && (data[1] == '0')) // control field
		  {
	            os.write(space,2*spc);
		    os << "<cf t=\"";
		    os.write(data,3);
		    os << "\">";
	            cp = data+3;
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
		    os << "</cf>";
	            if (spc > 0) os << '\n';
		  }
		  else // data field
		  {
	            os.write(space,2*spc);
		    os << "<df t=\"";
		    os.write(data,3);
		    os << "\" i1=\"" << data[3] << "\" i2=\"" << data[4] << "\">";
	            if (spc > 0) os << '\n';
		    inDF = 1;
		  }
		}
		else // if (sf->getId() == '1')
		   sf->printXML(os,(inDF)?3*spc:2*spc);
            }
	    if (inDF)
	    {
	       os.write(space,2*spc);
 	       os << "</df>";
	       if (spc > 0) os << '\n';
	    }
	    if (inS1)
	    {
	       os.write(space,spc);
	       os << "</s1>";
	       if (spc > 0) os << '\n';
	    }
	    os.write(space,spc);
	    os <<  "</df>";
	    if (spc > 0) os << '\n';
            break;
    case 0:
	    break;
   }
}



void Field::deleteControlCharacters()
{
   int sz;
   char *newdata = NULL;
   switch (fieldType)
   {
    case 2:
	    // TODO: check control field contents //
	    if (fdata != NULL)
	    {
		newdata = strutils::deleteControlCharacters(fdata);
	    }
	    break;
    case 1:
            sz = subfields.size();
            for (int j = 0 ; j < sz ; ++j)
            {
               SubField *sf = subfields[j];
               sf->deleteControlCharacters();
            }
            break;
    case 0:
	    break;
   }
}

