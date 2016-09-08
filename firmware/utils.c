

void fill_rolling_array(s32 array[], u8 index_current, u8 index_max,  s32 value) {
    u8 index = index_current + 1;
    if (index > index_max) {index = 0;}
    array[index] = value;
}

s32 get_nLastSamples_sum(s32 array[], u8 index_current, u8 index_max, u8 n) {
    u8 i = 0;
    u8 i_ = 0;
    s32 sum = 0;
    for(i=0;i<n;i++) {
        if(index_current - i < 0) {i_ = (index_max + 1) - i;}
        else {i_ = index_current - i;}
        sum += array[i_];
    }
    return sum;
}