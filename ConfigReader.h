//
// Created by Shidfar Hodizoda on 9/12/16.
//

#ifndef UNISERVER_CONFIGREADER_H
#define UNISERVER_CONFIGREADER_H


#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <streambuf>
#include "json/json.h"

using namespace std;

Json::Value parseJsonFile(string path);


#endif //UNISERVER_CONFIGREADER_H
