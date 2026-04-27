#include "templateMol.h"
#include "mTools.h"
#include <QFile>
#include <QMessageBox>
#include "mainwindow.h"
#include <openbabel/ring.h>
#include <openbabel/kekulize.h>

templateMol::templateMol()
{
    atomList.clear();
    bondList.clear();
    ringList.clear();
    hotIndex=0;
}


void templateMol::centralize()
{
    vector3 MassCenter=VZero;
    double totalMass=0.0;

    for (unsigned int  i=0;i<numAtoms();i++) {
        double mass=atomicMass(i);
        MassCenter += atomPos(i)*mass;
        totalMass += mass;
    }

    MassCenter /= totalMass;

    //cout << MassCenter<<endl;

    for (unsigned int j=0;j<numAtoms();j++){
        vector3 p=atomPos(j)-MassCenter;
        setAtomPos(j,p);
    }
}



void templateMol::reCalculateBonds()
{
    double r1, r2;
    QString a1,a2;
    vector3 p1,p2;
    for(unsigned int i=0;i<numAtoms();i++)
    {
        if(getAtombyIndex(i)->Symbol()=="X") continue;
        p1=getAtomPosbyIndex(i);
        r1=getAtomRadiusbyIndex(i);

        //cout << i+1<<endl;
        for(unsigned int j=i+1;j<numAtoms();j++)
        {
            if(getAtombyIndex(j)->Symbol()=="X") continue;
            r2=getAtomRadiusbyIndex(j);
            p2=getAtomPosbyIndex(j);

            if( (p1-p2).length() > (r1+r2)*1.1) continue;

            HBond * bond = new HBond(i,j,1);
            bondList.push_back(bond);
            getAtombyIndex(i)->appendLinkdedAtomId(j);
            getAtombyIndex(j)->appendLinkdedAtomId(i);
        }
    }
}


void templateMol::LoadFile(QString _file)
{
    clearAll();

    fileName=_file;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //QMessageBox::information(0, "Warning","Cannot Open File!");
        return;
    }

    double x, y, z;
    QString  atomLabel;
    QStringList lst;
    QString line;

    hotIndex=0;
    unsigned int IndexHydrogen=0;
    bool isHotDefined=false;
    bool findFirstHydrogen=false;

    unsigned int _numBonds=0;
    unsigned int _numAtoms=0;

    while(!file.atEnd())
    {
        //first line
        line=file.readLine();
        name=line.simplified();

        //cout << line.toStdString().c_str()<<endl;

        //second line
        line=file.readLine();
        unsigned int _numAtoms=line.toInt();

        for (unsigned int i=0;i<_numAtoms;i++)
        {
            line=file.readLine().simplified();

            lst=line.split(" ",Qt::SkipEmptyParts);

            if (line.contains("hot")) {
                hotIndex=i;
                isHotDefined=true;
            }


            if  (lst.size()<3 || lst.size()>5)
                continue;

            //cout << line.toStdString().c_str()<<endl;

            atomLabel=lst[0].simplified();
            x=lst[1].toDouble();
            y=lst[2].toDouble();
            z=lst[3].toDouble();

            HAtom * atom = new  HAtom();
            atom->setSymbol(atomLabel);
            atom->setCartesian(x,y,z);
            atomList.push_back(atom);

            if(!findFirstHydrogen)
            if(atomLabel=="H")  {
                IndexHydrogen=atomList.size()-1;
                findFirstHydrogen=true;
            }
        }

        if(!isHotDefined)
            hotIndex=IndexHydrogen;

        //second line
        line=file.readLine().simplified();
        _numBonds=line.toInt();
        for (unsigned int i=0;i<_numBonds;i++)
        {
            line=file.readLine().simplified();
            lst=line.split(" ",Qt::SkipEmptyParts);
            if  (lst.size()!=3) continue;

            unsigned int bondorder=lst[2].toInt();

            //aromatic
            if(bondorder==4) // order = 4 is defined in gaussian view
            {
                bondorder=6;  // order = 6
                isExistedAromaticBond=true;
                getAtombyIndex(lst[0].toInt()-1)->setAromatic(true);
                getAtombyIndex(lst[1].toInt()-1)->setAromatic(true);
            }

            HBond * bond = new HBond(lst[0].toInt()-1,lst[1].toInt()-1,bondorder);

            //cout << bond->atomIndex0()+1 << " "<<bond->atomIndex1()+1<< " " << bond->getBondOrder()<<endl;
            bondList.push_back(bond);


            getAtombyIndex(lst[0].toInt()-1)->appendLinkdedAtomId(lst[1].toInt()-1);
            getAtombyIndex(lst[1].toInt()-1)->appendLinkdedAtomId(lst[0].toInt()-1);
        }
    }

    file.close();

    centralize();
    if(_numBonds<1)
        reCalculateBonds();




    //check aromatic based on bond order==6 or not?
    if (isExistedAromaticBond)
    {
        //check aromatic based on openbabel
        perceiveRings();

        //further check based on bond order
        for (unsigned int i=0;i<numRings();i++)
        {
            HRing  * ring = getRingbyId(i);

            if(ring->isAromatic) continue;

            bool isAromatic=true;
            for (unsigned int j=0;j<ring->size;j++) {
                if (isAtomAromatic(ring->atomIdList[j])) continue;
                    isAromatic=false;
                    break;
            }
            ring->isAromatic=isAromatic;
        }
    }



    perceivePlaneBonds();


    //displayAtomList();
   // displayBondList();
    displayRingList();
}


void templateMol::LoadFile_MetalTemplate(QString _file)
{
    clearAll();

    fileName=_file;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(0, "Warning","Cannot Open File!");
    }

    double x, y, z;
    QString  atomLabel;
    QStringList lst;
    QString line;


    hotIndex=0;
    unsigned int IndexHydrogen=0;
    bool isHotDefined=false;
    bool findFirstHydrogen=false;

    while(!file.atEnd())
    {
        //first line
        line=file.readLine();
        name=line.simplified();

        //second line
        line=file.readLine();
        unsigned int _numAtoms=line.toInt();

        for (unsigned int i=0;i<_numAtoms;i++)
        {
            line=file.readLine().simplified();

            lst=line.split(" ",Qt::SkipEmptyParts);

            if (line.contains("hot")) {
                hotIndex=i;
                isHotDefined=true;
            }


            if  (lst.size()<3 || lst.size()>5)
                continue;

            atomLabel=lst[0].simplified();
            x=1.3*lst[1].toDouble();
            y=1.3*lst[2].toDouble();
            z=1.3*lst[3].toDouble();

            vector3 v(x,y,z);


            HAtom * atom = new  HAtom();
            atom->setSymbol(atomLabel);
            atom->setCartesian(x,y,z);
            atomList.push_back(atom);

            if(!findFirstHydrogen)
            if(atomLabel=="H")  {
                IndexHydrogen=atomList.size()-1;
                findFirstHydrogen=true;
            }
        }

        if(!isHotDefined)
            hotIndex=IndexHydrogen;

        //second line
        line=file.readLine().simplified();
        unsigned int _numBonds=line.toInt();
        for (unsigned int i=0;i<_numBonds;i++)
        {
            line=file.readLine().simplified();
            lst=line.split(" ",Qt::SkipEmptyParts);
            if  (lst.size()!=3) continue;

            unsigned int bondorder=lst[2].toInt();

            //aromatic
            if(bondorder==4) // order = 4 is defined in gaussian view
            {
                bondorder=6;  // order = 6
                isExistedAromaticBond=true;
                getAtombyIndex(lst[0].toInt()-1)->setAromatic(true);
                getAtombyIndex(lst[1].toInt()-1)->setAromatic(true);
            }

            HBond * bond = new HBond(lst[0].toInt()-1,lst[1].toInt()-1,bondorder);

            //cout << bond->atomIndex0()+1 << " "<<bond->atomIndex1()+1<< " " << bond->getBondOrder()<<endl;
            bondList.push_back(bond);

            getAtombyIndex(lst[0].toInt()-1)->appendLinkdedAtomId(lst[1].toInt()-1);
            getAtombyIndex(lst[1].toInt()-1)->appendLinkdedAtomId(lst[0].toInt()-1);
        }
    }

    file.close();


    //all the hydrogen atoms need to be elongated again



    centralize();



    perceivePlaneBonds();
}


void templateMol::displayBondList()
{
    cout << "\n\nBonds of the molecule:"<<endl;
    for (unsigned int i=0;i<numBonds();i++) {
        cout << i+1 <<": "<<getBondbyIndex(i)->atomIndex0()+1<< "  ";
        cout <<getBondbyIndex(i)->atomIndex1()+1<<" ";
        cout << getBondbyIndex(i)->getBondOrder()<<endl;
    }
    cout << endl<< endl;
}

void templateMol::displayAtomList()
{
    double x,y,z;
    for (unsigned int i=0;i<numAtoms();i++)
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




void templateMol::perceivePlaneBondbyId(unsigned int id)
{
    if(getBondbyIndex(id)->getBondOrder()<2)
        return;

    unsigned int idx0= getBondbyIndex(id)->atomIndex0();
    unsigned int idx1= getBondbyIndex(id)->atomIndex1();

    //initialize
    vector3 v=getAtomPosbyIndex(idx1)-getAtomPosbyIndex(idx0);
    vector3 v1;
    v.createOrthoVector(v1);
    getBondbyIndex(id)->setPlane(v1);


    unsigned int idx2=100001;

    HAtom * atom=getAtombyIndex(idx0);
    if (atom->numLinkdedAtoms()>1)
    for (unsigned int i=0;i<atom->numLinkdedAtoms();i++)
    {
         if (atom->getLinkdedAtomId(i)==idx1)
            continue;
         if(getAtombyIndex(i)->atomicNum()<1)
             continue;
        idx2=atom->getLinkdedAtomId(i);

        if(getAtombyIndex(idx2)->atomicNum()<1)
            continue;
        break;
    }

    if (idx2>100000)
    {
        atom=getAtombyIndex(idx1);
        if (atom->numLinkdedAtoms()>1)
        for (unsigned int i=0;i<atom->numLinkdedAtoms();i++)
        {
            if (atom->getLinkdedAtomId(i)==idx0)
                continue;
            idx2=atom->getLinkdedAtomId(i);

            if(getAtombyIndex(idx2)->atomicNum()<1)
                continue;
            break;
        }
    }

    if (idx2>100000) return;

    v=cross(getAtomPosbyIndex(idx1)-getAtomPosbyIndex(idx0),
       getAtomPosbyIndex(idx1)-getAtomPosbyIndex(idx2));

    //cout <<  " find ok "<<endl;
    getBondbyIndex(id)->setPlane(v.normalize());
}



void templateMol::perceivePlaneBonds()
{
   unsigned int  size=numBonds();

    if (size<1) return;

    for (unsigned int i=0;i<size;i++) {
        unsigned int ob=getBondbyIndex(i)->getBondOrder();
        if (ob==2 || ob==3 || ob==6)
            perceivePlaneBondbyId(i);
    }
}




void templateMol::buildOBMol()
{
    obmol.Clear();
    obmol.BeginModify();

    unsigned int idx=0;
    foreach(HAtom * atom, atomList)
    {
        OpenBabel::OBAtom *a = obmol.NewAtom();

        a->Clear();
        a->SetVector(atom->x(), atom->y(), atom->z());
        a->SetAtomicNum(atom->atomicNum());
        a->SetType(atom->Symbol().toStdString().c_str());
        a->SetId(idx+1);
        idx++;
    }


    //copy bonds
    if (numBonds()>0) {
        QVector<HBond *>::iterator iter1;
        for (iter1=bondList.begin();iter1!=bondList.end();iter1++)
        {
            obmol.AddBond((*iter1)->atomIndex0() + 1,
                          (*iter1)->atomIndex1() + 1,
                          (*iter1)->getBondOrder());
        }
    }

    obmol.EndModify(true);
    //obmol.SetTotalSpinMultiplicity(1);
    //obmol.SetPartialChargesPerceived();
}

void templateMol::displayRingList()
{
    cout << "\nRings inside molecule:"<<endl;
    unsigned int size=numRings();
    if (size<1) {
        cout<< "No ring existed!"<<endl;
        return;
    }


    for (unsigned int k=0;k<size;k++)
    {
         cout << k+1<< " "<< ringList[k]->atomIdList.size() << ": ";
         for (unsigned int l=0;l<ringList[k]->atomIdList.size();l++)
         {
             cout << ringList[k]->atomIdList[l]+1<< " ";
         }
         cout << endl;
    }
    cout << endl;
}


void templateMol::perceiveRings()
{
    if (numBonds() < 1)  return;
    buildOBMol();

    std::vector<OpenBabel::OBRing *> obrings;
    obmol.ConnectTheDots();
    obmol.PerceiveBondOrders();

    obmol.SetAromaticPerceived(true);
    obrings = obmol.GetSSSR();

    OpenBabel::OBAromaticTyper aromaticTyper;
    aromaticTyper.AssignAromaticFlags (obmol);


    OpenBabel::vector3  center, norm1, norm2;

    foreach(OpenBabel::OBRing *r, obrings)
    {
          //unsigned int nAtoms=r->Size();

          HRing * ring = new HRing();
          ring->atomIdList.clear();

          vector<int>::iterator j;
          for(j = r->_path.begin(); j != r->_path.end(); ++j)
              ring->atomIdList.push_back( (*j)-1);

          r->findCenterAndNormal(center,norm1,norm2);
          ring->center.Set(center.x(),center.y(),center.z());
          ring->norm.Set(norm1.x(),norm1.y(),norm1.z());

          unsigned int size=r->Size();

          //radius
          double radius=50.0;
          for (unsigned int i=0;i<size;i++)
          {
              double distToCenter=(getAtomPosbyIndex(ring->atomIdList[i])-ring->center).length();
              if(distToCenter < radius)
                  radius = distToCenter;
          }

          ring->size=ring->atomIdList.size();
          ring->radius=radius;
          ring->isAromatic=r->IsAromatic();
          ringList.push_back(ring);     
    }

    //displayRingList();

    if (bondList.size() <3) return;
    if (ringList.size()<1) return;

   //update bond information
    vector<unsigned int>::iterator it,it1;
    int i,k;
    for (i=0;i<bondList.size();i++)
    {
        for (k=0;k<ringList.size();k++)
        {
            it=std::find(ringList[k]->atomIdList.begin(),
                         ringList[k]->atomIdList.end(),
                         bondList[i]->atomIndex0());
            if(it==ringList[k]->atomIdList.end()) continue;

            it1=std::find(ringList[k]->atomIdList.begin(),
                          ringList[k]->atomIdList.end(),
                          bondList[i]->atomIndex1());
            if(it1==ringList[k]->atomIdList.end()) continue;

            bondList[i]->appendRingId(k);
            if(bondList[i]->getBondOrder()>1)
                bondList[i]->setAromatic(true);
            //bondList[i]->setBondOrder(6);
        }
    }
}





void templateMol::rotate(vector3 v, double angle)
{
    double  ele[3][3];

    double ang = angle*3.14159265359/180.0;

    double c = cos(ang), t=1-c,  s = sin(ang);


    vector3 vec = v;
    v.normalize();

    double   x = vec[0],
             y = vec[1],
             z = vec[2];

      ele[0][0] = x*x*t+c ;
      ele[0][1] = x*y*t-z*s;
      ele[0][2] = x*z*t+y*s;

      ele[1][0] = x*y*t+z*s;
      ele[1][1] = y*y*t+c;
      ele[1][2] = y*z*t-x*s;

      ele[2][0] = x*z*t-y*s;
      ele[2][1] = y*z*t+x*s;
      ele[2][2] = z*z*t+c;

      vector3  a,b;

      for (unsigned int i=0;i<numAtoms();i++)
      {
          a=atomList[i]->Pos();
          b.SetX(  a.x()*ele[0][0] + a.y()*ele[0][1] + a.z()*ele[0][2] );
          b.SetY(  a.x()*ele[1][0] + a.y()*ele[1][1] + a.z()*ele[1][2] );
          b.SetZ(  a.x()*ele[2][0] + a.y()*ele[2][1] + a.z()*ele[2][2]);

          atomList[i]->setPos(b);
      }


}

void templateMol::rotate(matrix3x3 & m)
{
    vector3  a,b;

    for (unsigned int i=0;i<numAtoms();i++)
    {
        a=atomList[i]->Pos();

        b.SetX(  a.x()*m(0,0)  + a.y()*m(0,1) + a.z()*m(0,2)  );
        b.SetY(  a.x()*m(1,0)  + a.y()*m(1,1) + a.z()*m(1,2)  );
        b.SetZ(  a.x()*m(2,0)  + a.y()*m(2,1) + a.z()*m(2,2) );

        atomList[i]->setPos(b);
    }
}


void templateMol::translate(vector3 v)
{
    for (unsigned int  i=0;i<numAtoms();i++)
        atomList[i]->setPos(atomList[i]->Pos()+v);

}




void templateMol::clearAll()
{
    clearAtoms();
    clearBonds();
    clearRings();
}

void templateMol::clearRings()
{
    if (ringList.size() > 0)
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


void templateMol::clearAtoms()
{
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



void templateMol::clearBonds()
{
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

   // for (auto iter=atomList.begin();iter!=atomList.end();iter++)
   //      (*iter)->clearLinkedAtomId();

}
