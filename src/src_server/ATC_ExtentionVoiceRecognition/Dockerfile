FROM node:14-alpine
LABEL maintainer="Marcel Ochsendorf info@marcelochsendorf.com"
# CREATE WORKDIR
RUN mkdir -p /home/node/app/node_modules && chown -R node:node /home/node/app
WORKDIR /home/node/app
# COPY PACAKGE FILES TO INSTALL PACKAGES FIRST
COPY package*.json ./
# DEFUALT USER
USER node
# INSTALL NPM PACKAGE
RUN npm install
# COPY ALL OTHER FILES TO FOLDER
COPY --chown=node:node . .
RUN ls
RUN rm -f ./RUN_IN_CONTAINER
RUN ls
# EXPOSE APP UNDER PORT 8880 -> SEE CONFIG.JSON FILE
EXPOSE 8880
# RUN NPM START COMMAND TO START THE WEBSERVER
CMD [ "npm", "start" ]