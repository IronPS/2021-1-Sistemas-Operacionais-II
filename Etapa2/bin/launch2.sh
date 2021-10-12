#!/bin/bash

CWD=$(realpath $(dirname "$0"))

         gnome-terminal --disable-factory --title="Server 0" --working-directory="${CWD}" -- ./ServerApp -p 53660 -i 0 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58010,58011,58012,58013 & disown
sleep 1; gnome-terminal --disable-factory --title="Server 1" --working-directory="${CWD}" -- ./ServerApp -p 53661 -i 1 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58010,58011,58012,58013 & disown
sleep 1; gnome-terminal --disable-factory --title="Server 2" --working-directory="${CWD}" -- ./ServerApp -p 53662 -i 2 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58010,58011,58012,58013 & disown
sleep 1; gnome-terminal --disable-factory --title="Server 3" --working-directory="${CWD}" -- ./ServerApp -p 53663 -i 3 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58010,58011,58012,58013 & disown
