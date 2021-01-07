var app = require('express')();
var http = require('http').createServer(app);
var io = require('socket.io')(http);
var express = require('express');
var router = express.Router();
var CFG = require('../config'); //include the cofnig file


/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Game' });
});


router.get('/redirect', function(req, res, next) {
  console.log(req.body);
  var dest = req.queryString("destination");
  var real_dest = "/";
  if(dest){
  switch (dest){
    case "github": real_dest=CFG.getConfig().github_repository;break;
    case "impressum":real_dest=CFG.getConfig().impressum_ulr;break;
    default:real_dest="/";break;
  }
  }
  res.redirect(real_dest);
});


router.get('/statistics', function(req, res, next) {
  res.render('statistics_home', { title: 'Game' });
});


router.get('/profile', function(req, res, next) {
  res.render('statistics_profile', { title: 'Game' });
});



router.get('/webclient', function(req, res, next) {
  res.render('webclient', { title: 'Game' });
});


router.get('/server_config', function(req, res, next) {
  res.render('server_config', { title: 'SERVER CONFIG' });
});

router.get('/logs', function(req, res, next) {
  res.render('log_viewer', { title: 'LOG VIEWER' });
});


//------------------------- SOCKET IO ROUTER ------------------- //

io.on('connection', (socket) => {
  console.log('a user connected');
});








module.exports = router;
