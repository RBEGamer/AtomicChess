#!/bin/bash
# ORIGINAL VERSION https://www.raspberrypi.org/forums/viewtopic.php?t=294165
# MODIFIED
VERSION=10.2.0
LANG=c,c++,fortran

if [ -d gcc-$VERSION ]; then
  cd gcc-$VERSION
  rm -rf obj
else
  echo "Download source tree ..."
  wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-$VERSION/gcc-$VERSION.tar.xz
  echo "Uncompress source tree ..."
  tar xf gcc-$VERSION.tar.xz
  rm -f gcc-$VERSION.tar.xz
  cd gcc-$VERSION
  contrib/download_prerequisites
fi
mkdir -p obj
cd obj

# Pi3+, Pi3, Pi4 in 32-bit mode
../configure --enable-languages=$LANG --with-cpu=cortex-a53 \
  --with-fpu=neon-fp-armv8 --with-float=hard --build=arm-linux-gnueabihf \
  --host=arm-linux-gnueabihf --target=arm-linux-gnueabihf

make -j `nproc`
sudo make install
