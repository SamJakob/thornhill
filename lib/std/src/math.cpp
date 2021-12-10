#include <math>

namespace std {

    long long int ceilToN(long double value, long long int multiple) {
        long long int roundedValue = ceil(value);

        bool isPositive = roundedValue >= 0;
        if (!isPositive)
            roundedValue = -roundedValue;

        long long int result;

        if ((multiple & (multiple - 1)) == 0) {
            result = (roundedValue + (multiple - 1)) & -multiple;
            return isPositive ? result : -result;
        }

        result = ((roundedValue + (multiple - 1)) / multiple) * multiple;
        return isPositive ? result : -result;
    }

}