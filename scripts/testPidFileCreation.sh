#!/bin/bash
if [ -e "$1" ]
then
	echo "Removing existing pid file ($1)..."
	rm -f $1
fi
./ngpommedgui
if [ ! -e "$1" ]
then
	exit 1
else
	exit 0
fi
