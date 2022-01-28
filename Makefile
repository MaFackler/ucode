MAKEFLAGS=--no-print-directory
BUILDDIR=build

all: $(BUILDDIR)
	make -C $<

clean:
	make clean -C $(BUILDDIR)
	rm -Rf $(BUILDDIR)

test:
	./$(BUILDDIR)/uc --exit

tdd:
	find src/ | entr sh -c 'clear && make && clear && make test'

run:
	./$(BUILDDIR)/ucode

$(BUILDDIR):
	mkdir $@
	cmake -B $@ -S .

unity:
	mkdir $(BUILDDIR)
	cmake -B $(BUILDDIR) -S . -DCMAKE_UNITY_BUILD=ON

install:
	cp ./$(BUILDDIR)/uc /usr/local/bin/
