#include "sister.hpp"
#include <string>

sister::sister(int index) 
    : sidekick("Sister " + std::to_string(index), 1, 2) {}
