#include "InteractorT.h"
#include "utils.h"
#include <vtkAbstractPropPicker.h>

vtkStandardNewMacro(actorInteractorStyleT);

actorInteractorStyleT::actorInteractorStyleT(QObject *parent) : QObject(parent)
{
    HighlightColor = Qt::yellow;

    m_pickedActor=nullptr;
    m_highlightActor=nullptr;
    molT=nullptr;

    selectedAtomId=0;

    vtkColor(HighlightColor, selectedColor);
}



void actorInteractorStyleT::setView3dT (TemplateView3d * t)
{
    view3dT=t;
}

void actorInteractorStyleT::setMolT(templateMol * m)
{
    if (m) molT=m;
    else molT=nullptr;
}

vtkCamera *  actorInteractorStyleT::getCamera()
{
    return GetDefaultRenderer()->GetActiveCamera();
}

void actorInteractorStyleT::OnKeyPress()
{
    std::string key=GetInteractor()->GetKeySym();
    //cout << key<<endl;

    if(key == "l")
    {
        view3dT->setAtomLabelVisible(!view3dT->isAtomLabelVisible());
        view3dT->renderMol();
    }

    // Forward events
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}



void actorInteractorStyleT::OnLeftButtonDown()
{
    view3dT->setLinkedMode();

    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

    vtkAssemblyPath *path = nullptr;
    int * eventPos = GetInteractor()->GetEventPosition();
    this->FindPokedRenderer(eventPos[0], eventPos[1]);
    GetInteractor()->StartPickCallback();
    auto picker = vtkAbstractPropPicker::SafeDownCast(GetInteractor()->GetPicker());

    if ( picker != nullptr )
    {
        picker->Pick(eventPos[0], eventPos[1], 0.0, this->CurrentRenderer);
        path = picker->GetPath();
    }

    m_pickedActor=nullptr;
    m_pickedActor = picker->GetActor();
    if (!m_pickedActor) return;

    if(!view3dT->isHydrogenAtom(m_pickedActor->AtomId()) )
            return;

    removeHighlightActor();
    m_highlightActor=m_pickedActor;


    //remember
    selectedAtomId=m_highlightActor->AtomId();
    view3dT->setSelectedAtomId(selectedAtomId);
    view3dT->renderSelectedAtom();
    view3dT->update();
    view3dT->setLinkedMode();
}

void actorInteractorStyleT::highlightActor()
{
     this->HighlightProp(m_highlightActor);
}


void actorInteractorStyleT::removeHighlightActor()
{
    view3dT->removeHighlightLabel();
    this->HighlightProp(nullptr);
}


unsigned int  actorInteractorStyleT::getSelectedAtomId ()
{
    return selectedAtomId;
}
