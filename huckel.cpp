#include <iostream>
#include <fstream>
#include <iomanip>
#include <BaseMathOperators.h>
#include <MathOperators.h>
#include <Complex.h>
#include <Matrix.h>

using namespace std;
using namespace hmma;
int main()
{
    cout<<"Please input file name:"<<endl;
    string filename;
    cin>>filename;

    ifstream myfile(filename.c_str());
    if(!myfile){cout<<"Unable to open file!"; exit(1);}
    int n;
    myfile>>n;
    cout<<"A "<<n<<" x "<<n<<" determinant."<<endl;
    DDMatrix Huckel(n,n,0.0);

    for(int row=0;row<n;row++)
    {
            for(int col=0;col<n;col++)
                    {
                             char element;
                                  myfile>> element;
                                       cout<<element<<" ";
                                         int Helement;
                                             if(element=='1') Helement=1;
                                                 else if (element=='0') Helement=0;
                                                     else if (element=='X'||element=='x') Helement=0;
                                                         else {cout<<" You have a wrong input!"<<endl; exit(1);}
                                                             if((col==row)&&(element!='X')&&(element!='x')){cout<<" You have a wrong input!"<<endl; exit(1);}
                                                                   Huckel(row,col)=Helement;
                                                                    }
                                                                        cout<<endl;
    }
    /*for(int row=0;row<n;row++)
        for(int col=0;col<n;col++)
                { if (Huckel(row,col)!=Huckel(col,row))
                        { cout<<"The input determinant is not Symmetric!!!";
                                cout<<endl;
                                        exit(1);
                                                }
                                                    } 
                                                    */
                                                    if(!Huckel.is_symmetric ()) 
                                                    { 
                                                            cout<<"The input determinant is not Symmetric!!!";
                                                                cout<<endl;
                                                                    exit(1);
                                                    }
                                                    cout<<endl;
                                                    DDMatrix eigenvals;
                                                    DDMatrix eigenvecs;
                                                    Huckel.eigen_space (eigenvals, eigenvecs, true);
                                                    for(int i=0;i<n;i++)
                                                    {
                                                            cout<< std::fixed<< std::setprecision(4);

                                                                double lambda=eigenvals(0,i);
                                                                    cout<<std::noshowpos<<"E"<<n-i<<"=alpha"<<std::showpos<<lambda<<"beta"<<endl;
                                                                        cout<<"Psi"<<std::noshowpos<<n-i<<"=";
                                                                            for(int j=0;j<n;j++)
                                                                                    {   if(fabs(eigenvecs(j,i))<1.0e-6) continue;
                                                                                            cout<<std::showpos<<eigenvecs(j,i)<<"psi"<<std::noshowpos<<j+1;
                                                                                                }
                                                                                                    cout<<endl<<endl;
                                                    }
}
