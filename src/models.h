void logitMetro(int *Y, double **X, double *beta, int n_samp,      
		int n_dim, int n_cov, double *beta0, double **A0,     
		double *Var, int n_gen, int *counter);

void bNormalReg(double *Y, double **X, double *beta, double sig2, 
		int n_samp, int n_cov, int pbeta, double *beta0, 
		double **A0, int psig2, double s0, int nu0, 
		int sig2fixed);
  
void bprobitGibbs(int *Y, double **X, double *beta, int n_samp, 
		  int n_cov, int prior, double *beta0, double **A0, 
		  int mda, int n_gen);