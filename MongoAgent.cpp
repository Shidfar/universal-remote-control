//
// Created by Shidfar Hodizoda on 9/12/16.
//

#include "MongoAgent.h"

MongoAgent::MongoAgent()
{
    {
        if(!this->instance.initialized()) {

            cout << "Failed to initialize the client driver: " << instance.status() << endl;
            return;
        }

        this->behaviourcollectionname = "raxs.behaviours";
        this->eventcollectionname = "raxs.events";
        this->datacollectionname = "raxs.data";
        this->latestdatacollection = "raxs.latest_data";
        this->dbconn = new DBClientConnection();
        this->dbconn->connect(string("localhost:27017"));
//    this->clientInterface = new DBClientInterface();
//    this->clientInterface->connect(string("localhost:27017"));
    }
}
MongoAgent::~MongoAgent()
{
    ;
}

/*

#include "EEMongoDBClient.h"
#include <boost/iterator/iterator_concepts.hpp>

EEMongoClient::EEMongoClient()
{
    if(!this->instance.initialized()) {

	cout << "Failed to initialize the client driver: " << instance.status() << endl;
	return;
    }

    this->behaviourcollectionname = "raxs.behaviours";
    this->eventcollectionname = "raxs.events";
    this->datacollectionname = "raxs.data";
    this->latestdatacollection = "raxs.latest_data";
    this->dbconn = new DBClientConnection();
    this->dbconn->connect(string("localhost:27017"));
//    this->clientInterface = new DBClientInterface();
//    this->clientInterface->connect(string("localhost:27017"));
}

EEMongoClient::~EEMongoClient()
{

}
//string deviceidentifier, string sensoridentifier,
void EEMongoClient::insertDocument(string deviceidentifier, string sensoridentifier, string sa_id, string sa_type_id, string data , string zone_id, string unit_id, string datetime)
{
    INFORM(" [%s] %s_%s\n", datetime.c_str(), deviceidentifier.c_str(), sensoridentifier.c_str());
    BSONObj bobj = BSON( "sa_id" <<sa_id  << "sa_type_id" <<sa_type_id  << "data" <<data  << "zone_id" <<zone_id  <<"unit_id" <<unit_id  << "datetime" <<datetime   );
    if(strstr(sensoridentifier.c_str(), "METERING") != NULL)
    {
        INFORM(" [M] METERING\n");
        this->dbconn->insert(this->datacollectionname, bobj);
    }
    else
    {
        INFORM(" [M] DATA\n");
//        this->dbconn->insert(this->latestdatacollection, bobj);
        this->dbconn->update(this->latestdatacollection,  BSON("sa_id" <<sa_id  << "sa_type_id" <<sa_type_id  << "zone_id" <<zone_id  <<"unit_id" <<unit_id ), bobj );
        auto_ptr<DBClientCursor> cursor = this->dbconn->query(this->latestdatacollection, BSON("sa_id" <<sa_id  << "sa_type_id" <<sa_type_id  << "zone_id" <<zone_id  <<"unit_id" <<unit_id ));
        if(!cursor->more())
        {
            this->dbconn->insert(this->latestdatacollection, bobj);
        }
    }
}

list< string > EEMongoClient::getAllEventId()
{
    list< string > result;
    auto_ptr<DBClientCursor> cursor = this->dbconn->query(this->eventcollectionname, BSONObj());
    if(!cursor.get()) {
	return result;
    }

    while( cursor->more() ) {
	BSONObj obj = cursor->next();
	BSONElement eobj;
	obj.getObjectID(eobj);
	result.push_back(eobj.OID().toString());
    }

    return result;
}

map< string, string > EEMongoClient::getActionStateMap()
{
    map< string, string > result;
    auto_ptr<DBClientCursor> cursor = this->dbconn->query(this->behaviourcollectionname, BSONObj());
    if(!cursor.get()) {
	return result;
    }

    while( cursor->more() ) {
	BSONObj obj = cursor->next();

	string key = string(obj.getStringField("deviceidentifier")) + string(obj.getStringField("sensoridentifier"));
	result.insert(pair< string, string >( key, "-1" ));
    }

    return result;
}

EventNode EEMongoClient::getEventNode(string _id)
{
    EventNode node;

    BSONObjBuilder query;
    query.append( "_id" , OID(_id) );
    BSONObj res = this->dbconn->findOne(this->eventcollectionname , query.obj());

    node.id = _id;
    node.eventName = res["name"];

    vector<BSONElement> behaviourlist = res["behaviourlist"].Array();
    for (vector<BSONElement>::iterator it = behaviourlist.begin(); it != behaviourlist.end(); ++it) {
	BSONElement e = *it;

	BSONObjBuilder query2;
	query2.append( "_id" , e.OID() );
	BSONObj behaviourobj = this->dbconn->findOne(this->behaviourcollectionname, query2.obj());

	Behaviour beh;
	beh.deviceidentifier = behaviourobj.getStringField("deviceidentifier");
	beh.sensoridentifier = behaviourobj.getStringField("sensoridentifier");
	beh.mode = behaviourobj.getStringField("transmitmode");
	beh.condition = behaviourobj.getStringField("condition");
	beh.value = behaviourobj.getStringField("value");
	beh.checked = false;

	node.addBehavior(beh);
    }

    vector<BSONElement> actionlist = res["actionlist"].Array();
    for (vector<BSONElement>::iterator it = actionlist.begin(); it != actionlist.end(); ++it) {
	BSONElement e = *it;

	BSONObjBuilder query2;
	query2.append( "_id" , e.OID() );
	BSONObj behaviourobj = this->dbconn->findOne(this->behaviourcollectionname, query2.obj());

	Action act;
	act.deviceidentifier = behaviourobj.getStringField("deviceidentifier");
	act.sensoridentifier = behaviourobj.getStringField("sensoridentifier");
	act.protocolid = behaviourobj.getStringField("protocol");
	act.type = behaviourobj.getStringField("type");
	act.value = behaviourobj.getStringField("value");

	node.addAction(act);
    }

    return node;
}

bool EEMongoClient::updateEventTriggeredTime(string _id, string _datetime)
{
    BSONObjBuilder query;
    query.append( "_id" , OID(_id) );

    BSONObjBuilder upd;
    upd.append( "lasttriggered", _datetime);

    this->dbconn->update(this->eventcollectionname , query.obj(), upd.obj());
}

 * */