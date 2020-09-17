FROM python:3.7-alpine as base
LABEL maintainer="Marcel Ochsendorf info@marcelochsendorf.com"

WORKDIR /app
COPY . .
RUN pip3 install -r ./requirements.txt
EXPOSE 5000
CMD ["python3", "main.py"]