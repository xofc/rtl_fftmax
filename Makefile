# FLAGS=-march=armv6 -mfloat-abi=hard -mfpu=vfp
# FLAGS=-march=armv6zk -mcpu=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp
FLAGS =

all: rtl_fftmax

rtl_fftmax: rtl_fftmax.c
	gcc rtl_fftmax.c -o rtl_fftmax -lfftw3 -lm
