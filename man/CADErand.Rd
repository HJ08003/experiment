% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/CADErand.R
\name{CADErand}
\alias{CADErand}
\title{Randomization-based method for the complier average direct effect and the complier average spillover effect}
\usage{
CADErand(data)
}
\arguments{
\item{data}{A data frame containing the relevant variables. The names for the variables should be: ``Z'' for the treatment assignment,  ``D''  for the actual received treatment, ``Y'' for the outcome, ``A'' for the treatment assignment mechanism and ``id'' for the cluster ID. The variable for the cluster id should be a factor.}
}
\value{
A list of class \code{CADErand} which contains the following items:
\item{CADE1}{ The point estimate of CADE(1).  } \item{CADE0}{ The point estimate of CADE(0).  } 
\item{CADE1}{ The point estimate of CASE(1).  } \item{CASE0}{ The point estimate of CASE(0).  } 
\item{var.CADE1}{ The  variance estimate of CADE(1).   } 
\item{var.CADE0}{ The  variance estimate of CADE(0).   } 
\item{var.CASE1}{ The  variance estimate of CASE(1).   } 
\item{var.CASE0}{ The  variance estimate of CASE(0).   }
}
\description{
This function computes the point estimates and variance estimates of the complier average direct effect (CADE)  and the complier average spillover effect (CASE).
The estimators calculated using this function are cluster-weighted, i.e., the weights are equal for each cluster. To obtain the indivudal-weighted
 estimators, please multiply the recieved treatment and the outcome by \code{n_jJ/N}, where
 \code{n_j} is the number of individuals in cluster \code{j}, \code{J} is the number of clusters and 
 \code{N} is the total number of individuals.
}
\details{
For the details of the method implemented by this function, see the
references.
}
\references{
Kosuke Imai, Zhichao Jiang and Anup Malani (2018).
\dQuote{Causal Inference with Interference and Noncompliance in the Two-Stage Randomized Experiments}, \emph{Technical Report}. Department of Politics, Princeton
University.
}
\author{
Kosuke Imai, Department of Politics, Princeton University
\email{kimai@Princeton.Edu}, \url{http://imai.princeton.edu};
Zhichao Jiang, Department of Politics, Princeton University
\email{zhichaoj@princeton.edu}.
}
\keyword{experiments}
\keyword{randomized}
\keyword{two-stage}
