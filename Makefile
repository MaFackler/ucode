MAKEFLAGS=--no-print-directory
all: build
	make -C build

clean:
	make clean -C build

build:
	mkdir $@
	cmake -B $@ -S .
