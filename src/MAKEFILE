#
# Makefile for etherdfs, requires Open Watcom v1.9
# Copyright (C) 2017 Mateusz Viste
#
# http://etherdfs.sourceforge.net
#

all: etherdfs.exe

genmsg.exe: genmsg.c version.h
	wcl -y -0 -s -d0 -lr -ms -we -wx -os genmsg.c -fe=genmsg.exe

chint.obj: chint086.asm
	wasm -0 chint086.asm -fo=chint.obj -ms

etherdfs.exe: genmsg.exe etherdfs.c chint.obj dosstruc.h globals.h version.h
	genmsg.exe
	wcl -y -0 -s -d0 -lr -ms -we -wx -k1024 -fm=etherdfs.map -os chint.obj etherdfs.c -fe=etherdfs.exe
	upx -9 --8086 etherdfs.exe

# -y      ignore the WCL env. variable, if any
# -0      generate code for 8086
# -s      disable stack overflow checks
# -d0     don't include debugging information
# -lr     compile to a DOS real-mode application
# -ms     small memory model
# -we     treat all warnings as errors
# -wx     set warning level to max
# -k1024  set stack size to 1024 bytes (for the non-resident part)
# -fm=    generate a map file
# -os     optimize for size
# -fe     set output file name

clean: .symbolic
	if exist etherdfs.exe del etherdfs.exe
	if exist genmsg.exe del genmsg.exe
	del *.obj

pkg: .symbolic etherdfs.exe
	if exist etherdfs.zip del etherdfs.zip
	zip -9 -k etherdfs.zip etherdfs.exe etherdfs.txt history.txt
	if exist ethersrc.zip del ethersrc.zip
	zip -9 -k ethersrc.zip *.h *.c *.asm *.txt makefile
