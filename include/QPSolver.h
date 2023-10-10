/**
 * @file  : QPSolver.h
 * @author: Jon Woolfrey
 * @date  : August 2023
 * @brief : Header file containing template class.
 *
 * This software is publicly available under the GNU General Public License V3.0. You are free to
 * use it and modify it as you see fit. If you find it useful, please acknowledge it.
 *
 * @see https://github.com/Woolfrey/SimpleQPSolver
 */

#ifndef QPSOLVER_H_
#define QPSOLVER_H_

#include <Eigen/Dense>                                                                              // Linear algebra and matrix decomposition
#include <iostream>                                                                                 // cerr, cout
#include <vector>                                                                                   // vector

using namespace Eigen;                                                                              // Eigen::Dynamic, Eigen::Matrix
using namespace std;                                                                                // std::invalid_argument, std::to_string

template <class DataType = float>
class QPSolver
{
	public:
		/**
		 * Constructor.
		 */
		QPSolver() {}
			
		/**
		 * Minimize 0.5*x'*H*x + x'*f, where x is the decision variable.
		 * @param H The Hessian matrix. It is assumed to be positive semi-definite.
		 * @param f A vector.
		 * @return The optimal solution for x.
		 */
		static Vector<DataType,Dynamic>
		solve(const Matrix<DataType, Dynamic, Dynamic>  &H,
                      const Vector<DataType, Dynamic>           &f);
	
		             
		/**
		 * Linear least squares of a problem y - A*x.
		 * @param y The vector of outputs or observations
		 * @param A The matrix defining the linear relationship between y and x
		 * @param W A positive-definite weighting on the y values.
		 * @return The vector x which returns the minimum norm || y - A*x ||
		 */             
		static Vector<DataType,Dynamic>
		least_squares(const Vector<DataType,Dynamic>           &y,
			      const Matrix<DataType, Dynamic, Dynamic> &A,
			      const Matrix<DataType, Dynamic, Dynamic> &W);

		/**
		 * Solve a least squares problem where the decision variable has more elements than the output.
		 * The problem is of the form:
		 * min 0.5*(xd - x)'*W*(xd - x)
		 * subject to: A*x = y
		 * @param xd A desired value for the solution.
		 * @param W A weighting on the desired values / solution
		 * @param A The matrix for the linear equality constraint
		 * @param y Equality constraint vector
		 * @return The optimal solution for x.
		 */
		static Vector<DataType,Dynamic>
		redundant_least_squares(const Vector<DataType, Dynamic>          &xd,
		                        const Matrix<DataType, Dynamic, Dynamic> &W,
		                        const Matrix<DataType, Dynamic, Dynamic> &A,
		                        const Vector<DataType, Dynamic>          &y);
		                                                         
		/**
		 * Solve linear least squares with upper and lower bounds on the solution.
		 * The problem is of the form:
		 * min 0.5*(y - A*x)'*W*(y - A*x)
		 * subject to: xMin <= x <= x
		 * This method uses an interior point algorithm to satisfy inequality constraints and thus requires a start point as an argument.
		 * @param y The vector component of the linear equation.
		 * @param A The matrix that maps x to y.
		 * @param xMin The lower bound on the decision variable
		 * @param xMax The upper bound on the decision variable.
		 * @param x0 A start point for the algorithm.
		 * @return The optimal solution within the constraints.
		 */
		Vector<DataType,Dynamic>
		constrained_least_squares(const Vector<DataType, Dynamic>          &y,
		                          const Matrix<DataType, Dynamic, Dynamic> &A,
		                          const Matrix<DataType, Dynamic, Dynamic> &W,
		                          const Vector<DataType, Dynamic>          &xMin,
		                          const Vector<DataType, Dynamic>          &xMax,
		                          const Vector<DataType, Dynamic>          &x0);
		             
		/**
		 * Solve a redundant least squares problem with upper and lower bounds on the solution.
		 * The problem is of the form:
		 * min 0.5*(xd - x)'*W*(xd - x)
		 * subject to: A*x = y
		 *           xMin <= x <= xMax
		 * It uses an interior point algorithm and thus requires a start point as an argument.
		 * @param xd Desired value for the solution.
		 * @param W Weighting on the desired value / solution.
		 * @param A Linear equality constraint matrix.
		 * @param y Linear equality constraint vector.
		 * @param xMin Lower bound on the solution.
		 * @param xMax upper bound on the solution.
		 * @param x0 Starting point for the algorithm.
		 */                  
		Vector<DataType,Dynamic>
		constrained_least_squares(const Vector<DataType, Dynamic>          &xd,
		                          const Matrix<DataType, Dynamic, Dynamic> &W,
		                          const Matrix<DataType, Dynamic, Dynamic> &A,
		                          const Vector<DataType, Dynamic>          &y,
		                          const Vector<DataType, Dynamic>          &xMin,
		                          const Vector<DataType, Dynamic>          &xMax,
		                          const Vector<DataType, Dynamic>          &x0);
		
		/**
		 * Solve a redundant least squares problem with inequality constraints on the solution.
		 * The problem is of the form:
		 * min 0.5*(xd - x)'*W*(xd - x)
		 * subject to: A*x = y
		 *             B*x < z
		 * It uses an interior point algorithm and thus requires a start point as an argument.
		 * @param xd Desired value for the solution.
		 * @param W Weighting on the desired value / solution.
		 * @param A Equality constraint matrix.
		 * @param y Equality constraint vector.
		 * @param B Inequality constraint matrix.
		 * @param z Inequality constraint vector.
		 * @param x0 Starting point for the algorithm.
		 */  
		Vector<DataType,Dynamic>
		constrained_least_squares(const Vector<DataType, Dynamic>          &xd,
		                          const Matrix<DataType, Dynamic, Dynamic> &W,
		                          const Matrix<DataType, Dynamic, Dynamic> &A,
		                          const Vector<DataType, Dynamic>          &y,
		                          const Matrix<DataType, Dynamic, Dynamic> &B,
		                          const Vector<DataType, Dynamic>          &z,
		                          const Vector<DataType, Dynamic>          &x0);
		
		/**
		 * Solve a generic quadratic programming problem with inequality constraints.
		 * The problem is of the form:
		 * min 0.5*x'*H*x + x'*f
		 * subject to: B*x < z
		 * This method uses an interior point algorithm and thus requires a start point as an argument.
		 * @param H A positive semi-definite matrix such that H = H'.
		 * @param f A vector for the linear component of the problem.
		 * @param B Inequality constraint matrix.
		 * @param z Inequality constraint vector.
		 * @param x0 Start point for the algorithm.
		 * @return x: A solution that minimizes the problem whilst obeying inequality constraints.
		 */
		Vector<DataType,Dynamic>  
		solve(const Matrix<DataType, Dynamic, Dynamic> &H,
		      const Vector<DataType, Dynamic>          &f,
		      const Matrix<DataType, Dynamic, Dynamic> &B,
		      const Vector<DataType, Dynamic>          &z,
		      const Vector<DataType, Dynamic>          &x0);
		
		/**
		 * Set the tolerance for the step size in the interior point aglorithm.
		 * The algorithm terminates if alpha*dx < tolerance, where dx is the step and alpha is a scalar.
		 * @param tolerance As it says.
		 * @return Returns false if the input argument is invalid.
		 */
		bool set_tolerance(const DataType &tolerance);
		
		/**
		 * Set the maximum number of steps in the interior point algorithm before terminating.
		 * @param number As it says.
		 * @return Returns false if the input argument is invalid.
		 */
		bool set_max_steps(const unsigned int &number);
		
		/**
		 * Set the scalar for the constraint barriers in the interior point aglorithm.
		 * Inequality constraints are converted to a log barrier function. The scalar determines how steep the initial barriers are.
		 * @param scalar The initial scalar value when starting the interior point algorithm.
		 * @return Returns false if the argument was invalid.
		 */
		bool set_barrier_scalar(const DataType &scalar);
		
		/**
		 * Set the rate at which the constraint barriers are reduced in the interior point aglorithm.
		 * The barrier is reduced at a given rate / scale every step in the algorithm to safely approach constraints.
		 * @param rate A scalar < 1 which the barrier scalar is reduced by every step.
		 * @return Returns false if the argument is invalid.
		 */
		bool set_barrier_reduction_rate(const DataType &rate);
		
		/**
		 * @return Returns the step size alpha*||dx|| for the final iteration in the interior point algorithm.
		 */
		DataType step_size() const { return this->stepSize; }
		
		/**
		 * @return Returns the number of iterations it took to solve the interior point algorithm.
		 */
		unsigned int num_steps() const { return this->numSteps; }
		
		/**
		 * @return Returns the last solution from when the interior point algorithm was previously called.
		 */
		Vector<DataType, Dynamic> last_solution() const { return this->lastSolution; }
		
		/**
		 * Clears the last solution such that last_solution().size() == 0.
		 */
		void clear_last_solution() { this->lastSolution.resize(0); }
		
		/**
		 * The interior point algorithm will use the dual method to solve a redundant QP problem.
		 */
		void use_dual();
		
		/**
		 * The interior point algorithm will use the primal method to solve a redundant QP problem.
		 */
		void use_primal();
		
	private:
		
		DataType tol = 1e-02;                                                               ///< Minimum value for the step size before terminating the interior point algorithm.
		DataType stepSize;                                                                  ///< Step size on the final iteration of the interior point algorithm.
		DataType barrierReductionRate = 1e-03;                                              ///< Constraint barrier scalar is multiplied by this value every step in the interior point algorithm.
		DataType initialBarrierScalar = 100;                                                ///< Starting value for the constraint barrier scalar in the interior point algorithm.
		
		enum Method {dual, primal} method = primal;                                         ///< Used to select which method to solve for with redundant least squares problems.                                               
		
		unsigned int maxSteps = 20;                                                         ///< Maximum number of iterations to run interior point method before terminating.
		
		unsigned int numSteps = 0;                                                          ///< Records the number of steps it took to solve a problem with the interior point algorithm.
		
		Vector<DataType,Dynamic> lastSolution;                                              ///< Final solution returned by interior point algorithm. Can be used as a starting point for future calls to the method.
		
		/**
		 * The std::min function doesn't like floats, so I had to write my own ಠ_ಠ
		 * @return Returns the minimum between to values 'a' and 'b'.
		 */
		DataType min(const DataType &a, const DataType &b)
		{
			DataType minimum = (a < b) ? a : b;
			return minimum;
		}
		
};                                                                                                  // Required after class declaration

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //              Solve a standard QP problem of the form min 0.5*x'*H*x + x'*f                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType> inline
Vector<DataType,Dynamic>
QPSolver<DataType>::solve(const Matrix<DataType, Dynamic, Dynamic> &H,
		          const Vector<DataType, Dynamic>          &f)
{
	if(H.rows() != H.cols())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] solve(): "
		                       "Expected a square matrix for the Hessian H but it was "
		                       + to_string(H.rows()) + "x" + to_string(H.cols()) + ".");
	}
	else if(H.rows() != f.rows())
	{	
		throw invalid_argument("[ERROR] [QP SOLVER] solve(): "
		                       "Dimensions of arguments do not match. "
		                       "The Hessian H was " + to_string(H.rows()) + "x" + to_string(H.cols()) +
		                       " and the f vector was " + to_string(f.size()) + "x1.");
	}
	else 	return H.ldlt().solve(-f);                                                          // Too easy lol ᕙ(▀̿̿ĺ̯̿̿▀̿ ̿) ᕗ
}

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //           Solve an unconstrained least squares problem: min 0.5(y-A*x)'*W*(y-A*x)              //
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType> inline
Vector<DataType,Dynamic>
QPSolver<DataType>::least_squares(const Vector<DataType, Dynamic>          &y,
                                  const Matrix<DataType, Dynamic, Dynamic> &A,
                                  const Matrix<DataType, Dynamic, Dynamic> &W)
{
	if(A.rows() < A.cols())                                                                     // Redundant system, use other function
	{
		throw invalid_argument("[ERROR] [QP SOLVER] least_squares(): "
		                       "The A matrix has more rows than columns ("
		                       + to_string(A.rows()) + "x" + to_string(A.cols()) + "). "
		                       "Did you mean to call redundant_least_squares()?");	                    		                   
	}
	if(W.rows() != W.cols())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] least_squares(): "
		                       "Expected a square weighting matrix W but it was "
		                       + to_string(W.rows()) + "x" + to_string(W.cols()) + ".");
	}
	else if(y.rows() != W.rows() and W.cols() != A.rows())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] least_squares(): "
		                       "Dimensions of input arguments do not match. "
		                       "The y vector was " + to_string(y.size()) + "x1, "
		                       "the A matrix had " + to_string(A.rows()) + " rows, and "
		                       "the weighting matrix W was " + to_string(W.rows()) + "x" + to_string(W.cols()) + ".");
	}
	else	return (A.transpose()*W*A).ldlt().solve(A.transpose()*W*y);                         // x = (A'*W*A)^-1*A'*W*y
}

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //    Solve least squares problem of the form min 0.5*(xd - x)'*W*(xd - x) subject to: A*x = y    //
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType> inline
Vector<DataType,Dynamic>
QPSolver<DataType>::redundant_least_squares(const Vector<DataType, Dynamic>          &xd,
                                            const Matrix<DataType, Dynamic, Dynamic> &W,
                                            const Matrix<DataType, Dynamic, Dynamic> &A,
                                            const Vector<DataType, Dynamic>          &y)
{
	if(A.rows() >= A.cols())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
		                       "The equality constraint matrix has more rows than columns ("
		                       + to_string(A.rows()) + " >= " + to_string(A.cols()) + "). "
		                       "Did you mean to call the other least squares function?");
	}
	else if(W.rows() != W.cols())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
		                       "Expected the weighting matrix to be square but it was "
		                       + to_string(W.rows()) + "x" + to_string(W.cols()) + ".");
	}
	else if(xd.size() != W.rows() or W.cols() != A.cols())
	{	
		throw invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
		                       "Dimensions for the decision variable do not match. "
		                       "The desired vector had " + to_string(xd.size()) + " elements, "
		                       "the weighting matrix was " + to_string(W.rows()) + "x" + to_string(W.cols()) + ", and "
		                       "the constraint matrix had " + to_string(A.cols()) + " columns.");
        }
        else if(y.size() != A.rows())
        {    	
        	throw invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
        	                       "Dimensions for the equality constraint do not match. "
        	                       "The constraint vector had " + to_string(y.size()) + " elements, and "
        	                       "the constraint matrix had " + to_string(A.rows()) + " rows.");
        }
        else
        {   		
		MatrixXf invWA = W.ldlt().solve(A.transpose());                                     // Makes calcs a little easier
		
		return xd + invWA*(A*invWA).ldlt().solve(y - A*xd);                                 // xd + W^-1*A'*(A*W^-1*A')^-1*(y-A*xd)
	}
}

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //      Solve a constrained problem: min 0.5*(y - A*x)'*W*(y - A*x) s.t. xMin <= x <= xMax        //
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType> inline
Vector<DataType,Dynamic>
QPSolver<DataType>::constrained_least_squares(const Vector<DataType, Dynamic>           &y,
                                              const Matrix<DataType, Dynamic, Dynamic>  &A,
                                              const Matrix<DataType, Dynamic, Dynamic>  &W,
                                              const Vector<DataType, Dynamic>           &xMin,
                                              const Vector<DataType, Dynamic>           &xMax,
                                              const Vector<DataType, Dynamic>           &x0)
{
	// Ensure that the input arguments are sound.
	if(y.size() != A.rows() or A.rows() != W.rows())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] constrained_least_squares(): "
		                       "Dimensions of the linear equation do not match. "
		                       "The y vector had " + to_string(y.size()) + " elements, "
		                       "the A matrix had " + to_string(A.rows()) + " rows, and "
		                       "the weighting matrix W had " + to_string(W.rows()) + " rows.");
	}
	else if(W.rows() != W.cols())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] constrained_least_squares(): "
		                       "Expected the weighting matrix W to be square, but it was "
		                       + to_string(W.rows()) + "x" + to_string(W.cols()) + ".");
	}
	else if(A.cols() != xMin.size() or xMin.size() != xMax.size() or xMax.size() != x0.size())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] constrained_least_squares(): "
		                       "Dimensions for decision variable do not match. "
		                       "The A matrix had " + to_string(A.cols()) + " columns, "
		                       "the xMin argument had " + to_string(xMin.size()) + " elements, "
		                       "the xMax argument had " + to_string(xMax.size()) + " elements, and "
		                       "the start point x0 had " + to_string(x0.size()) + " elements.");
	}
	
	// Convert to standard form and solve with the interior point algorithm
	
	unsigned int n = x0.size();
	
	// B = [  I ] < z = [  xMax ]
	//     [ -I ]       [ -xMin ]
	
	Matrix<DataType,Dynamic,Dynamic> B(2*n,n);
	B.block(0,0,n,n).setIdentity();
	B.block(n,0,n,n) = -Matrix<DataType,Dynamic,Dynamic>::Identity(n,n);
	
	Vector<DataType,Dynamic> z(2*n);
	z.head(n) =  xMax;
	z.tail(n) = -xMin;
	
	Matrix<DataType,Dynamic,Dynamic> AtW = A.transpose()*W;                                     // Makes calcs a tiny bit faster

	return solve(AtW*A, -AtW*y, B, z, x0);                                                      // Send to interior point algorithm and solve
}

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //    Solve a constrained problem min 0.5*(xd - x)'*W*(xd - x) s.t. A*x = y, xMin <= x <= xMax    //
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType> inline
Vector<DataType,Dynamic>
QPSolver<DataType>::constrained_least_squares(const Vector<DataType, Dynamic>           &xd,
                                              const Matrix<DataType, Dynamic, Dynamic>  &W,
                                              const Matrix<DataType, Dynamic, Dynamic>  &A,
                                              const Vector<DataType, Dynamic>           &y,
                                              const Vector<DataType, Dynamic>           &xMin,
                                              const Vector<DataType, Dynamic>           &xMax,
                                              const Vector<DataType, Dynamic>           &x0)
{
	if(xMin.size() != xMax.size())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] constrained_least_squares(): "
		                       "Dimensions of inequality constraints do not match. "
		                       "The xMin argument had " + to_string(xMin.size()) + " elements, and "
		                       "the xMax argument had " + to_string(xMax.size()) + " elements.");
	}
		                       
	// Convert constraints to standard form and pass on to generic function
	
	unsigned int n = xMin.size();
	
	// B = [  I ] < z = [  xMax ]
	//     [ -I ]     = [ -xMin ]
	
	Matrix<DataType,Dynamic,Dynamic> B(2*n,n);
	B.block(0,0,n,n).setIdentity();
	B.block(n,0,n,n) = -Matrix<DataType,Dynamic,Dynamic>::Identity(n,n);
	
	Vector<DataType,Dynamic> z(2*n);
	z.head(n) =  xMax;
	z.tail(n) = -xMin;
	
	return constrained_least_squares(xd, W, A, y, B, z, x0);
}

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //        Solve a constrained problem min 0.5*(xd - x)'*W*(xd - x) s.t. A*x = y, B*x < z          //
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType> inline
Vector<DataType,Dynamic>
QPSolver<DataType>::constrained_least_squares(const Vector<DataType, Dynamic>          &xd,
                                              const Matrix<DataType, Dynamic, Dynamic> &W,
                                              const Matrix<DataType, Dynamic, Dynamic> &A,
                                              const Vector<DataType, Dynamic>          &y,
                                              const Matrix<DataType, Dynamic, Dynamic> &B,
                                              const Vector<DataType, Dynamic>          &z,
                                              const Vector<DataType, Dynamic>          &x0)
{
	// Ensure input arguments are sound
	if(xd.size() != W.rows() or W.rows() != A.cols() or A.cols() != B.cols() or B.cols() != x0.size())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] constrained_least_squares(): "
		                       "Dimensions for decision variable do not match. "
		                       "The desired value xd had " + to_string(xd.size()) + " elements, "
		                       "the weighting matrix W had " + to_string(W.rows()) + " rows, "
		                       "the equality constraint matrix A had " + to_string(A.cols()) + " columns, "
		                       "the inequality constraint matrix B had " + to_string(B.cols()) + " columns, and "
		                       "the start point x0 had " + to_string(x0.size()) + " elements.");
	}
	else if(W.rows() != W.cols())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] constrained_least_squares(): "
		                       "Expected the weighting matrix W to be square, but it was "
		                       + to_string(W.rows()) + "x" + to_string(W.cols()) + ".");
	}
	else if(A.rows() != y.size())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] constrained_least_squares(): "
		                       "Dimensions for equality constraint do not match. "
		                       "The equality constraint matrix A had " + to_string(A.rows()) + " rows, and "
		                       "the equality constraint vector y had " + to_string(y.size()) + " elements.");
	}
	else if(B.rows() != z.rows())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] constrained_least_squared(): "
		                       "Dimensions for inequality constraint do no match. "
		                       "The inequality constraint matrix B had " + to_string(B.rows()) + " rows, and "
		                       "the inequality constraint vector z had " + to_string(z.size()) + " elements.");
	}
	
	if(this->method == primal)
	{	
		unsigned int c = B.rows();                                                          // Number of inequality constraints
		unsigned int m = A.rows();                                                          // Number of equality constraints
		unsigned int n = A.cols();                                                          // Decision variable
		
		// H = [  0  -A ]
		//     [ -A'  W ]
		Matrix<DataType,Dynamic,Dynamic> H(m+n,m+n);
		H.block(0,0,m,m).setZero();
		H.block(0,m,m,n) = -A;
		H.block(m,0,n,m) = -A.transpose();
		H.block(m,m,n,n) = W;
		
		// f = [    y  ]
		//     [ -W*xd ]
		Vector<DataType,Dynamic> f(m+n);
		f.head(m) = y;
		f.tail(n) = -W*xd;
		
		// new_x0 = [ lambda ]
		//          [   x0   ]
		Vector<DataType,Dynamic> new_x0(m+n);
		new_x0.head(m) = (A*W.ldlt().solve(A.transpose())).ldlt().solve(y - A*xd);          // Initial guess for Lagrange multipliers
		new_x0.tail(n) = x0;
		
		// newB = [ 0 B ]
		Matrix<DataType,Dynamic,Dynamic> newB(c,m+n);
		newB.block(0,0,c,m).setZero();
		newB.block(0,m,c,n) = B;
		
		this->lastSolution = solve(H,f,newB,z,new_x0).tail(n);                              // We don't need the Lagrange multipliers
		
		return this->lastSolution;                                                          // Return decision variable x
	}
	else if(this->method == dual)
	{
		// x = xd + W^-1*A'*lambda
		
		// lambda = (A*W^-1*A')^-1*(y - A*xd)
		
		Matrix<DataType,Dynamic,Dynamic> invWAt = W.ldlt().solve(A.transpose());            // Makes calcs a little easier
		
		Matrix<DataType,Dynamic,Dynamic> H = A*invWAt;                                      // Hessian matrix for dual problem
		
		LDLT<Matrix<DataType,Dynamic,Dynamic>> Hdecomp(H);                                  // Saves a bit of time
		
		Vector<DataType,Dynamic> xr = invWAt*(H,-y,B*invWAt,z,Hdecomp.solve(y));            // Solve the range space
		
		Vector<DataType,Dynamic> xn = xd - invWAt*Hdecomp.solve(A*xd);                      // Compute null space component
		
		DataType alpha = 1.0;
		for(int i = 0; i < z.size(); i++)
		{
			DataType a = B.row(i).dot(xr);
			
			DataType b = B.row(i).dot(xn);
			
			DataType dist = z(i) - a - b;
			
			if(dist <= 0) alpha = min(alpha, 0.99*abs((dist - a)/b));
		}
		
		this->lastSolution = xr + alpha*xn;
		
		return this->lastSolution;
	}
	else
	{
		throw runtime_error("[ERROR] [QP SOLVER] constrained_least_squares(): "
		                    "Method for solving redundant least squares was neither 'dual' "
		                    "nor 'primal'. How did that happen?");
	}
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //          Solve a problem of the form: min 0.5*x'*H*x + x'*f subject to: B*x <= z              //        
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType> inline
Vector<DataType,Dynamic>
QPSolver<DataType>::solve(const Matrix<DataType, Dynamic, Dynamic> &H,
                          const Vector<DataType,Dynamic>           &f,
                          const Matrix<DataType, Dynamic, Dynamic> &B,
                          const Vector<DataType,Dynamic>           &z,
                          const Vector<DataType,Dynamic>           &x0)
{
	// Ensure arguments are sound
	if(H.rows() != H.cols())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] solve(): "
		                       "Expected the Hessian matrix H to be square but it was " 
		                       + to_string(H.rows()) + "x" + to_string(H.cols()) + ".");
	}
	else if(H.cols() != f.size() or f.size() != B.cols() or B.cols() != x0.size())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] solve(): "
		                       "Dimensions of arguments for decision variable do not match. "
		                       "The Hessian matrix had " + to_string(H.cols()) + " rows/columns, "
		                       "the vector f had " + to_string(f.size()) + " elements, "
		                       "the inequality constraint matrix B had " + to_string(B.cols()) + " columns, and "
		                       "the start point x0 had " + to_string(x0.size()) + " elements.");
	}
	else if(B.rows() != z.size())
	{
		throw invalid_argument("[ERROR] [QP SOLVER] solve(): "
		                       "Dimensions for inequality constraint do not match. "
		                       "The inequality constraint matrix B had " + to_string(B.rows()) + " rows, and "
		                       "the inequality constraint vector z had " + to_string(z.size()) + " elements.");
	}
	
	// h = 0.5*x'*H*x + x'*f - sum log(d_i),   d_i = z_i - b_i'*x
	// g = H*x + f + sum (1/d_i)*b_i
	// I = H + sum (1/d_i^2)*b_i*b_i'
	
	// Variables used in this scope
	DataType u = this->initialBarrierScalar;                                                    // As it says
	unsigned int dim = x0.size();                                                               // Dimensions of the decision varialbe
	unsigned int numConstraints = z.size();                                                     // As it says
	Matrix<DataType,Dynamic,Dynamic> I(dim,dim);                                                // Hessian matrix
	Vector<DataType,Dynamic> g(dim);                                                            // Gradient vector
	vector<DataType> d(numConstraints);                                                         // Distance to every constraint
	vector<Vector<DataType,Dynamic>> b(numConstraints);                                         // Row vectors of constraint matrix (transposed)
	vector<Matrix<DataType,Dynamic,Dynamic>> bbt(numConstraints);                               // Outer product of row vectors
	Vector<DataType,Dynamic> x(dim);                                                            // We want to solve for this
	
	// Do some pre-processing
	bool initialConstraintViolated = false;
	for(int j = 0; j < numConstraints; j++)
	{
		b[j]   = B.row(j).transpose();                                                      // Transpose the row vector
		bbt[j] = b[j]*b[j].transpose();                                                     // Compute the outer product

		d[j] = z(j) - b[j].dot(x0);                                                         // Distance to constraint
		
		if(d[j] <= 0) initialConstraintViolated = true;                                     // Flag
	}
	
	// Set the start point
	if(initialConstraintViolated)
	{
		Vector<DataType,Dynamic> dz = 1e-03*Vector<DataType,Dynamic>::Ones(numConstraints);       // Add a tiny offset so we're not exactly on the constraint      
		
		     if(numConstraints > dim) x = (B.transpose()*B).ldlt().solve(B.transpose()*(z - dz)); // Underdetermined system
		else if(numConstraints < dim) x =  B.transpose()*(B*B.transpose()).ldlt().solve(z - dz);  // Overdetermined system
		else			      x =  B.partialPivLu().solve(z - dz);                        // Exact solution
	}
	else	x = x0;                                                                                   // Given start point
	
	// Run the interior point algorithm
	for(int i = 0; i < this->maxSteps; i++)
	{
		this->numSteps = i+1;                                                               // Increment the counter
		
		// (Re)set values for new loop
		g = H*x + f;                                                                        // Gradient vector
		I = H;                                                                              // Hessian matrix
		
		// Compute distance to every constraint
		for(int j = 0; j < numConstraints; j++)
		{
			d[j] = z(j) - b[j].dot(x);                                                  // Distance to constraint
			
			if(i == 0 and d[j] <= 0)
			{
				throw runtime_error("[ERROR] [QP SOLVER] solve(): Unable to find a solution that satisfies constraints.");
			}
			
			if(d[j] <= 0) d[j] = 1e-03;                                                 // Constraint violated; set a small, but non-zero distance
		 
			g += (u/d[j])*b[j];                                                         // Add up gradient
			I += (u/(d[j]*d[j]))*bbt[j];                                                // Add up Hessian
		}

		Vector<DataType,Dynamic> dx = I.ldlt().solve(-g);                                   // Compute Newton step
		
		// Compute scalar for step size so that constraint is not violated on next step
		DataType alpha = 1.0;
		for(int j = 0; j < numConstraints; j++)
		{
			DataType dotProd = b[j].dot(dx);                                            // Makes calcs a little easier
			
			if(d[j] - dotProd <= 0) alpha = min(alpha,0.9*d[j]/dotProd);                // Shrink scalar if constraint violated
		}
		
		dx *= alpha;                                                                        // Scale the step
		
		this->stepSize = dx.norm();                                                         // Magnitude of the step size
		
		if(this->stepSize <= this->tol) break;                                              // If smaller than tolerance, break
		
		// Increment values for next loop
		x += dx;                                                                            // Increment state
		u *= this->barrierReductionRate;                                                    // Reduce barrier
	}
	
	this->lastSolution = x;                                                                     // Save the value
	
	return x;
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //                  Set the rate at which the barrier scalar reduces: u *= beta                  //
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType>
bool QPSolver<DataType>::set_barrier_reduction_rate(const DataType &rate)
{
	if(rate <= 0 or rate >= 1)
	{
		cerr << "[ERROR] [QP SOLVER] set_barrier_reduction_rate(): "
		        "Input argument was " << to_string(rate) << " but it must be between 0 and 1.\n";
		             
		return false;
	}
	else
	{
		this->barrierRedunctionRate = rate;
		
		return true;
	}
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //       Set the magnitude of the step size for which the interior point method terminates       //
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType>
bool QPSolver<DataType>::set_tolerance(const DataType &tolerance)
{
	if(tolerance <= 0)
	{
		cerr << "[ERROR] [QP SOLVER] set_tolerance(): "
		     << "Input argument was " << to_string(tolerance) << " but it must be positive.\n";
		 
		return false;
	}
	else
	{
		this->tol = tolerance;
		
		return true;
	}
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //            Set the number of steps in the interior point method before terminating            //
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType>
bool QPSolver<DataType>::set_max_steps(const unsigned int &number)
{
	if(number == 0)
	{
		cerr << "[ERROR] [QP SOLVER] set_num_steps(): "
		     << "Input argument was 0 but it must be greater than zero.\n";
		
		return false;
	}
	else
	{
		this->maxSteps = number;
		
		return true;
	}
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //               Set the scalar on the barrier function for inequality constraints               //
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DataType>
bool QPSolver<DataType>::set_barrier_scalar(const DataType &scalar)
{
	if(scalar <= 0)
	{
		cerr << "[ERROR] [QP SOLVER] set_barrier_scalar(): "
		     << "Input argument was " << to_string(scalar) << " but it must be positive.\n";
		
		return false;
	}
	else
	{
		this->initialBarrierScalar = scalar;
		
		return true;
	}
}
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //                           Use the dual method to solve the QP problem                          //
////////////////////////////////////////////////////////////////////////////////////////////////////
template<class DataType>
void QPSolver<DataType>::use_dual()
{
	this->method = dual;
	
	std::cout << "[INFO] [QP SOLVER] Using the dual method to solve.\n";
}

  ////////////////////////////////////////////////////////////////////////////////////////////////////
 //                         Use the primal method to solve the QP problem                          //
////////////////////////////////////////////////////////////////////////////////////////////////////
template<class DataType>
void QPSolver<DataType>::use_primal()
{
	this->method = primal;
	
	std::cout << "[INFO] [QP SOLVER] Using the primal method to solve.\n";
}

#endif
