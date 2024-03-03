//
// Created by ritwi on 3/2/2024.
//

#include "float_compare.h"
#include <cmath>

bool compare_float(float x, float y, float epsilon){
    if(std::fabs(x - y) < epsilon)
        return true; //they are same
    return false; //they are not same
}

void is_changed(
        float* current_pots,float* next_pots,float epsilon,bool* changed_pots,
        bool* current_stomps,bool* next_stomps,bool* changed_stomps
        ){
    for(int i=0;i<4;i++){
        changed_pots[i] = !compare_float(current_pots[i],next_pots[i],epsilon);
    }
    for(int i=0;i<2;i++){
        changed_stomps[i] = current_stomps[i]!=next_stomps[i];
    }
}
