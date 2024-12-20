#include <iostream>
#include "fluid.h"

int main() {

    Fluid<double, float, Fast_fixed<32, 11>, 36, 84> fluid;
    //Fluid<Fast_fixed<43, 21>, Fast_fixed<34, 17>, Fixed<32, 7>, 36, 84> fluid;
    //fluid.run();
    read(fluid.field);
    fluid.run();
}