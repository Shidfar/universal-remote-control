//
// Created by Shidfar Hodizoda on 9/12/16.
//

#include "ConfigReader.h"

Json::Value parseJsonFile(string path)
{
    ifstream file(path.c_str());
    stringstream buffer;
    buffer <<file.rdbuf();
    string message = buffer.str();
    file.close();

    Json::Value root;
    Json::Reader reader;

    reader.parse(message, root);

    return root;
}