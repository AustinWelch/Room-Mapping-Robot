

all: .compile .jar


.compile: 
	javac com/ti/msp430/imagereformer/*java

.jar:
	jar cmfv ImageReformer.mf ImageReformer.jar  com/ti/msp430/imagereformer/* images/*

.clean:
	rm ImageReformer.jar com/ti/msp430/imagereformer/*.class
