#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QString>
#include "Mol.h"

class templateMol
{
public:
    templateMol();
    ~templateMol() {clearAll();}


    QString fileName,name;

    QVector <HRing *> ringList;
    QVector <HAtom *> atomList;
    QVector <HBond *> bondList;
    unsigned int  hotIndex;

    OpenBabel::OBMol  obmol;


    unsigned int  getHotIndex() {return hotIndex;}
    void LoadFile(QString );

    void LoadFile_MetalTemplate(QString);

    void clearAll();
    void clearAtoms();
    void clearBonds();
    void clearRings();

    HAtom * getAtombyIndex(unsigned int id) {return atomList[id];}
    void setLinkedAtom(unsigned int idx);
    //unsigned int getLinkedAtom(unsigned int id) {return ;};



    unsigned int numAtoms() {return atomList.size();}
    unsigned int numBonds() {return bondList.size();}

    QString getSymbol (unsigned int id) {return atomList[id]->Symbol();}
    vector3 getPos (unsigned int id) {return atomList[id]->Pos();}
    HBond * getBond(unsigned int id) {return bondList[id];}
    HAtom * getAtom(unsigned int id) {return atomList[id];}


    bool isAtomAromatic(unsigned int id) {return atomList[id]->isAromatic();}

    unsigned int    getAtomicNum(unsigned int id)  {return atomList[id]->atomicNum();}
    double  getAtomXbyIndex(unsigned int id) {return atomList[id]->x();}
    double  getAtomYbyIndex(unsigned int id) {return atomList[id]->y();}
    double  getAtomZbyIndex(unsigned int id) {return atomList[id]->z();}
    double  getAtomRadiusbyIndex(unsigned int id) {return atomList[id]->radius();}

    vector3 getAtomPosbyIndex(unsigned int id) {return atomList[id]->Pos();}

    HBond * getBondbyIndex(unsigned int id) {return bondList[id];}


    void rotate(vector3 v, double angle);
    void rotate(matrix3x3 & m);
    void translate(vector3 v);
    void centralize();


    void perceivePlaneBondbyId(unsigned int id);
    void perceivePlaneBonds();
    void perceiveRings();

    void buildOBMol();
    bool fromOBMol();

    void reCalculateBonds();

    vector3 atomPos(unsigned int idx) {
        if (idx<numAtoms() )
            return atomList[idx]->Pos();
        return VZero;
    }


    void setAtomPos (unsigned int idx, vector3 p)
    {
        if (idx >= numAtoms()) return;
        atomList[idx]->setPos(p);
    }


    double atomicMass (unsigned int idx)
    {
        if (idx<numAtoms())
            return atomList[idx]->Mass();
        return 0.0;
    }

    QString atomSymbol (unsigned int idx) {
        if (idx<numAtoms() )
            return atomList[idx]->Symbol();
        return "X";
    }

    unsigned int atomicNum (unsigned int idx) {
        if (idx<numAtoms() )
            return atomList[idx]->atomicNum();
        return 0;
    }

    unsigned int numRings() {return ringList.size();}


    HRing * getRingbyId(unsigned int id) {return ringList[id];}
    void displayRingList();
    void displayBondList();
    void displayAtomList();
    bool isExistedAromaticBond;

    QString getAtomSymbol (unsigned int id) {return atomList[id]->Symbol();}


};

#endif // TEMPLATE_H
