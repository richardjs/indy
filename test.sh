#!/bin/bash

move=$(./indy 00111000001000122222001)
if [ "$move" != "A5-C2" ]; then
	echo "falied basic win"
fi

move=$(./indy 10001120001002122200002)
if [ "$move" != "D1-C2" ]; then
	echo "falied depth 1 block"
fi

move=$(./indy 10110001120200000222002)
if [ "$move" != "B5-B2" ]; then
	echo "falied depth 2 block"
fi
