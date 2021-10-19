
SHELL     = /bin/bash
CWD       = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

.PHONY: launch1 launch2 clean kill killServer killClient

launch1:
	@gnome-terminal --tab --disable-factory --title="Server 0" --working-directory="${CWD}" -- ./ServerApp -p 58000 -i 0 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -s 58000,58001,58002,58003 -r 53656,53657,53658,53659,53660,53661,53662,53663,53664,53665,53666,53667 & disown;
	@sleep 1; gnome-terminal --tab --disable-factory --title="Server 1" --working-directory="${CWD}" -- ./ServerApp -p 58001 -i 1 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -s 58000,58001,58002,58003 -r 53656,53657,53658,53659,53660,53661,53662,53663,53664,53665,53666,53667 & disown;
	@sleep 1; gnome-terminal --tab --disable-factory --title="Server 2" --working-directory="${CWD}" -- ./ServerApp -p 58002 -i 2 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -s 58000,58001,58002,58003 -r 53656,53657,53658,53659,53660,53661,53662,53663,53664,53665,53666,53667 & disown;
	@sleep 1; gnome-terminal --tab --disable-factory --title="Server 3" --working-directory="${CWD}" -- ./ServerApp -p 58003 -i 3 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -s 58000,58001,58002,58003 -r 53656,53657,53658,53659,53660,53661,53662,53663,53664,53665,53666,53667 & disown;
	@echo "127.0.0.1 53656" > servers.data
	@echo "127.0.0.1 53657" >> servers.data
	@echo "127.0.0.1 53658" >> servers.data
	@echo "127.0.0.1 53659" >> servers.data

launch2:
	@gnome-terminal --disable-factory --title="Server 0" --working-directory="${CWD}" -- ./ServerApp -p 58010 -i 0 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -s 58010,58011,58012,58013 -r 53756,53757,53758,53759,53760,53761,53762,53763,53764,53765,53766,53767 & disown;
	@sleep 1; gnome-terminal --disable-factory --title="Server 1" --working-directory="${CWD}" -- ./ServerApp -p 58011 -i 1 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -s 58010,58011,58012,58013 -r 53756,53757,53758,53759,53760,53761,53762,53763,53764,53765,53766,53767 & disown;
	@sleep 1; gnome-terminal --disable-factory --title="Server 2" --working-directory="${CWD}" -- ./ServerApp -p 58012 -i 2 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -s 58010,58011,58012,58013 -r 53756,53757,53758,53759,53760,53761,53762,53763,53764,53765,53766,53767 & disown;
	@sleep 1; gnome-terminal --disable-factory --title="Server 3" --working-directory="${CWD}" -- ./ServerApp -p 58013 -i 3 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -s 58010,58011,58012,58013 -r 53756,53757,53758,53759,53760,53761,53762,53763,53764,53765,53766,53767 & disown;
	@echo "127.0.0.1 53660" > servers.data
	@echo "127.0.0.1 53661" >> servers.data
	@echo "127.0.0.1 53662" >> servers.data
	@echo "127.0.0.1 53663" >> servers.data

clean: kill
	rm -rf ${CWD}/*.db
	rm -f ${CWD}/servers.data

kill: killServer killClient
	@:;

killServer:
	@killall -SIGINT ServerApp > /dev/null 2>&1 || :;

killClient:
	@killall -SIGINT ClientApp > /dev/null 2>&1 || :;