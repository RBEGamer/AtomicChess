FROM ubuntu:20.04

WORKDIR /var/buildroot


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

    
RUN locale-gen en_US.utf8



RUN echo 'root:root' |chpasswd

ENV O=/buildroot_output
RUN touch .config
RUN touch kernel.config
RUN mkdir -p /var/buildroot/output/images


RUN mkdir /root/.ssh

RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*



# COPY BUILDROOT FILES INTO CONTAINER
RUN rm -Rf /var/buildroot
COPY ./ /var/buildroot
# RESTORE BUILDROOT CONFIG
RUN bash /var/buildroot/restore_config.sh
# CHECK GENERATED PUB/PRIV KEYS FOR SW UPDATE EXISTS
RUN bash /var/buildroot/check_keys.sh
# RUN A INITIAL MAKE TO DOWNLOAD THE INITIAL PACKAGES
#RUN make -j15




# AFTER DOCKER RUN => BUILD THE IMAGE WITH ATC PACKAGES AND ALSO BUILD SW UPDATE PACKAGES
CMD ["/var/buildroot/build.sh"]

