export ARCHITECTURE := arm
export CROSS_COMPILER := arm-linux-

obj-m   :=Linkedlist.o

KDIR := /home/deepesh/buildroot/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220
PWD := $(shell pwd)

all: default
default:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCHITECTURE) CROSS_COMPILE=$(CROSS_COMPILER) modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions 
