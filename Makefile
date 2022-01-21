MAKEFLAGS=--no-print-directory
all: build
	make -C build

clean:
	make clean -C build

test:
	./build/ucode --exit

run:
	./build/ucode

build:
	mkdir $@
	cmake -B $@ -S .
