#include <QApplication>
#include <QClipboard>
#include <algorithm>

#include "View3D.h"
#include "math/vector3.h"
#include "mTools.h"
#include "mainwindow.h"
#include "templateMol.h"

#include <vtkCamera.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkDataSetMapper.h>

//#include <QVTKOpenGLWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkDataSet.h>

#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkInteractorStyle.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTDxInteractorStyleCamera.h>
#include <vtkTDxInteractorStyleSettings.h>
#include <vtkTubeFilter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPlaneSource.h>
#include <vtkBox.h>
#include <vtkLightCollection.h>

//#include "vtkCapsuleSource.h"

#include <vtkOrientationMarkerWidget.h>

#include <QVTKInteractor.h>

#include <vtkSmartPointer.h>
#include <vtkVolumeProperty.h>

#include <vtkPolyData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include <vtkCubeSource.h>
#include <vtkSphereSource.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataNormals.h>
#include <vtkAxesActor.h>
#include <vtkTransform.h>
#include <vtkCommand.h>
#include <vtkReflectionFilter.h>
#include <vtkNamedColors.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkVolumeProperty.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>


#include <vtkMolecule.h>
#include <vtkMoleculeMapper.h>
#include <vtkMoleculeToAtomBallFilter.h>
#include <vtkSimpleBondPerceiver.h>
#include <vtkTriangle.h>
#include <vtkPolygon.h>
#include <vtkLight.h>

#include <vtkStringArray.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <QMessageBox>
#include <QTime>

#include <vtkPolyhedron.h>
#include <vtkPentagonalPrism.h>
#include <vtkHexagonalPrism.h>
#include <vtkPolygon.h>
#include <vtkRegularPolygonSource.h>
#include <vtkDiskSource.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkVectorText.h>
#include <vtkTextActor.h>
#include <vtkTransformFilter.h>
#include <vtkTexturedSphereSource.h>
#include <vtkTransformTextureCoords.h>
#include <vtkTexture.h>
#include <vtkTextSource.h>
#include <vtkCoordinate.h>
#include <vtkTubeFilter.h>
#include <vtkParametricTorus.h>
#include <vtkParametricFunctionSource.h>

#include <vtkFollower.h>
#include <vtkVectorText.h>
#include <vtkLabeledDataMapper.h>
#include <vtkAssembly.h>
#include <vtkCoordinate.h>
#include <vtkMatrix4x4.h>
#include <vtkTransformCoordinateSystems.h>
#include <QColorDialog>
#include "templateView3d.h"

#include <QTimer>
#include <QDir>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstddef>
#include <unistd.h>

#include <QTextStream>
#include "OperatorTree.h"



void View3D::renderCs( )
{
    if(list_sigmaV_visible[0]<1) return;
    renderSigmaV(90);
}



void View3D::renderCi( )
{
    if(!_isCiVisible) return;

    auto sphere = vtkSphereSource::New();
    sphere->SetPhiResolution(20);
    sphere->SetThetaResolution(20);
    sphere->Update();
    sphere->SetRadius(0.03*discCn_scale);
    sphere->SetCenter(0.0,0.0,Cn_Length/2.0+0.02);

    auto actor = vtkActor::New();
    auto mapper = vtkPolyDataMapper::New();
    actor->GetProperty()->SetColor(yellow);
    actor->SetMapper(mapper);
    actor->SetPickable(false);
    mapper->SetInputConnection(sphere->GetOutputPort());
    m_renderer->AddActor(actor);
}

void View3D::renderCenter(double radius,double color[3])
{
    if(!_isCiVisible) return;

    auto sphere = vtkSphereSource::New();
    sphere->SetPhiResolution(20);
    sphere->SetThetaResolution(20);
    sphere->Update();
    sphere->SetRadius(radius);
    sphere->SetCenter(0.0,0.0,0.0);

    auto actor = vtkActor::New();
    auto mapper = vtkPolyDataMapper::New();
    actor->GetProperty()->SetColor(color);
    actor->SetMapper(mapper);
    actor->SetPickable(false);
    mapper->SetInputConnection(sphere->GetOutputPort());
    m_renderer->AddActor(actor);
}


void View3D::renderInverseIn(unsigned int n, double color[3])
{
    double length=moleculeLength/2.0;
    length*=1.2;

    //principal Cn disc
    if(n==2)
    {
        auto  disk0 =  vtkSmartPointer<vtkCylinderSource>::New();
        disk0->SetCenter(0.2, 0.0, 0.0);
        disk0->SetResolution(80);
        disk0->SetHeight(discThickness);
        disk0->SetRadius(discRadius);
        disk0->SetCapping(true);
        disk0->Update();

        auto  disk1 =  vtkSmartPointer<vtkCylinderSource>::New();
        disk1->SetCenter(-0.2, 0.0, 0.0);
        disk1->SetResolution(80);
        disk1->SetHeight(discThickness);
        disk1->SetRadius(discRadius);
        disk1->SetCapping(true);
        disk1->Update();

        vtkPolyDataBooleanFilter *bf = vtkPolyDataBooleanFilter::New();
        bf->SetInputData(0, disk0->GetOutput());
        bf->SetInputData(1, disk1->GetOutput());
        bf->SetOperModeToIntersection();
        bf->Update();

        auto trans =  vtkSmartPointer<vtkTransform>::New();
        trans->PostMultiply();
        trans->RotateX(90.0);
        trans->Translate(0.0,0.0,length+0.001);

        auto actor = vtkActor::New();
        actor->GetProperty()->SetColor(color);
        actor->GetProperty()->ShadingOn();
        actor->SetUserTransform(trans);
        actor->GetProperty()->SetInterpolationToGouraud();
        actor->SetPickable(false);

        auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection( bf->GetOutputPort() );
        mapper->ScalarVisibilityOff();
        actor->SetMapper(mapper);
        m_renderer->AddActor(actor);
    }
}



void View3D::renderCnv(unsigned int n)
{
    renderPrincipleCylinder();

    //odd
    if(n%2>0)
    {
        double angle=180/n;

        //the same color
        if(list_sigmaV_visible.size()<1) {
            for(unsigned int i=0;i<n;i++)
                renderSigmaV(90+angle*i);
        }
        else {
            for(unsigned int i=0;i<n;i++)    {
                if(list_sigmaV_visible[i]>0)
                    renderSigmaV(90+angle*i);
            }
        }
    }
    else
    {
        double angle=360/n,ang0=angle*0.5;
        unsigned int m=n/2;

        if(list_sigmaV_visible.size()<1)
        {
            //two different colors
            for(unsigned int i=0;i<m;i++)
                renderSigmaV(angle*i);

            for(unsigned int i=0;i<m;i++)
                renderSigmaD(angle*i+ang0);
        }

        else
        {
            for(unsigned int i=0;i<m;i++)    {
                if(list_sigmaV_visible[i]>0)
                    renderSigmaV(angle*i);
            }
            for(unsigned int i=0;i<m;i++)     {
                if(list_sigmaD_visible[i]>0)
                    renderSigmaD(angle*i+ang0);
            }
        }

    }
}


void View3D::renderDn(unsigned int n, bool renderPrincipal)
{
    //maybe Dnd, Dnh
    if (n%2==1)
        isI2n=true;

    //Dn
    if(PointGroup.length()<3)
        isI2n=false;




    double angle=0.0,
           ang0=180.0/n;



    //odd
    if (n%2>0)
    {
        for (unsigned int k=0; k<n; k++)
        {
            angle+=ang0;
            if(list_C2_Horizontal_visible[k]>0)
                renderHorizontalC2(1, angle,C2_Length);
        }
    }
    else {//even
        for (unsigned int k=0; k<n; k++)
        {
            angle+=ang0;

            if(list_C2_Horizontal_visible[k]<1)
                continue;

            if (k%2==0)
                renderHorizontalC2(1, angle,C2_Length);
            else
                renderHorizontalC2(2, angle,C2_Length);
        }
    }

    if(renderPrincipal)
        renderPrincipleCylinder();
}


void View3D::renderCn( )
{
    renderPrincipleCylinder();
}




void View3D::renderCnh(unsigned int n)
{
    renderSigmaH();

    if (n%2==0) //even
    {
        if(_isCnVisible) renderCi();
        isI2n=false;
        renderCenter(0.1*discCn_scale,yellow);
    }
    else { //I2n
        isI2n=true;
    }

    renderPrincipleCylinder();
}


void View3D::renderDnd(unsigned int n)
{
    if (moleculeLength<1.0)
        moleculeLength=2.0;

    unsigned int order=n;
    isI2n=false;

    if (order%2==0)  //D2d(S4=I4), D4d(I8=S8)
    {
        isI2n=true;
    }
    else {
        if (_isCnVisible) renderCi();
        renderCenter(0.1*discCn_scale,yellow);
    }
    renderPrincipleCylinder();

    renderDn(n,false);


    //odd
    if(n%2>0)
    {
        double angle=180/n;

        //the same color
        if(list_sigmaV_visible.size()<1) {
            for(unsigned int i=0;i<n;i++)
                renderSigmaD(90+angle*(i+0.5));
        }
        else {
            for(unsigned int i=0;i<n;i++)
                if(list_sigmaV_visible[i]>0)
                    renderSigmaD(90+angle*(i+0.5));
        }
    }

    else
    {
        double angle=360/n,ang0=angle*0.5;
        unsigned int m=n/2;

        //two different colors
        if(list_sigmaV_visible.size() + list_sigmaD_visible.size()<1)
        {
            for(unsigned int i=0;i<m;i++)
                renderSigmaV(angle*(i+0.25));
            for(unsigned int i=0;i<m;i++)
                renderSigmaD(angle*(i+0.25)+ang0);

            return;
        }

        //two different colors
        for(unsigned int i=0;i<m;i++)   {
            if(list_sigmaV_visible[i]>0)
                renderSigmaV(angle*(i+0.25));
        }

        for(unsigned int i=0;i<m;i++)    {
            if(list_sigmaD_visible[i]>0)
                renderSigmaD(angle*(i+0.25)+ang0);
        }
    }
}


void View3D::renderSn(unsigned int n)
{
    isI2n=false;
    if(!_isCnVisible) return;

    //odd Sn=Cnh
    //if(n%2>0)  return;

    if(n%4<1) {
        isI2n=true;
        renderPrincipleCylinder(n/2,color_Cn);
    }
    else
    {
        isI2n=false;
        renderPrincipleCylinder(n/2,color_Cn);
        renderCi();
        renderCenter(0.1*discCn_scale,yellow);
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
}


void View3D::renderDnh(unsigned int n )
{
    if(n%2==0)
    {
        if(_isCnVisible) renderCi();
        isI2n=false;
        renderCenter(0.1*discCn_scale,yellow);
    }

    renderSigmaH();





    //odd
    if(n%2>0)
    {
        double angle=180.0/n;

        //the same color
        if(list_sigmaV_visible.size()<1)
        {
            for(unsigned int i=0;i<n;i++)
                renderSigmaD(90.0+angle*i);
        }

        else {
            for(unsigned int i=0;i<n;i++)
            {
                if(list_sigmaV_visible[i]>0)
                    renderSigmaD(90.0+angle*i);
            }
        }
    }
    else
    {
        double angle=360.0/n,ang0=angle*0.5;
        unsigned int m=n/2;

        if(list_sigmaV_visible.size()+list_sigmaD_visible.size()<1)
        {
            //two different colors
            for(unsigned int i=0;i<m;i++)
                renderSigmaV(angle*i);


            for(unsigned int i=0;i<m;i++)
                renderSigmaD(angle*i+ang0);

            return;
        }

        //two different colors
        for(unsigned int i=0;i<m;i++)   {
            if(list_sigmaV_visible[i]>0)
                renderSigmaV(angle*i);
        }

        for(unsigned int i=0;i<m;i++)    {
            if(list_sigmaD_visible[i]>0)
                renderSigmaD(angle*i+ang0);
        }
    }

    renderDn(n);

}

void View3D::renderO( )
{
    //  cout << " list_C3_visible"<<endl;
    //  for (auto i : list_C3_visible)
    //     cout << list_C3_visible[i]<<endl;


    double length=Cn_Length;


    //renderCubeSkeleton(length);

    renderCubeSkeleton(HorizontalLengh);



    //render
    if(_isCnVisible){
        //vertical
        auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
        cylinder->SetCenter(0.0, 0.0, 0.0);
        cylinder->SetResolution(4);
        cylinder->SetHeight(discThickness);
        cylinder->SetCapping(true);
        cylinder->SetRadius(discRadius*discCn_scale);
        cylinder->Update();

        //y
        if(list_C4_visible[2]>0)
            for(unsigned int i=0;i<2;i++)
            {
                auto transform=  vtkSmartPointer<vtkTransform>::New();
                transform->PostMultiply();
                transform->RotateX(90.0);
                transform->Translate(0.0,0.0,length*0.5);

                transform->RotateX(90+180*i);

                auto actor = vtkActor::New();
                actor->GetProperty()->SetColor(color_Cn);
                actor->GetProperty()->ShadingOn();
                actor->SetUserTransform(transform);
                actor->SetPickable(false);

                actor->GetProperty()->SetInterpolationToGouraud();

                auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputData(cylinder->GetOutput());
                actor->SetMapper(mapper);
                m_renderer->AddActor(actor);
            }



        //z
        if(list_C4_visible[0]>0)
            for(unsigned int i=0;i<2;i++)
            {
                auto rotation=  vtkSmartPointer<vtkTransform>::New();
                rotation->PostMultiply();
                rotation->RotateX(90.0);
                rotation->Translate(0.0,0.0,length*0.5);

                rotation->RotateY(180*i);

                auto actor = vtkActor::New();
                actor->GetProperty()->SetColor(color_Cn);
                actor->GetProperty()->ShadingOn();
                actor->SetUserTransform(rotation);
                actor->SetPickable(false);

                actor->GetProperty()->SetInterpolationToGouraud();

                auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputData(cylinder->GetOutput());
                actor->SetMapper(mapper);
                m_renderer->AddActor(actor);
            }

        //x
        if(list_C4_visible[1]>0)
            for(unsigned int i=0;i<2;i++)
            {
                auto rotation=  vtkSmartPointer<vtkTransform>::New();
                rotation->PostMultiply();
                rotation->RotateX(90.0);
                rotation->Translate(0.0,0.0,length*0.5);

                rotation->RotateY(180*i+90.0);

                auto actor = vtkActor::New();
                actor->GetProperty()->SetColor(color_Cn);
                actor->GetProperty()->ShadingOn();
                actor->SetUserTransform(rotation);
                actor->SetPickable(false);

                actor->GetProperty()->SetInterpolationToGouraud();

                auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputData(cylinder->GetOutput());
                actor->SetMapper(mapper);
                m_renderer->AddActor(actor);
            }


        auto  cylinderAxis =  vtkSmartPointer<vtkCylinderSource>::New();
        cylinderAxis->SetCenter(0.0, 0.0, 0.0);
        cylinderAxis->SetResolution(20);
        cylinderAxis->SetHeight(length);
        cylinderAxis->SetRadius(principalAxisRadius);
        cylinderAxis->SetCapping(true);
        cylinderAxis->Update();

        for(unsigned int i=0;i<3;i++)
        {
            if(list_C4_visible[i]<1) continue;
            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();

            if(i<2) {
                transform->RotateX(90.0);
                transform->RotateY(90.0*i);
            }



            auto actorAxis = vtkActor::New();
            actorAxis->GetProperty()->SetColor(color_Cn);
            actorAxis->GetProperty()->ShadingOn();
            actorAxis->GetProperty()->SetInterpolationToGouraud();
            actorAxis->SetUserTransform(transform);
            actorAxis->SetPickable(false);

            auto mapperAxis =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapperAxis->SetInputData(cylinderAxis->GetOutput());
            actorAxis->SetMapper(mapperAxis);
            m_renderer->AddActor(actorAxis);
        }
    }




    //C3 111
    //if(_isC3_Visible)    {
        double l=C3_Length*0.5;

        //up
        for(unsigned int i=0;i<2;i++)
        {
            if(list_C3_visible[i] < 1) continue;

            auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
            cylinder->SetCenter(0.0, 0.0, 0.0);
            cylinder->SetResolution(3);
            cylinder->SetHeight(discThickness);
            cylinder->SetCapping(true);
            cylinder->SetRadius(discRadius*discC3_scale);
            cylinder->Update();

            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateWXYZ(90.0,1,0,0);
            transform->RotateWXYZ(54.7,0,1,0);

            transform->RotateWXYZ(45.0+180.0*i,0,0,1);
            transform->Translate(l*cos(3.142*i),l*cos(3.142*i),l);
            transform->RotateWXYZ(60.0,cos(3.142*i),cos(3.142*i),1);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_C3);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);
            m_renderer->AddActor(actor);
        }

        //below
        for(unsigned int i=0;i<2;i++)
        {
            if(list_C3_visible[i+2] < 1) continue;
            auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
            cylinder->SetCenter(0.0, 0.0, 0.0);
            cylinder->SetResolution(3);
            cylinder->SetHeight(discThickness);
            cylinder->SetCapping(true);
            cylinder->SetRadius(discRadius*discC3_scale);
            cylinder->Update();

            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();

            transform->RotateWXYZ(90.0,1,0,0);
            transform->RotateWXYZ(35.3+90.0,0,1,0);

            transform->RotateWXYZ(135.0+180.0*i,0,0,1);
            transform->Translate(-l*cos(3.142*i),l*cos(3.142*i),-l);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_C3);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);
            m_renderer->AddActor(actor);
        }


        vector3 beg(l,l,l);
        vector3 end(0,0,0);

        if(list_C3_visible[0] > 0)
            renderTube(beg,end,C3_Radius,color_C3);


        beg.Set(-l,-l,l);
        end.Set(0,0,0);

        if(list_C3_visible[1] > 0)
            renderTube(beg,end,C3_Radius,color_C3);

        beg.Set(-l,l,-l);
        end.Set(0,0,0);

        if(list_C3_visible[2] > 0)
            renderTube(beg,end,C3_Radius,color_C3);

        beg.Set(l,-l,-l);
        end.Set(0,0,0);
        if(list_C3_visible[3] > 0)
            renderTube(beg,end,C3_Radius,color_C3);
  //  }


    //other six C2
    //if(_isC2_1_Visible)    {
        //  cout << " list_C2_visible"<<endl;
        //  for (auto i : list_C2_visible)
        //      cout << i<<endl;

        l=C2_Length*0.707;

        //disk up
        for(unsigned int i=0;i<4;i++)
        {
            if(list_C2_visible[i]<1) continue;
            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->Scale(discC2_scale,1.0,discC2_scale);
            transform->Translate(0.0,l,0.0);

            transform->RotateX(45.0);  //up
            transform->RotateZ(i*90);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color1_C2);

            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(bfPolyData->GetOutputPort() );
            mapper->ScalarVisibilityOff();
            actor->SetMapper(mapper);
            actor->SetPickable(false);

            m_renderer->AddActor(actor);
        }


        //disk below
        for(unsigned int i=0;i<4;i++)
        {
            if(list_C2_visible[i]<1) continue;
            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->Scale(discC2_scale,1.0,discC2_scale);
            transform->Translate(0.0,l,0.0);

            transform->RotateX(-45.0);
            transform->RotateZ(180.0);
            transform->RotateZ(i*90.0);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color1_C2);

            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(bfPolyData->GetOutputPort() );
            mapper->ScalarVisibilityOff();
            actor->SetMapper(mapper);
            actor->SetPickable(false);

            m_renderer->AddActor(actor);
        }

        //cyinder
        for(unsigned int i=0;i<4;i++)
        {
            if(list_C2_visible[i]<1) continue;
            auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
            cylinder->SetCenter(0.0, 0.0, 0.0);
            cylinder->SetResolution(30);
            cylinder->SetHeight(2.0*l);
            cylinder->SetCapping(true);
            cylinder->SetRadius(principalAxisRadius);
            cylinder->Update();

            auto transform =  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateX(45.0);
            transform->RotateZ(i*90.0);

            auto actor2 = vtkActor::New();
            actor2->GetProperty()->SetColor(color1_C2);

            actor2->GetProperty()->ShadingOn();
            actor2->SetUserTransform(transform);
            actor2->SetPickable(false);
            actor2->GetProperty()->SetInterpolationToGouraud();

            auto mapper2 =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper2->SetInputData(cylinder->GetOutput());
            actor2->SetMapper(mapper2);
            m_renderer->AddActor(actor2);
        }

        //cyinder inside XOY
        for(unsigned int i=0;i<2;i++)
        {
            if(list_C2_visible[i+4]<1) continue;
            auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
            cylinder->SetCenter(0.0, 0.0, 0.0);
            cylinder->SetResolution(30);
            cylinder->SetHeight(2.0*l);
            cylinder->SetCapping(true);
            cylinder->SetRadius(principalAxisRadius);
            cylinder->Update();

            auto transform =  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateZ(45.0+i*90);

            auto actor2 = vtkActor::New();
            actor2->GetProperty()->SetColor(color1_C2);

            actor2->GetProperty()->ShadingOn();
            actor2->SetUserTransform(transform);
            actor2->SetPickable(false);
            actor2->GetProperty()->SetInterpolationToGouraud();

            auto mapper2 =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper2->SetInputData(cylinder->GetOutput());
            actor2->SetMapper(mapper2);
            m_renderer->AddActor(actor2);
        }

        //disks inside XOY
        if(list_C2_visible[4]>0)
            for(unsigned int i=0;i<2;i++)
            {
                auto transform=  vtkSmartPointer<vtkTransform>::New();
                transform->PostMultiply();transform->Scale(discC2_scale,1.0,discC2_scale);

                transform->Translate(0.0,l,0.0);
                transform->RotateY(90);

                transform->RotateZ(45+i*180);

                auto actor = vtkActor::New();
                actor->GetProperty()->SetColor(color1_C2);

                actor->GetProperty()->ShadingOn();
                actor->SetUserTransform(transform);
                actor->GetProperty()->SetInterpolationToGouraud();

                auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputConnection(bfPolyData->GetOutputPort() );
                mapper->ScalarVisibilityOff();
                actor->SetMapper(mapper);
                actor->SetPickable(false);

                m_renderer->AddActor(actor);
            }

        if(list_C2_visible[5]>0)
            for(unsigned int i=0;i<2;i++)
            {
                auto transform=  vtkSmartPointer<vtkTransform>::New();
                transform->PostMultiply();transform->Scale(discC2_scale,1.0,discC2_scale);

                transform->Translate(0.0,l,0.0);
                transform->RotateY(90);

                transform->RotateZ(135+i*180);

                auto actor = vtkActor::New();
                actor->GetProperty()->SetColor(color1_C2);

                actor->GetProperty()->ShadingOn();
                actor->SetUserTransform(transform);
                actor->GetProperty()->SetInterpolationToGouraud();

                auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputConnection(bfPolyData->GetOutputPort() );
                mapper->ScalarVisibilityOff();
                actor->SetMapper(mapper);
                actor->SetPickable(false);

                m_renderer->AddActor(actor);
            }
   // }
}


void View3D::renderI( )
{
    //  C5^C2=31.717474413001757
    //  C5^C2=58.28252556502243
    //  C5^C3=37.377368142265787
    //  C5^C5=63.43494882292201
    //  C3^C3=???
    //  C3^C2=???
    //  C5={0,0,1};C5={sin63.434948822922,0.0,cos63.434948822922}
    //  C3={cos36.0*sin37.377368142265787,sin36.0*sin37.377368142265787,cos37.377368142265787}
    //  C2={sin31.717474413001757,0.0,cos31.717474413001757}

    double length=HorizontalLengh*1.1;


    if(_isCnVisible && _isC5_Visible) //----------------------------------
    {
        //vertical
        auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
        cylinder->SetCenter(0.0, 0.0, 0.0);
        cylinder->SetResolution(5);
        cylinder->SetHeight(discThickness*thickness_Cn_Scale);
        cylinder->SetCapping(true);
        cylinder->SetRadius(discRadius*discCn_scale);
        cylinder->Update();

        for(unsigned int i=0;i<6;i++)
        {
            if(list_C5_visible[i]<1) continue;
            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateX(90.0);
            transform->Translate(0.0,0.0,Cn_Length);


            if(i>0) {
                transform->RotateY(63.4349);
                transform->RotateZ(72*(i-1));
            }
            else
                transform->RotateZ(36.0);


            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_Cn);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);
            m_renderer->AddActor(actor);
        }

        for(unsigned int i=0;i<6;i++)
        {
            if(list_C5_visible[i]<1) continue;
            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->RotateX(90.0);
            rotation->Translate(0.0,0.0,-Cn_Length);

            if(i>0) {
                if(horizontalC2_type==1)
                    rotation->RotateZ(36.0);
                rotation->RotateY(63.4349);
                rotation->RotateZ(72*(i-1));
            }
            else {
                if(horizontalC2_type>1)
                    rotation->RotateZ(36.0);
                //rotation->RotateZ(36.0);
            }

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_Cn);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(rotation);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);
            m_renderer->AddActor(actor);
        }


        auto  cylinderAxis =  vtkSmartPointer<vtkCylinderSource>::New();
        cylinderAxis->SetCenter(0.0, 0.0, 0.0);
        cylinderAxis->SetResolution(20);
        cylinderAxis->SetHeight(Cn_Length*2.0);
        cylinderAxis->SetRadius(Cn_Radius);
        cylinderAxis->SetCapping(true);
        cylinderAxis->Update();

        for(unsigned int i=0;i<6;i++)
        {
            if(list_C5_visible[i]<1) continue;
            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateX(90.0);

            if(i>0) {
                transform->RotateY(63.4349);
                transform->RotateZ(72*(i-1));
            }

            auto actorAxis = vtkActor::New();
            actorAxis->GetProperty()->SetColor(color_Cn);
            actorAxis->GetProperty()->ShadingOn();
            actorAxis->GetProperty()->SetInterpolationToGouraud();
            actorAxis->SetUserTransform(transform);
            actorAxis->SetPickable(false);

            auto mapperAxis =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapperAxis->SetInputData(cylinderAxis->GetOutput());
            actorAxis->SetMapper(mapperAxis);
            m_renderer->AddActor(actorAxis);
        }
    }


    if(_isC3_Visible)//-----------------------------------------
    {
        length=C3_Length*2.0;

        auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
        cylinder->SetCenter(0.0, 0.0, 0.0);
        cylinder->SetResolution(3);
        cylinder->SetRadius(discRadius*discC3_scale);
        cylinder->SetHeight(discThickness*thickness_C3_Scale);
        cylinder->SetCapping(true);
        cylinder->Update();

        for(unsigned int i=0;i<5;i++)
        {
            if(list_C3_visible[i]<1) continue;

            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateX(90.0);
            transform->Translate(0.0,0.0,length*0.5);

            transform->RotateY(37.377368142265787);
            transform->RotateZ(36.0+72.0*i);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_C3);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);
            m_renderer->AddActor(actor);
        }


        for(unsigned int i=0;i<5;i++)
        {
            if(list_C3_visible[i]<1) continue;

            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateX(90.0);
            if(horizontalC2_type==1)
                transform->RotateZ(60.0);
            transform->Translate(0.0,0.0,length*0.5);

            transform->RotateY(180.0+37.377368142265787);
            transform->RotateZ(36.0+72.0*i);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_C3);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);
            m_renderer->AddActor(actor);
        }

        for(unsigned int i=0;i<5;i++)
        {
            if(list_C3_visible[i+5]<1) continue;

            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateX(90.0);



            transform->Translate(0.0,0.0,length*0.5);

            transform->RotateY(100.8123);
            transform->RotateZ(72*i);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_C3);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);
            m_renderer->AddActor(actor);
        }

        for(unsigned int i=0;i<5;i++)
        {
            if(list_C3_visible[i+5]<1) continue;

            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();

            transform->RotateX(90.0);
            if(horizontalC2_type==1)
                transform->RotateZ(60.0);
            transform->Translate(0.0,0.0,length*0.5);


            transform->RotateY(280.8123);
            transform->RotateZ(72*i);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_C3);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);
            m_renderer->AddActor(actor);
        }


        auto  cylinderAxis =  vtkSmartPointer<vtkCylinderSource>::New();
        cylinderAxis->SetCenter(0.0, 0.0, 0.0);
        cylinderAxis->SetResolution(20);
        cylinderAxis->SetHeight(length);
        cylinderAxis->SetRadius(C3_Radius);
        cylinderAxis->SetCapping(true);
        cylinderAxis->Update();

        for(unsigned int i=0;i<5;i++)
        {
            if(list_C3_visible[i]<1) continue;

            auto rotation1=  vtkSmartPointer<vtkTransform>::New();
            rotation1->PostMultiply();
            rotation1->RotateX(90.0);

            rotation1->RotateY(37.377368142265787);
            rotation1->RotateZ(36.0+72.0*i);

            auto actorAxis = vtkActor::New();
            actorAxis->GetProperty()->SetColor(color_C3);
            actorAxis->GetProperty()->ShadingOn();
            actorAxis->GetProperty()->SetInterpolationToGouraud();
            actorAxis->SetUserTransform(rotation1);
            actorAxis->SetPickable(false);

            auto mapperAxis =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapperAxis->SetInputData(cylinderAxis->GetOutput());
            actorAxis->SetMapper(mapperAxis);
            m_renderer->AddActor(actorAxis);
        }

        for(unsigned int i=0;i<5;i++)
        {
            if(list_C3_visible[i+5]<1) continue;
            auto rotation1=  vtkSmartPointer<vtkTransform>::New();
            rotation1->PostMultiply();
            rotation1->RotateX(90.0);
            if(horizontalC2_type==1)
                rotation1->RotateZ(60.0);

            rotation1->RotateY(100.8123);
            rotation1->RotateZ(72*i);

            auto actorAxis = vtkActor::New();
            actorAxis->GetProperty()->SetColor(color_C3);
            actorAxis->GetProperty()->ShadingOn();
            actorAxis->GetProperty()->SetInterpolationToGouraud();
            actorAxis->SetUserTransform(rotation1);
            actorAxis->SetPickable(false);

            auto mapperAxis =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapperAxis->SetInputData(cylinderAxis->GetOutput());
            actorAxis->SetMapper(mapperAxis);
            m_renderer->AddActor(actorAxis);
        }
    }



    if(_isC2_1_Visible)
    {
        length=C2_Length*2.0;

        for(unsigned int i=0;i<5;i++)
        {
            if(list_C2_visible[i]<1) continue;

            auto rotation =  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(discC2_scale,1.0,discC2_scale);
            rotation->RotateX(90.0);
            rotation->Translate(0.0,0.0,length*0.5);

            rotation->RotateY(31.717);


            //!  C5^C2=31.717474413001757
            //!  C5^C2=58.28252556502243

            rotation->RotateZ(72*i);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color1_C2);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();
            actor->SetPickable(false);

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection( bfPolyData->GetOutputPort() );
            mapper->ScalarVisibilityOff();
            actor->SetMapper(mapper);
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();

            m_renderer->AddActor(actor);
        }

        for(unsigned int i=0;i<5;i++)
        {
            if(list_C2_visible[i]<1) continue;

            auto rotation =  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(discC2_scale,1.0,discC2_scale);
            rotation->RotateX(90.0);
            rotation->Translate(0.0,0.0,length*0.5);

            rotation->RotateY(211.717);

            //!  C5^C2=31.717474413001757
            //!  C5^C2=58.28252556502243

            rotation->RotateZ(72*i);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color1_C2);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();
            actor->SetPickable(false);

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection( bfPolyData->GetOutputPort() );
            mapper->ScalarVisibilityOff();
            actor->SetMapper(mapper);
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();

            m_renderer->AddActor(actor);
        }



        for(unsigned int i=0;i<5;i++)
        {
            if(list_C2_visible[i+5]<1) continue;

            auto rotation =  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(discC2_scale,1.0,discC2_scale);
            rotation->RotateX(90.0);
            rotation->Translate(0.0,0.0,length*0.5);
            rotation->RotateX(90.0);
            rotation->RotateY(-58.3);




            if(i%2<1)
                rotation->RotateZ(36*i);
            else
                rotation->RotateZ(180+36*i);

            //!  C5^C2=31.717474413001757
            //!  C5^C2=58.28252556502243



            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color1_C2);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();
            actor->SetPickable(false);

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection( bfPolyData->GetOutputPort() );
            mapper->ScalarVisibilityOff();
            actor->SetMapper(mapper);
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();

            m_renderer->AddActor(actor);
        }

        for(unsigned int i=0;i<5;i++)
        {
            if(list_C2_visible[i+5]<1) continue;

            auto rotation =  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(discC2_scale,1.0,discC2_scale);
            rotation->RotateX(90.0);
            rotation->Translate(0.0,0.0,length*0.5);
            rotation->RotateX(90.0);
            rotation->RotateY(61.72);


            if(i%2<1)
                rotation->RotateZ(180+36*i);
            else
                rotation->RotateZ(36*i);


            //!  C5^C2=31.717474413001757
            //!  C5^C2=58.28252556502243



            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color1_C2);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();
            actor->SetPickable(false);

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection( bfPolyData->GetOutputPort() );
            mapper->ScalarVisibilityOff();
            actor->SetMapper(mapper);
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();

            m_renderer->AddActor(actor);
        }


        //!  C5^C2=31.717474413001757
        //!  C5^C2=58.28252556502243
        //!  C5^C3=37.377368142265787
        for(unsigned int i=0;i<5;i++)
        {
            if(list_C2_visible[i+10]<1) continue;

            auto rotation =  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(discC2_scale,1.0,discC2_scale);
            rotation->RotateX(90.0);
            rotation->Translate(0.0,0.0,length*0.5);
            rotation->RotateZ(90);
            rotation->RotateY(58.283);


            //!  C5^C2=31.717474413001757
            //!  C5^C2=58.28252556502243

            rotation->RotateZ(36+72.0*i);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color1_C2);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();
            actor->SetPickable(false);

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection( bfPolyData->GetOutputPort() );
            mapper->ScalarVisibilityOff();
            actor->SetMapper(mapper);
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();

            m_renderer->AddActor(actor);
        }


        for(unsigned int i=0;i<5;i++)
        {
            if(list_C2_visible[i+10]<1) continue;

            auto rotation =  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(discC2_scale,1.0,discC2_scale);
            rotation->RotateX(90.0);
            rotation->Translate(0.0,0.0,length*0.5);
            rotation->RotateZ(90);


            rotation->RotateY(238.283);

            //!  C5^C2=31.717474413001757
            //!  C5^C2=58.28252556502243

            rotation->RotateZ(36+72.0*i);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color1_C2);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();
            actor->SetPickable(false);

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection( bfPolyData->GetOutputPort() );
            mapper->ScalarVisibilityOff();
            actor->SetMapper(mapper);
            actor->SetUserTransform(rotation);
            actor->GetProperty()->SetInterpolationToGouraud();

            m_renderer->AddActor(actor);
        }



        auto  cylinderAxis =  vtkSmartPointer<vtkCylinderSource>::New();
        cylinderAxis->SetCenter(0.0, 0.0, 0.0);
        cylinderAxis->SetResolution(20);
        cylinderAxis->SetHeight(length);
        cylinderAxis->SetRadius(C2_Radius);
        cylinderAxis->SetCapping(true);
        cylinderAxis->Update();

        for(unsigned int i=0;i<5;i++)
        {
            if(list_C2_visible[i]<1) continue;
            auto rotation =  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->RotateX(90.0);
            rotation->RotateY(31.717);

            rotation->RotateZ(72*i);

            auto actorAxis = vtkActor::New();
            actorAxis->GetProperty()->SetColor(color1_C2);
            actorAxis->GetProperty()->ShadingOn();
            actorAxis->GetProperty()->SetInterpolationToGouraud();
            actorAxis->SetUserTransform(rotation);
            actorAxis->SetPickable(false);

            auto mapperAxis =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapperAxis->SetInputData(cylinderAxis->GetOutput());
            actorAxis->SetMapper(mapperAxis);
            m_renderer->AddActor(actorAxis);
        }



        for(unsigned int i=0;i<5;i++)
        {
            if(list_C2_visible[i+5]<1) continue;
            auto rotation =  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();

            rotation->RotateZ(36*i);

            auto actorAxis = vtkActor::New();
            actorAxis->GetProperty()->SetColor(color1_C2);
            actorAxis->GetProperty()->ShadingOn();
            actorAxis->GetProperty()->SetInterpolationToGouraud();
            actorAxis->SetUserTransform(rotation);
            actorAxis->SetPickable(false);

            auto mapperAxis =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapperAxis->SetInputData(cylinderAxis->GetOutput());
            actorAxis->SetMapper(mapperAxis);
            m_renderer->AddActor(actorAxis);
        }


        for(unsigned int i=0;i<5;i++)
        {
            if(list_C2_visible[i+10]<1) continue;
            auto rotation =  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->RotateY(90.0);
            rotation->RotateX(90.0);
            rotation->RotateY(121.7);
            rotation->RotateZ(180+36);

            rotation->RotateZ(72.0*i);

            auto actorAxis = vtkActor::New();
            actorAxis->GetProperty()->SetColor(color1_C2);
            actorAxis->GetProperty()->ShadingOn();
            actorAxis->GetProperty()->SetInterpolationToGouraud();
            actorAxis->SetUserTransform(rotation);
            actorAxis->SetPickable(false);

            auto mapperAxis =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapperAxis->SetInputData(cylinderAxis->GetOutput());
            actorAxis->SetMapper(mapperAxis);
            m_renderer->AddActor(actorAxis);
        }

    }
}




void View3D::renderT()
{
    //renderCubeSkeleton(C2_Length);

    renderCubeSkeleton(HorizontalLengh);

    //principal axis
    if(_isCnVisible )
    {
        if(list_S4_visible[0]>0)
        {
            renderVerticalC2(0.0,C2_Length,color_Cn);

            //render I4
            if(_isI2nVisible && isI2n)
            {
                auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
                cylinder->SetCenter(0.0, 0.0, 0.0);
                cylinder->SetResolution(4);
                cylinder->SetHeight(discThickness*0.9);
                cylinder->SetCapping(true);
                cylinder->SetRadius(discC2_scale*discRadius*1.2);
                cylinder->Update();

                //two times
                for (unsigned int i=0;i<2;i++)
                {
                    auto transform=  vtkSmartPointer<vtkTransform>::New();
                    transform->PostMultiply();
                    transform->Translate(0.0,C2_Length*0.5,0.0);
                    transform->RotateX(90+180*i);


                    auto actor = vtkActor::New();
                    actor->GetProperty()->SetColor(white);
                    actor->GetProperty()->ShadingOn();
                    actor->SetUserTransform(transform);
                    actor->SetPickable(false);
                    actor->GetProperty()->SetInterpolationToGouraud();

                    auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
                    mapper->SetInputData(cylinder->GetOutput());
                    actor->SetMapper(mapper);
                    m_renderer->AddActor(actor);
                }
            }
        }
    }


    //if(_isC2_1_Visible) {

        if(list_S4_visible[1]>0)
            renderHorizontalC2(0.0,C2_Length,color_Cn);

        if(list_S4_visible[2]>0)
            renderHorizontalC2(90.0,C2_Length,color_Cn);

        if(_isI2nVisible && isI2n)
        {
            //render I4
            auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
            cylinder->SetCenter(0.0, 0.0, 0.0);
            cylinder->SetResolution(4);
            cylinder->SetHeight(discThickness*0.9);
            cylinder->SetCapping(true);
            cylinder->SetRadius(discC2_scale*discRadius*1.2);
            cylinder->Update();

            //four times
            for (unsigned int i=0;i<4;i++)
            {
                if( i==0 ||i==2)
                    if (list_S4_visible[2]<1) continue;

                if( i==1 ||i==3)
                    if(list_S4_visible[1]< 1) continue;

                auto transform=  vtkSmartPointer<vtkTransform>::New();
                transform->PostMultiply();
                transform->Translate(0.0,C2_Length*0.5,0.0);
                transform->RotateZ(90*i);

                auto actor = vtkActor::New();
                actor->GetProperty()->SetColor(white);
                actor->GetProperty()->ShadingOn();
                actor->SetUserTransform(transform);
                actor->SetPickable(false);
                actor->GetProperty()->SetInterpolationToGouraud();

                auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputData(cylinder->GetOutput());
                actor->SetMapper(mapper);
                m_renderer->AddActor(actor);
            }
        }

   // }



    // cout << "list_C3_visible"<< endl;
    //  for(uint j=0;j<list_C3_visible.size();j++)
    //      cout << list_C3_visible[j]<< endl;


    if(_isC3_Visible)
    {
        //double l=length*0.5;
        double l=C3_Length*0.5;

        //up
        for(unsigned int i=0;i<2;i++)
        {
            if(list_C3_visible[i] < 1) continue;

            auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
            cylinder->SetCenter(0.0, 0.0, 0.0);
            cylinder->SetResolution(3);
            cylinder->SetHeight(discThickness);
            cylinder->SetCapping(true);
            cylinder->SetRadius(discRadius*discC3_scale);
            cylinder->Update();

            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateWXYZ(90.0,1,0,0);
            transform->RotateWXYZ(54.7,0,1,0);

            transform->RotateWXYZ(45.0+180.0*i,0,0,1);
            transform->Translate(l*cos(3.142*i),l*cos(3.142*i),l);
            transform->RotateWXYZ(60.0,cos(3.142*i),cos(3.142*i),1);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_C3);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);


            m_renderer->AddActor(actor);

        }


        //below
        for(unsigned int i=0;i<2;i++)
        {
            if(list_C3_visible[i+2] < 1) continue;

            auto  cylinder =  vtkSmartPointer<vtkCylinderSource>::New();
            cylinder->SetCenter(0.0, 0.0, 0.0);
            cylinder->SetResolution(3);
            cylinder->SetHeight(discThickness);
            cylinder->SetCapping(true);
            cylinder->SetRadius(discRadius*discC3_scale);
            cylinder->Update();

            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateWXYZ(90.0,1,0,0);
            transform->RotateWXYZ(35.3+90.0,0,1,0);

            transform->RotateWXYZ(135.0+180.0*i,0,0,1);
            transform->Translate(-l*cos(3.142*i),l*cos(3.142*i),-l);

            auto actor = vtkActor::New();
            actor->GetProperty()->SetColor(color_C3);
            actor->GetProperty()->ShadingOn();
            actor->SetUserTransform(transform);
            actor->SetPickable(false);

            actor->GetProperty()->SetInterpolationToGouraud();

            auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(cylinder->GetOutput());
            actor->SetMapper(mapper);
            m_renderer->AddActor(actor);
        }



        vector3 beg(l,l,l);
        vector3 end(0,0,0);

        if(list_C3_visible[0] > 0)
            renderTube(beg,end,C3_Radius,color_C3);


        beg.Set(-l,-l,l);
        end.Set(0,0,0);

        if(list_C3_visible[1] > 0)
            renderTube(beg,end,C3_Radius,color_C3);

        beg.Set(-l,l,-l);
        end.Set(0,0,0);

        if(list_C3_visible[2] > 0)
            renderTube(beg,end,C3_Radius,color_C3);

        beg.Set(l,-l,-l);
        end.Set(0,0,0);
        if(list_C3_visible[3] > 0)
            renderTube(beg,end,C3_Radius,color_C3);
    }
}


void View3D::renderTh( )
{
    renderT();

    //render center
    if(_isCiVisible)
    {
        auto sphere = vtkSphereSource::New();
        sphere->SetPhiResolution(20);
        sphere->SetThetaResolution(20);
        sphere->Update();
        sphere->SetRadius(0.05*discC2_scale);
        auto actor = vtkActor::New();
        auto mapper = vtkPolyDataMapper::New();
        actor->GetProperty()->SetColor(yellow);
        actor->SetMapper(mapper);
        actor->SetPickable(false);

        mapper->SetInputConnection(sphere->GetOutputPort());
        m_renderer->AddActor(actor);
    }

    //render six center
    if(_isCiVisible )
    {
        auto sphere = vtkSphereSource::New();
        sphere->SetPhiResolution(20);
        sphere->SetThetaResolution(20);
        sphere->Update();
        sphere->SetRadius(0.04*discC2_scale);

        // cout<<"list_S4_visible"<<endl;
        // for(uint i=0;i<3;i++ )       cout << list_S4_visible[i]<< " ";
        //  cout<<endl;

        //z
        if(_isCnVisible)
            for(unsigned int i=0;i<2;i++)
            {
                auto rotation=  vtkSmartPointer<vtkTransform>::New();
                rotation->PostMultiply();
                rotation->Translate(0.0,0.0,C2_Length/2.0);
                rotation->RotateX(180.0*i);
                auto actor = vtkActor::New();
                auto mapper = vtkPolyDataMapper::New();
                actor->GetProperty()->SetColor(yellow);
                actor->SetUserTransform(rotation);

                actor->SetMapper(mapper);
                actor->SetPickable(false);
                mapper->SetInputConnection(sphere->GetOutputPort());
                m_renderer->AddActor(actor);
            }


        //x
        if(list_S4_visible[1]>0 && _isC2_1_Visible)
            for(unsigned int i=0;i<2;i++)
            {
                auto rotation=  vtkSmartPointer<vtkTransform>::New();
                rotation->PostMultiply();
                rotation->Translate(C2_Length/2.0,0.0,0.0);
                rotation->RotateY(180.0*i);
                auto actor = vtkActor::New();
                auto mapper = vtkPolyDataMapper::New();
                actor->GetProperty()->SetColor(yellow);
                actor->SetMapper(mapper);
                actor->SetPickable(false);
                actor->SetUserTransform(rotation);

                mapper->SetInputConnection(sphere->GetOutputPort());
                m_renderer->AddActor(actor);
            }


        //y
        if(list_S4_visible[2]>0 && _isC2_1_Visible)
            for(unsigned int i=0;i<2;i++)
            {
                auto rotation=  vtkSmartPointer<vtkTransform>::New();
                rotation->PostMultiply();
                rotation->Translate(0.0,C2_Length/2.0,0.0);
                rotation->RotateX(180.0*i);
                auto actor = vtkActor::New();
                auto mapper = vtkPolyDataMapper::New();
                actor->GetProperty()->SetColor(yellow);
                actor->SetMapper(mapper);
                actor->SetPickable(false);
                actor->SetUserTransform(rotation);

                mapper->SetInputConnection(sphere->GetOutputPort());
                m_renderer->AddActor(actor);
            }

    }

    //render three circle/square SigmaH
    auto polygonSource =  vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource->SetNumberOfSides(1000);
    polygonSource->SetRadius(HorizontalLengh/2.0);

    auto circle =  vtkSmartPointer<vtkRegularPolygonSource>::New();
    circle->SetNumberOfSides(1000);
    circle->SetRadius(HorizontalLengh/2.0);
    circle->GeneratePolygonOff();

    if(_isSigmaVVisible && typeSigmaV==2)
    {
        auto actor = vtkActor::New();
        auto mapper = vtkPolyDataMapper::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetOpacity(OpacityV);
        actor->GetProperty()->SetColor(color_sigmaV);
        actor->GetProperty()->ShadingOn();
        actor->SetPickable(false);

        auto rotation=  vtkSmartPointer<vtkTransform>::New();
        rotation->PostMultiply();
        rotation->Scale(1.0,verticalLength/HorizontalLengh,1.0);
        rotation->RotateX(90.0);
        rotation->RotateZ(90.0);
        actor->SetUserTransform(rotation);

        mapper->ScalarVisibilityOn();
        mapper->SetInputConnection(polygonSource->GetOutputPort());

        actor->GetProperty()->SetInterpolationToGouraud();
        m_renderer->AddActor(actor);

        auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper1->SetInputConnection(circle->GetOutputPort());;
        auto actor1 =  vtkSmartPointer<vtkActor>::New();
        actor1->SetPickable(false);

        //actor1->GetProperty()->SetOpacity(OpacityV);
        actor1->GetProperty()->SetColor(color_sigmaV);
        actor1->GetProperty()->ShadingOff();
        actor1->SetUserTransform(rotation);
        actor1->SetMapper(mapper1);
        m_renderer->AddActor(actor1);
    }


    if(_isSigmaDVisible && typeSigmaD==2)
    {
        auto actor = vtkActor::New();
        auto mapper = vtkPolyDataMapper::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(color_sigmaD);
        actor->GetProperty()->SetOpacity(OpacityD);
        actor->GetProperty()->ShadingOn();
        actor->SetPickable(false);

        auto rotation=  vtkSmartPointer<vtkTransform>::New();
        rotation->PostMultiply();
        rotation->Scale(1.0,verticalLength/HorizontalLengh,1.0);
        rotation->RotateX(90.0);
        actor->SetUserTransform(rotation);

        mapper->ScalarVisibilityOn();
        mapper->SetInputConnection(polygonSource->GetOutputPort());

        actor->GetProperty()->SetInterpolationToGouraud();
        m_renderer->AddActor(actor);

        auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper1->SetInputConnection(circle->GetOutputPort());;
        auto actor1 =  vtkSmartPointer<vtkActor>::New();
        actor1->SetPickable(false);

        actor1->GetProperty()->SetColor(color_sigmaD);
        //actor1->GetProperty()->SetOpacity(OpacityD);
        actor1->GetProperty()->ShadingOff();
        actor1->SetUserTransform(rotation);
        actor1->SetMapper(mapper1);
        m_renderer->AddActor(actor1);

    }

    if(_isSigmaHVisible && typeSigmaH==2)
    {
        auto actor = vtkActor::New();
        auto mapper = vtkPolyDataMapper::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetOpacity(OpacityH);
        actor->GetProperty()->SetColor(color_sigmaH);
        actor->GetProperty()->ShadingOn();
        actor->SetPickable(false);

        auto rotation=  vtkSmartPointer<vtkTransform>::New();
        rotation->PostMultiply();
        rotation->Scale(1.0,verticalLength/HorizontalLengh,1.0);
        actor->SetUserTransform(rotation);

        mapper->ScalarVisibilityOn();
        mapper->SetInputConnection(polygonSource->GetOutputPort());

        actor->GetProperty()->SetInterpolationToGouraud();
        m_renderer->AddActor(actor);

        auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper1->SetInputConnection(circle->GetOutputPort());;
        auto actor1 =  vtkSmartPointer<vtkActor>::New();
        actor1->SetPickable(false);

        actor1->GetProperty()->SetColor(color_sigmaH);
        //actor1->GetProperty()->SetOpacity(OpacityH);
        actor1->GetProperty()->ShadingOff();
        actor1->SetUserTransform(rotation);
        actor1->SetMapper(mapper1);
        m_renderer->AddActor(actor1);

    }



    double l=HorizontalLengh;
    auto cube = vtkSmartPointer<vtkCubeSource>::New();
    cube->SetCenter(0.0, 0.0, 0.0);
    cube->SetXLength(thicknessV);
    cube->SetYLength(l);
    cube->SetZLength(l);
    cube->Update();



    if(_isSigmaVVisible && typeSigmaV==1)
    {
        auto actor = vtkActor::New();
        auto mapper = vtkPolyDataMapper::New();

        actor->SetMapper(mapper);
        actor->GetProperty()->ShadingOn();
        actor->SetPickable(false);

        actor->GetProperty()->SetColor(color_sigmaV);
        actor->GetProperty()->SetOpacity(OpacityV);


        mapper->ScalarVisibilityOn();
        mapper->SetInputConnection(cube->GetOutputPort());
        actor->GetProperty()->SetInterpolationToGouraud();
        m_renderer->AddActor(actor);
        mapper->Delete();




        //Line-frame
        double shift=0.01;
        double v=l*0.5-shift; //hide a little

        double p1[3] = {-v,0,-v};
        double p2[3] = {v,0,-v};
        double p3[3] = {v,0,v};
        double p4[3] = {-v,0,v};

        auto points =  vtkSmartPointer<vtkPoints>::New();

        points->InsertNextPoint(p1);
        points->InsertNextPoint(p2);
        points->InsertNextPoint(p3);
        points->InsertNextPoint(p4);
        points->InsertNextPoint(p1);

        auto line=vtkSmartPointer<vtkLineSource>::New();
        line->SetPoints(points);
        line->Update();

        auto actor_outline = vtkActor::New();
        actor_outline->GetProperty()->SetColor(color_sigmaV);
        actor_outline->GetProperty()->ShadingOff();

        actor_outline->GetProperty()->SetLineStipplePattern(0xf0f0);
        actor_outline->GetProperty()->SetLineStippleRepeatFactor(1);

        actor_outline->SetPickable(false);
        actor_outline->GetProperty()->SetPointSize(1.0);
        actor_outline->GetProperty()->SetLineWidth(1.0);
        actor_outline->GetProperty()->ShadingOff();

        auto rotation1=  vtkSmartPointer<vtkTransform>::New();
        rotation1->PostMultiply();
        rotation1->RotateZ(90.0);
        actor_outline->SetUserTransform(rotation1);

        auto mapper_skeleton =  vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper_skeleton->SetInputConnection( line->GetOutputPort() );
        mapper_skeleton->ScalarVisibilityOn();

        actor_outline->SetMapper(mapper_skeleton);
        m_renderer->AddActor(actor_outline);
    }

    if(_isSigmaDVisible && typeSigmaD==1)
    {
        cube->SetXLength(thicknessD);
        cube->Update();

        auto actor = vtkActor::New();
        auto mapper = vtkPolyDataMapper::New();

        actor->SetMapper(mapper);
        actor->GetProperty()->ShadingOn();
        actor->SetPickable(false);

        auto rotation=  vtkSmartPointer<vtkTransform>::New();
        rotation->PostMultiply();
        actor->GetProperty()->SetColor(color_sigmaD);
        actor->GetProperty()->SetOpacity(OpacityD);
        rotation->RotateZ(90.0);

        actor->SetUserTransform(rotation);

        mapper->ScalarVisibilityOn();
        mapper->SetInputConnection(cube->GetOutputPort());
        actor->GetProperty()->SetInterpolationToGouraud();
        m_renderer->AddActor(actor);
        mapper->Delete();


        //Line-frame
        double shift=0.01;
        double v=l*0.5-shift; //hide a little

        double p1[3] = {-v,0,-v};
        double p2[3] = {v,0,-v};
        double p3[3] = {v,0,v};
        double p4[3] = {-v,0,v};

        auto points =  vtkSmartPointer<vtkPoints>::New();

        points->InsertNextPoint(p1);
        points->InsertNextPoint(p2);
        points->InsertNextPoint(p3);
        points->InsertNextPoint(p4);
        points->InsertNextPoint(p1);

        auto line=vtkSmartPointer<vtkLineSource>::New();
        line->SetPoints(points);
        line->Update();

        auto actor_outline = vtkActor::New();
        actor_outline->GetProperty()->SetColor(color_sigmaD);
        actor_outline->GetProperty()->ShadingOff();

        actor_outline->GetProperty()->SetLineStipplePattern(0xf0f0);
        actor_outline->GetProperty()->SetLineStippleRepeatFactor(1);

        actor_outline->SetPickable(false);
        actor_outline->GetProperty()->SetPointSize(1.0);
        actor_outline->GetProperty()->SetLineWidth(1.0);
        actor_outline->GetProperty()->ShadingOff();

        auto rotation1=  vtkSmartPointer<vtkTransform>::New();
        rotation1->PostMultiply();
        //rotation1->RotateX(90.0);
        actor_outline->SetUserTransform(rotation1);

        auto mapper_skeleton =  vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper_skeleton->SetInputConnection( line->GetOutputPort() );
        mapper_skeleton->ScalarVisibilityOn();

        actor_outline->SetMapper(mapper_skeleton);
        m_renderer->AddActor(actor_outline);
    }

    if(_isSigmaHVisible && typeSigmaH==1)
    {
        cube->SetXLength(thicknessH);
        cube->Update();

        auto actor = vtkActor::New();
        auto mapper = vtkPolyDataMapper::New();

        actor->SetMapper(mapper);
        actor->GetProperty()->ShadingOn();
        actor->SetPickable(false);

        auto rotation=  vtkSmartPointer<vtkTransform>::New();
        rotation->PostMultiply();

        actor->GetProperty()->SetColor(color_sigmaH);
        actor->GetProperty()->SetOpacity(OpacityH);

        //actor->GetProperty()->SetColor(brown);

        rotation->RotateZ(90.0);
        rotation->RotateX(90.0);

        actor->SetUserTransform(rotation);

        mapper->ScalarVisibilityOn();
        mapper->SetInputConnection(cube->GetOutputPort());
        actor->GetProperty()->SetInterpolationToGouraud();
        m_renderer->AddActor(actor);
        mapper->Delete();



        //Line-frame
        double shift=0.01;
        double v=l*0.5-shift; //hide a little

        double p1[3] = {-v,0,-v};
        double p2[3] = {v,0,-v};
        double p3[3] = {v,0,v};
        double p4[3] = {-v,0,v};

        auto points =  vtkSmartPointer<vtkPoints>::New();

        points->InsertNextPoint(p1);
        points->InsertNextPoint(p2);
        points->InsertNextPoint(p3);
        points->InsertNextPoint(p4);
        points->InsertNextPoint(p1);

        auto line=vtkSmartPointer<vtkLineSource>::New();
        line->SetPoints(points);
        line->Update();

        auto actor_outline = vtkActor::New();
        actor_outline->GetProperty()->SetColor(color_sigmaH);
        actor_outline->GetProperty()->ShadingOff();

        actor_outline->GetProperty()->SetLineStipplePattern(0xf0f0);
        actor_outline->GetProperty()->SetLineStippleRepeatFactor(1);

        actor_outline->SetPickable(false);
        actor_outline->GetProperty()->SetPointSize(1.0);
        actor_outline->GetProperty()->SetLineWidth(1.0);
        actor_outline->GetProperty()->ShadingOff();

        auto rotation1=  vtkSmartPointer<vtkTransform>::New();
        rotation1->PostMultiply();
        rotation1->RotateX(90.0);
        actor_outline->SetUserTransform(rotation1);

        auto mapper_skeleton =  vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper_skeleton->SetInputConnection( line->GetOutputPort() );
        mapper_skeleton->ScalarVisibilityOn();

        actor_outline->SetMapper(mapper_skeleton);
        m_renderer->AddActor(actor_outline);
    }

}

void View3D::renderTd( )
{
    isI2n=true;
    renderT();
    isI2n=false;

    double l=HorizontalLengh;

    double thickness=HorizontalLengh*0.005;
    auto cube = vtkSmartPointer<vtkCubeSource>::New();
    cube->SetCenter(0.0, 0.0, 0.0);
    cube->SetXLength(thickness);
    cube->SetYLength(l*1.414);
    cube->SetZLength(l);



    if(typeSigmaD==1)
    //if(_isSigmaDVisible && typeSigmaD==1)
        for (int k=0; k<2; k++)
        {
            if(list_sigmaD_visible[k]<1) continue;
            cube->SetXLength(thicknessD);
            cube->Update();

            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();

            actor->SetMapper(mapper);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();

            actor->GetProperty()->SetColor(color_sigmaD);
            actor->GetProperty()->SetOpacity(OpacityD);
            rotation->RotateZ(angle+45.0);

            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());
            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            mapper->Delete();
        }


    //if(_isSigmaVVisible && typeSigmaV==1)
    if(typeSigmaV==1)
        for (int k=0; k<2; k++)
        {
            if(list_sigmaV_visible[k]<1) continue;
            cube->SetXLength(thicknessV);
            cube->Update();

            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();

            actor->SetMapper(mapper);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();


            actor->GetProperty()->SetColor(color_sigmaV);
            //actor->storeColor(color_sigmaV[0],color_sigmaV[1],color_sigmaV[2]);
            actor->GetProperty()->SetOpacity(OpacityV);
            rotation->RotateZ(angle+45.0);
            rotation->RotateX(90.0);

            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());
            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            mapper->Delete();
        }

    //if(_isSigmaHVisible && typeSigmaH==1)
    if(typeSigmaH==1)
        for (int k=0; k<2; k++)
        {
            if(list_sigmaH_visible[k]<1) continue;
            cube->SetXLength(thicknessH);
            cube->Update();

            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();

            actor->SetMapper(mapper);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();

            actor->GetProperty()->SetColor(color_sigmaH);
            //actor->storeColor(color_sigmaV[0],color_sigmaV[1],color_sigmaV[2]);
            actor->GetProperty()->SetOpacity(OpacityV);
            rotation->RotateZ(angle+45.0);
            rotation->RotateY(90.0);

            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());
            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            mapper->Delete();
        }



    //render three circle SigmaD
    auto polygonSource =  vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource->SetNumberOfSides(1000);
    polygonSource->SetRadius(HorizontalLengh/2.0);

    auto circle =  vtkSmartPointer<vtkRegularPolygonSource>::New();
    circle->SetNumberOfSides(1000);
    circle->SetRadius(HorizontalLengh/2.0);
    circle->GeneratePolygonOff();

    //if(_isSigmaHVisible && typeSigmaH==2)
    if(typeSigmaH==2)
    {
        for (int k=0; k<2; k++)
        {
            if(list_sigmaH_visible[k]<1) continue;
            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityH);
            actor->GetProperty()->SetColor(color_sigmaH);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(1.0,1.414,1.0);
            rotation->RotateX(90.0);
            rotation->RotateX(angle+45.0);
            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(color_sigmaH);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }
    }

   // if(_isSigmaVVisible && typeSigmaV==2)
     if(typeSigmaV==2)
        for (int k=0; k<2; k++)
        {
            if(list_sigmaV_visible[k]<1) continue;
            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityV);
            actor->GetProperty()->SetColor(color_sigmaV);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(1.414,1.0,1.0);
            rotation->RotateY(angle+45.0);
            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(color_sigmaV);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }


  //  if(_isSigmaDVisible && typeSigmaD==2)
       if(typeSigmaD==2)
        for (int k=0; k<2; k++)
        {
            if(list_sigmaD_visible[k]<1) continue;
            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityD);
            actor->GetProperty()->SetColor(color_sigmaD);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(1.414,1.0,1.0);
            rotation->RotateX(90.0);
            rotation->RotateZ(angle+45.0);
            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(color_sigmaD);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }
}



void View3D::renderOh( )
{
    renderO();

    if(_isCiVisible)
    {
        auto sphere = vtkSphereSource::New();
        sphere->SetPhiResolution(20);
        sphere->SetThetaResolution(20);
        sphere->Update();
        sphere->SetRadius(0.05*discCn_scale);
        auto actor = vtkActor::New();
        auto mapper = vtkPolyDataMapper::New();
        actor->GetProperty()->SetColor(yellow);
        actor->SetMapper(mapper);
        actor->SetPickable(false);

        mapper->SetInputConnection(sphere->GetOutputPort());
        m_renderer->AddActor(actor);
    }

    //render Center
    if(_isCiVisible && _isCnVisible)
    {
        auto sphere = vtkSphereSource::New();
        sphere->SetPhiResolution(20);
        sphere->SetThetaResolution(20);
        sphere->Update();
        sphere->SetRadius(0.05*discCn_scale);


        if(list_C4_visible[0]>0)
            for(unsigned int i=0;i<2;i++)
            {
                auto rotation=  vtkSmartPointer<vtkTransform>::New();
                rotation->PostMultiply();
                rotation->Translate(0.0,0.0,Cn_Length/2.0);
                rotation->RotateY(180.0*i);
                auto actor = vtkActor::New();
                auto mapper = vtkPolyDataMapper::New();
                actor->GetProperty()->SetColor(yellow);
                actor->SetMapper(mapper);
                actor->SetPickable(false);
                actor->SetUserTransform(rotation);

                mapper->SetInputConnection(sphere->GetOutputPort());
                m_renderer->AddActor(actor);
            }


        if(list_C4_visible[1]>0)
            for(unsigned int i=0;i<4;i++)
            {
                auto rotation=  vtkSmartPointer<vtkTransform>::New();
                rotation->PostMultiply();
                rotation->Translate(0.0,0.0,Cn_Length/2.0);
                rotation->RotateY(90.0+180.0*i);
                auto actor = vtkActor::New();
                auto mapper = vtkPolyDataMapper::New();
                actor->GetProperty()->SetColor(yellow);
                actor->SetMapper(mapper);
                actor->SetPickable(false);
                actor->SetUserTransform(rotation);

                mapper->SetInputConnection(sphere->GetOutputPort());
                m_renderer->AddActor(actor);
            }


        if(list_C4_visible[2]>0)
            for(unsigned int i=0;i<2;i++) {
                auto rotation=  vtkSmartPointer<vtkTransform>::New();
                rotation->PostMultiply();
                rotation->Translate(0.0,Cn_Length/2.0,0.0);
                rotation->RotateX(180.0*i);
                auto actor = vtkActor::New();
                auto mapper = vtkPolyDataMapper::New();
                actor->GetProperty()->SetColor(yellow);
                actor->SetMapper(mapper);
                actor->SetPickable(false);
                actor->SetUserTransform(rotation);

                mapper->SetInputConnection(sphere->GetOutputPort());
                m_renderer->AddActor(actor);
            }
    }


    //render three sigmaH
    double l = verticalLength;
    double thickness=thicknessH;

    //if(_isSigmaHVisible && typeSigmaH==1)
    if(typeSigmaH==1)
    {
        auto cube = vtkSmartPointer<vtkCubeSource>::New();
        cube->SetCenter(0.0, 0.0, 0.0);
        cube->SetXLength(thickness);
        cube->SetYLength(l);
        cube->SetZLength(l);
        cube->Update();

        for (int k=0; k<3; k++)
        {
            if(list_sigmaH_visible[k]<1) continue;

            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();

            actor->SetMapper(mapper);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);
            actor->GetProperty()->SetColor(color_sigmaH);
            actor->GetProperty()->SetOpacity(OpacityH);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();


            if (k<1)
            {
                rotation->RotateZ(90.0);
                rotation->RotateX(90.0);
            }
            else {
                rotation->RotateZ(angle);
            }


            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());
            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            mapper->Delete();
        }
    }

    l=HorizontalLengh;
    thickness=thicknessV;
    auto cube = vtkSmartPointer<vtkCubeSource>::New();
    cube->SetCenter(0.0, 0.0, 0.0);
    cube->SetXLength(thickness);
    cube->SetYLength(l*1.414);
    cube->SetZLength(l);
    cube->Update();

    //cout << " list_sigmaD_visible"<<endl;
    //for (auto i : list_sigmaD_visible)
    //    cout << i<<endl;

   // if(_isSigmaVVisible && typeSigmaV==1  )
    if(typeSigmaV==1)
    {
        for (int k=0; k<2; k++)
        {
            if (list_sigmaD_visible[k]<1) continue;

            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();

            actor->SetMapper(mapper);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();

            rotation->RotateZ(angle+45.0);

            actor->GetProperty()->SetColor(color_sigmaV);
            actor->GetProperty()->SetOpacity(OpacityV);
            // rotation->RotateZ(angle+45.0);
            // rotation->RotateX(90.0);

            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());
            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            mapper->Delete();
        }
    }

    //if(_isSigmaDVisible && typeSigmaD==1)
    if(typeSigmaD==1)
    {
        //six sigmaD
        for (int k=0; k<2; k++)
        {
            if(list_sigmaD_visible[k+2]<1) continue;

            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();

            actor->SetMapper(mapper);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();

            actor->GetProperty()->SetColor(brown);
            actor->GetProperty()->SetOpacity(OpacityD);


            rotation->RotateZ(angle+45.0);
            rotation->RotateX(90.0);

            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());
            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            mapper->Delete();
        }

        for (int k=0; k<2; k++)
        {
            if(list_sigmaD_visible[k+4]<1) continue;

            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();

            actor->SetMapper(mapper);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();

            actor->GetProperty()->SetColor(color_sigmaD);
            //actor->storeColor(color_sigmaV[0],color_sigmaV[1],color_sigmaV[2]);
            actor->GetProperty()->SetOpacity(OpacityD);
            rotation->RotateZ(angle+45.0);
            rotation->RotateY(90.0);

            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());
            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            mapper->Delete();
        }
    }


    //---------------------------------------------------------------------
    //render three circles SigmaH
    auto polygonSource =  vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource->SetNumberOfSides(1000);
    polygonSource->SetRadius(verticalLength/2.0);

    auto circle =  vtkSmartPointer<vtkRegularPolygonSource>::New();
    circle->SetNumberOfSides(1000);
    circle->SetRadius(verticalLength/2.0);
    circle->GeneratePolygonOff();

    //if(_isSigmaHVisible && typeSigmaH==2)
    if(typeSigmaH==2)
    {
        for (int k=0; k<3; k++)
        {
            if(list_sigmaH_visible[k]<1) continue;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityH);
            actor->GetProperty()->SetColor(color_sigmaH);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();

            if (k==1)
                rotation->RotateX(90.0);
            if (k>1)
                rotation->RotateY(90.0);

            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(color_sigmaH);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }
    }

    //six Sigma-D
    //render three circle SigmaD
    polygonSource->SetNumberOfSides(1000);
    polygonSource->SetRadius(HorizontalLengh/2.0);

    circle->SetNumberOfSides(1000);
    circle->SetRadius(HorizontalLengh/2.0);
    circle->GeneratePolygonOff();

    //if(_isSigmaVVisible && typeSigmaV==2)
    if(typeSigmaV==2)
        for (int k=0; k<2; k++)
        {
            if (list_sigmaD_visible[k]<1) continue;

            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityV);
            actor->GetProperty()->SetColor(color_sigmaV);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(1.0,1.414,1.0);
            rotation->RotateY(90);
            rotation->RotateZ(angle+45.0);
            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(color_sigmaV);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }

    if(typeSigmaD==2)
    //if( _isSigmaDVisible  && typeSigmaD==2)
        for (int k=0; k<2; k++)
        {
            if (list_sigmaD_visible[k+2]<1) continue;
            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityV);
            actor->GetProperty()->SetColor(brown);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(1.414,1.0,1.0);
            rotation->RotateY(angle+45.0);
            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(brown);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }

//    if( _isSigmaDVisible && typeSigmaD==2)
    if(typeSigmaD==2)
        for (int k=0; k<2; k++)
        {
            if (list_sigmaD_visible[k+4]<1) continue;
            double angle=k*90;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityD);
            actor->GetProperty()->SetColor(color_sigmaD);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->Scale(1.0,1.414,1.0);
            rotation->RotateX(angle+45.0);
            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(color_sigmaD);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }
}



void View3D::renderIh( )
{
    renderI();

    if(_isCiVisible)
    {
        renderCenter(0.1*discCn_scale,yellow);

        if(_isCnVisible && _isC5_Visible)
        {
            auto sphere = vtkSphereSource::New();
            sphere->SetPhiResolution(20);
            sphere->SetThetaResolution(20);
            sphere->Update();
            sphere->SetRadius(0.05*discCn_scale);
            sphere->SetCenter(0.0,0.0,Cn_Length);

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->GetProperty()->SetColor(yellow);
            actor->SetMapper(mapper);
            actor->SetPickable(false);
            mapper->SetInputConnection(sphere->GetOutputPort());
            m_renderer->AddActor(actor);
        }
    }




    auto cube = vtkSmartPointer<vtkCubeSource>::New();
    cube->SetXLength(thicknessV);
    cube->SetYLength(HorizontalLengh);
    cube->SetZLength(HorizontalLengh);
    cube->Update();


    unsigned int n=5;
    //if(_isSigmaVVisible  &&  typeSigmaV==1)
    if(typeSigmaV==1)
    {
        cube->SetXLength(thicknessV);
        cube->Update();

        for (unsigned int k=0; k<n; k++)
        {
            if(list_sigmaV_visible[k]<1) continue;
            double angle=k*180.0/n;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->GetProperty()->SetColor(color_sigmaV);
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityV);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->RotateX(90.0);
            rotation->RotateZ(angle+90.0);
            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);
            mapper->Delete();
        }
    }


    //vector3 C3(cos(36.0/180*3.142)*sin(37.377368142265787/180*3.142),
    //           sin(36.0/180*3.142)*sin(37.377368142265787/180*3.142),
    //           cos(37.377368142265787/180*3.142) );

    vector3 C3(0.4911,  0.3569,  0.7946);

    //if(_isSigmaDVisible &&   typeSigmaD==1)
    if(typeSigmaD==1)
    {
        cube->SetXLength(thicknessD);
        cube->Update();
        for (unsigned int k=0; k<3; k++)
        {
            if(list_sigmaD_visible[k]<1) continue;

            double angle=k*180.0/3;
            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->GetProperty()->SetColor(color_sigmaD);
            //actor->storeColor(color_sigmaV[0],color_sigmaV[1],color_sigmaV[2]);
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityD);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();

            rotation->RotateX(90);
            rotation->RotateZ(36);
            rotation->RotateWXYZ(37.377,-0.5878, 0.8090, 0.0000 );
            rotation->RotateWXYZ(90+angle,C3.x(),C3.y(),C3.z());

            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);
            mapper->Delete();
        }
    }

    //  C5^C2=31.717474413001757
    //  C5^C2=58.28252556502243
    //  C5^C3=37.377368142265787
    //  C5^C5=63.43494882292201
    //  C3^C2=???
    //  C5={0,0,1};C5={sin63.434948822922,0.0,cos63.434948822922}
    //  C3={cos36.0*sin37.377368142265787,sin36.0*sin37.377368142265787,cos37.377368142265787}
    //  C2={sin31.717474413001757,0.0,cos31.717474413001757}

    vector3 C2(0.5258, 0.0, 0.8506);
    //if(_isSigmaHVisible &&   typeSigmaH==1)
    if(typeSigmaH==1)
    {
        cube->SetYLength(verticalLength);
        cube->SetZLength(verticalLength);
        cube->SetXLength(thicknessH);
        cube->Update();

        for (unsigned int k=0; k<2; k++)
        {
            if(list_sigmaH_visible[k]<1) continue;
            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->GetProperty()->SetColor(color_sigmaH);
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityH);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->RotateWXYZ(31.717,0,1,0);
            rotation->RotateWXYZ(90.*k,C2.x(),C2.y(),C2.z());
            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(cube->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);
            mapper->Delete();
        }
    }





    //render circle-plane
    auto polygonSource =  vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource->SetNumberOfSides(1000);
    polygonSource->SetRadius(verticalLength/2.0);

    auto circle =  vtkSmartPointer<vtkRegularPolygonSource>::New();
    circle->SetNumberOfSides(1000);
    circle->SetRadius(verticalLength/2.0);
    circle->GeneratePolygonOff();

    //if(_isSigmaHVisible && typeSigmaH==2)
    if(typeSigmaH==2)
    {
        for (int k=0; k<2; k++)
        {
            if(list_sigmaH_visible[k]<1) continue;
            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityH);
            actor->GetProperty()->SetColor(color_sigmaH);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();

            rotation->RotateY(90);
            rotation->RotateWXYZ(31.717,0,1,0);
            rotation->RotateWXYZ(90.*k,C2.x(),C2.y(),C2.z());

            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(color_sigmaH);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }
    }


    polygonSource->SetRadius(HorizontalLengh/2.0);
    polygonSource->Update();
    circle->SetRadius(HorizontalLengh/2.0);
    circle->Update();

    //if(_isSigmaVVisible  &&  typeSigmaV==2)
    if(typeSigmaV==2)
    {
        for (unsigned int k=0; k<5; k++)
        {
            if(list_sigmaV_visible[k]<1) continue;
            double angle=k*36.0;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->GetProperty()->SetColor(color_sigmaV);
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityV);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->RotateX(90);
            rotation->RotateZ(angle);
            actor->SetUserTransform(rotation);

            actor->SetUserTransform(rotation);
            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(color_sigmaV);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }
    }


    //  C5^C2=31.717474413001757
    //  C5^C2=58.28252556502243
    //  C5^C3=37.377368142265787
    //  C5^C5=63.43494882292201
    //  C3^C2=???
    //  C5={0,0,1};C5={sin63.434948822922,0.0,cos63.434948822922}
    //  C3={cos36.0*sin37.377368142265787,sin36.0*sin37.377368142265787,cos37.377368142265787}
    //  C2={sin31.717474413001757,0.0,cos31.717474413001757}
    //vector3 C3(0.4911,  0.3569,  0.7946);

    //  vector3 vv(0,1,0);
    //matrix3x3 m;
    // m.rotateZAngle(180/5.0);
    //cout << m*vv<<endl;

    //if(_isSigmaDVisible  &&  typeSigmaD==2)
    if(typeSigmaD==2)
    {
        for (unsigned int k=0; k<3; k++)
        {
            if(list_sigmaD_visible[k]<1) continue;
            double angle=k*60.0;

            auto actor = vtkActor::New();
            auto mapper = vtkPolyDataMapper::New();
            actor->GetProperty()->SetColor(color_sigmaD);
            actor->SetMapper(mapper);
            actor->GetProperty()->SetOpacity(OpacityD);
            actor->GetProperty()->ShadingOn();
            actor->SetPickable(false);

            auto rotation=  vtkSmartPointer<vtkTransform>::New();
            rotation->PostMultiply();
            rotation->RotateY(90);
            rotation->RotateZ(36);
            rotation->RotateWXYZ(37.377,-0.5878, 0.8090, 0.0000 );
            rotation->RotateWXYZ(90+angle,C3.x(),C3.y(),C3.z());
            actor->SetUserTransform(rotation);

            mapper->ScalarVisibilityOn();
            mapper->SetInputConnection(polygonSource->GetOutputPort());

            actor->GetProperty()->SetInterpolationToGouraud();
            m_renderer->AddActor(actor);

            auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputConnection(circle->GetOutputPort());;
            auto actor1 =  vtkSmartPointer<vtkActor>::New();
            actor1->SetPickable(false);

            actor1->GetProperty()->SetColor(color_sigmaD);
            actor1->GetProperty()->ShadingOff();
            actor1->SetUserTransform(rotation);
            actor1->SetMapper(mapper1);
            m_renderer->AddActor(actor1);
        }
    }
}




//----------------------------------------------------------------
void View3D::setVisibleAll_PG()
{
    _isCiVisible=true;
    _isCnVisible=true;

    _isC2_Visible=_isC3_Visible=_isC4_Visible=_isC5_Visible=_isC6_Visible=isI2n=true;
    _isSigmaHVisible=_isSigmaVVisible=_isSigmaDVisible=true;

    setC2_Visible(1);
    setAllC2_1_Visible(1);
    setAllC2_2_Visible(1);

    setAllSigma_Visible(1);
    setAllSigmaV_Visible(1);
    setAllSigmaD_Visible(1);
    setAllSigmaH_Visible(1);

    setAllS4_Visible(1);
    setAllC5_Visible(1);
    setAllC4_Visible(1);
    setAllC3_Visible(1);
    setAllC2_Visible(1);

    refresh();
}
void View3D::setInvisibleAll_PG()
{
    _isCnVisible=false;
    _isCiVisible=false;
    _isC2_Visible=_isC3_Visible=_isC4_Visible=_isC5_Visible=_isC6_Visible=isI2n=false;

    _isSigmaHVisible=_isSigmaVVisible=_isSigmaDVisible=false;

    //cout <<"setInvisibleAll_PG"<<endl;
    //cout <<list_C2_Horizontal_visible.size()<<endl;

    setC2_Visible(0);
    setAllC2_1_Visible(0);
    setAllC2_2_Visible(0);

    setAllSigma_Visible(0);
    setAllSigmaV_Visible(0);
    setAllSigmaD_Visible(0);
    setAllSigmaH_Visible(0);

    setAllS4_Visible(0);
    setAllC5_Visible(0);
    setAllC4_Visible(0);
    setAllC3_Visible(0);
    setAllC2_Visible(0);
    refresh();
}

bool View3D::isOneOfC2_Visible()
{
    cout << "View3D::isOneOfC2_Visible() " <<list_C2_visible.size()<<endl;

    uint t=0;
    for(uint i : list_C2_visible)
        t=t+i;

    if(t>0) return true;
    return false;
}

bool View3D::isOneOfC2_1_Visible()
{
    //cout << "View3D::isOneOfC2_1_Visible() " <<list_C2_1_Horizontal_visible.size()<<endl;
    //cout << "View3D::list_C2_visible() " <<list_C2_visible.size()<<endl;

    //list_C2_Horizontal_visible

   // cout << list_C2_visible[0]<<endl;

    uint t=0;
    for(uint i : list_C2_1_Horizontal_visible)
        t=t+i;


    for(uint i : list_C2_Horizontal_visible)
        t=t+i;

    if(t>0) return true;
    return false;
}

bool View3D::isOneOfC2_2_Visible()
{
    uint t=0;
    for(uint i : list_C2_2_Horizontal_visible)
        t=t+i;

    if(t>0) return true;
    return false;
}



void View3D::setC2_1_Visible(uint idx, uint b)
{
    list_C2_1_Horizontal_visible[idx]=b;
}

void View3D::setC2_2_Visible(uint idx, uint b)
{
    list_C2_2_Horizontal_visible[idx]=b;
}

void View3D::setC2_Visible(uint idx, uint b)
{
    if(list_C2_Horizontal_visible.size()>0)
        list_C2_Horizontal_visible[idx]=b;

    if(list_C2_visible.size()>0)
        list_C2_visible[idx]=b;
}


void View3D::setAllC2_1_Visible(uint b)
{
    for(uint i=0;i<list_C2_1_Horizontal_visible.size();i++ )
        list_C2_1_Horizontal_visible[i]=b;
}
void View3D::setAllC2_2_Visible( uint b)
{
    for(uint i=0;i<list_C2_2_Horizontal_visible.size();i++ )
        list_C2_2_Horizontal_visible[i]=b;
}
void View3D::setAllC2_Visible(uint b)
{
    for(uint i=0;i<list_C2_visible.size();i++ )
        list_C2_visible[i]=b;

    for(uint i=0;i<list_C2_Horizontal_visible.size();i++ )
        list_C2_Horizontal_visible[i]=b;

}


void View3D::setSigmaV_Visible(uint idx, uint b)
{
    if(idx>=list_sigmaV_visible.size()) return;
    list_sigmaV_visible[idx]=b;
}


void View3D::setSigmaD_Visible(uint idx, uint b)
{
    if(idx>=list_sigmaD_visible.size()) return;

    list_sigmaD_visible[idx]=b;
}

void View3D::setSigmaH_Visible(uint idx, uint b)
{
    if(idx>=list_sigmaH_visible.size()) return;
    list_sigmaH_visible[idx]=b;
}

void View3D::setSigma_Visible(uint idx, uint b)
{
    if(idx>=list_sigma_visible.size()) return;
    list_sigma_visible[idx]=b;
}



void View3D::setAllSigmaV_Visible(uint b)
{
    for(uint i=0;i<list_sigmaV_visible.size();i++ )
        list_sigmaV_visible[i]=b;
}

void View3D::setAllSigma_Visible(uint b)
{
    for(uint i=0;i<list_sigmaV_visible.size();i++ )
        list_sigmaV_visible[i]=b;
}

void View3D::setAllSigmaD_Visible(uint b)
{
    for(uint i=0;i<list_sigmaD_visible.size();i++ )
        list_sigmaD_visible[i]=b;
}

void View3D::setAllSigmaH_Visible(uint b)
{
    for(uint i=0;i<list_sigmaH_visible.size();i++ )
        list_sigmaH_visible[i]=b;
}




void View3D::setAllS4_Visible(uint b)
{
    for(uint i=0;i<list_S4_visible.size();i++ )
        list_S4_visible[i]=b;
}

void View3D::setAllC3_Visible(uint b)
{
    for(uint i=0;i<list_C3_visible.size();i++ )
        list_C3_visible[i]=b;
}

void View3D::setAllC4_Visible(uint b)
{
    for(uint i=0;i<list_C4_visible.size();i++ )
        list_C4_visible[i]=b;
}

void View3D::setAllC5_Visible(uint b)
{
    for(uint i=0;i<list_C5_visible.size();i++ )
        list_C5_visible[i]=b;
}


void View3D::setC3_Visible(uint idx, uint b)
{
    list_C3_visible[idx]=b;
}

void View3D::setC4_Visible(uint idx, uint b)
{
    list_C4_visible[idx]=b;
}

void View3D::setS4_Visible(uint idx, uint b)
{
    list_S4_visible[idx]=b;
}

void View3D::setC5_Visible(uint idx, uint b)
{
    list_C5_visible[idx]=b;
}

void View3D::initSigma_list(uint n)
{
    list_sigma_visible.clear();
    list_sigma_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_sigma_visible[i]=1;
}

void View3D::initSigmaH_list(uint n)
{
    list_sigmaH_visible.clear();
    list_sigmaH_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_sigmaH_visible[i]=1;
}


void View3D::initSigmaV_list(uint n)
{
    list_sigmaV_visible.clear();
    list_sigmaV_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_sigmaV_visible[i]=1;

    // cout << list_sigmaV_visible.size()<< "  View3D::initSigmaV_list(uint n) " <<endl;
}

void View3D::initSigmaD_list(uint n)
{
    list_sigmaD_visible.clear();
    list_sigmaD_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_sigmaD_visible[i]=1;
}

void View3D::initC2_Horizontal_list(uint n)
{
    list_C2_Horizontal_visible.clear();
    list_C2_Horizontal_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_C2_Horizontal_visible[i]=1;

    //cout <<"View3D::initC2_Horizontal_list(uint n)"<<endl;
    //cout << list_C2_Horizontal_visible.size()<<endl;
}

void View3D::initC2_1_Horizaontal_list(uint n)
{
    list_C2_1_Horizontal_visible.clear();
    list_C2_1_Horizontal_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_C2_1_Horizontal_visible[i]=1;
}


void View3D::initC2_2_Horizaontal_list(uint n)
{
    list_C2_2_Horizontal_visible.clear();
    list_C2_2_Horizontal_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_C2_2_Horizontal_visible[i]=1;
}

void View3D::initC2_list(uint n)
{
    list_C2_visible.clear();
    list_C2_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_C2_visible[i]=1;
}

void View3D::initC3_list(uint n) {
    list_C3_visible.clear();
    list_C3_visible.resize(n);
    for(uint i=0; i<n;i++)
        list_C3_visible[i]=1;
}

void View3D::initC4_list(uint n)
{
    list_C4_visible.clear();
    list_C4_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_C4_visible[i]=1;
}

void View3D::initC5_list(uint n)
{
    list_C5_visible.clear();
    list_C5_visible.resize(n);

    for(uint i=0; i<n;i++)
        list_C5_visible[i]=1;
}

void View3D::initS4_list(uint n)
{
    list_S4_visible.clear();
    list_S4_visible.resize(n);
    for(uint i=0; i<n;i++)
        list_S4_visible[i]=1;
}
void View3D::initIdxFrame()
{
    idxFrame=0;
}


void View3D::startMovie_rotate()
{
   nFrames=mainWindow->getMovieFrames();
   mol_movie->rotate_movie(rotateVector_movie,rotateAngle_movie);
   refresh();

   if(idxFrame==nFrames-1 && !mainWindow->isLoopPlaying())
   {
       treeWidget->stopTimer();
       refresh();
       return;
   }

   if(idxFrame==nFrames)
   {
       idxFrame=0;
       refresh();
       sleep(1);
       //usleep(1000);
   }

   idxFrame++;
}

void View3D::genReflectionAtoms()
{
    mol_movie->genReflectionAtoms(reflectionVector_movie);
}

void View3D::genRotationReflectionAtoms(vector3 _axis, double _angle)
{
    mol_movie->genRotationReflectionAtoms(_axis,_angle);
}
void View3D::genRotationReversionAtoms(vector3 _axis, double _angle)
{
    mol_movie->genRotationReversionAtoms(_axis,_angle);
}


void View3D::genReverseAtoms()
{
    mol_movie->genReverseAtoms();
}

void View3D::clearMovieData()
{
    if(!mol_movie) return;
    mol_movie->clearReflectionAtoms();
    mol_movie->clearReverseAtoms();
    mol_movie->clearNormalAtoms();
}

void View3D::startMovie_reflect()
{
    nFrames=mainWindow->getMovieFrames();

    mol_movie->reflect_movie(reflectionVector_movie,nFrames,idxFrame);

    refresh();


    if(idxFrame==nFrames && !mainWindow->isLoopPlaying())
    {
        treeWidget->stopTimer();
        refresh();
        return;
    }

    if(idxFrame==nFrames+1)
    {
        idxFrame=0;
        refresh();
        sleep(1);
        //usleep(1000);
    }

    idxFrame++;
}


void View3D::startMovie_reverse()
{
    nFrames=mainWindow->getMovieFrames();

    mol_movie->reverse_movie(nFrames,idxFrame);

    refresh();


    if(idxFrame==nFrames && !mainWindow->isLoopPlaying())
    {
        treeWidget->stopTimer();
        refresh();
        return;
    }

    if(idxFrame==nFrames+1)
    {
        idxFrame=0;
        refresh();
        sleep(1);
    }

    idxFrame++;
}


void View3D::startMovie_rotate_reflect()
{
    nFrames=mainWindow->getMovieFrames();

    if(idxFrame==2*nFrames-1 && !mainWindow->isLoopPlaying())
    {
        treeWidget->stopTimer();
        refresh();
        return;
    }


    if(idxFrame<nFrames)
        mol_movie->rotate_movie(rotateVector_movie,rotateAngle_movie);
    else
        mol_movie->reflect_movie(rotateVector_movie,nFrames,idxFrame-nFrames);

    refresh();

    if(idxFrame==2*nFrames+1)
    {
        idxFrame=0;
        refresh();
        sleep(1);
    }

    idxFrame++;
}


void View3D::startMovie_rotate_reverse()
{
    nFrames=mainWindow->getMovieFrames();

    if(idxFrame==2*nFrames-1 && !mainWindow->isLoopPlaying())
    {
        treeWidget->stopTimer();
        refresh();
        return;
    }


    if(idxFrame<nFrames)
        mol_movie->rotate_movie(rotateVector_movie,rotateAngle_movie);
    else
        mol_movie->reverse_movie(nFrames,idxFrame-nFrames);

    refresh();

    if(idxFrame==2*nFrames+1)
    {
        idxFrame=0;
        refresh();
        sleep(1);
    }

    idxFrame++;
}


void View3D::setOperatorTree (OperatorTree * tw )
{
    treeWidget = tw;
}


void View3D::copyMol()
{
    if(mol_movie==nullptr)
        mol_movie=new HMol();

    if(mol_movie->NumAtoms()>0)
        mol_movie->clearAll();

    //atoms
    for(uint i=0;i<mol->NumAtoms();i++)
    {
        mol_movie->copyAtom(mol->getAtom(i)->atomicNum(),
                            mol->getAtom(i)->Symbol(),
                            mol->getAtom(i)->cartesian(),
                            mol->getAtom(i)->radius(),
                            mol->getAtom(i)->Mass(),
                            mol->getAtom(i)->Color());
    }


    //bonds
    for(uint i=0;i<mol->NumBonds();i++)
    {
        mol_movie->copyBond(mol->getBondbyIndex(i)->atomIndex0(),
                            mol->getBondbyIndex(i)->atomIndex1(),
                            mol->getBondbyIndex(i)->getBondOrder(),
                            mol->getBondbyIndex(i)->getRingId(),
                            mol->getBondbyIndex(i)->getPlane());
    }




    for(uint i=0;i<mol->numRings();i++)
    {
        mol_movie->copyRing(mol->getRingbyId(i)->atomIdList,
                            mol->getRingbyId(i)->norm,
                            mol->getRingbyId(i)->center,
                            mol->getRingbyId(i)->radius,
                            mol->getRingbyId(i)->type,
                            mol->getRingbyId(i)->color,
                            mol->getRingbyId(i)->scale,
                            mol->getRingbyId(i)->isAromatic
                            );
    }


    mol_movie->genNormalAtoms();

    cout << mol_movie->NumAtoms() <<endl;
    cout << mol_movie->NumBonds() <<endl;
    cout << mol_movie->numRings() <<endl;
    cout <<"void View3D::copyMol() done"<<endl;
}



void View3D::renderAtoms_movie()
{
    if(mol_movie->NumAtoms()<1) return;

    double x,y,z;
    vector3 color;

    for (unsigned int i=0;i<mol_movie->NumAtoms();i++)
    {
        if (!isHydrogen_Visible)
            if(mol_movie->getAtomSymbol(i)=="H") continue;

        x = mol_movie->getAtomXbyIndex(i);
        y = mol_movie->getAtomYbyIndex(i);
        z = mol_movie->getAtomZbyIndex(i);

        auto sphere = vtkSphereSource::New();
        sphere->SetPhiResolution(20);
        sphere->SetThetaResolution(20);

        double radius=scale_atom*mol_movie->getAtomRadiusbyIndex(i);
        sphere->SetRadius(radius*scale_movie);
        sphere->Update();
        color = MakeGlColor(mol_movie->atomList[i]->Color());

        auto actor = vtkActor::New();
        initializeActor(actor);
        actor->SetPosition(x,y,z);
        auto mapper = vtkPolyDataMapper::New();
        actor->SetPickable(false);

        actor->GetProperty()->SetColor(color.x(),color.y(),color.z());
        actor->GetProperty()->SetOpacity(opacity_movie);

        actor->SetMapper(mapper);

        mapper->SetInputConnection(sphere->GetOutputPort());

        m_renderer->AddActor(actor);

        mapper->Delete();
        sphere->Delete();

        if(i<mol_movie->NumAtoms()-1) continue;
    }
}

void View3D::renderBonds_movie()
{
    for (unsigned int j=0;j<mol_movie->NumBonds();j++)
    {
        unsigned int bo=mol_movie->bondList[j]->getBondOrder();

        if (!isHydrogen_Visible)   {
            if(mol_movie->getAtomSymbol(mol_movie->bondList[j]->atomIndex0())=="H") continue;
            if(mol_movie->getAtomSymbol(mol_movie->bondList[j]->atomIndex1())=="H") continue;
        }


        switch (bo)
        {
        case 1: renderSingleBond_movie(j);
            break;

        case 2: renderDoubleBond_movie(j);
            break;

        case 3: renderTripleBond_movie(j);
            break;

        case 4: renderQuadrupleBond_movie(j);
            break;


        case 5: renderHydrogenBond_movie(j);
            break;


        case 6: renderAromaticBond_movie(j);
            break;

        case 7: renderWeakBond_movie(j);
            break;
        }
    }
}

void View3D::renderRings_movie()
{
    //cout <<"void View3D::renderRings_movie() "<<endl;
    renderAromaticRings_movie();
}



void View3D::renderSingleBond_movie(unsigned int idx)
{
    vector3 color;
    HBond * bond= mol_movie->bondList[idx];
    unsigned int idx1=bond->atomIndex0();
    unsigned int idx2=bond->atomIndex1();

    vector3 p0=mol_movie->getAtomPosbyIndex(idx1);
    vector3 p1=mol_movie->getAtomPosbyIndex(idx2);

    double s=0.5;

    if(mol_movie->getAtombyIndex(idx1)->atomicNum() != mol_movie->getAtombyIndex(idx2)->atomicNum())
    {
        double length=(p1-p0).length();
        double r1=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx1)->radius();
        double r2=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx2)->radius();
        double d=0.5*(length-r2-r1);
        s=(d+r1)/length;
    }

    vector3 p=p0+s*(p1-p0);


    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(p0[0], p0[1], p0[2]);
    line->SetPoint2(p[0], p[1], p[2]);

    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(bondRadius*scale_movie);
    tubeFilter->SetNumberOfSides(10);

    color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
    auto actor = vtkActor::New();  initializeActor(actor);
    actor->SetPickable(false);
    auto mapper = vtkPolyDataMapper::New();

    actor->GetProperty()->SetColor(color.x(),color.y(),color.z());
    actor->GetProperty()->SetOpacity(opacity_movie);


    actor->SetMapper(mapper);
    mapper->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(actor);


    auto line1 = vtkSmartPointer<vtkLineSource>::New();
    line1->SetPoint1(p[0], p[1], p[2]);
    line1->SetPoint2(p1[0], p1[1], p1[2]);

    auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter1->SetInputConnection(line1->GetOutputPort());
    tubeFilter1->SetRadius(bondRadius*scale_movie);
    tubeFilter1->SetNumberOfSides(10);

    auto actor1 = vtkActor::New();initializeActor(actor1);
    actor1->SetPickable(false);
    auto mapper1 = vtkPolyDataMapper::New();
    color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());
    actor1->GetProperty()->SetColor(color.x(),color.y(),color.z());

    actor1->SetMapper(mapper1);


    actor1->GetProperty()->SetColor(color.x(),color.y(),color.z());
    actor1->GetProperty()->SetOpacity(opacity_movie);


    mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
    m_renderer->AddActor(actor1);

    mapper->Delete();
    mapper1->Delete();
}



void View3D::renderDoubleBond_movie(unsigned int idx)
{
    HBond * bond= mol_movie->bondList[idx];

    vector3 color;

    unsigned int idx1=bond->atomIndex0();
    unsigned int idx2=bond->atomIndex1();

    vector3 v0=mol_movie->getAtomPosbyIndex(idx1);
    vector3 v1=mol_movie->getAtomPosbyIndex(idx2);

    double s=0.5;


    if (bond->isInsideRing())
    {
        //render single bond
        if (!_isKekuleStyle)
        {
            if(mol_movie->getAtombyIndex(idx1)->atomicNum() != mol_movie->getAtombyIndex(idx2)->atomicNum())
            {
                double length=(v1-v0).length();
                double r1=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx1)->radius();
                double r2=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx2)->radius();
                double d=0.5*(length-r2-r1);
                s=(d+r1)/length;
            }

            vector3 p=v0+s*(v1-v0);


            auto line = vtkSmartPointer<vtkLineSource>::New();
            line->SetPoint1(v0[0], v0[1], v0[2]);
            line->SetPoint2(p[0], p[1], p[2]);

            auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
            tubeFilter->SetInputConnection(line->GetOutputPort());
            tubeFilter->SetRadius(bondRadius*scale_movie);
            tubeFilter->SetNumberOfSides(10);

            color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
            auto actor = vtkActor::New();  initializeActor(actor);
            actor->SetPickable(false);
            auto mapper = vtkPolyDataMapper::New();

            actor->GetProperty()->SetColor(color.x(),color.y(),color.z());
            actor->GetProperty()->SetOpacity(opacity_movie);


            actor->SetMapper(mapper);
            mapper->SetInputConnection(tubeFilter->GetOutputPort());
            m_renderer->AddActor(actor);


            auto line1 = vtkSmartPointer<vtkLineSource>::New();
            line1->SetPoint1(p[0], p[1], p[2]);
            line1->SetPoint2(v1[0], v1[1], v1[2]);

            auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
            tubeFilter1->SetInputConnection(line1->GetOutputPort());
            tubeFilter1->SetRadius(bondRadius*scale_movie);
            tubeFilter1->SetNumberOfSides(10);

            auto actor1 = vtkActor::New();initializeActor(actor1);
            actor1->SetPickable(false);
            auto mapper1 = vtkPolyDataMapper::New();
            color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());
            actor1->GetProperty()->SetColor(color.x(),color.y(),color.z());
            actor1->SetMapper(mapper1);
            actor1->GetProperty()->SetColor(color.x(),color.y(),color.z());
            actor1->GetProperty()->SetOpacity(opacity_movie);

            mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
            m_renderer->AddActor(actor1);

            mapper->Delete();
            mapper1->Delete();
            return;
        }  
    }






    vector3 v=v1-v0;
    vector3 shift;

    if (bond->getPlane().length()>0.5)
        shift=cross(bond->getPlane(),v).normalize();
    else
        v.createOrthoVector(shift);
    shift*=0.10;


    //different atom: the border lies the middle of two sphere-surface
    if(mol_movie->getAtombyIndex(idx1)->atomicNum() != mol_movie->getAtombyIndex(idx2)->atomicNum())
    {
        double length=v.length();
        double r1=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx1)->radius();
        double r2=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx2)->radius();
        double d=0.5*(length-r2-r1);
        s=(d+r1)/length;
    }

    vector3 mid0=v0+s*v;

    for (unsigned int k=0;k<2;k++)
    {
        vector3 p0=v0+shift,
                p1=v1+shift,
                mid=mid0+shift;

        if(k>0) {//break;
            p0=v0-shift; p1=v1-shift;mid=mid0-shift;}

        auto line = vtkSmartPointer<vtkLineSource>::New();
        line->SetPoint1(p0[0], p0[1], p0[2]);
        line->SetPoint2(mid[0], mid[1], mid[2]);

        auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
        tubeFilter->SetInputConnection(line->GetOutputPort());
        tubeFilter->SetRadius(bondRadius*scale_movie);
        tubeFilter->SetNumberOfSides(10);

        color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
        auto actor = vtkActor::New();initializeActor(actor);
        actor->SetPickable(false);
        auto mapper = vtkPolyDataMapper::New();

        actor->GetProperty()->SetColor(color.x(),color.y(),color.z());
        actor->GetProperty()->SetOpacity(opacity_movie);


        actor->SetMapper(mapper);


        mapper->SetInputConnection(tubeFilter->GetOutputPort());
        m_renderer->AddActor(actor);


        auto line1 = vtkSmartPointer<vtkLineSource>::New();
        line1->SetPoint1(mid[0], mid[1], mid[2]);
        line1->SetPoint2(p1[0], p1[1], p1[2]);

        auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
        tubeFilter1->SetInputConnection(line1->GetOutputPort());
        tubeFilter1->SetRadius(bondRadius*scale_movie);
        tubeFilter1->SetNumberOfSides(10);

        auto actor1 = vtkActor::New();initializeActor(actor1);
        actor1->SetPickable(false);
        auto mapper1 = vtkPolyDataMapper::New();
        color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());


        actor1->GetProperty()->SetColor(color.x(),color.y(),color.z());
        actor1->GetProperty()->SetOpacity(opacity_movie);

        actor1->SetMapper(mapper1);

        mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
        m_renderer->AddActor(actor1);

        mapper->Delete();
        mapper1->Delete();
    }
}




void View3D::renderTripleBond_movie(unsigned int idx)
{
    vector3 color;
    HBond * bond= mol_movie->bondList[idx];
    unsigned int idx1=bond->atomIndex0();
    unsigned int idx2=bond->atomIndex1();


    vector3 v0=mol_movie->getAtomPosbyIndex(idx1);
    vector3 v1=mol_movie->getAtomPosbyIndex(idx2);

    vector3 v=v1-v0;
    vector3 shift;
    v.createOrthoVector(shift);
    shift=0.1*shift;
    if (bond->getPlane().length()>0.5)
        shift=0.1*cross1(bond->getPlane(),v1-v0);
    v1=v1-shift; v0=v0-shift;


    //the same atom
    double s=0.5;

    //different atom, the border of two half-bonds lies the middle of two sphere-surface
    if(mol->getAtombyIndex(idx1)->atomicNum() != mol->getAtombyIndex(idx2)->atomicNum())
    {
        double length=(v1-v0).length();
        double r1=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx1)->radius();
        double r2=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx2)->radius();
        double d=0.5*(length-r2-r1);
        s=(d+r1)/length;
    }


    for (unsigned int k=0;k<3;k++)
    {
        vector3 p0=v0+shift*k,  p1=v1+shift*k;
        vector3 p=p0+s*(p1-p0);

        auto line = vtkSmartPointer<vtkLineSource>::New();
        line->SetPoint1(p0[0], p0[1], p0[2]);
        line->SetPoint2(p[0], p[1], p[2]);

        auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
        tubeFilter->SetInputConnection(line->GetOutputPort());
        tubeFilter->SetRadius(bondRadius*scale_movie);
        tubeFilter->SetNumberOfSides(10);

        color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
        auto actor = vtkActor::New();
        initializeActor(actor);
        actor->SetPickable(false);
        auto mapper = vtkPolyDataMapper::New();


        actor->GetProperty()->SetColor(color.x(),color.y(),color.z());
        actor->GetProperty()->SetOpacity(opacity_movie);
        actor->SetMapper(mapper);


        mapper->SetInputConnection(tubeFilter->GetOutputPort());
        m_renderer->AddActor(actor);


        auto line1 = vtkSmartPointer<vtkLineSource>::New();
        line1->SetPoint1(p[0], p[1], p[2]);
        line1->SetPoint2(p1[0], p1[1], p1[2]);

        auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
        tubeFilter1->SetInputConnection(line1->GetOutputPort());
        tubeFilter1->SetRadius(bondRadius*scale_movie);
        tubeFilter1->SetNumberOfSides(10);

        auto actor1 = vtkActor::New();initializeActor(actor1);
        actor1->SetPickable(false);
        auto mapper1 = vtkPolyDataMapper::New();
        color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());

        actor1->GetProperty()->SetColor(color.x(),color.y(),color.z());
        actor1->GetProperty()->SetOpacity(opacity_movie);
        actor1->SetMapper(mapper1);

        mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
        m_renderer->AddActor(actor1);

        mapper->Delete();
        mapper1->Delete();
    }
}



void View3D::renderQuadrupleBond_movie(unsigned int idx)
{
    vector3 color;
    HBond * bond= mol_movie->bondList[idx];
    unsigned int idx1=bond->atomIndex0();
    unsigned int idx2=bond->atomIndex1();


    vector3 v0=mol_movie->getAtomPosbyIndex(idx1);
    vector3 v1=mol_movie->getAtomPosbyIndex(idx2);
    vector3 v=v1-v0;


    vector3  shift,shiftv;

    if (bond->getPlane().length()>0.5)
    {
        shift=0.12*bond->getPlane();
        shiftv=0.12*cross1(bond->getPlane(),v);
    }
    else {
        v.createOrthoVector(shift);
        shiftv=0.12*cross1(v,shift);
        shift=0.12*shift;
    }


    unsigned int beg=numActors();

    //the same atom
    double s=0.5;

    //different atom, the border of two half-bonds lies the middle of two sphere-surface
    //(sphere1)--*--(sphere2)
    if(mol_movie->getAtombyIndex(idx1)->atomicNum() != mol_movie->getAtombyIndex(idx2)->atomicNum())
    {
        double length=(v1-v0).length();
        double r1=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx1)->radius();
        double r2=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx2)->radius();
        double d=0.5*(length-r2-r1);
        s=(d+r1)/length;
    }

    vector3 mid0=v0+s*(v1-v0);



    v0=v0+(shift+shiftv)/2.0;
    v1=v1+(shift+shiftv)/2.0;
    mid0=mid0+(shift+shiftv)/2.0;


    for (int k=0;k<2;k++)
    {
        for (int l=0;l<2;l++)
        {
            vector3 p0=v0+shift*(k-1)+shiftv*(l-1),
                    p1=v1+shift*(k-1)+shiftv*(l-1),
                    mid=mid0+shift*(k-1)+shiftv*(l-1);

            auto line = vtkSmartPointer<vtkLineSource>::New();
            line->SetPoint1(p0[0], p0[1], p0[2]);
            line->SetPoint2(mid[0], mid[1], mid[2]);

            auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
            tubeFilter->SetInputConnection(line->GetOutputPort());
            tubeFilter->SetRadius(bondRadius*scale_movie);
            tubeFilter->SetNumberOfSides(10);

            color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
            auto actor = vtkActor::New();initializeActor(actor);
            actor->SetPickable(false);
            auto mapper = vtkPolyDataMapper::New();


            actor->GetProperty()->SetColor(color.x(),color.y(),color.z());
            actor->GetProperty()->SetOpacity(opacity_movie);
            actor->SetMapper(mapper);



            mapper->SetInputConnection(tubeFilter->GetOutputPort());
            m_renderer->AddActor(actor);


            auto line1 = vtkSmartPointer<vtkLineSource>::New();
            line1->SetPoint1(mid[0], mid[1], mid[2]);
            line1->SetPoint2(p1[0], p1[1], p1[2]);

            auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
            tubeFilter1->SetInputConnection(line1->GetOutputPort());
            tubeFilter1->SetRadius(bondRadius*scale_movie);
            tubeFilter1->SetNumberOfSides(10);

            auto actor1 = vtkActor::New();initializeActor(actor1);
            actor1->SetPickable(false);

            auto mapper1 = vtkPolyDataMapper::New();
            color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());

            actor1->GetProperty()->SetColor(color.x(),color.y(),color.z());
            actor1->GetProperty()->SetOpacity(opacity_movie);
            actor1->SetMapper(mapper1);

            actor1->setBondId(idx);
            actor1->setAtomId(idx2);
            actor1->setAtomId1(idx1);
            actor->setId(beg);
            actor->setBondOrder(4);

            mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
            m_renderer->AddActor(actor1);

            mapper->Delete();
            mapper1->Delete();
        }
    }

}

void View3D::renderHydrogenBond_movie(unsigned int idx)
{
    vector3 color;
    HBond * bond= mol_movie->bondList[idx];
    unsigned int idx1=bond->atomIndex0();
    unsigned int idx2=bond->atomIndex1();


    vector3 v1=mol_movie->getAtomPosbyIndex(idx1);
    vector3 v2=mol_movie->getAtomPosbyIndex(idx2);
    vector3 v=v2-v1;

    unsigned int NSteps=10;

    //the same atom
    double s=0.5;


    //different atom, the border of two half-bonds lies the middle of two sphere-surface
    if(mol_movie->getAtombyIndex(idx1)->atomicNum() != mol_movie->getAtombyIndex(idx2)->atomicNum())
    {
        double length=v.length();
        double r1=scale_bond*mol_movie->getAtombyIndex(idx1)->radius();
        double r2=scale_bond*mol_movie->getAtombyIndex(idx2)->radius();
        double d=0.5*(length-r2-r1);
        s=(d+r1)/length;
    }

    vector3 mid0=v1+s*v;;

    vector3 p0=v1,
            p1=v2,
            mid=mid0;

    //dash line ----------------------------------------;
    double mlength=(v2-v1).length()/NSteps;
    vector3 step=mlength*(p1-p0).normalize();

    p0=p0+step/2.0;
    p1=p1+step/2.0;


    color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
    for (unsigned int k=0;k<NSteps-1;k++)
    {
        vector3 pp0=p0+k*step,
                pp1=p0+(k+1)*step;

        if(k<1) continue;

        if(k%2==0) continue;

        if(k>(NSteps-1)/2)
            color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());


        unsigned int times=2;
        while (times--)
        {
            auto sphere = vtkSphereSource::New();
            sphere->SetPhiResolution(20);
            sphere->SetThetaResolution(20);
            sphere->SetRadius(bondRadius*scale_movie);
            sphere->Update();

            auto a1 = vtkActor::New();
            initializeActor(a1);
            a1->SetPickable(false);

            if(times==1)
                a1->SetPosition(pp0[0], pp0[1], pp0[2]);
            else
                a1->SetPosition(pp1[0], pp1[1], pp1[2]);


            a1->GetProperty()->SetColor(blue);

            auto m1 = vtkPolyDataMapper::New();
            a1->SetMapper(m1);
            m1->SetInputConnection(sphere->GetOutputPort());
            m_renderer->AddActor(a1);

            m1->Delete();
            sphere->Delete();
        }
    }

}


void View3D::renderWeakBond_movie(unsigned int idx)
{
    vector3 color;
    HBond * bond= mol_movie->bondList[idx];
    unsigned int idx1=bond->atomIndex0();
    unsigned int idx2=bond->atomIndex1();


    vector3 v1=mol_movie->getAtomPosbyIndex(idx1);
    vector3 v2=mol_movie->getAtomPosbyIndex(idx2);
    vector3 v=v2-v1;


    unsigned int NSteps=10;

    //the same atom
    double s=0.5;


    //different atom, the border of two half-bonds lies the middle of two sphere-surface
    if(mol_movie->getAtombyIndex(idx1)->atomicNum() != mol_movie->getAtombyIndex(idx2)->atomicNum())
    {
        double length=v.length();
        double r1=scale_movie*scale_bond*mol_movie->getAtombyIndex(idx1)->radius();
        double r2=scale_movie*scale_bond*mol_movie->getAtombyIndex(idx2)->radius();
        double d=0.5*(length-r2-r1);
        s=(d+r1)/length;
    }

    vector3 mid0=v1+s*v;;

    vector3 p0=v1,
            p1=v2,
            mid=mid0;

    //dash line ----------------------------------------;
    double mlength=(v2-v1).length()/NSteps;
    vector3 step=mlength*(p1-p0).normalize();

    p0=p0+step/2.0;
    p1=p1+step/2.0;


    color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
    for (unsigned int k=0;k<NSteps-1;k++)
    {
        vector3 pp0=p0+k*step,
                pp1=p0+(k+1)*step;

        if(k<1) continue;

        if(k%2==0) continue;

        if(k>(NSteps-1)/2)
            color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());

        auto line = vtkSmartPointer<vtkLineSource>::New();
        line->SetPoint1(pp0[0], pp0[1], pp0[2]);
        line->SetPoint2(pp1[0], pp1[1], pp1[2]);
        auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
        tubeFilter->SetInputConnection(line->GetOutputPort());
        tubeFilter->SetRadius(bondRadius*scale_movie);
        tubeFilter->SetNumberOfSides(10);
        tubeFilter->SetCapping(true);

        auto a = vtkActor::New();
        initializeActor(a);
        a->SetPickable(false);
        auto m = vtkPolyDataMapper::New();

        a->GetProperty()->SetColor(color.x(),color.y(),color.z());
        a->GetProperty()->SetOpacity(opacity_movie);
        a->SetMapper(m);


        m->SetInputConnection(tubeFilter->GetOutputPort());
        m_renderer->AddActor(a);
        m->Delete();


        unsigned int times=2;
        while (times--)
        {
            auto sphere = vtkSphereSource::New();
            sphere->SetPhiResolution(20);
            sphere->SetThetaResolution(20);
            sphere->SetRadius(bondRadius*0.6);
            sphere->Update();


            auto a1 = vtkActor::New();
            a1->SetPickable(false);
            initializeActor(a1);


            a1->storeColor(color.x(),color.y(),color.z());

            //cout << " times " << times << endl;

            if(times==1)
                a1->SetPosition(pp0[0], pp0[1], pp0[2]);
            else
                a1->SetPosition(pp1[0], pp1[1], pp1[2]);


            a1->GetProperty()->SetColor(color.x(),color.y(),color.z());
            a1->GetProperty()->SetOpacity(opacity_movie);


            auto m1 = vtkPolyDataMapper::New();
            a1->SetMapper(m1);
            m1->SetInputConnection(sphere->GetOutputPort());
            m_renderer->AddActor(a1);

            m1->Delete();
            sphere->Delete();
        }
    }

}


void View3D::renderAromaticBond_movie(unsigned int idx)
{
    HBond * bond= mol_movie->bondList[idx];
    if( bond->isInsideRing())
    {
        renderAromaticBond_insideRings_movie(idx);
        return;
    }

    vector3 color;
    unsigned int idx1=bond->atomIndex0();
    unsigned int idx2=bond->atomIndex1();


    vector3 v1=mol_movie->getAtomPosbyIndex(idx1);
    vector3 v2=mol_movie->getAtomPosbyIndex(idx2);
    vector3 v=v2-v1;

    unsigned int beg=numActors();
    unsigned int NSteps=6;

    //the same atom
    double s=0.5;

    //different atom, the border of two half-bonds lies the middle of two sphere-surface
    if(mol_movie->getAtombyIndex(idx1)->atomicNum() != mol_movie->getAtombyIndex(idx2)->atomicNum())
    {
        double length=v.length();
        double r1=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx1)->radius();
        double r2=scale_movie*scale_atom*mol_movie->getAtombyIndex(idx2)->radius();
        double d=0.5*(length-r2-r1);
        s=(d+r1)/length;
    }

    vector3 mid0=v1+s*v;
    vector3 shift,shift0;


    if (bond->getPlane().length()>0.5) {
        shift0=0.1*cross1(bond->getPlane(),v);
    }
    else {
        v.createOrthoVector(shift);
        shift0=0.1*shift;
    }


    vector3 p0=v1,   p1=v2,   mid=mid0;

    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(p0[0], p0[1], p0[2]);
    line->SetPoint2(mid[0], mid[1], mid[2]);

    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(bondRadius);
    tubeFilter->SetNumberOfSides(20);

    color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
    auto actor = vtkActor::New();initializeActor(actor);
    actor->SetPickable(false);
    auto mapper = vtkPolyDataMapper::New();
    actor->GetProperty()->SetColor(color.x(),color.y(),color.z());

    actor->storeColor(color.x(),color.y(),color.z());
    actor->GetProperty()->SetOpacity(opacity_movie);
    actor->SetMapper(mapper);


    mapper->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(actor);


    auto line1 = vtkSmartPointer<vtkLineSource>::New();
    line1->SetPoint1(mid[0], mid[1], mid[2]);
    line1->SetPoint2(p1[0], p1[1], p1[2]);

    auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter1->SetInputConnection(line1->GetOutputPort());
    tubeFilter1->SetRadius(bondRadius);
    tubeFilter1->SetNumberOfSides(20);

    auto actor1 = vtkActor::New();
    initializeActor(actor1);
    actor1->SetPickable(false);
    auto mapper1 = vtkPolyDataMapper::New();
    color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());
    actor1->GetProperty()->SetColor(color.x(),color.y(),color.z());

    actor1->storeColor(color.x(),color.y(),color.z());
    actor1->GetProperty()->SetOpacity(opacity_movie);
    actor1->SetMapper(mapper1);


    mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
    m_renderer->AddActor(actor1);

    mapper->Delete();
    mapper1->Delete();


    //dash line ----------------------------------------
    p0=v1+2.0*shift0;  p1=v2+2.0*shift0;
    double mlength=(v2-v1).length()/NSteps;
    vector3 step=mlength*(p1-p0).normalize();

    p0=p0+step/2.0;
    p1=p1+step/2.0;

    //rotate standard capsule(010) to target one
    vector3 pp=(p1-p0).normalize();
    vector3 _normal=VZ;
    double _ang=0.0;
    if( (!isEqual(pp,VY)) || (!isNegativeEqual(pp,VY))) {
        _normal=cross(VY,pp);
        _ang=vectorAngle(VY,pp);
    }


    color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
    vector3 pp0,pp1,cc;
    for (unsigned int k=0;k<NSteps-1;k++)
    {
        if(k<1) continue;
        if(k%2==0) continue;

        pp0=p0+k*step;
        pp1=p0+(k+1)*step;
        cc=(pp0+pp1)*0.5;

        if(k>(NSteps-1)/2)
            color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());

        auto capsule = vtkSmartPointer<vtkCapsuleSource>::New();
        capsule->SetRadius(bondRadius*0.6);
        capsule->SetThetaResolution (10);
        capsule->SetPhiResolution(10);
        capsule->SetCylinderLength((pp1-pp0).length());

        auto a = vtkActor::New();
        initializeActor(a);
        auto m = vtkPolyDataMapper::New();


        a->GetProperty()->SetColor(color.x(),color.y(),color.z());
        a->GetProperty()->SetOpacity(opacity_movie);

        a->storeColor(color.x(),color.y(),color.z());
        a->SetMapper(m);

        a->setBondId(idx);
        a->setId(beg);
        a->setAtomId(idx1);
        a->setAtomId1(idx2);
        a->setBondOrder(6);

        if(abs(_ang)>0.01) {
            auto transform=  vtkSmartPointer<vtkTransform>::New();
            transform->PostMultiply();
            transform->RotateWXYZ(_ang,_normal.x(),_normal.y(),_normal.z());
            transform->Translate(cc.x(),cc.y(),cc.z());
            a->SetUserTransform(transform);
        }

        m->SetInputConnection(capsule->GetOutputPort());
        m_renderer->AddActor(a);
        m->Delete();
    }
}


void View3D::renderAromaticBond_insideRings_movie(unsigned int idx)
{
    vector3 color;
    HBond * bond= mol_movie->bondList[idx];
    unsigned int idx1=bond->atomIndex0();
    unsigned int idx2=bond->atomIndex1();


    vector3 v1=mol_movie->getAtomPosbyIndex(idx1);
    vector3 v2=mol_movie->getAtomPosbyIndex(idx2);
    vector3 v=v2-v1;
    vector3 midp=0.5*(v2+v1);

    unsigned int NSteps=6;
    double _length=v.length()/NSteps;


    //the same atom
    double s=0.5;

    //different atom, the border of two half-bonds lies the middle of two sphere-surface
    if(mol_movie->getAtombyIndex(idx1)->atomicNum() != mol_movie->getAtombyIndex(idx2)->atomicNum())
    {
        double length=v.length();
        double r1=scale_atom*mol_movie->getAtombyIndex(idx1)->radius();
        double r2=scale_atom*mol_movie->getAtombyIndex(idx2)->radius();
        double d=0.5*(length-r2-r1);
        s=(d+r1)/length;
    }

    vector3 mid0=v1+s*v;


    vector3 p0=v1,   p1=v2,   mid=mid0;

    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(p0[0], p0[1], p0[2]);
    line->SetPoint2(mid[0], mid[1], mid[2]);

    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(bondRadius);
    tubeFilter->SetNumberOfSides(20);

    color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
    auto actor = vtkActor::New();
    initializeActor(actor);
    actor->SetPickable(false);
    auto mapper = vtkPolyDataMapper::New();

    actor->GetProperty()->SetColor(color.x(),color.y(),color.z());

    actor->storeColor(color.x(),color.y(),color.z());
    actor->SetMapper(mapper);

    mapper->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(actor);

    auto line1 = vtkSmartPointer<vtkLineSource>::New();
    line1->SetPoint1(mid[0], mid[1], mid[2]);
    line1->SetPoint2(p1[0], p1[1], p1[2]);

    auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter1->SetInputConnection(line1->GetOutputPort());
    tubeFilter1->SetRadius(bondRadius);
    tubeFilter1->SetNumberOfSides(20);

    auto actor1 = vtkActor::New();
    initializeActor(actor1);
    actor->SetPickable(false);

    auto mapper1 = vtkPolyDataMapper::New();
    color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());
    actor1->GetProperty()->SetColor(color.x(),color.y(),color.z());

    actor1->storeColor(color.x(),color.y(),color.z());
    actor1->SetMapper(mapper1);


    mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
    m_renderer->AddActor(actor1);

    mapper->Delete();
    mapper1->Delete();


    if(!_isKekuleStyle) return;

    HRing  * ring=nullptr ;
    vector3 v0,step;
    vector3 normal,_normal;
    vector3 pp0,pp1,cc;


    //rotate standard capsule(010) to target one
    vector3 pp=v.normalize();
    _normal=VZ;
    double _ang=0.0;
    if( (!isEqual(pp,VY)) || (!isNegativeEqual(pp,VY))) {
        _normal=cross(VY,pp);
        _ang=vectorAngle(VY,pp);
    }


    //dash line inside ring
    vector3 shift,shift0;
    for (unsigned int i=0;i<bond->sizeofRingId();i++)
    {
        ring = mol_movie->getRingbyId(bond->getRingId(i));
        normal=ring->norm;

        //method I
        shift=cross(normal, v);
        shift0=0.12*shift.normalize();


        //if outside, reverse
        if( dot(ring->center-midp,shift)<0.0)
            shift0=-shift0;

        p0=v1+2.0*shift0;
        p1=v2+2.0*shift0;


        step=_length*(p1-p0).normalize();

        p0=p0+step/2.0;
        p1=p1+step/2.0;

        color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex0())->Color());
        for (unsigned int k=0;k<NSteps-1;k++)
        {
            if(k<1) continue;
            if(k%2==0) continue;

            pp0=p0+k*step;
            pp1=p0+(k+1)*step;
            cc=(pp0+pp1)*0.5;

            if(k>(NSteps-1)/2)
                color = MakeGlColor(mol_movie->getAtombyIndex(bond->atomIndex1())->Color());

            auto capsule = vtkSmartPointer<vtkCapsuleSource>::New();
            capsule->SetRadius(bondRadius*0.6);
            capsule->SetThetaResolution (10);
            capsule->SetPhiResolution(10);
            capsule->SetCylinderLength((pp1-pp0).length());

            auto a = vtkActor::New();
            initializeActor(a);
            a->SetPickable(false);
            auto m = vtkPolyDataMapper::New();


            a->GetProperty()->SetColor(color.x(),color.y(),color.z());

            a->storeColor(color.x(),color.y(),color.z());
            a->SetMapper(m);


            if(abs(_ang)>0.01) {
                auto transform=vtkSmartPointer<vtkTransform>::New();
                transform->PostMultiply();
                transform->RotateWXYZ(_ang,_normal.x(),_normal.y(),_normal.z());
                transform->Translate(cc.x(),cc.y(),cc.z());
                a->SetUserTransform(transform);
            }

            m->SetInputConnection(capsule->GetOutputPort());
            m_renderer->AddActor(a);
            m->Delete();
        }
    }
}

//render all rings of the molecule
void View3D::renderAromaticRings_movie()
{
    if(_isKekuleStyle) return;

    if (mol_movie->numRings()<1)  return;

    //cout << "Number of ring " << mol->numRings()<<endl;

    for (unsigned int i=0;i<mol_movie->numRings();i++)
    {
        HRing  * ring = mol_movie->getRingbyId(i);

        auto parametricObject = vtkSmartPointer<vtkParametricTorus>::New();
        auto parametricFunctionSource =	vtkSmartPointer<vtkParametricFunctionSource>::New();
        parametricFunctionSource->SetParametricFunction(parametricObject);

        parametricObject->SetRingRadius(ring->radius-0.5);
        if(ring->radius<1.0)
            parametricObject->SetRingRadius(ring->radius-0.6);
        parametricObject->SetCrossSectionRadius(scale_movie*bondRadius*scale_ringRadius*ring->scale);

        parametricFunctionSource->SetUResolution(50);
        parametricFunctionSource->SetVResolution(50);
        parametricFunctionSource->SetWResolution(50);

        parametricFunctionSource->Update();

        auto mapper =   vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(parametricFunctionSource->GetOutputPort());
        mapper->ScalarVisibilityOn();

        matrix3x3 m;
        m.SetupRotateMatrix(VZ,ring->norm);

        auto transform=  vtkSmartPointer<vtkTransform>::New();
        transform->PostMultiply();
        if( !isEqual (VZ,ring->norm) && !isNegativeEqual (VZ,ring->norm) )
        {
            auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
            for( int i = 0; i < 3; i++ )
                for( int j = 0; j < 3; j++ )
                    matrix->SetElement(i,j,m(i,j));

            for( int i = 0; i < 4; i++ ) {
                matrix->SetElement(3,i,0.0);
                matrix->SetElement(i,3,0.0);
            }
            matrix->SetElement(3,3,1.0);

            transform->SetMatrix(matrix);
        }
        transform->Translate(ring->center.x(),ring->center.y(),ring->center.z());


        auto actor = vtkSmartPointer<vtkActor>::New();
        initializeActor(actor);
        actor->SetMapper(mapper);
        actor->SetPickable(false);

        actor->GetProperty()->SetColor(ring->color.x(),ring->color.y(),ring->color.z());
        actor->GetProperty()->SetOpacity(opacity_movie);
        actor->SetUserTransform(transform);

        m_renderer->AddActor(actor);
    }
}



void View3D::renderSimpleSingleBond_movie(unsigned int idx)
{
    vector3 color={0.6,0.6,0.6};
    HBond * bond= mol_movie->bondList[idx];
    unsigned int idx1=bond->atomIndex0();
    unsigned int idx2=bond->atomIndex1();

    vector3 p0=mol_movie->getAtomPosbyIndex(idx1);
    vector3 p1=mol_movie->getAtomPosbyIndex(idx2);


    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(p0[0], p0[1], p0[2]);
    line->SetPoint2(p1[0], p1[1], p1[2]);

    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(scale_movie*bondRadius);
    tubeFilter->SetNumberOfSides(10);

    //color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
    auto actor = vtkActor::New();
    initializeActor(actor);
    actor->SetPickable(false);
    auto mapper = vtkPolyDataMapper::New();

    vector3 greyColor(0.6,0.6,0.6);
    actor->GetProperty()->SetColor(greyColor.x(),greyColor.y(),greyColor.z());
    actor->storeColor(color.x(),color.y(),color.z());
    actor->setBondOrder(1);

    actor->GetProperty()->SetDiffuse(0.9);
    actor->GetProperty()->SetAmbient(0.3);
    actor->GetProperty()->SetSpecular(0.2);
    actor->GetProperty()->SetSpecularPower(6.0);

    actor->GetProperty()->SetColor(color.x(),color.y(),color.z());
    actor->GetProperty()->SetOpacity(opacity_movie);
    actor->SetMapper(mapper);


    mapper->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(actor);
    mapper->Delete();
}


void View3D::setReflectionVector_movie(vector3 v)
{
    reflectionVector_movie=v;
}
