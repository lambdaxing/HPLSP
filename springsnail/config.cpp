#include "config.h"

static const vector<host> logicHosts = {
    host{"59.111.160.195", 80, 5},
    host{"59.111.19.33", 80, 5}
};

static const char* localhost = "172.17.10.100";
static const int localport = 8080;

vector<host> gethosts() {
    return logicHosts;
}

const char* getlocalhost() {
    return localhost;
}

int getlocalport() {
    return localport;
}
