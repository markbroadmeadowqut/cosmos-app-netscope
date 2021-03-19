APP = netscope
CC = gcc	

$(APP): $(APP).c
	$(CC) -o $@ $<

clean:
	rm $(APP)
