//
// Created by Shidfar Hodizoda on 9/7/16.
//

#ifndef UNISERVER_MYSQLAGENT_H
#define UNISERVER_MYSQLAGENT_H

#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include <json/json.h>
#include <sstream>
#include <iostream>

#include "ConfigReader.h"

using namespace std;

class MySqlAgent
{
public:
    MySqlAgent();

private:
    bool Connection_OK();
    string server;
    string user;
    string password;
    string database;
};


#endif //UNISERVER_MYSQLAGENT_H
