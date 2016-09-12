//
// Created by Shidfar Hodizoda on 9/12/16.
//

#ifndef UNISERVER_MONGOAGENT_H
#define UNISERVER_MONGOAGENT_H

#include <iostream>
#include <string.h>
#include <list>
#include <vector>
#include <mongo/client/dbclient.h>
#include <mongo/client/dbclientinterface.h>
#include <boost/iterator/iterator_concepts.hpp>
#include <libbson-1.0bson.h>


using namespace std;
using namespace mongo;

class MongoAgent
{
public:
    MongoAgent();
    virtual  ~MongoAgent();

private:
    DBClientConnection* dbconn;
    DBClientInterface* clientInterface;
    client::GlobalInstance instance;

    string datacollectionname;
    string latestdatacollection;
    string eventcollectionname;
    string behaviourcollectionname;
};


#endif //UNISERVER_MONGOAGENT_H


/*


#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <mongo/client/dbclient.h>
#include <mongo/client/dbclientinterface.h>


#include "EEModel.h"

using namespace std;
using namespace mongo;

class EEMongoClient {
public:
  EEMongoClient();
  virtual ~EEMongoClient();
//this->mClient->insertDocument(deviceidentifier, sensoridentifier, sa_id, sa_type_id, data, zone_id,  unit_id, datetime);
  void insertDocument(string deviceidentifier, string sensoridentifier, string sa_id, string sa_type_id, string data , string zone_id, string unit_id, string datetime);
//  void insertDocument(string deviceidentifier, string sensoridentifier, string sa_id, string sa_type_id, string data , string zone_id, string unit_id, string datetime);

  list<string> getAllEventId();
  map< string, string > getActionStateMap();

  EventNode getEventNode(string _id);
  bool updateEventTriggeredTime(string _id, string _datetime);

private:
  DBClientConnection *dbconn;
  DBClientInterface * clientInterface;
  client::GlobalInstance instance;

  string datacollectionname;
  string latestdatacollection;
  string eventcollectionname;
  string behaviourcollectionname;

};


#endif

 * */