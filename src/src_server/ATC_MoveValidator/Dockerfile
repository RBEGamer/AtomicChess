FROM python:3.7-alpine as base
LABEL maintainer="Marcel Ochsendorf info@marcelochsendorf.com"

FROM base as builder
RUN mkdir /install
WORKDIR /install
COPY requirements.txt /requirements.txt
RUN pip3 install --install-option="--prefix=/install" -r /requirements.txt

FROM base

COPY --from=builder /install /usr/local
COPY . /app
WORKDIR /app
RUN rm -f ./RUN_IN_CONTAINER
EXPOSE 5000
CMD ["python", "app.py"]