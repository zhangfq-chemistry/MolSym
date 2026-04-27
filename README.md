(1) Inserting these to  definiation of  vtkActor.h
//------------------------------------------
  int  _id, _idMol,  _idMO,  _idAtom, _idAtom1, _idBond, _idUnitCellBox, _idRing;
  int  _idAtomLabel,  _idBondLabel,  _idChargeLabel, _idFreeValenceLabel;
  double  _color[3];
  int  _bondorder;

  void initialId();
  void  setBondOrder (int b) {_bondorder=b;}
  int   getBondOrder () {return  _bondorder;}
  
  void setRingId(int i) {_idRing=i;}
  int getRingId () {return  _idRing;}

public:	
  void setId (int i) {_id=i;}
  void setMolId  ( int i ) {_idMol=i;}
  void setMOId   ( int i ) {_idMO=i;}
  void setAtomId ( int i ) {_idAtom=i;}
  void setAtomId1 ( int i ) {_idAtom1=i;}
  void setBondId ( int i ) {_idBond=i;}

  void setUnitCellBoxId ( int i ) {_idUnitCellBox=i;}

  int  Id () {return _id;}
  int  MolId () {return _idMol;}
  int  MOId ()  {return _idMO;}

  int  AtomId () {return _idAtom;} 
  int  AtomId1 () {return _idAtom1;}

  int  BondId () {return _idBond;}
  
   void setAtomLabelId (int i) {_idAtomLabel=i;}
    void setBondLabelId (int i) {_idBondLabel=i;}
   void setChargeLabelId (int i) {_idChargeLabel=i;}
   void setFreeValenceLabelId (int i) {_idFreeValenceLabel=i;}
    
   int   getAtomLabelId () {return  _idAtomLabel;}
   int   getBondabelId() {return  _idBondLabel;}
   int   getChargeLabelId () {return  _idChargeLabel;} 
    int  getFreeValenceLabelId () {return  _idFreeValenceLabel;} 

  int  UnitCellBoxId () {return _idUnitCellBox;} 

  void storeColor(double r,double g, double b) ;
  void recoverInitialColor ()  ;

  void vtkActor::storeColor(double r,double g, double b) 
  {	
  _color[0]=r;	_color[1]=g;	_color[2]=b;
  } 
  
 void vtkActor::recoverInitialColor () 
 {
    this->GetProperty()->SetColor( _color); 
 }
//----------------------------------------------
(2)compile and install QtColorWidgets/openbabel in 3rd
(3) install xtb from website

<img width="661" height="662" alt="symmetry-4" src="https://github.com/user-attachments/assets/e6e4f8d9-ff59-491c-9248-ad894849f72d" />
<img width="1920" height="1080" alt="symmetry-3" src="https://github.com/user-attachments/assets/27ea7f72-5501-48e2-aee0-8bc59bc3c15f" />
<img width="1920" height="1080" alt="symmetry-2" src="https://github.com/user-attachments/assets/ca13c7f6-3d3d-48aa-9bcb-7e96b87505e3" />
<img width="1920" height="1080" alt="symmetry-1" src="https://github.com/user-attachments/assets/5dacb54d-7fe5-4a82-b025-9364b41cb8ab" />
