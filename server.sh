#!/bin/bash

while :
do
    cd server_side
    
    make clean

    make

    clear

    if [ -z "$1" ]
    then
        echo "[+]Port: 8888"
        ./server 8888
    else
        echo "[+]Port: $1"
        ./server $1
    fi

    cd ..

    echo "[+]Server closed"
done