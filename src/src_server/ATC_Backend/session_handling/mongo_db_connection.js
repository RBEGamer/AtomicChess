var mongo = require('mongodb');
var MongoClient = require('mongodb').MongoClient;
var CONFIG = require('../config'); //include the cofnig file


var IsMongoConnected = null;
var MongoDBDatabase = null;


process.on("exit", function(){
    MongoClient.close();
});


function create_collection(_db,_collectionname) {
    _db.createCollection(_collectionname, function(err, res) {
        if (err){
            console.error("Collection cant be created created!" + _collectionname);
            console.error(err);
            return;
        }
        console.info("Collection created!" + _collectionname);

    });
}
//CONNECTS TO THE MONGODB AND CREATED THE COLLECTIONS AND DATABASES
function connect_db() {
    //TODO ADD {
    //    useNewUrlParser: true,
    //    useUnifiedTopology: true
    //  } AND USE THEN CATCH STUFF https://stackoverflow.com/questions/57546635/warning-on-connecting-to-mongodb-with-a-node-server
    MongoClient.connect(CONFIG.getConfig().mongodb_connection_url ,function (err,db) {
        if (err) {
            console.log(err);
            return;
        }
        //
        MongoDBDatabase = db.db(CONFIG.getConfig().mongodb_database_name);
        IsMongoConnected = true;
        //CREATE COLLECTION IF ALREADY EXISTS THIS WILL BE IGNORED
        create_collection(MongoDBDatabase, CONFIG.getConfig().mongodb_collection_games);
        create_collection(MongoDBDatabase, CONFIG.getConfig().mongodb_collection_profiles);
        create_collection(MongoDBDatabase, CONFIG.getConfig().mongodb_collection_lobby);
    });
}


module.exports = {
    connect: function(){
        return connect_db();
    },
    getProfileCollection: function () {
        return MongoDBDatabase.collection(CONFIG.getConfig().mongodb_collection_profiles);
    },
    getLobbyCollection: function () {
        return MongoDBDatabase.collection(CONFIG.getConfig().mongodb_collection_lobby);
    },
    getGameCollection: function () {
        return MongoDBDatabase.collection(CONFIG.getConfig().mongodb_collection_games);
    }
}