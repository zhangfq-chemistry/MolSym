#ifndef TEMPLATEVIEW3D_H
#define TEMPLATEVIEW3D_H

#include <QObject>
#include <QWidget>
#include <QColor>

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
#include <vtkTransform.h>
#include <vtkCapsuleSource.h>
#include <QObject>

#include <vtkCamera.h>
#include <vtkMatrix4x4.h>

#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkActorCollection.h>

#include <QColor>
#include <QVector>
#include <QList>
#include <vector>

#include "math/vector3.h"
#include <QVTKInteractor.h>


using namespace std;
class HMol ;
class templateMol;
class MainWindow;

class actorInteractorStyleT;

class TemplateView3d : public QVTKOpenGLNativeWidget
{
  Q_OBJECT
public:
  explicit TemplateView3d(QWidget *parent = 0);
    ~TemplateView3d() {clearAll();}

    void initializeActor (vtkActor * );
    void clearAll();

    void updateView ();


    void setMainWindow(MainWindow *);
    void setMolT (templateMol  *);
    void renderMol();

    void renderBonds();
    void renderAtoms();
    void renderSelectedAtom();


    void renderWeakBond(unsigned int );
    void renderAromaticBond(unsigned int );
    void renderTripleBond(unsigned int );
    void renderDoubleBond(unsigned int );
    void renderSingleBond(unsigned int );
    void renderQuadrupleBond(unsigned int );
    void renderAromaticBond_insideRings(unsigned int );

    void renderRingCenter( );
    void renderAromaticRings( );
    bool isKekuleStyle () {return _isKekuleStyle;}
    void setKekuleStyle (bool b) {_isKekuleStyle=b;}


    void loadTemplateMol (QString );
    void loadMetalTemplateMol (QString );


    unsigned int numActors();

    bool isHydrogenAtom(uint idx);
    void setSelectedAtomId (unsigned int i) {selectAtomId=i;}
    unsigned int getSelectedAtomId () {return selectAtomId;}
    void removeHighlightLabel();

    void setAtomLabelVisible(bool t){_isAtomLabelVisible=t;}
    bool isAtomLabelVisible() {return _isAtomLabelVisible;}

    void setFirstTime() {isfirstTime=true;}

    void setLinkedMode() ;

    //virtual void closeEvent(QCloseEvent * event);

private:
    bool  isfirstTime,_isLinkedMode;
    double scale; //display scale of atom
    double  bondRadius;
    templateMol  * tmol;
    actorInteractorStyleT * interactor;
    MainWindow * mainWindow;
    vtkSmartPointer<vtkCamera> camera;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vector3 bkcolor;
    vector3 selectedColor;
    unsigned int selectAtomId;
    bool _isAtomLabelVisible;
    bool _isKekuleStyle=true;
};




#endif // TEMPLATEVIEW3D_H
