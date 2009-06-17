#include<stdio.h>
#include<math.h>
#include <sys/time.h>
#define NN 5000
#define INC 1
#define TYPE double


void dgemm
   (const int M, const int N, const int K, const double alpha, const double *A, const int lda, const double *B, const int ldb, const double beta, double *C, const int ldc)
{
   const double *stM = A + 3600;
   const double *stN = B + 3600;
   #define incAk 4
   const int incAm = 180, incAn = -3600;
   #define incBk 4
   const int incBm = -60, incBn = 240;
   const int incAk0 = ((incAk) / 12), incBk0 = ((incBk) / 12);
   #define incCm 4
   const int incCn = (((ldc) << 2)) - 60;
   double *pC0=C, *pC1=pC0+(ldc), *pC2=pC1+(ldc), *pC3=pC2+(ldc);
   const double *pA0=A, *pA1=pA0+(lda), *pA2=pA1+(lda), *pA3=pA2+(lda);
   const double *pB0=B, *pB1=pB0+(ldb), *pB2=pB1+(ldb), *pB3=pB2+(ldb);
   register int Kloop=14;
   register double rA0, rA1, rA2, rA3, ra0, ra1, ra2, ra3;
   register double rB0, rB1, rB2, rB3, rb0, rb1, rb2, rb3;
   register double rC0_0, rC1_0, rC2_0, rC3_0, rC0_1, rC1_1, rC2_1, rC3_1, rC0_2, rC1_2, rC2_2, rC3_2, rC0_3, rC1_3, rC2_3, rC3_3;
    const double *pD4;
   do 
   {
   	  pD4=pB0+240;
      do 
      {

		asm volatile("mult.g $9,$9,$9\n	mult.g $9,$9,$9\n	mult.g $9,$9,$9\n mult.g $9,$0,$9\n"
			"bnel   $9,$0, preB1 \nnop":::"t1");
		asm volatile("j	entranceB1\nnop\n");
		asm volatile("preB1:");
		asm volatile("ld	   $9, 0(%0)"::"r"(pD4):"t1");
		asm volatile("ld	   $9, 32(%0)"::"r"(pD4):"t1");
		asm volatile("ld	   $9, 64(%0)"::"r"(pD4):"t1");
		asm volatile("ld	   $9, 96(%0)"::"r"(pD4):"t1");
		asm volatile("sync");
		asm volatile("entranceB1:\n");
		pD4 += 16;

		rA0=*pA0;
        	rB0=*pB0;
        	rA1=*pA1;
		rB1=*pB1;
        	rA2=*pA2; 		 
        	rB2=*pB2;
		rA3=*pA3;
        	rB3=*pB3;
        	
		ra0=pA0[1];		 rC0_0=rA0*rB0;		 rC1_0=rA1*rB0;			 
        	rb0=pB0[1];		 rC0_1=rA0*rB1;		 rC1_1=rA1*rB1;
  		ra1=pA1[1];		 rC2_0=rA2*rB0;		 rC2_1=rA2*rB1;
		rb1=pB1[1];		 rC0_2=rA0*rB2;		 rC1_2=rA1*rB2;
        	ra2=pA2[1];		 rC2_2=rA2*rB2;		 rC3_2=rA3*rB2;
		rb2=pB2[1];		 rC3_0=rA3*rB0;		 rC3_1=rA3*rB1;
        	ra3=pA3[1]; 		 rC0_3=rA0*rB3;		 rC1_3=rA1*rB3;			 
        	rb3=pB3[1];		 rC2_3=rA2*rB3;		 rC3_3=rA3*rB3;
		 
		 asm volatile("move $6, %0\nKloop52A:\n"::"r"(Kloop):"a2");       
		 asm volatile(".align 5");
			rA0=pA0[2];	 rC0_0+=ra0*rb0;	rC1_0+=ra1*rb0;		asm volatile("nop");
         		rB0=pB0[2];	 rC0_1+=ra0*rb1;	rC1_1+=ra1*rb1;		asm volatile("nop");
         		rA1=pA1[2];	 rC2_0+=ra2*rb0;	rC2_1+=ra2*rb1;		asm volatile("nop");
			rB1=pB1[2];	 rC0_2+=ra0*rb2;	rC1_2+=ra1*rb2;		asm volatile("nop");
         		rA2=pA2[2];	 rC2_2+=ra2*rb2;	rC3_2+=ra3*rb2;		asm volatile("nop");
         		rB2=pB2[2];	 rC3_0+=ra3*rb0;	rC3_1+=ra3*rb1;		asm volatile("nop");
			rA3=pA3[2];	 rC0_3+=ra0*rb3;	rC1_3+=ra1*rb3;		asm volatile("nop");
         		rB3=pB3[2];	 rC2_3+=ra2*rb3;	rC3_3+=ra3*rb3;		asm volatile("nop");
                	
			ra0=pA0[3];	 rC0_0+=rA0*rB0;	rC1_0+=rA1*rB0;		pA0+=incAk;
         		rb0=pB0[3];	 rC0_1+=rA0*rB1;	rC1_1+=rA1*rB1;		pB0+=incBk;
         		ra1=pA1[3];	 rC2_0+=rA2*rB0;	rC2_1+=rA2*rB1;		pA1+=incAk;
			rb1=pB1[3];	 rC0_2+=rA0*rB2;	rC1_2+=rA1*rB2;		pB1+=incBk;
         		ra2=pA2[3];	 rC2_2+=rA2*rB2;	rC3_2+=rA3*rB2;		pA2+=incAk;
			rb2=pB2[3];	 rC3_0+=rA3*rB0;	rC3_1+=rA3*rB1;		pB2+=incBk;
         		ra3=pA3[3]; 	 rC0_3+=rA0*rB3;	rC1_3+=rA1*rB3;		pA3+=incAk;
         		rb3=pB3[3];	 rC2_3+=rA2*rB3;	rC3_3+=rA3*rB3;		pB3+=incBk;
                	
			rA0=*pA0;	rC0_0+=ra0*rb0;		rC1_0+=ra1*rb0;		asm volatile("nop");         
			rB0=*pB0;	rC0_1+=ra0*rb1;		rC1_1+=ra1*rb1;		asm volatile("nop");
         		rA1=*pA1;	rC2_0+=ra2*rb0;		rC2_1+=ra2*rb1;		asm volatile("nop");
			rB1=*pB1;	rC0_2+=ra0*rb2;		rC1_2+=ra1*rb2;		asm volatile("nop");
         		rA2=*pA2;	rC2_2+=ra2*rb2;		rC3_2+=ra3*rb2;		asm volatile("nop");
         		rB2=*pB2;	rC3_0+=ra3*rb0;		rC3_1+=ra3*rb1;		asm volatile("nop");
			rA3=*pA3;	rC0_3+=ra0*rb3;		rC1_3+=ra1*rb3;		asm volatile("nop");
         		rB3=*pB3;	rC2_3+=ra2*rb3;		rC3_3+=ra3*rb3; 	asm volatile("nop");
			
			ra0=pA0[1];	 rC0_0+=rA0*rB0;	rC1_0+=rA1*rB0;		asm volatile("addiu $6,$6,-1\n");
         		rb0=pB0[1];	 rC0_1+=rA0*rB1;	rC1_1+=rA1*rB1;		asm volatile("nop");
         		ra1=pA1[1];	 rC2_0+=rA2*rB0;	rC2_1+=rA2*rB1;		asm volatile("nop");
			rb1=pB1[1];	 rC0_2+=rA0*rB2;	rC1_2+=rA1*rB2;		asm volatile("nop");
         		ra2=pA2[1];	 rC2_2+=rA2*rB2;	rC3_2+=rA3*rB2;		asm volatile("nop");
			rb2=pB2[1];	 rC3_0+=rA3*rB0;	rC3_1+=rA3*rB1;		asm volatile("nop");
         		ra3=pA3[1]; 	 rC0_3+=rA0*rB3;	rC1_3+=rA1*rB3;		asm volatile("nop");
         		rb3=pB3[1];	 rC2_3+=rA2*rB3;	rC3_3+=rA3*rB3;
		 asm volatile("bne	$6,$0,Kloop52A\nnop\n");

		rA0=pA0[2];		 rC0_0+=ra0*rb0;		 rC1_0+=ra1*rb0;
        	rB0=pB0[2];		 rC0_1+=ra0*rb1;		 rC1_1+=ra1*rb1;
        	rA1=pA1[2];		 rC2_0+=ra2*rb0;		 rC2_1+=ra2*rb1;
		rB1=pB1[2];		 rC0_2+=ra0*rb2;		 rC1_2+=ra1*rb2;
        	rA2=pA2[2]; 		 rC2_2+=ra2*rb2;		 rC3_2+=ra3*rb2;
        	rB2=pB2[2];		 rC3_0+=ra3*rb0;		 rC3_1+=ra3*rb1;
		rA3=pA3[2];		 rC0_3+=ra0*rb3;		 rC1_3+=ra1*rb3;
        	rB3=pB3[2];		 rC2_3+=ra2*rb3;		 rC3_3+=ra3*rb3;  		 
		
		ra0=pA0[3];		 rC0_0+=rA0*rB0;		 rC1_0+=rA1*rB0;		 pA0+=incAk;         
		rb0=pB0[3];		 rC0_1+=rA0*rB1;		 rC1_1+=rA1*rB1;		 pB0+=incBk;
        	ra1=pA1[3];		 rC2_0+=rA2*rB0;		 rC2_1+=rA2*rB1;		 pA1+=incAk;
		rb1=pB1[3];		 rC0_2+=rA0*rB2;		 rC1_2+=rA1*rB2;		 pB1+=incBk;
        	ra2=pA2[3];		 rC2_2+=rA2*rB2;		 rC3_2+=rA3*rB2;		 pA2+=incAk;
		rb2=pB2[3];		 rC3_0+=rA3*rB0;		 rC3_1+=rA3*rB1;		 pB2+=incBk;
        	ra3=pA3[3]; 		 rC0_3+=rA0*rB3;		 rC1_3+=rA1*rB3;		 pA3+=incAk;
        	rb3=pB3[3];		 rC2_3+=rA2*rB3;		 rC3_3+=rA3*rB3;		 pB3+=incBk;
        	
		rC0_0+=ra0*rb0;		 rC1_0+=ra1*rb0;
		rC0_1+=ra0*rb1;		 rC1_1+=ra1*rb1;
		rC2_0+=ra2*rb0;		 rC2_1+=ra2*rb1;
		rC0_2+=ra0*rb2;		 rC1_2+=ra1*rb2;
		rC2_2+=ra2*rb2;		 rC3_2+=ra3*rb2;
		rC3_0+=ra3*rb0;		 rC3_1+=ra3*rb1;
		rC0_3+=ra0*rb3;		 rC1_3+=ra1*rb3;
		rC2_3+=ra2*rb3;		 rC3_3+=ra3*rb3; 
        	
		rA0 = beta * *pC0; 	
		rA1 = beta * pC0[1];
		rA2 = beta * pC0[2];
		rA3 = beta * pC0[3];
		rB0 = beta * *pC1;
		rB1 = beta * pC1[1];
		rB2 = beta * pC1[2];
		rB3 = beta * pC1[3];
        	             
		ra0 = beta * *pC2;		rC0_0+=rA0;		 
		ra1 = beta * pC2[1];		rC1_0+=rA1;
		ra2 = beta * pC2[2];		rC0_1+=rB0;		 
		ra3 = beta * pC2[3];		rC1_1+=rB1;
		rb0 = beta * *pC3;		rC2_0+=rA2;
		rb1 = beta * pC3[1];		rC3_0+=rA3;		 
		rb2 = beta * pC3[2];		rC3_1+=rB3;
		rb3 = beta * pC3[3];		rC2_1+=rB2;
        	
		rC0_2+=ra0;		 
		rC1_2+=ra1;
		rC2_2+=ra2;		 
		rC3_2+=ra3;
		rC0_3+=rb0;		 
		rC1_3+=rb1;
		rC2_3+=rb2;		 
		rC3_3+=rb3; 

        	*pC0=rC0_0; pC0[1]=rC1_0; pC0[2]=rC2_0; pC0[3]=rC3_0; 
        	*pC1=rC0_1; pC1[1]=rC1_1; pC1[2]=rC2_1; pC1[3]=rC3_1; 
        	*pC2=rC0_2; pC2[1]=rC1_2; pC2[2]=rC2_2; pC2[3]=rC3_2; 
	      	*pC3=rC0_3; pC3[1]=rC1_3; pC3[2]=rC2_3; pC3[3]=rC3_3; 

	      	pC0 += incCm;          pC1 += incCm;          pC2 += incCm;	pC3 += incCm;
	      	pA0 += incAm;          pA1 += incAm;          pA2 += incAm;	pA3 += incAm;
        	pB0 += incBm;          pB1 += incBm;          pB2 += incBm;	pB3 += incBm;

      }
      while(pA0 != stM);
      pC0 += incCn;      pC1 += incCn;      pC2 += incCn;		pC3 += incCn;
      pA0 += incAn;      pA1 += incAn;		pA2 += incAn;		pA3 += incAn;
      pB0 += incBn;		 pB1 += incBn;		pB2 += incBn;		pB3 += incBn;

   }
   while(pB0 != stN);
}
#undef incAk
#undef incBk
#undef incCm
#undef incAm
#undef incBm
#undef incAn
#undef incBn




int main()
{
struct timeval tpstart,tpend;
double runtime;  
int i,len,l,times;
int lda,ldb,ldc,M,N,K;
TYPE alpha,beta;
alpha = 1.2;
beta = 3.5;
TYPE* A = malloc(sizeof(TYPE) * NN * NN);
TYPE* B = malloc(sizeof(TYPE) * NN * NN);
TYPE* C = malloc(sizeof(TYPE) * NN * NN);
srand(2);
for(i=0; i<NN*NN; i++)
{
	A[i] = rand()/100000000.0;
	B[i] = rand()/100000000.0;
	C[i] = rand()/100000000.0;
}
                                     
for(len=60; len<=100; len++){ 
	times=50000/len*100/len*100;
	if(times<1) times=1; 
	M=len;N=len;K=len;
	lda=len;ldb=len;ldc=len;
	  
 	gettimeofday(&tpstart,NULL);                                                     
	for(i=0; i<times; i++){                                                    
  		dgemm(M,N, K, alpha, A, lda,B, ldb, beta, C, ldc);
	}
	gettimeofday(&tpend,NULL); 
	runtime=(tpend.tv_sec-tpstart.tv_sec)*1000000+(tpend.tv_usec-tpstart.tv_usec);      
	printf("total time = %.5f\ttimes=%d\ttime per call = %.3fus\tspeed=%.2fMflops\n", runtime/1e6, times, runtime/times, (double)2*60*times/runtime*60*K);         
	                                                                                                                                                                                                                                                                                                                                                
     } 
free(A);
free(B);
free(C);                                                                                                                                                      
return 1;  
}



