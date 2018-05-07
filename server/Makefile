all: run

# default make run is running in docker
run: docker_make run_docker

# This command runs in native Ubuntu environment
run_native: rest_server
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib ./rest_server

rest_server: rest_server.c
	gcc -o rest_server rest_server.c -lulfius -ljansson

call_server:
	curl -XGET localhost:1234/game

copy_needed_libs:
	mkdir -p libs
	cp /usr/local/lib/libjansson.so.4 ./libs/
	cp /usr/local/lib/libulfius.so.2.3.4 ./libs/libulfius.so
	cp /usr/local/lib/libyder.so.1.3.3 ./libs/libyder.so
	cp /usr/local/lib/liborcania.so.1.2.3 ./libs/liborcania.so

docker_make: rest_server copy_needed_libs
	# Copy systems libs into a local dir so that we can include them
	# in the Docker container
	sudo docker build -t ttt-server .

# Running this way will connect your terminal to the docker container,
# allowing getchar() to work as a method for shutting down the server
# --rm means delete the container automatically after exit
run_docker: docker_make
	sudo docker run -i -t -p 1234:1234 --rm ttt-server:latest

clean:
	rm -rf ./rest_server ./libs
	sudo docker images -a | grep "ttt-server" | awk '{print $3}' | xargs docker rmi
