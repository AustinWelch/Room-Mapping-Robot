
# Makefile
 
.SUFFIXES: .java .class


default: classes 

classes:
	javac -classpath "${CLASSPATH}" *.java

helpfiles:
	cd helpfiles; $(MAKE);

clean:
	rm -rf *.class 

