FROM ubuntu:20.04

WORKDIR /var/


ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt upgrade -y && \
    apt-get install -y \
    build-essential \
    python \
    python3 \
    bash \
    bc \
    binutils \
    build-essential \
    bzip2 \
    cpio \
    g++ \
    gcc \
    git \
    gzip \
    locales \
    libncurses5-dev \
    libdevmapper-dev \
    libsystemd-dev \
    make \
    mercurial \
    whois \
    patch \
    perl \
    python \
    rsync \
    sed \
    tar \
    vim \
    unzip \
    wget \
    bison \
    flex \
    libssl-dev \
    libfdt-dev \
    nano


COPY . /var/


# AFTER DOCKER RUN => BUILD THE IMAGE WITH ATC PACKAGES AND ALSO BUILD SW UPDATE PACKAGES
CMD ["./entrypoint.sh"]
