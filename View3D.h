#ifndef VIEW3D
#define VIEW3D

#include <QWidget>
#include <QColor>
#include <vector>
#include <QMap>

//#include <QVTKOpenGLWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkDataSet.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>

#include <vtkImplicitPlaneWidget2.h>
#include <vtkImplicitPlaneRepresentation.h>



#include <vtkOutlineFilter.h>
#include <vtkStripper.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkImplicitBoolean.h>

#include <vtkCutter.h>
#include <vtkFillHolesFilter.h>
#include <vtkStripper.h>
#include <vtkFeatureEdges.h>
#include <vtkRegularPolygonSource.h>
//#include <QVTKOpenGLWidget.h>
#include <QVTKOpenGLWindow.h>
#include <vtkDelaunay2D.h>
#include <vtkLookupTable.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkPolyLine.h>
#include <vtkLineSource.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkPNGWriter.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProp3DCollection.h>

#include "vtkbool/vtkPolyDataBooleanFilter.h"

#include "Mol.h"
#include "math/vector3.h"
#include <QTimer>

//#include "mouseEventInteractorStyle.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QVTKInteractor.h>
#include "InteractorArea.h"

using namespace std;
class HMol ;
class MainWindow;
class templateMol;
class TemplateView3d;
class OperatorTree;

//enum MouseOperationType {PencilDrawing, LinkTemplate, Rotation, Translation, SingleSelection, MultiSelection };



class View3D : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    explicit View3D(QWidget *parent = 0);
    ~View3D() {clearAll();}

    void dragEnterEvent(QDragEnterEvent * event) override;
    void dropEvent(QDropEvent * event) override;


  // Add a data set to the scene
  void addDataSet(vtkSmartPointer<vtkDataSet> dataSet);

  // Remove the data set from the scene
  void removeLastActor();
  void removeAllActors();
  void removeDynamicLineActor();


  void copyMol();

  HMol  * mol, * mol_movie=nullptr;
  MainWindow * mainWindow;
  templateMol * tmol;
  TemplateView3d  * view3dT;

  void setTemplateView3d (TemplateView3d  *);

  HMol  * getMol ();
  MainWindow * getMainWindow () ;
  void refresh();


  MouseOperationType mouseOperationMode;
  MouseOperationType currentMouseOperationMode();


  void setDefaultOperationMode();

  void setPencilDrawMode();
  void setLinkTemplateMode();
  void setRotateMode();
  void setTranslateMode();
  void setSelectionMode();

  void setOperateSelectedMode();


  bool isLinkTemplateMode() ;
  bool isPencilDrawMode();
  bool isSingleSelectionMode();
  bool isSelectionMode();

  bool isRotateMode();
  bool isTranslateMode();




public:
  //int idx;
 // void setId(int id){idx=id;}
 // int getId(){return idx;}

  bool  _isXYZAxisVisible;
  bool  isPGVisible;

  bool isXYZAxisVisible () {return _isXYZAxisVisible;}
  void setXYZAxisVisible (bool t);



public:
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkPlane> plane;
    vtkSmartPointer<vtkClipPolyData> clipper_pos,clipper_neg;

    unsigned int numActors();
    void renderMol(HMol *  );
    void renderMol() {refresh();}
    void renderAtoms();
    void renderBonds();


    double scale_movie=1.0;
    double opacity_movie=0.8;
    void setMovieScale(double s) {scale_movie=s;}
    void setOpacity_movie(double s){opacity_movie=s;}
    double getOpacity_movie(){return opacity_movie;}

    void renderAtoms_movie();
    void renderBonds_movie();
    void renderRings_movie();

    void renderAxis();
    void renderAtomLabel();
    void renderBondLabel();

    void renderLine(double p0[3],double p1[3],float width,double color[3],vtkTransform * transform);
    void renderLine (vector3 beg, vector3 end,float width,double color[3]);
    void renderLineCenter (vector3 beg, vector3 end,float width,double color[3]);
    void renderLines (vector3 a, vector3 b, vector3 c, vector3 shift, float width,double color[3]);

    vector3 getBackgroundColor() {return bkcolor;}
    void setBKColor( float x, float y, float z );
    void setBKColor(vector3 v);
    void writePNG(QString );

    void renderTube(vector3 beg, vector3 end, double radius, double color[3]);
    void renderTubeCenter(vector3 beg, vector3 end, double radius, double color[3]);
    void renderTube(vector3 beg, vector3 end, vector3 center,double radius, double color[3]);
    void renderArcTube(vector3 beg, vector3 end, vector3 center, double color[3], unsigned int id);

    void setCenter (vector3 c){center=c;}

    void updateView();

    vtkCamera * Camera() {return camera;}

    vtkSmartPointer<vtkCamera> camera;
    void buildC2_Sybmol(double );

    void setHorizontalC2_type(unsigned int t)  {horizontalC2_type=t;}
    void renderHorizontalC2(unsigned int type=1,double angle=0.0,double length=1.0);
    void renderHorizontalC2(double angle,double length, double color[3]);
    void renderVerticalC2(double angle,double length, double color[3]);


    void renderHorizontalCircle(double color[3]);

    void renderC1();
    void renderCs();
    void renderCi( );
    void renderCenter(double radius,double color[3]);

    void renderCn();
    void renderCnv(unsigned int );
    void renderCnh(unsigned int );


    void renderDn(unsigned int n=1, bool renderPrincipal=true);
    void renderDnd(unsigned int );
    void renderDnh(unsigned int );


    void renderT();
    void renderO();
    void renderI();

    void renderTd();
    void renderTh();
    void renderOh();
    void renderIh();

    void renderInverseIn(unsigned int n,double color[3]);
    void renderSn(unsigned int );
    void renderCoov();

    void renderDooh();
    void render_Circle();

    void renderPrincipleCylinder();
    void renderPrincipleCylinder(unsigned int order,double color[3]);

    void renderVerticalMirror(int type=1);
    void renderSigmaV(double angle=0.0);
    void renderSigmaD(double angle=0.0);
    void renderSigmaH();

    double getOpacityV() {return OpacityV;}
    double getOpacityD() {return OpacityD;}
    double getOpacityH() {return OpacityH;}


    void  setColorSigmaV(vector3 v);
    void  setColorSigmaD(vector3 v) ;
    void  setColorSigmaH(vector3 v) ;
    void  setColorSigmaV(double r,double g, double b) {color_sigmaV[0]=r;color_sigmaV[1]=g;color_sigmaV[2]=b;}
    void  setColorSigmaD(double r,double g, double b) {color_sigmaD[0]=r;color_sigmaD[1]=g;color_sigmaD[2]=b;}
    void  setColorSigmaH(double r,double g, double b) {color_sigmaH[0]=r;color_sigmaH[1]=g;color_sigmaH[2]=b;}


    vector3 getSigmaVColor();
    vector3 getSigmaDColor() ;
    vector3 getSigmaHColor() ;

    void setOpacityV(double v) {OpacityV=v;}
    void setOpacityD(double v) {OpacityD=v;}
    void setOpacityH(double v) {OpacityH=v;}

    void setColorSigmaV(double c[3]) ;
    void setColorSigmaD(double c[3]) ;
    void setColorSigmaH(double c[3]);
    void setColorCn(double c[3]) ;
    void setColorI2n(double c[3]) ;
    void setColorC2_1(double c[3]) ;
    void setColorC2_2(double c[3]) ;


    void setColorCn(double r,double g, double b) {color_Cn[0]=r;color_Cn[1]=g;color_Cn[2]=b;}
    void setColorC2_1(double r,double g, double b) {color1_C2[0]=r;color1_C2[1]=g;color1_C2[2]=b;}
    void setColorC2_2(double r,double g, double b) {color2_C2[0]=r;color2_C2[1]=g;color2_C2[2]=b;}
    void setColorC3(double r,double g, double b) {color_C3[0]=r;color_C3[1]=g;color_C3[2]=b;}

    void setColorCn(vector3 c) ;
    void setColorI2n(vector3 c) ;
    void setColorC2_1(vector3 c) ;
    void setColorC2_2(vector3 c) ;

    vector3 getColorCn();
    vector3 getColorIn() ;
    vector3 getColorC2_1() ;
    vector3 getColorC2_2();
    vector3 getColorC3();

    vector3 getColorAromaticRing();
    vector3 getFilledColorAromaticRing();
    void setColorAromaticRing(double r,double g, double b);
    void setFilledColorAromaticRing(double r,double g, double b);

    bool isAtomLabelVisible ();
    void setAtomLabelVisible(bool b);


    bool isBondLabelVisible ();
    void setBondLabelVisible(bool b);


    bool isSymmetryElementVisible() {return isPGVisible;}
    void setSymmetryElementVisible(bool b);
    void setHorizontalC2Visible(bool b);

    void setSigmaHVisible(bool b);
    void setSigmaVVisible(bool b);
    void setSigmaDVisible(bool b);

    bool isSigmaHVisible() {return _isSigmaHVisible;}
    bool isSigmaVVisible() {return _isSigmaVVisible;}
    bool isSigmaDVisible() {return _isSigmaDVisible;}

    bool isCnVisible() {return _isCnVisible;}
    void setCnVisible(bool b);

    bool isI2nVisible() {return _isI2nVisible;}
    void setI2nVisible(bool b) {_isI2nVisible=b;}

    bool isCiVisible() {return _isCiVisible;}
    void setCiVisible(bool b) {_isCiVisible=b;}
    void setCsVisible(bool b) {isCsVisible=b;}

    void setC2_1_Visible(bool b) {_isC2_1_Visible=b;}
    void setC2_2_Visible(bool b) {_isC2_2_Visible=b;}

    bool isC2_Visible () {return _isC2_Visible;}
    void setC2_Visible(bool b) {_isC2_Visible=_isC2_2_Visible=_isC2_1_Visible=b;}

    bool isC3_Visible() {return _isC3_Visible;}
    bool isC4_Visible() {return _isC4_Visible;}
    bool isC5_Visible() {return _isC5_Visible;}
    bool isC6_Visible() {return _isC6_Visible;}


    void initSigmaH_list(uint );
    void initSigmaV_list(uint );
    void initSigmaD_list(uint );
    void initSigma_list(uint );

    void initC2_1_Horizaontal_list(uint );
    void initC2_2_Horizaontal_list(uint );
    void initC2_Horizontal_list(uint );

    void initC2_list(uint);
    void initC3_list(uint);
    void initC4_list(uint);
    void initC5_list(uint);
    void initS4_list(uint);

    void setC2_1_Visible(uint idx, uint b);
    void setC2_2_Visible(uint idx, uint b);
    void setC2_Visible  (uint idx, uint b);

    void setSigma_Visible (uint idx, uint b);
    void setSigmaV_Visible(uint idx, uint b);
    void setSigmaD_Visible(uint idx, uint b);
    void setSigmaH_Visible(uint idx, uint b);

    void setVisibleAll_PG();
    void setInvisibleAll_PG();

    void setAllC2_1_Visible(uint b);
    void setAllC2_2_Visible( uint b);
    void setAllC2_Visible(uint b);

    void setAllSigma_Visible(uint b);
    void setAllSigmaV_Visible(uint b);
    void setAllSigmaD_Visible(uint b);
    void setAllSigmaH_Visible(uint b);

    void setAllS4_Visible(uint b);
    void setAllC3_Visible(uint b);
    void setAllC4_Visible(uint b);
    void setAllC5_Visible(uint b);

    bool isOneOfC2_1_Visible();
    bool isOneOfC2_2_Visible();
    bool isOneOfC2_Visible();


    void setS4_Visible(uint idx, uint b);
    void setC3_Visible(uint idx, uint b);
    void setC4_Visible(uint idx, uint b);
    void setC5_Visible(uint idx, uint b);


    void setC3_Visible(bool b);
    void setC4_Visible(bool b);

    void setC5_Visible(bool b);
    void setC6_Visible(bool b);

    void setMolVisible(bool b) {isMol_Visible=b;}
    void setHydrogenVisible(bool b) {isHydrogen_Visible=b;}

    bool isHydrogenHide() {return (!isHydrogen_Visible);}
    bool isMolHide() {return (!isMol_Visible);}



   // void setScale(double s) {scale=s;updateView();}

    void setSymmetry(QString s);
    QString getSymmetry () {return PointGroup;}

    void renderSimpleSingleBond(unsigned int);
    void renderHydrogenBond(unsigned int);
    void renderWeakBond(unsigned int);

    void renderTripleBond(unsigned int);
    void renderDoubleBond(unsigned int);
    void renderSingleBond(unsigned int);
    void renderQuadrupleBond(unsigned int);

    void renderAromaticRings();
    void renderAromaticBond(uint);
    void renderAromaticBond_insideRings(uint);

    void renderSimpleSingleBond_movie(unsigned int );
    void renderHydrogenBond_movie(unsigned int );
    void renderWeakBond_movie(unsigned int );

    void renderTripleBond_movie(unsigned int );
    void renderDoubleBond_movie(unsigned int );
    void renderSingleBond_movie(unsigned int );
    void renderQuadrupleBond_movie(unsigned int );

    void renderAromaticRings_movie();
    void renderAromaticBond_movie(uint);
    void renderAromaticBond_insideRings_movie(uint);

    void removeSelectedAtomsBonds();
    void removeSelectedAtoms();
    void removeSelectedBonds();
    void removeSelectedRings();

    void renderCubeSkeleton(double );

    void centralize();
    void periodicTable();


public slots:
  void zoomToExtent();
  void onViewXY();
  void onViewXZ();
  void onViewYZ();
  void onViewReset();

  void onView_rotateClockwise(double degree);
  void onView_rotateCounterClockwise(double degree);
  void onView_rotateUp(double degree);
  void onView_rotateDown(double degree);
  void onView_rotateLeft(double degree);
  void onView_rotateRight(double degree);

  void setMolTypeStick() ;
  void setMolTypeBallStick() ;
  void setMolTypeLine();


  void clearAll();
  void renderSymmetryElement();

  void setAreaInteractor();


public:
  void setSelectedUpdateFromRendering(bool o) {updateSelectedFromRendering=o;}
  bool isSelectedUpdateFromRendering() {return updateSelectedFromRendering;}

  bool isKekuleStyle () {return _isKekuleStyle;}
  void setKekuleStyle (bool b) {_isKekuleStyle=b;}


  double getDiscThickness (){ return discThickness;}
  void   setDiscThickness (double a){discThickness=a; buildC2_Sybmol(discThickness);}

  void setVerticalLength (double a){verticalLength=a;}
  void setHorizontalLengh (double a){HorizontalLengh=a;}

  void setDiscSize_Cn(double a) {discCn_scale=a;}
  void setDiscSize_C2(double a) {discC2_scale=a;}
  void setDiscSize_C3(double a) {discC3_scale=a;}


  double getVerticalLength (){return verticalLength;}
  double getHorizontalLengh (){return HorizontalLengh;}

  double getDiscSize_Cn() {return discCn_scale;}
  double getDiscSize_C2 () {return discC2_scale;}
  double getDiscSize_C3 () {return discC3_scale;}

  double getThicknessCn_Scale () {return thickness_Cn_Scale;}
  double getThicknessC2_Scale () {return thickness_C2_Scale;}
  double getThicknessC3_Scale () {return thickness_C3_Scale;}

  void setThicknessCn_Scale (double t) {thickness_Cn_Scale=t;}
  void setThicknessC2_Scale (double t) {thickness_C2_Scale=t;}
  void setThicknessC3_Scale (double t) {thickness_C3_Scale=t;}


  double getThicknessSigmaV_Scale () {return thicknessSigmaV_Scale;}
  double getThicknessSigmaD_Scale () {return thicknessSigmaD_Scale;}
  double getThicknessSigmaH_Scale () {return thicknessSigmaH_Scale;}

  void setThicknessSigmaV_Scale (double t) {thicknessSigmaV_Scale=t;
                                            thicknessV=thicknessSigmaV0*t*2.0;}
  void setThicknessSigmaD_Scale (double t) {thicknessSigmaD_Scale=t;
                                            thicknessD=thicknessSigmaD0*t*2.0;}
  void setThicknessSigmaH_Scale (double t) {thicknessSigmaH_Scale=t;
                                            thicknessH=thicknessSigmaH0*t*2.0;}


  double getRadiusCn_Scale () {return CnRadius_Scale;}
  double getRadiusC2_Scale () {return C2Radius_Scale;}
  double getRadiusC3_Scale () {return C3Radius_Scale;}


  void setRadiusCn_Scale (double t) {CnRadius_Scale=t;
                                     Cn_Radius=principalAxisRadius*t;}
  void setRadiusC2_Scale (double t) {C2Radius_Scale=t;
                                     C2_Radius=principalAxisRadius*t;}
  void setRadiusC3_Scale (double t) {C3Radius_Scale=t;
                                     C3_Radius=principalAxisRadius*t;}


  double getCn_Length () {return Cn_Length;}
  double getC2_Length () {return C2_Length;}
  double getC3_Length () {return C3_Length;}

  void setCn_Length (double t) {Cn_Length=t;}
  void setC2_Length (double t) {C2_Length=t;}
  void setC3_Length (double t) {C3_Length=t;}


  unsigned int getOutLineType() {return outLineType;}
  void setOutLineType(unsigned int i);

  void initParameter();

  void setSigmaV_Type(unsigned int t) {typeSigmaV=t;}
  void setSigmaD_Type(unsigned int t) {typeSigmaD=t;}
  void setSigmaH_Type(unsigned int t) {typeSigmaH=t;}



  void setAtomScale(double s) {scale_atom=s*0.3;}
  void setBondScale(double s) {scale_bond=s;bondRadius=s*0.07;}

  double getAtomScale() {return scale_atom/0.3;}
  double getBondScale() {return scale_bond;}

  void setRingSizeScale(double s) {scale_ringSize=s;}
  void setRingRadiusScale(double s) {scale_ringRadius=s;}
  double getRingSizeScale() {return scale_ringSize;}
  double getRingRadiusScale() {return scale_ringRadius;}


  //pencil draw
  void    addCarbon(vector3 );
  void    addCarbon(double x, double y, double z);
  void    addCarbon_and_link2SelectedAtom(vector3, unsigned int i);

  vtkActor * getLastAtomActor() {return lastAtomActor; }
  void  linkTwoSelectedAtoms(uint, unsigned int );
  void  Link2LastAtom(vector3);
  void  Link2LastAtom(double x, double y, double z);

  void  translateSelectedAtoms(vector3 );
  void  rotateSelectedAtoms(vector3, matrix3x3 );
  void  symmetryPercept();

  vtkRenderer * getRender() {return m_renderer;}

  void  copySeleted();
  void  cutSelected();

  unsigned int numAtoms() {if (mol) return mol->NumAtoms(); return 0;}

private:
  QString copyMolData;
  vtkActor * lastAtomActor;
  double scale_ringSize,scale_ringRadius;
  vector3 LastCarbonPos;


private:
  unsigned int outLineType,typeSigmaH,typeSigmaV,typeSigmaD;
  bool _isKekuleStyle;
  bool updateSelectedFromRendering;

private:
  vtkSmartPointer<vtkRenderer> m_renderer;
  vector3 nodecolor;
  vector3 bkcolor,whiteColor;
  vector3 selectedColor;
  double scale_atom,scale_bond; //for display
  vector3 center;

  double  bondRadius,thickness_Cn;
  double  moleculeLength, principalAxisRadius;

  double  CnRadius_Scale,C2Radius_Scale,C3Radius_Scale,C4Radius_Scale;
  double  Cn_Radius,C2_Radius,C3_Radius,C4_Radius;

  double  Cn_Length_Scale,C2_Length_Scale,C3_Length_Scale,C4_Length_Scale;
  double  Cn_Length,C2_Length,C3_Length;


  double  discRadius,discCn_scale,discC2_scale,discC3_scale;

  double textColor[3],textSize;

  double color_Cn[3],color_In[3];
  double color1_C2[3],color2_C2[3],color_C3[3];

  double color_sigmaV[3],color_sigmaD[3],color_sigmaH[3];
  double yellow[3],blue[3],white[3],black[3],grey[3],brown[3];

  double OpacityH,OpacityV,OpacityD;
  double thicknessSigmaV0,thicknessSigmaD0,thicknessSigmaH0;
  double thicknessSigmaV_Scale,thicknessSigmaD_Scale,thicknessSigmaH_Scale;
  double thicknessH,thicknessD,thicknessV;


  double discThickness, thickness_Cn_Scale,thickness_C2_Scale,thickness_C3_Scale;

  bool isStick,isBallStick, isLine;
  bool _isAtomLabelVisible=false, _isBondLabelVisible=false;

  bool PrincipalAxisVisible,isHorizontalC2Visible;
  bool _isSigmaHVisible,_isSigmaVVisible,_isSigmaDVisible;
  bool _isCnVisible, _isSnVisible, _isI2nVisible;
  bool _isCiVisible, isCsVisible;
  bool _isC2_1_Visible, _isC2_2_Visible;
  bool isMol_Visible,isHydrogen_Visible;

  bool _isC2_Visible,_isC3_Visible,_isC4_Visible,_isC5_Visible,_isC6_Visible;
  bool isI2n;


  QString  PointGroup="C1";
  unsigned int  principalOrder=0;

  //vtkSmartPointer<vtkLight> light;
  vtkPolyDataBooleanFilter * bfPolyData;

  SelectAreaInteractorStyle * actorInteractorA;


   unsigned int horizontalC2_type;

   double opacity_neg,opacity_pos,opicity_node;
   vector3 negLobeColor,posLobeColor;
   double orbitalLineWidth;

   vector3 origin_SelectedAtoms;
   double length_SelectedAtoms;

   double verticalLength,HorizontalLengh;

   //selection operation
public:
   void updateMolLength();
   vector3 getOriginSelectedAtoms() {return origin_SelectedAtoms;}
   double  getLengthSelectedAtoms() {return length_SelectedAtoms;}

   vector <unsigned int > selectedAtomList, selectedRingList;
   vector < vector < unsigned int > > selectedBondList;  // <index, atom-beg, atom-end>

   vtkSmartPointer<vtkActorCollection> selectedAtomActors, selectedBondActors, selectedRingActors;
   vtkSmartPointer<vtkActorCollection> atomLabelActors,bondLabelActors;

   void updateAtomLabelState();
   void releaseAtomLabelActors();


   void releaseAllActors2();
   void releaseInteractorPickedActor();
   void releaseSelectedAtomActors();
   void releaseSelectedBondActors();
   void releaseSelectedRingActors();

   void clearSelectedAtomList();
   void clearSelectedBondList();
   void clearSelectedRingList();
   
   void updateSelectedRingListFromRingActors();
   void updateSelectedAtomListFromAtomActors();
   void updateSelectedBondListFromBondActors();
   void storeSelectBondList(unsigned int , unsigned int ); //help function

   void removefromAtomActorsList (vtkActor* );
   void removefromBondActorsList (vtkActor* );
   void removefromRingActorsList (vtkActor* );


   void releaseSingleActor(vtkActor* );
   void releaseSingleAtomActor(vtkActor* );
   void releaseSingleBondActor(vtkActor* );


   void displaySelectedAtomActorsList();

   void buildAtomLabel(vtkActor *);
   void removeAtomLabelActor (unsigned int );
   void removeAllAtomLabelActors();
   void releaseAllActors();

   void displaySelectedAtomList();
   void displaySelectedBondList();
   void displaySelectedRingList();

   unsigned int getSelectedAtomId(uint);
   unsigned int numSelectedAtoms();
   bool isAtomPicked(unsigned int );
   void renderSelectedAtomLabel();

   unsigned int getSelectedRingId(uint);
   unsigned int numSelectedRings();

   unsigned int getBondOrderSelectedById(unsigned int id);
   void getSelectedBondIds(unsigned int id, unsigned int & id1, unsigned int & id2);
   unsigned int numSelectedBonds();

   bool hasPicked(vtkActor * actor);
   void uniqueSelectAtomList();



   void loadTemplateMol (QString );
   void loadMetalTemplateMol (QString );

   void linkTemplate2Mol(unsigned int );

   void addNewTemplate(vector3 );
   void addNewTemplate(double x, double y, double z);
   void clearTemplate();


   void selectAll();
   void paste();

   double getSelectedAtomXbyIndex(unsigned int );
   double getSelectedAtomYbyIndex(unsigned int );
   double getSelectedAtomZbyIndex(unsigned int );

   void setSelectedAtomPosbyIndex(unsigned int ,double , double , double );

   void updateMol() {refresh();}


   void undo();
   void redo();
   void push2Stack(QString );

   void initializeActor (vtkActor * );

   void replaceSelelctedAtomWith(unsigned int id); //Period table

   unsigned int getPrincipalOrder() {return principalOrder;}

   void setSelectedBondOrder(unsigned int );
   unsigned int getBondOrderbyId(unsigned int );

   void reverseAromaticBond();
   void modifyAtomColor();
   void modifyAtomRadius();
   void buildRingForSelectedAtoms();
   unsigned int numSelectedObjects() ;

   bool isRingExistedforSelectedAtoms();
   void appendXAtomAtCenterofSelectedAtoms();

   void modifySelectedRing();


   void rotateLeft();
   void rotateRight();
   void rotateUp();
   void rotateDown();

   unsigned int getBondOrderbyIndex(unsigned int id) {return mol->getBondOrderbyIndex(id);}

   void addHydrogenAtomForSelected();
   bool isXExisted() {return mol->isXExisted();}
   bool isHydrogenAtomExisted() {return mol->isHydrogenAtomExisted();}

   bool canbeUndo();
   bool canbeRedo();
   bool canbePaste();
   void calculateDATC(); //distance, angle, torsion and center
   bool isHydrogenAtom(unsigned int );


   vector3  getAtomPos(unsigned int );
   vector3 getSelectedAtomPosbyIndex(unsigned int );

   void    setSelectedAtomSymbol(QString );

   void virtual focusOutEvent(QFocusEvent *) override;
   //void virtual contextMenuEvent(QContextMenuEvent *)override;


   bool firstTime;

   void updateLabelPosFunc(vtkObject*, unsigned long eid, void* clientdata, void *calldata);


   void  cleanTmpDir();
   void  generateAnimationFile(int id);

   QString executePath;
   uint nFrames,idxFrame;
   bool renderMovie=false;
   void renderMol_movie();
   void setRenderMovie(bool t) {renderMovie=t;}


   void  initIdxFrame();
   void  startMovie_rotate();
   void  startMovie_reflect();
   void  startMovie_reverse();
   void  startMovie_rotate_reflect();
   void  startMovie_rotate_reverse();

   void  genRotationReversionAtoms(vector3, double );
   void  genRotationReflectionAtoms(vector3, double );
   void  genReflectionAtoms();
   void  genReverseAtoms();
   void  clearMovieData();


   void setOperatorTree (OperatorTree *  );

   void setRotateVector_movie(vector3 v) {rotateVector_movie=v.normalize();}
   void setRotateAngle_movie(double angle){rotateAngle_movie=angle;}
   vector3 rotateVector_movie,reflectionVector_movie;
   double  rotateAngle_movie;


   void setReflectionVector_movie(vector3 v);

   void displayList_sigmaV_visible() {
       cout << "\ndisplayList_sigmaV_visible"<< endl;
       for (auto i : list_sigmaV_visible)
           cout << i << " ";
       cout << endl;
   }

   void UFF();
   void MM2();
   void Gaff();
   void MMFF94();
   void Ghemical();


private:

   //similiar to bit-vector
   vector <uint > list_sigmaH_visible;
   vector <uint > list_sigmaV_visible;
   vector <uint > list_sigmaD_visible;
   vector <uint > list_sigma_visible;


   vector <uint > list_C2_Horizontal_visible;
   vector <uint > list_C2_1_Horizontal_visible;
   vector <uint > list_C2_2_Horizontal_visible;



   vector <uint > list_C2_visible;
   vector <uint > list_C3_visible;
   vector <uint > list_C4_visible;
   vector <uint > list_C5_visible;
   vector <uint > list_S2n_visible;

   vector <uint > list_S4_visible;

   QTimer * timer = nullptr;
   OperatorTree * treeWidget = nullptr;
   double normalCamera[3];
};




#endif
