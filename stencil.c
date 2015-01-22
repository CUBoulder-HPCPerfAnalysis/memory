#define _XOPEN_SOURCE

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CHK(err) do { if (err) { fprintf(stderr,"Error %d at %s:%d %s()\n",err,__FILE__,__LINE__,__func__); exit(err); } } while (0)
static double Sqr(double x) { return x*x; }

// Timing information
typedef unsigned long long cycles_t;
cycles_t rdtsc() {
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ((cycles_t)lo)|( ((cycles_t)hi)<<32);
}

typedef struct {
  long m[3]; // Grid dimensions
  double L[3]; // Grid extent [0,Lx]×[0,Ly]×[0,Lz]
} *Grid;

static int GridCreateVector(Grid g,double **x) {
  *x = malloc(g->m[0]*g->m[1]*g->m[2]*sizeof(double));
  double (*xx)[g->m[1]][g->m[2]] = (double(*)[g->m[1]][g->m[2]])*x;
  #pragma omp parallel for
  for (int i=0; i<g->m[0]; i++) {
    for (int j=0; j<g->m[1]; j++) {
      for (int k=0; k<g->m[2]; k++) {
        xx[i][j][k] = 0.;
      }
    }
  }
  return 0;
}
static int GridInterior(Grid g,int i,int j,int k) {
  return (   0 < i && i < g->m[0]-1
          && 0 < j && j < g->m[1]-1
          && 0 < k && k < g->m[2]-1);
}

static double Wave(double x) {
  return (x*x*x*x - x*x + 2*x*x*x - 2*x*x*x*x*x) / 0.14;}
static double Wave_xx(double x) {
  return (12*x*x - 2 + 12*x - 40*x*x*x) / 0.14;}
static int SetupProblem(Grid g,double *u,double *b) {
  double (*uu)[g->m[1]][g->m[2]] = (double(*)[g->m[1]][g->m[2]])u;
  double (*bb)[g->m[1]][g->m[2]] = (double(*)[g->m[1]][g->m[2]])b;

  #pragma omp parallel for
  for (int i=0; i<g->m[0]; i++) {
    for (int j=0; j<g->m[1]; j++) {
      for (int k=0; k<g->m[2]; k++) {
        const double *L = g->L;
        const double x[] = {L[0]*i / (g->m[0]-1), L[1]*j / (g->m[1]-1), L[2]*k / (g->m[2]-1)};
        uu[i][j][k] = Wave(x[0]/L[0]) * Wave(x[1]/L[1]) * Wave(x[2]/L[2]);
        if (GridInterior(g,i,j,k)) {
          bb[i][j][k] = -(  Wave_xx(x[0]/L[0]) * Wave   (x[1]/L[1]) * Wave   (x[2]/L[2])/Sqr(L[0])
                          + Wave   (x[0]/L[0]) * Wave_xx(x[1]/L[1]) * Wave   (x[2]/L[2])/Sqr(L[1])
                          + Wave   (x[0]/L[0]) * Wave   (x[1]/L[1]) * Wave_xx(x[2]/L[2])/Sqr(L[2]));
        } else bb[i][j][k] = uu[i][j][k];
      }
    }
  }
  return 0;
}

// Update u -= D^{-1} (A u - b) and compute 2-norm of unpreconditioned residual
static int Jacobi_7pt(Grid g,double *u,const double *b,double w,double *rnorm2) {
  double (*uu)[g->m[1]][g->m[2]] = (double(*)[g->m[1]][g->m[2]])u;
  const double (*bb)[g->m[1]][g->m[2]] = (const double(*)[g->m[1]][g->m[2]])b;
  double h[] = {g->L[0]/(g->m[0]-1),g->L[1]/(g->m[1]-1),g->L[2]/(g->m[2]-1)};
  double sum = 0;

  #pragma omp parallel for reduction(+:sum)
  for (int i=0; i<g->m[0]; i++) {
    for (int j=0; j<g->m[1]; j++) {
      for (int k=0; k<g->m[2]; k++) {
        double diag = 2/Sqr(h[0]) + 2/Sqr(h[1]) + 2/Sqr(h[2]);
        double residual;
        if (GridInterior(g,i,j,k)) {
          residual = diag * uu[i][j][k]
            - (uu[i-1][j][k] + uu[i+1][j][k])/Sqr(h[0])
            - (uu[i][j-1][k] + uu[i][j+1][k])/Sqr(h[1])
            - (uu[i][j][k-1] + uu[i][j][k+1])/Sqr(h[2])
            - bb[i][j][k];
        } else {
          diag = 1;
          residual = uu[i][j][k] - bb[i][j][k];
        }
        uu[i][j][k] -= (w/diag) * residual;
        sum += residual*residual;
      }
    }
  }
  *rnorm2 = sqrt(sum);
  return 0;
}

// Compute algebraic norm |u-v|_2 if v is non-NULL, otherwise |u|_2
static int Norm(Grid g,const double *u,const double *v,double *norm2,double *normMax) {
  double sum = 0,max = 0;
  #pragma omp parallel for reduction(+:sum)
  for (int i=0; i<g->m[0]*g->m[1]*g->m[2]; i++) {
    double x = u[i] - (v ? v[i] : 0.);
    sum += Sqr(x);
    if (fabs(x) > max) max = fabs(x);
  }
  *norm2 = sqrt(sum);
  *normMax = max;
  return 0;
}

static int usage(const char argv0[]) {
  fprintf(stderr,"Usage: %s -m mx[,my[,mz]] [-L Lx[,Ly[,Lz]]] [-n iterations] [-w omega] [-v]\n",argv0);
  return 1;
}

int main(int argc, char *argv[])
{
  Grid g = calloc(1,sizeof(*g));
  g->L[0] = g->L[1] = g->L[2] = 1.0;
  double *u,*uexact,*b,w = 1.0,bnorm2,bnormMax,unorm2,unormMax,enorm2,enormMax;
  int err,opt,niterations = 10,verbose = 0;
  cycles_t *cyclelog;

  while ((opt = getopt(argc,argv,"m:L:n:w:v")) != -1) {
    switch (opt) {
    case 'm': { // Number of grid points
      const char *ptr = optarg;
      for (int i=0; i<3; i++) {
        if (ptr) {
          char *end;
          g->m[i] = strtol(ptr,&end,10);
          if (*end == ',') ptr = end+1;
          else if (*end == '\0') ptr = NULL;
          else return usage(argv[0]);
        } else g->m[i] = g->m[i-1];
      }
      if (ptr) return usage(argv[0]); // User provided more than 3 args
    } break;
    case 'L': { // Physical extent of domain
      const char *ptr = optarg;
      for (int i=0; i<3; i++) {
        if (ptr) {
          char *end;
          g->L[i] = strtod(ptr,&end);
          if (*end == ',') ptr = end+1;
          else if (*end == '\0') ptr = NULL;
          else return usage(argv[0]);
        } else g->m[i] = g->m[i-1];
      }
      if (ptr) return usage(argv[0]); // User provided more than 3 args
    } break;
    case 'n':
      niterations = strtol(optarg,NULL,10);
      break;
    case 'w':
      w = strtod(optarg,NULL);
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return usage(argv[0]);
    }
  }
  if (!g->m[0]) return usage(argv[0]);

  printf("m %ld,%ld,%ld  L %f %f %f\n",g->m[0],g->m[1],g->m[2],g->L[0],g->L[1],g->L[2]);

  err = GridCreateVector(g,&u);CHK(err);
  err = GridCreateVector(g,&uexact);CHK(err);
  err = GridCreateVector(g,&b);CHK(err);

  err = SetupProblem(g,uexact,b);CHK(err);

  err = Norm(g,b,NULL,&bnorm2,&bnormMax);CHK(err);
  err = Norm(g,uexact,NULL,&unorm2,&unormMax);CHK(err);
  printf("L2 norm of exact solution |u|_2 %10.4e  |u|_max %10.4e\n",unorm2*(g->L[0]*g->L[1]*g->L[2])/(g->m[0]*g->m[1]*g->m[2]),unormMax);

  cyclelog = malloc(niterations*sizeof(cyclelog[0]));
  for (int i=0; i<niterations; i++) {
    double rnorm;
    cyclelog[i] = rdtsc();
    err = Jacobi_7pt(g,u,b,w,&rnorm);CHK(err);
    cyclelog[i] = rdtsc() - cyclelog[i];
    if (verbose) printf("Jacobi iteration % 3d  |r| %10.4e  |r|/|b| %10.4e\n",i,rnorm,rnorm/bnorm2);
  }

  err = Norm(g,u,uexact,&enorm2,&enormMax);CHK(err);
  printf("L2 norm of error |e|_2/|u|_2 %10.4e  |e|_max/|u|_max %10.4e\n",enorm2/unorm2,enormMax/unormMax);

  {
    cycles_t max = 0,min = 999999999999,sum = 0;
    for (int i=0; i<niterations; i++) {
      if (cyclelog[i] > max) max = cyclelog[i];
      if (cyclelog[i] < min) min = cyclelog[i];
      sum += cyclelog[i];
    }
    long ndof = g->m[0]*g->m[1]*g->m[2];
    printf("Cycles/DOF: best %10.4e  worst %10.4e  avg %10.4e\n",1.*min/ndof,1.*max/ndof,1.*sum/ndof/niterations);
  }

  free(cyclelog);
  free(u);
  free(uexact);
  free(b);
  free(g);
  return 0;
}
