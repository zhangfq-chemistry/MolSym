#include "templateView3d.h"

#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>

// #include <QVTKOpenGLWidget.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkCoordinate.h>
#include <vtkCornerAnnotation.h>
#include <vtkDataSet.h>
#include <vtkFollower.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricTorus.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextSource.h>
#include <vtkTubeFilter.h>

#include "InteractorT.h"
#include "mTools.h"
#include "mainwindow.h"
#include "templateMol.h"
#include <QFile>
#include <QMessageBox>

void TemplateView3d::initializeActor(vtkActor *actor) {
  if (actor == nullptr)
    return;

  actor->setId(-1);
  actor->setBondOrder(-1);
  actor->setRingId(-1);
  actor->setMolId(-1);
  actor->setAtomId(-1);
  actor->setAtomId1(-1);
  actor->setBondId(-1);
  // actor->setSelected(false);
}

TemplateView3d::TemplateView3d(QWidget *parent)
    : QVTKOpenGLNativeWidget(parent) {
  this->setWindowTitle("Template");

  vtkNew<vtkGenericOpenGLRenderWindow> window;
  setRenderWindow(window.Get());

  // background color
  bkcolor.Set(0.4, 0.5, 0.5);

  // Camera
  camera = vtkSmartPointer<vtkCamera>::New();
  camera->SetViewUp(0, 1, 0);
  camera->SetPosition(0, 0, 30);
  camera->SetFocalPoint(0, 0, 0);
  // camera->ParallelProjectionOn();

  // Renderer
  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  m_renderer->SetActiveCamera(camera);
  m_renderer->SetBackground(bkcolor.x(), bkcolor.y(), bkcolor.z());
  renderWindow()->AddRenderer(m_renderer);

  interactor = actorInteractorStyleT::New();
  interactor->SetDefaultRenderer(m_renderer);
  interactor->setView3dT(this);
  QVTKOpenGLNativeWidget::interactor()->SetInteractorStyle(interactor);

  selectedColor = buildColor(Qt::green);
  bondRadius = 0.06;
  selectAtomId = 0;
  _isAtomLabelVisible = false;

  scale = 0.3;
  isfirstTime = true;
}

void TemplateView3d::updateView() { renderWindow()->Render(); }

void TemplateView3d::clearAll() {
  vtkActorCollection *actorCollection = m_renderer->GetActors();
  actorCollection->InitTraversal();
  unsigned int numActors = actorCollection->GetNumberOfItems();

  for (int i = 0; i < numActors; ++i) {
    vtkActor *actor = actorCollection->GetNextActor();
    if (actor != nullptr)
      m_renderer->RemoveActor(actor);
  }

  // updateView();
}

unsigned int TemplateView3d::numActors() {
  vtkActorCollection *actorCollection = m_renderer->GetActors();
  actorCollection->InitTraversal();
  return actorCollection->GetNumberOfItems();
}

void TemplateView3d::setMainWindow(MainWindow *m) { mainWindow = m; }

void TemplateView3d::setMolT(templateMol *m) {
  tmol = m;
  isfirstTime = true;
}

void TemplateView3d::renderMol() {
  clearAll();
  if (tmol->numAtoms() < 1)
    return;
  renderAtoms();
  renderBonds();
  renderSelectedAtom();
  updateView();
}

bool TemplateView3d::isHydrogenAtom(uint idx) {
  if (idx >= tmol->numAtoms())
    return false;
  if (tmol->getAtomicNum(idx) == 1)
    return true;
  return false;
}

void TemplateView3d::renderBonds() {
  unsigned int bo = 0;
  for (unsigned int j = 0; j < tmol->numBonds(); j++) {
    bo = tmol->bondList[j]->getBondOrder();

    switch (bo) {
    case 1:
      renderSingleBond(j);
      break;
    case 2:
      renderDoubleBond(j);
      break;
    case 3:
      renderTripleBond(j);
      break;
    case 4:
      renderQuadrupleBond(j);
      break;
    case 6:
      renderAromaticBond(j);
      break;
    case 7:
      renderWeakBond(j);
      break;
    }
  }
  renderAromaticRings();
}

void TemplateView3d::renderWeakBond(unsigned int idx) {}

void TemplateView3d::renderTripleBond(unsigned int idx) {
  vector3 color;
  HBond *bond = tmol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  vector3 v0 = tmol->getAtomPosbyIndex(idx1);
  vector3 v1 = tmol->getAtomPosbyIndex(idx2);

  vector3 v = v1 - v0;
  vector3 shift;
  v.createOrthoVector(shift);
  shift = 0.1 * shift;
  if (bond->getPlane().length() > 0.5)
    shift = 0.1 * cross(bond->getPlane(), v1 - v0);
  v1 = v1 - shift;
  v0 = v0 - shift;

  unsigned int beg = numActors();
  double s = 0.5;

  // different atoms
  if (tmol->getAtombyIndex(idx1)->atomicNum() !=
      tmol->getAtombyIndex(idx2)->atomicNum()) {
    double length = v.length();
    double r1 = scale * tmol->getAtombyIndex(idx1)->radius();
    double r2 = scale * tmol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;

    for (unsigned int k = 0; k < 3; k++) {
      vector3 p0 = v0 + shift * k, p1 = v1 + shift * k;
      vector3 p = p0 + s * (p1 - p0);

      auto line = vtkSmartPointer<vtkLineSource>::New();
      line->SetPoint1(p0[0], p0[1], p0[2]);
      line->SetPoint2(p[0], p[1], p[2]);

      auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
      tubeFilter->SetInputConnection(line->GetOutputPort());
      tubeFilter->SetRadius(bondRadius * 0.8);
      tubeFilter->SetNumberOfSides(50);

      color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
      auto actor = vtkActor::New();
      auto mapper = vtkPolyDataMapper::New();
      actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
      actor->storeColor(color.x(), color.y(), color.z());
      actor->SetMapper(mapper);

      actor->SetPickable(false);
      actor->setId(beg + 1);
      actor->setAtomId(idx1);
      actor->setAtomId1(idx2);
      actor->setAtomLabelId(-1);

      mapper->SetInputConnection(tubeFilter->GetOutputPort());
      m_renderer->AddActor(actor);

      auto line1 = vtkSmartPointer<vtkLineSource>::New();
      line1->SetPoint1(p[0], p[1], p[2]);
      line1->SetPoint2(p1[0], p1[1], p1[2]);

      auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
      tubeFilter1->SetInputConnection(line1->GetOutputPort());
      tubeFilter1->SetRadius(bondRadius * 0.8);
      tubeFilter1->SetNumberOfSides(50);

      auto actor1 = vtkActor::New();
      auto mapper1 = vtkPolyDataMapper::New();
      color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex1())->Color());
      actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());
      actor1->storeColor(color.x(), color.y(), color.z());
      actor1->SetMapper(mapper1);

      actor1->SetPickable(false);
      actor1->setAtomId(idx2);
      actor1->setAtomId1(idx1);
      actor1->setId(beg);
      actor1->setAtomLabelId(-1);

      mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
      m_renderer->AddActor(actor1);

      mapper->Delete();
      mapper1->Delete();
    }
  }

  // the same atoms
  else {
    for (unsigned int k = 0; k < 3; k++) {
      vector3 p0 = v0 + shift * k, p1 = v1 + shift * k;

      auto line = vtkSmartPointer<vtkLineSource>::New();
      line->SetPoint1(p0[0], p0[1], p0[2]);
      line->SetPoint2(p1[0], p1[1], p1[2]);
      auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
      tubeFilter->SetInputConnection(line->GetOutputPort());
      tubeFilter->SetRadius(bondRadius * 0.9);
      tubeFilter->SetNumberOfSides(50);

      color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
      auto actor = vtkActor::New();
      auto mapper = vtkPolyDataMapper::New();
      actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
      actor->storeColor(color.x(), color.y(), color.z());
      actor->SetMapper(mapper);

      actor->SetPickable(false);

      actor->setId(beg); // setPairedId(beg)
      actor->setAtomId(idx1);
      actor->setAtomId1(idx2);
      actor->setAtomLabelId(-1);

      mapper->SetInputConnection(tubeFilter->GetOutputPort());
      m_renderer->AddActor(actor);
      mapper->Delete();
    }
  }
}

void TemplateView3d::renderAromaticRings() {
  if (_isKekuleStyle)
    return;

  if (tmol->numRings() < 1)
    return;
  vector3 color(0.5, 0.5, 0.5);
  for (unsigned int i = 0; i < tmol->numRings(); i++) {
    HRing *ring = tmol->getRingbyId(i);
    if (!ring->isAromatic)
      continue;
    auto parametricObject = vtkSmartPointer<vtkParametricTorus>::New();
    auto parametricFunctionSource =
        vtkSmartPointer<vtkParametricFunctionSource>::New();
    parametricFunctionSource->SetParametricFunction(parametricObject);
    parametricFunctionSource->Update();
    parametricObject->SetRingRadius(ring->radius - 0.5);
    parametricObject->SetCrossSectionRadius(0.06);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(parametricFunctionSource->GetOutputPort());
    mapper->ScalarVisibilityOn();

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->SetPickable(false);
    actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
    actor->setAtomLabelId(-1);
    actor->SetPosition(ring->center.x(), ring->center.y(), ring->center.z());
    m_renderer->AddActor(actor);

    // mapper->Delete();
  }
}

void TemplateView3d::renderAromaticBond(unsigned int idx) {
  HBond *bond = tmol->bondList[idx];
  if (bond->isInsideRing()) {
    renderAromaticBond_insideRings(idx);
    return;
  }

  vector3 color;
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  vector3 v1 = tmol->getAtomPosbyIndex(idx1);
  vector3 v2 = tmol->getAtomPosbyIndex(idx2);
  vector3 v = v2 - v1;

  unsigned int beg = numActors();
  unsigned int NSteps = 6;

  // the same atom
  double s = 0.5;

  // different atom, the border of two half-bonds lies the middle of two
  // sphere-surface
  if (tmol->getAtombyIndex(idx1)->atomicNum() !=
      tmol->getAtombyIndex(idx2)->atomicNum()) {
    double length = v.length();
    double r1 = scale * tmol->getAtombyIndex(idx1)->radius();
    double r2 = scale * tmol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;
  }

  vector3 mid0 = v1 + s * v;

  vector3 shift, shift0;

  if (bond->getPlane().length() > 0.5) {
    shift0 = 0.1 * cross1(bond->getPlane(), v);
  } else {
    v.createOrthoVector(shift);
    shift0 = 0.1 * shift;
  }

  vector3 p0 = v1, p1 = v2, mid = mid0;

  auto line = vtkSmartPointer<vtkLineSource>::New();
  line->SetPoint1(p0[0], p0[1], p0[2]);
  line->SetPoint2(mid[0], mid[1], mid[2]);

  auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
  tubeFilter->SetInputConnection(line->GetOutputPort());
  tubeFilter->SetRadius(bondRadius);
  tubeFilter->SetNumberOfSides(20);

  color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
  auto actor = vtkActor::New();
  initializeActor(actor);
  auto mapper = vtkPolyDataMapper::New();

  actor->GetProperty()->SetColor(color.x(), color.y(), color.z());

  actor->storeColor(color.x(), color.y(), color.z());
  actor->SetMapper(mapper);

  actor->setId(beg);
  actor->setAtomId(idx1);
  actor->setAtomId1(idx2);
  actor->setBondId(idx);
  actor->setBondOrder(6);

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
  auto mapper1 = vtkPolyDataMapper::New();
  color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex1())->Color());

  actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());

  actor1->storeColor(color.x(), color.y(), color.z());
  actor1->SetMapper(mapper1);

  actor1->setBondId(idx);
  actor1->setAtomId(idx2);
  actor1->setAtomId1(idx1);
  actor1->setId(beg);
  actor1->setBondOrder(6);

  mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
  m_renderer->AddActor(actor1);

  mapper->Delete();
  mapper1->Delete();

  // dash line ----------------------------------------
  p0 = v1 + 2.0 * shift0;
  p1 = v2 + 2.0 * shift0;
  double mlength = (v2 - v1).length() / NSteps;
  vector3 step = mlength * (p1 - p0).normalize();

  p0 = p0 + step / 2.0;
  p1 = p1 + step / 2.0;

  // rotate standard capsule(010) to target one
  vector3 pp = (p1 - p0).normalize();
  vector3 _normal = VZ;
  double _ang = 0.0;
  if ((!isEqual(pp, VY)) || (!isNegativeEqual(pp, VY))) {
    _normal = cross(VY, pp);
    _ang = vectorAngle(VY, pp);
  }

  color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
  vector3 pp0, pp1, cc;
  for (unsigned int k = 0; k < NSteps - 1; k++) {
    if (k < 1)
      continue;
    if (k % 2 == 0)
      continue;

    pp0 = p0 + k * step;
    pp1 = p0 + (k + 1) * step;
    cc = (pp0 + pp1) * 0.5;

    if (k > (NSteps - 1) / 2)
      color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex1())->Color());

    auto capsule = vtkSmartPointer<vtkCapsuleSource>::New();
    capsule->SetRadius(bondRadius * 0.6);
    capsule->SetThetaResolution(10);
    capsule->SetPhiResolution(10);
    capsule->SetCylinderLength((pp1 - pp0).length());

    auto a = vtkActor::New();
    initializeActor(a);
    auto m = vtkPolyDataMapper::New();

    a->GetProperty()->SetColor(color.x(), color.y(), color.z());

    a->storeColor(color.x(), color.y(), color.z());
    a->SetMapper(m);

    a->setBondId(idx);
    a->setId(beg);
    a->setAtomId(idx1);
    a->setAtomId1(idx2);
    a->setBondOrder(6);

    if (abs(_ang) > 0.01) {
      auto transform = vtkSmartPointer<vtkTransform>::New();
      transform->PostMultiply();
      transform->RotateWXYZ(_ang, _normal.x(), _normal.y(), _normal.z());
      transform->Translate(cc.x(), cc.y(), cc.z());
      a->SetUserTransform(transform);
    }

    m->SetInputConnection(capsule->GetOutputPort());
    m_renderer->AddActor(a);
    m->Delete();
  }
}

void TemplateView3d::renderAromaticBond_insideRings(unsigned int idx) {
  vector3 color;
  HBond *bond = tmol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  vector3 v1 = tmol->getAtomPosbyIndex(idx1);
  vector3 v2 = tmol->getAtomPosbyIndex(idx2);
  vector3 v = v2 - v1;
  vector3 midp = 0.5 * (v2 + v1);

  unsigned int NSteps = 6;
  double _length = v.length() / NSteps;

  unsigned int beg = numActors();

  // the same atom
  double s = 0.5;

  // different atom, the border of two half-bonds lies the middle of two
  // sphere-surface
  if (tmol->getAtombyIndex(idx1)->atomicNum() !=
      tmol->getAtombyIndex(idx2)->atomicNum()) {
    double length = v.length();
    double r1 = scale * tmol->getAtombyIndex(idx1)->radius();
    double r2 = scale * tmol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;
  }

  vector3 mid0 = v1 + s * v;

  vector3 p0 = v1, p1 = v2, mid = mid0;

  auto line = vtkSmartPointer<vtkLineSource>::New();
  line->SetPoint1(p0[0], p0[1], p0[2]);
  line->SetPoint2(mid[0], mid[1], mid[2]);

  auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
  tubeFilter->SetInputConnection(line->GetOutputPort());
  tubeFilter->SetRadius(bondRadius);
  tubeFilter->SetNumberOfSides(20);

  color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
  auto actor = vtkActor::New();
  initializeActor(actor);
  auto mapper = vtkPolyDataMapper::New();

  actor->GetProperty()->SetColor(color.x(), color.y(), color.z());

  actor->storeColor(color.x(), color.y(), color.z());
  actor->SetMapper(mapper);

  actor->setId(beg);
  actor->setAtomId(idx1);
  actor->setAtomId1(idx2);
  actor->setBondId(idx);
  actor->setBondOrder(6);

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
  auto mapper1 = vtkPolyDataMapper::New();
  color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex1())->Color());

  actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());

  actor1->storeColor(color.x(), color.y(), color.z());
  actor1->SetMapper(mapper1);

  actor1->setBondId(idx);
  actor1->setAtomId(idx2);
  actor1->setAtomId1(idx1);
  actor1->setId(beg);
  actor1->setBondOrder(6);

  mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
  m_renderer->AddActor(actor1);

  mapper->Delete();
  mapper1->Delete();

  if (!_isKekuleStyle)
    return;

  HRing *ring = nullptr;
  vector3 v0, step;
  vector3 normal, _normal;
  vector3 pp0, pp1, cc;

  // rotate standard capsule(010) to target one
  vector3 pp = v.normalize();
  _normal = VZ;
  double _ang = 0.0;
  if ((!isEqual(pp, VY)) || (!isNegativeEqual(pp, VY))) {
    _normal = cross(VY, pp);
    _ang = vectorAngle(VY, pp);
  }

  // dash line inside ring
  vector3 shift, shift0;
  for (unsigned int i = 0; i < bond->sizeofRingId(); i++) {
    ring = tmol->getRingbyId(bond->getRingId(i));
    normal = ring->norm;

    // method I
    shift = cross(normal, v);
    shift0 = 0.12 * shift.normalize();

    // if outside, reverse
    if (dot(ring->center - midp, shift) < 0.0)
      shift0 = -shift0;

    p0 = v1 + 2.0 * shift0;
    p1 = v2 + 2.0 * shift0;

    step = _length * (p1 - p0).normalize();

    p0 = p0 + step / 2.0;
    p1 = p1 + step / 2.0;

    color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
    for (unsigned int k = 0; k < NSteps - 1; k++) {
      if (k < 1)
        continue;
      if (k % 2 == 0)
        continue;

      pp0 = p0 + k * step;
      pp1 = p0 + (k + 1) * step;
      cc = (pp0 + pp1) * 0.5;

      if (k > (NSteps - 1) / 2)
        color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex1())->Color());

      auto capsule = vtkSmartPointer<vtkCapsuleSource>::New();
      capsule->SetRadius(bondRadius * 0.6);
      capsule->SetThetaResolution(10);
      capsule->SetPhiResolution(10);
      capsule->SetCylinderLength((pp1 - pp0).length());

      auto a = vtkActor::New();
      initializeActor(a);
      auto m = vtkPolyDataMapper::New();

      a->GetProperty()->SetColor(color.x(), color.y(), color.z());

      a->storeColor(color.x(), color.y(), color.z());
      a->SetMapper(m);

      a->setBondId(idx);
      a->setId(beg);
      a->setAtomId(idx1);
      a->setAtomId1(idx2);
      a->setBondOrder(6);

      if (abs(_ang) > 0.01) {
        auto transform = vtkSmartPointer<vtkTransform>::New();
        transform->PostMultiply();
        transform->RotateWXYZ(_ang, _normal.x(), _normal.y(), _normal.z());
        transform->Translate(cc.x(), cc.y(), cc.z());
        a->SetUserTransform(transform);
      }

      m->SetInputConnection(capsule->GetOutputPort());
      m_renderer->AddActor(a);
      m->Delete();
    }
  }
}

void TemplateView3d::renderDoubleBond(unsigned int idx) {
  vector3 color;
  HBond *bond = tmol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  // cout << idx1+1 << " "<< idx2+1<< " inside renderDoubleBond" <<endl;

  vector3 v0 = tmol->getAtomPosbyIndex(idx1);
  vector3 v1 = tmol->getAtomPosbyIndex(idx2);
  vector3 v = v1 - v0;

  vector3 shift = 0.08 * cross(bond->getPlane(), v);

  // cout << bond->getPlane().x();
  // cout << " "<< bond->getPlane().y();
  // cout << " "<< bond->getPlane().z() << " inside
  // TemplateView3d::renderDoubleBond"<<endl;

  unsigned int beg = numActors();
  double s = 0.5;

  // different atoms
  if (tmol->getAtombyIndex(idx1)->atomicNum() !=
      tmol->getAtombyIndex(idx2)->atomicNum()) {
    double length = (v1 - v0).length();
    double r1 = scale * tmol->getAtombyIndex(idx1)->radius();
    double r2 = scale * tmol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;

    vector3 mid0 = v0 + s * v;

    for (unsigned int k = 0; k < 2; k++) {
      vector3 p0 = v0 + shift, p1 = v1 + shift, mid = mid0 + shift;

      if (k > 0) { // break;
        p0 = v0 - shift;
        p1 = v1 - shift;
        mid = mid0 - shift;
      }

      auto line = vtkSmartPointer<vtkLineSource>::New();
      line->SetPoint1(p0[0], p0[1], p0[2]);
      line->SetPoint2(mid[0], mid[1], mid[2]);

      auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
      tubeFilter->SetInputConnection(line->GetOutputPort());
      tubeFilter->SetRadius(bondRadius);
      tubeFilter->SetNumberOfSides(50);

      color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
      auto actor = vtkActor::New();
      auto mapper = vtkPolyDataMapper::New();
      actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
      actor->storeColor(color.x(), color.y(), color.z());
      actor->SetMapper(mapper);

      actor->SetPickable(false);
      actor->setId(beg + 1);
      actor->setAtomId(idx1);
      actor->setAtomId1(idx2);
      actor->setAtomLabelId(-1);

      mapper->SetInputConnection(tubeFilter->GetOutputPort());
      m_renderer->AddActor(actor);

      auto line1 = vtkSmartPointer<vtkLineSource>::New();
      line1->SetPoint1(mid[0], mid[1], mid[2]);
      line1->SetPoint2(p1[0], p1[1], p1[2]);

      auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
      tubeFilter1->SetInputConnection(line1->GetOutputPort());
      tubeFilter1->SetRadius(bondRadius);
      tubeFilter1->SetNumberOfSides(50);

      auto actor1 = vtkActor::New();
      actor1->SetPickable(false);
      auto mapper1 = vtkPolyDataMapper::New();
      color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex1())->Color());
      actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());
      actor1->storeColor(color.x(), color.y(), color.z());
      actor1->SetMapper(mapper1);

      actor1->setAtomLabelId(-1);
      actor1->setAtomId(idx2);
      actor1->setAtomId1(idx1);
      actor->setId(beg);

      mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
      m_renderer->AddActor(actor1);

      mapper->Delete();
      mapper1->Delete();
    }
  }

  // the same atoms
  else {
    for (unsigned int k = 0; k < 2; k++) {

      vector3 p0 = v0 + shift, p1 = v1 + shift;
      if (k > 0) {
        p0 = v0 - shift;
        p1 = v1 - shift;
      }

      auto line = vtkSmartPointer<vtkLineSource>::New();
      line->SetPoint1(p0[0], p0[1], p0[2]);
      line->SetPoint2(p1[0], p1[1], p1[2]);
      auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
      tubeFilter->SetInputConnection(line->GetOutputPort());
      tubeFilter->SetRadius(bondRadius);
      tubeFilter->SetNumberOfSides(50);

      color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
      auto actor = vtkActor::New();
      auto mapper = vtkPolyDataMapper::New();
      actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
      actor->storeColor(color.x(), color.y(), color.z());
      actor->SetMapper(mapper);
      actor->SetPickable(false);

      actor->setAtomLabelId(-1);
      actor->setId(beg); // setPairedId(beg)
      actor->setAtomId(idx1);
      actor->setAtomId1(idx2);

      mapper->SetInputConnection(tubeFilter->GetOutputPort());
      m_renderer->AddActor(actor);
      mapper->Delete();
    }
  }
}

void TemplateView3d::setLinkedMode() {
  if (mainWindow) {
    mainWindow->setLinkedMode();
  }
}

void TemplateView3d::renderSingleBond(unsigned int idx) {
  vector3 color;
  HBond *bond = tmol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  vector3 p0 = tmol->getAtomPosbyIndex(idx1);
  vector3 p1 = tmol->getAtomPosbyIndex(idx2);

  double radius1 = tmol->getAtombyIndex(idx1)->radius(),
         radius2 = tmol->getAtombyIndex(idx2)->radius();

  double bondlength = (p0 - p1).length();

  unsigned int beg = numActors();

  // one pseudo Bq/X atom
  if (tmol->getAtombyIndex(idx1)->atomicNum() < 1 ||
      tmol->getAtombyIndex(idx1)->atomicNum() < 1) {
    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(p0[0], p0[1], p0[2]);
    line->SetPoint2(p1[0], p1[1], p1[2]);

    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(bondRadius);
    tubeFilter->SetNumberOfSides(20);

    color = MakeGlColor(ElementColors[0]);
    auto actor = vtkActor::New();
    auto mapper = vtkPolyDataMapper::New();

    actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
    actor->storeColor(color.x(), color.y(), color.z());
    actor->SetMapper(mapper);
    actor->SetPickable(false);

    actor->setAtomLabelId(-1);
    actor->setId(beg);
    actor->setAtomId(idx1);
    actor->setAtomId1(idx2);

    mapper->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(actor);
    mapper->Delete();
    return;
  }

  if (tmol->getAtombyIndex(idx1)->atomicNum() !=
      tmol->getAtombyIndex(idx2)->atomicNum()) {
    if (radius1 < 0.5 || radius1 < 0.5)
      radius1 = radius2 = bondlength / 2.0;

    double s = 0.5;

    double length = (p1 - p0).length();
    double r1 = scale * radius1;
    double r2 = scale * radius2;
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;

    vector3 p = p0 + s * (p1 - p0);

    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(p0[0], p0[1], p0[2]);
    line->SetPoint2(p[0], p[1], p[2]);

    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(bondRadius);
    tubeFilter->SetNumberOfSides(20);

    color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
    auto actor = vtkActor::New();
    auto mapper = vtkPolyDataMapper::New();
    actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
    actor->storeColor(color.x(), color.y(), color.z());
    actor->SetMapper(mapper);
    actor->SetPickable(false);

    actor->setId(beg + 1);
    actor->setAtomId(idx1);
    actor->setAtomId1(idx2);
    actor->setAtomLabelId(-1);

    mapper->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(actor);

    auto line1 = vtkSmartPointer<vtkLineSource>::New();
    line1->SetPoint1(p[0], p[1], p[2]);
    line1->SetPoint2(p1[0], p1[1], p1[2]);

    auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter1->SetInputConnection(line1->GetOutputPort());
    tubeFilter1->SetRadius(bondRadius);
    tubeFilter1->SetNumberOfSides(20);

    auto actor1 = vtkActor::New();
    actor1->SetPickable(false);
    auto mapper1 = vtkPolyDataMapper::New();
    color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex1())->Color());
    actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());
    actor1->storeColor(color.x(), color.y(), color.z());
    actor1->SetMapper(mapper1);

    actor1->setAtomLabelId(-1);
    actor1->setAtomId(idx2);
    actor1->setAtomId1(idx1);
    actor1->setId(beg);

    mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
    m_renderer->AddActor(actor1);

    mapper->Delete();
    mapper1->Delete();
  } else {
    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(p0[0], p0[1], p0[2]);
    line->SetPoint2(p1[0], p1[1], p1[2]);

    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(bondRadius);
    tubeFilter->SetNumberOfSides(20);

    color = MakeGlColor(tmol->getAtombyIndex(bond->atomIndex0())->Color());
    auto actor = vtkActor::New();
    auto mapper = vtkPolyDataMapper::New();
    actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
    actor->storeColor(color.x(), color.y(), color.z());
    actor->SetMapper(mapper);
    actor->SetPickable(false);

    actor->setAtomLabelId(-1);
    actor->setId(beg);
    actor->setAtomId(idx1);
    actor->setAtomId1(idx2);

    mapper->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(actor);
    mapper->Delete();
  }
}

void TemplateView3d::renderQuadrupleBond(unsigned int) {}

void TemplateView3d::renderSelectedAtom() {
  if (isfirstTime)
    selectAtomId = tmol->getHotIndex();

  double x = tmol->getAtomXbyIndex(selectAtomId);
  double y = tmol->getAtomYbyIndex(selectAtomId);
  double z = tmol->getAtomZbyIndex(selectAtomId);

  auto sphere = vtkSphereSource::New();
  sphere->SetPhiResolution(20);
  sphere->SetThetaResolution(20);

  double radius = 2.0 * scale * tmol->getAtomRadiusbyIndex(selectAtomId);
  sphere->SetRadius(radius);
  sphere->Update();

  auto actor = vtkActor::New();
  actor->SetPosition(x, y, z);
  auto mapper = vtkPolyDataMapper::New();

  actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                 selectedColor.z());

  interactor->m_highlightActor = actor;

  actor->SetMapper(mapper);
  actor->setAtomId(selectAtomId);
  actor->setAtomLabelId(selectAtomId);
  actor->setId(selectAtomId);
  actor->GetProperty()->SetOpacity(0.4);

  mapper->SetInputConnection(sphere->GetOutputPort());
  m_renderer->AddActor(actor);

  mapper->Delete();
  sphere->Delete();
  isfirstTime = false;
}

void TemplateView3d::renderAtoms() {
  if (tmol->numAtoms() < 1)
    return;

  double x, y, z;
  vector3 color;

  double radius;
  for (unsigned int i = 0; i < tmol->numAtoms(); i++) {
    x = tmol->getAtomXbyIndex(i);
    y = tmol->getAtomYbyIndex(i);
    z = tmol->getAtomZbyIndex(i);

    auto sphere = vtkSphereSource::New();
    sphere->SetPhiResolution(10);
    sphere->SetThetaResolution(10);

    radius = tmol->getAtomRadiusbyIndex(i);

    if (radius < 0.05)
      radius = 0.1;
    else
      radius = scale * tmol->getAtomRadiusbyIndex(i);
    sphere->SetRadius(radius);
    sphere->Update();
    color = MakeGlColor(tmol->atomList[i]->Color());

    auto actor = vtkActor::New();
    actor->SetPosition(x, y, z);
    auto mapper = vtkPolyDataMapper::New();
    actor->storeColor(color.x(), color.y(), color.z());
    actor->GetProperty()->SetColor(color.x(), color.y(), color.z());

    // only hydrogen can be selected
    if (tmol->getAtomicNum(i) == 1)
      actor->SetPickable(true);
    else
      actor->SetPickable(false);

    actor->SetMapper(mapper);
    actor->setAtomId(i);
    actor->setId(i);

    mapper->SetInputConnection(sphere->GetOutputPort());
    m_renderer->AddActor(actor);

    mapper->Delete();
    sphere->Delete();

    if (_isAtomLabelVisible) {
      // QString label= mol->getAtombyIndex(i)->Symbol();
      QString label = QString::number(i + 1);

      auto textSource = vtkSmartPointer<vtkTextSource>::New();
      textSource->SetText(label.toStdString().c_str());
      textSource->SetForegroundColor(0.0, 0.0, 1.0);
      textSource->BackingOff();
      textSource->Update();

      auto textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      textMapper->SetInputConnection(textSource->GetOutputPort());

      auto textActor = vtkSmartPointer<vtkFollower>::New();
      textActor->SetPickable(false);
      textActor->setAtomLabelId(-1);
      textActor->SetMapper(textMapper);
      textActor->SetScale(0.02, 0.02, 0.02);

      radius = radius / 2.0;

      textActor->AddPosition(x - radius / 2, y - radius / 2, z + radius * 3.0);

      textActor->SetCamera(camera);
      m_renderer->AddActor(textActor);
    }
  }
}

void TemplateView3d::renderRingCenter() {
  if (tmol->numRings() < 1)
    return;

  for (unsigned int i = 0; i < tmol->numRings(); i++) {
    auto sphere = vtkSphereSource::New();
    sphere->SetPhiResolution(20);
    sphere->SetThetaResolution(20);

    sphere->SetRadius(0.2);
    sphere->Update();

    vector3 color(1.0, 0.0, 0.0);

    auto actor = vtkActor::New();
    actor->SetPosition(tmol->getRingbyId(i)->center.x(),
                       tmol->getRingbyId(i)->center.y(),
                       tmol->getRingbyId(i)->center.z());
    auto mapper = vtkPolyDataMapper::New();
    actor->storeColor(color.x(), color.y(), color.z());

    actor->SetMapper(mapper);
    actor->setAtomId(i);
    actor->setAtomLabelId(-1);
    actor->setId(i);

    mapper->SetInputConnection(sphere->GetOutputPort());
    m_renderer->AddActor(actor);

    mapper->Delete();
    sphere->Delete();
  }
}

void TemplateView3d::loadTemplateMol(QString path) {
  tmol->LoadFile(path);
  renderMol();
}

void TemplateView3d::loadMetalTemplateMol(QString path) {

  tmol->LoadFile_MetalTemplate(path);
  renderMol();
}

// just remove label-actor
void TemplateView3d::removeHighlightLabel() {
  vtkActorCollection *actorCollection = m_renderer->GetActors();
  actorCollection->InitTraversal();
  unsigned int _numActors = actorCollection->GetNumberOfItems();

  for (int i = 0; i < _numActors; i++) {
    vtkActor *actor = actorCollection->GetNextActor();
    if (actor == nullptr || actor->getAtomLabelId() < 0 ||
        actor->getAtomLabelId() > 100000)
      continue;

    if (actor->getAtomLabelId() != selectAtomId)
      continue;
    m_renderer->RemoveActor(actor);
  }
}
