#pragma once

class CyclicInteger {
public:
    CyclicInteger(int lower, int upper) : _lowerLimit(lower), _upperLimit(upper), _value(lower) {}

    CyclicInteger(int lower, int upper, int val) {
        _lowerLimit = lower;
        _upperLimit = upper;
        _value = cycle(val);
    }

    CyclicInteger& operator=(int rhs) {
        _value = cycle(rhs);
        return *this;
    }

    CyclicInteger& operator+=(int rhs) {
        _value = cycle(_value + rhs);
        return *this;
    }

    CyclicInteger& operator-=(int rhs) {
        _value = cycle(_value - rhs);
        return *this;
    }

    CyclicInteger operator+(int rhs) const {
        return CyclicInteger(_lowerLimit, _upperLimit, _value + rhs);
    }

    CyclicInteger operator-(int rhs) const {
        return CyclicInteger(_lowerLimit, _upperLimit, _value - rhs);
    }

    CyclicInteger& operator++() {
        _value = cycle(_value + 1);
        return *this;
    }

    CyclicInteger operator++(int) {
        CyclicInteger temp = *this;
        ++(*this);
        return temp;
    }

    CyclicInteger& operator--() {
        _value = cycle(_value - 1);
        return *this;
    }

    CyclicInteger operator--(int) {
        CyclicInteger temp = *this;
        --(*this);
        return temp;
    }

    operator int() const {
        return _value;
    }

private:
    int _value;
    int _lowerLimit;
    int _upperLimit;

    int cycle(int val) const {
        int range = _upperLimit - _lowerLimit + 1;
        while (val < _lowerLimit) val += range;
        while (val > _upperLimit) val -= range;
        return val;
    }
};