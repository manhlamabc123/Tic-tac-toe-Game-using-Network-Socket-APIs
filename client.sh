#!/bin/bash

cd client_side
make clean
make
clear
if ([ -z "$1"] & [ -z "$2"]);
then
    echo "[+]IP Address: 127.0.0.1"
    echo "[+]Port: 8888"
    ./client 127.0.0.1 8888
else
    echo "[+]IP Address: $1"
    echo "[+]Port: $2"
    ./client $1 $2
fi