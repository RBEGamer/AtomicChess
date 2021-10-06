FROM python:3.7-alpine
LABEL maintainer="Marcel Ochsendorf info@marcelochsendorf.com"
WORKDIR /app
COPY . /app
RUN pwd
RUN ls -la
RUN pip3 install -r /app/requirements.txt
RUN pip3 install python-chess


RUN rm -f ./RUN_IN_CONTAINER
EXPOSE 5003
CMD ["python3", "./app.py"]
