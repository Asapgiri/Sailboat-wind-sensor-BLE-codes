#include "filtering.h"

/////////////////////////////////////////////////////////////////////////////////////
/// Filters
/////////////////////////////////////////////////////////////////////////////////////

template <class T>
T AverageFilter(const T* buffer, uint64_t window_size, uint64_t index) {
    T sum = 0;
    uint32_t i;

    for (i = 0; i < window_size; i++) {
        sum += buffer[i];
    }

    return sum / window_size;
}


/////////////////////////////////////////////////////////////////////////////////////
/// Filtering class functions
/////////////////////////////////////////////////////////////////////////////////////

//Filtered<T>& Filtered<T>::operator+=(T& opt) {
template <class T>
void Filtered<T>::Add(T opt) {
    filter_buffer[__index] = opt;
    __index++;

    if (__index >= FILTER_WINDOW_SIZE) {
        __index = 0;
    }

    filtered_value = __filter(filter_buffer, FILTER_WINDOW_SIZE, __index);
}

template <class T>
T Filtered<T>::GetFilteredValut() {
    return filtered_value;
}

template <class T>
Filtered<T>::Filtered() {
    __filter = &AverageFilter<T>;
}

template <class T>
void Filtered<T>::SetFilterer(T (*filterer)(const T* buffer, uint64_t window_size, uint64_t index)) {
    if (nullptr == filterer) {
        __filter = &AverageFilter<T>;
    }
    else {
        __filter = filterer;
    }
}
