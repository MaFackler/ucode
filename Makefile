MAKEFLAGS=--no-print-directory
BUILDDIR=build
RELEASEDIR=release

all: $(BUILDDIR)
	make -C $<

.PHONY: clean
clean:
	if [ -d $(BUILDDIR) ]; then make clean -C $(BUILDDIR); fi
	if [ -d $(RELEASEDIR) ]; then make clean -C $(RELEASEDIR); fi
	rm -Rf $(BUILDDIR)
	rm -Rf $(RELEASEDIR)

test:
	./$(BUILDDIR)/uc --exit

tdd:
	find src/ | entr sh -c 'clear && make && clear && make test'

run:
	./$(BUILDDIR)/uc

$(BUILDDIR):
	mkdir $@
	cmake -DCMAKE_BUILD_TYPE=Debug -B $@ -S .

unity:
	mkdir $(BUILDDIR)
	cmake -B $(BUILDDIR) -S . -DCMAKE_UNITY_BUILD=ON

$(RELEASEDIR):
	mkdir $@
	cmake -DCMAKE_BUILD_TYPE=Release -B $@ -S .

$(RELEASEDIR)/uc: $(RELEASEDIR)
	make -C $<

install: $(RELEASEDIR)/uc
	cp ./$(RELEASEDIR)/uc /usr/local/bin/
