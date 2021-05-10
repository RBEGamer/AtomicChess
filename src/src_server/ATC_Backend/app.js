
var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var session = require('express-session');//sessions
var sessionstore = require('sessionstore'); //sessions
var md5 = require('md5'); //sessions


var indexRouter = require('./routes/index');
var restRouter = require('./routes/rest');
var app = express();

//SETUP ALL SUB SYSTEMS BEFORE STARTING THE MAIN APPLICATION

//LOAD/CREATE CONFIG
var CFG = require('./config/config'); //include the cofnig file
CFG.init_config();

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
app.use(express.urlencoded({ extended: true }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use(express.json({limit: '1024mb',extended:true}));
app.use(express.urlencoded({limit: '1024mb',extended:true}));

app.set('trust proxy', 1);
//ALLOW CROSS ORIGIN REQUEST FOR DEBUGGING
app.use(function (req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});
//HANDLE UNTRUSTED INPUT
app.use(require('sanitize').middleware);


//ENABLE SESSIONS
app.use(session({
  secret: md5(String(Date.now())+CFG.getConfig().secret_addition), // MAKE A UNIQUE HASH EVERY TIME THE SERVER STARTS // SITEEFFECT: SESSION FROM OLD CLIENT WILL BE DESTROYED
  store: sessionstore.createSessionStore(),
  resave: true,
  saveUninitialized: true
}));

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


module.exports = app;
