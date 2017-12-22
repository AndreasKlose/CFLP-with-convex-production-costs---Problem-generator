/* File QuadCoeff.cpp
 *------------------------------------------------------------------
 * Reads instance from Klose & Goertz (2007) and Goertz & Klose (2012) 
 * of the linear CFLP and calculates some "reasonable" coefficient for 
 * the case of a quadratic or inverse throughput/production cost 
 * function to be applied for the convex production cost case.
 *
 * @version 03/08/16, 13/07/17
 * @author  Andreas Klose 
 *
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>

#include "CFLrdlp.h"      // reads problem data

using namespace std;

typedef struct TCFLdat // structure for easier accessing all required data
{
  int      m, n;// number of customers and potential facility sites
  int      ndat;// number of parameters in production cost functions
  int      totd;// total demand
  int*     pcap;// pointer to array of depot capacities
  int*     pdem;// pointer to array of customer demands
  double*  pfc; // pointer to array of depot fixed costs
  double** ptc; // pointer to matrix of supply/transport cost
  double** ppc; // pointer to 2-dim matrix of prod.cost function parameters
} TCFLdat;

// Some global variables for easier access in the procedures below
TCFLdat  data;        // gathers all problem data
TCFLdat* pdata=&data; // pointer to the problem data

//-----------------------------------------------------------------------------

void CompCoeff ( bool quadratic )
/* Let the production cost function h(u) at any facility respectively be
   h(u) = \alpha u^2 and h(u)=\alpha u/(s-u) in quadratic and inverse
   cost case, where u is the demand allocated to the facility and s its
   capacity. A base value for the parameter \alpha is then fixed such that 
   at a throughput value of u=D/p the production cost h(u) equal 20%
   of the average fixed cost, where D is the total demand and p a guessed
   number of open facilities obtained as p= 1.2 D/s, where s is the average
   capacity (that means about 20% more open facilities as on average required
   to cover total demand). Each single facility's cost function parameter 
   \alpha_i is then obtained as
   
                   \alpha_i = \alpha \gamma fmax/(f_i +  fmax ),
                   
   where \gamma is a uniformly distributed random number in [1.8,2.2],
   f_i is facility i's fixed cost and fmax the largest fixed cost value.
   This way, \alpha_i equals on average the base value for facilities of 
   largest fixed cost, while facilities of very small fixed cost show
   approximately a two times larger parameter value \alpha_i.
 */                       
{  
  // Get ratio of total capacity to total demand
  int totc = 0;
  int totd = 0;
  double fbar = 0.0;
  double fmax = 0.0;
  double cbar = 0.0;
  for ( int i=0; i < pdata->n; i++ )
  { 
    totc += pdata->pcap[i];
    fbar += pdata->pfc[i];
    cbar += pdata->pcap[i];
    if ( pdata->pfc[i] > fmax ) fmax = pdata->pfc[i];
  }  
  for ( int j=0; j < pdata->m; j++ ) totd += pdata->pdem[j];
  fbar /= (double)pdata->n;
  cbar /= (double)pdata->n;
  double r = (double)totc/(double)totd;
  
  // Fix desired/expected number p of open depots
  int p = (int)((double)pdata->n/r*1.2);   
  
  // Share demand evenly between the open depots
  double V = (double)totd / (double)p;
  
  // Set cost of throughbut V equal to 20% of average fixed cost
  double alpha = 0.0;
  if ( quadratic ) 
    alpha = 0.2*fbar/V/V;
  else 
  {
    alpha = 0.2*fbar*(cbar - V)/V;
    if ( alpha < 1.0E-07 )
    {
    	cout << "Error insignificant or negative inverse cost parameter" << endl;
    	exit(999);
    }
  }  
  
  // Now fix individual cost coefficient \alpha(i) by letting \alpha(i) be 
  // larger than alpha if fixed cost are low and lower than that if fixed 
  // cost are large.
  int nn = 1 + (int) (quadratic);
  pdata->ndat = nn;
  for ( int i=0; i < pdata->n; i++ )
  {
    // random seed not used, so seed = 1 => each execution gives same data
    pdata->ppc[i][0] = 0.0; // no linear cost
    double rnd = 1.8 + ( (double)rand() / RAND_MAX )*0.4;
    pdata->ppc[i][nn-1] = alpha*rnd/(1.0+pdata->pfc[i]/fmax);
  }
  
}

//-----------------------------------------------------------------------------

int main( int argc, char* argv[] )
{

  if ( argc < 2 )
  {
    cout << "Usage: " << argv[0] << " Inputfile" << endl;
    return(777);
  }
  char* DatName = argv[1];
  if ( ReadDLocProb( DatName, NULL, &(pdata->n), &(pdata->m), NULL, NULL, NULL, 
                     &(pdata->pcap),&(pdata->pfc), &(pdata->pdem), NULL, NULL, 
                     NULL, NULL, &(pdata->ptc), &(pdata->ndat), 
                     &(pdata->ppc) ) > 0 )                             
  {
    cout << "Error when reading data" << endl;
    exit(211);
  }
  
  // If program invoked without any additional argument, then
  // quadratic cost coefficients are computed.
  CompCoeff ( argc < 3 );
  
  // Output computed coefficients to terminal/screen
  cout << endl << "[NDAT] " << pdata->ndat << endl;
  cout << "NOOFDATS "<< pdata->ndat << endl;
  if ( pdata->ndat > 1 ) // quadratic cost
    for ( int i=0; i < pdata->n; i++ )
      cout << "0.0 " << setprecision(5) << pdata->ppc[i][pdata->ndat-1] << endl;
  else 
    for ( int i=0; i < pdata->n; i++ )
      cout << setprecision(6) << pdata->ppc[i][0] << endl;      
    
  return 0;

}

