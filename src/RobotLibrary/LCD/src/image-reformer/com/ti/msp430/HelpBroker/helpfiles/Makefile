
# Makefile to run doxygen on the *.md files and create the html 

default: clean html

clean:
	rm -rf html/ latex/

html:
	doxygen doxyfile
	cp idMap.xml html/.
