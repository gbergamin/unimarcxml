############################################################################
 #   	extractISO2709
 # 	Program for data extraction from files conformant to format ISO-2709.
 # 	Output can be generated in different formats:
 #  	1. simple text format
 #  	2. xml according to UNIMARCXML Schema  
 #  	<http://www.bncf.firenze.sbn.it/progetti/unimarc/slim/documentation/unimarcslim.xsd> 
 #  	this Schema <info:srw/schema/8/unimarcxml-v0.1>  can convert UNIMARC records as specified in the MARC
 #  	documentation including the encoding of the so called embedded fields (4XX fields)
 #
 #
 # 	Copyright (C) 2011  WEBDEV <http://www.webdev.it>, BNCF <http://www.bncf.firenze.sbn.it>
 #
 #    	This program is free software: you can redistribute it and/or modify
 #    	it under the terms of the GNU General Public License as published by
 #    	the Free Software Foundation, either version 3 of the License, or
 #    	(at your option) any later version.
 #
 #    	This program is distributed in the hope that it will be useful,
 #    	but WITHOUT ANY WARRANTY; without even the implied warranty of
 #    	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #    	GNU General Public License for more details.
 #
 #    	You should have received a copy of the GNU General Public License
 #    	along with this program.  If not, see <http://www.gnu.org/licenses/>.
############################################################################

TARGET	= extractISO2709

SRCDIR	= src
OBJDIR	= obj

OBJS	= ${OBJDIR}/${TARGET}.o ${OBJDIR}/RecordIso2709.o \
	  ${OBJDIR}/SubField.o ${OBJDIR}/Field.o ${OBJDIR}/FieldList.o	\
	  ${OBJDIR}/strutils.o


DEFS	= -DFORMAT_PATCH
# DEBUG	=  -ggdb

CFLAGS	= ${INCL} -I${SRCDIR} ${DEBUG} ${DEFS}

## uncomment to link on Mac OS X v10.6
# LIBS	= -lcrt1.10.6.o

## static linked
LDFLAGS	= --static ${LIBS} 

CC	= gcc
CPP	= g++
RM	= rm -f

# -------------------------------------------------------------

default:	${TARGET} 

${TARGET}: ${OBJDIR} ${OBJS}
	$(CPP) -o $@ ${OBJS} ${LDFLAGS}

clean:
	$(RM) ${OBJS} ${TARGET}

${OBJDIR}:
	mkdir -p $@

# ----------------------------------------- rules ---------------------------

${OBJDIR}/%.o:	${SRCDIR}/%.cpp
	$(CPP) -o $@ ${CFLAGS} -c $<

${OBJDIR}/%.o:	${SRCDIR}/%.c
	$(CC) -o $@ ${CFLAGS} -c $<


# ----------------------------------- dependencies ---------------------------

${OBJDIR}/RecordIso2709.o:	${SRCDIR}/RecordIso2709.h \
				${OBJDIR}/FieldList.o ${OBJDIR}/Field.o \
				${OBJDIR}/SubField.o
${OBJDIR}/FieldList.o:	${SRCDIR}/FieldList.h ${SRCDIR}/Field.h
${OBJDIR}/Field.o:	${SRCDIR}/Field.h ${SRCDIR}/strutils.h
${OBJDIR}/SubField.o:	${SRCDIR}/Field.h ${SRCDIR}/strutils.h


