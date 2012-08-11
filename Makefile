# Makefile for DAMP
# by Matt Craven

CC = gcc

ifdef DEBUG
CFLAGS = -Wall -g
else
CFLAGS = -fomit-frame-pointer -funroll-loops -mpentium -Wall -O3 -s
endif

LIBS = -lcda -lamp -lxaudio -lseer -lalleg
COMPRESSOR = upx

OBJ = graphics.o gui.o id3read.o indrv.o input.o jstick.o lcd.o \
      playlist.o script.o util.o beautify.o sleep.o lyrics3.o cd.o gtext.o \
      keys.o \
      audio.o audio_libamp.o audio_xaudio.o

XAUDIO_SDK_ROOT=C:/PROGRA~2/DJGPP/XASDK301
XAUDIO_SDK_LIB=$(XAUDIO_SDK_ROOT)/lib
XAUDIO_SDK_INC=$(XAUDIO_SDK_ROOT)/include

all: damp.exe

debug:
	$(MAKE) DEBUG=1

clean:
	@echo Removing object files
	del *.o

veryclean:
	@echo Removing object files...
	del *.o
	@echo Removing DAMP.EXE...
	del damp.exe

compress: damp.exe
	$(COMPRESSOR) damp.exe

damp.exe: damp.c $(OBJ) damp.h graphics.h gui.h id3read.h indrv.h input.h jstick.h lcd.h \
	playlist.h script.h util.h beautify.h sleep.h lyrics3.h cd.h gtext.h keys.h audio.h
	$(CC) $(CFLAGS) -o damp.exe damp.c $(OBJ) $(LIBS)

audio.o: audio.c audio.h audio_libamp.h audio_xaudio.h
	$(CC) -c $(CFLAGS) audio.c

audio_libamp.o: audio_libamp.c audio_libamp.h
	$(CC) -c $(CFLAGS) audio_libamp.c

audio_xaudio.o: audio_xaudio.c audio_xaudio.h
	$(CC) -I$(XAUDIO_SDK_INC) -c $(CFLAGS) audio_xaudio.c

beautify.o: beautify.c beautify.h
	$(CC) -c $(CFLAGS) beautify.c

cd.o:	cd.c cd.h damp.h playlist.h lcd.h input.h util.h
	$(CC) -c $(CFLAGS) cd.c

graphics.o: graphics.c graphics.h damp.h
	$(CC) -c $(CFLAGS) graphics.c

gtext.o: gtext.c gtext.h graphics.h damp.h
	$(CC) -c $(CFLAGS) gtext.c

gui.o: gui.c gui.h graphics.h
	$(CC) -c $(CFLAGS) gui.c

id3read.o: id3read.c id3read.h
	$(CC) -c $(CFLAGS) id3read.c

indrv.o: indrv.c indrv.h damp.h util.h
	$(CC) -c $(CFLAGS) indrv.c

input.o: input.c input.h util.h damp.h indrv.h jstick.h graphics.h keys.h
	$(CC) -c $(CFLAGS) input.c

jstick.o: jstick.c jstick.h damp.h
	$(CC) -c $(CFLAGS) jstick.c

keys.o: keys.c keys.h
	$(CC) -c $(CFLAGS) keys.c

lcd.o: lcd.c lcd.h damp.h playlist.h script.h id3read.h
	$(CC) -c $(CFLAGS) lcd.c

lyrics3.o: lyrics3.c lyrics3.h damp.h
	$(CC) -c $(CFLAGS) lyrics3.c

playlist.o: playlist.c playlist.h damp.h
	$(CC) -c $(CFLAGS) playlist.c

script.o: script.c damp.h input.h
	$(CC) -c $(CFLAGS) script.c

sleep.o: sleep.c sleep.h damp.h util.h
	$(CC) -c $(CFLAGS) sleep.c

util.o: util.c util.h damp.h playlist.h lcd.h graphics.h cd.h
	$(CC) -c $(CFLAGS) util.c
