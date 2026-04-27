#include "InteractorArea.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkAreaPicker.h>
#include <vtkPlanes.h>
#include <vtkProp3DCollection.h>
#include <vtkProp3D.h>
#include <QDebug>
#include <vtkTransform.h>
#include <vtkQuaternion.h>
#include <vtkMatrix3x3.h>
#include <QMenu>
#include <vtkCoordinate.h>

#include "utils.h"
#include "Mol.h"
#include "View3D.h"
#include "mainwindow.h"
#include "AreaPicker.h"
#include "atomProperty.h"

#define VTKISRBP_ORIENT  0
#define VTKISRBP_SELECT  1



vtkStandardNewMacro(SelectAreaInteractorStyle);

SelectAreaInteractorStyle::~SelectAreaInteractorStyle()
{

}


SelectAreaInteractorStyle::SelectAreaInteractorStyle(QObject *parent) : QObject(parent)
{
    HighlightColor = Qt::yellow;

    isShiftPressed=false;
    isControlPressed=false;
    isAltPressed=false;
    isMiddleButtonDown=false;
    isRightButtonDown=false;

    mol=nullptr;
    pickedActor=nullptr;

    mouseOperationMode=VTKRotation;
    NumberOfClicks = 0;

    vtkColor(HighlightColor, selectedColor);
}





MainWindow * SelectAreaInteractorStyle::getMainWindow()
{
    return view3d->getMainWindow();
}

unsigned int SelectAreaInteractorStyle::numSelectedObjects()
{
    return (view3d->numSelectedAtoms()+view3d->numSelectedBonds()+view3d->numSelectedRings());
}


unsigned int SelectAreaInteractorStyle::numSelectedAtoms()
{
    return view3d->numSelectedAtoms();
}

unsigned int SelectAreaInteractorStyle::numSelectedBonds()
{
    return view3d->numSelectedBonds();
}

unsigned int SelectAreaInteractorStyle::numSelectedRings()
{
    return view3d->numSelectedRings();
}



void  SelectAreaInteractorStyle::translateSelectedAtoms()
{
    v0=getHitModelPosition(GetInteractor()->GetEventPosition()[0],
            GetInteractor()->GetEventPosition()[1]);
    v1=getHitModelPosition(GetInteractor()->GetLastEventPosition()[0],
            GetInteractor()->GetLastEventPosition()[1]);
    view3d->translateSelectedAtoms(v0-v1);
}


void  SelectAreaInteractorStyle::rotateSelectedAtoms()
{
    vtkRenderWindowInteractor *rwi = this->Interactor;

    // first get the origin of the collection
    obj_centerV=view3d->getOriginSelectedAtoms();
    boxCenter[0]=obj_centerV[0];
    boxCenter[1]=obj_centerV[1];
    boxCenter[2]=obj_centerV[2];

    boundRadius = view3d->getLengthSelectedAtoms()*0.5;
    if(boundRadius<0.3) boundRadius=0.3;

    /*
    cout << "obj center: "<< boxCenter[0] << " ";
    cout << boxCenter[1] << " ";
    cout << boxCenter[2] << endl;
    cout << "bound radius: "<< boundRadius <<endl;
*/
    // Get the view up and view right vectors
    getCamera()->OrthogonalizeViewUp();
    getCamera()->ComputeViewPlaneNormal();
    getCamera()->GetViewUp(view_up);
    getCamera()->GetViewPlaneNormal(view_look);

    vtkMath::Normalize(view_up);
    vtkMath::Normalize(view_look);

    vtkMath::Cross(view_up, view_look, view_right);
    vtkMath::Normalize(view_right);

    // Get the furtherest point from object position+origin
    outsidept[0] = boxCenter[0] + view_right[0] * boundRadius;
    outsidept[1] = boxCenter[1] + view_right[1] * boundRadius;
    outsidept[2] = boxCenter[2] + view_right[2] * boundRadius;


    ComputeWorldToDisplay(boxCenter[0], boxCenter[1], boxCenter[2], disp_obj_center);
    ComputeWorldToDisplay(outsidept[0], outsidept[1], outsidept[2], outsidept);

    radius = sqrt(vtkMath::Distance2BetweenPoints(disp_obj_center,  outsidept));
    nxf = (rwi->GetEventPosition()[0] - disp_obj_center[0]) / radius;
    nyf = (rwi->GetEventPosition()[1] - disp_obj_center[1]) / radius;
    oxf = (rwi->GetLastEventPosition()[0] - disp_obj_center[0]) / radius;
    oyf = (rwi->GetLastEventPosition()[1] - disp_obj_center[1]) / radius;

    if (((nxf * nxf + nyf * nyf) <= 1.0) && ((oxf * oxf + oyf * oyf) <= 1.0))
    {
        newXAngle = vtkMath::DegreesFromRadians( asin( nxf ) );
        newYAngle = vtkMath::DegreesFromRadians( asin( nyf ) );
        oldXAngle = vtkMath::DegreesFromRadians( asin( oxf ) );
        oldYAngle = vtkMath::DegreesFromRadians( asin( oyf ) );

        double **rotate = new double*[2];

        rotate[0] = new double[4];
        rotate[1] = new double[4];

        rotate[0][0] = newXAngle - oldXAngle;
        rotate[0][1] = view_up[0];
        rotate[0][2] = view_up[1];
        rotate[0][3] = view_up[2];

        rotate[1][0] = oldYAngle - newYAngle;
        rotate[1][1] = view_right[0];
        rotate[1][2] = view_right[1];
        rotate[1][3] = view_right[2];


        auto newTransform =  vtkSmartPointer<vtkTransform>::New();
        newTransform->PostMultiply();
        for (int i = 0; i < 2; i++)
            newTransform->RotateWXYZ(rotate[i][0], rotate[i][1], rotate[i][2], rotate[i][3]);

        auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        newTransform->GetTranspose(matrix);
        //newTransform->GetInverse(matrix);

        delete [] rotate[0];
        delete [] rotate[1];
        delete [] rotate;

        matrix3x3 m;
        for (unsigned int i=0;i<3;i++)
            for (unsigned int j=0;j<3;j++)
                m.Set(i,j,matrix->GetElement(i,j));

        view3d->rotateSelectedAtoms(obj_centerV,m.inverse());

        //newTransform->Delete();
        //matrix->Delete();
    }
}



void SelectAreaInteractorStyle::OnKeyRelease()
{
    std::string key=GetInteractor()->GetKeySym();
    //cout << key<< " is relasse "<<endl;

    if(key == "Shift_L")
    {
        std::cout << "The Shift  was released" << std::endl;
        isShiftPressed=false;

        this->CurrentMode = VTKISRBP_ORIENT;
        view3d->setDefaultOperationMode();
    }

    if(key == "Control_L")
    {
        std::cout << "The Ctrl  was released" << std::endl;
        isControlPressed=false;

        this->CurrentMode = VTKISRBP_ORIENT;
        view3d->setDefaultOperationMode();
    }
}




void SelectAreaInteractorStyle::OnChar()
{
    std::string key=GetInteractor()->GetKeySym();

    if(key=="3") return;
    if(key=="w") return;
}

void SelectAreaInteractorStyle::OnKeyPress()
{
    this->CurrentMode = VTKISRBP_ORIENT;

    // Forward events
    //vtkInteractorStyleRubberBandPick::OnKeyPress();

    std::string key=GetInteractor()->GetKeySym();


    if(key == "Escape" || key == "escape" || key == "q" )
    {
        view3d->setDefaultOperationMode();
        view3d->releaseAllActors();

        pickedActor=nullptr;

        view3d->renderMol();
        return;
    }


    if(key == "space")
    {
        view3d->calculateDATC(); //distance, angle, torsion and center
        return;
    }

    if(key == "home")
    {
        view3d->onViewXY();
        return;
    }


    // Handle an arrow key
    if(key == "Up")
    {
        view3d->rotateUp();
        return;
    }

    if(key == "Down")
    {
        view3d->rotateDown();
        return;
    }

    if(key == "Right")
    {
        view3d->rotateRight();
        return;
    }

    if(key == "Left")
    {
        view3d->rotateLeft();
        return;
    }


    if(key == "Shift_L")
    {
        //std::cout << "The Shift  was pressed" << std::endl;
        isShiftPressed=true;
        this->CurrentMode = VTKISRBP_SELECT;
        view3d->setDefaultOperationMode();
        return;
    }

    if(key == "Control_L")
    {
        std::cout << "The Control  was pressed." << std::endl;
        isControlPressed=true;
        view3d->setDefaultOperationMode();
        return;
    }

    if(key == "Alt_L")
    {
        std::cout << "The Alter  was pressed." << std::endl;
        isAltPressed=true;
        view3d->setDefaultOperationMode();
        return;
    }




    //set bond order
    if(key == "1")
    {
        view3d->setSelectedBondOrder(1);
        view3d->refresh();
        return;
    }

    //C2
    if(key == "2")
    {
        if(view3d->numSelectedBonds()>0) {
            view3d->setSelectedBondOrder(2);
            view3d->refresh();
        }
        else {
            if(getMainWindow()->isC2_InVisible())
                getMainWindow()->setAllC2_Visible(1);
            else
                getMainWindow()->setAllC2_Visible(0);
        }
        return;
    }



    if(key == "3")
    {
        if(view3d->numSelectedBonds()>0)
            view3d->setSelectedBondOrder(3);
        else {
            if(getMainWindow()->isC3_InVisible())
                getMainWindow()->setAllC3_Visible(1);
            else
                getMainWindow()->setAllC3_Visible(0);
        }
        view3d->refresh();
        return;
    }


    if(key == "4")
    {
        if(view3d->numSelectedBonds()>0)
            view3d->setSelectedBondOrder(4);
        else
            view3d->setC4_Visible(!view3d->isC4_Visible());
        view3d->refresh();
        return;
    }



    if(key == "5")
    {
        view3d->setC5_Visible(!view3d->isC5_Visible());
        view3d->refresh();
        return;
    }


    if(key == "6")
    {
        if(view3d->numSelectedBonds()>0)
            view3d->setSelectedBondOrder(6);
        else
            view3d->setC6_Visible(!view3d->isC6_Visible());
        view3d->refresh();
        return;
    }

    if(key == "7")
    {
        if(view3d->numSelectedBonds()>0)
            view3d->setSelectedBondOrder(7);
        view3d->refresh();
        return;
    }




    //remove selected
    if(key == "Delete")
    {
        view3d->removeSelectedAtomsBonds();
        return;
    }



    // ctrl_a : selected all
    if(key == "a")
    {
        if(isControlPressed) {
            view3d->selectAll();
        }
        else
        {
            getMainWindow()->setXYZAxisVisible();
        }

        return;
    }

    if(key == "b")
    { 
        if(view3d->numSelectedAtoms()>0)
            view3d->setSelectedAtomSymbol("B");
        //else
        //    view3d->setBondLabelVisible(!view3d->isBondLabelVisible());

        view3d->refresh();
        return;
    }

    if(key == "i")
    {
        view3d->setCiVisible(!view3d->isCiVisible());
        view3d->refresh();
        return;
    }


    if(key == "c")
    {
        if(isControlPressed) {
            view3d->copySeleted();
            return;
        }

        if(view3d->numSelectedAtoms()>0)
        {
            view3d->setSelectedAtomSymbol("C");
            return;
        }

        if(!view3d->isSymmetryElementVisible())
        {
            view3d->centralize();
            return;
        }

        view3d->refresh();
        return;
    }


    //sigma-D
    if(key == "d")
    {
        if(view3d->isSymmetryElementVisible()) {
            getMainWindow()->setSigmaD_Visible();
        }
        else {
            if (view3d->numSelectedObjects()>0)
                view3d->removeSelectedAtomsBonds();
        }

        return;
    }

    if(key == "e")
    {
        view3d->periodicTable();
        return;
    }


    //find symmetry or turn to "F"
    if(key == "f")
    {
        if(view3d->numSelectedAtoms()>0)
        {
            view3d->setSelectedAtomSymbol("F");
            return;
        }

        view3d->symmetryPercept();
        return;
    }

    //sigma-H
    if(key == "h")
    {
        if(view3d->isHydrogenHide())
        {
            slot_HideHydrogenAtoms();
            return;
        }

        if(view3d->isSymmetryElementVisible()) {
            getMainWindow()->setSigmaH_Visible();
            //view3d->refresh();
            return;
        }


        if(view3d->numSelectedAtoms()>0)
        {
            view3d->addHydrogenAtomForSelected();
            view3d->refresh();
            return;
        }
    }


    if(key == "k")
    {
        getMainWindow()-> setKekuleStyle();
        //view3d->refresh();
        return;
    }



    if(key == "n")
    {
        getMainWindow()->setCn_Visible();
        //view3d->refresh();
        return;
    }


    //molecule visible
    if(key == "m")
    {
        if (view3d->isMolHide())
            getMainWindow()->setMolVisible();
        else
            getMainWindow()->setMolVisible();
        //view3d->refresh();
        return;
    }


    //label on/off
    if(key == "l")
    {
        //std::cout <<key << std::endl;
        getMainWindow()->setAtomLabelVisible();
        return;
    }


    //outline
    if(key == "o")
    {
        if(view3d->isSymmetryElementVisible())
            getMainWindow()->setOutLineVisible();
        else {
            if(view3d->numSelectedAtoms()>0)
                view3d->setSelectedAtomSymbol("O");
        }

        return;
    }



    //symmetry element
    if(key == "p")
    {
        /*
        if(view3d->numSelectedAtoms()>0)
        {
            view3d->setSelectedAtomSymbol("P");
            return;
        }
*/

        getMainWindow()->setSymmetryElementVisible();
        //view3d->refresh();
        return;
    }




    //add-Ring
    if(key == "r")
    {
        /*
        if(view3d->numSelectedAtoms()>0)
        {
            view3d->setSelectedAtomSymbol("Re");
            return;
        }

        view3d->buildRingForSelectedAtoms();
        */
        getMainWindow()->on_actionreCalculateBonds_triggered();
        return;
    }

    if(key == "s")
    {
        if(view3d->numSelectedAtoms()>0)
        {
            view3d->setSelectedAtomSymbol("S");
            return;
        }
    }


    // ctrl_v : paste
    if(key == "v")
    {
        if(isControlPressed)
            view3d->paste();
        else
        {
            getMainWindow()->setSigmaV_Visible();
            //view3d->refresh();
        }

        return;
    }


    if(key == "x")
    {
        if(isControlPressed)
            view3d->cutSelected();
        return;
    }


    if(key == "y")
    {
        //if(isControlPressed)
        view3d->redo();
        return;
    }


    if(key == "z")
    {
        //if(isControlPressed)
        view3d->undo();
        return;
    }


    if(key == "u")
    {
        view3d->UFF();
        return;
    }

    if(key == "g")
    {
        view3d->Ghemical();
        return;
    }

}






void SelectAreaInteractorStyle::linkMolecule()
{
    vtkAssemblyPath *path = nullptr;
    int * eventPos = GetInteractor()->GetEventPosition();
    this->FindPokedRenderer(eventPos[0], eventPos[1]);
    GetInteractor()->StartPickCallback();
    auto picker = vtkAbstractPropPicker::SafeDownCast(GetInteractor()->GetPicker());


    //build new molecule on empty place
    if( picker == nullptr )
    {
        cout << "picker == nullptr"<<endl;
        //view3d->addNewTemplate(eventPos[0], eventPos[1], 0.0);
        cout <<"!picker " << eventPos[0] << "  "<<  eventPos[1]<< " "<<0.0<<endl;
        //view3d->setDefaultOperationMode();
        //releaseAllActors();
        return;
    }

    if ( picker != nullptr )
    {
        picker->Pick(eventPos[0], eventPos[1], 0.0, this->CurrentRenderer);
        path = picker->GetPath();
    }

     //cout << "picker == nullptr"<<endl;

    vtkActor * pickedActor = picker->GetActor();

    if( pickedActor==nullptr)
    {
        double * pickPos = picker->GetPickPosition();
        view3d->addNewTemplate(pickPos[0], pickPos[1], 0.0);
        return;
    }
    else {
        view3d->setDefaultOperationMode();
        releaseAllActors();
    }


    if(pickedActor->Id()<0)
        return;

    if(pickedActor->Id()>100000) {
        return;
    }
    if(pickedActor->AtomId()>100000)
    {
        return;
    }



    view3d->linkTemplate2Mol(pickedActor->AtomId());
}



void SelectAreaInteractorStyle::OnLeftButtonDown()
{
    isLeftButtonDown=true;
    isMouseMove=false;
/*
    //model double click, selectAll
    NumberOfClicks++;
    int xdist = GetInteractor()->GetEventPosition()[0] - GetInteractor()->GetLastEventPosition()[0];
    int ydist = GetInteractor()->GetEventPosition()[1] - GetInteractor()->GetLastEventPosition()[1];


    int moveDistance = (int)sqrt((double)(xdist*xdist + ydist*ydist));
    if(moveDistance > 5)
        NumberOfClicks = 1;

    if(NumberOfClicks == 2)
    {
        cout << " double clicked"<<endl;
    }

    //https://vtk.org/Wiki/VTK/Examples/Cxx/Interaction/DoubleClick
    if(NumberOfClicks == 2)
    {
        NumberOfClicks = 0;
        doubleClick=true;

        cout << "Left double click "<<endl;


        //if selected something
        //do not work properly
        if(numSelectedAtoms()==1)
        {
            view3d->selectAll();
            vtkInteractorStyleRubberBandPick::OnLeftButtonDown();
            //view3d->updateView();
            return;
        }


        cout << "do not work properly"<<endl;
        if(numSelectedBonds()==1)
        {
            unsigned int j=view3d->selectedBondList[0][0];
            unsigned int bo=view3d->getBondOrderbyId(j);
            cout << j<< "  "<< bo<< " sfsdfs1"<<endl;
            if(bo==6) {
                cout << view3d->selectedBondList[0][1] << " "<<view3d->selectedBondList[0][2] <<endl;
                if(mol==nullptr)
                    mol=view3d->getMol();
                mol->reverseBondOrient(view3d->selectedBondList[0][1],view3d->selectedBondList[0][2]);
                view3d->renderMol(mol);
            }
        }
    }
*/

    //if(view3d->isSelectionMode())    CurrentMode=VTKISRBP_SELECT;
    //else     CurrentMode=VTKISRBP_ORIENT;


    vtkInteractorStyleRubberBandPick::OnLeftButtonDown();
}

void  SelectAreaInteractorStyle::loseFocus()
{
    pickedActor=nullptr;
    view3d->removeDynamicLineActor();
    view3d->setDefaultOperationMode();
    view3d->updateView();
    return;
}

void SelectAreaInteractorStyle::OnMouseMove()
{
    if(isLeftButtonDown || isMiddleButtonDown || isRightButtonDown)
        view3d->updateAtomLabelState();

    isMouseMove=true;
    int * movePos = GetInteractor()->GetEventPosition();


    // dynamic line from this atom
    if(pickedActor)
    {
        if(view3d->isPencilDrawMode())
        {
            int x = movePos[0];
            int y = movePos[1];

            v1=getHitModelPosition(x, y);
            v0=view3d->getAtomPos(pickedActor->AtomId());

            auto  tmpLineSource = vtkLineSource::New();
            tmpLineSource->SetPoint1(v0.x(),v0.y(),v0.z());
            tmpLineSource->SetPoint2(v1.x(),v1.y(),v1.z());
            tmpLineSource->Update();

            auto tmpLineMapper = vtkPolyDataMapper::New();
            tmpLineMapper->SetInputConnection(tmpLineSource->GetOutputPort());

            if(tmpLineActor)
                tmpLineActor->SetMapper(tmpLineMapper);
            else {
                tmpLineActor = vtkActor::New();
                tmpLineActor->setMOId(-10);
                tmpLineActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
                tmpLineActor->SetMapper(tmpLineMapper);
                tmpLineActor->SetPickable(false);
                view3d->getRender()->AddActor(tmpLineActor);
            }

            view3d->updateView();
            return;
        }
    }

    //translate/rotate objects selected, when shift-pressed or control-pressed
    if(numSelectedAtoms()>0 )
    {
        if (isShiftPressed || isControlPressed)
        {
            if(isRightButtonDown )
            {
                rotateSelectedAtoms();
                return;
            }

            if(isMiddleButtonDown  )
            {
                translateSelectedAtoms();
                return;
            }
        }
    }

    vtkInteractorStyleRubberBandPick::OnMouseMove();
}



void SelectAreaInteractorStyle::updateSelectedAtomListFromAtomActors()
{
    view3d->updateSelectedAtomListFromAtomActors();
}

void SelectAreaInteractorStyle::updateSelectedBondListFromBondActors()
{
    view3d->updateSelectedBondListFromBondActors();
}

void SelectAreaInteractorStyle::releaseAllSelectedActors()
{
    view3d->releaseAllActors();
}


void SelectAreaInteractorStyle::OnRightButtonDown()
{
    isRightButtonDown=true;
    isMouseMove=false;

    vtkInteractorStyleRubberBandPick::OnLeftButtonDown();
}

void SelectAreaInteractorStyle::OnRightButtonUp()
{
    isRightButtonDown=false;

    if(mouseOperationMode==VTKPencilDrawing)
    {
        view3d->setDefaultOperationMode();
        view3d->releaseAllActors();
        view3d->removeDynamicLineActor();
        pickedActor=nullptr;

        view3d->renderMol();

        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
        return;
    }

    if(mouseOperationMode==VTKLinkTemplate)
    {
        view3d->setDefaultOperationMode();

        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
        return;
    }


    if(!isMouseMove)
        contextMenu();

    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
}



void SelectAreaInteractorStyle::OnMiddleButtonDown()
{
    isMiddleButtonDown=true;
    view3d->setTranslateMode();

    if (numSelectedObjects() <1)   return;

    //translate objects selected, when shift-pressed or control-pressed
    if(numSelectedAtoms()>0)
    {
        beg[0] = GetInteractor()->GetEventPosition()[0];
        beg[1] = GetInteractor()->GetEventPosition()[1];
    }

    if ( !isShiftPressed && (!isControlPressed) ) {
        vtkInteractorStyleRubberBandPick::OnMiddleButtonDown();
        return;
    }
}



void SelectAreaInteractorStyle::OnMiddleButtonUp()
{
    if(mouseOperationMode==VTKPencilDrawing)
    {
        //cout << "SSSSSSSS"<<endl;
        view3d->setDefaultOperationMode();
        //cout << "SSSSSSSS"<<endl;
        releaseAllSelectedActors();
        //cout << "SSSSSSSS"<<endl;
        view3d->updateView();
        return;
    }

    else {
        view3d->setDefaultOperationMode();
    }


    int x = GetInteractor()->GetEventPosition()[0];
    int y = GetInteractor()->GetEventPosition()[1];

    v0=getHitModelPosition(x,y);

    isMiddleButtonDown=false;

    if ( !isShiftPressed && (!isControlPressed) )
        vtkInteractorStyleRubberBandPick::OnMiddleButtonUp();


    isMouseMove=false;
}




//help function
void SelectAreaInteractorStyle::link2SelectedActors(vtkActor * actor1, vtkActor * actor2)
{
    if(actor1->AtomId()>=0 && actor1->AtomId1()<0)
        if(actor2->AtomId()>=0 && actor2->AtomId1()<0){
            view3d->linkTwoSelectedAtoms(actor1->AtomId(),actor2->AtomId());
        }
}


//help function
void SelectAreaInteractorStyle::disposeSingleSelectedActor(vtkActor * actor)
{
    if(!isShiftPressed)
        releaseAllSelectedActors();

    if(view3d->isSymmetryElementVisible()) return;

    if(mouseOperationMode==VTKPencilDrawing)
    {
        if(actor->AtomId()>=0 && actor->AtomId1()<0)
        {
            view3d->releaseAllActors();
            view3d->selectedAtomActors->AddItem(actor);
            view3d->updateSelectedAtomListFromAtomActors();

            highlightActor(actor);

            view3d->updateView();
            return;
        }
    }



    //ring
    if(actor->getRingId()>=0)
    {
        //release this actor, if picked twice
        if(  view3d->selectedRingActors->IsItemPresent(actor) )
        {
            //cout << "\nselected before"<<endl;
            if(isShiftPressed) {
                releaseSingleRingActor(actor);
                HighlightProp(nullptr);
            }
        }
        else {
            //cout << "\nnot selected before"<<endl;
            view3d->selectedRingActors->AddItem(actor);
            highlightActor(actor);
        }

        view3d->updateSelectedRingListFromRingActors();
        view3d->updateView();
        return;
    }


    if(actor->BondId()>=0)
    {
        //bond -----------------------------------------------------
        if(view3d->selectedBondActors->IsItemPresent(actor))
        {
            releaseRelatedBondActors(actor);
            return;
        }

        //collect other half-bonds
        view3d->selectedBondActors->InitTraversal();
        vtkActorCollection* actorCollection = GetDefaultRenderer()->GetActors();
        actorCollection->InitTraversal();

        for (vtkIdType j=0;j<actorCollection->GetNumberOfItems();j++)
        {
            vtkActor * b = actorCollection->GetNextActor();
            if(b==nullptr) continue;
            if(actor->BondId() != b->BondId()) continue;

            if( !view3d->selectedBondActors->IsItemPresent(b) )
                view3d->selectedBondActors->AddItem(b);
            highlightActor(b);
        }
        this->HighlightProp(nullptr);
        view3d->updateSelectedBondListFromBondActors();
        return;
    }



    //atom ----------------------------------------------------
    if(actor->AtomId()>=0 && actor->AtomId1()<0 )
    {
        //release this actor, if picked twice
        if( view3d->selectedAtomActors->IsItemPresent(actor) )
        {
            //cout << "selected before"<<endl;

            if(isShiftPressed) {
                releaseSingleActomActor(actor);
                this->HighlightProp(nullptr);
            }
        }
        else
        {
            //cout << "not selected before"<<endl;
            view3d->selectedAtomActors->AddItem(actor);
            highlightActor(actor);
        }

        view3d->updateSelectedAtomListFromAtomActors();
        //view3d->refresh();

        view3d->setSelectedUpdateFromRendering(false);
        view3d->refresh();
        view3d->setSelectedUpdateFromRendering(true);
    }
}




void SelectAreaInteractorStyle::OnLeftButtonUp()
{
    isLeftButtonDown=false;
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

    if(mouseOperationMode==VTKPencilDrawing)
    {
        tmpLineActor = vtkActor::New();
        tmpLineActor=nullptr;

        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
        int * eventPos = GetInteractor()->GetEventPosition();
        
        this->FindPokedRenderer(eventPos[0], eventPos[1]);
        GetInteractor()->StartPickCallback();

        auto picker = vtkAbstractPropPicker::SafeDownCast(GetInteractor()->GetPicker());
        if ( picker != nullptr )
        {
            picker->Pick(eventPos[0], eventPos[1], 0.0, this->CurrentRenderer);

            if( picker->GetActor()!=nullptr )
            {
                if (picker->GetActor()->AtomId()>=0 && picker->GetActor()->AtomId1()<0)
                {
                    //add new atoms
                    if(pickedActor!=nullptr)
                    {
                        link2SelectedActors(picker->GetActor(),pickedActor);

                        //remove dynamic line
                        tmpLineActor = vtkActor::New();

                        //remove pickedActor
                        pickedActor=vtkActor::New();
                        pickedActor=nullptr;

                        view3d->releaseSelectedAtomActors();

                        releaseAllSelectedActors();
                        view3d->setDefaultOperationMode();
                        return;
                    }


                    pickedActor=picker->GetActor();
                    disposeSingleSelectedActor(pickedActor);


                    view3d->renderMol();

                    return;
                }

                //bond or ring
                if(picker->GetActor()->BondId()>=0 ||  picker->GetActor()->getRingId() >=0 )
                {
                    view3d->setDefaultOperationMode();
                    return;
                }
            }

            if(pickedActor) {
                v0=getHitModelPositionBasedonLinkeredActor(eventPos[0], eventPos[1],pickedActor);
                view3d->addCarbon_and_link2SelectedAtom(v0,pickedActor->AtomId());
            }
            else {
                v0=getHitModelPosition(eventPos[0], eventPos[1]);
                view3d->addCarbon(v0);
            }

            //update pickedActor
            pickedActor=view3d->getLastAtomActor();

            disposeSingleSelectedActor(pickedActor);
            return;
        }
    }
    pickedActor=nullptr;


    if(mouseOperationMode==VTKLinkTemplate)
    {
        linkMolecule();
        view3d->renderMol();
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
        return;
    }


    if(mouseOperationMode==VTKDefaultOperation)
    {
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
        //return;
    }

    //mouse not move
    if(!isMouseMove)
    {
        int * eventPos = GetInteractor()->GetEventPosition();
        this->FindPokedRenderer(eventPos[0], eventPos[1]);
        GetInteractor()->StartPickCallback();
        auto picker = vtkAbstractPropPicker::SafeDownCast(GetInteractor()->GetPicker());

        if ( picker != nullptr )
        {
            picker->Pick(eventPos[0], eventPos[1], 0.0, this->CurrentRenderer);

            //nothing is selected, release all
            if( picker->GetActor()==nullptr)
            {
                if ( !isShiftPressed && (!isControlPressed) )
                {
                    view3d->setDefaultOperationMode();
                    releaseAllActors();
                }
            }
            else //single selection (There are several objects are selected, only the first one is disposed)
            {
                //disposeSingleSelectedActor(vtkActor::SafeDownCast(picker->GetActor()));
                disposeSingleSelectedActor(picker->GetActor());
            }
        }

        return;
    }


    int min[2], max[2];
    int * size = this->Interactor->GetRenderWindow()->GetSize();
    min[0]=this->StartPosition[0] <= this->EndPosition[0] ? this->StartPosition[0] : this->EndPosition[0];
    if (min[0] < 0)     min[0] = 0;
    if (min[0] >= size[0])     min[0] = size[0] - 1;


    min[1]=this->StartPosition[1] <= this->EndPosition[1] ? this->StartPosition[1] : this->EndPosition[1];
    if (min[1] < 0)  min[1] = 0;
    if (min[1] >= size[1])    min[1] = size[1] - 1;

    max[0]=this->EndPosition[0] > this->StartPosition[0] ? this->EndPosition[0] : this->StartPosition[0];
    if (max[0] < 0)   max[0] = 0;
    if (max[0] >= size[0])  max[0] = size[0] - 1;


    max[1]=this->EndPosition[1] > this->StartPosition[1] ? this->EndPosition[1] : this->StartPosition[1];
    if (max[1] < 0)       max[1] = 0;
    if (max[1] >= size[1])    max[1] = size[1] - 1;



    double x0=min[0],y0=min[1],x1=max[0],y1=max[1];

    auto areaPicker=vtkSmartPointer<AreaPicker>::New();
    areaPicker->SetRenderer(GetDefaultRenderer());
    areaPicker->SetPickCoords(x0,y0,x1,y1);

    //store bonds
    std::vector <unsigned int> singleBonds,otherBonds;

    vtkPlanes* frustum = areaPicker->GetFrustum();
    unsigned int numSelected=areaPicker->AreaPick(min[0], min[1], max[0], max[1], GetDefaultRenderer());


    //parse mouse operation
    //ctrl && shift:operate selected atoms
    if( isShiftPressed && isControlPressed ) {
        setOperateSelectedMode();
        return;
    }


    if( !isShiftPressed ) return;


    //only shift is pressed: multi-selection mode
    unsigned int numSelAtoms=0,numSelBonds=0,numSelRings=0;

    //filter selected atoms
    areaPicker->Prop3Ds->InitTraversal();


    auto _atomActors =  vtkSmartPointer<vtkActorCollection>::New();
    auto _bondActors =  vtkSmartPointer<vtkActorCollection>::New();
    auto _ringActors =  vtkSmartPointer<vtkActorCollection>::New();



  //  cout << areaPicker->NumSelectedProps () <<  " is selected "<<endl;;
    for (unsigned int i=0;i<areaPicker->NumSelectedProps() ;i++)
    {
        auto actor = vtkActor::SafeDownCast(areaPicker->Prop3Ds->GetNextProp ());
        if(actor==nullptr) continue;

/*
        cout << i+1 <<" :";
        cout << 1+ actor->AtomId() <<  "   ";
        cout << 1+ actor->AtomId1() <<  "   ";
        cout << 1+ actor->getRingId() <<  "   ";
        cout << endl;
*/
        //ring
        if(actor->getRingId()>=0) {
            _ringActors->AddItem(actor);
            continue;
        }

        //bond
        if(actor->BondId()>=0){
            _bondActors->AddItem(actor);
            continue;
        }

        //atom
        if(actor->AtomId()>=0) {
            _atomActors->AddItem(actor);
        }
    }


    numSelRings= _ringActors->GetNumberOfItems();
    if( numSelRings>0)
    {
        _ringActors->InitTraversal();
        for (unsigned int i=0;i<numSelRings; i++)
        {
            auto a = _ringActors->GetNextActor();
            if(a==nullptr) continue;

            if(view3d->selectedRingActors->IsItemPresent(a))
            {
                //releaseSingleRingActor(a);
                continue;
            }

            view3d->selectedRingActors->AddItem(a);
            highlightActor(a);
        }
        view3d->updateSelectedRingListFromRingActors();
    }



    numSelAtoms=0;
    _atomActors->InitTraversal();
    for (unsigned int i=0;i<_atomActors->GetNumberOfItems(); i++)
    {
        auto a = _atomActors->GetNextActor();
        if(a==nullptr) continue;

        numSelAtoms++;

        //release this actor, if picked twice
        if(  view3d->selectedAtomActors->IsItemPresent(a) )
        {
            if(isMouseMove) continue; //mouse moves, remain selected
            releaseSingleActomActor(a);
            this->HighlightProp(nullptr);
        }
        else {//cout << "not selected before"<<endl;
            view3d->selectedAtomActors->AddItem(a);
            highlightActor(a);
            buildAtomLabel(a);
        }
    }
    view3d->updateSelectedAtomListFromAtomActors();


    numSelBonds= _bondActors->GetNumberOfItems();






    //only selected one bond, mouse is not move
    if( numSelAtoms<1 && (!isMouseMove) && numSelBonds>0 )
    {
        auto actor = _bondActors->GetLastItem();
        int idx=actor->AtomId(),  idx1=actor->AtomId1();

        if(idx >= 0  && idx1 >= 0)
        {
            if(view3d->selectedBondActors->IsItemPresent(actor))
            {
                releaseRelatedBondActors(actor);
                return;
            }

            //collect other half-bonds
            view3d->selectedBondActors->InitTraversal();
            vtkActorCollection* actorCollection = GetDefaultRenderer()->GetActors();
            actorCollection->InitTraversal();

            //unsigned int times=0;
            for (vtkIdType j=0;j<actorCollection->GetNumberOfItems();j++)
            {
                vtkActor * b = actorCollection->GetNextActor();
                if(b==nullptr) continue;
                if(actor->BondId() != b->BondId()) continue;
                if( view3d->selectedBondActors->IsItemPresent(b) ) continue;
                view3d->selectedBondActors->AddItem(b);

                highlightActor(b);
            }

        }

        this->HighlightProp(nullptr);
        view3d->updateSelectedBondListFromBondActors();

        return;
    }



    if(numSelBonds<1) return;

    _bondActors->InitTraversal();
    for (unsigned int i=0;i<numSelBonds; i++)
    {
        auto actor = _bondActors->GetNextActor();
        if(actor==nullptr) continue;

        if(view3d->selectedBondActors->IsItemPresent(actor)) continue;

        int idx=actor->AtomId(),
                idx1=actor->AtomId1();

        if(idx<0 ) continue;
        if(idx1<0) continue;

        //cout << "id of bond is : "<< actor->Id()<< ": " <<idx+1<<"->" << idx1+1<<" with order "<< actor->getBondOrder() <<endl;

        //release this actor, if picked twice
        if(  view3d->selectedBondActors->IsItemPresent(actor)  )
        {
            if(!isShiftPressed) continue;

            if(isMouseMove) continue; //mouse moves, remain selected
            releaseRelatedBondActors(actor);
            continue;
        }


        for (unsigned int j=0;j<areaPicker->NumSelectedProps () ;j++)
        {
            auto actor1 = vtkActor::SafeDownCast(areaPicker->Prop3Ds->GetItemAsObject (j));
            if(actor1==nullptr) continue;

            //the same one or not?
            if(actor1->Id() != actor->Id()) continue;

            //find pair
            if(actor1->AtomId()!=idx1 ) continue;
            if(actor1->AtomId1()!=idx)  continue;

            //single bond contains two halves
            if (actor->getBondOrder() == 1 )
            {
                view3d->selectedBondActors->AddItem(actor);
                view3d->selectedBondActors->AddItem(actor1);
                highlightActor(actor);
                highlightActor(actor1);
                singleBonds.push_back(actor->BondId());

                // cout << "add " << actor->Id() << " to single aaaaaaa"<<endl;
            }
            else {
                otherBonds.push_back(actor->BondId());
            }
        }

    }

    //non-single bond contains a lot
    collectLeftHalfBonds(otherBonds);

    this->HighlightProp(nullptr);
    view3d->updateSelectedBondListFromBondActors();
}


//help function
void SelectAreaInteractorStyle::collectLeftHalfBonds(vector <unsigned int > v)
{
    //collect those non-single-bonds
    if(v.size()<1) return;
    sort ( v.begin(), v.end());
    vector <uint> :: iterator iter =  unique ( v.begin(), v.end());
    v.erase(iter,v.end());

    view3d->selectedBondActors->InitTraversal();
    vtkActorCollection* actorCollection = GetDefaultRenderer()->GetActors();


    vtkActor * a, *b;
    for (unsigned int i=0; i < v.size(); i++)
    {
        //collect half-bonds left
        actorCollection->InitTraversal();
        for (vtkIdType j=0;j<actorCollection->GetNumberOfItems();j++)
        {
            b = actorCollection->GetNextActor();
            if(b==nullptr) continue;

            if(v[i]!=b->BondId()) continue;
            if( view3d->selectedBondActors->IsItemPresent(b) ) continue;

            view3d->selectedBondActors->AddItem(b);
            highlightActor(b);
        }
    }

}



void SelectAreaInteractorStyle::releaseRelatedBondActors(vtkActor* actor)
{
    view3d->removefromBondActorsList (actor);
}

bool SelectAreaInteractorStyle::isAtomPicked(unsigned int id)
{
    return view3d->isAtomPicked(id);
}


void SelectAreaInteractorStyle::releaseSingleActomActor(vtkActor* actor)
{
    view3d->removefromAtomActorsList (actor);
}

void SelectAreaInteractorStyle::releaseSingleRingActor(vtkActor* actor)
{
    view3d->removefromRingActorsList (actor);
}



void SelectAreaInteractorStyle::highlightActor(vtkActor * a)
{
    if (a!=nullptr)  {
        //this->HighlightProp(a);
        a->GetProperty()->SetColor(selectedColor);
    }
}

void SelectAreaInteractorStyle::releaseAllActors()
{
    view3d->releaseAllActors();
    this->HighlightProp(nullptr);
}

void SelectAreaInteractorStyle::buildAtomLabel(vtkActor * a)
{
    view3d->buildAtomLabel(a);
}

bool SelectAreaInteractorStyle::isAtomActorPicked(vtkActor * a)
{
    if (view3d->selectedAtomActors->IsItemPresent(a)) return true;
    return false;
}


bool SelectAreaInteractorStyle::isBondActorPicked(vtkActor * a)
{
    if (view3d->selectedBondActors->IsItemPresent(a)) return true;
    return false;
}







void SelectAreaInteractorStyle::setView3d (View3D * t)
{
    view3d=t;
    view3d->selectedAtomActors= vtkSmartPointer<vtkActorCollection>::New();
    view3d->selectedBondActors= vtkSmartPointer<vtkActorCollection>::New();
}

void SelectAreaInteractorStyle::setMol(HMol * m)
{
    if (m) mol=m;
    else mol=nullptr;
}

MouseOperationType SelectAreaInteractorStyle::currentMouseOperationMode()
{
    return mouseOperationMode;
}

bool SelectAreaInteractorStyle::isLinkTemplateMode()
{
    if (mouseOperationMode==VTKLinkTemplate) return true;
    return false;
}

bool SelectAreaInteractorStyle::isPencilDrawMode()
{
    if (mouseOperationMode==VTKPencilDrawing) return true;
    return false;
}



bool SelectAreaInteractorStyle::isSelectionMode()
{
    if (mouseOperationMode==VTKSelection) return true;
    return false;
}

bool SelectAreaInteractorStyle::isRotateMode()
{
    if (mouseOperationMode==VTKRotation) return true;
    return false;
}

bool SelectAreaInteractorStyle::isTranslateMode()
{
    if (mouseOperationMode==VTKTranslation) return true;
    return false;
}

void SelectAreaInteractorStyle::setDefaultOperationMode()
{
    mouseOperationMode=VTKRotation;
}

void SelectAreaInteractorStyle::setPencilDrawMode()
{
    mouseOperationMode=VTKPencilDrawing;
}

void SelectAreaInteractorStyle::setLinkTemplateMode()
{
    mouseOperationMode=VTKLinkTemplate;
}

void SelectAreaInteractorStyle::setRotateMode()
{
    mouseOperationMode=VTKRotation;
}

void SelectAreaInteractorStyle::setTranslateMode()
{
    mouseOperationMode=VTKTranslation;
}

void SelectAreaInteractorStyle::setSelectionMode()
{
    mouseOperationMode=VTKSelection;
}


void SelectAreaInteractorStyle::setOperateSelectedMode()
{
    mouseOperationMode=VTKOperateSelected;
}



vector3 SelectAreaInteractorStyle::getHitModelPositionBasedonLinkeredActor(unsigned int x, unsigned int y, vtkActor * ref)
{
    vector3 p= getHitModelPosition(x,y);

    if(ref==nullptr) return p;

    vector3 refP=view3d->getAtomPos(ref->AtomId());

    double  xC,yC,zC;
    getCamera()->GetViewPlaneNormal(xC,yC,zC);
    vector3 normalCamera(xC,yC,zC);

    vector3 shift=normalCamera*dot(refP,normalCamera);
    p+=shift;
    return p;
}


//set the z=0 of the view-plane
vector3 SelectAreaInteractorStyle:: getHitModelPosition(unsigned int x, unsigned int y)
{
    double worldpos[4];
    this->ComputeDisplayToWorld(x, y, 0, worldpos);

    //correct the position of the Z buffer
    auto plane =  vtkSmartPointer<vtkPlane>::New();
    double origin[] = { 0, 0, 0 };
    double normal[3];
    getCamera()->ComputeViewPlaneNormal();
    getCamera()->GetViewPlaneNormal(normal);
    plane->SetOrigin(origin);
    plane->SetNormal(normal);

    //input
    double p[3]= {worldpos[0], worldpos[1], worldpos[2]};

    //output
    double projected[3];

    plane->ProjectPoint(p, origin, normal, projected);
    vector3 pp(projected);
    return pp;
}



vector3 SelectAreaInteractorStyle:: getHitModelPosition(unsigned int p[2])
{
    return getHitModelPosition (p[0],p[1]);
}
vtkCamera * SelectAreaInteractorStyle::getCamera()
{
    return view3d->Camera();
}


void SelectAreaInteractorStyle::contextMenu()
{
    QMenu  popMenu;

    view3d->updateSelectedAtomListFromAtomActors();

    unsigned int numSelAtoms=view3d->numSelectedAtoms();
    unsigned int numSelBonds=view3d->numSelectedBonds();
    unsigned int numSelRings=view3d->numSelectedRings();
    unsigned int numSelObj=numSelAtoms+numSelBonds+numSelRings;

    //cout << " selected atoms "<< numSelAtoms<<endl;
    //cout << " selected bonds "<< numSelBonds<<endl;
    //cout << " selected rings "<< numSelRings<<endl<<endl;;
/*
    QAction actionUndo (QIcon(":/images/editUndo.png"),"Undo");
    actionUndo.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionUndo);
    connect(&actionUndo,  SIGNAL( triggered() ), this, SLOT( slot_Undo() ) );
    if(!view3d->canbeUndo())   actionUndo.setEnabled(false);

    QAction actionRedo (QIcon(":/images/editRedo.png"),"redo");
    actionRedo.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionRedo);
    connect(&actionRedo,  SIGNAL( triggered() ), this, SLOT( slot_Redo() ) );
    if(!view3d->canbeRedo())  actionRedo.setEnabled(false);




    //QAction actionDelete (QIcon(":/images/removeX.png"),"删除");
    QAction actionDelete (QIcon(":/images/removeX.png"),"delete");
    actionDelete.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionDelete);
    connect(&actionDelete,  SIGNAL( triggered() ), this, SLOT( slot_Delete() ) );

    QAction actionCopy (QIcon(":/images/copy.png"),"copy");
    actionCopy.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionCopy);
    popMenu.addAction(&actionCopy);
    connect(&actionCopy, SIGNAL( triggered() ), this, SLOT( slot_Copy() ) );


    QAction actionCut (QIcon(":/images/cut.png"),"cut");
    actionCut.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionCut);
    connect(&actionCut, SIGNAL( triggered() ), this, SLOT( slot_Cut() ) );


    QAction actionPaste (QIcon(":/images/EditPaste.png"),"paste");
    actionPaste.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionPaste);
    connect(&actionPaste, SIGNAL( triggered() ), this, SLOT( slot_Paste() ) );

    //something is wrong here!
    if(!view3d->canbePaste())
        actionPaste.setEnabled(false);

    if(numSelObj<1)
        actionDelete.setEnabled(false);

    if(numSelAtoms<1){
        actionCopy.setEnabled(false);
        actionCut.setEnabled(false);
    }

    popMenu.addSeparator();
*/
    QAction actionAtomColor ("原子颜色");
    actionAtomColor.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionAtomColor);
    connect(&actionAtomColor,SIGNAL(triggered()),  this, SLOT( slot_AtomColor()) );

    QAction actionAtomSize ("原子尺寸");
    actionAtomSize.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionAtomSize);
    connect(&actionAtomSize, SIGNAL(triggered()),  this, SLOT( slot_AtomSize())  );

    popMenu.addSeparator();

/*
    QAction actionAddRing ("构建新环");
    actionAddRing.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionAddRing);
    if(view3d->isRingExistedforSelectedAtoms() )
        actionAddRing.setEnabled(false);
    if(numSelAtoms<3)
        v.setEnabled(false);
*/

    QAction actionModifyRing ("改变圆环");
    actionModifyRing.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionModifyRing);


    QAction actionAddCenter ("加入中心");
    actionAddCenter.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionAddCenter);
    if(view3d->numSelectedAtoms()<1)
        actionAddCenter.setEnabled(false);


    //connect(&actionAddRing,  SIGNAL(triggered()),  this, SLOT( slot_AddRing())  );
    connect(&actionModifyRing,SIGNAL(triggered()), this, SLOT( slot_ModifyRing()) );
    connect(&actionAddCenter, SIGNAL(triggered()), this, SLOT( slot_AddCenter()) );


    if(numSelAtoms<1){
        actionAtomColor.setEnabled(false);
        actionAtomSize.setEnabled(false);
       // actionAddRing.setEnabled(false);
    }

    if(numSelRings!=1)
        actionModifyRing.setEnabled(false);

    /*
    QAction actionReverseArBond ("芳香键反转");
    actionReverseArBond.setFont ( QFont("Courier", 13));
    popMenu.addAction(&actionReverseArBond);
    connect(&actionReverseArBond,SIGNAL(triggered()), this, SLOT( slot_ReverseArBond() ) );
    if(numSelBonds<1)
        actionReverseArBond.setEnabled(false);
    else
        if( view3d->getBondOrderSelectedById(0)!= 6)
            actionReverseArBond.setEnabled(false);
*/
    popMenu.addSeparator();
    //-----------------------------------------------------------------------------
    if(view3d->isXExisted())
    {
        QAction actionRemoveX ("删除X原子");
        actionRemoveX.setFont ( QFont("Courier", 13));;
        popMenu.addAction(&actionRemoveX);
        connect(&actionRemoveX,  SIGNAL( triggered() ), this, SLOT( slot_RemoveX() ) );
    }

    if(view3d->numAtoms())
    {
        QAction actionCentralize ("分子居中");
        actionCentralize.setFont ( QFont("Courier", 13));;
        popMenu.addAction(&actionCentralize);
        connect(&actionCentralize,  SIGNAL( triggered() ), this, SLOT( slot_Centralize() ) );
    }

    /*
    QAction actionHideAtomLabel ("显示原子标号");
    actionHideAtomLabel.setFont ( QFont("Courier", 13));
    if(view3d->isAtomLabelVisible())
        actionHideAtomLabel.setText("隐藏原子标号");
    popMenu.addAction(&actionHideAtomLabel);
    connect(&actionHideAtomLabel,  SIGNAL( triggered() ), this, SLOT( slot_HideAtomLabel() ) );


    //if(view3d->isHydrogenAtomExisted()) {
        QAction actionHideHydrogenAtoms ("隐藏氢原子");
        actionHideHydrogenAtoms.setFont ( QFont("Courier", 13));
        if(view3d->isHydrogenHide())
            actionHideHydrogenAtoms.setText("显示氢原子");
        popMenu.addAction(&actionHideHydrogenAtoms);
        connect(&actionHideHydrogenAtoms,  SIGNAL( triggered() ), this, SLOT( slot_HideHydrogenAtoms() ) );

        if(!view3d->isHydrogenAtomExisted())
            actionHideHydrogenAtoms.setEnabled(false);
   // }


    QAction actionHideMolecule ("隐藏分子");
    actionHideMolecule.setFont ( QFont("Courier", 13));
    if(view3d->isMolHide())
        actionHideMolecule.setText("显示分子");
    popMenu.addAction(&actionHideMolecule);
    connect(&actionHideMolecule,  SIGNAL( triggered() ), this, SLOT( slot_HideMolecule() ) );

    QAction actionHideSymmetryElements ("显示点群元素");
    actionHideSymmetryElements.setFont ( QFont("Courier", 13));
    if(view3d->isSymmetryElementVisible())
        actionHideSymmetryElements.setText("隐藏点群元素");
    popMenu.addAction(&actionHideSymmetryElements);
    connect(&actionHideSymmetryElements,  SIGNAL( triggered() ), this, SLOT( slot_HideSymmetryElements() ) );

    QAction actionHideOutline ("显示轮廓线" ,view3d);
    actionHideOutline.setFont ( QFont("Courier", 13));
    if(view3d->getOutLineType()>0)
        actionHideOutline.setText("隐藏轮廓线");
    popMenu.addAction(&actionHideOutline);
    connect(&actionHideOutline,   SIGNAL( triggered() ), this, SLOT( slot_HideOutline() ) );

    QAction actionHideAxis ("隐藏坐标轴" ,view3d);
    actionHideAxis.setFont ( QFont("Courier", 13));
    if(!view3d->isXYZAxisVisible())
        actionHideAxis.setText("显示坐标轴");
    popMenu.addAction(&actionHideOutline);
    connect(&actionHideAxis,  SIGNAL( triggered() ), this, SLOT( slot_HideAxis() ) );
*/

    popMenu.exec( QCursor::pos());
}

void SelectAreaInteractorStyle::slot_HideAtomLabel()
{
    view3d->setAtomLabelVisible(!view3d->isAtomLabelVisible());
    view3d->refresh();
}

void SelectAreaInteractorStyle::slot_HideAxis()
{
    view3d->setXYZAxisVisible(!view3d->isXYZAxisVisible());
    view3d->refresh();
}
void SelectAreaInteractorStyle::slot_HideSymmetryElements()
{
    view3d->setSymmetryElementVisible(!view3d->isSymmetryElementVisible());
    view3d->refresh();
}


void SelectAreaInteractorStyle::slot_HideOutline()
{
    if(view3d->getOutLineType()<1) {
        view3d->setOutLineType(1);
    }
    else {
        view3d->setOutLineType(0);
    }

    view3d->refresh();
}

void SelectAreaInteractorStyle::slot_HideMolecule()
{
    if(view3d->isMolHide())
        view3d->setMolVisible(true);
    else
        view3d->setMolVisible(false);
    view3d->refresh();
}


void SelectAreaInteractorStyle::slot_HideHydrogenAtoms()
{
    getMainWindow()->setHydrogenVisible();
}

void SelectAreaInteractorStyle::slot_RemoveX()
{
    mol->removeX();
    view3d->renderMol(mol);
}

void SelectAreaInteractorStyle::slot_Delete()
{
    view3d->removeSelectedAtomsBonds();
}

void SelectAreaInteractorStyle::slot_ReverseArBond()
{
    view3d->reverseAromaticBond();
}

void SelectAreaInteractorStyle::slot_AtomColor()
{
    view3d->modifyAtomColor();
}

void SelectAreaInteractorStyle::slot_AtomSize()
{
    view3d->modifyAtomRadius();
}

void SelectAreaInteractorStyle::slot_AddRing()
{
    view3d->buildRingForSelectedAtoms();
}
void SelectAreaInteractorStyle::slot_AddCenter()
{
    view3d->appendXAtomAtCenterofSelectedAtoms();
}

void SelectAreaInteractorStyle::slot_Centralize()
{
    view3d->centralize();
}
void SelectAreaInteractorStyle::slot_ModifyRing()
{
    view3d->modifySelectedRing(); //only one is selected
}

void SelectAreaInteractorStyle::slot_Copy()
{
    view3d->copySeleted();
}

void SelectAreaInteractorStyle::slot_Cut()
{
    view3d->cutSelected();
}
void SelectAreaInteractorStyle::slot_Paste()
{
    view3d->paste();
}


void SelectAreaInteractorStyle::slot_Undo()
{
    view3d->undo();
    view3d->updateMol();
}
void SelectAreaInteractorStyle::slot_Redo()
{
    view3d->redo();
    view3d->updateMol();
}
