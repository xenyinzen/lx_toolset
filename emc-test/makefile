cpu: cpu.c net netmon display usb
	cc -o cpu cpu.c -lpthread
net: net.c
	cc -o net -lnet net.c
netmon: netmon.c
	cc -o netmon -lpcap netmon.c
display: display.c
	cc -o display display.c
usb: usb.c
	cc -o usb usb.c

clean:
	rm cpu net display netmon
