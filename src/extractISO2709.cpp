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

#include      <iostream>
#include      <fstream>
#include      <cstdlib>

#include      "RecordIso2709.h"


#define  PROGRAMNAME "extractISO2709"
#define  VERSION     "version 1.0"
#define  COPYRIGHT "Copyright (C) 2011  WEBDEV http://www.webdev.it, BNCF http://www.bncf.firenze.sbn.it"
#define  SCARTATI "scartati.out"


using namespace std;


long	reccount = 0L;
const char* scartout = SCARTATI;

void printVersion()
{
    std::cout << PROGRAMNAME << " " << VERSION << "\n"
              << COPYRIGHT << "\n"
              << "This program comes with ABSOLUTELY NO WARRANTY.\n"
              << "This is free software, and you are welcome to redistribute it\n"
              << "under the GNU General Public License (see <http://www.gnu.org/licenses/>).\n";
}


void help()
{
    std::cout << "\n";
    printVersion();
    std::cout << "\n";
    std::cout << "usage:   extractISO2709 [-h] [-V] [-t] [-k] [-x] [-i indent] [input-file] [output-file]\n\n"
              << "\t-h : print this help message\n"
              << "\t-V : print version\n"
              << "\t-t : output as text\n"
              << "\t-k : do not output control characters [0x00 - 0x1F]\n"
              << "\t-x : output as XML (unimarcslim)\n"
              << "\t-i : indent XML output by 'indent' white spaces\n\n"
              << "\tif output-file is not specified, output will be written to standard out\n"
              << "\tif input-file is not specified, output will be read from standard input\n\n";
}



void error(int errt , const char* errmsg)
{
   switch(errt)
   {
      case 1: fprintf(stderr, "%s\n[%d] record number: %ld\n",errmsg, errt, reccount);
              break;

      case 2: fprintf(stderr, "%s\n[%d]\n",errmsg, errt);
              break;
   }
}


void fatal( int errt , const char* errmsg )
{
   switch( errt )
   {
      case 1: fprintf(stderr, "%s\n[%d] record number: %ld\n",errmsg, errt, reccount);
              break;

      case 2: fprintf(stderr, "%s\n[%d]\n",errmsg, errt);
              break;
   }
   exit(-1);
}


void printXmlHeader( std::ostream *fout )
{
  *fout << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
  << "<collection xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
  << "xsi:schemaLocation=\"http://www.bncf.firenze.sbn.it/unimarc/slim ./unimarcslim.xsd\""
  << ">\n";
  //<< "xmlns=\"http://www.bncf.firenze.sbn.it/unimarc/slim\">"
}


void printXmlFooter( std::ostream *fout )
{
    *fout << "</collection>\n";
}



//---------------------------------------------------------------------------

int main(int argc, char **argv)
{
      FILE   *fpr,  *fpw;

      int      cnt = 1;
      int      rrr = 1;
      int      opt_print = 0;
      int      delete_controlchar = 0;
      int      opt_xml   = 0;
      int      goodrecs = 0;
      int      badrecs = 0;
      int      indent = 0;
      char     *outputFilename;

      std::ostream *fout = &std::cout;
      std::ifstream finp;


   // process command line options //

   while( (argv[cnt] != (char*)0) && *(argv[cnt]) == '-')
   {
      switch( *(++argv[cnt]) )
      {
        case 'i':
                  indent = atoi(++argv[cnt]) - 1;
                  if (indent < 0) indent = 0;
                  break;
        case 'k':
                  // delete control characters from data (0x00 - 0x31)
                  delete_controlchar = 1;
        case 's':
                  scartout = argv[++cnt];
                  break;
        case 't':
                  // print out record in user friendly text format //
                  opt_print = 1;
                  break;
        case 'x':
                  // print out record in XML format //
                  opt_xml = 1;
                  break;
        case 'V':
                  printVersion();
                  exit(0);
        default:  
                  // invalid options
                  help();
                  exit(2);
      }
      ++cnt;
   }



   // open input //
   if (argv[cnt] != NULL)
   {
      finp.open ( argv[cnt] );

      if (! finp.is_open() ) 
      {
         std::cerr << "\n\nERROR: opening input-file  " << argv[cnt] << '\n';
         exit(1);
      }
   }
   ++cnt;

   // open output //
   std::ofstream outfs;
   if ( cnt < argc )
   {
     outputFilename = argv[cnt];
     outfs.open(outputFilename,std::ios::binary);
     if (outfs.is_open())
        fout = &outfs;
     else
     {
        std::cerr << "\n\nERROR: opening output-file  " << outputFilename << '\n';
        exit(1);
     }
   }

   // open log scartati //
   std::ofstream scart(scartout);
   if (! scart) 
   {
      std::cerr << "\n\nERROR: opening output-file  " << scartout << '\n';
      exit(1);
   }


   // loop over input file //
   RecordIso2709 recordiso;
   if (opt_xml)
      printXmlHeader(fout);

   if (finp.is_open())
      recordiso.setInputStream(finp);
   else
      recordiso.setInputStream(std::cin);

   while (recordiso.read())
   {
     int ok = 1;

      if (recordiso.getStatus() != RecordIso2709::OK) 
      {
         ok = 0;
         std::cerr << "status: " <<  recordiso.getStatus() << '\n';
      }

      if (ok)
      {
          if (delete_controlchar)
          {
             // delete chars 0x00 - 0x31 from data //
             recordiso.deleteControlCharacters();
          }

          if (opt_print)
             recordiso.print(*fout);
          else
          if (opt_xml)
               recordiso.printXML(*fout,indent);
          else
             recordiso.write_iso(*fout);
          ++goodrecs;
      }
      else
      {
         ++badrecs;
         recordiso.print(scart); // records scartati in user readable format //
         scart << '\n';
      }
      ++reccount;
   }

   if (opt_xml)
      printXmlFooter(fout);

   std::cerr << "total records: " << reccount
		<< "  good: " << dec <<  goodrecs
		<< "   bad: " << badrecs << '\n';
   return(0);
   
}//main//


