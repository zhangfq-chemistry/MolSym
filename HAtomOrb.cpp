#include "HAtomOrb.h"

#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkDelaunay2D.h>
#include <vtkVoronoi2D.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkProbeFilter.h>
#include <algorithm>

#include "mainwindow.h"
#include "SolveEquation.h"

#include <vector>
#include  "matrix3/vector3.h"

using namespace std;


double ElementMasses[27] = {0, 1.00794, 4.002602, 6.941,
   9.012182, 10.811, 12.0107, 14.0067, 15.9994, 18.9984032, 20.1797,
   22.98976928, 24.3050, 26.9815386, 28.0855, 30.973762, 32.065, 35.453, 39.948,
   39.0983, 40.078, 44.955912, 47.867, 50.9415, 51.9961, 54.938045, 55.845
};





double Factorial(int n) {
    if (n==0) return 1.0;
    if(n == 1)     return 1.0;
    else    return n * Factorial(n - 1);
}

// Associated Laguerre polynomial with unity norm
double AssociatedLaguerre (int n, int lambda, double x) {
    int i;
    double l=0, l1, l2;
    double c, c2;

    // starting values
    l2 = 1.0/sqrt(Factorial(lambda));
    if (n == 0) return l2;
    c2 = sqrt(lambda+1);
    l1 = -(x-lambda-1)*l2/c2;
    if (n == 1)
        return l1;
    // recurrence
    for (i=2; i<=n; ++i) {
        c = sqrt(i*(lambda+i));
        l = -l1*(x-lambda-2*i+1)/c - l2*c2/c;
        l2 = l1;
        l1 = l;
        c2 = c;
    }
    return l;
}


// Associated Legendre polynomial with unity norm
double AssociatedLegendre (int l, int m, float x)
{
    double tmp;
    double p=0, p1, p2;
    double c, c2;
    int i;

    // compute starting values for recurrence
    tmp = 1.0;
    for (i=2; i<=2*m; i+=2)
        tmp *= (1-x*x)*(i+1.0)/i;

    p2 = sqrt(tmp/2);
    if (l == m)
        return p2;

    p1 = sqrt(2*m+3)*x*p2;
    if (l == m+1)
        return p1;

    // recurrence
    c2 = sqrt((4.0*m*m+8.0*m+3)/(2*m+1.0));
    for (i=m+2; i<=l; ++i) {
        c = sqrt((4.0*i*i-1.0)/(1.0*i*i-1.0*m*m));
        p = p1*x*c - p2*c/c2;
        p2 = p1;
        p1 = p;
        c2 = c;
    }
    return p;
}





int rnd(int a, int b) {
    return rand() % (b-a+1)+ a;
}


double sgnd(double x)
{
    if (x>0.0) return(1.0);
    if (x<0.0) return(-1.0);
    return(1.0);
}

double powl(double val, int p)
{
   if (p ==0) return 1.0;
   if (p < 0) return pow(val,p);

  double ret;
  if (!p)  return(val!=0);
  ret = 1.0;
  while (p) {
    if (p&1)  ret *= val;
    val *= val;  p >>= 1;
  }

  return ret;
}








// Wavefunction of the electron in a hydrogen atom
double HAO::Hydrogen_Psi (double r, double theta, double phi)
{

    if (r<0.001)
        r=0.001;

    double cos_theta=cos(theta*3.141592654/180.0);

    double alpha = 2.0*Z/n/a0;
    double p=alpha*r;

    // radial part
    double R=pow(p,l)*exp(-p/2.0)*AssociatedLaguerre(n-l-1,2*l+1,p);
    double factor=sqrt(alpha*alpha*alpha/2.0/n);
    R=R*factor;


    // angular parts
    double Theta = AssociatedLegendre(l,abs(m),cos_theta);

    double Phi=1.0/sqrt(2.0*3.141592654); //m=0

    phi=phi*3.141592654/180.0;
    double sqrtPi=sqrt(3.141592654);
    if (m > 0)    Phi=sin(m*phi)/sqrtPi; //m<0
    if (m < 0)    Phi=cos(m*phi)/sqrtPi; //m>0

    double Y=Theta*Phi;

    return R*Y*scale;
}


double HAO::Hydrogen_Psi (vector3 pos1)
{
    double R, Theta, Phi;

    vector3 pos0 = pos1 - pos;

    double  x=pos0[0],
            y=pos0[1],
            z=pos0[2];

    double x2=x*x,y2=y*y,z2=z*z;
    double r = pos0.length();

    if (r<0.001)
        r=0.001;

    if (abs(x)<0.001)
        x=sgnd(x)*0.001;

    double phi = atan2(y, x);
    double theta = acos(z/r);
    double cos_theta=z/r;

    double alpha = 2.0*Z/n/a0;
    double p=alpha*r;

    // radial part
    R=pow(p,l)*exp(-p/2.0)*AssociatedLaguerre(n-l-1,2*l+1,p);
    double factor=sqrt(alpha*alpha*alpha/2.0/n);
    R=R*factor;


    // angular parts
    Theta = AssociatedLegendre(l,abs(m),cos_theta);

    Phi=1.0/sqrt(2.0*3.141592654); //m=0
    if (m < 0)    Phi=sin(m*phi)/sqrt(3.141592654); //m<0
    if (m > 0)    Phi=cos(m*phi)/sqrt(3.141592654); //m>0

    double Y=Theta*Phi;

    return R*Y*scale;
}



double HAO::Slater_Psi (vector3 pos1)
{
    double R, Theta, Phi;

    vector3 pos0 = pos1 - pos;

    double  x=pos0[0],
            y=pos0[1],
            z=pos0[2];

    double x2=x*x,y2=y*y,z2=z*z;
    double r = pos0.length();

    if (r<0.001)
        r=0.001;

    if (abs(x)<0.001)
        x=sgnd(x)*0.001;

    double phi = atan2(y, x);
    double theta = acos(z/r);
    double cos_theta=z/r;

    double alpha = 2.0*Z/n/a0;
    double p=alpha*r;

    // radial part
    double ksi=1.0*Z;
    R=pow(r,n-1)*exp(-ksi*r);
    double factor=powl(2*ksi,n)*sqrt(2*ksi/Factorial(2*n));
    R=R*factor;


    // angular parts
    Theta = AssociatedLegendre(l,abs(m),cos_theta);

    Phi=1.0/sqrt(2.0*3.141592654); //m=0
    if (m < 0)    Phi=sin(m*phi)/sqrt(3.141592654); //m<0
    if (m > 0)    Phi=cos(m*phi)/sqrt(3.141592654); //m>0

    double Y=Theta*Phi;


    return R*Y*scale;
}



double HAO::Hydrogen_RDF (double r)
{
    double  R, RDF;

    if (r<0.000001)
        r=0.0000001;

    double alpha=2.0*Z/n/a0;
    double p=alpha*r;

    R = pow(p,l)*exp(-p/2.0)*AssociatedLaguerre(n-l-1,2*l+1,p);
    double factor=sqrt(alpha*alpha*alpha/2.0/n);
    R=R*factor;

    RDF=r*r*R*R;

    return RDF;
}

double HAO::Hydrogen_R(double x, double y, double z)
{
    double x2=x*x,y2=y*y,z2=z*z;
    double r = sqrt(x2+y2+z2);

    if (r<0.001)        r=0.001;

    double alpha=2.0*Z/n/a0;
    double p=alpha*r;

    // radial part
    double R=pow(p,l)*exp(-p/2.0)*AssociatedLaguerre(n-l-1,2*l+1,p);

    double factor=sqrt(alpha*alpha*alpha/2.0/n);
    R=R*factor;

    return R;
}



double HAO::Hydrogen_R (double r)
{
    if (r<0.001)
        r=0.001;

    double alpha=2.0*Z/n/a0;
    double p=alpha*r;

    // radial part
    double R=pow(p,l)*exp(-p/2.0)*AssociatedLaguerre(n-l-1,2*l+1,p);

    double factor=sqrt(alpha*alpha*alpha/2.0/n);
    R=R*factor;

    return R;
}



double HAO::Hydrogen_Y (vector3 pos1)
{
    vector3 pos0 = pos1 - pos;

    double  x=pos0[0],
            y=pos0[1],
            z=pos0[2];

    double x2=x*x,y2=y*y,z2=z*z;
    double r = sqrt(x*x+y*y+z*z);

    if (r<0.000001)
        r=0.0000001;

    if (abs(x)<0.000001)
        x=sgnd(x)*0.000001;

    double phi = atan2(y, x);
    double theta = acos(z/r);
    double cos_theta=z/r;

    double sin_mphi=sin(m*phi);
    double cos_mphi=cos(m*phi);

    double  Theta, Phi ,Y;
    Theta = AssociatedLegendre(l,abs(m),cos_theta);
    Phi=1.0/sqrt(2.0*3.141592654); //m=0
    if (m > 0)    Phi=sin_mphi/sqrt(3.141592654); //m>0
    if (m < 0)    Phi=cos_mphi/sqrt(3.141592654); //m<0

    Y=Theta*Phi;
    return Y;
}





void HAO::nlm2Name()
{
    QString name;

    if (l==0 )
        name="S";

    else if (l==1) //P
    {
           name="Pz";           //0
           if (m>0)  name="Px"; //1
           if (m<0)  name="Py"; //-1
    }

    else if (l==2){ //D

           name="Dz2"; //0

           if  (m==1)        name="Dxz";    //1
           else  if (m==-1)  name="Dyz";    //-1

           else  if (m==2)   name="Dxy";     //2
           else  if (m==-2)  name="Dx2-y2"; //-2
    }

    else if (l==3){ //F

           name="Fz3"; //0

           if (m==1)        name="Fxz2"; //1
           else  if (m==-1)  name="Fyz2"; //-1

           else  if (m==2)   name="Fz(x2-y2)"; //2
           else  if (m==-2)  name="Fzxy"; //-2

           else  if (m==3)   name="Fx(x2-3y2)"; //3
           else  if (m==-3)  name="Fy(3x2-y2)"; //-3
    }

    else if (l==4){ //G

        name="Gz4"; //0

        if (m==1)   name="Gz3x"; //1
        else  if (m==-1)  name="Gz3y"; //-1

        else  if (m==2)   name="Gz2xy"; //2
        else  if (m==-2)  name="Gz2(x2-y2)"; //-2

        else  if (m==3)   name="Gzx3"; //3
        else  if (m==-3)  name="Gzy3"; //-3

        else  if (m==4)   name="Gxy(x2-y2)"; //3
        else  if (m==-4)  name="G(x4+y4)"; //-3
    }

    ObitalName.clear();
    ObitalName.setNum(n);
    ObitalName=ObitalName+name;
}




void HAO::Name2nlm(QString AOName)
{
    ObitalName=AOName;

    // 1S,2S,3S,4S,5S,6S,7S
    // 2P,3P,4P,5P,6P,7P
    // 3D,4D, 5D,6D,7D
    // 4F,5F,6F,7F
    // 5G, 6G, 7G

    QChar first=AOName[0],
          second = AOName[1];

    if (first.isDigit()) {
        n=AOName[0].digitValue();
    }


    second.toUpper();
    if (second=="S" )         l=0;
    else if (second=="P")     l=1;
    else if (second=="D")     l=2;
    else if (second=="F")     l=3;
    else if (second=="G")     l=4;
    else if (second=="H")     l=5;
    else l=0;

    QString name=AOName;
    name.remove(0,1);
    if (name.startsWith("Pz"))          m=0;
    else if (name.startsWith("Px"))     m=1;
    else if (name.startsWith("Py"))     m=-1;


    else if (name.startsWith("Dz2"))     m=0;
    else if (name.startsWith("Dxz"))     m=1;
    else if (name.startsWith("Dyz"))     m=-1;
    else if (name.startsWith("Dxy") )    m=2;
    else if (name.startsWith("Dx2-y2"))  m=-2;

    else if (name.startsWith("Fz3"))        m=0;
    else if (name.startsWith("Fxz2"))       m=1;
    else if (name.startsWith("Fyz2"))       m=-1;
    else if (name.startsWith("Fz(x2-y2)"))  m=2;
    else if (name.startsWith("Fzxy"))       m=-2;
    else if (name.startsWith("Fx(x2-3y2)")) m=3;
    else if (name.startsWith("Fy(3x2-y2"))  m=-3;

    //waiting for check
    else if (name.startsWith("Gz4"))        m=0;
    else if (name.startsWith("Gz3x"))       m=1;
    else if (name.startsWith("Gz3y"))       m=-1;
    else if (name.startsWith("Gz2xy"))      m=2;
    else if (name.startsWith("Gz2(x2-y2)")) m=-2;
    else if (name.startsWith("Gzx3"))       m=3;
    else if (name.startsWith("Gzy3"))       m=-3;
    else if (name.startsWith("Gxy(x2-y2)")) m=4;
    else if (name.startsWith("G(x4+y4)"))   m=-4;
    else m=0;
}




double HAO::HydrogenPsi_byName(vector3 pos, QString AOName)
{
    double R = HydrogenR_byName(pos,AOName),
           Y = HydrogenY_byName(pos,AOName);

    return R*Y;
}

double  HAO::HydrogenPsi_byName(vector3 pos)
{
    if (ObitalName=="none")
        nlm2Name();

    return HydrogenPsi_byName(pos,ObitalName);
}



double HAO::HydrogenR_byName(vector3 pos1, QString AOName)
{
    vector3 pos0 = pos1 - pos;

    double  x=pos0.x(),
            y=pos0.y(),
            z=pos0.z();

    double r=sqrt(x*x+y*y+z*z);

    if (r<0.001) r=0.001;
    if (abs(x)<0.001)  x=sgnd(x)*0.001;

    double p=1.0,p2,p3,p4,p5, p6;

    double  Z3=Z*Z*Z;
    double  Zeff=Z/a0;

    double  x2=x*x;
    double  y2=y*y;
    double  z2=z*z;

    double  x4=x2*x2,
            y4=y2*y2,
            z4=z2*z2;

    double  r2=r*r;
    double  r3=r2*r;
    double  r4=r3*r;

    double  Zeff32 = pow(Zeff,1.5);

    double NS=1/sqrt(4*PI);
    double Ys = NS;

    /*
     // NR=sqrt (2^3*(n-l-1)! / (2*n * (n+l)!* (n+l)! * (n+l)! )
     R1s = 2 * Zeff32 * exp(-p/2);
     R2s = 1/(2*sqrt(2) ) * ( 2 - p ) * Zeff32 * exp(-p/2);
     R3s = 1/(9*sqrt(3) ) * ( 6 - 6*p + p2 ) * Zeff32 * exp(-p/2);
     R4s = 1/96 * ( 24 - 36*p + 12*p2 - p3 ) * Zeff32 * exp(-p/2);
     R5s = 1/(300*sqrt(5) )  * ( 120 - 240*p + 120*p2 - 20*p3 + p4 ) * Zeff32 * exp(-p/2);
     R6s = 1/(2160*sqrt(6) ) * ( 720 - 1800*p + 1200*p2 - 300*p3 + 30*p4 - p5 ) * Zeff32 * exp(-p/2);
     //R7s = 1                *(5040 - 15120*ρ + 12600*ρ2 - 4200*ρ3 + 630*ρ4 - 42*ρ5 + ρ6)  * Zeff32 * exp(-p/2);
 */
    double R=1.0;
    double alpha=1.0;



    if (AOName.startsWith("1S") )
    {
        n=1; l=0;
        p = 2.0 * Z * r/a0;
        R = 2.0 * Zeff32 * exp(-p/2.0);

    }
    else if (AOName.startsWith("2S"))
    {
        n=2; l=0;

        alpha=2.0 * Z/(n*a0);
        p = alpha * r;

        R = 1.0/(2.0*sqrt(2.0) ) * ( 2 - p) * Zeff32 * exp(-p/2);
    }


    else if (AOName.startsWith("3S"))
    {
        n=3; l=0;

        alpha=2.0 * Z/(n*a0);
        p = alpha * r;
        p2=p*p;

        //R = 1/(4.0*sqrt(6) ) * ( 6 - 2*p + p2/9 ) * Zeff32 * exp(-p/2);
        R = 1/(9.0*sqrt(3.0) ) * ( 6 - 6*p + p2 ) * Zeff32 * exp(-p/2);
    }

    else if (AOName.startsWith("4S"))
    {
        n=4; l=0;
        alpha=2.0 * Z/(n*a0);
        p = alpha * r;
        p2=p*p;p3=p2*p;
        R = 1/96. * ( 24. - 36.0*p + 12*p2 - p3 ) * Zeff32 * exp(-p/2);
    }

    else if (AOName.startsWith("5S"))
    {
        n=5;l=0;
        p = 2.0 * Z * r / (n*a0);p2=p*p;p3=p2*p;p4=p2*p2;
        R = 1/(300.*sqrt(5.0) )  * ( 120. - 240.*p + 120.*p2 - 20.*p3 + p4 ) * Zeff32 * exp(-p/2.0);
    }

    else if (AOName.startsWith("6S"))
    {

        n=6;l=0;
        p = 2.0 * Z * r / (n*a0);
        p2=p*p;
        p3=p2*p;
        p4=p2*p2;
        p5=p2*p3;
        R = 1/(2160.*sqrt(6.0) ) * ( 720. - 1800.*p + 1200.*p2 - 300.*p3 + 30.*p4 - p5 ) * Zeff32 * exp(-p/2);
    }

    /*
    R2p  = 1/(2*sqrt(6)) * p * Zeff32 * exp(-p/2);
    R3p  = 1/(9*sqrt(6)) * p * ( 4 - p ) * Zeff32 * exp(-p/2);
    R4p  = 1/(32*sqrt(15)) * p* ( 20 - 10*p + p2 ) * Zeff32 * exp(-p/2);
    R5p  = 1/(150/sqrt(30)) * p *( 120 - 90*p + 18*p2 - p3 ) * Zeff32 * exp(-p/2);
    R6p  = 1/(432/sqrt(210)) * p * ( 840 - 840*p + 252*p2 - 28*p3 + p4 ) *  Zeff32 * exp(-p/2);
*/

    else if (AOName.startsWith("2P"))
    {
        n=2; l=1;
        p = 2.0 * Z * r / (n*a0);
        R  = 1/(2.*sqrt(6.0)) * p * Zeff32 * exp(-p/2);
    }

    else if  (AOName.startsWith("3P"))
    {
        n=3;l=1;
        p = 2.0 * Z * r / (n*a0);
        R  = 1/(9.*sqrt(6)) * p * ( 4. - p ) * Zeff32 * exp(-p/2.);
    }

    else if  (AOName.startsWith("4P"))
    {
        n=4;l=1;
        p = 2.0 * Z * r / (n*a0);p2=p*p;
        R  = 1./(32.*sqrt(15.)) * p* ( 20. - 10.*p + p2 ) * Zeff32 * exp(-p/2.);
    }

    else if  (AOName.startsWith("5P"))
    {
        n=5;l=1;
        p = 2.0 * Z * r / (n*a0);p2=p*p;p3=p2*p;
        R  = 1./(150.*sqrt(30.)) * p *( 120. - 90.*p + 18.*p2 - p3 ) * Zeff32 * exp(-p/2.);
    }

    else if  (AOName.startsWith("6P"))
    {
        n=6;l=1;
        p = 2.0 * Z * r / (n*a0);p2=p*p;p3=p2*p;p4=p2*p2;
        R  = 1./(432.*sqrt(210.)) * p * ( 840. - 840.*p + 252.*p2 - 28.*p3 + p4 ) *  Zeff32 * exp(-p/2.);

    }

    /*
    R3d = 1/(9*sqrt(30)) * p2 * Zeff32 * exp(-p/2);
    R4d = 1/(96*sqrt(5)) * p2 *( 6 - p ) * Zeff32 * exp(-p/2);
    R5d = 1/(150*sqrt(70) ) * p2 *( 42 - 14*p + p2 ) * Zeff32 * exp(-p/2);
    R6d = 1/(864*sqrt(105) ) * p2 *( 336 - 168*p + 24*p2 - p3 ) * Zeff32 * exp(-p/2);
    */

    else if  (AOName.startsWith("3D"))
    {
        n=3; l=2;
        p = 2.0 * Z * r / (n*a0);p2=p*p;
        R = 1./(9.0*sqrt(30)) * p2 * Zeff32 * exp(-p/2);
    }

    else if (AOName.startsWith("4D"))
    {
        n=4;  l=2;
        p = 2.0 * Z * r / (n*a0);p2=p*p;
        R = 1.0/(96.*sqrt(5)) * p2 *( 6. - p ) * Zeff32 * exp(-p/2);
    }

    else if (AOName.startsWith("5D"))
    {
        n=5; l=2;
        p = 2.0 * Z * r / (n*a0);
        p2=p*p;

        R = 1.0/(150.*sqrt(70.) ) * p2 *( 42.0 - 14.0*p + p2 ) * Zeff32 * exp(-p/2.0);

    }

    else if (AOName.startsWith("6D"))
    {
        n=5; l=2;
        p = 2.0 * Z * r / (n*a0);p2=p*p;p3=p2*p;
        R = 1.0/(864.*sqrt(105.) ) * p2 *( 336. - 168.*p + 24.*p2 - p3 ) * Zeff32 * exp(-p/2.0);
    }


    else if (AOName.startsWith("4F"))
    {
        n=4; l=3;
        p = 2.0 * Z * r / (n*a0);p2=p*p;p3=p2*p;
        R = 1/(96.*sqrt(35.)) * p3 * Zeff32 * exp(-p/2.0);

    }

    else if  (AOName.startsWith("5F"))
    {
        n=5; l=3;
        p = 2.0 * Z * r / (n*a0);p3=p*p*p;
        R = 1.0/(300.*sqrt(70.)) * p3 * ( 8.0 - p ) * Zeff32* exp(-p/2.0);
    }

    else if  (AOName.startsWith("6F")) {
        n=6;l=3;
        p = 2.0 * Z * r / (n*a0);p2=p*p;p3=p2*p;

        R = 1./(2592.*sqrt(35.)) * p3 * ( 72.0 - 18.0*p + p2 ) * Zeff32* exp(-p/2.0);
    }

    else if  (AOName.startsWith("5G"))
    {
        n=5; l=4;
        p = 2.0 * Z * r / (n*a0);
        p2=p*p;p4=p2*p2;

        R =  1.0/(900.*sqrt(70.0)) * p4 * Zeff32*exp(-p/2.0);
    }

    else if  (AOName.startsWith("6G"))
    {
        n=6; l=4;
        p = 2.0 * Z * r / (n*a0);
        p2=p*p;
        p4=p2*p2;

        R =  1.0 /(12960.8*sqrt(7.0)) * p4 * (10.0-p) * Zeff32* exp(-p/2.0);
    }


    else if  (AOName.startsWith("6H"))
    {
        n=6; l=5;
        p = 2.0 * Z * r / (n*a0);
        p2=p*p;
        p5=p2*p2*p;

        R =  1.0 /(12960.*sqrt(77.)) * p5 * Zeff32* exp(-p/2.0);
    }

    return R;
}



double  HAO::HydrogenY_byName(vector3 pos1, QString AOName)
{
    QString AOName1=AOName;

    AOName1.remove(0,1);

    //cout << AOName.toStdString().c_str()<<endl;
    //cout << AOName1.toStdString().c_str()<<endl;

    vector3 pos0 = pos1 - pos;
    double  x=pos0[0],
            y=pos0[1],
            z=pos0[2];

    double r=sqrt(x*x+y*y+z*z);
    if(r<0.001) r=0.001;


    double  x2=x*x;
    double  y2=y*y;
    double  z2=z*z;

    double  x4=x2*x2, y4=y2*y2,z4=z2*z2;

    double  r2=r*r;
    double  r3=r2*r;
    double  r4=r2*r2;

    double Y=1.0;
     if ( AOName1.startsWith ("S")  ) {
         Y=1.0/sqrt(PI)/2.0;
         m=0;
         return Y;
     }

     else if ( AOName1.startsWith("P")  ) {

         // |m|=abs(m)
         // NY = 1/sqrt(2*PI) * sqrt( (2*l+1)! * (l-|m|)! / (2* (l+|m| )! )
         //double NPY=1/sqrt(4*PI) * sqrt(3);
         //Ypx = NPY * x / r, Ypy = NPY * y / r, Ypz = NPY * z / r;

         if  (AOName1.startsWith("Px"))  {Y = sqrt(3.0/PI/4.0) * x / r; m=1; }
         if  (AOName1.startsWith("Py"))  {Y = sqrt(3.0/PI/4.0) * y / r; m=-1;}
         if  (AOName1.startsWith("Pz"))  {Y = sqrt(3.0/PI/4.0) * z / r; m=0;}

         //cout << m<<endl;
         //cout << "---------------------"<<endl;
         return Y;
     }

     else if ( AOName1.startsWith("D") ) {
               /*
               Ydxy = NDY * x * y / r2 ;
               Ydxz = NDY * x * z / r2 ;
                dyz = NDY * y * z / r2 ;

               Ydx2_y2 = 1/sqrt(4*PI)*sqrt(15/4) * ( x2 - y2 ) / r2 ;
               Ydz2 = 1/sqrt(4*PI)* sqrt(5/4) * ( 2*z2 - ( x2 + y2 ) ) / r2
               */

          if  (AOName1.startsWith("Dxz"))     {Y = sqrt(15.0/PI)/2.0 * x * z / r2 ;m=1;}
          if  (AOName1.startsWith("Dyz"))     {Y = sqrt(15.0/PI)/2.0 * y * z / r2 ;m=-1;}

          if  (AOName1.startsWith("Dxy"))     {Y = sqrt(15.0/PI)/2.0 * x * y / r2 ;m=2;}
          if  (AOName1.startsWith("Dx2-y2"))  {Y = sqrt(15.0/PI)/4.0 * ( x2 - y2 ) / r2 ;m=-2;}

          if  (AOName1.startsWith("Dz2"))     {Y = sqrt(5.0/PI)/4.0 * ( 2*z2 - ( x2 + y2 ) ) / r2 ;m=0;}
          return Y;
     }

     else if ( AOName1.startsWith("F")  )  {
               /*
               double NFY= 1/sqrt(4*PI) *sqrt(7/4);
               double C = 1/sqrt(4*PI);
               Yfx3  = C * sqrt(7/4) * x * ( 5*x2 - 3*r2 ) / r3 ;
               Yfy3  = C * sqrt(7/4) * y * ( 5*y2 - 3*r2 ) / r3 ;
               Yfz3  = C * sqrt(7/4) * z * ( 5*z2 - 3*r2 ) / r3 ;

               Yfxz2 = C * sqrt(42/16) * x *( 5*z2 - r2 ) / r3 ;
               Yfyz2 = C * sqrt(42/16) * y *( 5*z2 - r2 ) / r3 ;

               Yfy_3x2_y2 = C * sqrt(70/16) * y *( 3*x2 - y2 ) / r3 ;
               Yfx_x2_3y2 = C * sqrt(70/16) * x* ( x2 - 3*y2 ) / r3 ;

               Yfz_x2_y2 =  C * sqrt(105/4) * z * ( x2 - y2 ) / r3 ;
               Yfxyz =      C * sqrt(105/4) * x * y * z / r3 ;

               Yfx_z2_y2 = C * sqrt(105/4) * x *( z2 - y2 ) / r3 ;
               Yfy_z2_x2 = C * sqrt(105/4) * y *( z2 - x2 ) / r3 ;
                         */


               if  (AOName1.startsWith("Fx3"))    {Y = sqrt(70.0/PI)/8.0 * x * ( 5*x2 - 3*r2 ) / r3 ;}
               if  (AOName1.startsWith("Fy3"))    {Y =  sqrt(70.0/PI)/8.0  * y * ( 5*y2 - 3*r2 ) / r3 ;}
               if  (AOName1.startsWith("Fz3"))     {Y =  sqrt(7.0/PI)/4.0 * z * ( 5*z2 - 3*r2 ) / r3 ;m=0;}

               if  (AOName1.startsWith("Fxz2"))    {Y =  sqrt(42./PI)/8.0 * x *( 5*z2 - r2 ) / r3 ; m=1;}
               if  (AOName1.startsWith("Fyz2"))    {Y =  sqrt(42./PI)/8.0 * y *( 5*z2 - r2 ) / r3; m=-1;}


               //wrong here!
               //cout << m<< "inside ------------------------------------"<<endl;
               if  (AOName1.startsWith("Fz_x2_y2"))     {
                   cout << "something is wrong here\n";
                   Y =   sqrt(105.0/PI)/4.0 * z * ( x2 - y2 ) / r3 ;
                   m=2;
               }

               if  (AOName1.startsWith("Fxyz") || AOName1.startsWith("Fzxy")) {
                    cout << "something is wrong here\n";
                   Y =   sqrt(105.0/PI)/2.0 * x * y * z / r3 ;
                   m=-2;
               }


               //if  (AOName1.startsWith("Fx_z2_y2"))     {Y =   sqrt(70./PI)/8.0 * x *( z2 - y2 ) / r3 ;m=3;}
               //if  (AOName1.startsWith("Fy_z2_x2"))     {Y =   sqrt(70./PI)/8.0 * y *( z2 - x2 ) / r3 ;m=-3;}

               //wrong here!
               if  (AOName1.startsWith("Fx_x2_3y2"))    {
                   cout << "something is wrong here\n";
                   Y = sqrt(70./PI)/8.0  * x * ( x2 - 3*y2 ) / r3 ;
                   m=3;
               }

               if  (AOName1.startsWith("Fy_3x2_y2"))    {
                   cout << "something is wrong here\n";
                   Y = sqrt(70./PI)/8.0  * y * ( 3*x2 - y2 ) / r3 ;
                   m=-3;
               }

               return Y;
    }


     //wrong here!
     else if ( AOName1.startsWith("G"))
     {
              cout << "something is wrong inside all the G functions\n";

               double NY = 1 /(900*sqrt(70));
               /*
               Ygz4   = NY  * ( 35 * z4 - 30 * z2 * r2  + 3 * r4 ) / r4 ;
               Ygz3x  = NY * x * z *( 4*z2 - 3*x2 - 3*y2 ) / r4 ;
               Ygz3y  = NY * y* z *( 4*z2 - 3*x2 - 3*y2 ) / r4 ;
               Ygz2xy = NY * x* y* ( 6*z2 - x2 - y2 ) / r4 ;
               Ygz2_x2_y2 = NY * (x2 - y2) * (6*z2 - x2 - y2 ) / r4 ;
               Ygzx3 = NY * x * z * ( x2 - 3*y2 ) / r4 ;
               Ygzy3 = NY * y * z* ( 3*x2 - y2 ) / r4 ;
               Ygxy_x2_y2 = NY * x * y* ( x2 - y2 ) / r4 ;
               Ygx4_y4   = NY * (x4 + y4 - 6*x2 * y2 ) / r4 ;
              */

                if  (AOName1.startsWith("Gz4"))
                     {Y   = NY  * ( 35 * z4 - 30 * z2 * r2  + 3 * r4 ) / r4 ;}
                if  (AOName1.startsWith("Gz3x"))
                     {Y  = NY * x * z *( 4*z2 - 3*x2 - 3*y2 ) / r4 ;}
                if  (AOName1.startsWith("Gz3y"))
                     {Y  = NY * y* z *( 4*z2 - 3*x2 - 3*y2 ) / r4 ;}
                if  (AOName1.startsWith("Gz2xy"))
                     {Y = NY * x* y* ( 6*z2 - x2 - y2 ) / r4 ;}

                if  (AOName1.startsWith("Gz2_x2_y2"))
                     {Y = NY * (x2 - y2) * (6*z2 - x2 - y2 ) / r4 ;}

                if  (AOName1.startsWith("Gzx3"))
                    {Y = NY * x * z * ( x2 - 3*y2 ) / r4 ;}
                if  (AOName1.startsWith("Gzy3"))
                    {Y = NY * y * z* ( 3*x2 - y2 ) / r4 ;}

                if  (AOName1.startsWith("Gxy_x2_y2"))
                    {Y = NY * x * y* ( x2 - y2 ) / r4 ;}

                if  (AOName1.startsWith("Gx4_y4"))
                    {Y   = NY * (x4 + y4 - 6*x2 * y2 ) / r4 ;}
                return Y;
     }


     return Y;
}






void HAO::buildNodes()
{
     if(hasNodesBuild) return;

     lml=abs(m);
     uint nR=n-l-1;
     uint nT=l-lml;
     uint nP=lml;
     uint total=n-1;


     if (nR>0) buildRadialNodes();
     if (l>0)  buildAngularNodes();

     hasNodesBuild=true;

     cout <<"\n\n----------------------------------"<<endl;
     cout << "Hydrogen Atomic Nodes of "<< ObitalName.toStdString().c_str()<<":"<<endl;
     cout << "  Quantum number  n   l   m:"<< "  "<<n <<"  " <<l<<"  "<<m<< endl;
     cout << "  There are: n-l-1="<< nR <<" radial nodes"<<endl;
     cout << "  There are: l-|m|="<< nT <<" Theta/angular nodes"<<endl;
     cout << "  There are: |m|="<< nP <<" Phi/angular nodes"<<endl;
}





void HAO::buildRadialNodes()
{
    if (hasNodesBuild) return;


    lml=abs(m);

    uint nR=n-l-1;


    if (nR<1) return;

     vector < double >  poly_R, poly_gradR; //radial
     poly_R.clear(), poly_gradR.clear();

     double alpha=2.0*Z/n/a0;
     double  coeff_R=pow(alpha,3/2.0)*sqrt(Factorial(n-l-1)*Factorial(n+l)/n/2.0);
     double term=1.0;

     int k=n-l-1,j;
     for (j=0; j<=k; j++){
                 term=coeff_R*powl(-1.0,j+1)/Factorial(2*l+j+1)/Factorial(j)/Factorial(n-l-j-1);
                 poly_R.push_back(term);
                 //poly_gradR.push_back(term*(n-l-j-1));
     }

     double term0=poly_R[poly_R.size()-1];
     for (int i=0;i<poly_R.size();i++)
         poly_R[i]=poly_R[i]/term0;

     /*
     //search radial zero points=n-l-1
     QRRoots roots(nR);
     for (int i=0;i<poly_R.size();i++)
         roots.a[i]=poly_R[i];

     roots.qrSolve();
     Nodes_R.clear();
     for(int j=0; j<nR; j++) {
        Nodes_R.push_back(roots.xr[j]/alpha);
     }
*/

    SolveEquation roots(poly_R);
    roots.Solve();

    for(int j=0; j<nR; j++)
       cout << roots.answer[j]<<endl;

    Nodes_R.clear();
    if (roots.answer.size()>0)
    for(int j=0; j<nR; j++)
       Nodes_R.push_back(roots.answer[j]/alpha);

     //sort
     sort(Nodes_R.begin(), Nodes_R.end());
     std::reverse(Nodes_R.begin(),Nodes_R.end());

/*
     cout << poly_R.size()<<endl;
     cout << Nodes_R.size()<<endl;
     cout <<"------input-----------"<<endl;
     for (int i=0;i<poly_R.size();i++)
        cout << poly_R[i]<<endl;
     cout <<"------solution-----------"<<endl;
     for(int j=0; j<nR; j++)
        cout << Nodes_R[j]<<endl;

     cout <<"-----------------"<<endl;
*/

     hasNodesBuild=true;
}


double HAO::calc_least_r(double eps)
{
    buildRadialNodes();

    double r0=3.0;
    if (Nodes_R.size()>0)
        r0=Nodes_R[0];
    else
        r0=1.0*n;


    double r=r0;



    //scan R
    cout << "begin scan R:"<<endl;
    while (1)
    {
         r=r+1.0;
         if (abs(Hydrogen_R(r))>eps) continue;
         break;
    }

    cout << "done!"<<endl;
    cout << "the least r is "<<r<<endl;

    return r;
}


double HAO::calc_least_rdf()
{
    buildRadialNodes();

    double eps=0.001;

    double r0=3.0;
    if (Nodes_R.size()>0)
        r0=Nodes_R[0];
    else
        r0=1.0*n;


    double r=r0;

    //scan R
    cout << "begin scan R:"<<endl;
    while (1)
    {
         r=r+0.5;
         if (abs(Hydrogen_RDF(r))>eps) continue;
         break;
    }

    cout << "done!"<<endl;
    cout << "the least r is "<<r<<endl;

    return r;
}





 void HAO::buildAngularNodes()
 {
     lml=abs(m);
     uint nT=l-lml;
     uint nP=lml;


    if (nT>0) {
        int m2=m+lml;
        QVector < double >  poly_T, poly_gradT; //theta
        poly_T.clear(), poly_gradT.clear();

        double term=1.0;
        double  Coeff_T=powl(-1.0, m2/2)/powl(2.0,l)*sqrt(l+0.5) * sqrt( Factorial(l-lml)/Factorial(l+lml));

        int k=floor((l-lml)/2);
        for (int j=0; j<=k; j++) {
                 term=Coeff_T*powl(-1.0,j)*Factorial(2*l-2*j)/Factorial(j)/Factorial(l-j)/Factorial(l-lml-2*j);
                 poly_T.push_back( term );
                 poly_gradT.push_back( term * (l-lml-2*j));
        }

        int size=poly_T.size();
        QRRoots roots(size-1);
        double term0=poly_T[size-1];
        for (int i=0;i<size;i++) {
              roots.a[i]=poly_T[size-i-1]/term0;
        }


        //the roots of associated is cos(theta)^2
        roots.qrSolve();
        Nodes_T.clear();
        uint n=0, j=0;
        while (1)
        {
            //cout << roots.xr[j]<<endl;
            if (abs(roots.xr[j]) <0.01) {
                Nodes_T.push_back(0.01);
                n=n+1;
                isXOYNode=true;
            }
            else {
                Nodes_T.push_back(roots.xr[j]);
                n=n+2;
            }
            if (n>=nT) break;
            j=j+1;
        }
    }



    Nodes_P.clear();
    if (lml<1) return;
    double phi=180.0/2/lml;
    for(int k=0; k<lml; k++)
    {
        if (m>0) //sin
            Nodes_P.push_back(phi*k*2+phi);
        else
            Nodes_P.push_back(phi*k*2);
    }

}







double HMO::calc_Psi  (double x, double y, double z)
{
    vector3 pos(x,y,z);
    return calc_Psi(pos);
}


double HMO::calc_Psi(vector3 pos)
{
    Psi=0.0;
    for (int i=0;i<AOs.size();i++)
    {
        if (isHybridizedOrbital || isMolecularOrbital)
            Psi+=AOs[i]->Slater_Psi(pos);
        else
            Psi+=AOs[i]->Hydrogen_Psi(pos);
            //Psi+=AOs[i]->HydrogenPsi_byName(pos);
            //Psi+=AOs[i]->Slater_Psi(pos);
    }
    return Psi;
}




void HMO::buildCloudyPoints(int N)
{
/*
    NPoints=N;
    long MaxN=N;

    vector3 pos;
    long x, y, z;

    Points.clear();
    while (MaxN--)
    {
        x=rnd(-10,10);
        y=rnd(-10,10);
        z=rnd(-10,10);

        pos.Set(x,y,z);

        Points.push_back(pos);

        int phase = 1;
        phase_Points.push_back(phase);

     //if (cut_zone(x, cut, 0))       continue;

     //double val = calc_Psi(pos);
  }
*/
}

void HMO::clear()
{
    cout <<"-----------------------------"<<endl;
    cout << "Cleaning HMO begins..."<<endl;

    visible_pos_Lobe_Mesh=true;
    visible_neg_Lobe_Mesh=true;
    orbital_type=FULL;


    if (dataXOY.size()>0) {
        for (uint i=0;i<dataXOY.size();i++)
            dataXOY[i].clear();
        dataXOY.clear();
        cout << "dataXOY  are done!"<<endl;
    }

    if (dataXOZ.size()>0) {
        for (uint i=0;i<dataXOZ.size();i++)
            dataXOZ[i].clear();
        dataYOZ.clear();
        cout << "dataYOZ  are done!"<<endl;
    }

    if (dataXOZ.size()>0) {
        for (uint i=0;i<dataYOZ.size();i++)
            dataYOZ[i].clear();
        dataXOZ.clear();
        cout << "dataXOZ  are done!"<<endl;
    }

   uint size=AOs.size();
   if (size>0) {
       while (size--) {
           if (AOs[size]!=nullptr)
               delete AOs[size];
       }
       AOs.clear();
       cout << "AOs  are done!"<<endl;
   }

   if (RData.size()+R2Data.size() + RDFData.size()>0) {
       if (RData.size()>0)  RData.clear();
       if (R2Data.size()>0)  R2Data.clear();
       if (RDFData.size()>0)  RDFData.clear();
       cout << "RData  is done!"<<endl;
   }

  if (pData)   {
      pData=vtkPolyData::New();
      pData->Delete();
      pData=nullptr;
      cout << "pData  is done!"<<endl;
  }

  if (pData1)   {
      pData1=vtkPolyData::New();
      pData1->Delete();
      pData1=nullptr;
      cout << "pData1  is done!"<<endl;

  }

  if (pData0)   {
      pData0=vtkPolyData::New();
      pData0->Delete();
      pData0=nullptr;
      cout << "pData0  is done!"<<endl;
  }



   if (surfaceMC0) {
       surfaceMC0=vtkSmartPointer<vtkMarchingCubes>::New();
       surfaceMC0->Delete();
       //surfaceMC0=nullptr;
       cout << "surfaceMC0  is done!"<<endl;
   }


   if (surfaceMC1) {
       surfaceMC1=vtkSmartPointer<vtkMarchingCubes>::New();
       surfaceMC1->Delete();
       //surfaceMC1=nullptr;
       cout << "surfaceMC1  is done!"<<endl;
   }


   if (Surface_MC_Zero) {
       Surface_MC_Zero=vtkSmartPointer<vtkMarchingCubes>::New();
       Surface_MC_Zero->Delete();
       //Surface_MC_Zero=nullptr;
       cout << "Surface_MC_Zero  is done!"<<endl;
   }


   cout << "surfaceMC is done!"<<endl;

   if (surfaceFE0) {
       surfaceFE0=vtkSmartPointer<vtkFlyingEdges3D>::New();
       surfaceFE0->Delete();
       //surfaceFE0=nullptr;

       cout << "surfaceFE0 is done!"<<endl;
   }


   if (surfaceFE1) {
       surfaceFE1=vtkSmartPointer<vtkFlyingEdges3D>::New();
       surfaceFE1->Delete();
       //surfaceFE1=nullptr;
       cout << "surfaceFE1 is done!"<<endl;
   }


   if (Surface_FE_Zero) {
       Surface_FE_Zero=vtkSmartPointer<vtkFlyingEdges3D>::New();
       Surface_FE_Zero->Delete();
       //Surface_FE_Zero=nullptr;
       cout << "Surface_FE_Zero is done!"<<endl;
   }


   if (CubeDataXOY)     {delete CubeDataXOY;CubeDataXOY=nullptr;}
   if (CubeDataXOZ)     {delete CubeDataXOZ;CubeDataXOZ=nullptr;}
   if (CubeDataYOZ)     {delete CubeDataYOZ;CubeDataYOZ=nullptr;}
   if (CubeDataWidge)   {delete CubeDataWidge;CubeDataWidge=nullptr;}
   if (CubeDataQuarter) {delete CubeDataQuarter;CubeDataQuarter=nullptr;}
   if (CubeDataCorner)  {delete CubeDataCorner;CubeDataCorner=nullptr;}

   if (CubeDataQuarter1) {delete CubeDataQuarter1;;CubeDataQuarter1=nullptr;}
   if (CubeDataQuarter2) {delete CubeDataQuarter2;;CubeDataQuarter2=nullptr;}


   proj_scale=2;

   cout << "CubeData is done!"<<endl;
   cout <<"-----------------------------"<<endl;
}

void HMO::sendMessage(QString message)
{
    if (!m_parent || m_parent==nullptr) return;
    ((MainWindow *) m_parent)->sendMessage(message);
}

void HMO::appendMessage(QString  message)
{
    if (!m_parent || m_parent==nullptr) return;
    ((MainWindow *) m_parent)->appendMessage(message);
}


void HMO::buildCubeData()
{
    uint n = getAO(0)->n;

    bool hasNodesBuild=getAO(0)->hasNodesBuild;
    uint NumRadialNodes=getAO(0)->Nodes_R.size();


    if (n>=7 ) {
        isoValue=0.0001;
        maxBox=220.0; //200
        NPoints=120;
        if (NumRadialNodes<1)
            NPoints=60;
    }


    switch (n)
    {
    case 6:
        isoValue=0.0005;
        maxBox=180.0;
        NPoints=120;
        if (NumRadialNodes<1)    NPoints=80;
        break;

    case 5:
        isoValue=0.001;
        maxBox=150.0;
        NPoints=120;
        if (NumRadialNodes<1)   NPoints=80;
        break;

    case 4:
        isoValue=0.002;
        maxBox=90.0;
        NPoints=120;
        if (NumRadialNodes<1) NPoints=60;
        break;

    case 3:
        isoValue=0.006;
        maxBox=40.0;
        NPoints=100;
        if (NumRadialNodes<1) NPoints=60;
        break;

    case 2:
        isoValue=0.01;
        maxBox=50.0;
        NPoints=100;
        break;

    case 1:
        isoValue=0.05;
        maxBox=5.0;
        NPoints=50;
        break;
    }

    int Nx=NPoints,Ny=NPoints,Nz=NPoints;
    int N=NPoints;


    //find the smallest r
    double r=0;
    if (this->NumAOs()<2){
        r=this->getAO(0)->calc_least_r(isoValue/5.0);
    }
    if (r>1.0) maxBox=floor(r);
    minBox=-maxBox;


/*
    cout << "the least r is "<<r <<" inside buildCubeData()"<<endl;
    cout << "default :"<<endl;
    cout << " "<< maxBox<<  " "<< minBox<< " " <<isoValue<< endl;

    maxBox=r;
    minBox=-r;
    isoValue=isov*2.0;

    cout << "modified :"<<endl;
    cout << " "<< maxBox<<  " "<< minBox<< " " <<isoValue<< endl;
*/

    //includes (0,0,0), and do not jump over
    interval=(maxBox-minBox)/N;
    maxBox=interval*N/2.0;

    minBox=-maxBox;

    int x,y,z;
    double  x0=minBox,
            y0=minBox,
            z0=minBox;


    sendMessage("Building cube data ... ");
    sendMessage(QString("dim:     %1*%1*%1").arg(N));
    sendMessage(QString("range:   %1   %2").arg(minBox).arg(maxBox));
    sendMessage(QString("interval:  %1").arg(interval));
    sendMessage(QString("interval:  %1").arg(isoValue));



    cout << "\nbuilding cube data  " <<endl;
    cout << "dim: " <<N <<endl;
    cout << "range: " <<minBox  <<" " << maxBox<< endl;
    cout << "interval: " <<interval  << endl;
    cout << "isovalue: " <<isoValue  << endl<< endl;


    if (CubeData || CubeData!=nullptr ) {
        delete CubeData;
        CubeData=nullptr;
    }
    CubeData = new double [Nx*Ny*Nz];

    //calculate Psi
    double psi=0.0,sum=0.0, psi2=0.0;

#pragma omp parallel for
    for( z=0; z < Nz ; z++)
        for( y=0; y < Ny;  y++)
            for( x=0; x < Nx;  x++){
                psi=calc_Psi(x0+interval*x,y0+interval*y , z0+interval*z);
                //psi2=psi*psi;
                //sum += psi2;

                //if  (psi>0.0)     CubeData[(z*N+y)*N+x]=psi2;
                //else              CubeData[(z*N+y)*N+x]=-psi2;

                CubeData[(z*N+y)*N+x]=psi;
            }


    /*
    sum=sum/(Nx*Ny*Nz);
    //normalize
    for( z=0; z < Nz;  z++){
        for( y=0; y < Ny;  y++){
            for( x=0; x < Nx;  x++){
                psi=CubeData[(z*N+y)*N+x];
                CubeData[(z*N+y)*N+x]=psi/sum;
            }
        }
    }
*/




    //dispose five kinds of data
    if (CubeDataXOY || CubeDataXOY!=nullptr) {
        delete CubeDataXOY;
    }

    if (CubeDataXOZ || CubeDataXOZ!=nullptr) {
         delete CubeDataXOZ;
    }

    if (CubeDataYOZ || CubeDataYOZ!=nullptr) {
         delete CubeDataYOZ;
    }

    if (CubeDataWidge || CubeDataWidge!=nullptr) {
         delete CubeDataWidge;
    }

    if (CubeDataQuarter || CubeDataQuarter!=nullptr) {
         delete CubeDataQuarter;
    }

    if (CubeDataQuarter1 || CubeDataQuarter1!=nullptr) {
         delete CubeDataQuarter1;
    }

    if (CubeDataQuarter2 || CubeDataQuarter2!=nullptr) {
         delete CubeDataQuarter2;
    }

    if (CubeDataCorner || CubeDataCorner!=nullptr) {
         delete CubeDataCorner;
    }




    CubeDataXOY= new double [Nx*Ny*Nz];
    CubeDataXOZ= new double [Nx*Ny*Nz];
    CubeDataYOZ= new double [Nx*Ny*Nz];

    CubeDataWidge= new double [Nx*Ny*Nz];
    CubeDataQuarter= new double [Nx*Ny*Nz];
    CubeDataCorner= new double [Nx*Ny*Nz];

    CubeDataQuarter1= new double [Nx*Ny*Nz];
    CubeDataQuarter2= new double [Nx*Ny*Nz];



    //a[M][N][P],  a[m][n][p]， a[(m*N+n)*P+p]
    //a[Nz][Ny][Nx],a[z][y][x], a[(z*Ny+y)*Nx+x]

    double d=0.0;


#pragma omp parallel for
    for( z=0; z < Nz;  z++){
        for( y=0; y < Ny;  y++){
            for( x=0; x < Nx;  x++)
            {
                d=CubeData[(z*Ny+y)*Nx+x];
                CubeDataXOY[(z*Ny+y)*Nx+x]=d;
                if (z0+interval*z > 0.0)
                    CubeDataXOY[(z*Ny+y)*Nx+x]=0.0;

                CubeDataYOZ[(z*Ny+y)*Nx+x]=d;
                if (x0+interval*x > 0.0)
                    CubeDataYOZ[(z*Ny+y)*Nx+x]=0.0;

                CubeDataXOZ[(z*Ny+y)*Nx+x]=d;
                if (y0+interval*y > 0.0)
                    CubeDataXOZ[(z*Ny+y)*Nx+x]=0.0;


                CubeDataWidge[(z*Ny+y)*Nx+x]=d;
                if ( (y0+interval*y > 0.0)  &&  (z0+interval*z > 0.0) )
                    CubeDataWidge[(z*Ny+y)*Nx+x]=0.0;

                //z<0 and y<0;
                CubeDataQuarter[(z*Ny+y)*Nx+x]=d;
                if ( (z0+interval*z > 0.0)  )
                    CubeDataQuarter[(z*Ny+y)*Nx+x]=0.0;
                if ( (y0+interval*y > 0.0)  )
                    CubeDataQuarter[(z*Ny+y)*Nx+x]=0.0;

                //z<0 and y>0;
                CubeDataQuarter1[(z*Ny+y)*Nx+x]=d;
                if ( (z0+interval*z > 0.0)  )
                    CubeDataQuarter1[(z*Ny+y)*Nx+x]=0.0;
                if ( (y0+interval*y < 0.0)  )
                    CubeDataQuarter1[(z*Ny+y)*Nx+x]=0.0;


                //z>0 and y>0;
                CubeDataQuarter2[(z*Ny+y)*Nx+x]=d;
                if ( (z0+interval*z < 0.0)  )
                    CubeDataQuarter2[(z*Ny+y)*Nx+x]=0.0;
                if ( (y0+interval*y < 0.0)  )
                    CubeDataQuarter2[(z*Ny+y)*Nx+x]=0.0;

                CubeDataCorner[(z*Ny+y)*Nx+x]=d;
                if ( (x0+interval*x > 0.0)  )
                    CubeDataCorner[(z*Ny+y)*Nx+x]=0.0;
                if ( (z0+interval*z > 0.0)  )
                    CubeDataCorner[(z*Ny+y)*Nx+x]=0.0;
                if ( (y0+interval*y > 0.0)  )
                    CubeDataCorner[(z*Ny+y)*Nx+x]=0.0;
            }
        }
    } 

    appendMessage("OK!");
    cout << "OK!"<<endl;
}



void HMO::ExtractIsosurface(vtkImageData * volume0, vtkImageData * volume1)
{
    //extractIsoSurfMethod=FlyingEdge;
    extractIsoSurfMethod=MarchingCube;

    switch (extractIsoSurfMethod)
    {
    case MarchingCube:
        sendMessage("Extract isosurface based on MarchingCube algorithms...");

        //positlve lobe
        surfaceMC0 = vtkSmartPointer<vtkMarchingCubes>::New();
        surfaceMC0->SetInputData(volume0);
        surfaceMC0->SetValue(0, isoValue);

        surfaceMC0->ComputeNormalsOn();
        //MoSurface_positive->ComputeGradientsOn();
        //MoSurface_positive->ComputeScalarsOn();
        surfaceMC0->Update();

        pData=vtkPolyData::New();
        pData=surfaceMC0->GetOutput();
        //MoSurface_positive->Delete();

        //negative
        surfaceMC1 = vtkSmartPointer<vtkMarchingCubes>::New();
        surfaceMC1->SetInputData(volume1);
        surfaceMC1->SetValue(0, isoValue);


        surfaceMC1->ComputeNormalsOn();
        //MoSurface_positive->ComputeGradientsOn();
        //MoSurface_positive->ComputeScalarsOn();
        surfaceMC1->Update();

        pData1=vtkPolyData::New();
        pData1=surfaceMC1->GetOutput();
        //MoSurface_positive->Delete();


        /*
        Surface_MC_Zero=vtkSmartPointer<vtkMarchingCubes>::New();
        Surface_MC_Zero->New();
        Surface_MC_Zero->SetInputData(volume0);
        Surface_MC_Zero->ComputeNormalsOn();
        Surface_MC_Zero->SetValue(0, 0.0);
        Surface_MC_Zero->ComputeGradientsOn();
        Surface_MC_Zero->ComputeScalarsOn();
        Surface_MC_Zero->Update();
        pData0=Surface_MC_Zero->GetOutput();
        */
        break;

    case FlyingEdge:
        sendMessage("Extract isosurface based on FlyingEdge algorithms...");
        //positive
        surfaceFE0 = vtkSmartPointer<vtkFlyingEdges3D>::New();
        surfaceFE0->SetInputData(volume0);

        surfaceFE0->ComputeNormalsOn();
        surfaceFE0->SetValue(0, isoValue);
        surfaceFE0->ComputeGradientsOn();
        surfaceFE0->ComputeScalarsOn();
        surfaceFE0->Update();
        pData=surfaceFE0->GetOutput();


        //negative
        surfaceFE1 = vtkSmartPointer<vtkFlyingEdges3D>::New();
        surfaceFE1->SetInputData(volume1);

        surfaceFE1->ComputeNormalsOn();
        surfaceFE1->SetValue(0, isoValue);
        surfaceFE1->ComputeGradientsOn();
        surfaceFE1->ComputeScalarsOn();
        surfaceFE1->Update();
        pData1=surfaceFE1->GetOutput();

        /*
        Surface_FE_Zero=vtkSmartPointer<vtkFlyingEdges3D>::New();
        Surface_FE_Zero->New();
        Surface_FE_Zero->SetInputData(volume0);
        Surface_FE_Zero->ComputeNormalsOn();
        Surface_FE_Zero->SetValue(0, 0.0);
        Surface_FE_Zero->ComputeGradientsOn();
        Surface_FE_Zero->ComputeScalarsOn();
        Surface_FE_Zero->Update();
        pData0=Surface_FE_Zero->GetOutput();
        */
        break;

    case  MarchingTetrahedra:
        break;

    case  DualContouring:
        sendMessage("Extract isosurface based on DualContouring algorithms...");
        break;

    case  DiscretekdHierachy:
        sendMessage("Extract isosurface based on Discrete k-d Hierachy algorithms...");
        break;
    }

    appendMessage("OK!");
}



//method1
void HMO::buildWidgeIsosurface1()
{
    int dim=NPoints;
    interval=(maxBox-minBox)/dim;

    vtkImageData * volume0 = vtkImageData::New();
    volume0->SetDimensions(dim,dim,dim);
    volume0->SetSpacing(interval, interval,interval);
    volume0->AllocateScalars(VTK_DOUBLE,1);
    volume0->SetOrigin(-maxBox,-maxBox,-maxBox);
    volume0->Modified();

    vtkImageData * volume1 = vtkImageData::New();
    volume1->SetDimensions(dim,dim,dim);
    volume1->SetSpacing(interval, interval,interval);
    volume1->AllocateScalars(VTK_DOUBLE,1);
    volume1->SetOrigin(-maxBox,-maxBox,-maxBox);
    volume1->Modified();


#pragma omp parallel for
    for(int z=0; z < dim;  z++)
        for(int y=0; y < dim;  y++)
            for(int x=0; x < dim;  x++)
            {
                double * d = static_cast<double*>(volume0->GetScalarPointer(x,y,z));
                double * d1 = static_cast<double*>(volume1->GetScalarPointer(x,y,z));
                d[0]=CubeDataXOY[(z*dim+y)*dim+x];
                d1[0]=-CubeDataXOY[(z*dim+y)*dim+x];
            }


    ExtractIsosurface(volume0, volume1);

#pragma omp parallel for
    for (int i=0;i<pData->GetNumberOfPoints();i++)
    {
        double * p = pData->GetPoint(i);
        if (p[2] < 0.0) continue;
        p[2]=0.0;
        pData->GetPoints()->SetPoint(i,p);
    }
    auto appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
    appendFilter->AddInputData(pData);


 #pragma omp parallel for
    for (int i=0;i<pData1->GetNumberOfPoints();i++)
    {
        double * p = pData1->GetPoint(i);
        if (p[2] < 0.0) continue;
        p[2]=0.0;
        pData1->GetPoints()->SetPoint(i,p);
    }

    auto appendFilter1 = vtkSmartPointer<vtkAppendPolyData>::New();
    appendFilter1->AddInputData(pData1);


#pragma omp parallel for
    for(int z=0; z < dim;  z++)
        for(int y=0; y < dim;  y++)
            for(int x=0; x < dim;  x++)
            {
                double * d = static_cast<double*>(volume0->GetScalarPointer(x,y,z));
                double * d1 = static_cast<double*>(volume1->GetScalarPointer(x,y,z));
                d[0]=CubeDataXOZ[(z*dim+y)*dim+x];
                d1[0]=-CubeDataXOZ[(z*dim+y)*dim+x];
            }


    ExtractIsosurface(volume0, volume1);
#pragma omp parallel for
    for (int i=0;i<pData->GetNumberOfPoints();i++)
    {
        double * p = pData->GetPoint(i);
        if (p[1] < 0.0) continue;
        p[1]=0.0;
        pData->GetPoints()->SetPoint(i,p);
    }
    appendFilter->AddInputData(pData);
    appendFilter->Update();

#pragma omp parallel for
    for (int i=0;i<pData1->GetNumberOfPoints();i++)
    {
        double * p = pData1->GetPoint(i);
        if (p[1] < 0.0) continue;
        p[1]=0.0;
        pData1->GetPoints()->SetPoint(i,p);
    }
    appendFilter1->AddInputData(pData1);
    appendFilter1->Update();


    auto cleanFilter =  vtkSmartPointer<vtkCleanPolyData>::New();
    cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
    //cleanFilter->SetTolerance(0.001);
    cleanFilter->Update();


    auto cleanFilter1 =  vtkSmartPointer<vtkCleanPolyData>::New();
    cleanFilter1->SetInputConnection(appendFilter1->GetOutputPort());
    //cleanFilter->SetTolerance(0.001);
    cleanFilter1->Update();


    pData->DeepCopy(cleanFilter->GetOutput());
    pData1->DeepCopy(cleanFilter1->GetOutput());
}

//method2, also not good
void HMO::buildWidgeIsosurface()
{
    int dim=NPoints;
    interval=(maxBox-minBox)/dim;

    vtkImageData * volume0 = vtkImageData::New();
    volume0->SetDimensions(dim,dim,dim);
    volume0->SetSpacing(interval, interval,interval);
    volume0->AllocateScalars(VTK_DOUBLE,1);
    volume0->SetOrigin(-maxBox,-maxBox,-maxBox);
    volume0->Modified();

    vtkImageData * volume1 = vtkImageData::New();
    volume1->SetDimensions(dim,dim,dim);
    volume1->SetSpacing(interval, interval,interval);
    volume1->AllocateScalars(VTK_DOUBLE,1);
    volume1->SetOrigin(-maxBox,-maxBox,-maxBox);
    volume1->Modified();

    //build XOY ------------------------------------------------------------------
#pragma omp parallel for
    for(int z=0; z < dim;  z++)
#pragma omp parallel for
        for(int y=0; y < dim;  y++)
#pragma omp parallel for
            for(int x=0; x < dim;  x++)
            {
                double * d = static_cast<double*>(volume0->GetScalarPointer(x,y,z));
                double * d1 = static_cast<double*>(volume1->GetScalarPointer(x,y,z));
                d[0]=CubeDataXOY[(z*dim+y)*dim+x];
                d1[0]=-CubeDataXOY[(z*dim+y)*dim+x];
            }
    ExtractIsosurface(volume0, volume1);

    //adjust manually
#pragma omp parallel for
    for (int i=0;i<pData->GetNumberOfPoints();i++)
    {
        double * p = pData->GetPoint(i);
        if (p[2] > 0.0) {
            p[2]=0.0;
            pData->GetPoints()->SetPoint(i,p);
        }
    }

#pragma omp parallel for
    for (int i=0;i<pData1->GetNumberOfPoints();i++)
    {
        double * p = pData1->GetPoint(i);
        if (p[2] > 0.0) {
            p[2]=0.0;
            pData1->GetPoints()->SetPoint(i,p);
        }
    }




    //cut
    auto  planeYZ  =   vtkSmartPointer<vtkPlane>::New();
    planeYZ->SetOrigin(0, 0, 0);
    planeYZ->SetNormal(0, 1, 0);

    auto clipperYZ =vtkSmartPointer<vtkClipPolyData>::New();
    clipperYZ->SetInputData(pData);
    clipperYZ->SetClipFunction(planeYZ);
    clipperYZ->GenerateClipScalarsOn();
    clipperYZ->GenerateClippedOutputOn();
    //clipperYZ->SetValue(0.0);
    clipperYZ->Update();


    //dispose negative data
    auto clipperYZ1 =vtkSmartPointer<vtkClipPolyData>::New();
    clipperYZ1->SetInputData(pData1);
    clipperYZ1->SetClipFunction(planeYZ);
    clipperYZ1->GenerateClipScalarsOn();
    clipperYZ1->GenerateClippedOutputOn();
    //clipperYZ1->SetValue(0.0);
    clipperYZ1->Update();


    //store
    auto data1_p =  vtkSmartPointer<vtkPolyData>::New();
    auto data1_n =  vtkSmartPointer<vtkPolyData>::New();
    data1_p->DeepCopy(clipperYZ->GetOutput());
    data1_n->DeepCopy(clipperYZ1->GetOutput());




    //build XOZ ------------------------------------------------------------------
#pragma omp parallel
    #pragma omp for
    for(int z=0; z < dim;  z++)
#pragma omp parallel
        #pragma omp for
        for(int y=0; y < dim;  y++)
#pragma omp parallel
            #pragma omp for
            for(int x=0; x < dim;  x++)
            {
                double * d = static_cast<double*>(volume0->GetScalarPointer(x,y,z));
                double * d1 = static_cast<double*>(volume1->GetScalarPointer(x,y,z));
                d[0]=CubeDataXOZ[(z*dim+y)*dim+x];
                d1[0]=-CubeDataXOZ[(z*dim+y)*dim+x];
            }


    ExtractIsosurface(volume0, volume1);


    //adjust
#pragma omp parallel
    #pragma omp for
    for (int i=0;i<pData->GetNumberOfPoints();i++)
    {
        double * p = pData->GetPoint(i);
        if (p[1] > 0.0) {
            p[1]=0.0;
            pData->GetPoints()->SetPoint(i,p);
        }
    }

#pragma omp parallel
    #pragma omp for
    for (int i=0;i<pData1->GetNumberOfPoints();i++)
    {
        double * p = pData1->GetPoint(i);
        if (p[1] > 0.0) {
            p[1]=0.0;
            pData1->GetPoints()->SetPoint(i,p);
        }
    }


    //cut
    auto  planeXY  =   vtkSmartPointer<vtkPlane>::New();
    planeXY->SetOrigin(0, 0, 0);
    planeXY->SetNormal(0, 0, 1);


    auto clipperXY =vtkSmartPointer<vtkClipPolyData>::New();
    clipperXY->SetInputData(pData);
    clipperXY->SetClipFunction(planeXY);
    clipperXY->GenerateClipScalarsOn();
    clipperXY->GenerateClippedOutputOn();
    clipperXY->Update();

    //dispose negative data
    auto clipperXY1 =vtkSmartPointer<vtkClipPolyData>::New();
    clipperXY1->SetInputData(pData1);
    clipperXY1->SetClipFunction(planeXY);
    clipperXY1->GenerateClipScalarsOn();
    clipperXY1->GenerateClippedOutputOn();
    clipperXY1->Update();



    //store
    auto data2_p =  vtkSmartPointer<vtkPolyData>::New();
    auto data2_n =  vtkSmartPointer<vtkPolyData>::New();
    data2_p->DeepCopy(clipperXY->GetOutput());
    data2_n->DeepCopy(clipperXY1->GetOutput());


    //build quarter ------------------------------------------------------------------
#pragma omp parallel for
    for(int z=0; z < dim;  z++)
#pragma omp parallel for
        for(int y=0; y < dim;  y++)
#pragma omp parallel for
            for(int x=0; x < dim;  x++)
            {
                double * d = static_cast<double*>(volume0->GetScalarPointer(x,y,z));
                double * d1 = static_cast<double*>(volume1->GetScalarPointer(x,y,z));
                d[0]=CubeDataQuarter[(z*dim+y)*dim+x];
                d1[0]=-CubeDataQuarter[(z*dim+y)*dim+x];
            }


    ExtractIsosurface(volume0, volume1);

    //adjust
#pragma omp parallel for
    for (int i=0;i<pData->GetNumberOfPoints();i++)
    {
        double * p = pData->GetPoint(i);
        if (p[2] > 0.0) p[2]=0.0;
        if (p[1] > 0.0) p[1]=0.0;

        pData->GetPoints()->SetPoint(i,p);
    }

#pragma omp parallel for
    for (int i=0;i<pData1->GetNumberOfPoints();i++)
    {
        double * p = pData1->GetPoint(i);
        if (p[2] > 0.0) p[2]=0.0;
        if (p[1] > 0.0) p[1]=0.0;

        pData1->GetPoints()->SetPoint(i,p);
    }


    planeXY->SetNormal(0, 0, -1);
    clipperXY->SetInputData(pData);
    clipperXY->SetClipFunction(planeXY);
    clipperXY->GenerateClipScalarsOn();
    clipperXY->GenerateClippedOutputOn();
    clipperXY->Update();

    planeYZ->SetNormal(0, -1, 0);
    clipperYZ->SetInputData(clipperXY->GetOutput());
    clipperYZ->SetClipFunction(planeYZ);
    clipperYZ->GenerateClipScalarsOn();
    clipperYZ->GenerateClippedOutputOn();
    clipperYZ->Update();


    clipperXY1->SetInputData(pData1);
    clipperXY1->SetClipFunction(planeXY);
    clipperXY1->GenerateClipScalarsOn();
    clipperXY1->GenerateClippedOutputOn();
    clipperXY1->Update();

    clipperYZ1->SetInputData(clipperXY1->GetOutput());
    clipperYZ1->SetClipFunction(planeYZ);
    clipperYZ1->GenerateClipScalarsOn();
    clipperYZ1->GenerateClippedOutputOn();
    clipperYZ1->Update();

    //store
    auto data3_p =  vtkSmartPointer<vtkPolyData>::New();
    auto data3_n =  vtkSmartPointer<vtkPolyData>::New();
    data3_p->DeepCopy(clipperYZ->GetOutput());
    data3_n->DeepCopy(clipperYZ1->GetOutput());



    //combine three parts
    auto appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
    appendFilter->AddInputData(data1_p);
    appendFilter->AddInputData(data2_p);
    appendFilter->AddInputData(data3_p);

    auto appendFilter1 = vtkSmartPointer<vtkAppendPolyData>::New();
    appendFilter1->AddInputData(data1_n);
    appendFilter1->AddInputData(data2_n);
    appendFilter1->AddInputData(data3_n);



    //the latest filter
    auto cleanFilter =  vtkSmartPointer<vtkCleanPolyData>::New();
    cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
    //cleanFilter->SetTolerance(0.001);
    cleanFilter->Update();


    auto cleanFilter1 =  vtkSmartPointer<vtkCleanPolyData>::New();
    cleanFilter1->SetInputConnection(appendFilter1->GetOutputPort());
    //cleanFilter1->SetTolerance(0.001);
    cleanFilter1->Update();



    pData->DeepCopy(cleanFilter->GetOutput());
    pData1->DeepCopy(cleanFilter1->GetOutput());



    //refresh pData
    auto points=vtkSmartPointer<vtkPoints>::New();
    for (int i=0;i<pData->GetNumberOfPoints();i++)
    {
        double * p = pData->GetPoint(i);
        points->InsertNextPoint(p[0], p[1], p[2]);
    }

    auto Points=vtkSmartPointer<vtkPolyData>::New();
    Points->SetPoints(points);
    auto surf=vtkSmartPointer<vtkSurfaceReconstructionFilter>::New();
    surf->SetNeighborhoodSize(4);
    surf->SetInputData(Points);
    pData->DeepCopy(surf->GetOutput());


    auto points1=vtkSmartPointer<vtkPoints>::New();
    for (int i=0;i<pData1->GetNumberOfPoints();i++)
    {
        double * p = pData1->GetPoint(i);
        points1->InsertNextPoint(p[0], p[1], p[2]);
    }


    auto Points1=vtkSmartPointer<vtkPolyData>::New();
    Points1->SetPoints(points1);
    auto surf1=vtkSmartPointer<vtkSurfaceReconstructionFilter>::New();
    surf1->SetNeighborhoodSize(4);
    surf1->SetInputData(Points1);
    pData1->DeepCopy(surf1->GetOutput());


    auto skinNormals =  vtkSmartPointer<vtkPolyDataNormals>::New();
    skinNormals->SetInputData(pData);
    skinNormals->SetFeatureAngle(80.0);
    skinNormals->ComputePointNormalsOn();
    skinNormals->ComputeCellNormalsOn();
    skinNormals->SetSplitting(1);
    skinNormals->SetConsistency(0);
    skinNormals->SetAutoOrientNormals(0);
    skinNormals->SetFlipNormals(0);
    skinNormals->SetNonManifoldTraversal(1);
    skinNormals->Update();
    pData->DeepCopy(skinNormals->GetOutput());


    auto skinNormals1 = vtkSmartPointer<vtkPolyDataNormals>::New();
    skinNormals1->SetInputData(pData1);
    skinNormals1->SetFeatureAngle(80.0);
    skinNormals1->ComputePointNormalsOn();
    skinNormals1->ComputeCellNormalsOn();

    skinNormals1->SetSplitting(1);
    skinNormals1->SetConsistency(0);
    skinNormals1->SetAutoOrientNormals(0);
    skinNormals1->SetFlipNormals(0);
    skinNormals1->SetNonManifoldTraversal(1);
    skinNormals1->Update();
    pData1->DeepCopy(skinNormals1->GetOutput());


 /*
    // Create a dataset from the grid points
    auto gridPolyData = vtkSmartPointer<vtkPolyData>::New();
    gridPolyData->SetPoints(pData1->GetPoints());

    auto probeFilter =  vtkSmartPointer<vtkProbeFilter>::New();
    //probeFilter->SetSourceData(pData1);
    probeFilter->SetInputData(gridPolyData);
    probeFilter->Update();
    pData1->DeepCopy(probeFilter->GetOutput());




    auto connectivityFilter =  vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
    connectivityFilter->SetInputData(pData1);
    connectivityFilter->SetExtractionModeToAllRegions();
    connectivityFilter->Update();

    pData1->DeepCopy(connectivityFilter->GetOutput());
*/

}


void HMO::buildMOIsosurface()
{
    if (isMolecularOrbital) {
        NPoints=80;
        minBox=-20.0;
        maxBox=20.0;
        isoValue=0.001;
        cout << "Build Molecular Obital " <<endl;

        for (uint i=0;i<NumAOs();i++)
            if (getAO(i)->getZ()>1) {
                NPoints=200;
                break;
            }
    }

    if (isHybridizedOrbital ) {
        NPoints=100;
        minBox=-8.0;
        maxBox=8.0;
        isoValue=0.001;
        cout << "Build Hybridized Obital " <<endl;
    }


    int Nx=NPoints,Ny=NPoints,Nz=NPoints;
    int N=NPoints;



    //includes (0,0,0), and do not jump over
    interval=(maxBox-minBox)/N;
    maxBox=interval*N/2.0;

    minBox=-maxBox;

    int x,y,z;
    double  x0=minBox,
            y0=minBox,
            z0=minBox;


    sendMessage("Building cube data ... ");
    sendMessage(QString("dim:     %1*%1*%1").arg(N));
    sendMessage(QString("range:   %1   %2").arg(minBox).arg(maxBox));
    sendMessage(QString("interval:  %1").arg(interval));
    sendMessage(QString("interval:  %1").arg(isoValue));


    cout << "\nbuilding cube data for hybridized/molecular orbital " <<endl;
    cout << "dim: " <<N <<endl;
    cout << "range: " <<minBox  <<" " << maxBox<< endl;
    cout << "interval: " <<interval  << endl;
    cout << "isovalue: " <<isoValue  << endl<< endl;


    if (CubeData || CubeData!=nullptr ) {
        delete CubeData;
        CubeData=nullptr;
    }
    CubeData = new double [Nx*Ny*Nz];

    //calculate Psi
    double psi=0.0,
           sum=0.0,
           psi2=0.0;


#pragma omp parallel for
    for( z=0; z < Nz ; z++)
        for( y=0; y < Ny;  y++)
            for( x=0; x < Nx;  x++){
                psi=calc_Psi(x0+interval*x,y0+interval*y , z0+interval*z);
                //psi2=psi*psi;
                //sum += psi2;

                //if  (psi>0.0)     CubeData[(z*N+y)*N+x]=psi2;
                //else              CubeData[(z*N+y)*N+x]=-psi2;


                CubeData[(z*N+y)*N+x]=psi;

                //cout << psi<<endl;
            }

    int dim=NPoints;
    interval=(maxBox-minBox)/dim;
    vtkImageData * volume0 = vtkImageData::New();
    volume0->SetDimensions(dim,dim,dim);
    volume0->SetSpacing(interval, interval,interval);
    volume0->AllocateScalars(VTK_DOUBLE,1);
    volume0->SetOrigin(-maxBox,-maxBox,-maxBox);
    volume0->Modified();

    vtkImageData * volume1 = vtkImageData::New();
    volume1->SetDimensions(dim,dim,dim);
    volume1->SetSpacing(interval, interval,interval);
    volume1->AllocateScalars(VTK_DOUBLE,1);
    volume1->SetOrigin(-maxBox,-maxBox,-maxBox);
    volume1->Modified();

    double v=0.0;
#pragma omp parallel for
    for(int z=0; z < dim;  z++)
        for(int y=0; y < dim;  y++)
            for(int x=0; x < dim;  x++)
            {
                double * d = static_cast<double*>(volume0->GetScalarPointer(x,y,z));
                double * d1 = static_cast<double*>(volume1->GetScalarPointer(x,y,z));

                d[0]=CubeData[(z*dim+y)*dim+x];
                d1[0]=-CubeData[(z*dim+y)*dim+x];
            }


    ExtractIsosurface(volume0, volume1);


    cout << "Done"<<endl;
    appendMessage("Done!");
}


void HMO::buildIsosurface()
{
     if (isMolecularOrbital || isHybridizedOrbital )
    {
         buildMOIsosurface();
         return;
    }


    buildCubeData();

    sendMessage("Building isosurface ...");

    cout <<"Building isosurface ..."<<endl;


    if (orbital_type==Widge){
        buildWidgeIsosurface();
        return;
    }


    int dim=NPoints;
    interval=(maxBox-minBox)/dim;
    vtkImageData * volume0 = vtkImageData::New();
    volume0->SetDimensions(dim,dim,dim);
    volume0->SetSpacing(interval, interval,interval);
    volume0->AllocateScalars(VTK_DOUBLE,1);
    volume0->SetOrigin(-maxBox,-maxBox,-maxBox);
    volume0->Modified();

    vtkImageData * volume1 = vtkImageData::New();
    volume1->SetDimensions(dim,dim,dim);
    volume1->SetSpacing(interval, interval,interval);
    volume1->AllocateScalars(VTK_DOUBLE,1);
    volume1->SetOrigin(-maxBox,-maxBox,-maxBox);
    volume1->Modified();


    double  x0=minBox,
            y0=minBox,
            z0=minBox;

    interval=(maxBox-minBox)/dim;


    double v=0.0;
#pragma omp parallel for
    for(int z=0; z < dim;  z++){
        for(int y=0; y < dim;  y++){

            for(int x=0; x < dim;  x++)
            {
                double * d = static_cast<double*>(volume0->GetScalarPointer(x,y,z));
                double * d1 = static_cast<double*>(volume1->GetScalarPointer(x,y,z));

                switch (orbital_type) {
                case  FULL :
                    d[0]=CubeData[(z*dim+y)*dim+x];
                    d1[0]=-CubeData[(z*dim+y)*dim+x];
                    break;

                case  XOY :
                    d[0]=CubeDataXOY[(z*dim+y)*dim+x];
                    d1[0]=-CubeDataXOY[(z*dim+y)*dim+x];
                    break;

                case  XOZ :
                    d[0]=CubeDataXOZ[(z*dim+y)*dim+x];
                    d1[0]=-CubeDataXOZ[(z*dim+y)*dim+x];
                    break;

                case  YOZ :
                    d[0]=CubeDataYOZ[(z*dim+y)*dim+x];
                    d1[0]=-CubeDataYOZ[(z*dim+y)*dim+x];
                    break;

                case  Widge :
                    d[0]=CubeDataWidge[(z*dim+y)*dim+x];
                    d1[0]=-CubeDataWidge[(z*dim+y)*dim+x];
                    break;

                case  Quarter :
                    d[0]=CubeDataQuarter[(z*dim+y)*dim+x];
                    d1[0]=-CubeDataQuarter[(z*dim+y)*dim+x];
                    break;

                case  Corner:
                    d[0]=CubeDataCorner[(z*dim+y)*dim+x];
                    d1[0]=-CubeDataCorner[(z*dim+y)*dim+x];
                    break;
                }

            }
        }
    }

     //obtained pData(positive),pData1(negative);
     ExtractIsosurface(volume0, volume1);

     switch (orbital_type) {
     case  XOY :
#pragma omp parallel for
         for (int i=0;i<pData->GetNumberOfPoints();i++)
         {
             double * p = pData->GetPoint(i);
             if (p[2] > 0.0)
             {
                 p[2]=0.0;
                 pData->GetPoints()->SetPoint(i,p);
             }

         }
         break;


     case  XOZ :
#pragma omp parallel for
         for (int i=0;i<pData->GetNumberOfPoints();i++)
         {
             double * p = pData->GetPoint(i);
             if (p[1] > 0.0)
             {
                 p[1]=0.0;
                 pData->GetPoints()->SetPoint(i,p);
             }
         }
         break;

     case  YOZ :
#pragma omp parallel for
         for (int i=0;i<pData->GetNumberOfPoints();i++)
         {
             double * p = pData->GetPoint(i);
             if (p[0] > 0.0) {
                 p[0]=0.0;
                 pData->GetPoints()->SetPoint(i,p);
             }
         }
         break;

         /*case  Widge :
             if (p[2] > 0.0 &&   p[1] > 0.0) {
                 p[2]=0.0;
                 p[1]=0.0;
                 pData->GetPoints()->SetPoint(i,p);
             }
             break;*/

     case  Quarter :
#pragma omp parallel for
         for (int i=0;i<pData->GetNumberOfPoints();i++)
         {
             double * p = pData->GetPoint(i);
             if (p[2] > 0.0) p[2]=0.0;
             if (p[1] > 0.0) p[1]=0.0;
             pData->GetPoints()->SetPoint(i,p);
         }
         break;

     case  Corner :
#pragma omp parallel for
         for (int i=0;i<pData->GetNumberOfPoints();i++)
         {
             double * p = pData->GetPoint(i);
             if (p[2] > 0.0) p[2]=0.0;
             if (p[1] > 0.0) p[1]=0.0;
             if (p[0] > 0.0) p[0]=0.0;
             pData->GetPoints()->SetPoint(i,p);
         }
         break;
     }


     switch (orbital_type) {
     case  XOY :
#pragma omp parallel for
         for (int i=0;i<pData1->GetNumberOfPoints();i++)
         {
             double * p = pData1->GetPoint(i);
             if (p[2] > 0.0)
             {
                 p[2]=0.0;
                 pData1->GetPoints()->SetPoint(i,p);
             }
         }
         break;


     case  XOZ :
#pragma omp parallel for
         for (int i=0;i<pData1->GetNumberOfPoints();i++)
         {
             double * p = pData1->GetPoint(i);
             if (p[1] > 0.0)
             {
                 p[1]=0.0;
                 pData1->GetPoints()->SetPoint(i,p);
             }
         }
         break;

     case  YOZ :
#pragma omp parallel for
         for (int i=0;i<pData1->GetNumberOfPoints();i++)
         {
             double * p = pData1->GetPoint(i);
             if (p[0] > 0.0) {
                 p[0]=0.0;
                 pData1->GetPoints()->SetPoint(i,p);
             }
         }
         break;

         /*case  Widge :
             if (p[2] > 0.0 &&   p[1] > 0.0) {
                 p[2]=0.0;
                 p[1]=0.0;
                 pData->GetPoints()->SetPoint(i,p);
             }
             break;*/

     case  Quarter :
#pragma omp parallel for
         for (int i=0;i<pData1->GetNumberOfPoints();i++)
         {
             double * p = pData1->GetPoint(i);
             if (p[2] > 0.0) p[2]=0.0;
             if (p[1] > 0.0) p[1]=0.0;
             pData1->GetPoints()->SetPoint(i,p);
         }
         break;

     case  Corner :
#pragma omp parallel for
         for (int i=0;i<pData1->GetNumberOfPoints();i++)
         {
             double * p = pData1->GetPoint(i);
             if (p[2] > 0.0) p[2]=0.0;
             if (p[1] > 0.0) p[1]=0.0;
             if (p[0] > 0.0) p[0]=0.0;
             pData1->GetPoints()->SetPoint(i,p);
         }
         break;
     }

     cout <<"Done!"<<endl;

     if ( orbital_type == FULL) return;

     auto skinNormals =  vtkSmartPointer<vtkPolyDataNormals>::New();
     skinNormals->SetInputData(pData);
     skinNormals->SetFeatureAngle(60.0);
     skinNormals->Update();
     pData->DeepCopy(skinNormals->GetOutput());

     auto skinNormals1 =  vtkSmartPointer<vtkPolyDataNormals>::New();
     skinNormals1->SetInputData(pData1);
     skinNormals1->SetFeatureAngle(60.0);
     skinNormals1->Update();
     pData1->DeepCopy(skinNormals1->GetOutput());
}


void HMO::setProjType(uint type)
{
    projType=proj_NONE;
    switch (type){
    case 1:
        projType=proj_XOY;
        break;
    case 2:
        projType=proj_XOZ;
        break;
    case 3:
        projType=proj_YOZ;
        break;
    }
}


void HMO::build_MOProjectionData()
{
    if ( (projType!=proj_XOY) && (projType!=proj_YOZ) && (projType!=proj_XOZ) )
        return;


    cout << "Building projection for molecular orbitals ..."<<endl;
    sendMessage("Building projection for molecular orbitals ...");

    //includes (0,0,0)
    proj_scale=2;
    uint N=proj_scale*NPoints;

    double interval0=(maxBox-minBox)/N;
    maxBox=interval0*N/2.0;
    minBox=-maxBox;

    double  x0=minBox,
            y0=minBox,
            z0=minBox;
    uint i,j,k;


    if (projType==proj_XOY)
    {
        sendMessage("Projection to XOY plane");
        cout <<" Projection to XOY plane"<<endl;

        if(dataXOY.size()>0)    dataXOY.clear();

        dataXOY.resize(N);
        #pragma omp parallel for
        for ( i=0;i<N;i++) dataXOY[i].resize(N);

        #pragma omp parallel for
        for( i=0; i < N;  i++)
        #pragma omp parallel for
            for( j=0; j < N;  j++) {
                dataXOY[i][j]=calc_Psi(x0+interval0*i,y0+interval0*j, 0.0);
            }

    }

    if (projType==proj_YOZ)
    {
        sendMessage("Projection to YOZ plane");
        cout <<" Projection to YOZ plane"<<endl;

        if(dataYOZ.size()>0)
            dataYOZ.clear();

        dataYOZ.resize(N);

        #pragma omp parallel for
        for (j=0;j<N;j++) dataYOZ[j].resize(N);

        #pragma omp parallel for
        for( j=0; j < N;  j++)

        #pragma omp parallel for
            for( k=0; k < N;  k++)
                dataYOZ[j][k]=calc_Psi(0.0,y0+interval0*j, z0+interval0*k);
        return;
    }

    if (projType==proj_XOZ)
    {
        sendMessage("Projection to XOZ plane");
        cout <<" Projection to XOZ plane"<<endl;

        if(dataXOZ.size()>0)
            dataXOZ.clear();

        dataXOZ.resize(N);
#pragma omp parallel for
        for ( i=0;i<N;i++) dataXOZ[i].resize(N);

#pragma omp parallel for
        for( i=0; i < N;  i++)
#pragma omp parallel for
            for( k=0; k < N;  k++)
                dataXOZ[i][k]=calc_Psi(x0+interval0*i, 0.0,z0+interval0*k);
        return;
    }


    cout <<"Done!"<<endl;
}







void HMO::build_AOProjectionData()
{
    sendMessage("Building projection for atomic orbitals ...");

    //includes (0,0,0)
    proj_scale=3;
    uint Nx=NPoints,
         Ny=NPoints,
         Nz=NPoints,
         N=proj_scale*NPoints;

   // cout << "------------"<<endl;
  //  cout << maxBox<<endl;
  //  cout << interval<<endl;
  //  cout<<N<<endl;


    double interval0=(maxBox-minBox)/N;
    maxBox=interval0*N/2.0;
    minBox=-maxBox;

    cout << "------------"<<endl;
    cout << maxBox<<endl;
    cout << interval0<<endl;


    double  x0=minBox,
            y0=minBox,
            z0=minBox;
    uint i,j,k;



    //only one type is generated based on AO'name
    HAO *ao = getAO(0);
    QString AOName=ao->getName();
    AOName.remove(0,1);

    cout <<" calculating AO projection data:"<<endl;
    //cout << AOName.toStdString().c_str()<<endl;


    projType=proj_NONE;

    if ( AOName.startsWith("S")
         || AOName.startsWith("Px") || AOName.startsWith("Py")
         || AOName.startsWith("Dxy") || AOName.startsWith("Dx2-y2")
         || AOName.startsWith("Fx3") || AOName.startsWith("Fy3")
         || AOName.startsWith("Fx(x2-3y2)") || AOName.startsWith("Fy(3x2-y2)")
         || AOName.startsWith("Gxy(x2-y2)") || AOName.startsWith("G(x4+y4)")
         )
    {
        sendMessage("Projection to XOY plane");

        projType=proj_XOY;

        cout <<" Projection to XOY plane"<<endl;

        if(dataXOY.size()>0)    dataXOY.clear();
        dataXOY.resize(N);
        #pragma omp parallel for
        for ( i=0;i<N;i++) dataXOY[i].resize(N);

        #pragma omp parallel for
        for( i=0; i < N;  i++)
        #pragma omp parallel for
            for( j=0; j < N;  j++)
                dataXOY[i][j]=calc_Psi(x0+interval0*i,y0+interval0*j, 0.0);

        cout <<" ... Done!"<<endl;
        return;
    }

    if ( AOName.startsWith("Dyz") ||AOName.startsWith("Fyz2")
         || AOName.startsWith("Gz3y") || AOName.startsWith("Gzy3")
        )
    {
        projType=proj_YOZ;
        sendMessage("Projection to YOZ plane");
        cout <<" Projection to YOZ plane"<<endl;

        if(dataYOZ.size()>0)    dataYOZ.clear();
        dataYOZ.resize(N);
        #pragma omp parallel for
        for ( i=0;i<N;i++) dataYOZ[i].resize(N);

        #pragma omp parallel for
        for( j=0; j < N;  j++)
        #pragma omp parallel for
            for( k=0; k < N;  k++)
                dataYOZ[j][k]=calc_Psi(0.0,y0+interval0*j, z0+interval0*k);

        cout <<" ... Done!"<<endl;
        return;
    }

    else if ( AOName.startsWith("Pz")
              || AOName.startsWith("Dz2")
              || AOName.startsWith("Dxz")
              || AOName.startsWith("Fz3") || AOName.startsWith("Fxz2")
              || AOName.startsWith("Gz4") || AOName.startsWith("Gz3x")
              || AOName.startsWith("Gzx3") || AOName.startsWith("Gz3x")
              )
    {
        sendMessage("Projection to XOZ plane");

        projType=proj_XOZ;
        cout <<" Projection to XOZ plane"<<endl;

        if(dataXOZ.size()>0)    dataXOZ.clear();
        dataXOZ.resize(N);
#pragma omp parallel for
        for ( i=0;i<N;i++) dataXOZ[i].resize(N);


#pragma omp parallel for
        for( i=0; i < N;  i++)
#pragma omp parallel for
            for( k=0; k < N;  k++)
                dataXOZ[i][k]=calc_Psi(x0+interval0*i, 0.0,z0+interval0*k);

        cout <<" ... Done!"<<endl;

        return;
    }
}




void HMO::calc_RadialThreeKindsData ()
{
    sendMessage("Calculating R,R2 and RDF functions ...");

    RData.clear();
    R2Data.clear();
    RDFData.clear();

   double interval=0.1;

   double r_max=1.2*AOs[0]->calc_least_r();
   double r=0.0;

   for (int i=0; ;i++)
   {
       if(r>r_max) break;

       r+=interval;

       double  R=0.0,
               R2=0.0,
               RDF=0.0;
       double value=0.0;

       for (int j=0;j<AOs.size();j++)
       {
           value=AOs[j]->Hydrogen_R(r);
           R+=value;
           R2+=value*value;
           RDF+=value*value*r*r;
       }
       RData.append(QPointF(i, R));
       R2Data.append(QPointF(i, R2));
       RDFData.append(QPointF(i, RDF));
   }

    appendMessage("Done!");
}



