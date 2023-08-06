#include <QPSolver.h>                                                                               // Declaration of functions

/*
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //                        Solve a generic QP problem min 0.5*x'*H*x + x'*f                       //
///////////////////////////////////////////////////////////////////////////////////////////////////
Eigen::VectorXf QPSolver<DataType>::solve(const Eigen::MatrixXf &H,
                                          const Eigen::VectorXf &f)
{
	if(H.rows() != H.cols())
	{
		throw std::invalid_argument("[ERROR] [QP SOLVER] solve(): "
		                            "Expected a square matrix for the Hessian H but it was "
		                            + std::to_string(H.rows()) + "x" + std::to_string(H.cols()) + ".");
	}
	else if(H.rows() != f.size())
	{	
		throw std::invalid_argument("[ERROR] [QP SOLVER] solve(): "
		                            "Dimensions of arguments do not match. "
		                            "The Hessian H was " + std::to_string(H.rows()) + "x" + std::to_string(H.cols()) +
		                            " and the f vector was " + std::to_string(f.size()) + "x1.");
	}
	else 	return H.ldlt().solve(-f);                                                          // Too easy lol ᕙ(▀̿̿ĺ̯̿̿▀̿ ̿) ᕗ
}


  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //          Solve a constrained QP problem: min 0.5*x'*H*x + x'*f subject to: B*x >= z           //
///////////////////////////////////////////////////////////////////////////////////////////////////
Eigen::VectorXf QPSolver::solve(const Eigen::MatrixXf &H,
                                const Eigen::VectorXf &f,
                                const Eigen::MatrixXf &B,
                                const Eigen::VectorXf &z,
                                const Eigen::VectorXf &x0)
{
	int dim = x0.size();                                                                        // Dimensions for the state vector
	int numConstraints = B.rows();                                                              // As it says
	
	// Check that the inputs are sound
	if(H.rows() != H.cols())
	{
		throw std::invalid_argument("[ERROR] [QP SOLVER] solve(): Expected a square matrix for the Hessian "
		                            "but it was " + std::to_string(H.rows()) + "x" + std::to_string(H.cols()) + ".");
	}
	else if(f.size() != dim
	     or H.rows() != dim
	     or B.cols() != dim)
	{
		throw std::invalid_argument("[ERROR] [QP SOLVER] solve(): Dimensions of arguments do not match. "
		                            "The Hessian was " + std::to_string(H.rows()) + "x" + std::to_string(H.cols()) + ", "
		                            "the f vector was " + std::to_string(f.size()) + "x1, and "
		                            "the constraint matrix B had " + std::to_string(B.cols()) + " columns.");
	}
	else if(B.rows() != z.size())
	{
		throw std::invalid_argument("[ERROR] [QP SOLVER] solve(): Dimensions for constraints do not match. "
		                            "The constraint matrix B had " + std::to_string(B.rows()) + " rows, and "
		                            "the constaint vector z had " + std::to_string(z.size()) + " elements.");
	}
	else
	{
		// Solve the following optimization problem with Guass-Newton method:
		//
		//    min f(x) = 0.5*x'*H*x + x'*f - u*sum(log(d_i))
		//
		// where d_i = b_i*x - c_i is the distance to the constraint
		//
		// Then the gradient and Hessian are:
		//
		//    g(x) = H*x + f - u*sum((1/d_i)*b_i')
		//
		//    I(x) = H + u*sum((1/(d_i^2))*b_i'*b_i)
		
		// Local variables
	
		Eigen::MatrixXf I;                                                                  // Hessian matrix
		Eigen::VectorXf g(dim);                                                             // Gradient vector
		Eigen::VectorXf dx = Eigen::VectorXf::Zero(dim);                                    // Newton step = -I^-1*g
		Eigen::VectorXf x = x0;                                                             // Assign initial state variable
		
		float alpha;                                                                        // Scalar for Newton step
		float beta  = this->beta0;                                                          // Shrinks barrier function
		float u     = this->u0;                                                             // Scalar for barrier function
	
		std::vector<float> d; d.resize(numConstraints);
		
		// Do some pre-processing
		std::vector<Eigen::VectorXf> bt(numConstraints);
		std::vector<Eigen::MatrixXf> btb(numConstraints);
		for(int j = 0; j < numConstraints; j++)
		{
			bt[j]  = B.row(j).transpose();                                              // Row vectors of B transposed
			btb[j] = bt[j]*bt[j].transpose();                                           // Outer product of row vectors
		}
		
		// Run the interior point method
		for(int i = 0; i < this->steps; i++)
		{
			// (Re)set values for new loop
			g.setZero();                                                                // Gradient vector
			I = H;                                                                      // Hessian for log-barrier function
			
			// Compute distance to each constraint
			for(int j = 0; j < numConstraints; j++)
			{
				d[j] = bt[j].dot(x) - z(j);                                         // Distance to jth constraint
				
				if(d[j] <= 0)
				{
					if(i == 0)
					{
						throw std::runtime_error("[ERROR] [QP SOLVER] solve(): "
					                                 "Start point x0 is outside the constraints.");
					}
		
					d[j] = 1e-03;                                               // Set a small, non-zero value
					u   *= 100;                                                 // Increase the barrier function
				}
						
				g += -(u/d[j])*bt[j];                                               // Add up gradient vector
				I +=  (u/(d[j]*d[j]))*btb[j];                                       // Add up Hessian
			}
			
			g += H*x + f;                                                               // Finish summation of gradient vector

			dx = I.ldlt().solve(-g);                                                    // Robust Cholesky decomp
			
			// Ensure the next position is within the constraint
			alpha = this->alpha0;                                                       // Reset the scalar for the step size
			for(int j = 0; j < numConstraints; j++)
			{
				float dotProduct = bt[j].dot(dx);                                   // Makes things a little easier
				
				if( d[j] + alpha*dotProduct < 0 )                                   // If constraint violated on next step...
				{
					float temp = (1e-04 - d[j])/dotProduct;                     // Compute optimal scalar to avoid constraint violation
					
					if(temp < alpha) alpha = temp;                              // If smaller, override
				}
			}

			if(alpha*dx.norm() < this->tol) break;                                      // Change in position is insignificant; must be optimal
			
			// Update values for next loop
			x += alpha*dx;                                                              // Increment state
			u *= beta;                                                                  // Decrease barrier function
		}
			
		this->lastSolution = x;                                                             // Save this value for future use
		
		return x;
	}
}	

  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //          Solve an unconstrained least squares problem: min 0.5(y-A*x)'*W*(y-A*x)              //
///////////////////////////////////////////////////////////////////////////////////////////////////
Eigen::VectorXf QPSolver::least_squares(const Eigen::VectorXf &y,
                                        const Eigen::MatrixXf &A,
                                        const Eigen::MatrixXf &W)
{
	if(A.rows() < A.cols())                                                                     // Redundant system, use other function
	{
		throw std::invalid_argument("[ERROR] [QP SOLVER] least_squares(): "
		                            "The A matrix has more rows than columns ("
		                            + std::to_string(A.rows()) + "x" + std::to_string(A.cols()) + "). "
		                            "Did you mean to call the function for redundant least squares?");	                    		                   
	}
	if(W.rows() != W.cols())
	{
		throw std::invalid_argument("[ERROR] [QP SOLVER] least_squares(): "
		                            "Expected a square weighting matrix W but it was "
		                            + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + ".");
	}
	else if(y.size() != W.rows() and A.rows() != y.size())
	{
		throw std::invalid_argument("[ERROR] [QP SOLVER] least_squares(): "
		                            "Dimensions of input arguments do not match. "
		                            "The y vector was " + std::to_string(y.size()) + "x1, "
		                            "the A matrix had " + std::to_string(A.rows()) + " rows, and "
		                            "the weighting matrix W was " + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + ".");
	}
	else	return (A.transpose()*W*A).ldlt().solve(A.transpose()*W*y);                         // x = (A'*W*A)^-1*A'*W*y
}
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //    Solve a constrained least squares problem 0.5*(y-A*x)'*W*(y-A*x) s.t. xMin <= x <= xMax    //
///////////////////////////////////////////////////////////////////////////////////////////////////                           
Eigen::VectorXf QPSolver::least_squares(const Eigen::VectorXf &y,
                                        const Eigen::MatrixXf &A,
                                        const Eigen::MatrixXf &W,
                                        const Eigen::VectorXf &xMin,
                                        const Eigen::VectorXf &xMax,
                                        const Eigen::VectorXf &x0)
{
	if(W.rows() != W.cols())
	{	
		throw std::invalid_argument("[ERROR] [QP SOLVER] least_squares(): "
		                            "Expected a square weighting matrix W but it was "
		                            + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + ".");
	}
	else if(y.size() != W.rows() and A.rows() != y.size())
	{	
		throw std::invalid_argument("[ERROR] [QP SOLVER] least_squares(): "
		                            "Dimensions of input arguments do not match. "
		                            "The y vector was " + std::to_string(y.size()) + "x1, "
		                            "the A matrix had " + std::to_string(A.rows()) + " rows, and "
		                            "the weighting matrix W was " + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + ".");
	}
	else if(A.cols() != xMin.size() or xMin.size() != xMax.size() or xMax.size() != x0.size())
	{

		throw std::invalid_argument("[ERROR] [QP SOLVER] least_squares(): "
		                            "Dimensions for the decision variable do not match. "
		                            "The A matrix had " + std::to_string(A.cols()) + " columns, "
		                            "the xMin vector had " + std::to_string(xMin.size()) + " elements, "
		                            "the xMax vector had " + std::to_string(xMax.size()) + " elements, and "
		                            "the start point x0 had " + std::to_string(x0.size()) + " elements.");
	}
	else
	{
		int n = x0.size();
		
		// Set up constraint matrices in standard form Bx >= c where:
		// B*x = [ -I ] >= [ -xMax ]
		//       [  I ]    [  xMin ]
		Eigen::MatrixXf B(2*n,n);
		B.block(n,0,n,n).setIdentity();
		B.block(0,0,n,n) = -B.block(n,0,n,n);

		Eigen::VectorXf z(2*n);
		z.head(n) = -xMax;
		z.tail(n) =  xMin;
		
		Eigen::MatrixXf AtW = A.transpose()*W;                                              // Makes calcs a little simpler

		return solve(AtW*A,-AtW*y, B, z, x0);                                               // Convert to standard form and solve
	}
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //       Solve a least squares problem of the form min 0.5*(xd-x)'*W*(xd-x)  s.t. A*x = y        //
///////////////////////////////////////////////////////////////////////////////////////////////////                                  
Eigen::VectorXf QPSolver::redundant_least_squares(const Eigen::VectorXf &xd,
                                                  const Eigen::MatrixXf &W,
                                                  const Eigen::VectorXf &y,
                                                  const Eigen::MatrixXf &A)
{
	if(W.rows() != W.cols())
	{
		throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
		                            "Expected the weighting matrix to be square but it was "
		                            + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + ".");
	}
	else if(xd.size() != W.rows() or W.cols() != A.cols())
	{	
		throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
		                            "Dimensions for the decision variable do not match. "
		                            "The desired vector xd had " + std::to_string(xd.size()) + " elements, "
		                            "the weighting matrix was " + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + ", and "
		                            "the constraint matrix A had " + std::to_string(A.cols()) + " columns.");
        }
        else if(y.size() != A.rows())
        {    	
        	throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
        	                            "Dimensions for the equality constraint do not match. "
        	                            "The y vector had " + std::to_string(y.size()) + " elements, and "
        	                            "the constraint matrix had " + std::to_string(A.rows()) + " rows.");
        }
        else
        {   		
		Eigen::MatrixXf B = W.ldlt().solve(A.transpose());                                  // Makes calcs a little easier
		
		return xd - B*(A*B).ldlt().solve(y - A*xd);                                         // xd - W^-1*A'*(A*W^-1*A')^-1*(y-A*xd)
	}
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //     Solve a problem of the form min 0.5*(xd-x)'*W*(xd-x)  s.t. A*x = y, xMin <= x <= xMax     //
///////////////////////////////////////////////////////////////////////////////////////////////////  
Eigen::VectorXf QPSolver::redundant_least_squares(const Eigen::VectorXf &xd,
                                                  const Eigen::MatrixXf &W,
                                                  const Eigen::VectorXf &y,
                                                  const Eigen::MatrixXf &A,
                                                  const Eigen::VectorXf &xMin,
                                                  const Eigen::VectorXf &xMax,
                                                  const Eigen::VectorXf &x0)
{
	unsigned int m = y.size();
	unsigned int n = x0.size();
	
	if(W.rows() != W.cols())
	{	
		throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
		                            "Expected the weighting matrix to be square but it was "
		                            + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + ".");
	}
	else if(xd.size()   != n
	     or W.rows()    != n
	     or A.cols()    != n
	     or xMin.size() != n
	     or xMax.size() != n)
	{	
		throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
		                            "Dimensions for the decision variable do not match. "
		                            "The desired vector xd had " + std::to_string(xd.size()) + " elements, "
		                            "the weighting matrix W had " + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + " elements, "
		                            "the constraint matrix A had " + std::to_string(A.cols()) + " columns, "
		                            "the xMin vector had " + std::to_string(xMin.size()) + " elements, "
		                            "the xMax vector had " + std::to_string(xMax.size()) + " elements, and "
		                            "the start point x0 had " + std::to_string(n) + " elements.");
	}
        else if(A.rows() != m)
        {	
        	throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
        	                            "Dimensions for the equality constraint do not match. "
        	                            "The y vector had " + std::to_string(y.size()) + " elements, and "
        	                            "the A matrix had " + std::to_string(A.rows()) + " rows.");
        }
	else
	{
		// Primal:
		// min 0.5*(xd - x)'*W*(xd - x)
		// subject to: A*x = y
		//             B*x > z
		
		// Lagrangian L = 0.5*(xd - x)'*W*(xd - x) + lambda'*(y - A*xd)
		
		// Dual:
		// min 0.5*lambda'*A*W^-1*A'*lambda - lambda'*(y - A*xd)
		// subject to B*x > z
		
		Eigen::MatrixXf invWAt = W.ldlt().solve(A.transpose());                             // Makes calcs a little easier
		
		Eigen::MatrixXf H = A*invWAt;                                                       // Hessian matrix for dual problem
		
		Eigen::LDLT<Eigen::MatrixXf> H_decomp; H_decomp.compute(H);                         // Perform robust Cholesky decomposition
		
		// B*x= [ -I  ] * x > z = [ -xMax ]
		//      [  I  ]           [  xMin ]
		
		Eigen::MatrixXf B(2*n,n);
		B.block(0,0,n,n) = -Eigen::MatrixXf::Identity(n,n);
		B.block(n,0,n,n).setIdentity();
				
		Eigen::VectorXf z(2*n);
		z.head(n) = -xMax;
		z.tail(n) =  xMin;
		
		// Ensure the null space projection of the desired solution xd
		// is feasible or the dual method will fail
		
		Eigen::VectorXf xn = xd - invWAt*H_decomp.solve(A*xd);
		
		float alpha = 1.0;
		
		for(int i = 0; i < n; i++)
		{	
			     if(xn(i) >= xMax(i)) alpha = std::min((float)0.9*xMax(i)/xn(i), alpha);
			else if(xn(i) <= xMin(i)) alpha = std::min((float)0.9*xMin(i)/xn(i), alpha);
		}
			
		Eigen::VectorXf new_xd = alpha*xd;                                                  // Scale so that its projection is feasible
		
		Eigen::VectorXf f = y - A*new_xd;                                                   // Vector for optimisation problem
		
		Eigen::VectorXf lambda0 = -H_decomp.solve(A*(x0-new_xd));                           // Start point
		
		this->lastSolution = new_xd - invWAt*solve(H,f,-B*invWAt,z-B*new_xd,lambda0);       // Save the solution
		
		return this->lastSolution;
		
		// Old method - slow but robust
		/*
		// Convert to standard form 0.5*x'*H*x + x'*f subject to B*x >= z
		// where "x" is now [lambda' x' ]'
		
		// H = [ 0  A ]
		//     [ A' W ]
		Eigen::MatrixXf H(m+n,m+n);
		H.block(0,0,m,m).setZero();
		H.block(0,m,m,n) = A;
		H.block(m,0,n,m) = A.transpose();
		H.block(m,m,n,n) = W;
		
		// B = [ 0 -I ]
		//     [ 0  I ]
		Eigen::MatrixXf B(2*n,m+n);
		B.block(0,0,2*n,m).setZero();
		B.block(n,m,  n,n).setIdentity();
		B.block(0,m,  n,n) = -B.block(n,m,n,n);

		// z = [ -xMax ]
		//     [  xMin ]
		Eigen::VectorXf z(2*n);
		z.head(n) = -xMax;
		z.tail(n) =  xMin;

		// f = [   -y  ]
		//     [ -W*xd ]
		Eigen::VectorXf f(m+n);
		f.head(m) = -y;
		f.tail(n) = -W*xd;
		
		Eigen::VectorXf startPoint(m+n);
		startPoint.head(m) = (A*W.partialPivLu().inverse()*A.transpose()).partialPivLu().solve(A*xd - y);
		startPoint.tail(n) = x0;
		
		return (solve(H,f,B,z,startPoint)).tail(n);                                         // Convert to standard form and solve
		
	}
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////
 //          Solve problem of the form 0.5*(xd-x)'*W*(xd-x) s.t. A*x = y, B*x > z                 //
///////////////////////////////////////////////////////////////////////////////////////////////////
Eigen::VectorXf QPSolver::redundant_least_squares(const Eigen::VectorXf &xd,
                                                  const Eigen::MatrixXf &W,
                                                  const Eigen::VectorXf &y,
                                                  const Eigen::MatrixXf &A,
                                                  const Eigen::VectorXf &z,
                                                  const Eigen::MatrixXf &B,
                                                  const Eigen::VectorXf &x0)
{
	unsigned int m = y.size();
	unsigned int n = x0.size();
	
	if(W.rows() != W.cols())
	{	
		throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
		                            "Expected the weighting matrix to be square but it was "
		                            + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + ".");
	}
	else if(xd.size() != n
	     or W.rows()  != n
	     or A.cols()  != n
	     or B.cols()  != n)
	{	
		throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
		                            "Dimensions for the decision variable do not match. "
		                            "The desired vector xd had " + std::to_string(xd.size()) + " elements, "
		                            "the weighting matrix W had " + std::to_string(W.rows()) + "x" + std::to_string(W.cols()) + " elements, "
		                            "the equality constraint matrix A had " + std::to_string(A.cols()) + " columns, "
		                            "the inequality constraint matrix B had " + std::to_string(B.cols()) + " columns, and "
		                            "the start point x0 had " + std::to_string(n) + " elements.");
	}
        else if(A.rows() != m)
        {	
        	throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
        	                            "Dimensions for the equality constraint do not match. "
        	                            "The y vector had " + std::to_string(y.size()) + " elements, and "
        	                            "the A matrix had " + std::to_string(A.rows()) + " rows.");
        }
        else if(B.rows() != z.size())
        {
        	throw std::invalid_argument("[ERROR] [QP SOLVER] redundant_least_squares(): "
        	                            "Dimensions for constraints do not match. "
        	                            "The inequality constraint matrix B had " + std::to_string(B.rows()) + " rows, and "
        	                            "the inequality constraint vector z had " + std::to_string(z.size()) + " elements.");
        }
        else
        {
		try
		{
			Eigen::MatrixXf invWAt = W.ldlt().solve(A.transpose());                     // Makes calcs a little easier
			
			Eigen::MatrixXf H = A*invWAt;                                               // Hessian
			
			this->lastSolution = xd - invWAt * solve(H,
				                                 y - A*xd,                          // Vector f
				                                -B*invWAt,                          // Modified matrix B
				                                 z - B*xd,                          // Modified constraint vector
				                                -H.ldlt().solve(A*(x0-xd)));        // Start point for solver
			
			return this->lastSolution;
		}
		catch(const std::exception &exception)
		{
			std::cout << exception.what() << std::endl;                                 // Print lower-level error
			
			throw std::runtime_error("[ERROR] [QP SOLVER] redundant_least_squares(): "
			                         "Ensure that the projection of the desired value xd "
			                         "satisfies the constraints.");
		}
	}	
}
*/
