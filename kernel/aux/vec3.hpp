#ifndef MATH_VEC3_H
#define MATH_VEC3_H

#include <iostream>
#include <cmath>

namespace Math
{

template <typename T> class Vec3;
typedef Vec3<int>           Vec3i;
typedef Vec3<unsigned>      Vec3u;
typedef Vec3<float>         Vec3f;
typedef Vec3<double>        Vec3d;
typedef Vec3<short>         Vec3s;
typedef Vec3<char>          Vec3c;
typedef Vec3<unsigned char> Vec3uc;


const float FEQ_EPS  = 1e-5f;
const float FEQ_EPS2 = 1e-12f;
const float FEQ_INF  = 1e12f;


//! Compare two numbers and test if they are "approximately equal".
/*!
    \returns true iff x1 is the same as x2 with an absolute error of
             no more than e,

                abs(x1 - x2) <= e
*/
template <typename T>
inline bool
equalWithAbsError(T x1, T x2, T e)
{
    return ((x1 > x2) ? x1 - x2 : x2 - x1) <= e;
}


//! Compare two numbers and test if they are "approximately equal".
/*!
    \returns true iff x1 is the same as x2 with an relative error of
             no more than e,

                abs(x1 - x2) <= e * x1
*/
template <typename T>
inline bool
equalWithRelError(T x1, T x2, T e)
{
    return ((x1 > x2) ? x1 - x2: x2 - x1) <= e * ((x1 > 0) ? x1: -x1);
}


//! Templated versions of standard math routines
/*!
    The Math class provides the ability to choose the appropriate precision
    math routine based on a template parameter.
*/
template <typename T>
struct MathT
{
    static T acos (T x)      {return ::acos(double(x));}
    static T asin (T x)      {return ::asin(double(x));}
    static T atan (T x)      {return ::atan(double(x));}
    static T atan2(T x, T y) {return ::atan2(double(x), double(y));}
    static T cos  (T x)      {return ::cos(double(x));}
    static T sin  (T x)      {return ::sin(double(x));}
    static T tan  (T x)      {return ::tan(double(x));}
    static T exp  (T x)      {return ::exp(double(x));}
    static T exp2 (T x)      {return ::exp2(double(x));}
    static T expm1(T x)      {return ::expm1(double(x));}
    static T log  (T x)      {return ::log(double(x));}
    static T log10(T x)      {return ::log10(double(x));}
    static T log1p(T x)      {return ::log1p(double(x));}
    static T log2 (T x)      {return ::log2(double(x));}
    static T modf (T x, T *iptr)
    {
        double ival;
        T rval(::modf(double(x), &ival));
        *iptr = ival;
        return rval;
    }
    static T pow  (T x, T y) {return ::pow(double(x), double(y));}
    static T cbrt (T x)      {return ::cbrt(double(x));}
    static T sqrt (T x)      {return ::sqrt(double(x));}
    static T sqrt2(T x)      {return x > 0 ? ::sqrt((double)x) : 0;}
    static T sqr(T x)        {return x*x;}
    static T fabs (T x)      {return ::fabs(double(x));}
    static T ceil (T x)      {return ::ceil(double(x));}
    static T floor(T x)      {return ::floor(double(x));}
    static T fmod (T x, T y) {return ::fmod(double(x), double(y));}
    static T hypot(T x, T y) {return ::hypot(double(x), double(y));}
};


//! Specialization for single-precision math routines.
template <>
struct MathT<float>
{
    static float acos (float x)           {return ::acosf(x);}	
    static float asin (float x)           {return ::asinf(x);}
    static float atan (float x)           {return ::atanf(x);}
    static float atan2(float x, float y)  {return ::atan2f(x, y);}
    static float cos  (float x)           {return ::cosf(x);}
    static float sin  (float x)           {return ::sinf(x);}
    static float tan  (float x)           {return ::tanf(x);}
    static float exp  (float x)           {return ::expf(x);}
    static float exp2 (float x)           {return ::exp2f(x);}
    static float expm1(float x)           {return ::expm1f(x);}
    static float log  (float x)           {return ::logf(x);}
    static float log10(float x)           {return ::log10f(x);}
    static float log1p(float x)           {return ::log1pf(x);}
    static float log2 (float x)           {return ::log2f(x);}
    static float modf (float x, float *y) {return ::modff(x, y);}
    static float pow  (float x, float y)  {return ::powf(x, y);}
    static float sqrt (float x)           {return ::sqrtf(x);}
    static float sqrt2(float x)           {return x > 0 ? ::sqrtf(x) : 0;}
    static float cbrt (float x)           {return ::cbrtf(x);}
    static float ceil (float x)           {return ::ceilf(x);}
    static float fabs (float x)           {return ::fabsf(x);}
    static float floor(float x)           {return ::floorf(x);}
    static float fmod (float x, float y)  {return ::fmodf(x, y);}
    //static float hypot(float x, float y)  {return ::hypotf(x, y);}
};



//! A general 3D vector class.
/*!
    This class handles storing and manipulating 3D vectors.
*/
template <typename T>
class Vec3
{
public:
    T x, y, z;
    
    //-----------------------------------------------------------------------
    //! \name Constructors and assignment
	//@{
    //-----------------------------------------------------------------------
    //! Default constructor.
    /*!
        Call this constructor if you do not wish to initialize the vector to
        anything. It is more efficient than the others since it does nothing.
    */
    Vec3() {}

    //! Parameter constructor.
    /*!
        Initialize the vector with the three parameters.
        \param a Value to set the x component to.
        \param b Value to set the y component to.
        \param c Value to set the z component to.
    */
    Vec3(T a, T b, T c) : x(a), y(b), z(c) {}

    //! Parameter constructor.
    /*!
        Initialize each component to \a a.
        \param a The value to initialize to.
    */
    explicit Vec3(T a) : x(a), y(a), z(a) {}
    
    //! Parameter constructor.
    /*!
        Initialize the vector from an array of doubles
        \param v The array of doubles to initialize from.
    */
    template <typename S>
    Vec3(const S* v) : x(T(v[0])), y(T(v[1])), z(T(v[2])) {}
    
    //! Copy constructor.
    /*!
        Initialize the vector to be a copy of \a v.
        \param v The vector to create a copy of.
    */
    template <typename S>
    Vec3(const Vec3<S>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)) {}

    //! Assignment operator.
    /*!
        Assigns the values from \a a to this Vec3.
    */
    const Vec3 & operator=(const Vec3& a) {x = a.x; y = a.y; z = a.z; return *this;}
    
    //! Assignment operator.
    /*!
        Sets all components of this Vec3 to \a a.
    */
    const Vec3 & operator=(T a) {x = y = z = a; return *this;}
    
    //! Assignment operator from C array
    template <typename S>
    const Vec3 & operator=(const S* v) {x = T(v[0]); y = T(v[1]); z = T(v[2]);; return *this;}
    //@}
	
	
	
    //-----------------------------------------------------------------------
	//! \name Casting operators
    //@{
    //-----------------------------------------------------------------------

	//! Constant casting operator.
    /*!
        Casts this constant Vec3 to an array as a const T pointer.
    */
    const T* toArray() const {return (const T*)&x;}
    
    //! Casting operator.
    /*!
        Casts this Vec3 to an array as a T pointer.
    */
    T* toArray() {return (T*)&x;}
    //@}

	
    //-----------------------------------------------------------------------
	//! \name Element access and manipulation.
    //@{
    //-----------------------------------------------------------------------
    //! Access operator.        
    /*!
        Returns the ith component of the vector.
        \param i The component to return.
        \warning i must be either 0, 1, or 2 in order to get expected results.
    */
    T & operator[](int i) {return (&x)[i];}
    
    //! Constant access operator.
    /*!
        Returns the ith component of a constant vector.
        \param i The component to return.
        \warning i must be either 0, 1, or 2 in order to get expected results.
    */
    const T & operator[](int i) const {return (&x)[i];}

    void set(T a) {x = y = z = a;}
    void set(T a, T b, T c) {x = a; y = b; z = c;}
    void set(const Vec3 v) {x = v.x; y = v.y; z = v.z;}
    template <typename S>
    void set(const Vec3<S>& v) {x = T(v.x); y = T(v.y); z = T(v.z);}
    //@}


    //-----------------------------------------------------------------------
    //! \name Addition.
	//@{
    //-----------------------------------------------------------------------
    //! Component-wise vector addition operator.
    Vec3 operator+(const Vec3& v) const
    {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }
    
    //! Component-wise vector addition-assignment operator.
    const Vec3 & operator+=(const Vec3& v)
    {
        x += v.x; y += v.y; z += v.z; return *this;
    }

    //! Scalar addition-assignment operator.
    const Vec3 & operator+=(T a) {x += a; y += a; z += a; return *this;}
    //@}


    //-----------------------------------------------------------------------
    //! \name Subtraction.
	//@{
    //-----------------------------------------------------------------------
    //! Component-wise vector subtraction operator.
    Vec3 operator-(const Vec3& v) const
    {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }
    
    //! Component-wise vector subtraction-assignment operator.
    const Vec3 & operator-=(const Vec3& v)
    {
        x -= v.x; y -= v.y; z -= v.z; return *this;
    }
    
    //! Component-wise scalar subtraction assignment operator.
    const Vec3 & operator-=(T a) {x -= a; y -= a; z -= a; return *this;}
    //@}


    //-----------------------------------------------------------------------
    //! \name Multiplication.
	//@{
    //-----------------------------------------------------------------------
    //! Scalar multiplication operator.
    Vec3 operator*(T a) const {return Vec3(x * a, y * a, z * a);}
    
    //! Component-wise vector multiplication operator.
    Vec3 operator*(const Vec3& v) const
    {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }
    
    //! Scalar multiplication-assignment operator.
    const Vec3 & operator*=(T a) {x *= a; y *= a; z *= a; return *this;}
    
    //! Component-wise vector multiplication-assignment operator.
    const Vec3 & operator*=(const Vec3& v)
    {
        x *= v.x; y *= v.y; z *= v.z; return *this;
    }
    
    //! Negation operator.
    Vec3 operator-() const {return Vec3(-x, -y, -z);}
    const Vec3 & negate() {x = -x; y = -y; z = -z; return *this;}
    //@}


    //-----------------------------------------------------------------------
    //! \name Division.
	//@{
    //-----------------------------------------------------------------------
    //! Scalar division operator.
    Vec3 operator/(T a) const {return (*this) * (T(1) / a);}
    
    //! Component-wise vector division operator.
    Vec3 operator/(const Vec3 & v) const
    {
        return Vec3(x / v.x, y / v.y, z / v.z);
    }
    
    //! Scalar division-assignment operator.
    const Vec3 & operator/=(T a) {*this *= T(1) / a; return *this;}
    
    //! Component-wise vector division-assignment operator.
    const Vec3 & operator/=(const Vec3 & v)
    {
        x /= v.x; y /= v.y; z /= v.z; return *this;
    }
    //@}


    //-----------------------------------------------------------------------
    //! \name Equality.
	//@{
    //-----------------------------------------------------------------------
    //! Vector equivalence operator.
    /*!
        Tests to see if each component of \a v is equal to each component of
        this Vec3.
    */
    bool operator==(const Vec3 & v) const
    {
        return(v.x == x && v.y == y && v.z == z);
    }
    
    //! Vector difference operator.
    /*!
        Tests to see if any component is different between the two Vec3s.
    */
    bool operator!=(const Vec3 & v) const
    {
        return(v.x != x || v.y != y || v.z != z);
    }
    
    //! Compare two vectors and test if they are "approximately equal".
    /*!
        \returns true iff the coefficients of this and v are the same with
                 an absolute error of no more than e, i.e., for all i
        
                    abs(this[i] - v[i]) <= e
    */
    bool equalWithAbsError(const Vec3 &v, T e) const
    {
        for (int i = 0; i < 3; i++)
            if (!Math::equalWithAbsError((*this)[i], v[i], e))
                return false;
        return true;
    }
    
    //! Compare two vectors and test if they are "approximately equal".
    /*!
        \returns true iff the coefficients of this and v are the same with
                 an absolute error of no more than e, i.e., for all i
        
                    abs(this[i] - v[i]) <= e * abs(this[i])
    */
    bool equalWithRelError(const Vec3 &v, T e) const
    {
        for (int i = 0; i < 3; i++)
            if (!Math::equalWithRelError((*this)[i], v[i], e))
                return false;
        return true;
    }
    //@}


    //-----------------------------------------------------------------------
    //! \name Length, normalization, etc.
	//@{
    //-----------------------------------------------------------------------
    //! Length<sup>2</sup>.
    /*!
        Returns the geometric length<sup>2</sup> of the vector.
    */
    T length2() const {return dot(*this, *this);}
    
    //! Length.
    /*!
        Returns the geometric length of the vector.
    */
    T length() const {return MathT<T>::sqrt(length2());}
    
    //! Normalizes the vector and return its length.
    /*!
        Scales each component of the vector in order to get unit
        length without changing direction.
    
        \return The length of the vector prior to normalization.
    */
    T unitize()
    {
        T l = length();
        *this /= l;
        return l;
    }
    
    //! Normalize a vector and return a reference to it.
    /*!
        Scales each component of the vector in order to get unit
        length without changing direction.
    
        \return A reference to the vector.
    */
    const Vec3 & normalize()
    {
        return(*this /= length());
    }
    
    //! Return a normalized copy of the vector.
    Vec3 normalized() const
    {
        return(*this / length());
    }
    
    //! Rotate this vector about another vector, w, by theta radians.
    const Vec3 & rotate(T theta, const Vec3 & w);
    
    //! Return a rotated copy of the vector
    Vec3 rotated(T theta, const Vec3 & w) const;
    
    //@}

    static unsigned dimensions() {return 3;}
    typedef T    BaseType;
    
    T max() const { return std::max(x, std::max(y, z)); }
    T min() const { return std::min(x, std::min(y, z)); }
};


//! Multiply a scalar by a Vec3.
template <typename S, typename T>
inline Vec3<T>
operator*(S s, const Vec3<T>& v)
{
    return Vec3<T>(T(v.x * s), T(v.y * s), T(v.z * s));
}


//! Add a Vec3 to a scalar.
template <typename S, typename T>
inline Vec3<T>
operator+(S s, const Vec3<T>& v)
{
    return Vec3<T>(s + v.x, s + v.y, s + v.z);
}


//! Subtract a Vec3 from a scalar.
template <typename S, typename T>
inline Vec3<T>
operator-(S s, const Vec3<T>& v)
{
    return Vec3<T>(s - v.x, s - v.y, s - v.z);
}


//! The dot product of two Vec3s.
template <typename T>
inline T
dot(const Vec3<T>& a, const Vec3<T>& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}


//! The cross product of two Vec3s.
template <typename T>
inline Vec3<T>
cross(const Vec3<T>& a, const Vec3<T>& b)
{
    return Vec3<T>(a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x);
}


//! Rotate this vector about another vector, w, by theta radians.
template <typename T>
inline const Vec3<T> &
Vec3<T>::rotate(T theta, const Vec3 & w)
{
    T c = MathT<T>::cos(theta);
    T s = MathT<T>::sin(theta);

    Vec3 v0 = dot(*this, w) * w;
    Vec3 v1 = *this - v0;
    Vec3 v2 = cross(w, v1);

    return(*this = v0 + c * v1 + s * v2);
}


//! Return a rotated copy of the vector
template <typename T>
inline Vec3<T>
Vec3<T>::rotated(T theta, const Vec3 & w) const
{
    T c = MathT<T>::cos(theta);
    T s = MathT<T>::sin(theta);

    Vec3 v0 = dot(*this, w) * w;
    Vec3 v1 = *this - v0;
    Vec3 v2 = cross(w, v1);

    return v0 + c * v1 + s * v2;
}


//! Output to a stream.
/*!
    Writes \a a to the output stream \a out, and returns the modified stream.
    \return A copy of \a out with the \a a written to it.
    \sa operator>>(std::istream&, Vec3&)
    \relates Vec3
*/
template <typename T>
inline std::ostream &
operator<<(std::ostream& out, const Vec3<T>& v)
{
    return out << v.x << " " << v.y << " " << v.z ;
}


//! Input from a stream.
/*!
    Reads \a a from the input stream \a in, and returns the modified stream.
    \return A copy of \a in with the \a a already read from it.
    \sa operator<<(std::ostream&, const Vec3&)
    \relates Vec3
*/
template <typename T>
inline std::istream &
operator>>(std::istream& in, Vec3<T>& v)
{
    return in >> v.x >> v.y >> v.z ;
}


template <typename T>
inline Vec3<T>
clamp(const Vec3<T>& v, T l, T h)
{
    return Vec3<T>(clamp(v.x, l, h), clamp(v.y, l, h), clamp(v.z, l, h));
}


} // namespace Math

#endif // MATH_VEC3_H
