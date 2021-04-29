var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
var session = require('express-session');//sessions
var sessionstore = require('sessionstore'); //sessions
var sanitizer = require('sanitizer');
var md5 = require('md5'); //sessions
var app = express();
var crypto = require('crypto');
//LOAD CONFIG
var CFG = require('./config/config'); //include the cofnig file
CFG.init_config();
//CONNECT MONGO DB
var MDB = require("./mongo_db_connection");
MDB.connect();


//WHERE IS THE WORK DIR (containing the app.js)
var appDirectory = require('path').dirname(process.pkg ? process.execPath : (require.main ? require.main.filename : process.argv[0]));
// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
//SET LOGGER
app.use(logger('dev'));
//USE JSON DECODE FOR REQUESTS
app.use(express.json());
app.use(express.urlencoded({extended: true}));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use(express.json({limit: '1024mb', extended: true}));
app.use(express.urlencoded({limit: '1024mb', extended: true}));

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
    secret: md5(String(Date.now()) + CFG.getConfig().secret_addition), // MAKE A UNIQUE HASH EVERY TIME THE SERVER STARTS // SITEEFFECT: SESSION FROM OLD CLIENT WILL BE DESTROYED
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


function get_registered_devices(_user_hash, _callback) {
    MDB.getCollection().findOne({user_hash: _user_hash, DOCTYPE: "DEVICES"}, function (err, res) {
        _callback(err, res);
    });
}

function get_link_pin_device(_pin, _callback) {
    MDB.getCollection().findOne({link_pin: _pin, linked_table_hwid: null, DOCTYPE: "DEVICES"}, function (err, res) {
        _callback(err, res);
    });
}

function get_table_device(_hwid, _callback) {
    MDB.getCollection().findOne({linked_table_hwid: _hwid, DOCTYPE: "DEVICES"}, function (err, res) {
        _callback(err, res);
    });
}



app.get('/', function(req, res, next) {
    res.render('index', { title: 'Game' });
});


app.get('/register_table', (req, res) => {
    var hwid = req.queryString("hwid");
    var sid = req.queryString("userPin");
    //CHECK PARAMETER
    if (!hwid || !sid) {
        res.json({voice_response: 'register_table request failed', err: 'parameter missing'});
        return;
    }
    //CHECK IF DEVICE IS ALREADY REGISTERES
    get_link_pin_device(sid, function (grd_err, grd_res) {

        if (grd_err) {
            res.json({voice_response: 'alexa_register request failed', err: 'grd_err'});
            return;
        }
        //IF NO DEVICE FOUND WITH NOT LINKED STATE
        if (!grd_res) {
            res.json({voice_response: 'no unlinked device with this pin found', err: 'grd_res'});
            return;
        }
        //UPDATE => RESET THE LINK
        MDB.getCollection().updateOne({user_hash: grd_res.user_hash, DOCTYPE: "DEVICES"}, {
            $set: {
                linked_table_hwid: hwid,
                timestamp_linked: Date.now(),
                link_pin: sid
            }
        }, function (uo_err, uo_res) {
            if(uo_err){
                res.json({voice_response: 'table_register failed ', err: 'failed'});
            }else{
                res.json({voice_response: 'table_register success', err: null});
            }

        });
    });
});

app.get('/alexa_register', (req, res) => {

    var hwid = req.queryString("userId");
    var sid = req.queryString("userPin");


    if (!hwid || !sid) {
        res.json({voice_response: 'alexa_register request failed', err: 'parameter missing'});
        return;
    }

    //GEERATE HASH
    var user_hash = crypto.createHash('md5').update(hwid).digest('hex');
    //CHECK IF DEVICE IS ALREADY REGISTERES
    get_registered_devices(user_hash, function (grd_err, grd_res) {

        if (grd_err) {
            res.json({voice_response: 'alexa_register request failed', err: 'grd_err'});
            return;
        }

        if (!grd_res) {
            var user_entry = {
                alexa_user_account: hwid,
                user_hash: user_hash,
                DOCTYPE: "DEVICES",
                timestamp: Date.now(),
                linked_table_hwid: null,
                link_pin: sid,
                timestamp_linked:null,
                figure:null,
                to_field:null
            }


            MDB.getCollection().insertOne(user_entry, function (io_err, io_res) {
                if (io_err) {
                    res.json({voice_response: 'alexa_register failed', err: 'err'});
                } else {
                    res.json({voice_response: 'registration success', err: null});
                }

            });

        } else {
            //UPDATE => RESET THE LINK
            MDB.getCollection().updateOne({user_hash: user_hash, DOCTYPE: "DEVICES"}, {
                $set: {
                    linked_table_hwid: null,
                    timestamp: Date.now(),
                    link_pin: sid,
                    figure:null,
                    to_field:null
                }
            }, function (uo_err, uo_res) {
                res.json({voice_response: 'alexa_register already registered', err: ''});
            });

        }

        //TODO SAVE DEVICE TO DB
    });


});

app.get('/get_move', (req, res) => {
    var hwid = req.queryString("hwid");
    if (!hwid) {
        res.json({figure:null, to_field:null,err:true, err_str:"hwid missing"});
        return;
    }
    get_table_device(hwid, function (grd_err, grd_res) {
        if (grd_err) {
            res.json({figure:null, to_field:null,err:true, err_str:"get_table_device err"});
            return;
        }
        if (grd_res) {
            res.json({figure:grd_res.figure,to_field:grd_res.to_field,err:false, err_str:""});
        }else{
            res.json({figure:null, to_field:null,err:true, err_str:"move empty"});
        }
    });
});

app.get('/reset_move', (req, res) => {
    var hwid = req.queryString("hwid");
    if (!hwid) {
        res.json({err:true,err_str:"hwid missing"});
        return;
    }
    get_table_device(hwid, function (grd_err, grd_res) {
        if (grd_err) {
            res.json({err:true,err_str:"get_table_device err"});
            return;
        }
        if (grd_res) {
            MDB.getCollection().updateOne({linked_table_hwid: hwid, DOCTYPE: "DEVICES"}, {
                $set: {
                    figure:null,
                    to_field:null
                }
            }, function (uo_err, uo_res) {
                res.json({err:false,err_str:""});
            });
        }else{
            res.json({err:true,err_str:"grd_res err"});
        }
    });
});

app.post('/alexa_move', (req, res) => {
    var tt = req.body;
    var jsonbody = null;
    if (tt && tt.payload) {
        jsonbody = JSON.parse(tt.payload);
    }

    //CHECK IF FROM AMAZON
    if (jsonbody.requestEnvelope.context.System.apiEndpoint !== 'https://api.eu.amazonalexa.com') {
        res.json({voice_response: 'are you an alexa device ?', err: 'apiEndpoint check'});
        return;
    }

    //CHECK IF RIGHT INTENT
    if (jsonbody.requestEnvelope.request.intent.name !== "makemove") {
        res.json({voice_response: 'wrong intent', err: 'makemove'});
        return;
    }
    //CHECK IF MOVE IS POPULATED
    if (jsonbody.requestEnvelope.request.intent.slots.figure.value === "" || jsonbody.requestEnvelope.request.intent.slots.column.value === "" || jsonbody.requestEnvelope.request.intent.slots.row.value === "") {
        res.json({voice_response: 'slots invalid', err: 'column figure row'});
        return;
    }

    //GENERATE HASHED VERSION OF THE ALEXA DEVICE ID
    var user_hash = null;
    if (jsonbody.requestEnvelope.context.System.user.userId) {
        user_hash = crypto.createHash('md5').update(jsonbody.requestEnvelope.context.System.user.userId).digest('hex');
    }

    if (user_hash) {


        get_registered_devices(user_hash, function (grd_err, grd_res) {


            var move = {
                figure: String(jsonbody.requestEnvelope.request.intent.slots.figure.value),
                to_field: String(jsonbody.requestEnvelope.request.intent.slots.column.value) + String(jsonbody.requestEnvelope.request.intent.slots.row.value)
            };
            var move_hash = crypto.createHash('md5').update(move.figure + move.to_field).digest('hex');


            MDB.getCollection().updateOne({user_hash: user_hash, DOCTYPE: "DEVICES"}, {
                $set: {
                    figure: move.figure,
                    to_field: move.to_field,
                    move_hash: move_hash,
                }
            }, function (uo_err, uo_res) {
                res.json({voice_response: 'move stored', err: ''});
            });



        });


    } else {
        res.json({voice_response: 'userId not given', err: 'user_hash error'});
        return;
    }
});






var port = CFG.get_key("webserver_default_port");
console.log(`${port}`)
app.listen(port, () => {
    console.log(` at http://localhost:${port}`)
})

