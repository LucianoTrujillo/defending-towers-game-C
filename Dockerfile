FROM gcc:4.9
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN gcc *.c utiles.o -o defendiendo -std=c99 -Wconversion
RUN cd tests && gcc tests.c -o tests

