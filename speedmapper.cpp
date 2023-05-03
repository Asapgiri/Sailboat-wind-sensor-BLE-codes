#include <iostream>
#include "speedmapper.h"

Speedmapper::Speedmapper(struct map* _mappings, int _mapsize, bool _deep_copy) {
    deep_copy = _deep_copy;
    mapsize = _mapsize;

    if (deep_copy) {
        maps = new struct map[mapsize];
        memcpy(maps, _mappings, mapsize * sizeof(struct map));
    }
    else {
        maps = _mappings;
    }
}

Speedmapper::~Speedmapper() {
    if (deep_copy) {
        delete[] maps;
    }
}

float Speedmapper::Map(float rpm) {
    if (1 == mapsize) {
        maps[0].coeffitiant * rpm;
    }

    i = 1;
    while (i < mapsize && maps[i].limit < rpm) {
        i++;
    }

    return maps[i - 1].coeffitiant * rpm;
}
