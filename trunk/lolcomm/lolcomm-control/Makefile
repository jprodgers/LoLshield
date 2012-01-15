
.PHONY: clean

control: control.c ../lolcomm/lolproto.h
	${CC} -Wall -I../lolcomm -o $@ $< -lm

clean:
	rm -f control
