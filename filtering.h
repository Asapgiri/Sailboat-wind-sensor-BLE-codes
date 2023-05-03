#ifndef WS_FILTERING
#define WS_FILTERING

#include <Arduino.h>
#include "ws_config.h"

template <class T>
T AverageFilter(const T* buffer, uint64_t window_size, uint64_t index);

template <class T>
class Filtered {
private:
    T        filter_buffer[FILTER_WINDOW_SIZE];
    uint64_t __index = 0;
    T        filtered_value = 0;
    T        (*__filter)(const T* buffer, uint64_t window_size, uint64_t index);

public:
    Filtered();
    void SetFilterer(T (*filterer)(const T* buffer, uint64_t window_size, uint64_t index));
    //Filtered<T>& operator += (T& opt);
    void Add(T opt);
    T GetFilteredValut();

};


#endif