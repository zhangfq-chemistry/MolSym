#ifndef ACTORINTERACTORSTYLET_H
#define ACTORINTERACTORSTYLET_H

#include "templateView3d.h"

#include <vtkCamera.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkDataSetMapper.h>

//#include <QVTKOpenGLWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkDataSet.h>
#include <vtkTubeFilter.h>

#include "templateMol.h"
#include "mTools.h"
#include <QFile>
#include <QMessageBox>
#include "mainwindow.h"




class actorInteractorStyleT : public QObject,
                              public vtkInteractorStyleTrackballCamera
{
    Q_OBJECT
public:
    static actorInteractorStyleT *New();
    vtkTypeMacro(actorInteractorStyleT, vtkInteractorStyleTrackballCamera);



    void OnKeyPress() override;
    virtual void OnLeftButtonDown() override;
    //virtual void OnLeftButtonUp() override;

    //virtual void OnRightButtonDown() override;
    //virtual void OnRightButtonUp() override;
    //virtual void OnMouseMove() override;



    void setHighlightActor(bool highlight);
    void highlightActor(vtkActor * );
    void highlightActor();

    void setMolT (templateMol *);
    vtkCamera * getCamera();

    void setView3dT(TemplateView3d *);
    unsigned int  getSelectedAtomId ();
    void removeHighlightActor();


protected:
    explicit actorInteractorStyleT(QObject *parent = 0);


protected:
    TemplateView3d  * view3dT;
    vtkActor * m_pickedActor;
    templateMol  * molT;


    QColor HighlightColor;
    double selectedColor[3];
    unsigned int  selectedAtomId;

public:
    vtkActor *  m_highlightActor;
};




#endif // ACTORINTERACTORSTYLET_H
