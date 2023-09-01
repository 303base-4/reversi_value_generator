FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y g++ libgflags-dev autoconf automake libtool libgoogle-glog-dev libpng-dev gdb make valgrind
