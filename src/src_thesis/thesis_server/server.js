
var express = require('express');
var app = express();
var path = require('path');
var server = require('http').createServer(app);

var express = require('express');
var session = require('express-session');//sessions
var bodyParser = require('body-parser');//sessions
var sessionstore = require('sessionstore'); //sessions


var fs = require('fs');
var sanitizer = require('sanitizer');
var fileUpload = require('express-fileupload');
var listEndpoints = require('express-list-endpoints'); //for rest api explorer


var CNF = require('./config/config'); //include the cofnig file
CNF.init_config();

var appDirectory = require('path').dirname(process.pkg ? process.execPath : (require.main ? require.main.filename : process.argv[0]));
console.log(appDirectory);
CNF.set_key('app_directory',appDirectory);

var port = process.env.PORT || CNF.get_key('webserver_default_port') || 3000;
console.log(port);
var hostname = process.env.HOSTNAME || CNF.get_key('hostname') || 'http://127.0.0.1:' + port + '/';
console.log(hostname);



//-------- EXPRESS APP SETUP --------------- //
app.set('trust proxy', 1);
app.use(function (req, res, next) {
    if (!req.session) {
        return next(); //handle error
    }
    next(); //otherwise continue
});
app.set('views', __dirname + '/views');
app.engine('html', require('ejs').renderFile);
// Routing
app.use(express.static(path.join(__dirname, 'public')));
app.use(session({
    secret: CNF.get_key('session_secret') || 'sdfkmgomwepipwiomeoma',
    store: sessionstore.createSessionStore(),
    resave: true,
    saveUninitialized: true
}));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
    extended: true
}));
app.use(fileUpload());

app.use(require('sanitize').middleware);


app.use(function (req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});

server.listen(port, function () {
    console.log('Server listening at port %d', port);
});
// ---------------- END EXPRESS SETUP -------------- //










app.get('/', function (req, res) {
    res.redirect('/index');
});

app.get('/index', function (req, res) {
    res.render('index.ejs', {
        app_name_title: CNF.get_key('app_name_title') || 'APP TILE HERE',
        app_name_site: CNF.get_key('app_name_site') || 'APP NAME HERE',

    });
});



//423467

app.get('/document', function(req, res, next) {
    console.log(req.body);
    var tok = req.queryString('toc');
    var doc = req.queryString('doc');


    if(doc === "ba" && tok !== CNF.get_key('acess_token')){
        res.sendFile(path.join(appDirectory,"./protected/thesis_document.pdf"));
        return;
    }else  if(doc === "rep" && tok !== CNF.get_key('acess_token')){
        res.sendFile(path.join(appDirectory,"./protected/report_document.pdf"));
        return;
    }else  if(doc === "cv" && tok !== CNF.get_key('acess_token')){
        res.sendFile(path.join(appDirectory,"./protected/cv.pdf"));
        return;
    }else  if(doc === "opentmas" && tok !== CNF.get_key('acess_token_ot')){
        res.sendFile(path.join(appDirectory,"./protected/opentmas.pdf"));
        return;
    }

    req.redirect('/');
    return;

  });


app.get('/redirect', function(req, res, next) {

  var dest = req.queryString('destination');
  var toc = req.queryString('toc');

  console.log(dest);
  var real_dest = "/";

  if(CNF.get_key('redirect_urls')){
   real_dest = CNF.get_key('redirect_urls')[dest];
  if(real_dest == undefined || real_dest == null || real_dest === ""){
    real_dest = "/";
  }
}

  real_dest = String(real_dest).replace("&toc=","&tok="+toc);
  res.redirect(real_dest);
});






















//---------------------- FOR REST ENDPOINT LISTING ---------------------------------- //
app.get('/rest', function (req, res) {
    res.redirect('/restexplorer.html');
});

//RETURNS A JSON WITH ONLY /rest ENPOINTS TO GENERATE A NICE HTML SITE
var REST_ENDPOINT_PATH_BEGIN_REGEX = "^\/rest\/(.)*$"; //REGEX FOR ALL /rest/* beginning
var REST_API_TITLE = CNF.get_key('app_name') | 'APP NAME HERE';
var rest_endpoint_regex = new RegExp(REST_ENDPOINT_PATH_BEGIN_REGEX);
var REST_PARAM_REGEX = "\/:(.*)\/"; // FINDS /:id/ /:hallo/test
//HERE YOU CAN ADD ADDITIONAL CALL DESCTIPRION
var REST_ENDPOINTS_DESCRIPTIONS = [
    { endpoints: "/rest/update/:id", text: "UPDATE A VALUES WITH ID" }

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
                    element.url_parameters.push({ name: cleanedParams[finalCPIndex] });

                }
            }
            //ADD ENPOINT SET TO FINAL OUTPUT
            tmp.push(element);
        }
    }
    res.json({ api_name: REST_API_TITLE, endpoints: tmp });
});
