#pragma once

class ClampedInteger {
public:
    ClampedInteger(int lower, int upper) : _lowerLimit(lower), _upperLimit(upper), _value(lower) {}

    ClampedInteger(int lower, int upper, int val) {
        _lowerLimit = lower;
        _upperLimit = upper;
        _value = clamp(val);
    }

    ClampedInteger& operator=(int rhs) {
        _value = clamp(rhs);
        return *this;
    }

    ClampedInteger& operator+=(int rhs) {
        _value = clamp(_value + rhs);
        return *this;
    }

    ClampedInteger& operator-=(int rhs) {
        _value = clamp(_value - rhs);
        return *this;
    }

    ClampedInteger operator+(int rhs) {
        return ClampedInteger(_lowerLimit, _upperLimit, _value + rhs);
    }

    ClampedInteger operator-(int rhs) {
        return ClampedInteger(_lowerLimit, _upperLimit, _value - rhs);
    }

    ClampedInteger& operator++() {
        _value = clamp(_value + 1);
        return *this;
    }

    ClampedInteger operator++(int) {
        ClampedInteger temp = *this;
        ++(*this);
        return temp;
    }

    ClampedInteger& operator--() {
        _value = clamp(_value - 1);
        return *this;
    }

    ClampedInteger operator--(int) {
        ClampedInteger temp = *this;
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

    inline int clamp(int val) const {
        if (val < _lowerLimit) return _lowerLimit;
        if (val > _upperLimit) return _upperLimit;
        return val;
    }
};