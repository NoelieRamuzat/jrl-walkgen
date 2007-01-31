// This is vxl/VNL/numeric_limits.h
#ifndef vnl_numeric_limits_h_
#define vnl_numeric_limits_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
/**
*  \file
*  \brief Standard limits for numeric datatypes

*
*    Implementation of the May 96 ANSI Draft Working Paper (DWP)
*    numeric_limits class.  Numbering in
*    the documentation below refers to section 18.2 of the DWP.
*
*  \author Andrew W. Fitzgibbon, Oxford RRG
*  \date   28 Aug 96
*
   \verbatim
    Modifications:
    LSB (Manchester) 23/3/01 Documentation tidied
     Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
   \endverbatim
*
*/


/* A nasty hack to make it compile on windows (which can't cope
 * with initialising static consts inline
 */
#ifdef STATIC_CONST_INIT
#  undef STATIC_CONST_INIT
#endif
#ifdef _WIN32
#  define STATIC_CONST_INIT(x) /* = x */
#else
#  define STATIC_CONST_INIT(x) = x
#endif




namespace VNL {

/** 18.2.1.3  Type float_round_style                     [lib.round.style].
*/

enum FloatRoundStyle {
  vnl_round_indeterminate       = -1,
  vnl_round_toward_zero         =  0,
  vnl_round_to_nearest          =  1,
  vnl_round_toward_infinity     =  2,
  vnl_round_toward_neg_infinity =  3
};

#ifdef infinity
# error
#endif

/** Standard limits for numeric datatypes.
* Implementation of the May 96 ANSI Draft Working Paper (DWP)
* numeric_limits class.  Numbering in
* the documentation below refers to section 18.2 of the DWP.
*
*/
template<class T>
class NumericLimits
{
 public:

/** Distingishes between scalar types, which have specialisations, and non-scalar types, which don't.
*/
  static const bool is_specialized;

/** Minimum finite value.
*  Equivalent to CHAR_MIN, SHRT_MIN, FLT_MIN, DBL_MIN, etc.
*
*  For  floating types with denormalization, returns the minimum positive
*   normalized value, denorm_min().
*
*  Meaningful for all specializations in which  is_bounded  ==  true,  or
*   is_bounded == false && is_signed == false.
*/
  static T min();

/** Maximum finite value.
*  Equivalent to CHAR_MAX, SHRT_MAX, FLT_MAX, DBL_MAX, etc.
*  Meaningful for all specializations in which is_bounded == true.
*/
  static T max();

/** Number of radix digits which can be represented without change.
*  For built-in integer types, the number of non-sign bits in the representation.
*  For floating point types, the number of radix digits in the mantissa.
*  Equivalent to FLT_MANT_DIG, DBL_MANT_DIG, LDBL_MANT_DIG.
*/
  static const int  digits;

/** Number of base 10 digits which can be represented without change.
*  Equivalent to FLT_DIG, DBL_DIG, LDBL_DIG.
*  Meaningful for all specializations in which is_bounded == true.
*/
  static const int  digits10;

/** True if the type is signed.
*/
  static const bool is_signed;

/** True if the type is integer.
*/
  static const bool is_integer;

/** True if the type uses an exact representation.
*  All integer types are exact, but not vice versa.
*  For example, rational and fixed-exponent
*  representations are exact but not integer.
*/
  static const bool is_exact;

/**
*  For floating types, specifies the base or radix of the exponent
*    representation (often 2).  Equivalent to FLT_RADIX.
*  For integer types, specifies the base of the representation -
*    distinguishes types with bases other than 2 (e.g. BCD).
*/
  static const int  radix;

/**  Machine epsilon.
*  The difference between 1 and the least value greater
*    than 1 that is representable.  Equivalent to FLT_EPSILON, DBL_EPSILON,
*    LDBL_EPSILON.
*  Meaningful only for floating point types.
*/
  static T epsilon();

/**  Measure of the maximum rounding error.
*  This has a precise definition in
*    the Language Independent Arithmetic (LIA-1) standard.  Required by LIA-1.
*/
  static T round_error();


/** Minimum negative integer such that radix raised to that power is in range.
*  Equivalent to FLT_MIN_EXP, DBL_MIN_EXP, LDBL_MIN_EXP.
*  Meaningful only for floating point types.
*/
  static const int  min_exponent;

/** Minimum negative integer such that 10 raised to that power is in range.
*  Equivalent to FLT_MIN_10_EXP, DBL_MIN_10_EXP, LDBL_MIN_10_EXP.
*  Meaningful only for floating point types.
*/
  static const int  min_exponent10;

/** Maximum positive integer such that radix raised to that power is in range.
*  Equivalent to FLT_MAX_EXP, DBL_MAX_EXP, LDBL_MAX_EXP.
*  Meaningful only for floating point types.
*/
  static const int  max_exponent;

/** Maximum positive integer such that 10 raised to that power is in range.
*  Equivalent to FLT_MAX_10_EXP, DBL_MAX_10_EXP, LDBL_MAX_10_EXP.
*  Meaningful only for floating point types.
*/
  static const int  max_exponent10;

/** True if the type has a representation for positive infinity.
*  Meaningful only for floating point types.
*  Shall be true for all specializations in which is_iec559 == true.
*/
  static const bool has_infinity;

/** True if the type has a representation for a quiet (non-signaling).
*  ``Not a Number.''.  RLIA
*  Meaningful only for floating point types.
*  Shall be true for all specializations in which is_iec559 == true.
*/
  static const bool has_quiet_NaN;

/** True if the type has a representation for a signaling.
*  ``Not a Number.''.
*  Meaningful only for floating point types.
*  Shall be true for all specializations in which is_iec559 == true.
*/
  static const bool has_signaling_NaN;

/** True if the type allows denormalized values (variable number of exponent bits).
*  Meaningful only for flotaing point types.
*/
  static const bool has_denorm;

/** Representation of positive infinity, if available.
*/
  static T infinity();

/** Representation of a quiet ``Not a Number,'' if available.
*/
  static T quiet_NaN();

/** Representation of a signaling ``Not a Number,'' if available.
*/
  static T signaling_NaN();

/** Minimum positive denormalized value.
*  Meaningful for all floating point types.
*  In specializations for which has_denorm == false, returns the  minimum
*    positive normalized value.
*  For types with has_denorm == false, the member denorm_min() shall
*    return the same value as the member min().
*/
  static T denorm_min();

/** True if and only if the type adheres to IEC 559 standard.
*  International Electrotechnical Commission standard 559 is the same as IEEE 754.
*/
  static const bool is_iec559;

/** True if the set of values representable by the type is finite.
*  All built-in types are bounded, this member would be false for arbitrary
*  precision types.
*/
  static const bool is_bounded;

/** True if the type is modulo.
*  A type is modulo if it is  possible to add two positive numbers and have
*  a result which wraps around to a third number which is less.
*  Generally, this is false for floating types, true for unsigned integers,
*  and true for signed integers on most machines.
*/
  static const bool is_modulo;

/** True if trapping is implemented for the type.
*/
  static const bool traps;

/** True if tinyness is detected before rounding.\ Refer to IEC 559.
*/
  static const bool tinyness_before;

/** The rounding style for the type.\ Equivalent to FLT_ROUNDS.
*  Specializations for integer types shall return round_toward_zero.
*/
  static const FloatRoundStyle round_style;
};

// SPECIALIZATIONS :

//-------------------- #include <VNL/numeric_limits_int.h>

#ifdef IUE_64_BIT
# define vnl_tmp_signed32 ????
#else
# define vnl_tmp_signed32 int
#endif

template <>
class NumericLimits<vnl_tmp_signed32>
{
 public:
  static const bool is_specialized STATIC_CONST_INIT(true);
  inline static int min() { return -0x7fffffff; }
  inline static int max() { return  0x7fffffff; }
  static const int digits   STATIC_CONST_INIT(31);
  static const int digits10 STATIC_CONST_INIT(9);
  static const bool is_signed  STATIC_CONST_INIT(true);
  static const bool is_integer STATIC_CONST_INIT(true);
  static const bool is_exact   STATIC_CONST_INIT(true);
  static const int radix STATIC_CONST_INIT(2);
  inline static int epsilon()     { return 0; }
  inline static int round_error() { return 0; }
  static const int min_exponent   STATIC_CONST_INIT(-31);
  static const int min_exponent10 STATIC_CONST_INIT(-9);
  static const int max_exponent   STATIC_CONST_INIT(31);
  static const int max_exponent10 STATIC_CONST_INIT(9);
  static const bool has_infinity      STATIC_CONST_INIT(false);
  static const bool has_quiet_NaN     STATIC_CONST_INIT(false);
  static const bool has_signaling_NaN STATIC_CONST_INIT(false);
  static const bool has_denorm        STATIC_CONST_INIT(false);
  static int infinity() { return max(); }
  static int quiet_NaN();
  static int signaling_NaN();
  inline static int denorm_min()    { return min(); }
  static const bool is_iec559  STATIC_CONST_INIT(false);
  static const bool is_bounded STATIC_CONST_INIT(true);
  static const bool is_modulo  STATIC_CONST_INIT(true);
  static const bool traps      STATIC_CONST_INIT(false);
  static const bool tinyness_before STATIC_CONST_INIT(false);
  static const FloatRoundStyle round_style STATIC_CONST_INIT(vnl_round_toward_zero);
};

#undef vnl_tmp_signed32


template <>
class NumericLimits<long>
{
 public:
  static const bool is_specialized STATIC_CONST_INIT(true);
  inline static int min() { return -0x7fffffff; }
  inline static int max() { return  0x7fffffff; }
  static const int digits   STATIC_CONST_INIT(31);
  static const int digits10 STATIC_CONST_INIT(9);
  static const bool is_signed  STATIC_CONST_INIT(true);
  static const bool is_integer STATIC_CONST_INIT(true);
  static const bool is_exact   STATIC_CONST_INIT(true);
  static const int radix       STATIC_CONST_INIT(2);
  inline static int epsilon()     { return 0; }
  inline static int round_error() { return 0; }
  static const int min_exponent   STATIC_CONST_INIT(-31);
  static const int min_exponent10 STATIC_CONST_INIT(-9);
  static const int max_exponent   STATIC_CONST_INIT(31);
  static const int max_exponent10 STATIC_CONST_INIT(9);
  static const bool has_infinity      STATIC_CONST_INIT(false);
  static const bool has_quiet_NaN     STATIC_CONST_INIT(false);
  static const bool has_signaling_NaN STATIC_CONST_INIT(false);
  static const bool has_denorm        STATIC_CONST_INIT(false);
  static int infinity() { return max(); }
  static int quiet_NaN();
  static int signaling_NaN();
  inline static int denorm_min()    { return min(); }
  static const bool is_iec559  STATIC_CONST_INIT(false);
  static const bool is_bounded STATIC_CONST_INIT(true);
  static const bool is_modulo  STATIC_CONST_INIT(true);
  static const bool traps      STATIC_CONST_INIT(false);
  static const bool tinyness_before STATIC_CONST_INIT(false);
  static const FloatRoundStyle round_style STATIC_CONST_INIT(vnl_round_toward_zero);
};


template <>
class NumericLimits<unsigned long>
{
 public:
  static const bool is_specialized STATIC_CONST_INIT(true);
  inline static unsigned int min() { return 0; }
  inline static unsigned int max() { return 0xffffffff; }
  static const int digits   STATIC_CONST_INIT(sizeof(unsigned long) * 8 );
  static const int digits10 STATIC_CONST_INIT( (digits * 301) / 1000 );
  static const bool is_signed  STATIC_CONST_INIT(false);
  static const bool is_integer STATIC_CONST_INIT(true);
  static const bool is_exact   STATIC_CONST_INIT(true);
  static const int radix STATIC_CONST_INIT(2);
  inline static int epsilon()     { return 0; }
  inline static int round_error() { return 0; }
  static const int min_exponent   STATIC_CONST_INIT(-31);
  static const int min_exponent10 STATIC_CONST_INIT(-9);
  static const int max_exponent   STATIC_CONST_INIT(31);
  static const int max_exponent10 STATIC_CONST_INIT(9);
  static const bool has_infinity      STATIC_CONST_INIT(false);
  static const bool has_quiet_NaN     STATIC_CONST_INIT(false);
  static const bool has_signaling_NaN STATIC_CONST_INIT(false);
  static const bool has_denorm        STATIC_CONST_INIT(false);
  static int infinity() { return max(); }
  static int quiet_NaN();
  static int signaling_NaN();
  inline static int denorm_min()    { return min(); }
  static const bool is_iec559  STATIC_CONST_INIT(false);
  static const bool is_bounded STATIC_CONST_INIT(true);
  static const bool is_modulo  STATIC_CONST_INIT(true);
  static const bool traps      STATIC_CONST_INIT(false);
  static const bool tinyness_before STATIC_CONST_INIT(false);
  static const FloatRoundStyle round_style STATIC_CONST_INIT(vnl_round_toward_zero);
};

template <>
class NumericLimits<unsigned short >
{
 public:
  static const bool is_specialized STATIC_CONST_INIT(true);
  inline static unsigned int min() { return 0; }
  inline static unsigned int max() { return 0xffff; }
  static const int digits   STATIC_CONST_INIT(sizeof(unsigned short) * 8 );
  static const int digits10 STATIC_CONST_INIT( (digits * 301) / 1000 );
  static const bool is_signed  STATIC_CONST_INIT(false);
  static const bool is_integer STATIC_CONST_INIT(true);
  static const bool is_exact   STATIC_CONST_INIT(true);
  static const int radix STATIC_CONST_INIT(2);
  inline static int epsilon()     { return 0; }
  inline static int round_error() { return 0; }
  static const int min_exponent   STATIC_CONST_INIT(-31);
  static const int min_exponent10 STATIC_CONST_INIT(-9);
  static const int max_exponent   STATIC_CONST_INIT(31);
  static const int max_exponent10 STATIC_CONST_INIT(9);
  static const bool has_infinity      STATIC_CONST_INIT(false);
  static const bool has_quiet_NaN     STATIC_CONST_INIT(false);
  static const bool has_signaling_NaN STATIC_CONST_INIT(false);
  static const bool has_denorm        STATIC_CONST_INIT(false);
  static int infinity() { return max(); }
  static int quiet_NaN();
  static int signaling_NaN();
  inline static int denorm_min()    { return min(); }
  static const bool is_iec559  STATIC_CONST_INIT(false);
  static const bool is_bounded STATIC_CONST_INIT(true);
  static const bool is_modulo  STATIC_CONST_INIT(true);
  static const bool traps      STATIC_CONST_INIT(false);
  static const bool tinyness_before STATIC_CONST_INIT(false);
  static const FloatRoundStyle round_style STATIC_CONST_INIT(vnl_round_toward_zero);
};

template <>
class NumericLimits<short >
{
 public:
  static const bool is_specialized STATIC_CONST_INIT(true);
  inline static int min() { return -0x7fff; }
  inline static int max() { return  0x7fff; }
  static const int digits   STATIC_CONST_INIT(15);
  static const int digits10 STATIC_CONST_INIT(5);
  static const bool is_signed  STATIC_CONST_INIT(true);
  static const bool is_integer STATIC_CONST_INIT(true);
  static const bool is_exact   STATIC_CONST_INIT(true);
  static const int radix STATIC_CONST_INIT(2);
  inline static int epsilon()     { return 0; }
  inline static int round_error() { return 0; }
  static const int min_exponent   STATIC_CONST_INIT(-15);
  static const int min_exponent10 STATIC_CONST_INIT(-5);
  static const int max_exponent   STATIC_CONST_INIT(15);
  static const int max_exponent10 STATIC_CONST_INIT(5);
  static const bool has_infinity      STATIC_CONST_INIT(false);
  static const bool has_quiet_NaN     STATIC_CONST_INIT(false);
  static const bool has_signaling_NaN STATIC_CONST_INIT(false);
  static const bool has_denorm        STATIC_CONST_INIT(false);
  static int infinity() { return max(); }
  static int quiet_NaN();
  static int signaling_NaN();
  inline static int denorm_min()    { return min(); }
  static const bool is_iec559  STATIC_CONST_INIT(false);
  static const bool is_bounded STATIC_CONST_INIT(true);
  static const bool is_modulo  STATIC_CONST_INIT(true);
  static const bool traps      STATIC_CONST_INIT(false);
  static const bool tinyness_before STATIC_CONST_INIT(false);
  static const FloatRoundStyle round_style STATIC_CONST_INIT(vnl_round_toward_zero);
};

//-------------------- #include <VNL/numeric_limits_float.h>

// IEEE 754 single precision
template <>
class NumericLimits<float>
{
 public:
  static const bool is_specialized STATIC_CONST_INIT(true);
  inline static float min() { return 1.17549435E-38F; }
  inline static float max() { return 3.40282347E+38F; }
  static const int digits   STATIC_CONST_INIT(24);
  static const int digits10 STATIC_CONST_INIT(6);
  static const bool is_signed  STATIC_CONST_INIT(true);
  static const bool is_integer STATIC_CONST_INIT(false);
  static const bool is_exact   STATIC_CONST_INIT(false);
  static const int radix STATIC_CONST_INIT(2);
  inline static float epsilon()     { return 1.19209290E-07F; }
  inline static float round_error() { return 0.5F; }
  static const int min_exponent   STATIC_CONST_INIT(-125);
  static const int min_exponent10 STATIC_CONST_INIT(-37);
  static const int max_exponent   STATIC_CONST_INIT(128);
  static const int max_exponent10 STATIC_CONST_INIT(38);
  static const bool has_infinity      STATIC_CONST_INIT(true);
  static const bool has_quiet_NaN     STATIC_CONST_INIT(true);
  static const bool has_signaling_NaN STATIC_CONST_INIT(true);
  static const bool has_denorm        STATIC_CONST_INIT(false);
  static float infinity();
  static float quiet_NaN();
  static float signaling_NaN();
  inline static float denorm_min()    { return min(); }
  static const bool is_iec559  STATIC_CONST_INIT(true);
  static const bool is_bounded STATIC_CONST_INIT(true);
  static const bool is_modulo  STATIC_CONST_INIT(false);
  static const bool traps      STATIC_CONST_INIT(true);
  static const bool tinyness_before STATIC_CONST_INIT(true);
  static const FloatRoundStyle round_style STATIC_CONST_INIT(vnl_round_to_nearest);
};

//-------------------- #include <VNL/numeric_limits_double.h>

// IEEE 754 double precision with denorm
template <>
class NumericLimits<double>
{
 public:
  static const bool is_specialized STATIC_CONST_INIT(true);
  inline static double min() { return 2.2250738585072014e-308; }
  inline static double max() { return 1.7976931348623157e+308; }
  static const int digits   STATIC_CONST_INIT(53);
  static const int digits10 STATIC_CONST_INIT(15);
  static const bool is_signed  STATIC_CONST_INIT(true);
  static const bool is_integer STATIC_CONST_INIT(false);
  static const bool is_exact   STATIC_CONST_INIT(false);
  static const int radix STATIC_CONST_INIT(2);
  inline static double epsilon()     { return 2.220446049250313e-16; }
  inline static double round_error() { return 0.5; }
  static const int min_exponent   STATIC_CONST_INIT(-1021);
  static const int min_exponent10 STATIC_CONST_INIT(-307);
  static const int max_exponent   STATIC_CONST_INIT(1024);
  static const int max_exponent10 STATIC_CONST_INIT(308);
  static const bool has_infinity      STATIC_CONST_INIT(true);
  static const bool has_quiet_NaN     STATIC_CONST_INIT(true);
  static const bool has_signaling_NaN STATIC_CONST_INIT(true);
  static const bool has_denorm        STATIC_CONST_INIT(false);
  static double infinity();
  static double quiet_NaN();
  static double signaling_NaN();
  inline static double denorm_min() { return /* 5e-324 */ min(); }
  static const bool is_iec559  STATIC_CONST_INIT(true);
  static const bool is_bounded STATIC_CONST_INIT(true);
  static const bool is_modulo  STATIC_CONST_INIT(false);
  static const bool traps      STATIC_CONST_INIT(true);
  static const bool tinyness_before STATIC_CONST_INIT(true);
  static const FloatRoundStyle round_style STATIC_CONST_INIT(vnl_round_to_nearest);
};

}; // End namespace VNL

#endif // NumericLimits_h_