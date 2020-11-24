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
module.exports = router;
