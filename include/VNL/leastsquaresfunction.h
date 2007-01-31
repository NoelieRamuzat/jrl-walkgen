// This is vxl/VNL/least_squares_function.h
#ifndef vnl_least_squares_function_h_
#define vnl_least_squares_function_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
/**
* \file
* \brief Abstract base for minimising functions

* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   31 Aug 96
*
   \verbatim
   Modifications:
    280697 AWF Changed return type of f from double to void, as it wasn't used, and
               people were going to extra trouble to compute it.
    20 Apr 1999 FSM Added failure flag so that f() and grad() may signal failure to the caller.
    23/3/01 LSB (Manchester) Tidied documentation
    Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
   \endverbatim
*
*/
#include <string>
#include <VNL/vector.h>
#include <VNL/matrix.h>


namespace VNL {

/**  Abstract base for minimising functions.
*    LeastSquaresFunction is an abstract base for functions to be minimized
*    by an optimizer.  To define your own function to be minimized, subclass
*    from LeastSquaresFunction, and implement the pure virtual F (and
*    optionally GradF).
*
*    Whether or not f ought to be const is a problem.  Clients might well
*    want to cache some information during the call, and if they're compute
*    objects, will almost certainly be writing to members during the
*    computation.  For the moment it's non-const, but we'll see...
*/
class LeastSquaresFunction
{
 public:
  enum  UseGradient {
    no_gradient,
    use_gradient
  };
  bool failure;

/** Construct vnl_least_squares_function.
* Passing number of parameters (unknowns, domain dimension) and number of
* residuals (range dimension).
* The optional argument should be no_gradient if the gradf function has not
* been implemented.
*/
  LeastSquaresFunction(int number_of_unknowns, 
		       int number_of_residuals, 
		       UseGradient = use_gradient);

  virtual ~LeastSquaresFunction();

  // the virtuals may call this to signal a failure.
  void ThrowFailure();
  void ClearFailure();

/** The main function.
*  Given the parameter vector x, compute the vector of residuals fx.
*  Fx has been sized appropriately before the call.
*/
  virtual void F(Vector<double> const& x, Vector<double>& fx) = 0;

/** Calculate the Jacobian, given the parameter vector x.
*/
  virtual void GradF(Vector<double> const& x, Matrix<double>& jacobian);

/** Use this to compute a finite-difference gradient other than lmdif.
*/
  void FDGradF(Vector<double> const& x, Matrix<double>& jacobian,
               double stepsize);

/** Called after each LM iteration to print debugging etc.
*/
  virtual void Trace(int iteration,
                     Vector<double> const& x,
                     Vector<double> const& fx);

/** Compute the rms error at x by calling f and returning the norm of the residual vector.
*/
  double RMS(Vector<double> const& x);

/** Return the number of unknowns.
*/
  int GetNumberOfUnknowns() const { return p_; }

/** Return the number of residuals.
*/
  int GetNumberOfResiduals() const { return n_; }

/** Return true if the derived class has indicated that gradf has been implemented.
*/
  bool HasGradient() const { return use_gradient_; }

 protected:
  int p_;
  int n_;
  bool use_gradient_;
  std::string print_x_fmt_;
  std::string print_f_fmt_;

  void _init(int number_of_unknowns, int number_of_residuals);
};


}; // End namespace VNL

#endif // vnl_least_squares_function_h_