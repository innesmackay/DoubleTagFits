#ifndef INPUTS_H
#define INPUTS_H

#include <map>

namespace Inputs {

    inline std::map<int, double> ci = { {8, 0.411}, {7, 0.057}, {6, -0.578}, {5, -0.955}, {4, -0.608}, {3, 0.001}, {2, 0.643}, {1, 0.699} };
    inline std::map<int, double> si = { {8, 0.433}, {7, 0.854}, {6, 0.545}, {5, 0.032}, {4, -0.660}, {3, -1.000}, {2, -0.300}, {1, -0.091} };
    inline std::map<int, double> Ki = { {-8, 0.134}, {-7, 0.125}, {-6, 0.058}, {-5, 0.085}, {-4, 0.024}, {-3, 0.067}, {-2, 0.087}, {-1, 0.176}, {1, 0.083}, {2, 0.019}, {3, 0.021}, {4, 0.0160}, {5, 0.053}, {6, 0.012}, {7, 0.012}, {8, 0.028} };
    const double YMIX = 0.00636;
    const double FPLUS = 0.9406;
    const double XMIX = 0.00398;
    const double RKPI = 0.05865;
    const double RKPI_SQUARED = 0.05865*0.05865;
}

#endif //  Inputs_H



