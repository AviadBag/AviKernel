FROM ubuntu:latest

ENV PREFIX=/opt/cross
ENV TARGET=i686-elf
ENV PATH=/opt/cross/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# Install dependencies
RUN apt-get update -y
RUN apt-get install wget build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo grub-pc-bin nasm git xorriso vim qemu-system -y

WORKDIR ~/ 

# Download cross-compiler
RUN wget https://mirrorservice.org/sites/sourceware.org/pub/gcc/releases/gcc-11.3.0/gcc-11.3.0.tar.gz --no-check-certificate \
   &&  wget https://mirrors.kernel.org/gnu/binutils/binutils-2.38.tar.gz --no-check-certificate
RUN tar -xzf gcc-11.3.0.tar.gz \
   &&  tar -xzf binutils-2.38.tar.gz

# Build cross compiler
RUN mkdir build-binutils \
   &&  cd build-binutils \
   &&  ../binutils-2.38/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror \
   &&  make \
   &&  make install
RUN mkdir build-gcc \
   &&  cd build-gcc \
   &&  ../gcc-11.3.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers \
   &&  make all-gcc \
   &&  make all-target-libgcc \
   &&  make install-gcc \
   &&  make install-target-libgcc 

# Add to path
ENV PATH=/opt/cross/bin:/opt/cross/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# Clean up
RUN rm -rf *

# Later dependencies (So I won't have to rebuild the image every time I add one)
RUN apt-get install -y gdb

# Setup project
#RUN git clone https://github.com/AviadBag/AviKernel.git
#RUN mkdir AviKernel/isodir

# Make sure we are up-to-date
ENTRYPOINT cd AviKernel \
        && git pull \
	&& bash

