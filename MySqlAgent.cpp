//
// Created by Shidfar Hodizoda on 9/7/16.
//

#include "MySqlAgent.h"

MySqlAgent::MySqlAgent()
{
    Json::Value root = parseJsonFile("config.json");
    if(!root.empty())
    {
        string _server = root["mysqldatabase"]["server"].asString();
        string _user = root["mysqldatabase"]["user"].asString();
        string _password = root["mysqldatabase"]["password"].asString();
        string _database = root["mysqldatabase"]["database"].asString();

        this->server = _server;
        this->user = _user;
        this->password = _password;
        this->database = _database;

        printf(" > %s: Database config read successfully\n", __FUNCTION__);
    }
    else
    {
        printf(" > %s: Parsing error in config.json\n", __FUNCTION__);
    }
    this->Connection_OK();

}
bool MySqlAgent::Connection_OK()
{
    MYSQL *connect = mysql_init(NULL);
    if(!mysql_real_connect(connect, this->server.c_str(), this->user.c_str(), this->password.c_str(), this->database.c_str(), 0, NULL, 0))
    {
        printf(" > Could not connect to database\n %s\n", mysql_error(connect));
        connect = NULL;
        return false;
    }
    printf(" > Connection Check Successful\n");
    mysql_close(connect);
    return true;
}