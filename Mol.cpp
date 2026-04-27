#include "Mol.h"
#include "templateMol.h"
#include <QFile>
#include <QMessageBox>
#include <QStringList>

#include <QString>
#include <QIODevice>

#include <vector>
#include <QProcess>


#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/chains.h>
#include <openbabel/generic.h>
#include <openbabel/optransform.h>
#include <openbabel/plugin.h>

#include <openbabel/obutil.h>
#include <openbabel/base.h>

#include <openbabel/babelconfig.h>
#include <openbabel/data.h>
#include <openbabel/generic.h>
#include <openbabel/forcefield.h>
#include <openbabel/mol.h>
#include <openbabel/typer.h>


#include <fstream>
#include <cstdlib>



#include <openbabel/math/transform3d.h>



#include "mainwindow.h"

#include <cstdlib>
#include <cstdio>
#include <openbabel/ring.h>
#include <QApplication>
#include <QClipboard>


#include "math/matrix3x3.h"
#include "mTools.h"

#include <iostream>
#include <fstream>
#include <iomanip>
//#include "DDMatrix/BaseMathOperators.h"
//#include "DDMatrix/MathOperators.h"
//#include "DDMatrix/Complex.h"
//#include "DDMatrix/Matrix.h"
//#include "ringperceiver.h"
#include  <algorithm>



using OpenBabel::OBConversion;
using OpenBabel::OBFormat;
using OpenBabel::OBMol;
using OpenBabel::OBAtom;
using OpenBabel::OBAtomIterator;
using OpenBabel::CSSRFormat;


using std::ifstream;
using std::ofstream;

//using namespace hmma;





void HAtom::setSymbol ( QString s)
{
    _symbol=s;
}

void HAtom::setSymbol()
{
    if (_atomicNum>0)
        _symbol=QString::fromStdString(ElementNames[idx]);
}


void HAtom::modify2Symbol(QString s)
{
    _symbol=s;

    for (int i=0;i<119;i++) {
        if(QString::fromStdString(ElementNames[i])==_symbol)
        {
            _atomicNum=i;
            break;
        }
    }
    _mass=elementMasses[_atomicNum];
    _radius=CovalentRadii[_atomicNum];
    _color=ElementColors[_atomicNum];
    _selected=false;
}



QString HAtom::Symbol()
{
    return  _symbol;
}

QString HAtom::Symbol(unsigned int  idx)
{
    if (_symbol=="X" && _atomicNum>0)
        _symbol=QString::fromStdString(ElementNames[idx]);
    return  _symbol;
}



//assignment
HAtom & HAtom::operator= (const HAtom & other)
{
    _symbol=other._symbol;
    _atomicNum=other._atomicNum;
    _radius=other._radius;
    pos_cart=other.pos_cart;
    pos_frac=other.pos_frac;
    idx=other.idx;
    _color=other._color;
    _mass=other._mass;
    _scale=other._scale;

    return *this;
}




HAtom::HAtom(unsigned int __atomicNum, double x, double y, double z)
{
    _atomicNum=__atomicNum;
    _symbol=QString::fromStdString(ElementNames[_atomicNum]);
    pos_cart.Set(x,y,z);

    _mass=elementMasses[_atomicNum];
    _radius=CovalentRadii[_atomicNum];
    _color=ElementColors[_atomicNum];
    _selected=false;

    _scale=0.3;
}

HAtom::HAtom(unsigned int __atomicNum, vector3 p )
{
    _atomicNum=__atomicNum;
    _symbol=QString::fromStdString(ElementNames[_atomicNum]);
    pos_cart=p;

    _mass=elementMasses[_atomicNum];
    _radius=CovalentRadii[_atomicNum];
    _color=ElementColors[_atomicNum];
    _selected=false;
    _scale=0.3;
}

HAtom::HAtom(QString s, double x, double y, double z)
{
    _selected=false;
    _symbol=s;
    pos_cart.Set(x,y,z);

    for (int i=0;i<119;i++) {
        if(QString::fromStdString(ElementNames[i])==_symbol)
        {
            _atomicNum=i;
            break;
        }
    }


    _mass=elementMasses[_atomicNum];
    _radius=CovalentRadii[_atomicNum];
    _color=ElementColors[_atomicNum];
    _scale=0.3;

    // cout << atomicNum << " " <<radius <<endl;
}

HAtom::HAtom(QString s, vector3 p)
{
    _symbol=s;
    pos_cart=p;

    for (int i=0;i<119;i++) {
        if(QString::fromStdString(ElementNames[i])==_symbol) {_atomicNum=i; break; }
    }

    _mass=elementMasses[_atomicNum];
    _radius=CovalentRadii[_atomicNum];
    _color=ElementColors[_atomicNum];
    _selected=false;
    _scale=0.3;
}


void HAtom::update()
{
    //update atomicNum
    for (int i=0;i<119;i++) {
        if(QString::fromStdString(ElementNames[i])==_symbol) {_atomicNum=i; break; }
    }

    _mass=elementMasses[_atomicNum];
    _radius=CovalentRadii[_atomicNum];
    _color=ElementColors[_atomicNum];
}


double HAtom::Mass()
{
    if(_atomicNum<1) update();
    return _mass;
}

double HAtom::radius()
{
    if(_radius<0.1) update();
    return _radius;
}

void HAtom::removeLinkedAtomId(unsigned int id)
{
    unsigned int i;
    for ( i=0;i<LinkdedAtoms.size();i++)
    {
        if(LinkdedAtoms[i]==id)
        {
            LinkdedAtoms.removeAt(i);
            break;
        }
    }
}

void HAtom::removeNeighbor(unsigned int id)
{
    unsigned int i;
    for ( i=0;i<LinkdedAtoms.size();i++)
    {
        if(LinkdedAtoms[i]==id)
        {
            LinkdedAtoms.removeAt(i);
            break;
        }
    }
}




HMol::HMol()
{
    isAtomvisible=true;
    isAtomLabelVisible=false;

    xyzLength.Set(0.0,0.0,0.0);

    m_graph=nullptr;
}


void HMol::centralize()
{
    vector3 MassCenter=VZero;

    /*
    double totalMass=0.0;
    double mass=0.0;

    for (unsigned int i=0;i<NumAtoms();i++) {
        mass=atomicMass(i);
        MassCenter += atomPos(i)*mass;
        totalMass += mass;
    }

    MassCenter /= totalMass;

    cout << MassCenter<<endl;

    for (unsigned int j=0;j<NumAtoms();j++){
        setAtomPos(j,atomPos(j)-MassCenter);
    }
*/

    for (unsigned i=0;i<NumAtoms();i++)
        MassCenter += atomPos(i);

    MassCenter /= NumAtoms();

    for (unsigned j=0;j<NumAtoms();j++)
        setAtomPos(j,atomPos(j)-MassCenter);

    calcMolVolume();


    vector3 a;
    //update ring information
    for (unsigned i=0;i<numRings();i++)
    {
        a=getRingbyId(i)->center-MassCenter;
        getRingbyId(i)->center=a;
    }
    //perceivePlaneRingforBonds();
}



QString HMol::getCoordinates()
{
    QString coord="";

    if (NumAtoms()<1)  return coord;

    char ss[256];

    for (int i=0;i<atomList.size();i++)
    {
        std::sprintf (ss,"%s %10.6f  %10.6f  %10.6f\n",(atomList[i]->Symbol().toStdString()).c_str(),
                      atomList[i]->x(),atomList[i]->y(),atomList[i]->z());
        coord+=ss;
    }
}


bool HMol::hasSelectedAtoms()
{
    if(view3d==nullptr)
        view3d=m_parent->getView3D();

    if (view3d && view3d->numSelectedAtoms()>0)
        return true;
    return false;
}

//translate and rotation
void HMol::tranform(vector3 v, matrix3x3 m)
{
    vector3  a,b;

    if(!hasSelectedAtoms())
    {
        for (unsigned int i=0;i<NumAtoms();i++)
        {
            a=atomPos(i)-v;
            setAtomPos(i,m*a+v);
        }

        //perceivePlaneRingforBonds();
        //update bond plane information
        for (unsigned int i=0;i<NumBonds();i++)
        {
            if(getBondbyIndex(i)->getBondOrder()<2) continue;
            if(getBondbyIndex(i)->getBondOrder()>6) continue;

            if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

            a=getBondbyIndex(i)->getPlane();

            getBondbyIndex(i)->setPlane(m*a);
        }

        //update ring information
        for (unsigned int i=0;i<numRings();i++)
        {
            a=getRingbyId(i)->norm;
            getRingbyId(i)->norm=m*a;

            a=getRingbyId(i)->center;
            getRingbyId(i)->center=m*a;
        }
        return;
    }

    //selected atoms existed
    if(hasSelectedAtoms())
    {
        for (unsigned int i=0;i<NumAtoms();i++)
        {
            if(!getAtombyIndex(i)->isSelected())
                continue;
            a=atomPos(i)-v;
            setAtomPos(i,m*a+v);
            updateAtomInsideRingOrNot(i);
        }
    }
}

void HMol::rotate(matrix3x3 m)
{
    vector3  a,b;
    if(hasSelectedAtoms())
        cout << " hasSelectedAtoms" << " cannot rotate"<<endl;

    if(!hasSelectedAtoms())
    {
        for (unsigned int i=0;i<NumAtoms();i++)
        {
            setAtomPos(i,m*atomPos(i));
        }

        //perceivePlaneRingforBonds();
        //update bond plane information
        for (unsigned int i=0;i<NumBonds();i++)
        {
            if(getBondbyIndex(i)->getBondOrder()<2) continue;
            if(getBondbyIndex(i)->getBondOrder()>6) continue;

            if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

            a=getBondbyIndex(i)->getPlane();

            getBondbyIndex(i)->setPlane(m*a);
        }

        //update ring information
        for (unsigned int i=0;i<numRings();i++)
        {
            a=getRingbyId(i)->norm;
            getRingbyId(i)->norm=m*a;

            a=getRingbyId(i)->center;
            getRingbyId(i)->center=m*a;
        }
        return;
    }

    //selected atoms existed
    if(hasSelectedAtoms())
    {
        for (unsigned int i=0;i<NumAtoms();i++)
        {
            if(!getAtombyIndex(i)->isSelected())
                continue;

            a=atomPos(i);
            setAtomPos(i,m*a);
            updateAtomInsideRingOrNot(i);
        }
    }
}

void HMol::rotate(vector3  v, double angle )
{
    double  ele[3][3];

    double ang = angle*3.14159265359/180.0;

    double c = cos(ang), t=1-c,  s = sin(ang);


    vector3 vec = v;
    v.normalize();

    double  x = vec[0],
            y = vec[1],
            z = vec[2];

    ele[0][0] = x*x*t+c ;       ele[0][1] = x*y*t-z*s;       ele[0][2] = x*z*t+y*s;
    ele[1][0] = x*y*t+z*s;      ele[1][1] = y*y*t+c;         ele[1][2] = y*z*t-x*s;
    ele[2][0] = x*z*t-y*s;      ele[2][1] = y*z*t+x*s;       ele[2][2] = z*z*t+c;

    vector3  a,b;


    if(!hasSelectedAtoms())
    {
        for (unsigned int i=0;i<NumAtoms();i++)
        {
            a=atomPos(i);

            b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
            b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
            b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2]);

            setAtomPos(i,b);
        }

        //perceivePlaneRingforBonds();
        //update bond plane information
        for (unsigned int i=0;i<NumBonds();i++)
        {
            if(getBondbyIndex(i)->getBondOrder()<2) continue;
            if(getBondbyIndex(i)->getBondOrder()>6) continue;

            if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

            a=getBondbyIndex(i)->getPlane();
            b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
            b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
            b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2] );

            getBondbyIndex(i)->setPlane(b);
        }

        //update ring information
        for (unsigned int i=0;i<numRings();i++)
        {
            a=getRingbyId(i)->norm;
            b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
            b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
            b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2] );
            getRingbyId(i)->norm=b;

            a=getRingbyId(i)->center;
            b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
            b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
            b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2] );
            getRingbyId(i)->center=b;
        }
        return;
    }

    //selected atoms existed
    if(hasSelectedAtoms())
    {
        for (unsigned int i=0;i<NumAtoms();i++)
        {
            if(!getAtombyIndex(i)->isSelected())
                continue;

            a=atomPos(i);
            b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
            b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
            b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2] );

            setAtomPos(i,b);
            updateAtomInsideRingOrNot(i);
        }
    }
}



void HMol::rotate(vector3  v1, vector3  v2)
{   
    matrix3x3 m(1.0);

    if (isEqual(IMomentMatrix.GetColumn(2),VZ)) {
        return;
    }
    else if (isNegativeEqual(IMomentMatrix.GetColumn(2),VZ)) {
        m.Set(0,0,-1.0);
        m.Set(1,1,-1.0);
        m.Set(1,1,-1.0);
    }
    else {
        m.SetupRotateMatrix(v1,v2);
    }

    vector3 a,b;
    if(!hasSelectedAtoms())
    {
        for (unsigned int i=0;i<NumAtoms();i++)
            setAtomPos(i,m*atomPos(i));

        //update bond plane information
        for (unsigned int i=0;i<NumBonds();i++)
        {
            if(getBondbyIndex(i)->getBondOrder()<2) continue;
            if(getBondbyIndex(i)->getBondOrder()>6) continue;

            if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

            a=getBondbyIndex(i)->getPlane();
            getBondbyIndex(i)->setPlane(m*a);
        }

        //update ring information
        for (unsigned int i=0;i<numRings();i++)
        {
            a=getRingbyId(i)->norm;
            getRingbyId(i)->norm=m*a;

            a=getRingbyId(i)->center;
            getRingbyId(i)->center=m*a;
        }
        return;
    }


    //selected atoms existed
    if(hasSelectedAtoms())
    {
        for (unsigned int i=0;i<NumAtoms();i++)
        {
            if(!getAtombyIndex(i)->isSelected())
                continue;

            a=atomPos(i);
            setAtomPos(i,m*a);
            updateAtomInsideRingOrNot(i);
        }
    }
}


void HMol::rotate_movie(vector3  v, double angle )
{
    double  ele[3][3];

    double ang = angle*3.14159265359/180.0;

    double c = cos(ang), t=1-c,  s = sin(ang);

    vector3 vec = v;
    v.normalize();

    double   x = vec[0],  y = vec[1], z = vec[2];

    ele[0][0] = x*x*t+c ;       ele[0][1] = x*y*t-z*s;       ele[0][2] = x*z*t+y*s;
    ele[1][0] = x*y*t+z*s;      ele[1][1] = y*y*t+c;         ele[1][2] = y*z*t-x*s;
    ele[2][0] = x*z*t-y*s;      ele[2][1] = y*z*t+x*s;       ele[2][2] = z*z*t+c;

    vector3  a,b;


    for (unsigned int i=0;i<NumAtoms();i++)
    {
        a=atomPos(i);
        b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
        b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
        b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2]);
        setAtomPos(i,b);
    }


    for (unsigned int i=0;i<NumBonds();i++)
    {
        if(getBondbyIndex(i)->getBondOrder()<2) continue;
        if(getBondbyIndex(i)->getBondOrder()>6) continue;

        if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

        a=getBondbyIndex(i)->getPlane();
        b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
        b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
        b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2] );

        getBondbyIndex(i)->setPlane(b);
    }

    //update ring information
    for (unsigned int i=0;i<numRings();i++)
    {
        a=getRingbyId(i)->norm;
        b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
        b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
        b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2] );
        getRingbyId(i)->norm=b;

        a=getRingbyId(i)->center;
        b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
        b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
        b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2] );
        getRingbyId(i)->center=b;
    }
}


void HMol::reflect_movie(vector3  v, int interval, int idx)
{
    vector3  a,b,c;

    //atoms
    for (unsigned int i=0;i<NumAtoms();i++) {
        a=atomList_normal[i];
        b=atomList_reflection[i];
        if((b-a).length()<0.1) continue;
        c=idx*(b-a)/interval+a;
        setAtomPos(i,c);
    }


    /*
    for (unsigned int i=0;i<NumAtoms();i++) {
        HCoordinate * single = new HCoordinate();
        single->symbol=getAtomSymbol(i);
        single->p=getAtomPosbyIndex(i);
        atomList_bk.push_back(single);
    }
    for (unsigned int i=0;i<NumBonds();i++)
    {
        if(getBondbyIndex(i)->getBondOrder()<2) continue;
        if(getBondbyIndex(i)->getBondOrder()>6) continue;

        if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

        a=getBondbyIndex(i)->getPlane();
        getBondbyIndex(i)->setPlane(b);
    }
    */

    //rings
    for (unsigned int i=0;i<numRings();i++)
    {
        HRing * ring=getRingbyId(i);

        unsigned int size=ring->atomIdList.size();

        vector3 center=VZero;
        for(unsigned int j = 0; j < size; j++)
            center=center+getAtomPosbyIndex(ring->atomIdList[j]);

        center=center/size;

        //vector3 norm=VZ;
        //norm=cross(getAtomPosbyIndex(ring->atomIdList[0])-getAtomPosbyIndex(ring->atomIdList[1]),
        //        getAtomPosbyIndex(ring->atomIdList[0])-getAtomPosbyIndex(ring->atomIdList[2]));
        //norm.normalize();

        //ring->norm.Set(norm.x(),norm.y(),norm.z());
        ring->center.Set(center.x(),center.y(),center.z());
        ring->scale=1.0;
        ring->opacity=1.0;
        ring->type=0;
        ring->color.Set(0.65,0.65,0.65);

        //radius
        double radius=50.0;
        for (unsigned int i=0;i<size;i++)
        {
            double distToCenter=(getAtomPosbyIndex(ring->atomIdList[i])-center).length();
            if(distToCenter < radius)
                radius = distToCenter;
        }

        ring->radius=radius;
    }
}


void HMol::reverse_movie(int interval,int idx)
{
    vector3  a,b,c;

    //atoms
    for (unsigned int i=0;i<NumAtoms();i++) {
        a=atomList_normal[i];
        b=atomList_reversion[i];
        if((b-a).length()<0.1) continue;

        c=idx*(b-a)/interval+a;
        setAtomPos(i,c);
    }


    //rings
    for (unsigned int i=0;i<numRings();i++)
    {
        HRing * ring=getRingbyId(i);

        unsigned int size=ring->atomIdList.size();

        vector3 center=VZero;
        for(unsigned int j = 0; j < size; j++)
            center=center+getAtomPosbyIndex(ring->atomIdList[j]);

        center=center/size;

        vector3 norm=VZ;
        norm=cross(getAtomPosbyIndex(ring->atomIdList[0])-getAtomPosbyIndex(ring->atomIdList[1]),
                getAtomPosbyIndex(ring->atomIdList[0])-getAtomPosbyIndex(ring->atomIdList[2]));
        norm.normalize();

        ring->norm.Set(norm.x(),norm.y(),norm.z());
        ring->center.Set(center.x(),center.y(),center.z());
        ring->scale=1.0;
        ring->opacity=1.0;
        ring->type=0;
        ring->color.Set(0.65,0.65,0.65);

        //radius
        double radius=50.0;
        for (unsigned int i=0;i<size;i++)
        {
            double distToCenter=(getAtomPosbyIndex(ring->atomIdList[i])-center).length();
            if(distToCenter < radius)
                radius = distToCenter;
        }

        ring->radius=radius;
    }
}


//help functions
void HMol::updateAtomInsideRingOrNot(unsigned int id)
{
    unsigned int idRing=1000001;

    if (numRings()<1) return;

    vector <unsigned int > ringList;
    ringList.clear();
    for(unsigned int i=0;i<numRings();i++)
    {
        if(isAtomInsideRing(id, i))
            ringList.push_back(i);
    }

    if(ringList.size()<1) return;
    unsigned int num=ringList.size();


    vector3 pp=getAtomPosbyIndex(id);

    if(num>1)
        reverse(ringList.begin(),ringList.end());

    foreach (unsigned int idRing,ringList)
    {
        //up-down
        double angle=vectorAngle(getRingbyId(idRing)->norm,getRingbyId(idRing)->center,pp);

        if( abs(angle-90.0) > 2.0 ) {
            removeRingbyId(idRing);
            continue;
        }

        //radius
        double r=getRingbyId(idRing)->radius;
        double ll=(pp-getRingbyId(idRing)->center).length()-r;

        if (abs(ll)>0.1) {
            removeRingbyId(idRing); continue;
        }

        //horizontal angle
        angle=360.0/getRingbyId(idRing)->atomIdList.size();
        unsigned int neighId=getNeighborinsideRing(id,idRing);
        double ang=vectorAngle(getAtomPosbyIndex(neighId),getRingbyId(idRing)->center,getAtomPosbyIndex(id));

        if (abs(ang-angle)>2.0){
            removeRingbyId(idRing);
            continue;
        }
    }
}


unsigned int HMol::getNeighborinsideRing(unsigned int id, unsigned int idRing)
{
    if(getRingbyId(idRing)==nullptr) return 100001;

    unsigned int i=0;
    unsigned int size=getRingbyId(idRing)->atomIdList.size();
    for(i=0;i<size;i++)
    {
        if(id==getRingbyId(idRing)->atomIdList[i]) break;
    }

    if(i<1) return getRingbyId(idRing)->atomIdList[1];
    else  return getRingbyId(idRing)->atomIdList[i-1];
}


void HMol::translate(vector3 p)
{
    if(!hasSelectedAtoms())
    {
        for (unsigned int i=0;i<NumAtoms();i++) {
            setAtomPos(i,atomPos(i)+p);

        }

        vector3 a, b;
        //update bond plane information
        for (unsigned int i=0;i<NumBonds();i++)
        {
            if(getBondbyIndex(i)->getBondOrder()<2) continue;
            if(getBondbyIndex(i)->getBondOrder()>6) continue;

            if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

            a=getBondbyIndex(i)->getPlane()+p;

            getBondbyIndex(i)->setPlane(a);
        }

        //update ring information
        for (unsigned int i=0;i<numRings();i++)
        {
            a=getRingbyId(i)->center+p;
            getRingbyId(i)->center=a;
        }
    }
    else {
        //selected atoms existed
        for (unsigned int i=0;i<NumAtoms();i++)
        {
            if(!getAtombyIndex(i)->isSelected()) continue;
            setAtomPos(i,atomPos(i)+p);
            //setAtomPos(i,p);
            updateAtomInsideRingOrNot(i);
        }
    }
}

void HMol::translate(double x, double y, double z)
{
    vector3 p(x,y,z);
    translate(p);
}




void HMol::clearAll()
{
    removeBackupAtoms();
    removeAllXAtoms();
    removeAllAtoms();
    removeAllBonds();
    removeAllRings();
    removeAllArcs();
}

void HMol::removeAllArcs()
{
    if (arcList.size() < 1) return;
    for (auto it=arcList.begin();it!=arcList.end();++it)
    {
        if(*it != nullptr) {
            delete (*it);
            (*it) = nullptr;
        }
    }
    arcList.clear();
    QVector<HRing*>().swap(arcList);
}

void HMol::removeAllRings()
{
    if (ringList.size() < 1) return;
    for (auto it=ringList.begin();it!=ringList.end();++it)
    {
        if(*it != nullptr) {
            delete (*it);
            (*it) = nullptr;
        }
    }
    ringList.clear();
    QVector<HRing*>().swap(ringList);
}


void HMol::removeAllAtoms()
{  
    //remove bond first;
    removeAllBonds();
    if (atomList.size() < 1) return;
    for (auto it=atomList.begin();it!=atomList.end();++it)
    {
        if(*it != nullptr) {
            delete (*it);
            (*it) = nullptr;
        }
    }
    atomList.clear();
    QVector<HAtom*>().swap(atomList);
}

void HMol::removeAllXAtoms()
{
    if (atomXList.size() < 1) return;
    for (auto it=atomXList.begin();it!=atomXList.end();++it)
    {
        if(*it != nullptr) {
            delete (*it);
            (*it) = nullptr;
        }
    }
    atomXList.clear();
    QVector<HXAtom*>().swap(atomXList);
}



void HMol::removeAllBonds()
{
    removeAllRings();

    if (bondList.size() < 1) return;
    for (auto it=bondList.begin();it!=bondList.end();++it)
    {
        if(*it != nullptr) {
            delete (*it);
            (*it) = nullptr;
        }
    }
    bondList.clear();
    QVector<HBond*>().swap(bondList);


    if(atomList.size()>0)
        for (auto iter=atomList.begin();iter!=atomList.end();iter++)
        {
            (*iter)->clearLinkedAtomId();
        }
}



double HMol::Distance  (unsigned int id1, unsigned int id2)
{
    return vectorDistance(getAtomPosbyIndex(id1),getAtomPosbyIndex(id2));
}

double HMol::Angle (unsigned int id1, unsigned int id2, unsigned int id3)
{
    return vectorAngle(getAtomPosbyIndex(id1),
                       getAtomPosbyIndex(id2),
                       getAtomPosbyIndex(id3));
}



double HMol::Dihedral  (unsigned int id1, unsigned int id2, unsigned int id3, unsigned int id4)
{
    return TorsionAngle(getAtomPosbyIndex(id1),
                        getAtomPosbyIndex(id2),
                        getAtomPosbyIndex(id3),
                        getAtomPosbyIndex(id4)
                        );
}




void HMol::setView3D (View3D * v)
{
    if(v!=nullptr)
        view3d=v;
}

void HMol::removeAtombyId(vector <uint> idList)
{
    sort ( idList.begin(), idList.end());
    vector <uint> :: iterator iter = unique ( idList.begin(), idList.end());
    idList.erase(iter,idList.end());
    sort(idList.rbegin(), idList.rend());

    //remove from the tail one-by-one
    for (unsigned int i=0;i<idList.size();i++)
        removeAtombyId(idList[i]);
}

void HMol::replaceSelelctedAtomWith(unsigned int idx,vector <uint> selectedAtomList)
{
    for (unsigned int i=0;i<selectedAtomList.size();i++)
    {
        getAtombyIndex(selectedAtomList[i])->setSymbol(QString::fromStdString(ElementNames[idx]));
        getAtombyIndex(selectedAtomList[i])->update();
    }
}


bool HMol::isAtomInsideRing(unsigned int idAtom, unsigned int idRing)
{
    if(numRings()<1) return false;

    vector<uint>::iterator it = find(ringList[idRing]->atomIdList.begin(),
                                     ringList[idRing]->atomIdList.end(),
                                     idAtom);

    if (it != ringList[idRing]->atomIdList.end()) return true;
    return false;
}


void HMol::removeRingbyId(unsigned int id)
{
    delete ringList[id];
    ringList.remove(id);

    //refresh bond information
    unsigned int j=NumBonds();
    while (j--)
    {
        HBond * bond= bondList[j];
        if(!bond->isInsideRing()) continue;

        bond->removeRingId(id);
        for(unsigned int i=0;i<bond->sizeofRingId();i++) {
            if(bond->getRingId(i))
                bond->setRingId(i,bond->getRingId(i)-1);
        }
    } 
}

void HMol::removeSingleRingbyId(unsigned int id)
{
    delete ringList[id];
    ringList.remove(id);

    //refresh bond information
    unsigned int j=NumBonds();
    while (j--)
    {
        HBond * bond= bondList[j];
        if(!bond->isInsideRing()) continue;

        bond->removeRingId(id);
        for(unsigned int i=0;i<bond->sizeofRingId();i++) {
            if(bond->getRingId(i))
                bond->setRingId(i,bond->getRingId(i)-1);
        }

    }

    /*
    unsigned int j=NumBonds();
    while (j--)
    {
        HBond * bond= bondList[j];
        bond->setRingId(100000001);
    }

    j=numRings();
    while (j--)
    {
        unsigned int size=getRingbyId(j)->atomIdList.size();
        HBond * bond=nullptr;
        for (unsigned int i=0;i<size-1;i++)
        {
            bond=getBondbyAtomIndex(getRingbyId(j)->atomIdList[i],getRingbyId(j)->atomIdList[i+1]);
            if (bond==nullptr) continue;
            bond->setRingId(j);
        }
        bond=getBondbyAtomIndex(getRingbyId(j)->atomIdList[0],getRingbyId(j)->atomIdList[size-1]);
        if (bond!=nullptr) bond->setRingId(j);
    }
*/
}


void HMol::removeLonelyHydrogen()
{
    //remove terminal Hydrogen
    unsigned int id=NumAtoms();

    cout << id << " atoms left"<<endl;
    while(id--)
    {
        //Hydrogen?
        if(getAtombyIndex(id)->Symbol()!="H")    continue;

        //lonely?
        if(getAtombyIndex(id)->numNeighbors()>0) continue;


        cout << " wissssssss " <<id+1 <<endl;
        //continue;


        //refresh Bonds index
        unsigned int j=NumBonds();
        while (j--)
        {
            HBond * bond= bondList[j];
            unsigned int idx0=bond->atomIndex0();
            unsigned int idx1=bond->atomIndex1();

            if (idx0 > id )   bond->setBeginIndex(idx0-1);
            if (idx1 > id )   bond->setEndIndex(idx1-1);
        }


        //update infomation of ring
        j=numRings();
        if(j>=1)
        {
            //update information of ring left
            while (j--)
            {
                unsigned int k=ringList[j]->atomIdList.size();
                while(k--)
                {
                    unsigned int ii=ringList[j]->atomIdList[k];
                    if(ii>id)
                        ringList[j]->atomIdList[k]=ii-1;
                }
            }
        }

        //update information of atoms left
        delete atomList[id];
        atomList.remove(id);


        j=NumAtoms();
        while (j--)  atomList[j]->clearNeighbors();

        //update neighor information of each atom
        j=NumBonds();
        if(j>0)
            while (j--)
            {
                unsigned int idx0=bondList[j]->atomIndex0();
                unsigned int idx1=bondList[j]->atomIndex1();

                getAtombyIndex(idx1)->appendNeighbor(idx0);
                getAtombyIndex(idx0)->appendNeighbor(idx1);
            }
    }
}


void HMol::removeTerminalHydrogen(unsigned int id)
{
    unsigned int j=NumBonds();
    while (j--)
    {
        HBond * bond= bondList[j];
        unsigned int idx0=bond->atomIndex0();
        unsigned int idx1=bond->atomIndex1();

        if (idx0 == id || id==idx1)  {
            delete bondList[j];
            bondList.remove(j);
            continue;
        }

        if (idx0 > id )
            bond->setBeginIndex(idx0-1);

        if (idx1 > id )
            bond->setEndIndex(idx1-1);
    }

    if(bondList.size()<1) {
        bondList.clear();
        QVector<HBond*>().swap(bondList);
    }


    //update infomation of rings
    j=numRings();
    if(j>=1)
    {
        while (j--)
        {
            if(!isAtomInsideRing(id, j))  continue;
            removeSingleRingbyId(j);
        }

        //update information of ring left
        unsigned int k=numRings();
        while (k--)
        {
            unsigned int size=ringList[k]->atomIdList.size();
            while(size--)
            {
                unsigned int ii=ringList[k]->atomIdList[size];
                if(ii>id)
                    ringList[k]->atomIdList[size]=ii-1;
            }
        }
    }

    //remove this atom
    delete atomList[id];
    atomList.remove(id);

    //clear neighbor informations
    j=NumAtoms();
    while (j--)  atomList[j]->clearNeighbors();

    //update neighbor information of each atom
    j=NumBonds();
    while (j--)
    {
        unsigned int idx0=bondList[j]->atomIndex0();
        unsigned int idx1=bondList[j]->atomIndex1();

        getAtombyIndex(idx1)->appendNeighbor(idx0);
        getAtombyIndex(idx0)->appendNeighbor(idx1);
    }
}


void HMol::removeSingleAtombyId(unsigned int id)
{
    //remove the last atom with out bonds!
    if(atomList.size()==1) {
        clearAll();
        return;
    }

    //removed bonds of this atom
    unsigned int j=NumBonds();
    while (j--)
    {
        HBond * bond= bondList[j];
        unsigned int idx0=bond->atomIndex0();
        unsigned int idx1=bond->atomIndex1();

        if (idx0 == id || id==idx1)  {
            delete bondList[j];
            bondList.remove(j);
            continue;
        }

        if (idx0 > id )
            bond->setBeginIndex(idx0-1);

        if (idx1 > id )
            bond->setEndIndex(idx1-1);
    }

    if(bondList.size()<1) {
        bondList.clear();
        QVector<HBond*>().swap(bondList);
    }



    //update infomation of rings
    j=numRings();
    if(j>=1)
    {
        while (j--)
        {
            if(!isAtomInsideRing(id, j))  continue;
            removeSingleRingbyId(j);
        }

        //update information of ring left
        unsigned int k=numRings();
        while (k--)
        {
            unsigned int size=ringList[k]->atomIdList.size();
            while(size--)
            {
                unsigned int ii=ringList[k]->atomIdList[size];
                if(ii>id)
                    ringList[k]->atomIdList[size]=ii-1;
            }
        }
    }


    //remove this atom
    delete atomList[id];
    atomList.remove(id);

    //clear neighbor informations
    j=NumAtoms();
    while (j--)  atomList[j]->clearNeighbors();

    //update neighbor information of each atom
    j=NumBonds();
    while (j--)
    {
        unsigned int idx0=bondList[j]->atomIndex0();
        unsigned int idx1=bondList[j]->atomIndex1();

        getAtombyIndex(idx1)->appendNeighbor(idx0);
        getAtombyIndex(idx0)->appendNeighbor(idx1);
    }

}


void HMol::removeAtombyId(unsigned int id)
{
    //cout << "remove atom begin "<<endl;

    //remove the last atom with out bonds!
    if(atomList.size()==1) {
        clearAll();
        return;
    }


    //find the terminal hydrogen first
    unsigned int size=getAtombyIndex(id)->numNeighbors();
    while (size--)
    {
        unsigned int ii=getAtombyIndex(id)->getNeighborId(size);

        //Hydrogen?
        if(getAtombyIndex(ii)->Symbol()!="H")    continue;

        //lonely?
        if(getAtombyIndex(ii)->numNeighbors()>1) continue;

        removeTerminalHydrogen(ii);
    }


    //removed bonds of this atom
    unsigned int j=NumBonds();
    while (j--)
    {
        HBond * bond= bondList[j];
        unsigned int idx0=bond->atomIndex0();
        unsigned int idx1=bond->atomIndex1();

        if (idx0 == id || id==idx1)  {
            delete bondList[j];
            bondList.remove(j);
            continue;
        }

        if (idx0 > id )
            bond->setBeginIndex(idx0-1);

        if (idx1 > id )
            bond->setEndIndex(idx1-1);
    }

    if(bondList.size()<1) {
        bondList.clear();
        QVector<HBond*>().swap(bondList);
    }


    //update infomation of rings
    j=numRings();
    if(j>=1)
    {
        while (j--)
        {
            if(!isAtomInsideRing(id, j))  continue;
            removeSingleRingbyId(j);
        }

        //update information of ring left
        unsigned int k=numRings();
        while (k--)
        {
            unsigned int size=ringList[k]->atomIdList.size();
            while(size--)
            {
                unsigned int ii=ringList[k]->atomIdList[size];
                if(ii>id)
                    ringList[k]->atomIdList[size]=ii-1;
            }
        }
    }


    //remove this atom
    delete atomList[id];
    atomList.remove(id);

    //clear neighbor informations
    j=NumAtoms();
    while (j--)  atomList[j]->clearNeighbors();

    //update neighbor information of each atom
    j=NumBonds();
    while (j--)
    {
        unsigned int idx0=bondList[j]->atomIndex0();
        unsigned int idx1=bondList[j]->atomIndex1();

        getAtombyIndex(idx1)->appendNeighbor(idx0);
        getAtombyIndex(idx0)->appendNeighbor(idx1);
    }

    // displayBonds();
    // displayAtoms();


    //remove terminal Hydrogen
    //removeLonelyHydrogen();
}

void HMol::removeBondbyId(unsigned int id)
{
    unsigned int idx0=bondList[id]->atomIndex0();
    unsigned int idx1=bondList[id]->atomIndex1();
    removeBondbyAtomIds(idx0,idx1);
}



void HMol::removeBondbyAtomIds(unsigned int id0,unsigned int id1)
{
    unsigned int j=NumBonds();

    if(j<1) return;

    while (j--)
    {
        unsigned int idx0=bondList[j]->atomIndex0();
        unsigned int idx1=bondList[j]->atomIndex1();

        if ( id0==idx0 && id1==idx1)
        {
            getAtombyIndex(id0)->removeLinkedAtomId(id1);
            getAtombyIndex(id1)->removeLinkedAtomId(id0);

            delete bondList[j];
            bondList.remove(j);
            break;
        }

        if ( id1==idx0 && id0==idx1)
        {
            getAtombyIndex(id0)->removeLinkedAtomId(id1);
            getAtombyIndex(id1)->removeLinkedAtomId(id0);

            delete bondList[j];
            bondList.remove(j);
            break;
        }
    }


    //update infomation of rings
    j=numRings();
    if(j>=1)
    {
        while (j--)
        {
            if(isAtomInsideRing(id0, j) || isAtomInsideRing(id1, j))
                removeSingleRingbyId(j);
        }

        //update information of ring left
        unsigned int k=numRings();
        while (k--)
        {
            unsigned int size=ringList[k]->atomIdList.size();
            while(size--)
            {
                unsigned int ii=ringList[k]->atomIdList[size];
                if(ii>id0 || ii>id1)
                    ringList[k]->atomIdList[size]=ii-1;
            }
        }
    }
}



void HMol::removeBondbyAtomId(unsigned int id)
{
    QVector<HBond *>::iterator iter;
    for (iter=bondList.begin();iter!=bondList.end();iter++)
    {
        if ( id==(*iter)->atomIndex0() || id==(*iter)->atomIndex1())
        {
            delete (*iter);
            (*iter) = nullptr;
            bondList.erase(iter);
        }
    }
    perceivePlaneRingforBonds();
}


void HMol::loadFile(QString filename)
{
    if( filename.endsWith("xyz") ||  filename.endsWith("XYZ") )    {
        load_xyz(filename);
        return;
    }


    if( filename.endsWith("mol") ||  filename.endsWith("MOL") )    {
        load_mol(filename);
        return;
    }

    if( filename.endsWith("pdb") ||  filename.endsWith("PDB") )    {
        load_other(filename);
        return;
    }
    
    if( filename.endsWith("fch") ||  filename.endsWith("fchk")
            ||  filename.endsWith("FCH") ||  filename.endsWith("FCHK") )
    {
        load_GaussianFchk(filename);
        return;
    }


    load_other(filename);
}


void HMol::load_GaussianFchk(QString filename)
{
    
}



void HMol::parse_xbt_mol(QString text)
{
    double x, y, z;
    QString  atomLabel,line;
    QStringList lst=text.split("\n"),single;

    int atoms=0,bonds=0;
    int i,j=0,bo;
    for (i=0;i<10;i++)
    {
        line=lst[i].simplified();
        if (line.contains("999 V2000"))
        {
            single=line.split(QRegularExpression("\\s+"));
            atoms=single[0].toInt();
            bonds=single[1].toInt();
            break;
        }
    }


    j=0;
    while(i++)
    {
        j++;
        line=lst[i].simplified();
        single=line.split(QRegularExpression("\\s+"));
        x=single[0].toDouble();
        y=single[1].toDouble();
        z=single[2].toDouble();
        atomLabel=single[3];
        addAtom(atomLabel,x,y,z);
        if(j==atoms) break;
    }

    while(i++)
    {
        line=lst[i].simplified();
        if(line.contains("M")) break;
        single=line.split(QRegularExpression("\\s+"));


        bo=single[2].toInt();
        if(bo==4)   bo=6;
        addBond(single[0].toInt()-1,single[1].toInt()-1,bo);
    }

    perceivePlaneRingforBonds();
}

void HMol::loadxyzfromQString(QString text)
{
    clearAll();

    double x, y, z;
    QString  atomLabel,line;
    QStringList lst=text.split("\n"),single;

    bool isxtbmol=false;
    for (uint i=0;i<lst.size();i++)
    {
        line=lst[i].simplified();
        if (line.contains("999 V2000"))
        {
            isxtbmol=true;
            break;
        }

        single=line.split(QRegularExpression("\\s+"));
        if (single.size()!=4) continue;

        if(isNumber(single[0]))
            atomLabel=QString::fromStdString(ElementNames[single[0].toInt()]);
        if(isEnglish(single[0]))
            atomLabel=single[0];

        x=single[1].toDouble();
        y=single[2].toDouble();
        z=single[3].toDouble();

        addAtom(atomLabel,x,y,z);
    }


    if(!isxtbmol) {
        perceiveBondOrder();
        perceivePlaneRingforBonds();
        return;
    }

    parse_xbt_mol(text);
}



void HMol::load_xyz(QString filename)
{
    clearAll();

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(0, "Warning","Cannot Open File!");
    }

    double x, y, z;
    QString  atomLabel;
    QStringList lst;

    QString line;
    
    bool isRingsExisted=false;
    while(!file.atEnd())
    {
        line=file.readLine().simplified();
        if(line.contains("ring")) {
            isRingsExisted=true;
            break;
        }

        line.chop(1);
        cout<< line.toStdString()<<endl;
        if (isNumber(line)) continue;
        lst=line.split(QRegularExpression("\\s+"));

        cout <<lst.size()<<endl;

        if (lst.size()!=4) continue;

        x=lst[1].toDouble();
        y=lst[2].toDouble();
        z=lst[3].toDouble();

        if(isInt(lst[0])) {
             addAtom(lst[0].toInt(),x,y,z);
        }
        else {
            addAtom(lst[0],x,y,z);
        }
    }

    perceiveBondOrder();
    perceiveRings();
    reCalculateBonds();

    if(!isRingsExisted)
        file.close();

    vector <uint > r;
    while(!file.atEnd())
    {
        line=file.readLine().simplified();
        lst=line.split(QRegularExpression("\\s+"));
        r.clear();
        for(auto k:lst)  r.push_back(k.toInt()-1);

    }
    file.close();

}



void HMol::addAtom(string atomLabel,vector3 p)
{
    if(atomExisted(QString::fromStdString(atomLabel),p)) return;

    HAtom * atom = new HAtom(QString::fromStdString(atomLabel),p);
    atomList.push_back(atom);

    getLastAtom()->setIndex(atomList.size());
    //getLastAtom()->setScale(0.3);
}

void HMol::addAtom(QString atomLabel,vector3 p)
{
    if(atomExisted(atomLabel,p)) return;

    HAtom * atom = new HAtom(atomLabel,p);
    atomList.push_back(atom);

    getLastAtom()->setIndex(atomList.size());
    //getLastAtom()->setScale(0.3);
}


void HMol::addAtom(QString atomLabel,double x,double y,double z)
{
    if(atomExisted(atomLabel,x,y,z)) return;

    HAtom * atom = new HAtom(atomLabel,x,y,z);
    atomList.push_back(atom);

    getLastAtom()->setIndex(atomList.size()-1);
    //getLastAtom()->setScale(0.3);
}


void HMol::addAtom(string atomLabel,double x,double y,double z)
{
    if(atomExisted(QString::fromStdString(atomLabel),x,y,z)) return;

    HAtom * atom = new HAtom(QString::fromStdString(atomLabel),x,y,z);
    atomList.push_back(atom);

    getLastAtom()->setIndex(atomList.size()-1);
    //getLastAtom()->setScale(0.3);
}

void HMol::addAtom(unsigned int atomicNumber,double x,double y,double z)
{
    if(atomExisted(atomicNumber,x,y,z)) return;

    HAtom * atom = new HAtom(atomicNumber,x,y,z);
    atomList.push_back(atom);

    getLastAtom()->setIndex(atomList.size()-1);
    //getLastAtom()->setScale(0.3);
}

void HMol::addAtom(unsigned int atomicNumber, vector3 p)
{
    if(atomExisted(atomicNumber,p)) return;

    HAtom * atom = new HAtom(atomicNumber,p);
    atomList.push_back(atom);

    getLastAtom()->setIndex(atomList.size()-1);
    //getLastAtom()->setScale(0.3);
}



bool HMol::atomExisted(unsigned int atomicNum,vector3 p)
{
    if(atomList.size()<1) return false;
    foreach(HAtom * atom, atomList)
    {
        if(atom==nullptr) continue;
        double length=(p-atom->Pos()).length();
        if ( length<0.5 && atomicNum==atom->atomicNum())
            return true;
    }
    return false;
}

bool HMol::atomExisted(unsigned int atomicNum,double x,double y,double z)
{
    vector3 p(x,y,z);
    return atomExisted(atomicNum,p);
}


bool HMol::atomExisted(QString atomSymbol, vector3 p)
{
    if(atomList.size()<1) return false;

    foreach(HAtom * atom, atomList)
    {
        if(atom==nullptr) continue;
        double length=(p-atom->Pos()).length();
        if ( length<0.5 && atomSymbol==atom->Symbol())
            return true;
    }
    return false;
}


bool HMol::atomExisted(QString atomSymbol,double x,double y,double z)
{
    vector3 p(x,y,z);
    return atomExisted(atomSymbol,p);
}


HAtom * HMol::getAtom(uint id)
{
   return atomList[id];
}

HAtom * HMol::getLastAtom()
{
    unsigned int size=atomList.size();
    if (size>0)
        return atomList[size-1];
    return nullptr;
}




bool HMol::addAtom(HAtom * other)
{
    if(other==nullptr) return false;

    double eps=1e-6;

    double x=other->x();
    double y=other->y();
    double z=other->z();

    if(abs(x)<eps)        x=0.0;
    if(abs(y)<eps)        y=0.0;
    if(abs(z)<eps)        z=0.0;

    if(atomExisted(other->atomicNum(),other->cartesian()))
        return false;

    HAtom * atom = new HAtom();
    atom->setAtomicNum(other->atomicNum());
    atom->setSymbol(other->Symbol());

    // cout << "inside addAtom1" <<endl;
    // cout << other->atomicNum()<<endl;
    //  cout << atom->atomicNum()<<endl;

    //  cout << other->Symbol().toStdString().c_str()<<endl;
    //   cout << atom->Symbol().toStdString().c_str()<<endl;



    atom->setIndex(other->index());
    atom->setRadius (other->radius());
    atom->setMass(other->Mass());

    atom->setCartesian(x,y,z);

    atom->setColor (other->Color());

    atomList.push_back(atom);
    getLastAtom()->setIndex(atomList.size()-1);
    return true;
}



void HMol::appendAtom(QString s,double x,double y ,double z)
{
    if(atomExisted(s,x,y,z))
        return;

    HAtom * atom = new HAtom(s,x,y,z);
    atomList.push_back(atom);
    getLastAtom()->setIndex(atomList.size()-1);

    //refresh bond information

}

void HMol::appendAtom(unsigned int atomicNum,double x,double y ,double z)
{
    if(atomExisted(atomicNum,x,y,z))
        return;

    HAtom * atom = new HAtom(atomicNum,x,y,z);
    atomList.push_back(atom);
    getLastAtom()->setIndex(atomList.size()-1);

    //refresh bond information
}



void HMol::insertAtom(unsigned int idx, QString s,double x,double y ,double z)
{
    if(atomExisted(s,x,y,z))
        return;

    HAtom * atom = new HAtom(s,x,y,z);

    atomList.insert(idx-1,atom);


    //refresh Bonds index
    for (unsigned int j=0;j<NumBonds();j++)
    {
        HBond * bond= bondList[j];
        unsigned int idx1=bond->atomIndex0();
        unsigned int idx2=bond->atomIndex1();

        if (idx1 >= idx )
            bond->setBeginIndex(idx1+1);

        if (idx2 >= idx )
            bond->setEndIndex(idx2+1);
    }

    //calculate new bonds
    for (unsigned int i=0;i<NumAtoms();i++)
    {
        if (i==idx) continue;
        //if (distance(i,idx) > 1.0)

    }
}

void HMol::insertAtom(unsigned int idx, unsigned int atomicNum, double x,double y ,double z)
{
    HAtom * atom = new HAtom(atomicNum,x,y,z);
    atomList.insert(idx-1,atom);
}



void HMol::load_mol(QString filename)
{
    clearAll();

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(0, "Warning","Cannot Open File!");
    }

    double x, y, z;
    QString  atomLabel;
    QStringList lst;
    QString line;;
    while(!file.atEnd()) {
        line=file.readLine().simplified();
        if (isNumber(line)) continue;
        lst=line.split(QRegularExpression("\\s+"));

        if (lst.size()==9)
        {
            atomLabel=lst[3];
            x=lst[0].toDouble();
            y=lst[1].toDouble();
            z=lst[2].toDouble();

            addAtom(atomLabel,x,y,z);
            continue;
        }

        if (lst.size()==6 && atomList.size()>0)
        {
            addBond(lst[0].toInt()-1,lst[1].toInt()-1,lst[2].toInt());
        }
    }
    file.close();



    cout << "read "<<filename.toStdString().c_str() << " done!"<<endl;
}


void HMol::addBond(unsigned int beg, unsigned int end, unsigned int order)
{
    if(getBondOrderbyAtomIndex(beg,end)>0) return;

    HBond * bond = new HBond(beg,end,order);

    bondList.push_back(bond);

    getAtombyIndex(beg)->appendLinkdedAtomId(end);
    getAtombyIndex(end)->appendLinkdedAtomId(beg);
}

void HMol::addBond(unsigned int beg, unsigned int end)
{
    if(getBondOrderbyAtomIndex(beg,end)>0) return;
    addBond(beg,end,1);

}


//deep copy --------------------------
void HMol::copyRing(vector <uint> _atomIdList, vector3 norm, vector3 center, double radius, uint type, vector3 color, double scale, bool isAromatic )
{
    HRing * ring = new HRing();
    ring->atomIdList.clear();

    for(uint i=0;i<_atomIdList.size();i++)
        ring->atomIdList.push_back(_atomIdList[i]);

    ring->norm=norm;
    ring->center=center;
    ring->radius=radius;
    ring->type=type;
    ring->color=color;
    ring->scale=scale;
    ring->isAromatic=isAromatic;

    ringList.push_back(ring);
}


void HMol::copyBond(uint beg, uint end, uint order, vector<int> ringId, vector3 plane)
{
    HBond * bond = new HBond(beg,end,order);
    bond->setPlane(plane);

    for (auto i: ringId)
        bond->appendRingId(i);

    bondList.push_back(bond);
}


void HMol::copyAtom(uint _atomicNum,
                    QString _symbol,
                    vector3 p,
                    double _radius,
                    double _mass,
                    uint32_t _color
                    )
{

    HAtom * atom = new HAtom();
    atom->setAtomicNum(_atomicNum);
    atom->setSymbol(_symbol);
    atom->setRadius (_radius);
    atom->setMass(_mass);

    atom->setCartesian(p);

    atom->setColor (_color);
    atomList.push_back(atom);
    getLastAtom()->setIndex(atomList.size()-1);
}



















void HMol::perceiveBonds()
{
    for (unsigned int i=0; i<NumAtoms();i++)
    {
        double ri=getAtombyIndex(i)->radius();

        for (unsigned int j=i+1; j<NumAtoms();j++)
        {
            double rij=Distance(i,j);
            double rj=getAtombyIndex(j)->radius();

            if ( rij > 0.7 * (ri+rj))
                continue;
            addBond(i,j,1);
        }

    }
}


void HMol::perceiveBonds(int idxofAtom)
{


}




void HMol::load_other(QString filename)
{
    clearAll();

    // Construct the OpenBabel objects, set the file type
    OBConversion conv;
    OBFormat * inFormat;


    inFormat = conv.FormatFromExt(filename.toLatin1());
    if (!inFormat || !conv.SetInFormat(inFormat))
    {
        QMessageBox::information(0, "Warning","Unknown file type!");
    }


    // Now attempt to read the molecule in
    ifstream ifs;
    ifs.open(filename.toLocal8Bit()); // This handles utf8 file names etc
    if (!ifs)  {
        QMessageBox::information(0, "Warning","Cannot Open File!");
        return;
    }


    obmol.Clear();

    if (conv.Read(&obmol, &ifs))
    {
        obmol.ToInertialFrame();
        obmol.ConnectTheDots();
        //obmol.SetAromaticPerceived();
        obmol.PerceiveBondOrders();
        obmol.SetSSSRPerceived();
        fromOBMol();
        setFileName(fileName);
    }

    obmol.Clear();
    ifs.close();
}


//obmol.SetHydrogensAdded();
void HMol::perceiveBondOrder_OpenBabel()
{
    buildOBMol();
    obmol.ConnectTheDots();
    obmol.SetAromaticPerceived();
    obmol.PerceiveBondOrders();
    obmol.SetSSSRPerceived();

    removeAllBonds();




    std::vector<OpenBabel::OBBond*>::iterator j;
    for (OpenBabel::OBBond *obbond = obmol.BeginBond(j); obbond; obbond = obmol.NextBond(j))
    {
        unsigned int i=obbond->GetBeginAtom()->GetIdx()-1;
        unsigned int j=obbond->GetEndAtom()->GetIdx()-1;


        addBond(i,j, obbond->GetBondOrder() );

        // cout << obbond->GetBeginAtom()->GetIdx()-1 <<" " << obbond->GetEndAtom()->GetIdx()-1 <<" " <<obbond->GetBondOrder()<<endl;
    }


    obmol.Clear();

    perceivePlaneBonds();
    perceiveRings_OB();
}



void HMol::perceiveBondOrder()
{
    perceiveBondOrder_OpenBabel();
    perceiveHydrogenBonds();
}


void HMol::perceiveHydrogenBonds()
{
    return;

    unsigned int NA=NumAtoms();

    for(unsigned int i=0;i<NA;i++)
    {
        QString A=getAtomSymbol(i);
        if( (A!="O") && (A!="F") && (A!="N") && (A!="Cl") ) return;

        vector3 p0=getAtomPosbyIndex(i);
        double l;

        for(unsigned int j=i+1;j<NA;j++)
        {
            if(getAtomSymbol(j)!="H") continue;

            l=(p0-getAtomPosbyIndex(j)).length();

            if(l< 1.7 || l>2.5) continue;

            addBond(j,i,5);

            //getAtombyIndex(j)->appendLinkdedAtomId(i);
            //getAtombyIndex(i)->appendLinkdedAtomId(j);
        }

    }
}

void HMol::perceiveHydrogenBondId(unsigned int id)
{
    QString A=getAtomSymbol(id);
    if( (A!="O") && (A!="F") && (A!="N") && (A!="Cl") ) return;

    vector3 p0=getAtomPosbyIndex(id);
    double l;

    for(unsigned int i=0;i<NumAtoms();i++)
    {
        if(i=id) continue;
        if(getAtomSymbol(id)!="H") continue;

        l=(p0-getAtomPosbyIndex(i)).length();
        if(l< 1.7 || l>2.5) continue;

        addBond(id,i,5);

        getAtombyIndex(id)->appendLinkdedAtomId(i);
        getAtombyIndex(i)->appendLinkdedAtomId(id);
    }


}

void HMol::perceivePlaneBondbyId(unsigned int id)
{
    if(getBondbyIndex(id)->getBondOrder()<2)
        return;

//    cout << "\n\n\nsssssssssssssssssssssssssssssssss\n----------------------"<<endl;

   unsigned int idx0 = getBondbyIndex(id)->atomIndex0();
   unsigned int idx1 = getBondbyIndex(id)->atomIndex1();

//   cout << " percept plane for "<< idx0+1 << "<-->"<<idx1+1<< "  "<<getBondbyIndex(id)->getBondOrder()<<endl;


    vector3 norm=VZ;
    if (getBondbyIndex(id)->isInsideRing())
    {
        //cout << " inside ring \n" ;
        norm=getRingbyId(getBondbyIndex(id)->getRingId(0))->norm;

        vector3 v1=getAtomPosbyIndex(idx0);
        vector3 v2=getAtomPosbyIndex(idx1);
        vector3 v=v2-v1,mid0=0.5*(v1+v2);

        //update normal to ensure the dashed line inside
        unsigned int idRing=getBondbyIndex(id)->getRingId(0);

        HRing  * ring = getRingbyId(idRing);

        norm=ring->norm;
        vector3 v0=ring->center;
        vector3  shift0=0.1*cross(ring->norm, v);


        //make sure: outside for the first time
        if( (mid0-shift0-v0).length() < (mid0-v0).length())
            norm=-norm;


        getBondbyIndex(id)->setPlane(norm);
        return;
    }

//    cout << "\n outside ring " <<endl;;



    //simple initialize
    vector3 v=getAtomPosbyIndex(idx1)-getAtomPosbyIndex(idx0);
    v.createOrthoVector(norm);
    getBondbyIndex(id)->setPlane(norm);


    //search
    unsigned int idx2=1000001;

    // search those around the first atom (idx0)
    HAtom * atom=getAtombyIndex(idx0);
    vector  <uint>  vv;
    vv.clear();

    if (atom->numNeighbors()>=2)
    {
        //cout << "search "<< idx0+1<<endl;
        for (unsigned int i=0;i<atom->numNeighbors();i++)
        {
            unsigned int _id=atom->getNeighborId(i);


            if (_id==idx1)  //the same idx0---idx1
                continue;
            vv.push_back(_id);
        }
    }

    if(vv.size()>1)
    {
        idx2=vv[0];
        //cout << "find " <<idx0+1<< " "<< idx1+1<< " "<< idx2+1 << endl;
        norm=cross(getAtomPosbyIndex(idx1)-getAtomPosbyIndex(idx0),
                   getAtomPosbyIndex(idx1)-getAtomPosbyIndex(idx2)).normalize();

        getBondbyIndex(id)->setPlane(norm);

        return;
    }



    //failed, continue search those around another atom (idx1)
    atom=getAtombyIndex(idx1);
    vv.clear();
    if (atom->numLinkdedAtoms()>1)
    {
        for (unsigned int i=0;i<atom->numLinkdedAtoms();i++)
        {
            unsigned int _id=atom->getLinkdedAtomId(i);
            if (_id==idx0)   continue;

            if(getAtombyIndex(_id)->atomicNum()<1)
                continue;

            vv.push_back(_id);

        }

        if(vv.size()>1)
        {
            idx2=vv[0];
            norm=cross(getAtomPosbyIndex(idx1)-getAtomPosbyIndex(idx0),
                       getAtomPosbyIndex(idx1)-getAtomPosbyIndex(idx2)).normalize();

            getBondbyIndex(id)->setPlane(norm);
        }
    }
    //cout << " outside ring end2\n" <<endl;;
}



void HMol::perceivePlaneBonds()
{
    unsigned int size=NumBonds();
    if (size<1) return;

    for (unsigned int i=0;i<NumBonds();i++)
        perceivePlaneBondbyId(i);
}

unsigned int HMol::getBondOrderbyAtomIndex(unsigned int id1, unsigned int id2)
{
    HBond * bond=nullptr;

    unsigned int i,j;
    for (unsigned int k=0;k<NumBonds();k++)
    {
        i=getBondbyIndex(k)->atomIndex0();
        j=getBondbyIndex(k)->atomIndex1();

        if  ( (i==id1 && j==id2 )) {
            bond=getBondbyIndex(k);
            break;
        }

        if  ( (j==id1 && i==id2 )) {
            bond=getBondbyIndex(k);
            break;
        }
    }

    if (bond) return bond->getBondOrder();
    return 0;
}


HBond * HMol::getBondbyAtomIndex(unsigned int id1, unsigned int id2)
{
    unsigned int i,j;
    for (unsigned int k=0;k<NumBonds();k++)
    {
        i=getBondbyIndex(k)->atomIndex0();
        j=getBondbyIndex(k)->atomIndex1();
        if  ( i==id1 && j==id2 )
            return getBondbyIndex(k);

        if  ( j==id1 && i==id2 )
            return getBondbyIndex(k);
    }
    return nullptr;
}

HBond *  HMol::getBondIndexbyAtomIndex(unsigned int id1, unsigned int id2, unsigned int & id)
{
    unsigned int i,j;
    for (unsigned int k=0;k<NumBonds();k++)
    {
        id=k;
        i=getBondbyIndex(k)->atomIndex0();
        j=getBondbyIndex(k)->atomIndex1();
        if  ( i==id1 && j==id2 )
            return getBondbyIndex(k);

        if  ( j==id1 && i==id2 )
            return getBondbyIndex(k);
    }

    id=1000000001;
    return nullptr;
}



void HMol::perceiveAllAtomHybridization()
{
    for(unsigned int i=0;i<NumAtoms();i++) {
        AtomHybridization hybridization=perceiveHybridization(i);
        getAtombyIndex(i)->setHybridization(hybridization);

        if (hybridization==SP2 || hybridization==SP)
            getAtombyIndex(i)->setAromatic(true);
        else
            getAtombyIndex(i)->setAromatic(false);
    }
}

AtomHybridization HMol::perceiveHybridization(unsigned int id)
{
    unsigned int numberOfBonds=getAtombyIndex(id)->numNeighbors();
    AtomHybridization hybridization = SP3; // default to sp3

    // TODO: Handle hypervalent species, SO3, SO4, lone pairs, etc.
    if (numberOfBonds > 4) {
        //      hybridization = numberOfBonds; // e.g., octahedral, trig. bipyr.,
        hybridization=UNknown;
    }
    else
    {
        // Count multiple bonds
        unsigned int numTripleBonds = 0;
        unsigned int numDoubleBonds = 0;
        unsigned int numAromaticBonds = 0;

        for (unsigned int i=0;i<numberOfBonds;i++)
        {
            unsigned int bo=getBondOrderbyAtomIndex(id,getAtombyIndex(id)->getNeighborId(i));

            if (bo == 2)    numDoubleBonds++;
            else if (bo == 3)   numTripleBonds++;
            else if (bo == 6)   numAromaticBonds++;
        }


        if (numTripleBonds > 0 || numDoubleBonds > 1)
            hybridization = SP;
        else if (numDoubleBonds > 0)
            hybridization = SP2;
        else if (numAromaticBonds > 0)
            hybridization = SP2;

    }

    return hybridization;
}


void HMol::filterRings()
{
    int bo=0;
    int NRings=numRings();
    if(NRings==0) return;

    while (NRings--)
    {
        bool remove=false;
        
        bo=getBondOrderbyAtomIndex(getRingbyId(NRings)->atomIdList[0],
                                   getRingbyId(NRings)->atomIdList[getRingbyId(NRings)->size-1]);
        if(bo==5 || bo==7 || bo<2 )  {
            removeRingbyId(NRings);
            continue;
        }
        
        for(uint j=0;j<getRingbyId(NRings)->size-1;j++)
        {
            bo=getBondOrderbyAtomIndex(getRingbyId(NRings)->atomIdList[j],
                                       getRingbyId(NRings)->atomIdList[j+1]);

            if(bo==5 || bo==7 || bo<2 )  {
                remove=true;
                break;
            }
        }
        if(remove)  removeRingbyId(NRings);
    }

}



void HMol::perceiveRings_OB()
{
    removeAllRings();
    if (NumBonds() < 3)  return;
    buildOBMol();

    std::vector<OpenBabel::OBRing *> obrings;
    obmol.SetAtomTypesPerceived(true);
    obmol.SetAromaticPerceived(true);
    obmol.PerceiveBondOrders();
    obmol.EndModify();
    obrings = obmol.GetSSSR();


    OpenBabel::vector3  center, norm1, norm2;

    double radius=50.0;
    double distToCenter;
    unsigned int i,size;
    bool canAdd=false;
    int j,k;

    foreach(OpenBabel::OBRing *r, obrings)
    {
        HRing * ring = new HRing();
        ring->atomIdList.clear();

        //cout << getAtomSymbol(ring->atomIdList[0]).toStdString() <<endl;
        //cout << atomicNum(ring->atomIdList[0]) <<endl;
        //cout << atomicNum(ring->atomIdList[1]) <<endl;

        //cout << atomSymbol(ring->atomIdList[0]).toStdString() <<endl;
        //cout << atomSymbol(ring->atomIdList[1]).toStdString() <<endl;

        //if(atomicNum(ring->atomIdList[0])==5 &&  atomicNum(ring->atomIdList[1])==7) continue;
        //if(atomicNum(ring->atomIdList[0])==7 &&  atomicNum(ring->atomIdList[1])==5) continue;


        std::vector<int>::iterator j;
        for(j = r->_path.begin(); j != r->_path.end(); ++j)
            ring->atomIdList.push_back( (*j)-1);

        r->findCenterAndNormal(center,norm1,norm2);
        ring->norm.Set(norm2.x(),norm2.y(),norm2.z());
        ring->center.Set(center.x(),center.y(),center.z());


        size=r->Size();

        //radius
        radius=50.0;
        for (i=0;i<size;i++)
        {
            distToCenter=(getAtomPosbyIndex(ring->atomIdList[i])-ring->center).length();
            if(distToCenter < radius)
                radius = distToCenter;
        }
        canAdd=true;
        for (i=0;i<3;i++)
        {
            if( fabs(((getAtomPosbyIndex(ring->atomIdList[0])-ring->center).length() -radius)) > 0.8)
            {
                canAdd=false;
                break;
            }
        }
        if(!canAdd) continue;
		
		
		canAdd=true;
        int bo=0;
        for (i=0;i<size-1;i++)
        {
            bo=getBondOrderbyAtomIndex(ring->atomIdList[i],ring->atomIdList[i+1]);
            if(bo==5 || bo==7 || bo<2 )  {
               canAdd=false; break;
            }
        }
		if(!canAdd) continue;
		bo=getBondOrderbyAtomIndex(ring->atomIdList[0],ring->atomIdList[size-1]);
		if(bo==5 || bo==7 || bo<2 )    continue;
        

        //aromatic?
        bool aromatic=true;
        for (i=0;i<size;i++)
        {
            if(getAtombyIndex(ring->atomIdList[i])->isAromatic()) {
                //cout << ring->atomIdList[i]+1 <<" : " << "aromatic!"<<endl;
                continue;
            }

            aromatic=false;
            //cout << ring->atomIdList[i]+1 <<" : " << "not aromatic!"<<endl;
            break;
        }

        if(!aromatic) {
            delete ring;
            continue;
        }

        ring->size=ring->atomIdList.size();
        ring->radius=radius;
        ring->scale=1.0;
        ring->opacity=1.0;
        ring->type=0;
        ring->color.Set(0.65,0.65,0.65);
        ring->isAromatic=aromatic;
        ringList.push_back(ring);
    }

    //displayRingList();

    //update bond information
    if (bondList.size() <3) return;
    if (ringList.size()<1) return;

    vector<unsigned int>::iterator it,it1;
    for (j=0;j<bondList.size();j++)
    {
        for (k=0;k<ringList.size();k++)
        {
            it=std::find(ringList[k]->atomIdList.begin(),
                         ringList[k]->atomIdList.end(),
                         bondList[j]->atomIndex0());
            if(it==ringList[k]->atomIdList.end()) continue;

            it1=std::find(ringList[k]->atomIdList.begin(),
                          ringList[k]->atomIdList.end(),
                          bondList[j]->atomIndex1());
            if(it1==ringList[k]->atomIdList.end()) continue;


            bondList[j]->setPlane(ringList[k]->norm);

            if(bondList[j]->getBondOrder()<2) continue;
            bondList[j]->appendRingId(k);
            bondList[j]->setAromatic(true);
        }
    }
}


void HMol::addRing (vector <uint> idList,
                    vector3 ringCenter, vector3 ringNormal,
                    double ringRadius,double ringScale,
                    vector3 color, bool ringAromatic)
{
    if (idList.size()<3)
        return;

    HRing * ring = new HRing();
    ring->atomIdList.clear();

    for(unsigned int i=0; i<idList.size();i++)
        ring->atomIdList.push_back( idList[i]);

    ring->norm.Set(ringNormal.x(),ringNormal.y(),ringNormal.z());
    ring->center.Set(ringCenter.x(),ringCenter.y(),ringCenter.z());
    ring->radius=ringRadius;
    ring->type=0;
    ring->opacity=1.0;
    ring->color=color;
    ring->scale=ringScale;
    ring->isAromatic=ringAromatic;

    ringList.push_back(ring);
}

void HMol::addRing(vector <uint> idList)
{
    unsigned int size=idList.size();

    HRing * ring = new HRing();
    ring->atomIdList.clear();

    vector3 center=VZero;
    for(unsigned int j = 0; j < size; j++) {
        ring->atomIdList.push_back(idList[j]);
        center=center+getAtomPosbyIndex(idList[j]);
    }
    center=center/size;

    vector3 norm=VZ;
    norm=cross(getAtomPosbyIndex(idList[0])-getAtomPosbyIndex(idList[1]),
            getAtomPosbyIndex(idList[0])-getAtomPosbyIndex(idList[2]));
    norm.normalize();

    ring->norm.Set(norm.x(),norm.y(),norm.z());
    ring->center.Set(center.x(),center.y(),center.z());
    ring->scale=1.0;
    ring->opacity=1.0;
    ring->type=0;
    ring->color.Set(0.65,0.65,0.65);

    //radius
    double radius=50.0;
    for (unsigned int i=0;i<size;i++)
    {
        double distToCenter=(getAtomPosbyIndex(idList[i])-center).length();
        if(distToCenter < radius)
            radius = distToCenter;
    }

    bool aromatic=true;

    /*
    //aromatic?
    for (unsigned int i=0;i<size;i++)
    {
        if(getAtombyIndex(ring->atomIdList[i])->isAromatic()) {
            //cout << ring->atomIdList[i]+1 <<" : " << "aromatic!"<<endl;
            continue;
        }

        aromatic=false;
        //cout << ring->atomIdList[i]+1 <<" : " << "not aromatic!"<<endl;
        break;
    }

    if(!aromatic) {   delete ring;      return;     }
*/
    ring->size=ring->atomIdList.size();
    ring->radius=radius;
    ring->isAromatic=aromatic;
    ringList.push_back(ring);


    //update bond information
    unsigned int k=ringList.size()-1;
    for (unsigned int i=0;i<size-1;i++)
        getBondbyAtomIndex(idList[i],idList[i+1])->appendRingId(k) ;
    getBondbyAtomIndex(idList[0],idList[size-1])->appendRingId(k) ;
}





void HMol::displayRingList()
{
    unsigned int size=numRings();
    if (size<1) return;

    cout << endl;
    cout << "Rings inside molecule:"<<endl;
    for (unsigned int k=0;k<size;k++)
    {
        cout << k+1<< " "<< ringList[k]->atomIdList.size() << ": ";
        for (unsigned int l=0;l<ringList[k]->atomIdList.size();l++)
            cout << ringList[k]->atomIdList[l]+1<< " ";
        cout << ", normal :"<< ringList[k]->norm<< " ";
        cout << ", center :"<< ringList[k]->center<< " ";
        cout << ", isAromatic:"<<ringList[k]->isAromatic<< " ";

        cout << endl;
    }
    cout << endl;
}


void HMol::displayArcList()
{
    unsigned int size=numRings();
    if (size<1) return;

    cout << endl;
    cout << "Arcs inside molecule:"<<endl;
    for (unsigned int k=0;k<size;k++)
    {
        cout << k+1<< " "<< arcList[k]->atomIdList.size() << ": ";
        for (unsigned int l=0;l<arcList[k]->atomIdList.size();l++)
        {
            cout << arcList[k]->atomIdList[l]+1<< " ";
        }
        cout << endl;
    }
    cout << endl;
}




//trans to OBMol
void HMol::buildOBMol()
{
    obmol.Clear();
    obmol.BeginModify();

    // X atom is not existed
    if(isXExisted())
        return;


    unsigned int idx=0;
    foreach(HAtom * atom, atomList)
    {
        OpenBabel::OBAtom *a = obmol.NewAtom();

        a->Clear();
        a->SetVector(atom->x(), atom->y(), atom->z());
        a->SetAtomicNum(atom->atomicNum());
        a->SetType(atom->Symbol().toStdString().c_str());
        a->SetId(idx+1);

        //cout << obmol.NumAtoms()<<endl;

        idx++;
    }

    //copy bonds
    if (NumBonds()>0) {
        QVector<HBond *>::iterator iter1;
        for (iter1=bondList.begin();iter1!=bondList.end();iter1++)
        {
            //5=aromatic inside openbabel
            unsigned int bo=(*iter1)->getBondOrder();
            if (bo==6) bo=5;
            obmol.AddBond((*iter1)->atomIndex0() + 1,
                          (*iter1)->atomIndex1() + 1, bo);
        }
    }

    obmol.EndModify(true);
    return;


    /*
    // X atom existed
    //filter X atoms
    //removeAllXAtoms();

    for(unsigned int i=0;i<NumAtoms();i++)
    {
        if(getAtomSymbol(i)!="X") continue;


        HXAtom * xatom=new HXAtom;
        xatom->id=i;
        xatom->pos=getAtomPosbyIndex(i);

        for(unsigned int j=0;j<getAtombyIndex(i)->numNeighbors();j++)
            xatom->neighbor.push_back(getAtombyIndex(i)->getNeighborId(j));

        for(unsigned int j=0;j<NumAtoms();j++)
        {
            xatom->atomIdList.push_back(j);
        }
    }

    unsigned int idx=-1;
    unsigned int times=0;
    vector <unsigned int > idX;
    notXIdx.clear();
    foreach(HAtom * atom, atomList)
    {
        idx+=1;
        if(atom->Symbol()=="X")  {
            idX.push_back(idx);
            continue;
        }
        notXIdx.push_back(idx);

        OpenBabel::OBAtom *a = obmol.NewAtom();
        a->Clear();
        a->SetVector(atom->x(), atom->y(), atom->z());
        a->SetAtomicNum(atom->atomicNum());
        a->SetType(atom->Symbol().toStdString().c_str());
        a->SetId(idx+1-idX.size());
    }


    QVector<HBond *>::iterator iter1;
    for (iter1=bondList.begin();iter1!=bondList.end();iter1++)
    {
        //5=aromatic inside openbabel
        unsigned int bo=(*iter1)->getBondOrder();
        if (bo==6) bo=5;
        obmol.AddBond((*iter1)->atomIndex0() + 1,
                      (*iter1)->atomIndex1() + 1, bo);
    }


    obmol.EndModify(true);

    */


    //obmol.SetTotalSpinMultiplicity(1);
    // obmol.SetPartialChargesPerceived();
}

bool HMol::writeMol(const QString &fileName, const QString &fileType)
{
    // Check is we are replacing an existing file
    QFile file(fileName);
    bool replaceExistingFile = file.exists();

    if (!file.open(QFile::WriteOnly | QFile::Text))
        return false;

    file.close();

    QString newFileName = fileName;
    if (replaceExistingFile)
    {
        newFileName += ".new";
        QFile newFile(newFileName);
        if (!newFile.open(QFile::WriteOnly | QFile::Text)) {
            return false;
        }
        newFile.close();
    }

    // Construct the OpenBabel objects, set the file type
    OBConversion conv;
    OBFormat *outFormat;
    if (!fileType.isEmpty() && !conv.SetOutFormat(fileType.toLatin1()))
        return false;
    else {
        outFormat = conv.FormatFromExt(fileName.toLatin1());
        if (!outFormat || !conv.SetOutFormat(outFormat))
            return false;
    }

    // Now attempt to write the molecule in
    ofstream ofs;
    ofs.open(newFileName.toLocal8Bit()); // This handles utf8 file names etc
    if (!ofs) {// Should not happen, already checked file could be opened
        return false;
    }

    buildOBMol();

    OpenBabel::OBChainsParser chainparser;
    obmol.UnsetFlag(OB_CHAINS_MOL);
    chainparser.PerceiveChains(obmol);

    if (conv.Write(&obmol, &ofs)) {
        ofs.close();
        if (replaceExistingFile) {
            QFile newFile(newFileName);
            bool success;
            success = file.rename(fileName + ".old");
            if (success) {
                // Leave to ensure we work around a bug in Qt < 4.5.1
                file.setFileName(fileName + ".old");
                success = newFile.rename(fileName);
            }
            else {
                return false;
            }
            if (success) // renaming worked
                success = file.remove(); // remove the old file: WARNING -- would much prefer to just rename, but Qt won't let you
            else {
                return false;
            }

            if (success) {
                return true;
            }
            else {
                return false;
            }
        }
        else // No need for all that - this is a new file in an empty location
            return true;
    }
    else {
        return false;
    }
    // Assume something went wrong if we did not return true earlier
    //qDebug() << "OBWrapper should never get here...";
    return false;
}


bool HMol::fromOBMol()
{
    clearAll();

    // Begin by copying all of the atoms
    std::vector<OpenBabel::OBAtom*>::iterator i;
    for (OpenBabel::OBAtom *obatom = obmol.BeginAtom(i); obatom; obatom = obmol.NextAtom(i))
    {
        HAtom * atom = nullptr;

        //avoiding NaN
        if( isfinite(obatom->GetX()) )
            atom = new HAtom(obatom->GetAtomicNum(),obatom->GetX(),obatom->GetY(),obatom->GetZ());
        else
            atom = new HAtom(obatom->GetAtomicNum(),0.0,0.0,0.0);

        //cout << atom->radius()<<endl;
        if(atom)
            atomList.push_back(atom);
    }

    // Now bonds, we use the indices of the atoms to get the bonding right
    std::vector<OpenBabel::OBBond*>::iterator j;
    for (OpenBabel::OBBond *obbond = obmol.BeginBond(j); obbond; obbond = obmol.NextBond(j))
    {
        unsigned int i=obbond->GetBeginAtom()->GetIdx()-1;
        unsigned int j=obbond->GetEndAtom()->GetIdx()-1;
        HBond * bond = new HBond(i,j, obbond->GetBondOrder());

        if(obbond->GetBondOrder()==5)   bond->setBondOrder(6);
        else        bond->setBondOrder(obbond->GetBondOrder());

        bondList.push_back(bond);

        getAtombyIndex(i)->appendLinkdedAtomId(j);
        getAtombyIndex(j)->appendLinkdedAtomId(i);
    }

    //perceiveAllAtomHybridization();

    perceiveRings();
    perceivePlaneBonds();

    return true;
}


//mass-weighted
void HMol::perceptMoleculeShape()
{
    centralize();

    double Ixx=0.0, Ixy=0.0, Ixz=0.0,
            Iyy=0.0, Iyz=0.0,
            Izz=0.0;
    double x, y, z;
    //                  | Ixx     Ixy     Ixz |
    //          I   =   | Iyx     Iyy     Iyz |
    //                  | Izx     Izy     Izz |
    foreach(HAtom * atom, atomList)
    {
        x = atom->x();
        y = atom->y();
        z = atom->z();

        Ixx += (y*y + z*z)*atom->Mass();
        Iyy += (x*x + z*z)*atom->Mass();
        Izz += (x*x + y*y)*atom->Mass();
        Ixy -= (x*y)*atom->Mass();
        Ixz -= (x*z)*atom->Mass();
        Iyz -= (y*z)*atom->Mass();
    }

    double I[9] = {Ixx,Ixy,Ixz,Ixy,Iyy,Iyz,Ixz,Iyz,Izz};

    matrix3x3 InertialMomentMatrix(I);
    vector3 IMoment;
    IMomentMatrix= InertialMomentMatrix.findEigenvectorsIfSymmetric(IMoment);


    double a=IMoment[0],
            b=IMoment[1],
            c=IMoment[2];


    double eps=2.0*c/1000.0; //0.3% error
    cout << "The tolerance of InertialMoment:  " << eps << endl;


    //Ia < Ib < Ic
    molShape="Irregular";

    //Sphere: Ia = Ib = Ic
    if ( (abs(a-b) < eps) && (abs(b-c) < eps) && (abs(a-c) < eps) )
        molShape="Sphere";

    //Line   : Ia =0.0 and Ia < Ib = Ic
    else if ( abs(a)< eps)
        molShape="Line";

    //Prolate: Ia < Ib = Ic
    else if ( abs(b-c)< eps && (a < c) && abs(a-c) > eps )
        molShape="Prolate";


    //Polygon an Plane: Ia=Ib and Ia+Ib=Ic
    else if ( abs(a+b-c) < eps)
    {
        if ( abs(a-b)< eps ) // Ia=Ib
            molShape="Polygon";
        else
            molShape="Plane";  //Ia!=Ib
    }

    //Oblate:  Ia = Ib < Ic
    else if ( abs(a-b)< eps && (b<c) )
        molShape="Oblate";
}




void HMol::calcMolVolume()
{
    double x=0.0,y=0.0,z=0.0;
    double x0=0.0,y0=0.0,z0=0.0;

    for (unsigned int i=0;i<NumAtoms();i++)
    {
        x0 = fabs(getAtomXbyIndex(i));
        y0 = fabs(getAtomYbyIndex(i));
        z0 = fabs(getAtomZbyIndex(i));

        if (x < x0) x=x0;
        if (y < y0) y=y0;
        if (z < z0) z=z0;
    }

    //cout << x <<" "<< y <<" "<< z <<" inside calcMolVolume()"<<endl;
    xyzLength.Set(x,y,z);
    molLength=xyzLength.length();
}



void HMol::filterOrder_reverse(vector <uint> & _list)
{
    sort ( _list.begin(), _list.end());
    vector <uint> :: iterator iter = unique ( _list.begin(), _list.end());
    _list.erase(iter,_list.end());
    sort(_list.rbegin(), _list.rend());
}

void HMol::filterOrder(vector <uint> & _list)
{
    sort ( _list.begin(), _list.end());
    vector <uint> :: iterator iter = unique ( _list.begin(), _list.end());
    _list.erase(iter,_list.end());
    sort(_list.begin(), _list.end());
}


bool HMol::isMetal(unsigned int idx0)
{
    int num=getAtombyIndex(idx0)->atomicNum();

    if(num<3) return false;
    if(num>=5 && num<=10) return false;
    //if(num>10 && num<14) return true;
    if(num>=14 && num<19) return false;
    if(num==35 || num==53) return false;
    return true;
}


void HMol::findChildren(vector<int> &children,int first,int second)
{
    children.clear();

    //check it is terminal one or not?
    if(getAtombyIndex(second)->numLinkdedAtoms()==1){
        children.push_back(first);
        return;
    }

    buildOBMol();


    obmol.FindChildren(children,first+1,second+1);

    if(children.size()<1) return;
    for(unsigned int i=0;i<children.size();i++)
        children[i]=children[i]-1;
}

void HMol::findFragment(unsigned int idx0, vector<uint> & _ll)
{
    _ll.clear();

    _ll.push_back(idx0);
    uint from=0,to=1,from0=0;
    int id;

    while(1)
    {
        from0=to; //backup

        for(uint i=from;i<to;i++)
        {
            for(uint j=0; j<getAtombyIndex(_ll[i])->numNeighbors();j++)
            {
                id=getAtombyIndex(_ll[i])->getNeighborId(j);

                if(isMetal(id)) continue;


                if (find(_ll.begin(), _ll.end(), id) == _ll.end())
                    _ll.push_back(id);
            }
        }

        if(_ll.size()==to) break;

        from=from0;
        to=_ll.size();
    }

    filterOrder(_ll);


    //maybe there exists large one
    vector <uint>  _lll; _lll.clear();
    _lll.push_back(idx0);
    from=0;
    to=1;
    while(1)
    {
        from0=to; //backup

        for(uint i=from;i<to;i++)
        {
            for(uint j=0; j<getAtombyIndex(_lll[i])->numNeighbors();j++)
            {
                id=getAtombyIndex(_lll[i])->getNeighborId(j);
                if (find(_lll.begin(), _lll.end(), id) == _lll.end())
                    _lll.push_back(id);
            }
        }

        if(_lll.size()==to) break;

        from=from0;
        to=_lll.size();
    }


    filterOrder(_lll);

    if(_lll.size()<NumAtoms())
    {
        _ll.resize(_lll.size());
        for(uint i=0;i<_lll.size();i++)  _ll[i]=_lll[i];
    }
}

//do not contains idx0 itself
void HMol::findLargestFragment_1(unsigned int idx0, unsigned int idx1, vector<uint> & _ll)
{
    _ll.clear();
    int id;

    //the first time
    for(uint j=0; j<getAtombyIndex(idx0)->numNeighbors();j++)
    {
        id=getAtombyIndex(idx0)->getNeighborId(j);
        if(id==idx1) continue;
         _ll.push_back(id);
    }


    uint from=0,to=_ll.size(),from0=0;

    while(1)
    {
        from0=to; //backup

        for(uint i=from;i<to;i++)
        {
            for(uint j=0; j<getAtombyIndex(_ll[i])->numNeighbors();j++)
            {
                id=getAtombyIndex(_ll[i])->getNeighborId(j);

                //skip
                if(id==idx0) continue;

                //failed, if find idx1
                if(id==idx1) {
                    _ll.clear();
                    return ;
                }


                if (find(_ll.begin(), _ll.end(), id) == _ll.end())
                    _ll.push_back(id);

            }
        }

        if(_ll.size()==to) break;

        from=from0;
        to=_ll.size();
    }

    filterOrder(_ll);
}


void HMol::findLargestFragment(unsigned int idx0,  vector<uint> & _ll)
{
    _ll.clear();
    _ll.push_back(idx0);

    uint from=0,to=1,from0=0;
    int id;

    while(1)
    {
        from0=to; //backup

        for(uint i=from;i<to;i++)
        {
            for(uint j=0; j<getAtombyIndex(_ll[i])->numNeighbors();j++)
            {
                id=getAtombyIndex(_ll[i])->getNeighborId(j);

                if (find(_ll.begin(), _ll.end(), id) == _ll.end())
                    _ll.push_back(id);
            }
        }

        if(_ll.size()==to) break;

        from=from0;
        to=_ll.size();
    }

    filterOrder(_ll);
}

void HMol::findLargestFragment(unsigned int idx0, unsigned int idx1, vector<uint> & _ll)
{
    _ll.clear();
    _ll.push_back(idx0);

    int id;

    //the first time
    for(uint j=0; j<getAtombyIndex(idx0)->numNeighbors();j++)
    {
        id=getAtombyIndex(idx0)->getNeighborId(j);
        if(id==idx1) continue;
         _ll.push_back(id);
    }

    uint from=1,to=_ll.size(),from0=0;

    while(1)
    {
        from0=to; //backup

        for(uint i=from;i<to;i++)
        {
            for(uint j=0; j<getAtombyIndex(_ll[i])->numNeighbors();j++)
            {
                id=getAtombyIndex(_ll[i])->getNeighborId(j);

                //failed, if find idx1
                if(id==idx1) {
                    _ll.clear();
                    return ;
                }

                if (find(_ll.begin(), _ll.end(), id) == _ll.end())
                    _ll.push_back(id);
            }
        }

        if(_ll.size()==to) break;

        from=from0;
        to=_ll.size();
    }

    filterOrder(_ll);
}

void HMol::displayBonds()
{
    cout << "\n\nBonds of the molecule:"<<endl;
    for (unsigned int i=0;i<NumBonds();i++) {
        cout << i+1 <<": "<<getBondbyIndex(i)->atomIndex0()+1<< "<->";
        cout << getBondbyIndex(i)->atomIndex1()+1<<"==";
        cout << getBondbyIndex(i)->getBondOrder()<<"  ";
        cout << getBondbyIndex(i)->getPlane()<< "  ";
        for(auto j:getBondbyIndex(i)->getRingId())
            cout << j << "  "<<endl;
        cout << endl;
    }
    cout << endl<< endl;
}

void HMol::displayAtoms()
{
    unsigned int numAtoms=NumAtoms();
    double x,y,z;
    for (unsigned int i=0;i<numAtoms;i++)
    {
        x=getAtomXbyIndex(i);
        y=getAtomYbyIndex(i);
        z=getAtomZbyIndex(i);

        cout << i+1 <<": "<<getAtomSymbol(i).toStdString().c_str()<< " "<<x << " "<<y<<"  "<<z<< ":  ";
        for (unsigned int j=0;j<getAtombyIndex(i)->numLinkdedAtoms();j++)
            cout << getAtombyIndex(i)->getLinkdedAtomId(j)+1<<"    ";
        cout << endl;
    }
}


//put to a empty place
void HMol::addAtomfromTemplate(templateMol * tmol, vector3 p)
{
    unsigned int numAtomsT=tmol->numAtoms();
    //unsigned int numBondsT=tmol->numBonds();

    unsigned int numAtoms=NumAtoms();
    //unsigned int numBonds=NumBonds();

    //cout << "hit position: "<<p.x() <<" "<<p.y() <<" "<<p.z() <<endl;
    //cout << "Atoms and Bonds of Current Molcule "<<endl;
    //cout << numAtoms <<" "<<numBonds<<endl;
    //cout << "Atoms and Bonds of Template "<<endl;
    //cout << numAtomsT <<" "<<numBondsT<<endl;

    //atom existed
    if(numAtoms>0)
    {
        //check very near atom
        double minX=10.0,minY=10.0,minZ=10.0,
                maxX=-10.0,maxY=-10.0,maxZ=-10.0;

        double tminX=10.0,tminY=10.0,tminZ=10.0,
                tmaxX=-10.0,tmaxY=-10.0,tmaxZ=-10.0;

        double distX,distY,distZ;

        double x,y,z;


        //get max and min x,y,z of molecule
        //vector3 mCenter=VZero;
        for (unsigned int i=0;i<numAtoms;i++)
        {
            //cout <<i+1<<endl;
            //mCenter += atomPos(i);

            x=getAtomXbyIndex(i);
            y=getAtomYbyIndex(i);
            z=getAtomZbyIndex(i);

            if(x < minX) minX=x;
            if(y < minY) minY=y;
            if(z < minZ) minZ=z;

            if(x > maxX) maxX=x;
            if(y > maxY) maxY=y;
            if(z > maxZ) maxZ=z;
        }


        //get max and min x,y,z of template
        vector3 v;
        for (unsigned int i=0;i<numAtomsT;i++)
        {
            v=tmol->getPos(i) + p;
            x=v.x();
            y=v.y();
            z=v.z();

            if(x < tminX) tminX=x;
            if(y < tminY) tminY=y;
            if(z < tminZ) tminZ=z;

            if(x > tmaxX) tmaxX=x;
            if(y > tmaxY) tmaxY=y;
            if(z > tmaxZ) tmaxZ=z;
        }

        distX= 2.0+fabs(tminX - maxX);
        distY= 2.0+fabs(tminY - maxY);
        distZ= 2.0+fabs(tminZ - maxZ);

        if (distX < distY  && distX < distZ)
            p.SetX(distX+p.x());

        if (distY < distX  && distY < distZ)
            p.SetY(distY+p.y());

        if (distZ < distX  && distZ < distY)
            p.SetZ(distZ+p.z());


        for (unsigned int i=0;i<numAtomsT;i++)
            addAtom(tmol->getSymbol(i),tmol->getPos(i)+p);

        unsigned int beg=numAtoms;
        for (unsigned int i=0;i<tmol->numBonds();i++)
        {
            unsigned int idx0=tmol->getBond(i)->atomIndex0()+beg;
            unsigned int idx1=tmol->getBond(i)->atomIndex1()+beg;

            addBond(idx0,idx1,tmol->getBond(i)->getBondOrder());

            //getAtombyIndex(idx0)->appendLinkdedAtomId(idx1);
            //getAtombyIndex(idx1)->appendLinkdedAtomId(idx0);
        }
    }
    else {//empty molecule,  copy
        for (unsigned int i=0;i<numAtomsT;i++)
            addAtom(tmol->getSymbol(i),tmol->getPos(i)+p);

        for (unsigned int i=0;i<tmol->numBonds();i++) {
            unsigned int idx0=tmol->getBond(i)->atomIndex0();
            unsigned int idx1=tmol->getBond(i)->atomIndex1();
            addBond(idx0,idx1,tmol->getBond(i)->getBondOrder());

            //getAtombyIndex(idx0)->appendLinkdedAtomId(idx1);
            //getAtombyIndex(idx1)->appendLinkdedAtomId(idx0);
        }

    }

    //recalculate bond plane
    perceivePlaneRingforBonds();

    displayAtoms();
}



void HMol::linkTemplate2Mol_1(templateMol * tmol, unsigned int idx0, unsigned int idx0_t)
{
    unsigned int idx1=getAtombyIndex(idx0)->getLinkdedAtomId(0);
    vector3 vec1=getAtomPosbyIndex(idx0)-getAtomPosbyIndex(idx1);

    //linked atom of molecule (the Hydrogen must be replaced)
    double radius1 = getAtombyIndex(idx1)->radius();


    vector3 vec2=VZero;
    int num=tmol->getAtombyIndex(idx0_t)->numNeighbors();
    double radius2 = tmol->getAtombyIndex(idx0_t)->radius();

    for(int k=0;k<num;k++) {
        vec2 += tmol->getAtomPosbyIndex(idx0_t)
                -tmol->getAtomPosbyIndex(tmol->getAtombyIndex(idx0_t)->getLinkdedAtomId(k));
    }
    vec2/=-double(num);

    vec1.normalize();
    vec2.normalize();

    //return;

    //remove the hit hydrogen of current molecule
    //cout << idx0+1<< "  "<< idx1+1 <<" inside linkTemplate2Mol::linkTemplate2Mol"<<endl;
    //cout << idx0_t+1<< "  "<< idx1_t+1 <<" inside linkTemplate2Mol::linkTemplate2Mol"<<endl;

    double dist=radius1+radius2;

    cout << dist << "  "<<radius1 << "  "<< radius2<<endl;
    vector3 p1=getAtomPosbyIndex(idx1);
    vector3 trans = 1.5*vec1*dist+p1;

    //vec2=tmol->getAtomPosbyIndex(idx1_t)-tmol->getAtomPosbyIndex(idx0_t);
    //vec2.normalize();
    double angle= vectorAngle(vec2, vec1);


/*
   //cout << vec2.x()<< " "<< vec2.y()<< " "<< vec2.z()<< endl;
   //cout << vec1.x()<< " "<< vec1.y()<< " "<< vec1.z()<< endl;

   cout <<  " from: " << vec2.x()<< " "<< vec2.y()<< " "<< vec2.z()<< endl;
   cout <<  " to: "<< vec1.x()<< " "<< vec1.y()<< " "<< vec1.z()<< endl;
   cout <<  " with angle :"<<  angle << endl<< endl;

   cout <<  endl<< endl;
   cout << idx0+1<< "  "<< idx1+1 <<" inside linkTemplate2Mol"<<endl;
   cout << idx0_t+1<< "   inside linkTemplate2Mol"<<endl;

   vector3 d0=getAtomPosbyIndex(idx0),
           d1=getAtomPosbyIndex(idx1);

   vector3 e0=tmol->getAtomPosbyIndex(idx0_t);
           //e1=tmol->getAtomPosbyIndex(idx1_t);

   cout <<"\n\n";

   cout <<"molecule:\n";
   cout <<   d0<< endl;
   cout <<   d1<< endl;

   cout <<"template:\n";
   cout <<   e0<< endl;
   cout <<   vec2<< endl;
*/
   //cout << "\n remove the hydrogen of molecule inside HMol::linkTemplate2Mol " << idx0 <<endl;


    //return;

    removeAtombyId(idx0);

    //updateAllAtomLinker();
    //displayAtoms();
    //displayBonds();


    unsigned int beg=NumAtoms();

    vector3 a, b, p0;
    p0=vec2+tmol->getAtomPosbyIndex(idx0_t);
    if (angle < 3.0)
    {
        //cout << "positive  "<<endl;
        for (unsigned int i=0;i<tmol->numAtoms();i++)
        {
            a=tmol->getAtomPosbyIndex(i)-p0;
            addAtom(tmol->getSymbol(i),a+trans);
        }
    }
    else if ( fabs (angle-180.0) < 3.0)
    {
        for (unsigned int i=0;i<tmol->numAtoms();i++)
        {
            a=tmol->getAtomPosbyIndex(i)-p0;
            addAtom(tmol->getSymbol(i),-a+trans);
        }
    }
    else {
        matrix3x3 m;
        m.SetupRotateMatrix(vec2, vec1);

        //  cout <<m(0,0)<< " "<< m(0,1)<< " "<< m(0,2)<< endl;
        //  cout <<m(1,0)<< " "<< m(1,1)<< " "<< m(1,2)<< endl;
        //  cout <<m(2,0)<< " "<< m(2,1)<< " "<< m(2,2)<< endl;

        for (unsigned int i=0;i<tmol->numAtoms();i++)
        {
            a=tmol->getAtomPosbyIndex(i)-p0;
            addAtom(tmol->getSymbol(i),m*a+trans);
        }
    }




    // cout << "adding " <<tmol->numAtoms()-1 << " atoms from template"<<endl;
    // cout << "there are "<< NumAtoms()<< " atoms now!"<<endl;



    //the new bond between two fragment
    addBond(idx1,beg+idx0_t,1);

    unsigned int id0, id1;

    for (unsigned int j=0;j<tmol->numBonds();j++)
    {
        id0=tmol->getBond(j)->atomIndex0();
        id1=tmol->getBond(j)->atomIndex1();

        id0=beg+id0;
        id1=beg+id1;

        addBond(id0,id1,tmol->getBond(j)->getBondOrder());
    }
    updateAllAtomLinker();

    perceivePlaneRingforBonds();
}



void HMol::linkTemplate2Mol(templateMol * tmol, unsigned int idx0, unsigned int idx0_t)
{
    if(getAtombyIndex(idx0)->numLinkdedAtoms()<1) return;

    if(tmol->getAtombyIndex(idx0_t)->atomicNum()!=1) {
        linkTemplate2Mol_1(tmol, idx0,  idx0_t);
        return;
    }

    //  unsigned int numAtomsT=tmol->numAtoms();
    //  unsigned int numBondsT=tmol->numBonds();

    //  unsigned int numAtoms=NumAtoms();
    //  unsigned int numBonds=NumBonds();


    unsigned int idx1=getAtombyIndex(idx0)->getLinkdedAtomId(0);
    vector3 vec1=getAtomPosbyIndex(idx0)-getAtomPosbyIndex(idx1);

    //linked atom of molecule (the Hydrogen must be replaced)
    double radius1 = getAtombyIndex(idx1)->radius();


    //the linker of template must be hydrogen

    //find the linked atom of Template


    unsigned int  idx1_t=tmol->getAtombyIndex(idx0_t)->getLinkdedAtomId(0); //only one bonded atom
    double radius2 = tmol->getAtombyIndex(idx1_t)->radius();
    vector3 vec2=tmol->getAtomPosbyIndex(idx1_t)-tmol->getAtomPosbyIndex(idx0_t);

    //pseudo atom X/Bq
    if (radius2<0.5) radius2=2.5;

    vec1.normalize();
    vec2.normalize();


    //remove the hit hydrogen of current molecule
    // cout << idx0+1<< "  "<< idx1+1 <<" inside linkTemplate2Mol::linkTemplate2Mol"<<endl;
    //cout << idx0_t+1<< "  "<< idx1_t+1 <<" inside linkTemplate2Mol::linkTemplate2Mol"<<endl;


    double dist=radius1+radius2;
    vector3 p1=getAtomPosbyIndex(idx1);
    vector3 trans = vec1*dist+p1;


    vec2=tmol->getAtomPosbyIndex(idx1_t)-tmol->getAtomPosbyIndex(idx0_t);
    vec2.normalize();
    double angle= vectorAngle(vec2, vec1);


    /*
   //cout << vec2.x()<< " "<< vec2.y()<< " "<< vec2.z()<< endl;
   //cout << vec1.x()<< " "<< vec1.y()<< " "<< vec1.z()<< endl;


   cout <<  " from: " << vec2.x()<< " "<< vec2.y()<< " "<< vec2.z()<< endl;
   cout <<  " to: "<< vec1.x()<< " "<< vec1.y()<< " "<< vec1.z()<< endl;
   cout <<  " with angle :"<<  angle << endl<< endl;




   cout <<  endl<< endl;
   cout << idx0+1<< "  "<< idx1+1 <<" inside linkTemplate2Mol"<<endl;
   cout << idx0_t+1<< "  "<< idx1_t+1 <<" inside linkTemplate2Mol"<<endl;



   vector3 d0=getAtomPosbyIndex(idx0),
           d1=getAtomPosbyIndex(idx1);

   vector3 e0=tmol->getAtomPosbyIndex(idx0_t),
           e1=tmol->getAtomPosbyIndex(idx1_t);

   cout <<"\n\n";

   cout <<"molecule:\n";
   cout <<   d0.x()<< " "<< d0.y()<< " "<< d0.z()<< endl;
   cout <<   d1.x()<< " "<< d1.y()<< " "<< d1.z()<< endl;

   cout <<"template:\n";
   cout <<   e0.x()<< " "<< e0.y()<< " "<< e0.z()<< endl;
   cout <<   e1.x()<< " "<< e1.y()<< " "<< e1.z()<< endl;

   //cout << "\n remove the hydrogen of molecule inside HMol::linkTemplate2Mol " << idx0 <<endl;
*/

    removeAtombyId(idx0);

    //updateAllAtomLinker();
    //displayAtoms();
    //displayBonds();

    unsigned int beg=NumAtoms();

    vector3 a, b;
    vector3 p0=tmol->getAtomPosbyIndex(idx1_t);
    if ( angle < 3.0)
    {
        //cout << "positive  "<<endl;
        for (unsigned int i=0;i<tmol->numAtoms();i++)
        {
            if (i==idx0_t) continue;
            a=tmol->getAtomPosbyIndex(i)-p0;
            addAtom(tmol->getSymbol(i),a+trans);
        }
    }
    else if ( fabs (angle-180.0) < 3.0)
    {
        //cout << "negtive  "<<endl;
        for (unsigned int i=0;i<tmol->numAtoms();i++)
        {
            if (i==idx0_t) continue;
            a=tmol->getAtomPosbyIndex(i)-p0;
            addAtom(tmol->getSymbol(i),-a+trans);
        }
    }
    else {
        //cout << "normal angle"<<endl;
        matrix3x3 m;
        m.SetupRotateMatrix(vec2, vec1);

        //  cout <<m(0,0)<< " "<< m(0,1)<< " "<< m(0,2)<< endl;
        //  cout <<m(1,0)<< " "<< m(1,1)<< " "<< m(1,2)<< endl;
        //  cout <<m(2,0)<< " "<< m(2,1)<< " "<< m(2,2)<< endl;

        for (unsigned int i=0;i<tmol->numAtoms();i++)
        {
            //skip the hydrogen labelled to linker
            if (i==idx0_t) continue;
            a=tmol->getAtomPosbyIndex(i)-p0;
            addAtom(tmol->getSymbol(i),m*a+trans);
        }
    }




    // cout << "adding " <<tmol->numAtoms()-1 << " atoms from template"<<endl;
    // cout << "there are "<< NumAtoms()<< " atoms now!"<<endl;




    //the new bond between two fragment
    if (idx1_t > idx0_t)
        addBond(idx1,beg+idx1_t-1,1);
    else
        addBond(idx1,beg+idx1_t,1);

    unsigned int id0,id1,bo;
    for (unsigned int j=0;j<tmol->numBonds();j++)
    {
        id0=tmol->getBond(j)->atomIndex0();
        id1=tmol->getBond(j)->atomIndex1();

        //skip the bond of the highlight hydrogen
        if(id0==idx0_t) continue;
        if(id1==idx0_t) continue;

        if(id0 > idx0_t) id0=id0-1;
        if(id1 > idx0_t) id1=id1-1;

        id0=beg+id0;
        id1=beg+id1;

        bo=tmol->getBond(j)->getBondOrder();
        addBond(id0,id1,bo);
    }
    updateAllAtomLinker();

    perceivePlaneRingforBonds();
}

void HMol::perceivePlaneRingforBonds()
{
    perceiveRings();
    perceivePlaneBonds();
}

void HMol::updateAllAtomLinker()
{
    if(NumAtoms()<1) return;

    for (unsigned int i=0;i<NumAtoms();i++)
    {
        getAtombyIndex(i)->clearLinkedAtomId();
    }


    for (unsigned int j=0;j<NumBonds();j++)
    {
        unsigned int id0=getBondbyIndex(j)->atomIndex0();
        unsigned int id1=getBondbyIndex(j)->atomIndex1();

        getAtombyIndex(id0)->appendLinkdedAtomId(id1);
        getAtombyIndex(id1)->appendLinkdedAtomId(id0);
    }
}


void  HMol::updateAtomLinkerId(unsigned int idx)
{
    getAtombyIndex(idx)->clearLinkedAtomId();

    for (unsigned int j=0;j<NumBonds();j++)
    {
        unsigned int id0=getBondbyIndex(j)->atomIndex0();
        unsigned int id1=getBondbyIndex(j)->atomIndex1();

        if(id0==idx)
            getAtombyIndex(idx)->appendLinkdedAtomId(id1);
        if(id1==idx)
            getAtombyIndex(idx)->appendLinkdedAtomId(id0);
    }
}

void HMol::cleanMol()
{
    runMolecularMechanics();
}




void HMol::runXTB()
{
    /*
    QString path="/home/zhangfq/tmp/";
    QString fileName="zfq.xyz";
    system ("rm /home/zhangfq/tmp/* -rf ");

    //prepare xtb xyz
    QFile file(path+fileName);

    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    unsigned int numAtoms=NumAtoms();

    QString str2;
    str2.sprintf("%3d\n", numAtoms);
    file.write(str2.toUtf8());
    file.write("run xtb by zhangfq\n");

    double x,y,z;
    for (unsigned int i=0;i<numAtoms;i++)
    {
        QString asymbol=getAtomSymbol(i);
        x=getAtomXbyIndex(i);
        y=getAtomYbyIndex(i);
        z=getAtomZbyIndex(i);

        str2.sprintf("%3s %10.5f  %10.5f  %10.5f\n", asymbol.toStdString().c_str(), x,y,z);
        file.write(str2.toUtf8());
    }
    file.write("\n\n");
    file.close();

   */
}



void HMol::runMolecularMechanics(QString method)
{
    //prepare input file
    buildOBMol();

    unsigned int steps = 100;
    double criterion = 1e-5;
    unsigned int totalSteps = 0;


    OpenBabel::OBFFConstraints constraints;
    vector <uint> constraintIdList;


    string ff = method.toStdString();


    //UFF can be applicable for a wide variety of molecules
    auto pFF = OpenBabel::OBForceField::FindForceField(ff);
    if(!pFF) {
        cout << " OBForceField * pFF is invalid!"<<endl;
        return;
    }

    pFF->Setup(obmol,constraints);
    pFF->SetLogFile(&cout);
    pFF->SetLogLevel(OBFF_LOGLVL_MEDIUM);


    pFF->ConjugateGradientsInitialize(steps, criterion);
    bool done = true;
    while (done) {
        done = pFF->ConjugateGradientsTakeNSteps(1);
        totalSteps++;
        if(totalSteps>steps) break;
    }

    pFF->UpdateCoordinates(obmol);


    // Begin by copying all of the atoms
    std::vector<OpenBabel::OBAtom*>::iterator i;
    unsigned int j=0;
    for (OpenBabel::OBAtom *obatom = obmol.BeginAtom(i); obatom; obatom = obmol.NextAtom(i)) {
        getAtombyIndex(j)->setPos(obatom->GetX(),obatom->GetY(),obatom->GetZ());
        //cout << obatom->GetX() << " " << obatom->GetY()<< " " <<obatom->GetZ()<<endl;
        j++;
    }


    perceivePlaneRingforBonds();
}

void HMol::addatomfromQString(QString data)
{
    //cout << text.toStdString().c_str()<<endl;

    //if(!pastefromClipboard()) return;

    double x, y, z;
    QString  atomLabel,line;
    QStringList lst=data.split("\n"),single;

    for (unsigned int i=0;i<lst.size();i++)
    {
        line=lst[i].simplified();
        if (line.size()<5) continue;
        single=line.split(QRegularExpression("\\s+"));
        if (single.size()!=4) continue;

        if(isNumber(single[0]))
            atomLabel=QString::fromStdString(ElementNames[single[0].toInt()]);
        if(isEnglish(single[0]))
            atomLabel=single[0];

        x=single[1].toDouble();
        y=single[2].toDouble();
        z=single[3].toDouble();

        addAtom(atomLabel,x,y,z);
    }

    //perceiveBondOrder();
    //perceivePlaneRingforBonds();
}

QString HMol::getShift(QString data)
{
    //shift to avoid overlap
    removeBackupAtoms();

    vector3 shift=VZero;

    if(NumAtoms()<1)
        return data;

    vector3 p=VZero,Minxyzfromclip=VZero;
    vector3 endExistedAtoms=getAtomPosbyIndex(0);
    vector3 begExistedAtoms=getAtomPosbyIndex(0);

    if(NumAtoms() > 1){
        for (int i=1;i<NumAtoms();i++){
            p=getAtomPosbyIndex(i);
            //maxpos
            if(endExistedAtoms.x()<p.x()) endExistedAtoms.SetX(p.x());
            if(endExistedAtoms.y()<p.y()) endExistedAtoms.SetY(p.y());
            if(endExistedAtoms.z()<p.z()) endExistedAtoms.SetZ(p.z());
            //minpos
            if(begExistedAtoms.x()>p.x()) begExistedAtoms.SetX(p.x());
            if(begExistedAtoms.y()>p.y()) begExistedAtoms.SetY(p.y());
            if(begExistedAtoms.z()>p.z()) begExistedAtoms.SetZ(p.z());
        }
    }

    double x, y, z;
    QString  line;
    QStringList lst=data.split("\n"),single;

    line=lst[0].simplified();
    single=line.split(QRegularExpression("\\s+"));
    Minxyzfromclip.SetX(single[1].toDouble());
    Minxyzfromclip.SetY(single[2].toDouble());
    Minxyzfromclip.SetZ(single[3].toDouble());

    if(lst.size()>2)
        for (unsigned int i=1;i<lst.size();i++)
        {
            line=lst[i].simplified();
            if (line.size() < 2) continue;
            single=line.split(QRegularExpression("\\s+"));
            if (single.size()!=4) continue;

            x=single[1].toDouble();
            y=single[2].toDouble();
            z=single[3].toDouble();

            if(Minxyzfromclip.x()>x)
                Minxyzfromclip.SetX(x);

            if(Minxyzfromclip.y()>y)
                Minxyzfromclip.SetY(y);

            if(Minxyzfromclip.z()>z)
                Minxyzfromclip.SetZ(z);

        }

    shift = endExistedAtoms-Minxyzfromclip;

    shift.x() += 0.5;
    shift.y() += 0.5;
    shift.z() += 0.5;

    QString atomLabel,singleatom;
    vector3 q;
    QString newdata="";
    for (unsigned int i=0;i<lst.size();i++)
    {
        line=lst[i].simplified();
        if (line.size() < 2) continue;
        single=line.split(QRegularExpression("\\s+"));
        if (single.size()!=4) continue;

        atomLabel =single[0];
        q.SetX(single[1].toDouble());
        q.SetY(single[2].toDouble());
        q.SetZ(single[3].toDouble());

        q+=shift;
        singleatom=atomLabel+" "+QString::number(q.x(),'f',6)+" "+QString::number(q.y(),'f',6)+" "+QString::number(q.z(),'f',6)+"\n";
        newdata+=singleatom;
    }
    return newdata;
}

void HMol::reverseBondOrient(unsigned int idx0,unsigned int idx1)
{
    HBond * bond=getBondbyAtomIndex(idx0,idx1);
    bond->setBeginIndex(idx1);
    bond->setEndIndex(idx0);
}


bool HMol::canbePaste()
{
    //data from this program
    QString  Text= QApplication::clipboard()->text();//.simplified()--this function can convert data to 1 line.
    QString tmpText=Text;
    tmpText=tmpText.simplified();

    if (tmpText.normalized(QString::NormalizationForm_C) != tmpText)
        return false;

    if (tmpText.length() < 10)   return false;


    //cout << tmpText.length()<<endl;
    //cout << tmpText.toStdString().c_str()<<endl;


    QString data="";
    unsigned int initNumAtoms=NumAtoms();

    if(Text.contains("MolSymmetry-copy-data"))
    {
        /* MolSymmetry-copy-data
                   4
                   C    0.750134    -0.000023    0.000002    0.77    9474192
                   H    1.145634    -1.044702    0.000062    0.37    16777215
                   H    1.145156    0.522519    -0.904976    0.37    16777215
                   H    1.145174    0.522601    0.904923    0.37    16777215
                   3
                   0 1 1
                   0 2 1
                   0 3 1
           */

        QString  atomLabel,line,single;
        QStringList lst=Text.split("\n"),satom,sbond;

        unsigned int nAtoms=lst[1].toInt();
        if(nAtoms==0){
            //QMessageBox::information(0, "Warning","No data available!");
            return false;
        }
        unsigned int i;
        vector3 p;
        //delete first 2 lines
        lst.pop_front();
        lst.pop_front();

        for(i=0;i<nAtoms;i++){
            line=lst[i].simplified();
            satom=line.split(QRegularExpression("\\s+"));
            if(satom.size()!=6) continue;

            if(isNumber(satom[0]))
                atomLabel=QString::fromStdString(ElementNames[satom[0].toInt()]);
            else if(isEnglish(satom[0]))
                atomLabel=satom[0];
            else
                return false;

            if(isFloat(satom[1]) && isFloat(satom[2]) && isFloat(satom[3]) )
            {
                single = atomLabel + " " + satom[1]+ " " + satom[2]+ " " + satom[3]+"\n";
                data += single;
            }
            else
            {
                //QMessageBox::information(0, "Warning","Incorrect Data!");
                return false;
            }
        }

        //data=getShift(data);
        //addatomfromQString(data);

        unsigned int nBonds=lst[i].toInt();
        i=i+1;
        unsigned int beg,end;
        for (unsigned int j=0;j<nBonds;j++)
        {
            line=lst[i+j].simplified();
            //cout << line.toStdString().c_str()<<endl;
            sbond=line.split(QRegularExpression("\\s+"));
            if (sbond.size()!=3) return false;

            if(!isInt(sbond[0])) return false;
            if(!isInt(sbond[1])) return false;
            //beg=initNumAtoms+sbond[0].toInt();
            //end=initNumAtoms+sbond[1].toInt();
            //addBond(beg,end,sbond[2].toInt());
        }
       // perceivePlaneRingforBonds();
        return true;
    }

    //data from gaussian ot others
    Text = QApplication::clipboard()->text().simplified();
    if(Text.contains("GINC-")){
        data=parseGaussin09Summary(Text);
    }
    else
    {
        //replace all the comma with whitespace
        Text=Text.replace(QRegularExpression(",")," ");

        // QMessageBox::information(0, "Warning",Text);

        QStringList Lines = Text.trimmed().split("\n");
        if (Lines.size()<1) return false;

        QString single;
        QStringList ls;
        bool ok = false;

        unsigned int i, size=0;
        if (Lines.size()==1)
        {
            ls = Lines[0].trimmed().split(QRegularExpression("\\s+"));

            i=0;
            data="";

            //W  3.527590000000  0.124006000000  1.879670000000   R=1.992

            if(isEnglish(ls[0])  && isFloat(ls[1]) && isFloat(ls[2]) && isFloat(ls[3])  && ls[4].contains("R=") )
            {
                while  (i < ls.size())
                {
                    single = ls[i]+ " " + ls[i+1]+ " " + ls[i+2]+ " " + ls[i+3]+"\n";
                    data+=single;
                    i=i+6;
                }
            }


            /*
               1         22               -1.949599    4.505351    0.828982
               2          O               -1.425143    6.566943    1.077860
               3          O               -1.495672    4.077398    2.536112
               4          O               -2.730629    2.900709    0.362013
               5          O               -0.406798    4.239808   -0.032909
               6          O               -3.859200    5.310931    1.518737
               7          8               -2.809006    5.378771   -0.971769
               8          C               -0.682128    7.297813    0.361174
               9         22               -0.385940    3.776011    3.953315
               */
            if(ls[0]=="1" &&  isEnglishOrNumber (ls[1])  &&  ls[5]=="2"  )
            {
                while  (i < ls.size())
                {
                    single = ls[i+1]+ " " + ls[i+2]+ " " + ls[i+3]+ " " + ls[i+4]+"\n";
                    data+=single;
                    i=i+6;
                }
            }

            /*
               1         22           0       -1.949599    4.505351    0.828982
               2          8           0       -1.425143    6.566943    1.077860
               3          8           0       -1.495672    4.077398    2.536112
               4          8           0       -2.730629    2.900709    0.362013
               5          8           0       -0.406798    4.239808   -0.032909
               6          8           0       -3.859200    5.310931    1.518737
               7          8           0       -2.809006    5.378771   -0.971769
               8          6           0       -0.682128    7.297813    0.361174
               9         22           0       -0.385940    3.776011    3.953315
               */
            if(ls[0]=="1" && ls[6]=="2" && ls[2]=="0" )
            {
                while  (i < ls.size())
                {
                    single = ls[i+1]+ " " + ls[i+3]+ " " + ls[i+4]+ " " + ls[i+5]+"\n";
                    data+=single;
                    i=i+6;
                }
            }

            /*
               22           0       -1.949599    4.505351    0.828982
                8           0       -1.425143    6.566943    1.077860
                8           0       -1.495672    4.077398    2.536112
                8           0       -2.730629    2.900709    0.362013
                8           0       -0.406798    4.239808   -0.032909
                8           0       -3.859200    5.310931    1.518737
                8           0       -2.809006    5.378771   -0.971769
                6           0       -0.682128    7.297813    0.361174
               */
            if(ls[1]=="0" && ls[6]=="0") {
                while  (i < ls.size())
                {
                    single = ls[i]+ " " + ls[i+2]+ " " + ls[i+3]+ " " + ls[i+4]+"\n";
                    data+=single;
                    i=i+5;
                }
            }

            /*
               22       -1.949599    4.505351    0.828982
                8       -1.425143    6.566943    1.077860
                8       -1.495672    4.077398    2.536112
                8       -2.730629    2.900709    0.362013
                8       -0.406798    4.239808   -0.032909
                8       -3.859200    5.310931    1.518737
                8       -2.809006    5.378771   -0.971769
                6       -0.682128    7.297813    0.361174
               */
            if( isEnglishOrNumber (ls[0])  && isFloat(ls[1]) && isFloat(ls[2]) && isFloat(ls[3]) ) {
                while  (i < ls.size())
                {
                    single = ls[i]+ " " + ls[i+1]+ " " + ls[i+2]+ " " + ls[i+3]+"\n";
                    data+=single;
                    i=i+4;
                }
            }


            //QMessageBox::information(0, "Warning",data);
            //cout << "inside pastefromClipboard "<<endl;
            //cout << data.toStdString().c_str()<<endl;
            return true;
        }


        for (i=0; i < Lines.size(); i++)
        {
            single = Lines.at(i).trimmed();

            if ( single.size() < 4) continue;
            ls = single.trimmed().split(QRegularExpression("\\s+"));


            if ( ls.size() < 4 || ls.size() > 5)
                continue;

            if (ls.size() == 4 )
            {
                ls[1].toDouble(&ok);          if (!ok ) return false;
                ls[2].toDouble(&ok);          if (!ok ) return false;
                ls[3].toDouble(&ok);          if (!ok ) return false;
                single = ls[0]+ " " + ls[1]+ " " + ls[2]+ " " + ls[3];
            }

            if (ls.size() == 5 )
            {
                if(ls[1]=="0")
                    single = ls[0]+ " " + ls[2]+ " " + ls[3]+ " " + ls[4];
                else
                    single = ls[1]+ " " + ls[2]+ " " + ls[3]+ " " + ls[4];

                //QMessageBox::information(0, "Warning",single);
            }
            data+=single+"\n";
        }

        if (data.size() < 1) return false;
        return true;
    }

    //QMessageBox::information(0, "Warning",data);
    return false;
}

void HMol::pastefromClipboard()
{
    //data from this program
    QString Text = QApplication::clipboard()->text();//.simplified()--this function can convert data to 1 line.
    if (Text.length() < 5) {
        QMessageBox::information(0, "Warning","No data available!");
        return ;
    }
    if (Text.isEmpty()) {
        // QMessageBox::information(0, "Warning","No data available!");
        return ;
    }

    QString data="";
    unsigned int initNumAtoms=NumAtoms();

    if(Text.contains("MolSymmetry-copy-data"))
    {
        /* MolSymmetry-copy-data
                4
                C    0.750134    -0.000023    0.000002    0.77    9474192
                H    1.145634    -1.044702    0.000062    0.37    16777215
                H    1.145156    0.522519    -0.904976    0.37    16777215
                H    1.145174    0.522601    0.904923    0.37    16777215
                3
                0 1 1
                0 2 1
                0 3 1
        */

        QString  atomLabel,line,single;
        QStringList lst=Text.split("\n"),satom,sbond;

        unsigned int nAtoms=lst[1].toInt();
        if(nAtoms==0){
            QMessageBox::information(0, "Warning","No data available!");
            return ;
        }
        unsigned int i;
        vector3 p;
        //delete first 2 lines
        lst.pop_front();
        lst.pop_front();

        for(i=0;i<nAtoms;i++){
            line=lst[i].simplified();
            satom=line.split(QRegularExpression("\\s+"));
            if(satom.size()!=6) continue;

            if(isNumber(satom[0]))
                atomLabel=QString::fromStdString(ElementNames[satom[0].toInt()]);
            if(isEnglish(satom[0]))
                atomLabel=satom[0];

            if(isFloat(satom[1]) && isFloat(satom[2]) && isFloat(satom[3]) )
            {
                single = atomLabel + " " + satom[1]+ " " + satom[2]+ " " + satom[3]+"\n";
                data += single;
            }
            else
            {
                QMessageBox::information(0, "Warning","Incorrect Data!");
                return ;
            }
        }

        data=getShift(data);
        addatomfromQString(data);

        unsigned int nBonds=lst[i].toInt();
        i=i+1;
        unsigned int beg,end;
        for (unsigned int j=0;j<nBonds;j++)
        {
            line=lst[i+j].simplified();
            //cout << line.toStdString().c_str()<<endl;
            sbond=line.split(QRegularExpression("\\s+"));
            if (sbond.size()!=3) continue;

            beg=initNumAtoms+sbond[0].toInt();
            end=initNumAtoms+sbond[1].toInt();
            addBond(beg,end,sbond[2].toInt());
        }
        perceivePlaneRingforBonds();
        return ;
    }

    //data from gaussian ot others
    Text = QApplication::clipboard()->text().simplified();
    if(Text.contains("GINC-")){
        data=parseGaussin09Summary(Text);
        loadxyzfromQString(data);
    }
    else
    {
        //replace all the comma with whitespace
        Text=Text.replace(QRegularExpression(",")," ");

        // QMessageBox::information(0, "Warning",Text);

        QStringList Lines = Text.trimmed().split("\n");
        if (Lines.size()<1) return ;

        QString single;
        QStringList ls;
        bool ok = false;

        unsigned int i, size=0;
        if (Lines.size()==1)
        {
            ls = Lines[0].trimmed().split(QRegularExpression("\\s+"));

            i=0;
            data="";

            //W  3.527590000000  0.124006000000  1.879670000000   R=1.992

            if(isEnglish(ls[0])  && isFloat(ls[1]) && isFloat(ls[2]) && isFloat(ls[3])  && ls[4].contains("R=") )
            {
                while  (i < ls.size())
                {
                    single = ls[i]+ " " + ls[i+1]+ " " + ls[i+2]+ " " + ls[i+3]+"\n";
                    data+=single;
                    i=i+6;
                }
            }


            /*
            1         22               -1.949599    4.505351    0.828982
            2          O               -1.425143    6.566943    1.077860
            3          O               -1.495672    4.077398    2.536112
            4          O               -2.730629    2.900709    0.362013
            5          O               -0.406798    4.239808   -0.032909
            6          O               -3.859200    5.310931    1.518737
            7          8               -2.809006    5.378771   -0.971769
            8          C               -0.682128    7.297813    0.361174
            9         22               -0.385940    3.776011    3.953315
            */
            if(ls[0]=="1" &&  isEnglishOrNumber (ls[1])  &&  ls[5]=="2"  )
            {
                while  (i < ls.size())
                {
                    single = ls[i+1]+ " " + ls[i+2]+ " " + ls[i+3]+ " " + ls[i+4]+"\n";
                    data+=single;
                    i=i+6;
                }
            }

            /*
            1         22           0       -1.949599    4.505351    0.828982
            2          8           0       -1.425143    6.566943    1.077860
            3          8           0       -1.495672    4.077398    2.536112
            4          8           0       -2.730629    2.900709    0.362013
            5          8           0       -0.406798    4.239808   -0.032909
            6          8           0       -3.859200    5.310931    1.518737
            7          8           0       -2.809006    5.378771   -0.971769
            8          6           0       -0.682128    7.297813    0.361174
            9         22           0       -0.385940    3.776011    3.953315
            */
            if(ls[0]=="1" && ls[6]=="2" && ls[2]=="0" )
            {
                while  (i < ls.size())
                {
                    single = ls[i+1]+ " " + ls[i+3]+ " " + ls[i+4]+ " " + ls[i+5]+"\n";
                    data+=single;
                    i=i+6;
                }
            }

            /*
            22           0       -1.949599    4.505351    0.828982
             8           0       -1.425143    6.566943    1.077860
             8           0       -1.495672    4.077398    2.536112
             8           0       -2.730629    2.900709    0.362013
             8           0       -0.406798    4.239808   -0.032909
             8           0       -3.859200    5.310931    1.518737
             8           0       -2.809006    5.378771   -0.971769
             6           0       -0.682128    7.297813    0.361174
            */
            if(ls[1]=="0" && ls[6]=="0") {
                while  (i < ls.size())
                {
                    single = ls[i]+ " " + ls[i+2]+ " " + ls[i+3]+ " " + ls[i+4]+"\n";
                    data+=single;
                    i=i+5;
                }
            }

            /*
            22       -1.949599    4.505351    0.828982
             8       -1.425143    6.566943    1.077860
             8       -1.495672    4.077398    2.536112
             8       -2.730629    2.900709    0.362013
             8       -0.406798    4.239808   -0.032909
             8       -3.859200    5.310931    1.518737
             8       -2.809006    5.378771   -0.971769
             6       -0.682128    7.297813    0.361174
            */
            if( isEnglishOrNumber (ls[0])  && isFloat(ls[1]) && isFloat(ls[2]) && isFloat(ls[3]) ) {
                while  (i < ls.size())
                {
                    single = ls[i]+ " " + ls[i+1]+ " " + ls[i+2]+ " " + ls[i+3]+"\n";
                    data+=single;
                    i=i+4;
                }
            }


            //QMessageBox::information(0, "Warning",data);
            //cout << "inside pastefromClipboard "<<endl;
            //cout << data.toStdString().c_str()<<endl;
            loadxyzfromQString(data);
            return ;
        }


        for (i=0; i < Lines.size(); i++)
        {
            single = Lines.at(i).trimmed();

            if ( single.size() < 4) continue;
            ls = single.trimmed().split(QRegularExpression("\\s+"));


            if ( ls.size() < 4 || ls.size() > 5)
                continue;

            if (ls.size() == 4 )
            {
                ls[1].toDouble(&ok);          if (!ok ) return ;
                ls[2].toDouble(&ok);          if (!ok ) return ;
                ls[3].toDouble(&ok);          if (!ok ) return ;
                single = ls[0]+ " " + ls[1]+ " " + ls[2]+ " " + ls[3];
            }

            if (ls.size() == 5 )
            {
                if(ls[1]=="0")
                    single = ls[0]+ " " + ls[2]+ " " + ls[3]+ " " + ls[4];
                else
                    single = ls[1]+ " " + ls[2]+ " " + ls[3]+ " " + ls[4];

                //QMessageBox::information(0, "Warning",single);
            }
            data+=single+"\n";
        }

        if (data.size() < 1) return ;
        //data=getShift(data);
        loadxyzfromQString(data);
        return;
    }

    QMessageBox::information(0, "Warning",data);
}


QString HMol::parseGaussin09Summary(QString Text)
{
    /*
    1\1\GINC-LOCALHOST\SP\RB3LYP\TZVP\C1H4\ZHANGFQ\22-Jun-2018\0\\# b3lyp/
     TZVP IOP(6/7=3) gfinput gfprint\\Title Card Required\\0,1\C,0,-1.29032
     2649,0.483870916,0.\H,0,-0.9293678757,-0.5371027068,-0.000000888\H,0,-
     0.9293486003,0.9943506561,0.8841853074\H,0,-0.9293500505,0.9943516816,
     -0.8841853074\H,0,-2.3732240695,0.483884033,0.000000888\\Version=ES64L
     -G16RevA.03\State=1-A1\HF=-40.5369264\RMSD=3.417e-09\Dipole=0.,0.,0.\Q
     uadrupole=0.,0.,0.,0.,0.,0.\PG=TD [O(C1),4C3(H1)]\\@
     */
    QString all="";
    Text=Text.simplified();
    Text=Text.remove(QRegularExpression("\\s+"));
    QStringList Lines = Text.split("\n");
    for (unsigned int i=0; i < Lines.size(); i++)
        all+=Lines.at(i).trimmed();

    //QMessageBox::warning(this, "Script Execution Failed", all);
    //cout << Text.toStdString()<<endl;

    Lines.clear();

    all=all.trimmed();
    if(all.contains("\\")) Lines = all.split("\\");
    if(all.contains("||")) Lines = all.split("|");


    //QMessageBox::warning(this, "Script Execution Failed", all);
    float x,y,z;
    QStringList ls;
    QString single, one;

    all.clear();
    for (unsigned int i=0; i < Lines.size(); i++)
    {
        single = Lines.at(i).trimmed();
        //cout << single.toStdString();
        //continue;

        if (single.startsWith("Version") ) break;

        //cout << single.toStdString();
        ls = single.split(QRegularExpression(","));

        if (ls.size() != 5) continue;

        one= ls[0]+ " " + ls[2] + " " + ls[3]+ " " + ls[4] +"\n";
        all+=one;
        //cout << one.toStdString();
    }

    //QMessageBox::warning(this, "Script Execution Failed", all);
    return all;
}


//fixed atom or not
void HMol::setBondLengthBetweenTwoAtoms(unsigned int idx1, unsigned int idx2, double length,
                                        bool isFixedAtom1, bool isFixedAtom2)
{
    if(isFixedAtom1 && isFixedAtom2) return;

    vector3 pos1=getAtomPosbyIndex(idx1);
    vector3 pos2=getAtomPosbyIndex(idx2);

    //get translate vector
    vector3 v=pos2-pos1;
    double length0=v.length();

    v.normalize(); v=v*(length-length0);

    //move one of them
    if(isFixedAtom1 || isFixedAtom2)
    {
        //move atom 1
        if(isFixedAtom2){
            pos1=pos1-v;
            getAtombyIndex(idx1)->setPos(pos1);
        }

        //move atom 2
        if(isFixedAtom1){
            pos2=v+pos2;
            getAtombyIndex(idx2)->setPos(pos2);
        }

        perceivePlaneBonds();
        perceiveRings();
        return;
    }

    //move them both
    if((!isFixedAtom1) && (!isFixedAtom2))
    {
        v=v/2.0;
        pos1=pos1-v;pos2=pos2+v;
        getAtombyIndex(idx1)->setPos(pos1);
        getAtombyIndex(idx2)->setPos(pos2);
    }

    perceivePlaneBonds();
    perceiveRings();
}




void HMol::setBondLengthBetweenTwoFragments(unsigned int idx1, unsigned int idx2, double length,
                                            vector <uint> & fragment1,  vector <uint> & fragment2,
                                            bool isFixedFrag1, bool isFixedFrag2)
{

    if(isFixedFrag1 && isFixedFrag2) return;

    vector3 pos1=getAtomPosbyIndex(idx1);
    vector3 pos2=getAtomPosbyIndex(idx2);


    //get translate vector
    vector3 v=pos2-pos1;
    double length0=v.length();

    v.normalize();
    v=v*(length-length0);

    //move one of them
    if(isFixedFrag1 || isFixedFrag2)
    {
        //move frag 1
        if(isFixedFrag2){
            //pos1=pos1-v;
            for (unsigned int i=0;i<fragment1.size();i++) {
                vector3 v1=getAtomPosbyIndex(fragment1[i])-v;;
                getAtombyIndex(fragment1[i])->setPos(v1);
            }
        }

        //move frag 2
        if(isFixedFrag1){
            pos2=v+pos2;
            for (unsigned int i=0;i<fragment2.size();i++) {
                vector3 v1=getAtomPosbyIndex(fragment2[i])+v;;
                getAtombyIndex(fragment2[i])->setPos(v1);
            }
        }

        perceivePlaneBonds();
        perceiveRings();
        return;
    }

    //move them both
    if((!isFixedFrag1) && (!isFixedFrag2))
    {
        v=v/2.0;
        //pos1=pos1-v;pos2=pos2+v;
        //getAtombyIndex(idx1)->setPos(pos1);
        //getAtombyIndex(idx2)->setPos(pos2);

        for (unsigned int i=0;i<fragment1.size();i++) {
            vector3 v1=getAtomPosbyIndex(fragment1[i])-v;;
            getAtombyIndex(fragment1[i])->setPos(v1);
        }
        for (unsigned int i=0;i<fragment2.size();i++) {
            vector3 v1=getAtomPosbyIndex(fragment2[i])+v;;
            getAtombyIndex(fragment2[i])->setPos(v1);
        }
    }

    perceivePlaneBonds();
    perceiveRings();
}


void HMol::setBondAngleAtoms (unsigned int idx0, unsigned int idx1, unsigned int idx2, double bondAngle,
                              bool isFixedAtom1 , bool isFixedAtom2)
{
    if(isFixedAtom1 && isFixedAtom2) return;

    vector3 pos0=getAtomPosbyIndex(idx0);
    vector3 pos1=getAtomPosbyIndex(idx1);
    vector3 pos2=getAtomPosbyIndex(idx2);


    //get rotate vector
    vector3 v1,v2,v3;
    v1 = pos0 - pos1 ;
    v2 = pos2 - pos1 ;
    v3 = cross(v1,v2); //rotate axis


    double  angle0= vectorAngle(v1,v2);
    double  rotateAngle=bondAngle-angle0;
    if(fabs(rotateAngle)<0.6) return;

    vector3 v,vv;
    matrix3x3 m;

    //rotate one of them
    if(isFixedAtom1 || isFixedAtom2)
    {
        //rotate atom2
        if(isFixedAtom1)
        {
            m.SetupRotateMatrix(v3,rotateAngle);
            vv=getAtomPosbyIndex(idx2)-pos1;
            getAtombyIndex(idx2)->setPos(m*vv+pos1);
        }

        //rotate atom0
        if(isFixedAtom2)
        {
            m.SetupRotateMatrix(-v3,rotateAngle);
            vv=getAtomPosbyIndex(idx0)-pos1;
            getAtombyIndex(idx0)->setPos(m*vv+pos1);

        }

        perceivePlaneBonds();
        perceiveRings();
        return;
    }

    //rotate both atoms
    rotateAngle=rotateAngle/2.0;
    m.SetupRotateMatrix(v3,rotateAngle);
    vv=getAtomPosbyIndex(idx2)-pos1;
    getAtombyIndex(idx2)->setPos(m*vv+pos1);

    m.SetupRotateMatrix(-v3,rotateAngle);
    vv=getAtomPosbyIndex(idx0)-pos1;
    getAtombyIndex(idx0)->setPos(m*vv+pos1);

    perceivePlaneBonds();
    perceiveRings();
}




void HMol::setBondAngleFragments (unsigned int idx0, unsigned int idx1, unsigned int  idx2, double bondAngle,
                                  vector <uint> & fragment1, vector <uint> & fragment2,
                                  bool isFixedFrag1, bool isFixedFrag2)
{
    if(isFixedFrag1 && isFixedFrag2) return;

    vector3 pos0=getAtomPosbyIndex(idx0);
    vector3 pos1=getAtomPosbyIndex(idx1);
    vector3 pos2=getAtomPosbyIndex(idx2);


    //get rotate vector
    vector3 v1,v2,v3;
    v1 = pos0 - pos1 ;
    v2 = pos2 - pos1 ;
    v3 = cross(v1,v2); //rotate axis


    double  angle0= vectorAngle(v1,v2);
    double  rotateAngle=bondAngle-angle0;
    if(fabs(rotateAngle)<0.1) return;

    vector3 v;
    matrix3x3 m;

    //rotate one of them
    if(isFixedFrag1 || isFixedFrag2)
    {
        //rotate fragment2
        if(isFixedFrag1)
        {
            m.SetupRotateMatrix(v3,rotateAngle);

            for (unsigned int i=0;i<fragment2.size();i++) {
                vector3 vv=getAtomPosbyIndex(fragment2[i])-pos1;
                getAtombyIndex(fragment2[i])->setPos(m*vv+pos1);
            }
        }

        //rotate fragment1
        if(isFixedFrag2)
        {
            m.SetupRotateMatrix(-v3,rotateAngle);
            for (unsigned int i=0;i<fragment1.size();i++) {
                vector3 vv=getAtomPosbyIndex(fragment1[i])-pos1;;
                getAtombyIndex(fragment1[i])->setPos(m*vv+pos1);
            }
        }

        perceivePlaneBonds();
        perceiveRings();
        return;
    }

    //rotate both fragments
    rotateAngle=rotateAngle/2.0;
    m.SetupRotateMatrix(v3,rotateAngle);
    for (unsigned int i=0;i<fragment2.size();i++) {
        vector3 vv=getAtomPosbyIndex(fragment2[i])-pos1;;
        getAtombyIndex(fragment2[i])->setPos(m*vv+pos1);
    }


    m.SetupRotateMatrix(-v3,rotateAngle);
    for (unsigned int i=0;i<fragment1.size();i++) {
        vector3 vv=getAtomPosbyIndex(fragment1[i])-pos1;;
        getAtombyIndex(fragment1[i])->setPos(m*vv+pos1);
    }

    perceivePlaneBonds();
    perceiveRings();
}


void HMol::setTorsionAtoms(unsigned int idx0, unsigned int idx1, unsigned int idx2, unsigned int idx3,
                           double angle, bool isFixed0 , bool isFixed3)
{

    if(isFixed0 && isFixed3) return;

    //vector3 pos0=getAtomPosbyIndex(idx0);
    vector3 pos1=getAtomPosbyIndex(idx1);
    vector3 pos2=getAtomPosbyIndex(idx2);
    //vector3 pos3=getAtomPosbyIndex(idx3);

    double angle0=Dihedral(idx0,idx1,idx2,idx3);

    double  rotateAngle=angle-angle0;
    if(fabs(rotateAngle)<0.1) return;

    //get rotate vector
    vector3 v=pos2-pos1;
    matrix3x3 m;



    //rotate atom3
    if(isFixed0)
    {
        m.SetupRotateMatrix(v,rotateAngle);
        vector3 vv=getAtomPosbyIndex(idx3)-pos2;;
        getAtombyIndex(idx3)->setPos(m*vv+pos2);
        return;
    }

    //rotate fragment1
    if(isFixed3)
    {
        m.SetupRotateMatrix(-v,rotateAngle);
        vector3 vv=getAtomPosbyIndex(idx0)-pos2;;
        getAtombyIndex(idx0)->setPos(m*vv+pos2);
        return;
    }



    //rotate both fragments
    rotateAngle=rotateAngle/2.0;
    m.SetupRotateMatrix(v,rotateAngle);

    vector3 vv=getAtomPosbyIndex(idx3)-pos2;;
    getAtombyIndex(idx3)->setPos(m*vv+pos2);


    m.SetupRotateMatrix(-v,rotateAngle);
    vv=getAtomPosbyIndex(idx0)-pos2;;
    getAtombyIndex(idx0)->setPos(m*vv+pos2);


    perceivePlaneBonds();
    perceiveRings();
}



void HMol::setTorsionFragments(unsigned int idx0, unsigned int idx1, unsigned int idx2, unsigned int idx3, double angle,
                               vector <uint> & fragment1,  vector <uint> & fragment2,
                               bool isFixedFrag1 , bool isFixedFrag2)
{
    if(isFixedFrag1 && isFixedFrag2) return;

    //vector3 pos0=getAtomPosbyIndex(idx0);
    vector3 pos1=getAtomPosbyIndex(idx1);
    vector3 pos2=getAtomPosbyIndex(idx2);
    //vector3 pos3=getAtomPosbyIndex(idx3);

    double angle0=Dihedral(idx0,idx1,idx2,idx3);

    double  rotateAngle=angle-angle0;
    if(fabs(rotateAngle)<0.1) return;

    //get rotate vector
    vector3 vv;
    vector3 v=pos2-pos1;
    matrix3x3 m;

    //rotate one of two atoms
    if(isFixedFrag1 || isFixedFrag2)
    {
        //rotate fragment-2
        if(isFixedFrag1)
        {
            //cout << " isFixedFrag1"<<endl;
            m.SetupRotateMatrix(v,rotateAngle);
            for (unsigned int i=0;i<fragment2.size();i++) {
                vv=getAtomPosbyIndex(fragment2[i])-pos2;;
                getAtombyIndex(fragment2[i])->setPos(m*vv+pos2);
            }
        }

        //rotate fragment-1
        if(isFixedFrag2)
        {
            m.SetupRotateMatrix(-v,rotateAngle);
            for (unsigned int i=0;i<fragment1.size();i++) {
                vv=getAtomPosbyIndex(fragment1[i])-pos2;
                getAtombyIndex(fragment1[i])->setPos(m*vv+pos2);
            }
        }

        perceivePlaneBonds();
        perceiveRings();
        return;
    }

    //rotate both fragments
    rotateAngle=rotateAngle/2.0;
    m.SetupRotateMatrix(v,rotateAngle);
    for (unsigned int i=0;i<fragment2.size();i++) {
        vv=getAtomPosbyIndex(fragment2[i])-pos2;;
        getAtombyIndex(fragment2[i])->setPos(m*vv+pos2);
    }


    m.SetupRotateMatrix(-v,rotateAngle);
    for (unsigned int i=0;i<fragment1.size();i++) {
        vv=getAtomPosbyIndex(fragment1[i])-pos2;
        getAtombyIndex(fragment1[i])->setPos(m*vv+pos2);
    }

    perceivePlaneBonds();
    perceiveRings();
}


void HMol::flip (vector3 v)
{
    matrix3x3 m;
    m.reflectiontMatrix(v);

    vector3 a;
    for (unsigned int i=0;i<NumAtoms();i++)
    {
        a=atomPos(i);
        setAtomPos(i,m*a);
    }

    //update ring information
    for (unsigned int i=0;i<numRings();i++)
    {
        a=getRingbyId(i)->center;
        getRingbyId(i)->center=m*a;

        a=getRingbyId(i)->norm;
        getRingbyId(i)->norm=m*a;
    }


    //update bond plane information
    for (unsigned int i=0;i<NumBonds();i++)
    {
        if(getBondbyIndex(i)->getBondOrder()<2) continue;
        if(getBondbyIndex(i)->getBondOrder()>6) continue;

        if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

        a=getBondbyIndex(i)->getPlane();
        getBondbyIndex(i)->setPlane(m*a);
    }
}

//flip left-right
void HMol::flip_X()
{
    vector3 a;
    for (unsigned int i=0;i<NumAtoms();i++)
    {
        a=atomPos(i);
        a.SetX(-a.x());
        setAtomPos(i,a);
    }

    //update ring information
    for (unsigned int i=0;i<numRings();i++)
    {
        a=getRingbyId(i)->center;
        a.SetX(-a.x());
        getRingbyId(i)->center=a;

        a=getRingbyId(i)->norm;
        a.SetX(-a.x());
        getRingbyId(i)->norm=a;
    }

    //update bond plane information
    for (unsigned int i=0;i<NumBonds();i++)
    {
        if(getBondbyIndex(i)->getBondOrder()<2) continue;
        if(getBondbyIndex(i)->getBondOrder()>6) continue;

        if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

        a=getBondbyIndex(i)->getPlane();
        a.SetX(-a.x());
        getBondbyIndex(i)->setPlane(a);
    }
}

//flip top-bottom
void HMol::flip_Y()
{
    vector3 a;
    for (unsigned int i=0;i<NumAtoms();i++)
    {
        a=atomPos(i);
        a.SetY(-a.y());
        setAtomPos(i,a);
    }

    //update ring information
    for (unsigned int i=0;i<numRings();i++)
    {
        a=getRingbyId(i)->center;
        a.SetY(-a.y());
        getRingbyId(i)->center=a;

        a=getRingbyId(i)->norm;
        a.SetY(-a.y());
        getRingbyId(i)->norm=a;
    }



    //update bond plane information
    for (unsigned int i=0;i<NumBonds();i++)
    {
        if(getBondbyIndex(i)->getBondOrder()<2) continue;
        if(getBondbyIndex(i)->getBondOrder()>6) continue;

        if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

        a=getBondbyIndex(i)->getPlane();
        a.SetY(-a.y());
        getBondbyIndex(i)->setPlane(a);
    }
}

//flip front-back
void HMol::flip_Z()
{
    vector3 a;
    for (unsigned int i=0;i<NumAtoms();i++)
    {
        a=atomPos(i);
        a.SetZ(-a.z());
        setAtomPos(i,a);
    }

    //update ring information
    for (unsigned int i=0;i<numRings();i++)
    {
        a=getRingbyId(i)->center;
        a.SetZ(-a.z());
        getRingbyId(i)->center=a;

        a=getRingbyId(i)->norm;
        a.SetZ(-a.z());
        getRingbyId(i)->norm=a;
    }



    //update bond plane information
    for (unsigned int i=0;i<NumBonds();i++)
    {
        if(getBondbyIndex(i)->getBondOrder()<2) continue;
        if(getBondbyIndex(i)->getBondOrder()>6) continue;

        if (getBondbyIndex(i)->getPlane().length() < 0.6) continue;

        a=getBondbyIndex(i)->getPlane();
        a.SetZ(-a.z());
        getBondbyIndex(i)->setPlane(a);
    }
}


void HMol::removeBackupAtoms()
{
    if (atomList_bk.size() > 0)
        for (auto it=atomList_bk.begin();it!=atomList_bk.end();++it)
        {
            if(*it != nullptr) {
                delete (*it);
                (*it) = nullptr;
            }
        }

    atomList_bk.clear();
    QVector<HCoordinate*>().swap(atomList_bk);
}



void HMol::backupAtoms()
{
    removeBackupAtoms();
    atomList_bk.clear();

    for (unsigned int i=0;i<NumAtoms();i++) {
        HCoordinate * single = new HCoordinate();
        single->symbol=getAtomSymbol(i);
        single->p=getAtomPosbyIndex(i);
        atomList_bk.push_back(single);
    }
}


//just copy
void HMol::genNormalAtoms()
{
    atomList_normal.resize(NumAtoms());
    for (unsigned int i=0;i<NumAtoms();i++)
        atomList_normal[i]=getAtomPosbyIndex(i);
}

void HMol::genRotationAtoms(vector3 _axis, double angle)
{
    atomList_rotation.resize(NumAtoms());

    matrix3x3 m;
    m.SetupRotateMatrix(_axis,angle);
    for (unsigned int i=0;i<NumAtoms();i++)
        atomList_rotation[i]=m*getAtomPosbyIndex(i);
}

void HMol::genReflectionAtoms(vector3 _normal)
{
    matrix3x3 m;
    m.SetupReflectiontMatrix(_normal);

    atomList_reflection.resize(NumAtoms());
    for (unsigned int i=0;i<NumAtoms();i++)
        atomList_reflection[i]=m*getAtomPosbyIndex(i);
}

void HMol::genRotationReflectionAtoms(vector3 _normal, double angle)
{
    matrix3x3 m;

    //use atomList_normal to replace  atomList_rotation
    m.SetupRotateMatrix(_normal,angle);
    atomList_normal.resize(NumAtoms());
    for (unsigned int i=0;i<NumAtoms();i++) {
        atomList_normal[i]=m*getAtomPosbyIndex(i);
    }

    m.SetupReflectiontMatrix(_normal);
    atomList_reflection.resize(NumAtoms());
    for (unsigned int i=0;i<NumAtoms();i++)
        atomList_reflection[i]=m*atomList_normal[i];
}

void HMol::genRotationReversionAtoms(vector3 _normal, double angle)
{
    matrix3x3 m;

    //use atomList_normal to replace  atomList_rotation
    m.SetupRotateMatrix(_normal,angle);
    atomList_normal.resize(NumAtoms());
    for (unsigned int i=0;i<NumAtoms();i++) {
        atomList_normal[i]=m*getAtomPosbyIndex(i);
    }

    atomList_reversion.resize(NumAtoms());
    for (unsigned int i=0;i<NumAtoms();i++)
        atomList_reversion[i]=-1.0*atomList_normal[i];
}

void HMol::genReverseAtoms()
{
    matrix3x3 m;
    m.Set(0,0,-1);
    m.Set(1,1,-1);
    m.Set(2,2,-1);

    atomList_reversion.resize(NumAtoms());
    for (unsigned int i=0;i<NumAtoms();i++)
        atomList_reversion[i]=m*getAtomPosbyIndex(i);
}


void HMol::clearReflectionAtoms()
{
    atomList_reflection.clear();
}
void HMol::clearReverseAtoms()
{
    atomList_reversion.clear();
}
void HMol::clearNormalAtoms()
{
    atomList_normal.clear();
}



void HMol::recoverAtomCoordinateFromBackup()
{
    for (unsigned int i=0;i<NumAtoms();i++)
        getAtombyIndex(i)->setPos(atomList_bk[i]->p);
}

//remove one terminal hydrogen of this atom with id
bool HMol::removeOneTerminalHydrogen(unsigned int idx)
{
   // bool isTerminalHydrogen=false;

    vector <unsigned int > idsHydrogen;
    for (unsigned int i=0;i<getAtombyIndex(idx)->numNeighbors();i++)
    {
        unsigned int ii=getAtombyIndex(idx)->getNeighborId(i);
        if(getAtombyIndex(ii)->Symbol()!="H")
            continue;

        //terminal Hydrogen?
        if(getAtombyIndex(ii)->numNeighbors()==1)
            idsHydrogen.push_back(ii);
    }

    if(idsHydrogen.size()<1) return false;

    removeAtombyId(idsHydrogen[0]);

    return true;
}


void HMol::autoAdjustHydrogenWhole()
{
    autoAdjustHydrogen();
    cleanMol();
    autoAddHydrogen();
}


void HMol::autoAdjustHydrogen()
{
    QString Symbol;

    for(unsigned int id=0;id<NumAtoms();id++)
    {
        Symbol=getAtombyIndex(id)->Symbol();
        if(Symbol=="C" || Symbol=="Si" || Symbol=="Ge" ||
                Symbol=="N" || Symbol=="P"  ||
                Symbol=="P" || Symbol=="As" ||
                Symbol=="O" || Symbol=="F")

            autoAdjustHydrogen(id);
    }
}


void HMol::autoAdjustHydrogen(unsigned int idx)
{
    unsigned int times=0;
    float numBonds=0.0;

    unsigned int bondOrder,ii;

    for (unsigned int i=0;i<getAtombyIndex(idx)->numNeighbors();i++)
    {
        ii=getAtombyIndex(idx)->getNeighborId(i);
        bondOrder=getBondOrderbyAtomIndex(idx,ii);
        if(bondOrder<=4)
            numBonds+=bondOrder;

        if(bondOrder==6) //aromatic
            numBonds=+1.51;

        //if(bondOrder==5  || bondOrder==7) //hydogen && weak
        //    numBonds=+0.0;
    }

    QString Symbol=getAtombyIndex(idx)->Symbol();

    if(Symbol=="C" || Symbol=="Si" || Symbol=="Ge")
    {
        if(numBonds==4) return;
        if(numBonds>4)
        {
            removeOneTerminalHydrogen(idx);
            if(numBonds>3)  autoAdjustHydrogen(idx);
        }
        times=4-numBonds;
    }


    if(Symbol=="N" || Symbol=="P" )
    {
        if(numBonds==3) return;
        if(numBonds>3)
        {
            removeOneTerminalHydrogen(idx);
            if(numBonds>2)  autoAdjustHydrogen(idx);
        }

        times=3-numBonds;
    }


    if( Symbol=="P" || Symbol=="As")
    {
        if(numBonds==5) return;
        if(numBonds>5)
        {
            removeOneTerminalHydrogen(idx);
            if(numBonds>4)
                autoAdjustHydrogen(idx);
        }
        times=5-numBonds;
    }


    if(Symbol=="O")
    {
        if(numBonds==2) return;
        times=2-numBonds;
        if(numBonds>2)
        {
            removeOneTerminalHydrogen(idx);
            if(numBonds>1) autoAdjustHydrogen(idx);
        }
    }
}


void HMol::autoAddHydrogen(unsigned int idx)
{
    unsigned int times=0;
    double numBonds=0.0;

    cout << "adding hydrogen to "<<idx+1<<endl;
    for (unsigned int i=0;i<getAtombyIndex(idx)->numNeighbors();i++)
    {
        unsigned int ii=getAtombyIndex(idx)->getNeighborId(i);
        unsigned int bondOrder=getBondOrderbyAtomIndex(idx,ii);

        if(bondOrder<=4)
            numBonds += bondOrder;

        if(bondOrder==6) //aromatic
            numBonds+=1.5;

        if(bondOrder==5  || bondOrder==7) //hydogen && weak
            numBonds+=1.0;
    }

    QString Symbol=getAtombyIndex(idx)->Symbol();


    if(Symbol=="C" || Symbol=="Si" || Symbol=="Ge")
    {
        //cout << numBonds << " of " << Symbol.toStdString()<<idx+1 << " adding hydrogen"<<endl;;
        if(round(numBonds)==4) return;
        if(round(numBonds)>4)
        {
            //if(!removeOneTerminalHydrogen(idx));
            //autoAddHydrogen(idx);
            return;
        }
        times=4-round(numBonds);

    }


    if(Symbol=="N" || Symbol=="P" )
    {
        if(round(numBonds)==3) return;
        if(round(numBonds)>3)
        {
            //if(!removeOneTerminalHydrogen(idx));
            //autoAddHydrogen(idx);
            return;
        }

        times=3-round(numBonds);
    }


    if( Symbol=="P" || Symbol=="As") {

        if(round(numBonds)==5) return;
        if(round(numBonds)>5)
        {
            if(!removeOneTerminalHydrogen(idx));
            autoAddHydrogen(idx);
        }

        times=5-round(numBonds);
    }


    if(Symbol=="O") {
        times=2-round(numBonds);
        if(int(numBonds)>2)
        {
            if(!removeOneTerminalHydrogen(idx));
            autoAddHydrogen(idx);
        }
    }

    if(Symbol=="F")
        times=1-round(numBonds);

    cout << "adding "<< times << " hydrogen to "<< Symbol.toStdString()<<idx+1 << endl;;

    if(times<1) return;

    //cout << "adding Hydrogen for "<< idx+1<< "  atoms"<<endl;
    for(unsigned int i=0;i<times;i++)
        addHydrogen(idx);
}

//for whole molecule
void HMol::autoAddHydrogen()
{
    QString Symbol;
    for(unsigned int id=0;id<NumAtoms();id++)
    {
        Symbol=getAtombyIndex(id)->Symbol();
        if(Symbol=="C" || Symbol=="Si" || Symbol=="Ge" ||
                Symbol=="N" || Symbol=="P"  ||
                Symbol=="P" || Symbol=="As" ||
                Symbol=="O" || Symbol=="F")

            autoAddHydrogen(id);
    }
}

void HMol::addHydrogen(unsigned int idx)
{
    //random position to avoid eclipsed coordinate
    //double x= rand()%100*1.0;
    //double y= rand()%100*1.0;
    //double z= rand()%100*1.0;

    uint numCoord=getAtombyIndex(idx)->numNeighbors();

    vector3 vv=VZ;
    if(numCoord<1) vv=VZ;

    if(numCoord==1)
    {
        vv=(getAtomPosbyIndex(idx)-getAtomPosbyIndex(getAtombyIndex(idx)->getNeighborId(0))).normalize();
    }
    else
    {
        for(unsigned int i=0;i< getAtombyIndex(idx)->numNeighbors();i++)
            vv += (-getAtomPosbyIndex(getAtombyIndex(idx)->getNeighborId(i))+getAtomPosbyIndex(idx)).normalize();

        vv.normalize();
        if(vv.length_2()<0.2)
        {
            if(numCoord==3) {
                vector3 v1=getAtomPosbyIndex(getAtombyIndex(idx)->getNeighborId(0))-getAtomPosbyIndex(idx);
                vector3 v2=getAtomPosbyIndex(getAtombyIndex(idx)->getNeighborId(1))-getAtomPosbyIndex(idx);
                vector3 v3=getAtomPosbyIndex(getAtombyIndex(idx)->getNeighborId(2))-getAtomPosbyIndex(idx);
                vv=cross(v2-v1,v3-v2).normalize();
            }

            if(numCoord==2) {
                vector3 v1=getAtomPosbyIndex(getAtombyIndex(idx)->getNeighborId(0))-
                        getAtomPosbyIndex(getAtombyIndex(idx)->getNeighborId(1));
                if(!v1.createOrthoVector(vv)) {
                    double x= rand()%10*1.0;
                    double y= rand()%10*1.0;
                    double z= rand()%10*1.0;
                    vv.Set(x,y,z);
                }
            }
            if(numCoord>3)
            {
                double x= rand()%10*1.0;
                double y= rand()%10*1.0;
                double z= rand()%10*1.0;
                vv.Set(x,y,z);
            }
        }
    }


    vv.SetX(vv.x()+rand()%100/200.);
    vv.SetY(vv.y()+rand()%100/200.);
    vv.SetZ(vv.z()+rand()%100/200.);




    addAtom(1,1.5*vv+getAtomPosbyIndex(idx));
    addBond(idx,NumAtoms()-1,1);
    buildOBMol();

    //collect hydrogen atoms around this atom
    vector <unsigned int > hydrogenList;
    for (unsigned int i=0;i< getAtombyIndex(idx)->numNeighbors();i++)
    {
        unsigned int id=getAtombyIndex(idx)->getNeighborId(i);
        if (!getAtombyIndex(id)->isHydrogen()) continue;
        hydrogenList.push_back(id);
    }


    vector <unsigned int > All;
    for (unsigned int i=0;i<NumAtoms()-1;i++)
        All.push_back(i);

	
    cout << "hydrogen of this atoms: "<<endl;
    for (uint i=0;i<hydrogenList.size();i++)
        cout << hydrogenList[i]+1 << endl;

    for (uint i=0;i<hydrogenList.size();i++)
        cout << All[i]+1 << endl;

    OpenBabel::OBFFConstraints constraints;
    vector <unsigned int> constraintIdList;


    cout << "\n\nconstraint --------" <<endl;

    if(hydrogenList.size()>1)
    {
        std::set_difference(All.begin(), All.end(),
                            hydrogenList.begin(), hydrogenList.end(),
                            std::back_inserter(constraintIdList));

        for(unsigned int i=0;i< constraintIdList.size();i++) {
            constraints.AddAtomConstraint(constraintIdList[i]+1);
            cout<< constraintIdList[i]+1<<" ";
        }
    }
    else {
        for(unsigned int i=0;i< All.size();i++) {
            constraints.AddAtomConstraint(All[i]+1);
            cout << All[i]+1 <<endl;
        }
    }
    cout << "\nconstraint --------\n\n" <<endl;
	if(constraintIdList.size()==NumAtoms()) return;

    uint steps = 100;
    double criterion = 1e-5;

    OpenBabel:: OBForceField* pFF = OpenBabel::OBForceField::FindForceField("UFF");

    pFF->Setup(obmol,constraints);
    pFF->ConjugateGradientsInitialize(steps, criterion);


    uint totalSteps = 0;
    while (pFF->ConjugateGradientsTakeNSteps(1))
    {
        totalSteps++;
        if(totalSteps>steps) break;
    }

    pFF->UpdateCoordinates(obmol);

    fromOBMol();
}



void HMol::perceiveRings()
{
    perceiveRings_OB();
}


void HMol::perceiveMolLength(QString PG, double &  height, double & radius)
{
    height=radius=1.0;

    double x=0.0, y=0.0,z=0.0;
    vector3 pos;


    if (PG.startsWith("T") || PG.startsWith("O")|| PG.startsWith("I"))
    {
        for (unsigned int i=0;i<NumAtoms();i++)
        {
            pos=atomPos(i);

            z=abs(pos.z());
            x=abs(pos.x());
            y=abs(pos.y());

            if ( z > height)
                height=z;

            if ( x > height)
                height=x;

            if ( y > height)
                height=y;
        }

        radius=height;
    }



    if( PG.startsWith("S") )
    {

        return;
    }

    //Cnv  Cnh  Cn  CS  CI C00V
    if( PG.startsWith("C") )
    {
        if(PG=="CS")
        {

            return;
        }

        else if (PG=="CI")
        {

            return;
        }

        else if (PG=="C**V" || PG=="COOV" || PG=="C00V")
        {

            return;
        }


        //------------------------------------
        else if(PG.endsWith("H")) //!Cnh
        {

            return;
        }

        if (PG.endsWith("V"))	//!Cnv
        {


            return;
        }
        //-----------------------------------
        else    //!Cn
        {

            return;
        }
    }



    // Dn  Dnh
    if( PG.at(0)=='D')
    {
        if (PG=="D**H" || PG=="DOOH" || PG=="D00H")
        {

            return;
        }

        else if(PG.endsWith("H") )
        {

            return;
        }
        else if(PG.endsWith("D"))
        {

            return;
        }
        else //Dn
        {


            return;
        }
    }

}
void HMol::perceiveMolLength()
{
    height=radius=1.0;

    double x, y,z;
    vector3 pos;
    for (unsigned int i=0;i<NumAtoms();i++)
    {
        pos=atomPos(i);
        z=abs(pos.z());
        x=abs(pos.x());
        y=abs(pos.y());
        if ( z > height)
            height=z;

        if ( x > radius)
            radius=x;

        if ( y > radius)
            radius=y;
    }

    radius=radius*2.1;
    height=height*2.1;
}

void HMol::perceiveMolLength( double & _height , double & _radius)
{
    perceiveMolLength();

    _radius=radius;
    _height=height;
    //cout << "height &&  radius : " << _height << " "<<_radius<<endl;
}


void HMol::thermalDisplacement(double scale)
{
    //you can change scale here!


    double x,y,z;
    for(unsigned int i=0;i<NumAtoms();i++)
    {
        x=getAtomXbyIndex(i)+((double)rand()/RAND_MAX-0.5)/scale;
        y=getAtomYbyIndex(i)+((double)rand()/RAND_MAX-0.5)/scale;
        z=getAtomZbyIndex(i)+((double)rand()/RAND_MAX-0.5)/scale;

        getAtombyIndex(i)->setPos(x,y,z);
    }

}

void HMol::removeX()
{
    //removeAllBonds();
    //removeAllRings();
    //removeAllArcs();

    unsigned int num=NumAtoms();
    while(num--)
    {
        if(getAtomSymbol(num)=="X" || getAtomSymbol(num)=="Bq")
            removeAtombyId(num);
    }
}


//add long bond: (C5H5)2Fe
void HMol::reCalculateBonds()
{
    double r1, r2;
    QString a1,a2;
    vector3 p1,p2;
    for(unsigned int i=0;i<NumAtoms();i++)
    {
        if(getAtombyIndex(i)->Symbol()=="X") continue;
        p1=getAtomPosbyIndex(i);
        r1=getAtomRadiusbyIndex(i);

        if(getAtombyIndex(i)->Symbol()=="Ca" || getAtombyIndex(i)->Symbol()=="Na" || getAtombyIndex(i)->Symbol()=="K")
            r1*=0.9;

        for(unsigned int j=i+1;j<NumAtoms();j++)
        {
            if(getBondOrderbyAtomIndex(i,j)>0) continue;
            if(getAtombyIndex(j)->Symbol()=="X") continue;
            r2=getAtomRadiusbyIndex(j);
            p2=getAtomPosbyIndex(j);

            if(getAtombyIndex(i)->Symbol()=="Ca" || getAtombyIndex(i)->Symbol()=="Na" || getAtombyIndex(i)->Symbol()=="K")
                r2*=0.9;

            if( getAtombyIndex(i)->atomicNum()<10 && getAtombyIndex(j)->atomicNum()<10) {
                if( (p1-p2).length() < (r1+r2)*1.1)
                    addBond(i,j,1);
                continue;
            }

            if( (p1-p2).length() > (r1+r2)*1.36) continue;

            addBond(i,j,1);
        }
    }
}



void HMol::setRingAromatic(unsigned int id, bool ok)
{
    getRingbyId(id)->isAromatic=ok;

    unsigned int size=getRingbyId(id)->size;

    HBond * b;
    for(unsigned int i=0;i<size-1;i++)
    {
        b = getBondbyAtomIndex(getRingbyId(id)->atomIdList[i],
                               getRingbyId(id)->atomIdList[i+1]);
        b->setAromatic(ok);
        if(ok) b->setBondOrder(6);
    }

    b= getBondbyAtomIndex(getRingbyId(id)->atomIdList[0],
            getRingbyId(id)->atomIdList[size-1]);
    b->setAromatic(ok);
    if(ok) b->setBondOrder(6);
}

bool HMol::isXExisted()
{
    return  isAtomExisted("X");
}
bool HMol::isHydrogenAtomExisted()
{
    return  isAtomExisted("H");
}


bool HMol::isAtomExisted(QString ll)
{
    for(unsigned int i=0;i<NumAtoms();i++)
        if(getAtomSymbol(i)=="ll") return true;
    return false;
}


void HMol::moveAtomFromTo(unsigned int beg, unsigned int end)
{
    //move backward
    if(end>beg) {

        return;
    }

    //move forward
}

/*
std::string HMol::formula(const std::string& delimiter, int over) const
{
  // Adapted from chemkit:

  // A map of atomic symbols to their quantity
  std::map<unsigned char, size_t> composition;

  for (Array<unsigned char>::const_iterator it = m_atomicNumbers.begin(),
                                            itEnd = m_atomicNumbers.end();
       it != itEnd; ++it) {
    composition[*it]++;
  }

  std::stringstream result;
  std::map<unsigned char, size_t>::iterator iter;

  // Carbons first
  iter = composition.find(6);
  if (iter != composition.end()) {
    result << "C";
    if (iter->second > static_cast<size_t>(over))
      result << delimiter << iter->second;
    composition.erase(iter);

    // If carbon is present, hydrogens are next.
    iter = composition.find(1);
    if (iter != composition.end()) {
      result << delimiter << "H";
      if (iter->second > static_cast<size_t>(over))
        result << delimiter << iter->second;
      composition.erase(iter);
    }
  }

  // The rest:
  iter = composition.begin();
  while (iter != composition.end()) {
    result << delimiter << Elements::symbol(iter->first);
    if (iter->second > static_cast<size_t>(over))
      result << delimiter << iter->second;
    ++iter;
  }

  return result.str();
}

*/
