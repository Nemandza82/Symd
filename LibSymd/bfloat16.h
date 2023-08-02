#pragma once


namespace symd
{
    struct bfloat16
    {
    private:
        short data;

    public:        

        bfloat16()
        {
        }

        bfloat16(float x)
        {
            data = ((short*)(&x))[1];
        }

        operator float() const
        {
            float res = 0.0f;
            ((short*)(&res))[1] = data;
            return res;
        }

        bfloat16 operator+(const bfloat16& other) const
        {
            return (float)(*this) + (float)other;
        }

        bfloat16 operator-(const bfloat16& other) const
        {
            return (float)(*this) - (float)other;
        }

        bfloat16 operator*(const bfloat16& other) const
        {
            return (float)(*this) * (float)other;
        }

        bfloat16 operator/(const bfloat16& other) const
        {
            return (float)(*this) / (float)other;
        }
    };
}
