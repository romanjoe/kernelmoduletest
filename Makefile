CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
TARGET1 = md1
TARGET2 = md2
TARGET3 = md3
TARGET4 = log_level

obj-m	:=  $(TARGET1).o $(TARGET2).o $(TARGET3).o $(TARGET4).o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o *.cmd *.flags *.mod.c *.order
	@rm -f .*.*.cmd *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rt .tmp_versions

disclean: clean
	@rm *.ko *.symvers