FROM debian:latest

WORKDIR /usr/src/app

RUN apt update
RUN apt install -y libpng-dev libtiff-dev libassimp-dev git cmake build-essential wget unzip

RUN git clone https://github.com/jbeder/yaml-cpp.git

RUN cmake -S yaml-cpp -B yaml-cpp
RUN make -C yaml-cpp

RUN wget http://cimg.eu/files/CImg_latest.zip
RUN unzip CImg_latest.zip
RUN mv CImg-3.1.3_pre051622 cimg

COPY src src
COPY main.cpp main.cpp
COPY makefile makefile

CMD make