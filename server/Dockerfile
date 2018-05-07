FROM ubuntu:16.04

RUN apt-get update && apt-get install -y \
  libmicrohttpd-dev \
  libjansson-dev \
  libcurl4-gnutls-dev \
  libgnutls28-dev \
  libgcrypt20-dev 

# Copy the server C binary and the JSON library into the container
COPY ./rest_server /rest_server

COPY ./libs/libjansson.so.4 /usr/lib/
COPY ./libs/libulfius.so /usr/lib/ 
COPY ./libs/libyder.so /usr/lib/
COPY ./libs/liborcania.so /usr/lib/

# Expose the port that the server will listen on
EXPOSE 1234

# Set the LD_LIBRARY_PATH so that the server binary will find the JSON library
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/lib

CMD ["/rest_server"]