#ifndef HMOL_H
#define HMOL_H

#include "math/vector3.h"
#include "math/matrix3x3.h"

#include <openbabel/mol.h>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/generic.h>

#include <QString>
#include <QVector>
#include <vector>
#include <string.h>
#include <QList>

#include <vector>
#include <utility>


#include "mTools.h"

using namespace std;

class MainWindow;
class View3D;
class templateMol;
class HGraph;

enum AtomHybridization
{
  PerceivedOctaheadral = -6,
  PerceivedTrigonalBipyramidal = -5,
  PerceivedSquarePlanar = -4,
  PerceivedSP3 = -3,
  PerceivedSP2 = -2,
  PerceivedSP = -1,
  UNknown = 0,
  SP = 1,
  SP2 = 2,
  SP3 = 3,
  SquarePlanar = 4,
  TrigonalBipyramidal = 5,
  Octahedral = 6
};

struct HRing
{
    unsigned int  size;
    vector <unsigned int> atomIdList;
    vector3 center;
    vector3 norm;
    vector3 color;
    double radius;
    bool   isAromatic;
    bool   isSelected;
    double scale;// for display radius;
    unsigned int   type; //for display
    double opacity;
};


struct HXAtom
{
    unsigned int   id;
    vector3 pos;
    vector <uint> atomIdList;
    vector <uint> neighbor;
};


struct HCoordinate
{
    QString symbol;
    vector3 p;
};


class HAtom
{
public:
    HAtom() {
        _symbol="X";
        _atomicNum=0;
        pos_cart=VZero;
        _color=0xFFFFFF;
        _mass=0.0;
        _radius=0.0;
        _isAromatic=false;
        _hybridization=UNknown;
    };

    HAtom(QString, vector3 );
    HAtom(QString, double, double, double );
    HAtom(uint, double, double, double );
    HAtom(unsigned int , vector3  );

   ~HAtom() {};

   void setPos(vector3 pos) {pos_cart=pos; }
   void setPos(double x, double y, double z)
   {
       vector3 p(x,y,z);
       pos_cart=p;
   }
   void setCartesian(double x, double y, double z)
   {
       setPos(x,y,z);
   }
   void setCartesian  (vector3 p) {setPos(p);}


  vector3 cartesian  () {return pos_cart;}
  vector3 Pos ()        {return pos_cart;}


   void modify2Symbol (QString s);
   void setSymbol ( QString s);
   void setSymbol();
   QString Symbol ();
   QString Symbol(unsigned int );

   void setAtomicNum (int n) {_atomicNum=n;}
   int  atomicNum () {return _atomicNum;}


   double x(){return pos_cart.x();}
   double y(){return pos_cart.y();}
   double z(){return pos_cart.z();}


   uint32_t Color () {return _color;}
   void setColor (uint32_t c ) {_color=c;}


   bool isHydrogen() const { return _atomicNum == 1; }



   double radius();
   void setRadius(double r) {_radius=r;}

   double Mass();
   void   setMass(double m) {_mass=m;}

   unsigned int index() {return idx;}
   void setIndex(unsigned int i) {idx=i;}
   void update();


   //! Assignment
   HAtom & operator= (const HAtom & other);
   HAtom *  copy (const HAtom * other);

   void setSelected (bool b) {_selected=b;};
   bool isSelected (){return _selected;}

   void  setNeihgborId(unsigned int i, unsigned int d) {LinkdedAtoms[i]=d;}
   void  removeLinkedAtomId(unsigned int idx) ;
   void  appendLinkdedAtomId(unsigned int idx) {LinkdedAtoms.push_back(idx);}
   unsigned int  getLinkdedAtomId(unsigned int idx) {return LinkdedAtoms[idx];}

   unsigned int  numLinkdedAtoms() {return LinkdedAtoms.size();}
   void  clearLinkedAtomId() {LinkdedAtoms.clear();}
   bool  isLinkded(unsigned int j){if (LinkdedAtoms.contains(j)) return true;return false;}


   void  appendNeighbor(unsigned int idx) {LinkdedAtoms.push_back(idx);}
   unsigned int  getNeighborId(unsigned int idx) {return LinkdedAtoms[idx];}
   unsigned int  numNeighbors() {return LinkdedAtoms.size();}
   bool  isNeighbor(unsigned int j){if (LinkdedAtoms.contains(j)) return true;return false;}
   void  removeNeighbor(unsigned int idx) ;
   void  clearNeighbors() {LinkdedAtoms.clear();}


   bool  isAromatic() { if(numNeighbors()<=3) return true; return false;}
   //bool  isAromatic() {return _isAromatic;}

   void  setAromatic(bool b=true) {_isAromatic=b;}

   void   setScale (double s) {_scale=s;}
   double getScale () {return _scale;}

   void  setHybridization (AtomHybridization a) {_hybridization=a;}
   AtomHybridization  getHybridization() {return _hybridization;}

   bool isHydrogen() {if (_symbol=="H") return true; if (_atomicNum==1) return true; return false;}


private:
    QList <unsigned int > LinkdedAtoms;
    QString _symbol;
    int _atomicNum;
    double _radius,_mass;
    vector3 pos_cart,pos_frac;
    uint32_t _color;
    unsigned int idx,_uniqueAtomIdx;
    bool _selected;
    bool _isAromatic;
    double _scale;
    AtomHybridization _hybridization;
};


class HBond
{
private:
     unsigned int  bondOrder; // 1 = single, 2 = double,  3 = triple, 6 = aromatic)
     unsigned int  atomId0, atomId1;
     vector3 begin, end;
     bool isAromaticity;

     vector3 _plane;
     vector <int> idRings;
     bool   _selected;
     bool   _isAromatic;

public:
     bool isInsideRing () {
         if(idRings.size()>0)
             return true;
         return false;
     }


     explicit HBond() {_selected=false;idRings.clear();}
     HBond(int i,int j, int order) {
         atomId0=i;
         atomId1=j;
         bondOrder=order;
         isAromaticity=false;
         _plane=VZero;
         _selected=false;
         _isAromatic=false;
         idRings.clear();
     };


     ~HBond(){}

     unsigned int getBondOrder () {return bondOrder;}
     void setBondOrder (unsigned int b) {bondOrder=b;}

     void setSelected (bool b) {_selected=b;}
     bool isSelected (){return _selected;}


     unsigned int atomIndex0()
     {
         return  atomId0;
     }

     unsigned int atomIndex1()
     {
         return  atomId1;
     }

     void setIndex(unsigned int id1,unsigned int id2)
     {
         atomId0=id1;
         atomId1=id2;
     }

     void setBeginIndex(unsigned int id )  { atomId0=id; }
     void setEndIndex(unsigned int id )    { atomId1=id; }

     //void setBegin(vector3 & b )   {begin=b; }
     //void setEnd(vector3 & e )     {end=e; }

     void setAromaticity(bool t)   {isAromaticity=t;}

     void setPlane (vector3 v) {_plane=v;}
     void setPlane (double x, double y, double z) {_plane.Set(x,y,z);}
     vector3 getPlane () {return _plane;}


     vector <int> getRingId () {return idRings; }
     unsigned int getRingId (uint i) {return idRings[i]; }

     void removeRingId(uint i) {
         if(idRings.size()>0)
             idRings.erase(remove(idRings.begin(), idRings.end(), i), idRings.end());
     }

     void setRingId(int i, int id) {
         idRings[i]=id;
     }

     void appendRingId(int id) {addRingId(id);}
     void addRingId(int id)
     {
         if(idRings.size()<1)
             idRings.push_back(id);
         else {
             vector<int>::iterator r = find( idRings.begin( ), idRings.end( ), id );
             if ( r == idRings.end( ) )
                 idRings.push_back(id);
         }
     }

     unsigned int sizeofRingId () {return idRings.size(); }

     bool  isAromatic() {return _isAromatic;}
     void  setAromatic(bool b=true) {_isAromatic=b;}

     void swapIndex() { //cout << "swap index "<<endl;
                        //cout << atomId0 << " "<< atomId1<<endl;
                        unsigned int ii=atomId1;
                        atomId1=atomId0;
                        atomId0=ii;
                        //cout << atomId0 << " "<< atomId1<<endl;
                      }
};










//molecule class
class HMol
{
public:
    HMol() ;
    HMol(MainWindow * m):m_parent(m) {
        isAtomvisible=true;
        isAtomLabelVisible=false;
        view3d=nullptr;
    };
    

    ~HMol(){clearAll();}

    MainWindow * m_parent;

    QVector <HAtom *> atomList;
    QVector <HBond *> bondList;
    QVector <HRing *> ringList;
    QVector <HRing *> arcList;
    QVector <HXAtom*> atomXList;


    void backupAtoms();
    void genRotationAtoms(vector3,double);
    void genReflectionAtoms(vector3);
    void genReverseAtoms();
    void genNormalAtoms();
    void genRotationReflectionAtoms(vector3,double);
    void genRotationReversionAtoms(vector3,double);

    void clearReflectionAtoms();
    void clearReverseAtoms();
    void clearNormalAtoms();



    QVector <HCoordinate * > atomList_bk;
    QVector <vector3> atomList_reflection;
    QVector <vector3> atomList_reversion;
    QVector <vector3> atomList_normal;
    QVector <vector3> atomList_rotation;


    double Distance  (unsigned int id1, unsigned int id2);
    double Angle     (unsigned int id1, unsigned int id2, unsigned int id3);
    double Dihedral  (unsigned int id1, unsigned int id2, unsigned int id3, unsigned int id4);


    HBond * getBondbyAtomIndex(unsigned int , unsigned int );
    unsigned int getBondOrderbyAtomIndex(unsigned int , unsigned int );
    unsigned int getBondOrderbyIndex(unsigned int id) {return getBondbyIndex(id)->getBondOrder();}

    HBond * getBondIndexbyAtomIndex(unsigned int , unsigned int , unsigned int & );
    HBond * getBondbyIndex(unsigned int id) {  return bondList[id]; }


    HBond * getLastBond() { if (NumBonds()>0) return bondList[NumBonds()-1]; return nullptr;}


    void filterOrder(vector <uint> & );
    void filterOrder_reverse(vector <uint> & );
    bool isMetal(unsigned int );
    void findChildren(vector<int> &,int ,int );

    void findFragment(unsigned int , vector<uint> & );
    void findLargestFragment(unsigned int, unsigned int,  vector<uint> & );

    void findLargestFragment(unsigned int, vector<uint> & );
    void findLargestFragment_1(unsigned int, unsigned int,
                               vector<uint> & ); //do not contains idx0 itself

    bool isAtomLabelVisible;

public:
    vector3 atomPos(unsigned int idx) {
        if (idx<NumAtoms() )
            return atomList[idx]->Pos();
        return VZero;
    }


    void setAtomColor(unsigned int idx, uint32_t v)
    {
        atomList[idx]->setColor(v);
    }

    void setAtomSymbol(unsigned int idx, QString s)
    {
        atomList[idx]->modify2Symbol(s);
    }


    void setAtomRadius (unsigned int idx, double r)
    {
        atomList[idx]->setRadius(r);
    }

    void setAtomPos (unsigned int idx, vector3 p)
    {
        if (idx >= NumAtoms()) return;
        atomList[idx]->setPos(p);
    }

    void setAtomPos (unsigned int idx, double x, double y, double z)
    {
        if (idx >= NumAtoms()) return;
        vector3 p(x,y,z);
        atomList[idx]->setPos(p);
    }


    double atomicMass (unsigned int idx)
    {
        if (idx<NumAtoms())
            return atomList[idx]->Mass();
        return 0.0;
    }

    QString atomSymbol (unsigned int idx) {
        if (idx<NumAtoms() )
            return atomList[idx]->Symbol();
        return "X";
    }

    unsigned int atomicNum (unsigned int idx) {
        if (idx<NumAtoms() )
            return atomList[idx]->atomicNum();
        return 0;
    }


public:
    void setParent (MainWindow * p) {m_parent=p;}

    void loadFile(QString);
    void load_xyz(QString );
    void load_cif(QString );

    void load_mol(QString );
    void load_other(QString );

    //void load_cml(QString );



    void load_GaussianCube(QString );
    void load_GaussianOutPut(QString );
    void load_GaussianFchk(QString );


    void load_CP2K_inp(QString );

    void perceiveBondOrder();
    void perceiveBondOrder_OpenBabel();



    void addRing (vector <uint> idList);


    void addRing (vector <uint> idList,
                  vector3 ringCenter, vector3 ringNormal,
                  double ringRadius,double ringScale,
                  vector3 color,   bool ringAromatic);

    void addArc (vector <uint> idList,
                  vector3 ringCenter, vector3 ringNormal,
                  double ringRadius, bool ringAromatic);


    void perceiveRings_A();
    void perceiveRings_OB();
    void perceiveRings();
    void displayRingList();

    void perceiveArcs();
    void displayArcList();


    unsigned int NumBonds() {return bondList.size();}
    unsigned int NumAtoms() {return atomList.size();}
    void displayBonds();
    void displayAtoms();



    QString getAtomSymbol (unsigned int id) {return atomList[id]->Symbol();}
    HAtom * getAtombyIndex(unsigned int id) {if (id < atomList.size ()) return atomList[id];
                                                                return nullptr;}
    HAtom * getLastAtom();
    HAtom * getAtom(uint id);

    double  getAtomXbyIndex(unsigned int id) {return atomList[id]->x();}
    double  getAtomYbyIndex(unsigned int id) {return atomList[id]->y();}
    double  getAtomZbyIndex(unsigned int id) {return atomList[id]->z();}
    double  getAtomRadiusbyIndex(unsigned int id) {return atomList[id]->radius();}

    vector3 getAtomPosbyIndex(unsigned int id) {return atomList[id]->Pos();}




    void centralize();
    void rotate( vector3  v1, vector3  v2 );
    void rotate( vector3  v, double angle );
    void rotate (matrix3x3 );
    void translate(vector3 p);
    void translate(double x, double y, double z);
    void flip (vector3  v);
    void flip_X();
    void flip_Y();
    void flip_Z();

    bool atomExisted(uint,double ,double ,double);
    bool atomExisted(uint, vector3);
    bool atomExisted(QString ,double ,double ,double);
    bool atomExisted(QString ,vector3);


    void addAtom(string , vector3 p);
    void addAtom(QString ,double ,double ,double );
    void addAtom(QString , vector3);
    void addAtom(string ,double ,double ,double);
    void addAtom(uint, double ,double ,double);
    void addAtom(unsigned int ,vector3 p);

    bool addAtom(HAtom * other);

    void appendAtom(QString ,double ,double ,double);
    void appendAtom(unsigned int atomicNum,double ,double ,double);

    void insertAtom(unsigned int idx, QString s,double x,double y ,double z);
    void insertAtom(unsigned int idx, unsigned int atomicNum, double x,double y ,double z);


    void addBond(unsigned int , unsigned int );
    void addBond(unsigned int , unsigned int , unsigned int );



    QString getCoordinates();

    bool isAtomVisible() {return isAtomvisible;};

    void recoverAtomCoordinateFromBackup();
    void removeBackupAtoms();

    void removeAllXAtoms();
    void removeAllAtoms();
    void removeAllBonds();
    void removeAllRings();
    void removeAllArcs();
    void clearAll() ;

    bool hasSelectedAtoms();

    void removeSingleAtombyId(uint);
    void removeAtombyId(unsigned int ); //remove related terminal hydrogen
    void removeBondbyId(unsigned int );

    void removeAtombyId(vector <uint> );
    void removeLonelyHydrogen();
    void removeTerminalHydrogen(unsigned int );
    bool removeOneTerminalHydrogen(unsigned int );


    void removeBondbyAtomId(unsigned int );
    void removeBondbyAtomIds(uint,unsigned int );

    void buildOBMol();
    bool fromOBMol();


    void setFileName (QString s) {fileName=s;}
    QString getFileName () {return fileName;}


    vector3 Center() {return center;}
    void setCenter(vector3 c) {center=c;}


    void perceiveBonds ();
    void perceiveBonds(int idxofAtom);

    void setPointGroup(QString s) {PointGroup=s;}

    void addHydrogen (unsigned int idx=0);    //add one H each time
    void autoAddHydrogen(unsigned int ); //for single atom
    void autoAddHydrogen();  //for whole molecule
    void autoAdjustHydrogen(unsigned int ); //for single atom
    void autoAdjustHydrogen( ); //for whole atom
    void autoAdjustHydrogenWhole(); //for whole molecule

    void runMolecularMechanics(QString method="MMFF94");
    void cleanMol();
    void runXTB();
    void thermalDisplacement(double);


    bool huckelCalcuation();

    void reBuildHuckelMO(double);
    double buildHuckelMO(unsigned int );
    void collectHuckelAtoms();
    void collectHuckelSphericalAtoms();


    void calcMolVolume();

    void perceiveHydrogenBonds();
    void perceiveHydrogenBondId(unsigned int id);

    void perceivePlaneRingforBonds();
    void perceivePlaneBondbyId(uint);
    void perceivePlaneBonds();
    void setView3D (View3D * v) ;

    void linkTemplate2Mol(templateMol *, unsigned int , uint);
    void linkTemplate2Mol_1(templateMol *, unsigned int , uint);
    void addAtomfromTemplate(templateMol * , vector3 );
    void updateAllAtomLinker();
    void updateAtomLinkerId(unsigned int idx);

    unsigned int numRings() {return ringList.size();}
    HRing * getRingbyId(unsigned int id) {return ringList[id];}

    bool canbePaste();
    void pastefromClipboard();




    //-----------------------------------------------------------------------------------
    void setBondLengthBetweenTwoFragments(unsigned int idx1, unsigned int idx2, double length,
                                     vector <uint> & fragment1,  vector <uint> & fragment2,
                                     bool , bool );
    void setBondLengthBetweenTwoAtoms(unsigned int idx1, unsigned int idx2, double length,
                                     bool , bool );

    void setBondAngleAtoms (unsigned int idx0, unsigned int idx1, unsigned int idx2, double bondAngle,
                                                       bool, bool );
    void setBondAngleFragments (unsigned int idx0, unsigned int idx1, unsigned int  idx2, double bondAngle,
                               vector <uint> & , vector <uint> & ,
                               bool , bool);


    void setTorsionAtoms(unsigned int idx0, unsigned int idx1, unsigned int idx2, unsigned int idx3,
                         double , bool  , bool );

    void setTorsionFragments(unsigned int idx0, unsigned int idx1, unsigned int idx2, unsigned int idx3, double ,
                            vector <uint> & ,  vector <uint> & ,
                            bool  , bool );


    QString parseGaussin09Summary(QString );
    void loadxyzfromQString (QString );
    void addatomfromQString(QString data);
    void setCharge (float c) {charge=c;}
    float getCharge() {return charge;}

    void parse_xbt_mol(QString );

    double  getAtomScalebyIndex(unsigned int id) {return atomList[id]->getScale();}

    bool isAtomInsideRing(unsigned int , unsigned int  );
    void removeSingleRingbyId(uint);
    void removeRingbyId(uint);
    unsigned int getNeighborinsideRing(unsigned int , unsigned int );
    void updateAtomInsideRingOrNot(unsigned int );

    AtomHybridization perceiveHybridization(unsigned int );
    void perceiveAllAtomHybridization();

    void replaceSelelctedAtomWith(unsigned int ,vector <uint> );

    void removeX();
    void reCalculateBonds();

    void  setRingAromatic(uint, bool);

    bool isAtomExisted(QString );
    bool isHydrogenAtomExisted();
    bool isHydrogenAtom(unsigned int );
    bool isXExisted();
    void moveAtomFromTo(unsigned int , uint);

    bool checkClipboard();
    void reverseBondOrient(unsigned int,unsigned int);

public:
    void perceptMoleculeShape();
    void perceiveMolLength();
    void perceiveMolLength(double &  , double & );
    void perceiveMolLength(QString, double &  , double & );

    double getHeight() {return height;}
    double getRadius() {return radius;}
    double radius,height;

    QString molShape;
    bool writeMol(const QString &, const QString &);

    void tranform(vector3, matrix3x3 );

    //paste and avoiding overlap
    QString getShift(QString data);
    vector3   shiftVector;


    //for movie
    void rotateReflect_movie(vector3 v, double angle, int interval);

    void reflect_movie(vector3 v,int interval,int idx);
    void reverse_movie(int interval,int idx);

    void rotate_movie(vector3  v, double angle);

    void copyAtom(uint, QString,  vector3,   double,  double,  uint32_t);
    void copyBond(uint, uint, uint order, vector<int> ringId, vector3 plane);
    void copyRing(vector <uint>, vector3, vector3, double, uint, vector3, double, bool);
	void filterRings();



private:
    OpenBabel::OBMol  obmol;
    vector <uint> notXIdx;

    float charge;
    HGraph * m_graph;
    View3D * view3d;
    QString fileName;
    QString PointGroup;

    unsigned int idxActiveMO;

    unsigned int idx;
    bool isVisible;

    vector3 center;


    QString formula;
    bool isAtomvisible;


    double molLength;
    vector3 xyzLength;

    matrix3x3 IMomentMatrix;
};


#endif // HMOL_H
