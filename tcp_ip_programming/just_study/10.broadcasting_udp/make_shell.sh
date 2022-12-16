#!/bin/bash

str1="make"
str2="clean"

if [ "$1" == "$str1" ];
then
	gcc -o broadcster_udp broadcster_udp.c
	gcc -o reader_udp reader_udp.c
fi

if [ "$1" == "$str2" ];
then
	rm reader_udp broadcster_udp
fi
