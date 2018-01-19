#!/bin/sh

EXTRACTISO=./extractISO2709

##
# parametri:
#
#	-t output in formato ascii leggibile
#	-x output in formato XML (unimarcslim)
#	-i2 indent di due spazi, solo valido in combinazione con flag '-x'
##

options="-x"

INPUT=test/data.mrc
OUTPUT=test/output.xml
SCARTATI=test/scartati.txt

echo input : $INPUT
echo output: $OUTPUT
echo scartati: $SCARTATI

$EXTRACTISO -s $SCARTATI $options $INPUT > $OUTPUT

