

#ifndef _PID_HEADER_H_
#define _PID_HEADER_H_

#include <math.h>               // for fabs()
#include <array>

#include <kernel/aux/filter.hpp>

/**
 * PID controller class.
 * FilterClass is of type Filter1D 
 */
template<typename FilterClass=Filter1D<Filter1D_EMA, 1, float>, typename T=float>
class PID {
public:

    PID(const std::array<T, 3>& pid_coeff = {{0, 0, 0}},
        const T& initial_imax = 100.0,
        const FilterClass& d_filter=Filter1D<Filter1D_EMA, 1, T>(0.7f)):
        _integrator(0),
        _last_input(0),
        _last_derivative(0),
		_d_filter(d_filter)
    {

        _kp = pid_coeff[0];
        _ki = pid_coeff[1];
        _kd = pid_coeff[2];
        _imax = abs(initial_imax);

		// derivative is invalid on startup
		_last_derivative = NAN;
		
    }

    /// Iterate the PID, return the new control value
    ///
    /// Positive error produces positive output.
    ///
    /// @param error	The measured error value: desired - current value
    /// @param dt		The time delta in milliseconds (note
    ///					that update interval cannot be more
    ///					than 65.535 seconds due to limited range
    ///					of the data type).
    /// @param scaler	An arbitrary scale factor
    ///
    /// @returns		The updated control output.
    ///
    inline T       get_pid(T error, T dt);
    inline T       get_pi(T error, T dt);
    inline T       get_p(T error) const;
    inline T       get_i(T error, T dt);
    inline T       get_d(T error, T dt);

    /// Reset the PID integrator
    ///
    void        reset_I();

    
    /// @name	parameter accessors
    //@{

    // accessors
    T           kP() const { return _kp; }
    T           kI() const { return _ki; }
    T           kD() const { return _kd; }
    T           imax() const { return _imax; }
    void        kP(const T v) { _kp = v; }
    void        kI(const T v) { _ki = v; }
    void        kD(const T v) { _kd = v; }
    void        imax(const T v) { _imax = abs(v); }
    T           get_integrator() const { return _integrator; }
    void        set_integrator(T i) { _integrator = i; }
    
    FilterClass& d_filter() { return _d_filter; }

protected:
    T        _kp;
    T        _ki;
    T        _kd;
    T        _imax;

    T           _integrator;                                ///< integrator value
    T           _last_input;                                ///< last input for derivative
    T           _last_derivative;                           ///< last derivative for low-pass filter
    
    FilterClass _d_filter;                                  ///< derivative lowpass filter
};




template<typename FilterClass, typename T>
inline T PID<FilterClass, T>::get_p(T error) const
{
    return (T)error * _kp;
}

template<typename FilterClass, typename T>
inline T PID<FilterClass, T>::get_i(T error, T dt)
{
    if((_ki != 0) && (dt != 0)) {
        _integrator += ((T)error * _ki) * dt;
        if (_integrator < -_imax) {
            _integrator = -_imax;
        } else if (_integrator > _imax) {
            _integrator = _imax;
        }
        return _integrator;
    }
    return 0;
}

template<typename FilterClass, typename T>
inline T PID<FilterClass, T>::get_d(T input, T dt)
{
    if ((_kd != 0) && (dt != 0)) {
        T derivative;
		if (std::isnan(_last_derivative)) {
			// we've just done a reset, suppress the first derivative
			// term as we don't want a sudden change in input to cause
			// a large D output change			
			derivative = 0;
			_last_derivative = 0;
		} else {
			// calculate instantaneous derivative
			derivative = (input - _last_input) / dt;
		}

        // discrete low pass filter, cuts out the
        // high frequency noise that can drive the controller crazy
		derivative = _d_filter.nextValue(derivative);

        // update state
        _last_input             = input;
        _last_derivative    = derivative;

        // add in derivative component
        return _kd * derivative;
    }
    return 0;
}

template<typename FilterClass, typename T>
inline T PID<FilterClass, T>::get_pi(T error, T dt)
{
    return get_p(error) + get_i(error, dt);
}


template<typename FilterClass, typename T>
inline T PID<FilterClass, T>::get_pid(T error, T dt)
{
    return get_p(error) + get_i(error, dt) + get_d(error, dt);
}

template<typename FilterClass, typename T>
inline void PID<FilterClass, T>::reset_I()
{
    _integrator = 0;
	// mark derivative as invalid
    _last_derivative = NAN;
}


#endif /* _PID_HEADER_H_ */
