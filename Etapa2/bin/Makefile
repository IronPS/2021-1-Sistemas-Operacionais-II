
SHELL     = /bin/bash
CWD       = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

.PHONY: launch1 launch2 clean kill killServer killClient

launch1:
	@gnome-terminal --tab --disable-factory --title="Server 0" --working-directory="${CWD}" -- ./ServerApp -p 53656 -i 0 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58000,58001,58002,58003 -s 53656,53657,53658,53659 & disown;
	@sleep 1; gnome-terminal --tab --disable-factory --title="Server 1" --working-directory="${CWD}" -- ./ServerApp -p 53657 -i 1 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58000,58001,58002,58003 -s 53656,53657,53658,53659 & disown;
	@sleep 1; gnome-terminal --tab --disable-factory --title="Server 2" --working-directory="${CWD}" -- ./ServerApp -p 53658 -i 2 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58000,58001,58002,58003 -s 53656,53657,53658,53659 & disown;
	@sleep 1; gnome-terminal --tab --disable-factory --title="Server 3" --working-directory="${CWD}" -- ./ServerApp -p 53659 -i 3 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58000,58001,58002,58003 -s 53656,53657,53658,53659 & disown;
	@echo "127.0.0.1 53656" > servers.data
	@echo "127.0.0.1 53657" >> servers.data
	@echo "127.0.0.1 53658" >> servers.data
	@echo "127.0.0.1 53659" >> servers.data

launch2:
	@gnome-terminal --disable-factory --title="Server 0" --working-directory="${CWD}" -- ./ServerApp -p 53660 -i 0 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58010,58011,58012,58013 -s 53660,53661,53662,53663 & disown;
	@sleep 1; gnome-terminal --disable-factory --title="Server 1" --working-directory="${CWD}" -- ./ServerApp -p 53661 -i 1 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58010,58011,58012,58013 -s 53660,53661,53662,53663 & disown;
	@sleep 1; gnome-terminal --disable-factory --title="Server 2" --working-directory="${CWD}" -- ./ServerApp -p 53662 -i 2 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58010,58011,58012,58013 -s 53660,53661,53662,53663 & disown;
	@sleep 1; gnome-terminal --disable-factory --title="Server 3" --working-directory="${CWD}" -- ./ServerApp -p 53663 -i 3 -d 0,1,2,3 -a 127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1 -r 58010,58011,58012,58013 -s 53660,53661,53662,53663 & disown;
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