#ifndef MolSymmetry_H
#define MolSymmetry_H

#include <string>
#include <vector>
#include <math.h>
#include "matrix3x3.h"
#include "vector3.h"

using namespace std;

class HMol;


enum MoleculeShape {
    Polygonn,
    Plane,      //Ia + Ib = Ic
    Line,       //Ia = 0 Ib = Ic
    Sphere,     //Ia = Ib = Ic
    Oblate,     //Ia = Ib < Ic
    Prolate,    //Ia < Ib = Ic
    Irregular   //Ia < Ib < Ic
};

//point-group symmetry
class PGSymmetry
{
public:
    PGSymmetry(HMol *, double );
    HMol * mol;

public:

    unsigned int  NAtoms; //number of atoms
    vector < vector <double> > DistsMatrix;
    vector < vector <unsigned int > >   SubGroups;

    MoleculeShape shape;

public:
    bool    hasI,  isCoov, isDooh;
    bool    isDnd, isDnh, isDn, isS2n;
    bool    isCnv, isCn, isCnh;
    bool    isTd, isT, isTh;
    bool    isIh, isI;
    bool    isO,  isOh;
    bool    isSn, isCi, isCs;

public:
    unsigned int    principalAxisOrder,order;
    double  angle;
    string  PGSymbol;
    string  first,last,middle;

    double tol;

public:
    matrix3x3 IMomentMatrix;
    vector3 IMoment,MassCenter;


    //coordinates and masses of atoms
    vector < vector3 >  AtomCoordinates;
    vector <double>     AtomMasses;

    //type of atom: e.g. 0=X, 1 = H, 2=He ...
    vector  < unsigned int >    AtomTypes;
    vector  < string >  AtomSymbols;

public:
    vector3 PrincipalAxis;
    vector3 Ci;
    vector < vector3 > Cn;
    vector < vector3 > C2;
    vector < vector3 > Horizontal_C2;
    vector < vector3 > C3;
    vector < vector3 > C4;
    vector < vector3 > C5;
    vector < vector3 > C6;

    vector < vector3 > S2n;

    vector < vector3 > S12;
    vector < vector3 > S10;
    vector < vector3 > S8;
    vector < vector3 > S6;
    vector < vector3 > S4;


    vector < vector3 > SigmaD;
    vector < vector3 > SigmaV;
    vector < vector3 > SigmaH;

    void generalPerceptSymmetry();
    void detectPrincipalAxisOrder();


    void perceptMolShape();
    void GetData();
    void updateMolData();
    void refreshMol ();
    void buildDistanceMatrix();
    void buildSubGroups_perception();    //only for percept Symmetry
    void buildSubGroups_perception_cluster();    //only for percept Symmetry
    bool buildSubGroupsOnSymmetry_refine(); //only for patch
    bool buildSubGroupsOnSymmetry_patch();  //only for refine

    void CheckCenter();
    bool CheckMatrix( matrix3x3 & );

    bool CheckC2(vector3  );
    bool CheckCn(vector3, unsigned int );
    bool CheckS2n(vector3, unsigned int );
    bool CheckSigma(vector3);

    void SearchMirror();
    void SearchC2();
    void SearchC3();
    void SearchC4();
    void SearchC5();
    void SearchCn(unsigned int o,vector < vector3 > & );

    bool findFirstHorizontalC2(vector3 & );
    bool perceptHorizontal_C2(bool);
    bool perceptMirrorH();

    void perceptVerticalMirror(vector < vector3 > & ); //search Mirror based on parallel to Cn
    void perceptVerticalMirror(bool); //search Mirror based on perceptHorizontal C2

    void perceptSphere();
    void perceptOblate();
    void perceptProlate();

    void perceptThMirror(bool isCheck);
    void perceptTdMirror(bool isCheck);
    void perceptOhMirror(bool isCheck);
    void perceptIhMirror(bool isCheck);

    void appendVector3 (vector < vector3 > & , vector3 & );
    void refineSymmetryElements();
    void report(string & );
    void summary();

public:
    //build point group operation
    vector<matrix3x3 > PGOperation;
    vector<matrix3x3 > InvPGOperation;

    void buildInvOperation();
    void buildOperation();

    void buildOperation_Cs();
    void buildOperation_Ci();
    void buildOperation_Cn(unsigned int );
    void buildOperation_S2n(unsigned int );
    void buildOperation_Cnv(unsigned int );
    void buildOperation_Cnh(unsigned int );

    void buildOperation_Dn(unsigned int );
    void buildOperation_Dnd(unsigned int );
    void buildOperation_Dnh(unsigned int );

    void buildOperation_T();
    void buildOperation_Th();
    void buildOperation_Td();

    void buildOperation_O();
    void buildOperation_Oh();

    void buildOperation_I();
    void buildOperation_Ih();

    void buildOperation_D00h();
    void buildOperation_C00v();


    // Patch molecule
    unsigned int addingAtoms;
    void patchMolecule(unsigned int & );
    void patchC00v();
    void patchD00h();


    //regroup the atoms according to the point-group operation;
    //vector < vector < unsigned int > > AtomGroupList;

    //refine the cartesian data
    bool refine();  //based on symmetry elements
    void ClearAll();

    matrix3x3 getstandOrientMatrix() {return standOrientMatrix;}


public:
    void PerceptSymmetry(bool);
    void StandardOrientation();
    bool isOrientated;

    matrix3x3  rotMatrix,rotMatrix1,rotMatrix2;

    //rotate one vector to Z, and the second to X axis;
    void Orientation2ZX(vector3, vector3);

    //rotate one vector to Z, and the second to Y axis;
    void Orientation2ZY(vector3, vector3);

    void Orientation_Ci();
    void Orientation_Cs();
    void Orientation_Cn();
    void Orientation_S2n();
    void Orientation_Cnv();
    void Orientation_Cnh();
    void Orientation_Dn();
    void Orientation_Dnd();
    void Orientation_Dnh();
    void Orientation_Line();

    void Orientation_T();
    void Orientation_O();
    void Orientation_I();

    matrix3x3 OrientationFromTo(vector3,vector3);


//  tools
    bool isEquivalent (vector3, vector3 );
    bool isOrthogonal (vector3, vector3 );
    bool isRegularPolygon( vector < vector3 >); //using atoms' coordinates
    bool isRegularPolygon( vector < unsigned int > ); // using atoms' index

    void Translate(vector3 & );

    void Centralize();
    void CalcInertialMoment();

    bool isInside (vector < vector3> & , vector3 & );
    void CalcGCD (vector <unsigned int > & , unsigned int );


    void refinePrincipalAxis ();
    void setTolerance (double );
    void setSymmetry (string );
    unsigned int detectAxisOrder(vector3 );


    void generate_O_from_2C3(vector3, vector3);
    void generate_O_from_2C4(vector3, vector3);
    void generate_O_from_C4C2(vector3, vector3);

    void generate_I_from_2C2_orth(vector3, vector3);
    void generate_I_from_2C5(vector3, vector3); //neighbored C5

    void printInertialMoment();
    void ParseSymmetrySybmol (string );


    bool CombinationCn(vector < unsigned int > & arr, unsigned int data[], unsigned int start, unsigned int end,
                     unsigned int index, unsigned int r,
                     vector < vector3 > & container,
                     unsigned int order0);

    matrix3x3 standOrientMatrix;


};



#endif // MolSymmetry_H
