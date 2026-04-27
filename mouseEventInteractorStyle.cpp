#include "mouseEventInteractorStyle.h"

//#include <utils/utils.h>
//#include <vtkutils/vtkutils.h>

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPointPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

#include <QDebug>
#include "utils.h"




vtkStandardNewMacro(MouseEventInteractorStyle);
MouseEventInteractorStyle::MouseEventInteractorStyle(QObject *parent) : QObject(parent)
{
     HighlightColor = Qt::yellow;
     pickedActor = vtkSmartPointer<vtkActor>::New();


    // m_moveActor=true;
}

void MouseEventInteractorStyle::OnRightButtonDown()
{
    this->StartRotate();
}

void MouseEventInteractorStyle::OnRightButtonUp()
{
    this->EndRotate();
}

void MouseEventInteractorStyle::OnLeftButtonDown()
{
    //updateCurrentPos();
    //getPressedActor();

    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

    int x = GetInteractor()->GetEventPosition()[0];
    int y = GetInteractor()->GetEventPosition()[1];

    VTK_CREATE(vtkPicker, picker);
    picker->SetTolerance(0.001);
    picker->Pick(x, y, 0, this->GetDefaultRenderer());
    pickedActor = picker->GetActor();

    if(pickedActor != nullptr) {
        double clrArr[3];
        vtkColor(HighlightColor, clrArr);
        //pickedActor->GetProperty()->SetColor(clrArr);
    }


    emit mousePressed(m_pos);
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void MouseEventInteractorStyle::OnLeftButtonUp()
{
    vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    updateCurrentPos();

    pickedActor = vtkSmartPointer<vtkActor>::New();

    emit mouseReleased(m_pos);
}

void MouseEventInteractorStyle::OnMouseMove()
{
    if (this->State == VTKIS_ROTATE)
        vtkInteractorStyleTrackballCamera::OnMouseMove();

    updateCurrentPos();

    if (m_moveActor && pickedActor) {
        pickedActor->SetPosition(m_pos);
        GetDefaultRenderer()->GetRenderWindow()->Render();
    } else {
        emit mouseMoved(m_pos);
    }
}

void MouseEventInteractorStyle::OnKeyDown()
{
    vtkInteractorStyleTrackballCamera::OnKeyDown();
    emit ctrlPressed(GetInteractor()->GetControlKey());
}

void MouseEventInteractorStyle::OnKeyUp()
{
    vtkInteractorStyleTrackballCamera::OnKeyUp();
    emit ctrlPressed(GetInteractor()->GetControlKey());
}

void MouseEventInteractorStyle::setMoveActor(bool move)
{
    m_moveActor = move;
}

bool MouseEventInteractorStyle::moveActor() const
{
    return m_moveActor;
}

void MouseEventInteractorStyle::updateCurrentPos()
{
    int x = GetInteractor()->GetEventPosition()[0];
    int y = GetInteractor()->GetEventPosition()[1];

    VTK_CREATE(vtkPointPicker, picker);
    picker->SetTolerance(0.001);
    picker->Pick(x, y, 0, GetDefaultRenderer());
    ArrayAssigner<double>()(m_pos, picker->GetPickPosition());
}

void MouseEventInteractorStyle::getPressedActor()
{
    int x = GetInteractor()->GetEventPosition()[0];
    int y = GetInteractor()->GetEventPosition()[1];

    VTK_CREATE(vtkPicker, picker);
    picker->SetTolerance(0.001);
    picker->Pick(x, y, 0, GetDefaultRenderer());
    pickedActor = picker->GetActor();
}
