FROM ubuntu:20.04
WORKDIR /var/thesis
ARG DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install texlive-full texmaker pandoc sed nano -y

RUN apt install locales
#UN locale-gen en_US.utf8

CMD ["/var/thesis/build_thesis.sh"]
