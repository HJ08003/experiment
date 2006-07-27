#include <stddef.h>
#include <stdio.h>      
#include <string.h>
#include <math.h>
#include <Rmath.h>
#include <R.h>
#include "vector.h"
#include "subroutines.h"
#include "rand.h"
#include "models.h"

void R2bNormalReg(double *Y,        /* binary outcome variable */
		  double *dX,       /* model matrix */
		  double *beta,     /* fixed effects coefficients */
		  double *dsig2,    /* variance parameter */
		  int *n_samp,      /* # of obs */ 
		  int *n_cov,    /* # of covariates */
		  int *n_gen,    /* # of gibbs draws */
		  int *pbeta,     /* 0: improper prior 
				    p(beta|X) \propto 1
				    1: proper prior for (beta)
				    p(beta|X) = normal(beta0, A0)
				 */
		  double *beta0, /* prior mean for normal */
		  double *dA0,   /* prior precision for normal; can be
				    set to zero to induce improper prior
				    for beta alone
				 */
		  int *psig2,    /* 0: improper prior for sig2
				    p(sig2|X) \propto 1/sig2
				    1: proper prior for sig2
				    p(sigma2|X) = InvChi2(nu0, s0)
				 */
		  double *s0,    /* prior scale for InvChi2 */
		  int *nu0,      /* prior d.f. for InvChi2 */
		  int *sig2fixed, /* 1: sig2 fixed, 0: sig2 sampled */ 
		  double *betaStore, 
		  double *sig2Store
		  ) {

  /* storage parameters and loop counters */
  int i, j, k, main_loop, itemp;  
  int ibeta = 0, isig2 = 0;
  double sig2 = *dsig2;

  /* matrices */
  double **X = doubleMatrix(*n_samp+*n_cov, *n_cov+1);
  double **A0 = doubleMatrix(*n_cov, *n_cov);
  double **mtemp = doubleMatrix(*n_cov, *n_cov);

  /* get random seed */
  GetRNGstate();

  /* packing the data */
  itemp = 0;
  for (j = 0; j < *n_cov; j++)
    for (i = 0; i < *n_samp; i++) 
      X[i][j] = dX[itemp++];

  /* packing the prior */
  itemp = 0; 
  for (k = 0; k < *n_cov; k++)
    for (j = 0; j < *n_cov; j++)
      A0[j][k] = dA0[itemp++];

  /* adding prior as an additional data point */
  dcholdc(A0, *n_cov, mtemp);
  for (i = 0; i < *n_cov; i++) {
    X[*n_samp+i][*n_cov]=0;
    for (j = 0; j < *n_cov; j++) {
      X[*n_samp+i][*n_cov] += mtemp[i][j]*beta0[j];
      X[*n_samp+i][j] = mtemp[i][j];
    }
  }

  /* Gibbs Sampler! */
  for(main_loop = 1; main_loop <= *n_gen; main_loop++) {
    bNormalReg(Y, X, beta, sig2, *n_samp, *n_cov, *pbeta, beta0, A0,
	       *psig2, *s0, *nu0, *sig2fixed);

    /* Storing the output */
    for (j = 0; j < *n_cov; j++)
      betaStore[ibeta++] = beta[j];
    sig2Store[isig2++] = sig2;

    R_FlushConsole(); 
    R_CheckUserInterrupt();
  } /* end of Gibbs sampler */

  PutRNGstate();

  /* freeing memory */
  FreeMatrix(X, *n_samp+*n_cov);
  FreeMatrix(A0, *n_cov);
  FreeMatrix(mtemp, *n_cov);
}



void R2bprobitMixedGibbs(int *Y,           /* binary outcome variable */
			 double *dX,       /* model matrix for fixed
					      effects */
			 double *dZ,       /* model matrix for random
					      effects */
			 int *grp,         /* group indicator: 0, 1, 2,... */
			 double *beta,     /* fixed effects coefficients */
			 double *dgamma,   /* random effects coefficients */
			 double *dPsi,     /* covariance for random
					      effects */
			 int *n_samp,      /* # of obs */ 
			 int *n_fixed,     /* # of fixed effects */
			 int *n_random,    /* # of random effects */
			 int *n_grp,       /* # of groups */
			 int *n_samp_grp,  /* # of obs within group */
			 int *max_samp_grp, /* max # of obs within group */
			 double *beta0,    /* prior mean */
			 double *dA0,      /* prior precision */
			 int *tau0,        /* prior df */
			 double *dT0,      /* prior scale */
			 int *mda,         /* Want to use marginal data
					      augmentation for fixed effects ? */ 
			 int *n_gen,       /* # of gibbs draws */
			 /* storage of MCMC draws */
			 double *betaStore, 
			 double *gammaStore,
			 double *PsiStore
		       ) {

  /* storage parameters and loop counters */
  int i, j, k, main_loop, itemp;  
  int *vitemp = intArray(*n_grp);
  int ibeta = 0, igamma = 0, iPsi =0;

  /* matrices */
  double **X = doubleMatrix(*n_samp+*n_fixed, *n_fixed+1);
  double **Z = doubleMatrix(*n_samp, *n_random+1);
  double **gamma = doubleMatrix(*n_grp, *n_random);
  double **Psi = doubleMatrix(*n_random, *n_random);
  double **A0 = doubleMatrix(*n_fixed, *n_fixed);
  double **T0 = doubleMatrix(*n_random, *n_random);
  double **mtemp = doubleMatrix(*n_fixed, *n_fixed);
  double ***Zgrp = doubleMatrix3D(*n_grp, *max_samp_grp + *n_random, *n_random+1);

  /* get random seed */
  GetRNGstate();

  /* packing the data */
  itemp = 0;
  for (j = 0; j < *n_fixed; j++)
    for (i = 0; i < *n_samp; i++) 
      X[i][j] = dX[itemp++];

  itemp = 0;
  for (j = 0; j < *n_random; j++)
    for (i = 0; i < *n_samp; i++)
      Z[i][j] = dZ[itemp++];

  for (j = 0; j < *n_grp; j++)
    vitemp[j] = 0;
  for (i = 0; i < *n_samp; i++) {
    for (j = 0; j < *n_random; j++)
      Zgrp[grp[i]][vitemp[grp[i]]][j] = Z[i][j];
    vitemp[grp[i]]++;
  }
  
  /* packing the prior */
  itemp = 0;
  for (k = 0; k < *n_random; k++)
    for (j = 0; j < *n_random; j++)
      Psi[j][k] = dPsi[itemp++];
  
  itemp = 0;
  for (k = 0; k < *n_random; k++)
    for (j = 0; j < *n_grp; j++)
      gamma[j][k] = dgamma[itemp++];

  itemp = 0; 
  for (k = 0; k < *n_fixed; k++)
    for (j = 0; j < *n_fixed; j++)
      A0[j][k] = dA0[itemp++];

  itemp = 0; 
  for (k = 0; k < *n_random; k++)
    for (j = 0; j < *n_random; j++)
      T0[j][k] = dT0[itemp++];

  /* adding prior as an additional data point */
  dcholdc(A0, *n_fixed, mtemp);
  for (i = 0; i < *n_fixed; i++) {
    X[*n_samp+i][*n_fixed]=0;
    for (j = 0; j < *n_fixed; j++) {
      X[*n_samp+i][*n_fixed] += mtemp[i][j]*beta0[j];
      X[*n_samp+i][j] = mtemp[i][j];
    }
  }

  /* Gibbs Sampler! */
  for(main_loop = 1; main_loop <= *n_gen; main_loop++) {
    bprobitMixedGibbs(Y, X, Z, Zgrp, grp, beta, gamma, Psi, *n_samp,
		      *n_fixed, *n_random, *n_grp, n_samp_grp,
		      *max_samp_grp, 0, beta0, A0, *tau0, T0, *mda, 1);

    /* Storing the output */
    for (j = 0; j < *n_fixed; j++)
      betaStore[ibeta++] = beta[j];
    for (j = 0; j < *n_random; j++)
      for (k = j; k < *n_random; k++)
	PsiStore[iPsi++] = Psi[j][k];
    for (j = 0; j < *n_grp; j++)
      for (k = 0; k < *n_random; k++)
	gammaStore[igamma++] = gamma[j][k];

    R_FlushConsole(); 
    R_CheckUserInterrupt();
  } /* end of Gibbs sampler */

  PutRNGstate();

  /* freeing memory */
  free(vitemp);
  FreeMatrix(X, *n_samp+*n_fixed);
  FreeMatrix(Z, *n_samp);
  FreeMatrix(gamma, *n_grp);
  FreeMatrix(Psi, *n_random);
  FreeMatrix(A0, *n_fixed);
  FreeMatrix(T0, *n_random);
  FreeMatrix(mtemp, *n_fixed);
  Free3DMatrix(Zgrp, *n_grp, *max_samp_grp + *n_random);
}
