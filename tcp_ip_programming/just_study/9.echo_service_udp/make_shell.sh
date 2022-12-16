#!/bin/bash

str1="make"
str2="clean"

if [ "$1" == "$str1" ];
then
	gcc -o server server.c
	gcc -o client client.c
fi

if [ "$1" == "$str2" ];
then
	rm server client
fi
