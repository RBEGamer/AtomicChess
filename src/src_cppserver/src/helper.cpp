//
// Created by Marcel Ochsendorf on 24.01.24.
//
#include "helper.h"


bool cmdOptionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

char *getCmdOption(char **begin, char **end, const std::string &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return 0;
}

bool randomBoolean() {
    static std::default_random_engine generator(std::random_device{}());
    // With p = 0.5 you get equal probability for true and false
    static std::bernoulli_distribution distribution(0.5);
    return distribution(generator);
}

std::string sanitize_r(const std::string& _i){
    std::string  tmp = _i;
    sanitize(tmp);
    return tmp;
}
void sanitize(std::string &stringValue)
{
    // Add backslashes.
    for(auto i = stringValue.begin() ;  ;) {
        auto const pos = std::find_if(
                i,
                stringValue.end(),
                [](char const c) { return '\\' == c || '\'' == c || '"' == c; });
        if (pos == stringValue.end()) {
            break;
        }
        i = std::next(stringValue.insert(pos, '\\'), 2);
    }

    // Removes others.
    stringValue.erase(
            std::remove_if(
                    stringValue.begin(),
                    stringValue.end(),
                    [](char const c) {
                        return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c;
                    }),
            stringValue.end());
}
