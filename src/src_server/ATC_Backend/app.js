
var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');


var indexRouter = require('./routes/index');
var restRouter = require('./routes/rest');
var eas = require('express-async-errors');
var app = express();

//SETUP ALL SUB SYSTEMS BEFORE STARTING THE MAIN APPLICATION

//LOAD/CREATE CONFIG
var CNF = require('./config'); //include the cofnig file
CNF.init_config();

//CONNECT REDIS DB
var RDB = require('./session_handling/redis_db_connection');
RDB.connect();

//CONNECT MONGO DB
var MDB = require("./session_handling/mongo_db_connection");
MDB.connect();

//START SESSION CLEANUP
var SC = require("./session_handling/session_cleanup");
SC.startSessionCleanupCycle();

//START MATCHMAKING LOGIC
var MML = require("./chess_related_logic/matchmaking_logic");
MML.startMatchmakingCycle();



//WHERE IS THE WORK DIR (containing the app.js)
var appDirectory = require('path').dirname(process.pkg ? process.execPath : (require.main ? require.main.filename : process.argv[0]));

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
//SET LOGGER
app.use(logger('dev'));
//USE JSON DECODE FOR REQUESTS
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.set('trust proxy', 1);
//ALLOW CROSS ORIGIN REQUEST FOR DEBUGGING
app.use(function (req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});
//HANDLE UNTRUSTED INPUT
app.use(require('sanitize').middleware);

//USE THE ASSETS "FOLDER" TO PROVIDE LIBRARIES FROM NODE MODULES
app.use('/assets', [
  express.static(__dirname + '/node_modules/jquery/dist/'),
  express.static(__dirname + '/node_modules/bootstrap/dist/'),
  express.static(__dirname + '/node_modules/materialize-css/dist/')
]);

app.use(function (err, req, res, next) {
  if (err) {
    res.status(500);
    return res.json({err:err.message})
  }
})

app.use('/rest', restRouter);
app.use('/', indexRouter);




// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});





app.use((err, req, res, next) => {
  // response to user with 403 error and details
});



app.get('/restexplorer', function (req, res) {
  res.redirect('/restexplorer.html');
});

//RETURNS A JSON WITH ONLY /rest ENPOINTS TO GENERATE A NICE HTML SITE
var REST_ENDPOINT_PATH_BEGIN_REGEX = "^\/rest\/(.)*$"; //REGEX FOR ALL /rest/* beginning
var REST_API_TITLE = CNF.getConfig().app_name;
var rest_endpoint_regex = new RegExp(REST_ENDPOINT_PATH_BEGIN_REGEX);
var REST_PARAM_REGEX = "\/:(.*)\/"; // FINDS /:id/ /:hallo/test
//HERE YOU CAN ADD ADDITIONAL CALL DESCTIPRION
var REST_ENDPOINTS_DESCRIPTIONS = [{
  endpoints: "/rest/update/:id",
  text: "UPDATE A VALUES WITH ID"
}

];

app.get('/listendpoints', function (req, res) {
  var ep = listEndpoints(app);
  var tmp = [];
  for (let index = 0; index < ep.length; index++) {
    var element = ep[index];
    if (rest_endpoint_regex.test(element.path)) {
      //LOAD OPTIONAL DESCRIPTION
      for (let descindex = 0; descindex < REST_ENDPOINTS_DESCRIPTIONS.length; descindex++) {
        if (REST_ENDPOINTS_DESCRIPTIONS[descindex].endpoints == element.path) {
          element.desc = REST_ENDPOINTS_DESCRIPTIONS[descindex].text;
        }
      }
      //SEARCH FOR PARAMETERS
      //ONLY REST URL PARAMETERS /:id/ CAN BE PARSED
      //DO A REGEX TO THE FIRST:PARAMETER
      element.url_parameters = [];
      var arr = (String(element.path) + "/").match(REST_PARAM_REGEX);
      if (arr != null) {
        //SPLIT REST BY /
        var splittedParams = String(arr[0]).split("/");
        var cleanedParams = [];
        //CLEAN PARAEMETER BY LOOKING FOR A : -> THAT IS A PARAMETER
        for (let cpIndex = 0; cpIndex < splittedParams.length; cpIndex++) {
          if (splittedParams[cpIndex].startsWith(':')) {
            cleanedParams.push(splittedParams[cpIndex].replace(":", "")); //REMOVE :
          }
        }
        //ADD CLEANED PARAMES TO THE FINAL JOSN OUTPUT
        for (let finalCPIndex = 0; finalCPIndex < cleanedParams.length; finalCPIndex++) {
          element.url_parameters.push({
            name: cleanedParams[finalCPIndex]
          });

        }
      }
      //ADD ENPOINT SET TO FINAL OUTPUT
      tmp.push(element);
    }
  }
  res.json({
    api_name: REST_API_TITLE,
    endpoints: tmp
  });
});



module.exports = app;
