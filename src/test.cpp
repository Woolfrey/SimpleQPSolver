#include <iostream>                                                                                 // std::cout
#include <fstream>                                                                                  // std::ofstream
#include <QPSolver.h>                                                                               // Custom cass
#include <time.h>                                                                                   // clock_t

int main(int argc, char *argv[])
{
	// Variables used in this scope
	float t;
	clock_t timer;
	unsigned int m, n;                                                                          // Dimensions for the various problems.
	Eigen::MatrixXf H, A, comparison;                                                                      
	Eigen::VectorXf f, x, xMin, xMax, y;                                                                       
	QPSolver<float> solver;                                                                     // Create an instance of the class
	srand((unsigned int) time(NULL));                                                           // Seed the random number generator
	
	std::cout << "\n**********************************************************************\n"
	          <<   "*                        A GENERIC QP PROBLEM                        *\n"
	          <<   "**********************************************************************\n" << std::endl;
	          
	m = 5;
	n = 5;
	
	Eigen::MatrixXf temp = Eigen::MatrixXf::Random(n,n);
	H = (temp + temp.transpose())/2;
	f = Eigen::VectorXf::Random(n);
	
	std::cout << "\nThe SimpleQPSolver can be used to find solutions to generic quadratic programming "
	          << "problems of the form:\n"
	          << "\n     min 0.5*x'*H*x + x'*f \n"
	          << "\nwhere:\n"
	          << "   - x (nx1) is the decision variable,\n"
	          << "   - H (nxn) is a positive semi-definite matrix such that H = H', and\n"
	          << "   - f (nx1) is a vector for the linear component of the quadratic equation.\n"
	          << "\nFor this type of problem is is possible to call the class method without creating "
	          << "an object. For example, here is the H matrix (Hessian) for a " << m << "x" << n << " system:\n";
	
	std::cout << "\n" << H << std::endl;
	
	std::cout << "\nAnd here is the f vector (transposed):\n";
	
	std::cout << "\n" << f.transpose() << std::endl;
	
	std::cout << "\nThen we can call `Eigen::VectorXf x = QPSolver<float>::solve(H,f);' to get:\n";
	
	timer = clock();
	x = QPSolver<float>::solve(H,f);
	timer = clock() - timer;
	t  = (float)timer/CLOCKS_PER_SEC;
	
	std::cout << "\n" << x.transpose() << std::endl;
	
	std::cout << "\nIt took " << t*1000 << " ms to solve (" << 1/t << " Hz).\n"
	          << "\n(You could also use doubles here with Eigen::VectorXd x = QPSolver<double>::solve(H,f)).\n";
	          
	std::cout << "\n**********************************************************************\n"
	          <<   "*                      UNDERDETERMINED SYSTEMS                       *\n"
	          <<   "**********************************************************************\n" << std::endl;
	
	m = 10;
	n = 6;
	
	A = Eigen::MatrixXf::Random(m,n);
	y = A*Eigen::VectorXf::Random(n);
        
	std::cout << "\nThe QP solver can be used to minimize problems of the form:\n"
	          << "\n     min 0.5*|| y - A*x ||^2 = 0.5*(y - A*x)'*W*(y - A*x)\n"
	          << "\nwhere:\n"
	          << " - y (mx1) is given,\n" 
	          << " - A (mxn) is also given,\n"
	          << " - W (mxm) is a weighting matrix, and\n"
	          << " - x (nx1) is the decision variable.\n"
	          << "\nIt is assumed that m > n so the equations are underdetermined. "
	          << "This is a classic linear regression / linear least squares problem.\n";
	
	std::cout << "\nFor example, here is a " << m << "x" << n << " system where A is:\n";
	
	std::cout << "\n" << A << std::endl;
	
	std::cout << "\nAnd y (transposed) is:\n";
	
	std::cout << "\n" << y.transpose() << std::endl;
	
	std::cout << "\nWe can find an approximate solution by calling "
	          << "'Eigen::VectorXf x = QPSolver<float>least_squares(y,A,W)':\n";
	          
	timer = clock();
	x = QPSolver<float>::least_squares(y,A,Eigen::MatrixXf::Identity(m,m));
	timer = clock() - timer;
	t  = (float)timer/CLOCKS_PER_SEC;	
	
	std::cout << "\n" << x.transpose() << std::endl;
	
	std::cout << "\nThe error ||y - A*x|| is: " << (y - A*x).norm() << ", "
	          <<   "and it took " << t*1000 << " ms to solve (" << 1/t << " Hz).\n";
	          
	std::cout << "\n**********************************************************************\n"
	          <<   "*                      OVERDETERMINED SYSTEMS                        *\n"
	          <<   "**********************************************************************\n" << std::endl;	 
	          
	m = 6;
	n = 7;
	
	A = Eigen::MatrixXf::Random(m,n);
	y = A*Eigen::VectorXf::Random(n);
	
	std::cout << "\nWe can also solve systems where the solution is *over* determined. "
	          <<   "For example, the matrix A here is " << m << "x" << n << ":\n";
		
	std::cout << "\n" << A << std::endl;
	
	std::cout << "\nAnd the y vector (transposed) is:\n";
	
	std::cout << "\n" << y.transpose() << std::endl;
	         
	std::cout << "\nSince m < n there are infinite possible solutions. We can give the QP solver a "
	          <<   "desired value xd for the solution. This problem takes the form:\n";
	          
	std::cout << "\n      min 0.5*(xd - x)'*W*(xd - x)\n"
	          <<   "     subject to: A*x = y\n";
	
	std::cout << "\nWhere W (nxn) is a weighting matrix. We can then call "
	          <<   "'Eigen::VectorXf x = QPSolver<float>redundant_least_squares(xd,W,A,y);' to get:\n";
	          
	timer = clock();
	x = QPSolver<float>::redundant_least_squares(Eigen::VectorXf::Random(n),
	                                             Eigen::MatrixXf::Identity(n,n),
	                                             A, y);
	timer = clock() - timer;
	t  = (float)timer/CLOCKS_PER_SEC;	
	
	std::cout << "\n" << x.transpose() << std::endl;

	std::cout << "\nThe error ||y - A*x|| is: " << (y - A*x).norm() << ", "
	          <<   "and it took " << t*1000 << " ms to solve (" << 1/t << " Hz).\n";
	          
	std::cout << "\n**********************************************************************\n"
	          <<   "*                      CONSTRAINED SYSTEMS                           *\n"
	          <<   "**********************************************************************\n" << std::endl;
	          
	m = 6;
	n = 5;
	
	A = Eigen::MatrixXf::Random(m,n);
	x.resize(n); x << -3, 5, -1, 4, 0;
	y = A*x;
	
	xMin = -5*Eigen::VectorXf::Ones(n);
	xMax =  5*Eigen::VectorXf::Ones(n); xMax(1) = 2.7; xMax(3) = 3.6;                           // Manually override the limits
	          
	std::cout << "\nOften there are constrained on the solution. A generic form of this problem is:\n"
	          << "\n     min 0.5*x'*H*x + x'*f\n"
	          <<   "     subject to: B*x <= z\n"
	          << "\nwhere:\n"
	          << "   - B (cxn) is a constraint matrix, and\n"
	          << "   - z (cx1) is a constraint vector.\n"
	          << "\nFor this problem, we need to create an object: 'QPSolver<float> solver;\n"
	          << "Since this uses an iterative method for determining a solution, the solver also requires an initial guess x0 (nx1).\n"
	          << "We would call: `Eigen::VectorXf x = solver.solve(H,f,B,z,x0);'\n";
	          
	std::cout << "\nA more specific example is for a constrained least squares problem of the form:\n"
                  << "\n     min 0.5*(y - A*x)'*W*(y - A*x)\n"
                  << "\n     subject to: xMin <= x <= xMax\n"
                  << "\nwhere xMin (nx1) and xMax(nx1) are lower and upper bounds on the solution.\n"
                  << "\nThe equivalent constraints here are:\n"
                  << "\n    B = [  I ]  z = [ xMax ]\n"
                  <<   "        [ -I ]      [ xMin ]\n"
                  << "\n(I wrote special function for this case because I am lazy.)\n";     
                  
        std::cout << "\nFor the following system of A:\n";
        
        std::cout << "\n" << A << std::endl;
        
        std::cout << "\n and y (transposed):\n";
        
        std::cout << "\n" << y.transpose() << std::endl;
        
        std::cout << "\nWe can call 'Eigen::VectorXf x = solver.least_squares(y,A,W,xMin,xMax,x0);'\n";
        
	timer = clock();
	x = solver.constrained_least_squares(y,A,Eigen::MatrixXf::Identity(m,m),xMin,xMax,0.5*(xMin+xMax));
	timer = clock() - timer;
	t  = (float)timer/CLOCKS_PER_SEC;
	
	std::cout << "\nHere is xMin, the solution x, and xMax side-by-side:\n";
	comparison.resize(n,3); 
	comparison.col(0) = xMin;
	comparison.col(1) = x;
	comparison.col(2) = xMax;
	std::cout << "\n" << comparison << std::endl;

	std::cout << "\nThe error ||y - A*x|| is: " << (y - A*x).norm() << ", "
	          <<   "and it took " << t*1000 << " ms to solve (" << 1/t << " Hz).\n";
	          

	std::cout << "\n**********************************************************************\n"
	          <<   "*                CONSTRAINED SYSTEMS (REDUNDANT CASE)                *\n"
	          <<   "**********************************************************************\n" << std::endl;
	          
	m = 12;
	n = 17;
	
	A = Eigen::MatrixXf::Random(m,n);
	Eigen::VectorXf xTrue = Eigen::VectorXf::Random(n);
	y = A*xTrue;
	
	xMin = -5*Eigen::VectorXf::Ones(n);
	xMax =  5*Eigen::VectorXf::Ones(n);
	
	Eigen::VectorXf xd = 10*Eigen::VectorXf::Random(n);
	
	std::cout << "\nWe can even solve redundant systems subject to constraint:\n"
	          << "\n      min 0.5*(xd - x)'*W*(xd - x)\n"
	          << "     subject to: A*x = y\n"
	          << "                 B*x < z\n";
	          
	std::cout << "\nAgain, there is a function for least squares problems with lower and upper "
	          << "bounds on the solution:\n"
	          << "\n      B*x <= z  <--->  xMin <= x <= xMax\n";
	
	std::cout << "\nWe would call: 'solver.constrained_least_squares(xd,W,A,y,xMin,xMax,x0)'\n"
	          << "\nThere are two options for this case:\n"
	          << "\n  1. solver.use_dual() which is faster, but sensitive to initial guess x0, and\n"
	          <<   "  2. solver.use_primal() which is slower, but robust.\n";
	
	std::cout << "\nHere is the solution for a " << m << "x" << n << " system using the dual method:\n";
	
	timer = clock();
	x = solver.constrained_least_squares(xd,Eigen::MatrixXf::Identity(n,n),A,y,xMin,xMax,0.5*(xMin + xMax));
	timer = clock() - timer;
	float t1  = (float)timer/CLOCKS_PER_SEC;
	
	comparison.resize(n,3); 
	comparison.col(0) = xMin;
	comparison.col(1) = x;
	comparison.col(2) = xMax;
	std::cout << "\n" << comparison << std::endl;
	
	float error1 = (y - A*x).norm();
	
	std::cout << "\nThe error ||y - A*x|| is: " << error1 << ", "
	          <<   "and it took " << t1*1000 << " ms to solve (" << 1/t1 << " Hz).\n";
	
	solver.use_primal();
	
	std::cout << "\nUsing the primal method we get:\n";

	timer = clock();
	x = solver.constrained_least_squares(xd,Eigen::MatrixXf::Identity(n,n),A,y,xMin,xMax,0.5*(xMin+xMax));
	timer = clock() - timer;
	float t2  = (float)timer/CLOCKS_PER_SEC;
	
	float error2 = (y - A*x).norm();
	
	comparison.col(1) = x;
	std::cout << "\n" << comparison << std::endl;
	
	std::cout << "\nThe error ||y - A*x|| is: " << error2 << ", "
	          <<   "and it took " << t2*1000 << " ms to solve (" << 1/t2 << " Hz).\n";
	          
	std::cout << "\nThe dual method was " << t2/t1 << " times faster. "
	          << "The primal method was " << error1/error2 << " times more accurate.\n";
	
/*	srand((unsigned int) time(0));					                            // Random seed generator
	
	// Variables used in this scope
	int m, n;
	Eigen::MatrixXf A, comparison, W;
	Eigen::VectorXf x, xd, xHat, x0, xMin, xMax, y;
	QPSolver solver;
	clock_t timer;
	float time;
	
	std::cout << "\n************************************************************\n"
	          <<   "*                  UNDERDETERMINED SYSTEMS	           *\n"
	          <<   "************************************************************\n" << std::endl;
	          
	m = 6;
	n = 5;
	A  = Eigen::MatrixXf::Random(m,n);
	x  = Eigen::VectorXf::Random(n);
	y  = A*x;
	W  = Eigen::MatrixXf::Identity(m,m);
	
	std::cout << "\nHere is an underdetermined system y = A*x.\n" << std::endl;
	
	std::cout << "\nA:\n" << std::endl;
	std::cout << A << std::endl;
	
	std::cout << "\ny:\n" << std::endl;
	std::cout << y << std::endl;
	
	std::cout << "\nWe can use quadratic programming (QP) to get the best estimate of x.\n" << std::endl;
	
	timer = clock();
	xHat  = QPSolver::least_squares(y,A,W);
	timer = clock() - timer;
	time  = (float)timer/CLOCKS_PER_SEC;
	
	std::cout << "\nHere is the estimate for x:\n" << std::endl;
	std::cout << xHat << std::endl;
	
	std::cout << "\nThe error ||y-A*x|| is " << (y-A*xHat).norm() << ". "
	          << "It took " << time*1000 << " ms to solve (" << 1/time << " Hz).\n" << std::endl;
	          
	std::cout << "\n************************************************************\n"
	          <<   "*                    CONSTRAINED SYSTEM                    *\n"
	          <<   "************************************************************\n" << std::endl;
	
	m = 8;
	n = 8;
	
	A = Eigen::MatrixXf::Random(m,n);
	x.resize(n);
	x << 1, 2, 3, 4, 5, 3, 2, 1;
	
	y = A*x;
	W = Eigen::MatrixXf::Identity(m,m);
	
	xMin = -6*Eigen::VectorXf::Ones(n);
	xMax =  6*Eigen::VectorXf::Ones(n);
	xMax(2) = 0.9*x(2);
	xMax(3) = 0.9*x(3);
	
	x0   = 0.5*(xMin + xMax);
	
	std::cout << "\nConsider the problem to minimize ||y-A*x|| for xMin <= x <= xMax.\n" << std::endl;
	
	std::cout << "\nHere is A:\n" << std::endl;
	std::cout << A << std::endl;
	
	std::cout << "\nand y:\n" << std::endl;
	std::cout << y << std::endl;
	
	timer = clock();
	xHat  = solver.least_squares(y,A,W,xMin,xMax,x0);
	timer = clock() - timer;
	time  = (float)timer/CLOCKS_PER_SEC;
	
	std::cout << "\nHere is xMin, the estimate for x, and xMax side-by-side:\n" << std::endl;
	comparison.resize(n,3);
	comparison.col(0) = xMin;
	comparison.col(1) = xHat;
	comparison.col(2) = xMax;
	std::cout << comparison << std::endl;
	
	std::cout << "\nThe error norm ||y-A*x|| is " << (y-A*xHat).norm() << ". "
	          << "It took " << time*1000 << " ms to solve (" << 1/time << " Hz)." << std::endl;
        
	std::cout << "\n************************************************************\n"
	          <<   "*                 OVERDETERMINED SYSTEMS                   *\n"
	          <<   "************************************************************\n" << std::endl;
	m = 12;
	n = 17;

	x = Eigen::VectorXf::Random(n);
	A = Eigen::MatrixXf::Random(m,n);
	y = A*x;
	
	xMin = -5*Eigen::VectorXf::Ones(n);
	xMax =  5*Eigen::VectorXf::Ones(n);
	
	x0 = 0.5*(xMin + xMax);
	
	xd = 10*Eigen::VectorXf::Random(n);
	
	try
	{
		timer = clock();
		xHat  = solver.redundant_least_squares(xd,Eigen::MatrixXf::Identity(n,n),y,A,xMin,xMax,x0);
		timer = clock() - timer;
		time  = (float)timer/CLOCKS_PER_SEC;
	
		std::cout << "\nHere is xMin, x, xHat, and xMax:\n" << std::endl;
		comparison.resize(n,3);
		comparison.col(0) = xMin;
		comparison.col(1) = xHat;
		comparison.col(2) = xMax;
		std::cout << comparison << std::endl;

		std::cout << "\nThe error norm ||y - A*x|| is " << (y-A*xHat).norm() << ". "
			  << "It took " << time*1000 << " ms to solve (" << 1/time << " Hz)." << std::endl;
	}
	catch(const std::exception &exception)
	{
		std::cout << exception.what() << std::endl;
	}

	// Record statistics on performance
	std::ofstream out("qp_test_data.csv");
	for(int i = 0; i < 100; i++)
	{
		x = Eigen::VectorXf::Random(n);
		A = Eigen::MatrixXf::Random(m,n);
		y = A*x;
		
		xMin = -5*Eigen::VectorXf::Ones(n);
		xMax =  5*Eigen::VectorXf::Ones(n);
		
		x0 = 0.5*(xMin + xMax);
		
		xd = 10*Eigen::VectorXf::Random(n);
		
		timer = clock();
		xHat  = solver.redundant_least_squares(xd,Eigen::MatrixXf::Identity(n,n),y,A,xMin,xMax,x0);
		timer = clock() - timer;
		
		out << (float)timer/CLOCKS_PER_SEC << "," << (y-A*xHat).norm() << "\n";
	}
*/
	return 0; 
}
