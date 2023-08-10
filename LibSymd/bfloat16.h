#pragma once


namespace symd
{
    struct bfloat16
    {
    private:
        short data;

        short construct(float x)
        {
            return ((short*)(&x))[1];
        }

    public:        

        bfloat16()
        {
        }

        bfloat16(float x)
        {
            data = construct(x);
        }

        operator float() const
        {
            float res = 0.0f;
            ((short*)(&res))[1] = data;
            return res;
        }

        bfloat16 operator+(const bfloat16& other) const
        {
            return bfloat16((float)(*this) + (float)other);
        }

        bfloat16 operator-(const bfloat16& other) const
        {
            return bfloat16((float)(*this) - (float)other);
        }

        bfloat16 operator*(const bfloat16& other) const
        {
            return bfloat16((float)(*this) * (float)other);
        }

        bfloat16 operator/(const bfloat16& other) const
        {
            return bfloat16((float)(*this) / (float)other);
        }
    };

    bfloat16& operator+=(bfloat16& first, const bfloat16& sec)
    {
        first = first + sec;
        return first;
    }

    bfloat16& operator-=(bfloat16& first, const bfloat16& sec)
    {
        first = first - sec;
        return first;
    }

    bfloat16& operator*=(bfloat16& first, const bfloat16& sec)
    {
        first = first * sec;
        return first;
    }

    bfloat16& operator/=(bfloat16& first, const bfloat16& sec)
    {
        first = first * sec;
        return first;
    }
}

namespace std
{
    inline symd::bfloat16 abs(const symd::bfloat16& x)
    {
        float fabs_result = fabs((float)x);
        return (symd::bfloat16)fabs_result;
    }
}
