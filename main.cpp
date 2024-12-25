#include <iostream>
#include "precalc.h"
//#include "fluid.h"

int main() {

    Fluid<Fixed<32, 15>, double, Fast_fixed<32, 11>, 36, 84> fluid;
    //Fluid<Fast_fixed<43, 21>, Fast_fixed<34, 17>, Fixed<32, 7>, 36, 84> fluid;
    read(fluid.field);
    fluid.run();
}