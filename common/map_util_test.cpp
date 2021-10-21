#include "map_util.hpp"

#include <map>

using namespace snipet;

int main(int argc, const char*argv[]){
    std::map<int, int> m{{1,2}, {2,3}};
    int value = FindOrDie(m, 3);

    std::map<std::string, int> m1{{"abc", 3}};
    value = FindOrDie(m1, "a");
}