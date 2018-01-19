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
#include <cstdlib>
#include <cstdio>
#include <cstring>


#include "RecordIso2709.h"
#include "strutils.h"


using namespace std;

extern void error(int , const char*);


RecordIso2709::RecordIso2709()
{
   fldterm = FT;   // initialize field terminator //
   recterm = RT;   // initialize record terminator //
   status   = OK;
}

RecordIso2709::RecordIso2709( std::istream &input )
{
   fldterm = FT;   // initialize field terminator //
   recterm = RT;   // initialize record terminator //
   status  = OK;
   inps    = &input;
}


void RecordIso2709::setInputStream( std::istream &input )
{
    inps = &input;
}


int RecordIso2709::read()
{
   Field *fp;
   int j,rl;
   char ch;
   char *bp;
   long len, data_offs;
   int  direntry_size;
   int  recsz;

   clear();
   
   // read label //
   if ((*inps).eof())
      return 0;

   // skip initial white space //
   ch = (*inps).get();
   while (isspace(ch))
   {
      ch = (*inps).get();
   }
   (*inps).unget();

   // get label //
   (*inps).get( buf, LABELSIZE + 1, RT );

   bp = buf;
   for (int j = 0 ; j < LABELSIZE ; ++j)
      label[j] = *bp++;

#ifndef FORMAT_PATCH
   if (strutils::hasIllegalCharacters(label))
      status = BAD_LABEL;

   len = strutils::strntolong(label,5);   // record length [0-4] //
   if (len < LABELSIZE)
      return 0;

   rl  = len - LABELSIZE;      // length of remaining record data to read //

   // read rest of record //
   if ((*inps).eof())
      return 0;

   (*inps).get( bp, rl, 0 );
   
   if (buf[len-0] != RT)   // check record termination //
   {
      cerr <<  "invalid RT: " << buf << '\n';
      return 0;
   }
#endif


#ifdef FORMAT_PATCH
   if (strutils::hasIllegalCharacters(label))
      status = BAD_LABEL;

   len = strutils::strntolong(label,5);   // record length [0-4] //
   if (len < LABELSIZE)
      return 0;

   if ((*inps).eof())
   {
      cerr <<  "unexpected end of file: \n";
      return 0;
   }

   while ((!(*inps).eof()) && ((ch = (*inps).get()) != RT) )
   {
      *bp++  = ch;
   }
   if (ch == RT)
      *bp++ = RT;
   *bp = '\0';

   recsz = bp - buf ;
   
   if (len != recsz)
   {
      cerr <<  "invalid record length: " << len << " : " << recsz << '\n';
      //cerr <<  "invalid record length: " << buf << '\n';
      //status |= INVALID_RECORDLENGTH;
      //return 1;
   }
#endif

   data_offs  = strutils::strntolong(label+12,5);   // data offset  [12-17]
   Dimpl_Flen = CTOI(*(label+20));      // dir. entry length indication [20] //
   Dimpl_Foff = CTOI(*(label+21));      // dir. entry offset indication [21] //

   direntry_size = 3 + Dimpl_Flen + Dimpl_Foff;   // dir. entry size //
   num_entries = (data_offs - LABELSIZE -1 ) / direntry_size; // number of dir entries //

   bp = buf + LABELSIZE ;

   // parse dir entries //
   for ( j = 0 ; j < num_entries ; ++j )
   {
      Field *fld = new Field();
      dir.add(fld);
      fld->setTag(bp,3);
      bp += 3;
      fld->setLength( strutils::strntolong(bp, Dimpl_Flen) - 1 );
      bp += Dimpl_Flen;
      fld->setOffset( strutils::strntolong(bp, Dimpl_Foff) );
      bp += Dimpl_Foff;
   }

   // end of directory //
   if (*bp++ != FT)
      error(1 , "ERROR: on reading first field separator");

   // read data for each entry //
   fp = dir.getFirst();
   while (fp)
   {
      fp->setRawData( bp, fp->getLength() );
      bp += fp->getLength();
      fp = fp->getNext();
      if (*bp != FT)
      {
         char errmsg[200];
         sprintf(errmsg,"ERROR: label = %s, error reading field separator: %c (%x)\n",
               label, *bp, *bp);
         cerr << errmsg;
         status |= ILLEGAL_FIELDSEP;
         return 1;
      }
      ++bp;
   }

   if (*bp++ != RT)
      error(1 , "ERROR: on reading record separator\n");

   return  1;
}


void   RecordIso2709::clear( void )
{
    status   = OK;
    buf[0]   = '\0';
    buf[1]   = '\0';
    dir.clear();
}

void RecordIso2709::init( void )
{
}


void   RecordIso2709::print( std::ostream &outs )
{
   Field *fp;

   // print out record label //
   outs << "label: " << label << '\n';

   // print out each field //
   fp = dir.getFirst();
   while (fp)
   {
      fp->print(outs);
      fp = fp->getNext();
   }
   outs << '\n'; // output empty line after record //
}


void   RecordIso2709::printXML( std::ostream &outs, int indent )
{
   Field *fp;

   // print out record label //
   outs << "<rec>";
   if (indent > 0) outs << '\n';
   outs << "<lab>" << label << "</lab>";
   if (indent > 0) outs << '\n';

   // print out each field //
   fp = dir.getFirst();
   while (fp)
   {
      fp->printXML(outs,indent);
      fp = fp->getNext();
   }
   outs << "</rec>\n";
}



void   RecordIso2709::old_write_iso( std::ostream &outs )
{
   long   recsz , dataoffs;
   char   *formISO;
   long   bufpos;
   int   entry_count, j,i, direntry_size, formwidth = 80;
   Field *fp;

   direntry_size   = 3 + Dimpl_Flen + Dimpl_Foff;

   recsz = 0;
   fp   = dir.getFirst();
   entry_count = 0;

   while (fp)
   {
      ++entry_count;
      fp->setOffset(recsz);
      recsz += (fp->getLength() + 1);
      fp = fp->getNext();
   }

   dataoffs = LABELSIZE + 1 + entry_count * direntry_size;
   recsz += dataoffs + 1;
   formISO = (char*) calloc(1, sizeof(char) * recsz + 2);
   strncpy(formISO , label , LABELSIZE );

   strutils::longtostrn( formISO , recsz , 5);
   strutils::longtostrn( formISO + 12 , dataoffs , 5);

   //formISO[10] = '0';
   //formISO[11] = '0';
   formISO[20] = (char)ITOC(Dimpl_Flen);
   formISO[21] = (char)ITOC(Dimpl_Foff);

   bufpos = LABELSIZE;
   fp = dir.getFirst();
   while (fp)
   {
      strncpy(formISO + bufpos , fp->getTag() , 3);
      bufpos += 3;
      strutils::longtostrn( formISO + bufpos , fp->getLength()+1, Dimpl_Flen );
      bufpos += Dimpl_Flen;
      strutils::longtostrn( formISO + bufpos , fp->getOffset() , Dimpl_Foff );
      bufpos += Dimpl_Foff;
      fp = fp->getNext();
   }

   formISO[bufpos++] = FT;   // terminate field //
   /*
   fp = dir.getFirst();
   while (fp)
   {
      strncpy(formISO + bufpos , fp->getData() , fp->getLength() );
      bufpos += fp->getLength();
      formISO[bufpos++] = FT;   // terminate field //
      fp = fp->getNext();
   }
   */
   formISO[bufpos++] =  RT;   // terminate record  //
   formISO[bufpos] =  '\0';   // terminate string  //

   outs << formISO;

   free(formISO);
}


void RecordIso2709::write_iso( std::ostream &outs )
{
   long   recsz , dataoffs;
   char   *formISO;
   long   bufpos;
   long   fldoffs;
   int   entry_count, j,i, direntry_size, formwidth = 80;
   Field *fp;

   direntry_size = 3 + Dimpl_Flen + Dimpl_Foff;
   recsz         = 0;

   entry_count = 0;
   fp   = dir.getFirst();
   while (fp)
   {
      ++entry_count;
      recsz += fp->getLength() + 1; // + FT //
      fp = fp->getNext();
   }

   dataoffs = LABELSIZE + 1 + entry_count * direntry_size;
   //std::cerr << "recsz: " << recsz << '\n';
   //std::cerr << "dataoffs: " << dataoffs << '\n';

   recsz += dataoffs + 1;
   formISO = new char[sizeof(char)*recsz + 2];
   strncpy(formISO , label , LABELSIZE );

   strutils::longtostrn( formISO , recsz , 5);
   strutils::longtostrn( formISO + 12 , dataoffs , 5);

   //formISO[10] = '0';
   //formISO[11] = '0';
   formISO[20] = (char)ITOC(Dimpl_Flen);
   formISO[21] = (char)ITOC(Dimpl_Foff);

   bufpos = LABELSIZE;

   fldoffs = 0;
   fp = dir.getFirst();
   char   *dp = formISO + dataoffs + fldoffs;
   while (fp)
   {
      int len = fp->getLength() + 1; // fielddata + FT //

  //std::cerr << "tag: " << fp->getTag() << " data: " << fp->getData() << '\n';
      strncpy(formISO + bufpos , fp->getTag() , 3);
      bufpos += 3;
      strutils::longtostrn( formISO + bufpos , len, Dimpl_Flen );
      bufpos += Dimpl_Flen;
      strutils::longtostrn( formISO + bufpos , fldoffs , Dimpl_Foff );
      bufpos += Dimpl_Foff;

  // std::cerr << "tag=" << fp->getTag() << " data=" << fp->getData() << '\n'; 
  // std::cerr << "fldoffs: " << fldoffs  << " dataoffs: " << dataoffs << '\n';
  // std::cerr << "offs: " << (dp - formISO) << '\n';
      memcpy(dp , fp->getData() , len-1 );
      formISO[dataoffs+fldoffs+len-1] = FT;   // terminate field //

      fldoffs += len; // recalculate next field offset //
      fp = fp->getNext();
      dp = formISO + dataoffs + fldoffs;
   }
   *dp++ = RT;
   *dp   = '\0';

   formISO[bufpos++]    = FT;   // terminate directory //

   outs << formISO;

   delete[] formISO;
}



void   RecordIso2709::deleteControlCharacters()
{
   Field *fp = dir.getFirst();
   while (fp)
   {
     fp->deleteControlCharacters();
     fp = fp->getNext();
   }
}



int   RecordIso2709::getStatus()
{
   return status;
}


int   RecordIso2709::isValid()
{
   if (status != OK)
       return 0;
}

