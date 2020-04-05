# Gerald Arocena
# CSCI E-28, Spring 2020
# 4-5-2020 
# hw 4
# Makefile for pong.c

GCC    = gcc
CFLAGS = -Wall -Wextra -g 

pong: pong.c pong.h alarmlib.c alarmlib.h paddle.c paddle.h
	$(GCC) $(CFLAGS) pong.c alarmlib.c paddle.c -lcurses -o pong

clean:
	rm -f pong