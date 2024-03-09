//
// Created by ritwi on 3/2/2024.
//

#ifndef PEDALBOARD_FLOAT_COMPARE_H
#define PEDALBOARD_FLOAT_COMPARE_H
bool compare_float(float x, float y, float epsilon);
float round_to_precision(float x,int num_decimal_precision_digits);
void is_changed(
        float* current_pots,float* next_pots,float epsilon,bool* changed_pots,
        bool* current_stomps,bool* next_stomps,bool* changed_stomps
);
#endif //PEDALBOARD_FLOAT_COMPARE_H
