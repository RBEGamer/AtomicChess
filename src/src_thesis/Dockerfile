FROM ubuntu:20.04
WORKDIR /var/thesis
ARG DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install texlive-full texmaker pandoc sed nano -y

RUN apt install build-essential -y
RUN apt-get install haskell-platform -y
RUN apt install cabal-install -y

#RUN apt install locales
#RUN locale-gen en_US.utf8


#RUN export PATH="$HOME/.cabal/bin:$PATH"
#RUN cabal update

#RUN cabal install alex happy
#RUN cabal install --ghc-options="+RTS -M7G" -j1 --force-reinstalls pandoc
#RUN cabal install --ghc-options="+RTS -M7G" -j1 --force-reinstalls --dependencies-only pandoc-csv2table

CMD ["/var/thesis/build_thesis.sh"]
