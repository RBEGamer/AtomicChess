FROM node:14

# WOKR DIR
WORKDIR /usr/src/app
COPY ./package*.json ./
RUN npm install
# COPY SOURCE
COPY ./ .
RUN npm audit fix
EXPOSE 3020
CMD [ "node", "server.js" ]
