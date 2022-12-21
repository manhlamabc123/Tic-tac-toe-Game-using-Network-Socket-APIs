#!/bin/bash

if [ ! -f "server" ]; then
    echo "Server is not ready"
else
    ./client 127.0.0.1 8888
fi