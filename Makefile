MAKEFLAGS=--no-print-directory
all: build
	make -C build

clean:
	make clean -C build

test:
	./build/ucode --exit

tdd:
	find src/ | entr sh -c 'clear && make && clear && make test'

run:
	./build/ucode

build:
	mkdir $@
	cmake -B $@ -S .
