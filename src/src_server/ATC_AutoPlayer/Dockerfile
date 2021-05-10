FROM golang:latest
RUN mkdir /app
ADD . /app/
WORKDIR /app
RUN ls
RUN cd ./stockfish-11-linux/src/ && make clean && make build ARCH=autodetect
RUN go mod init AutoPlayer ; exit 0
RUN CGO_ENABLED=0 GOOS=linux go build -a -installsuffix cgo -o main .
CMD ["/app/main"]