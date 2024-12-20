#include <iostream>
#include "fluid.h"

int main() {

    //Fluid<Fixed<32, 8>, Fast_fixed<32, 8>, Fixed<32, 8>, 36, 84> fluid;
    Fluid<float, float, float, 36, 84> fluid;
    //fluid.run();
    read(fluid.field);
    fluid.run();
}