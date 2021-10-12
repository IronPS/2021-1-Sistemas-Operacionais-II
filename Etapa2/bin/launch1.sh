#!/bin/bash

CWD=$(realpath $(dirname "$0"))

         gnome-terminal --tab --disable-factory --title="Server 0" --working-directory="${CWD}" -- ./ServerApp -p 53656 -i 0 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58000,58001,58002,58003 & disown;
sleep 1; gnome-terminal --tab --disable-factory --title="Server 1" --working-directory="${CWD}" -- ./ServerApp -p 53657 -i 1 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58000,58001,58002,58003 & disown;
sleep 1; gnome-terminal --tab --disable-factory --title="Server 2" --working-directory="${CWD}" -- ./ServerApp -p 53658 -i 2 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58000,58001,58002,58003 & disown;
sleep 1; gnome-terminal --tab --disable-factory --title="Server 3" --working-directory="${CWD}" -- ./ServerApp -p 53659 -i 3 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58000,58001,58002,58003 & disown;
