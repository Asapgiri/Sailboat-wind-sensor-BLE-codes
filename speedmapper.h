#ifndef WS_SPEEDMAPPER
#define WS_SPEEDMAPPER

#include <Arduino.h>

/// Should be something like:
/// 
/// rpm  coeff
/// 0    1.0f
/// 10   1.1f
/// 15   1.2f
/// :
/// :
/// .

struct map {
    float limit;
    float coeffitiant;
};


class Speedmapper {
private:
    struct map* maps;
    bool deep_copy;
    int  mapsize;
    int  i;

public:
    Speedmapper(struct map* _mappings, int _mapsize, bool _deep_copy = true);
    ~Speedmapper();
    float Map(float rpm);
};


#endif