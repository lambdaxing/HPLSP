#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include "mgr.h"

using std::vector;

vector<host> gethosts();
const char* getlocalhost();
int getlocalport();

#endif