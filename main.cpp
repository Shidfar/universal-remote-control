#include <unistd.h>
#include <stdio.h>
#include <iostream>

#include "ServerEngine.h"

using namespace std;

int main()
{
    ServerEngine *serverEngine = new ServerEngine();
    while(true) usleep(10000);
    return 0;
}
