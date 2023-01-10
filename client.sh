#!/bin/bash

if [ ! -f "server_side/server" ]; then
    echo "[-]Server is not ready"
elif [ -f "client_side/client" ]; then
    cd client_side
    clear
    if [ -z "$1" ]
    then
        echo "[+]Port: 8888"
        ./client 18.136.148.247 8888
    else
        echo "[+]Port: $1"
        ./client 18.136.148.247 $1
    fi
else
    cd client_side
    make clean
    make
    clear
    if [ -z "$1" ]
    then
        echo "[+]Port: 8888"
        ./client 18.136.148.247 8888
    else
        echo "[+]Port: $1"
        ./client 18.136.148.247 $1
    fi
fi