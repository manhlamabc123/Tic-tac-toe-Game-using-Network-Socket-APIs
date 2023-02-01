# Game: Tic-tac-toe for multiplayer

<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about">About</a></li>
    <li><a href="#technologies">Technologies</a></li>
    <li><a href="#how-to-setup-mysql">How to setup MySQL</a></li>
    <li><a href="#how-to-run-client-and-server">How to run Client and Server</a></li>
    <li><a href="#how-to-use-ngrok">How to use Ngrok</a></li>
    <li><a href="#documents">Documents</a></li>
  </ol>
</details>

## About

* I made this project for college's final term assignment
* Subject: Unix Programming

## Technologies

* This project use `poll()` to handle multiple clients
* Front-end & Back-end: `C`
* Database: `mysql`
* Server: `ngrok`

## Prerequisite

* OS: Ubuntu 22.04.1 LTS x86_64
* gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0
* mysql: 8.0.31-0ubuntu0.22.04.1 for Linux on x86_64
* ngrok: 3.1.0

## How to setup MySQL

### To install MySQL on Ubuntu
Check "C & MySQL" and "MySQL" in Documents section for details

### To enable MySQL API in C
```
sudo apt install default-libmysqlclient-dev
```

### Create database
```
create database [database name]
```
* Feel free to change `[database name]`
* My was `socket_project`


### Create `accounts` table
```
CREATE TABLE accounts(  
    id int NOT NULL PRIMARY KEY AUTO_INCREMENT COMMENT 'Primary Key',
    username VARCHAR(255),
    password VARCHAR(255)
) COMMENT '';
```

### Create `games` table
```
CREATE TABLE games(  
    id int NOT NULL PRIMARY KEY AUTO_INCREMENT COMMENT 'Primary Key',
    date VARCHAR(255),
    first_player VARCHAR(255),
    second_player VARCHAR(255),
    board_size INT,
    moves VARCHAR(255)
) COMMENT '';
```

### Changing Host, User, Password
In `server_side/struct.h` change these following
```
#define HOST "localhost"
#define USER "root"
#define PASSWORD "20194616"
#define DATABASE "socket_programming"
```

## How to run Client and Server

### To clean up all `.o` files
```
bash setup.sh
```

### To run server 
```
bash server.sh [port]
```
* You can specify any `port` you want
* Default port is 8888
* You don't have to run this if the server is already running on other machine

### To run client
```
bash client.sh [IP Address] [port]
```
* If you are going to specify `IP Address` or `port`, you need to specify both
* Defaul IP Address and port are: 127.0.0.1 8888
* To connect to server, you need to specify both `IP Address` and `port`

### To run client using `Docker`
* Build image `client_side-client`
```
cd client_side
docker compose build
```
* Run container in bash mode
```
docker container run -it client_side-client bash
```
* Run client
```
./client [IP Address] [port]
```
  * For local:
    * Linux: use `ifconfig`, lock for `docker0`, `[IP Address]` is `docker0`'s inet
    * Windows, MacOS: use `docker.for.win` or `docker.for.mac` for `[IP Address]`
    * `[port]` default to `8888`
  * For Ngrok: check below

## How to use ngrok

Ngrok's website: [link](https://ngrok.com/)

### To install on Ubuntu
```
sudo snap install ngrok
```

### To host ngrok
```
ngrok tcp [port]
```
* `port` must be the same as server's local port
* Example result: `tcp://0.tcp.ap.ngrok.io:10955`
  * Use [this website](https://whatismyipaddress.com/hostname-ip), input `0.tcp.ap.ngrok.io` to get `IP Address`
  * `port` is 10955
  
## Documents
* C & MySQL: [website](https://zetcode.com/db/mysqlc/)
* Bash scripting: [youtube video](https://www.youtube.com/watch?v=SPwyp2NG-bE&t=239s) & Google search
* Book: [UNIX Network Programming(Volume1,3rd)](https://mathcs.clarku.edu/~jbreecher/cs280/UNIX%20Network%20Programming(Volume1,3rd).pdf)
* Ngrok: [TCP Tunnel](https://ngrok.com/docs/secure-tunnels/tunnels/tcp-tunnels)
* MySQL: [Install on Ubuntu](https://www.digitalocean.com/community/tutorials/how-to-install-mysql-on-ubuntu-20-04)
