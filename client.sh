#!/bin/bash

if [ -f "client_side/client" ]; then
    cd client_side
    clear
    if ([ -z "$1"] & [ -z "$2"]);
    then
        echo "[-]IP Address and Port are required"
    else
        echo "[+]IP Address: $1"
        echo "[+]Port: $2"
        ./client $1 $2
    fi
else
    cd client_side
    make clean
    make
    clear
    if ([ -z "$1"] & [ -z "$2"]);
    then
        echo "[-]IP Address and Port are required"
    else
        echo "[+]IP Address: $1"
        echo "[+]Port: $2"
        ./client $1 $2
    fi
fi