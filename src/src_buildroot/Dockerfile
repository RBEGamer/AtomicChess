FROM ubuntu:20.04


RUN apt update && apt upgrade

RUN apt  install -y openssh-server
RUN mkdir /var/run/sshd

RUN echo 'root:root' |chpasswd

RUN sed -ri 's/^#?PermitRootLogin\s+.*/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed -ri 's/UsePAM yes/#UsePAM yes/g' /etc/ssh/sshd_config

RUN mkdir /root/.ssh

RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*


WORKDIR /var/buildroot

COPY ./buildroot /var/buildroot

RUN bash /var/buildroot/restore_config.sh
RUN bash /var/buildroot/build.sh





EXPOSE 22

CMD    ["/usr/sbin/sshd", "-D"]