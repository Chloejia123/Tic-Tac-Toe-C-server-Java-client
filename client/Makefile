all: Client

run: Client
	java -cp bin:./libs/* Client

Client: src/Client.java
	javac -d bin -sourcepath src -cp ./libs/*:. src/Client.java

clean:
	rm -rf bin/* src/*.class
