//
// Created by Marcel Ochsendorf on 24.01.24.
//

#ifndef ATC_SERVER_HELPER_H
#define ATC_SERVER_HELPER_H

#include <string>
#include <memory>
#include <stdexcept>
#include <random>
#include <algorithm>

char *getCmdOption(char **begin, char **end, const std::string &option);
bool cmdOptionExists(char **begin, char **end, const std::string &option);

void sanitize(std::string &stringValue);
std::string sanitize_r(const std::string& _i);
bool randomBoolean();



#endif //ATC_SERVER_HELPER_H
