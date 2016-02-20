#!/bin/bash

move=$(./indy 00111000001000122222001)
if [ "$move" != "A5-C2" ]; then
	echo "falied basic win"
fi

move=$(./indy 00111000001000122222002)
if [ "$move" != "D1-C2" ]; then
	echo "falied basic block"
fi
