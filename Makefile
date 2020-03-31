# Gerald Arocena
# CSCI E-28, Spring 2020
# 
# Makefile for pong.c

GCC    = gcc
CFLAGS = -Wall -Wextra -g 

pong: pong.c pong.h alarmlib.c alarmlib.h paddle.c paddle.h
	$(GCC) $(CFLAGS) pong.c alarmlib.c paddle.c -lcurses -o pong

clean:
	rm -f pong