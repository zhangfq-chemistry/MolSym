#include <QApplication>
#include <QClipboard>
#include <algorithm>

#include "View3D.h"
#include "mTools.h"
#include "mainwindow.h"
#include "math/vector3.h"
#include "templateMol.h"

#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkBox.h>
#include <vtkCapsuleSource.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkInteractorStyle.h>
#include <vtkLightCollection.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTDxInteractorStyleCamera.h>
#include <vtkTDxInteractorStyleSettings.h>
#include <vtkTubeFilter.h>
#include <vtkWindowToImageFilter.h>

// #include "vtkCapsuleSource.h"

#include <vtkOrientationMarkerWidget.h>

#include <QVTKInteractor.h>

#include <vtkSmartPointer.h>
#include <vtkVolumeProperty.h>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <vtkAppendPolyData.h>
#include <vtkAxesActor.h>
#include <vtkCleanPolyData.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataNormals.h>
#include <vtkReflectionFilter.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkVolumeProperty.h>

#include <vtkLight.h>
#include <vtkMolecule.h>
#include <vtkMoleculeMapper.h>
#include <vtkMoleculeToAtomBallFilter.h>
#include <vtkPolygon.h>
#include <vtkSimpleBondPerceiver.h>
#include <vtkTriangle.h>

#include <QMessageBox>
#include <QTime>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkStringArray.h>

#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkCoordinate.h>
#include <vtkDiskSource.h>
#include <vtkHexagonalPrism.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricTorus.h>
#include <vtkPentagonalPrism.h>
#include <vtkPolygon.h>
#include <vtkPolyhedron.h>
#include <vtkRegularPolygonSource.h>
#include <vtkTextActor.h>
#include <vtkTextSource.h>
#include <vtkTexture.h>
#include <vtkTexturedSphereSource.h>
#include <vtkTransformFilter.h>
#include <vtkTransformTextureCoords.h>
#include <vtkTubeFilter.h>
#include <vtkVectorText.h>

#include "templateView3d.h"
#include <QColorDialog>
#include <QList>
#include <QMimeData>
#include <vtkAssembly.h>
#include <vtkCoordinate.h>
#include <vtkFollower.h>
#include <vtkLabeledDataMapper.h>
#include <vtkMatrix4x4.h>
#include <vtkTransformCoordinateSystems.h>
#include <vtkVectorText.h>

#ifndef _WIN32
int Point::_tag = 0;
#endif

void View3D::updateLabelPosFunc(vtkObject *, unsigned long eid,
                                void *clientdata, void *calldata) {
  updateAtomLabelState();
}

void View3D::initializeActor(vtkActor *actor) {
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

HMol *View3D::getMol() { return mol; }

MainWindow *View3D::getMainWindow() { return mainWindow; }

void View3D::setTemplateView3d(TemplateView3d *t) { view3dT = t; }

void View3D::setXYZAxisVisible(bool t) { _isXYZAxisVisible = t; }

void View3D::setSymmetryElementVisible(bool b) { isPGVisible = b; }

void View3D::setOutLineType(unsigned int i) { outLineType = i; }

void View3D::setAtomLabelVisible(bool b) { _isAtomLabelVisible = b; }

bool View3D::isAtomLabelVisible() { return _isAtomLabelVisible; }

bool View3D::isBondLabelVisible() { return _isBondLabelVisible; }
void View3D::setBondLabelVisible(bool b) { _isBondLabelVisible = b; }

void View3D::setCnVisible(bool b) { _isCnVisible = b; }

void View3D::setHorizontalC2Visible(bool b) { _isAtomLabelVisible = b; }

void View3D::setSigmaHVisible(bool b) { _isSigmaHVisible = b; }

void View3D::setSigmaVVisible(bool b) { _isSigmaVVisible = b; }

void View3D::setSigmaDVisible(bool b) { _isSigmaDVisible = b; }

View3D::View3D(QWidget *parent) : QVTKOpenGLNativeWidget(parent) {
  this->setWindowTitle("3D Viewer");

  vtkNew<vtkGenericOpenGLRenderWindow> window;
  setRenderWindow(window.Get());

  setAcceptDrops(true);

  // background color
  bkcolor.Set(0.4, 0.5, 0.6);
  whiteColor.Set(1.0, 1.0, 1.0);
  // bkcolor.Set(0.8,0.8,0.8);

  // Camera
  camera = vtkSmartPointer<vtkCamera>::New();
  camera->SetViewUp(0, 1, 0);
  camera->SetPosition(0, 0, 18);
  camera->SetFocalPoint(0, 0, 0);
  // camera->SetParallelScale(0.0);
  camera->ParallelProjectionOn();

  // Renderer
  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  m_renderer->SetActiveCamera(camera);
  m_renderer->SetBackground(bkcolor.x(), bkcolor.y(), bkcolor.z());

  m_renderer->LightFollowCameraOn();
  m_renderer->TwoSidedLightingOn();

  /*
  m_renderer->SetUseDepthPeeling(1);
  m_renderer->SetOcclusionRatio(0.1);
  m_renderer->SetMaximumNumberOfPeels(100);
*/

  renderWindow()->AddRenderer(m_renderer);
  renderWindow()->SetMultiSamples(0);
  renderWindow()->SetAlphaBitPlanes(1);

  _isXYZAxisVisible = false;

  plane = nullptr;
  clipper_pos = nullptr;
  clipper_neg = nullptr;

  opicity_node = 0.5;
  center = VZero;

  scale_atom = 0.3;
  scale_bond = 0.3;

  bondRadius = 0.07;

  principalAxisRadius = 0.04;

  setMolTypeBallStick();

  moleculeLength = 4.0;

  PointGroup = "C1";
  discThickness = 0.05;
  discRadius = 0.2;

  updateSelectedFromRendering = false;
  origin_SelectedAtoms = VZero;
  length_SelectedAtoms = 0.0;

  firstTime = true;

  buildC2_Sybmol(discThickness);

  setAreaInteractor();

  tmol = new templateMol();
  _isKekuleStyle = false;

  // mouseOperationMode=Rotation;

  selectedAtomActors = vtkSmartPointer<vtkActorCollection>::New();
  selectedBondActors = vtkSmartPointer<vtkActorCollection>::New();
  selectedRingActors = vtkSmartPointer<vtkActorCollection>::New();

  // correct label position
  atomLabelActors = vtkSmartPointer<vtkActorCollection>::New();

  selectedAtomActors->InitTraversal();
  selectedBondActors->InitTraversal();
  selectedRingActors->InitTraversal();
  atomLabelActors->InitTraversal();

  selectedAtomList.clear();
  selectedBondList.clear();
  selectedRingList.clear();

  initParameter();
  onViewReset();
}

void View3D::initParameter() {
  selectedColor = buildColor(Qt::yellow);

  grey[0] = 0.6;
  grey[1] = 0.6;
  grey[2] = 0.6;

  white[0] = 1.0;
  white[1] = 1.0;
  white[2] = 1.0;

  black[0] = 0.0;
  black[1] = 0.0;
  black[2] = 0.0;

  blue[0] = 0.0;
  blue[1] = 0.0;
  blue[2] = 1.0;

  yellow[0] = 1.0;
  yellow[1] = 1.0;
  yellow[2] = 0.2;

  brown[0] = 0.5;
  brown[1] = 0.3;
  brown[2] = 0.7;

  color1_C2[0] = color_sigmaV[0] = 0.10;
  color1_C2[1] = color_sigmaV[1] = 0.86;
  color1_C2[2] = color_sigmaV[2] = 0.94;

  color2_C2[0] = color_sigmaD[0] = 0.3;
  color2_C2[1] = color_sigmaD[1] = 0.9;
  color2_C2[2] = color_sigmaD[2] = 0.4;

  for (unsigned int i = 0; i < 3; i++) {
    color1_C2[i] = color_sigmaV[i];
    color2_C2[i] = color_sigmaD[i];
    color_C3[i] = color_sigmaD[i];
  }

  color_Cn[0] = 0.728;
  color_Cn[1] = 0.2;
  color_Cn[2] = 0.2;

  color_sigmaH[0] = 0.749;
  color_sigmaH[1] = 0.815;
  color_sigmaH[2] = 0.815;

  OpacityH = 1.0;
  OpacityV = 1.0;
  OpacityD = 1.0;

  verticalLength = HorizontalLengh = 2.0;
  discCn_scale = 1.0;
  discC2_scale = 1.0;
  discC3_scale = 1.0;

  isMol_Visible = true;
  isHydrogen_Visible = true;

  thicknessSigmaV0 = thicknessSigmaD0 = thicknessSigmaH0 = 0.01;
  thicknessSigmaV_Scale = thicknessSigmaD_Scale = thicknessSigmaH_Scale = 1.0;

  thicknessV = thicknessSigmaV0 * thicknessSigmaV_Scale * 2.0;
  thicknessD = thicknessSigmaD0 * thicknessSigmaD_Scale * 2.0;
  thicknessH = thicknessSigmaH0 * thicknessSigmaH_Scale * 2.0;

  thickness_C2_Scale = thickness_C3_Scale = thickness_Cn_Scale = 1.0;

  outLineType = 1;
  typeSigmaH = typeSigmaV = typeSigmaD = 1;

  isPGVisible = false;
  isHorizontalC2Visible = true;
  _isSigmaHVisible = _isSigmaVVisible = _isSigmaDVisible = true;
  _isCnVisible = _isSnVisible = _isI2nVisible = true;

  _isCiVisible = isCsVisible = true;

  _isC2_1_Visible = _isC2_2_Visible = true;

  _isC2_Visible = _isC3_Visible = _isC4_Visible = _isC5_Visible =
      _isC6_Visible = true;
  _isI2nVisible = true;
  isI2n = false;

  _isAtomLabelVisible = false;

  horizontalC2_type = 1;

  CnRadius_Scale = C2Radius_Scale = C3Radius_Scale = C4Radius_Scale = 1.0;
  ;
  Cn_Radius = C2_Radius = C3_Radius = C4_Radius =
      principalAxisRadius * CnRadius_Scale;

  Cn_Length = C2_Length = C3_Length = 2.0;

  scale_ringRadius = 0.7;
  scale_ringSize = 0.4;

  list_sigmaV_visible.clear();
  list_sigmaD_visible.clear();
  list_sigmaH_visible.clear();

  list_C2_1_Horizontal_visible.clear();
  list_C2_2_Horizontal_visible.clear();

  list_C2_Horizontal_visible.clear();
  list_C2_visible.clear();

  list_C3_visible.clear();
  list_C4_visible.clear();
  list_C5_visible.clear();
  list_S2n_visible.clear();
}

void View3D::setAreaInteractor() {
  actorInteractorA = SelectAreaInteractorStyle::New();
  actorInteractorA->SetDefaultRenderer(m_renderer);
  actorInteractorA->setView3d(this);
  ;
  interactor()->SetInteractorStyle(actorInteractorA);
}

void View3D::updateView() { renderWindow()->Render(); }

void View3D::setBKColor(vector3 v) { setBKColor(v.x(), v.y(), v.z()); }

void View3D::setBKColor(float x, float y, float z) {
  // cout << x << " "<< y << " "<< z << " ssssssssssss"<<endl;
  bkcolor.Set(x / 255.0, y / 255.0, z / 255.0);
  // cout << bkcolor.x() << " "<< bkcolor.y() << " "<< bkcolor.z() << "
  // ssssssssssss"<<endl;
  m_renderer->SetBackground(bkcolor.x(), bkcolor.y(), bkcolor.z());
  refresh();
}

void View3D::onViewXY() {
  onViewReset();

  camera->SetViewUp(0, 1, 0);
  double p[3];
  camera->GetPosition(p);
  vector3 v(p[0], p[1], p[2]);
  camera->SetPosition(0, 0, v.length());

  // camera->SetFocalPoint(0,0,0);
  camera->ComputeViewPlaneNormal();
  updateView();
}

void View3D::onViewXZ() {
  double p[3];
  camera->GetPosition(p);
  vector3 v(p[0], p[1], p[2]);

  onViewReset();
  camera->SetPosition(0, 0, v.length());
  camera->Elevation(-90);
  camera->SetFocalPoint(0, 0, 0);
  camera->ComputeViewPlaneNormal();
  updateView();
}

void View3D::onViewYZ() {
  double p[3];
  camera->GetPosition(p);
  vector3 v(p[0], p[1], p[2]);

  onViewReset();
  camera->SetPosition(0, 0, v.length());
  camera->SetRoll(-90.0);
  // camera->Azimuth(90);
  camera->Elevation(-90);
  camera->SetFocalPoint(0, 0, 0);
  camera->ComputeViewPlaneNormal();
  updateView();
}

void View3D::onViewReset() {
  // onViewXY();

  camera->SetViewUp(0, 1, 0);
  camera->SetPosition(0, 0, 10);
  camera->SetFocalPoint(0, 0, 0);
  camera->ComputeViewPlaneNormal();

  updateView();
  updateAtomLabelState();
}

void View3D::onView_rotateClockwise(double degree) {
  double angle = camera->GetRoll();
  camera->SetRoll(angle - degree);
  camera->ComputeViewPlaneNormal();
  updateView();
}

void View3D::onView_rotateCounterClockwise(double degree) {
  double angle = camera->GetRoll();
  camera->SetRoll(angle + degree);
  camera->ComputeViewPlaneNormal();
  updateView();
}

void View3D::onView_rotateUp(double degree) {
  camera->Elevation(-degree);
  camera->ComputeViewPlaneNormal();
  updateView();
}

void View3D::onView_rotateDown(double degree) {
  camera->Elevation(degree);
  camera->ComputeViewPlaneNormal();
  updateView();
}

void View3D::onView_rotateLeft(double degree) {
  camera->Azimuth(degree);
  camera->ComputeViewPlaneNormal();
  updateView();
}

void View3D::onView_rotateRight(double degree) {
  camera->Azimuth(-degree);
  camera->ComputeViewPlaneNormal();
  updateView();
}

void View3D::renderAxis() {
  if (!_isXYZAxisVisible)
    return;

  QString label = "X";

  double length = HorizontalLengh * 1.2;
  if (length < verticalLength)
    length = verticalLength * 1.2;

  length *= 0.5;
  double l = 0.1;
  for (int i = 0; i < 3; i++) {
    auto lineSource = vtkSmartPointer<vtkLineSource>::New();
    auto lineSource1 = vtkSmartPointer<vtkLineSource>::New();
    auto lineSource2 = vtkSmartPointer<vtkLineSource>::New();

    auto actor = vtkSmartPointer<vtkActor>::New();
    auto actor1 = vtkSmartPointer<vtkActor>::New();
    auto actor2 = vtkSmartPointer<vtkActor>::New();

    if (i < 1) {
      lineSource->SetPoint1(-length, 0, 0);
      lineSource->SetPoint2(length, 0, 0);

      lineSource1->SetPoint1(length, 0, 0);
      lineSource2->SetPoint1(length, 0, 0);

      lineSource1->SetPoint2(length - l, l, 0);
      lineSource2->SetPoint2(length - l, -l, 0);

      actor->GetProperty()->SetColor(1, 0, 0);
      actor1->GetProperty()->SetColor(1, 0, 0);
      actor2->GetProperty()->SetColor(1, 0, 0);

      label = "X";
    }

    if (i == 1) {
      lineSource->SetPoint1(0, -length, 0);
      lineSource->SetPoint2(0, length, 0);

      label = "Y";

      lineSource1->SetPoint1(0, length, 0);
      lineSource2->SetPoint1(0, length, 0);

      lineSource1->SetPoint2(l, length - l, 0);
      lineSource2->SetPoint2(-l, length - l, 0);

      actor->GetProperty()->SetColor(0, 1, 0);
      actor1->GetProperty()->SetColor(0, 1, 0);
      actor2->GetProperty()->SetColor(0, 1, 0);
    }

    if (i > 1) {
      lineSource->SetPoint1(0, 0, -length);
      lineSource->SetPoint2(0, 0, length);

      label = "Z";

      lineSource1->SetPoint1(0, 0, length);
      lineSource2->SetPoint1(0, 0, length);

      lineSource1->SetPoint2(l, 0, length - l);
      lineSource2->SetPoint2(-l, 0, length - l);

      actor->GetProperty()->SetColor(0, 0, 1);
      actor1->GetProperty()->SetColor(0, 0, 1);
      actor2->GetProperty()->SetColor(0, 0, 1);
    }

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(lineSource->GetOutputPort());
    actor->SetMapper(mapper);
    m_renderer->AddActor(actor);

    auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper1->SetInputConnection(lineSource1->GetOutputPort());
    actor1->SetMapper(mapper1);
    m_renderer->AddActor(actor1);

    auto mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputConnection(lineSource2->GetOutputPort());
    actor2->SetMapper(mapper2);
    m_renderer->AddActor(actor2);

    auto textSource = vtkSmartPointer<vtkTextSource>::New();
    textSource->SetText(label.toStdString().c_str());
    textSource->SetForegroundColor(0.0, 0.0, 1.0);
    textSource->BackingOff();

    auto textActor = vtkSmartPointer<vtkFollower>::New();
    if (i < 1) {
      textSource->SetForegroundColor(1.0, 0.0, 0.0);
      textActor->AddPosition(length + l, -0.09, 0.0);
      // textActor->GetProperty()->SetColor(0, 0, 1);
    }

    if (i == 1) {
      textSource->SetForegroundColor(0, 1, 0);
      textActor->AddPosition(-0.05, length + l, 0.0);
      // textActor->GetProperty()->SetColor(0, 1, 0);
    }

    if (i > 1) {
      textSource->SetForegroundColor(0, 0, 1);
      textActor->AddPosition(-0.05, -0.05, length + l);
      // textActor->GetProperty()->SetColor(0, 0, 1);
    }

    textSource->Update();
    auto textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    textMapper->SetInputConnection(textSource->GetOutputPort());

    textActor->SetPickable(false);
    textActor->SetMapper(textMapper);
    textActor->SetScale(0.01, 0.01, 0.01);

    textActor->SetCamera(camera);
    m_renderer->AddActor(textActor);
  }
}

void View3D::renderLines(vector3 a, vector3 b, vector3 c, vector3 shift,
                         float width, double color[3]) {
  // lines
  renderLine(VZero - shift, a - shift, width, color);
  renderLine(a - shift, a + b - shift, width, color);
  renderLine(a - shift, a + c - shift, width, color);

  // b
  renderLine(VZero - shift, b - shift, width, color);
  renderLine(b - shift, a + b - shift, width, color);
  renderLine(b - shift, c + b - shift, width, color);

  // c
  renderLine(VZero - shift, c - shift, width, color);
  renderLine(c - shift, a + c - shift, width, color);
  renderLine(c - shift, b + c - shift, width, color);

  renderLine(a + b - shift, a + b + c - shift, width, color);
  renderLine(b + c - shift, a + b + c - shift, width, color);
  renderLine(a + c - shift, a + b + c - shift, width, color);
}

void View3D::clearAll() {
  removeAllActors();

  releaseAtomLabelActors();
  releaseSelectedAtomActors();
  releaseSelectedBondActors();
  releaseSelectedRingActors();

  if (mol_movie) {
    mol_movie->clearAll();
    delete mol_movie;
  }
}

void View3D::refresh() {
  if (!mol)
    return;

  removeAllActors();
  renderAxis();

  renderAtoms();
  renderBonds();

  renderSymmetryElement();
  render_Circle();

  if (renderMovie)
    renderMol_movie();

  updateView();
}

void View3D::renderMol_movie() {
  renderBonds_movie();
  renderAtoms_movie();
  renderRings_movie();
}

// just update size of molecule
void View3D::renderMol(HMol *m) {
  if (m == nullptr) {
    cout << " mol is null " << endl;
    return;
  }

  mol = m;

  if (mol->NumAtoms() >= 2) {
    mol->calcMolVolume();
    updateMolLength();
  }

  removeAllActors();
  renderAxis();
  renderBonds();
  renderAtoms();

  renderSymmetryElement();
  updateView();
}

void View3D::renderSelectedAtomLabel() {
  unsigned int size = numSelectedAtoms();
  if (size < 1)
    return;

  double radius = 0.;
  vector3 v;

  for (unsigned int i = 0; i < size; i++) {
    unsigned int idx = selectedAtomList[i];

    QString label = QString::number(idx + 1);

    auto textSource = vtkSmartPointer<vtkTextSource>::New();
    textSource->SetText(label.toStdString().c_str());
    textSource->SetForegroundColor(0.0, 0.0, 1.0);
    textSource->BackingOff();
    textSource->Update();

    auto textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    textMapper->SetInputConnection(textSource->GetOutputPort());

    auto labelActor = vtkSmartPointer<vtkFollower>::New();
    labelActor->SetPickable(false);
    labelActor->SetMapper(textMapper);
    labelActor->SetScale(0.02, 0.02, 0.02);
    labelActor->setAtomLabelId(idx);

    radius = scale_atom * mol->getAtomRadiusbyIndex(idx) / 2.0;

    vector3 v = mol->getAtomPosbyIndex(idx);

    labelActor->AddPosition(v.x() - radius / 2., v.y() - radius / 2.,
                            v.z() + radius * 2.0);
    labelActor->SetCamera(camera);
    m_renderer->AddActor(labelActor);
  }
}

void View3D::updateAtomLabelState() {
  atomLabelActors->InitTraversal();
  unsigned int _numActors = atomLabelActors->GetNumberOfItems();

  if (_numActors < 1)
    return;

  double normalC[3];
  Camera()->GetViewPlaneNormal(normalC);
  vector3 pc(normalC);

  double radius, r2;
  for (unsigned int i = 0; i < _numActors; ++i) {
    radius = scale_atom * mol->getAtomRadiusbyIndex(i);
    vtkActor *textActor = atomLabelActors->GetNextActor();
    vector3 v = mol->getAtomPosbyIndex(i) + pc * radius;
    textActor->SetPosition(v.x(), v.y(), v.z());
  }
}

void View3D::renderAtomLabel() {
  if (!isMol_Visible)
    return;
  if (mol->NumAtoms() < 1)
    return;
  if (!_isAtomLabelVisible)
    return;

  atomLabelActors->RemoveAllItems();

  Camera()->GetViewPlaneNormal(normalCamera);
  vector3 pc(normalCamera), v;

  QString label;
  double radius;
  for (unsigned int i = 0; i < mol->NumAtoms(); i++) {
    radius = scale_atom * mol->getAtomRadiusbyIndex(i);

    label = QString::number(i + 1);

    // Create text
    auto textSource = vtkSmartPointer<vtkVectorText>::New();
    textSource->SetText(label.toStdString().c_str());
    textSource->Update();

    auto textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    textMapper->SetInputConnection(textSource->GetOutputPort());

    auto textActor = vtkSmartPointer<vtkFollower>::New();
    initializeActor(textActor);

    textActor->setAtomLabelId(i);
    textActor->SetPickable(false);
    textActor->SetMapper(textMapper);
    textActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    textActor->SetScale(0.2, 0.2, 0.2);

    v = mol->getAtomPosbyIndex(i) + pc * radius;
    textActor->SetPosition(v.x(), v.y(), v.z());
    textActor->SetCamera(camera);
    m_renderer->AddActor(textActor);

    atomLabelActors->AddItem(textActor);
  }
}

void View3D::renderBondLabel() {
  // cout <<"View3D::renderBondLabel()"<<endl;

  if (!isMol_Visible)
    return;
  if (!_isBondLabelVisible)
    return;

  if (mol->NumBonds() < 1)
    return;

  Camera()->GetViewPlaneNormal(normalCamera);
  vector3 pc(normalCamera), v;
  pc *= 0.1;

  QString label;
  for (unsigned int i = 0; i < mol->NumBonds(); i++) {
    label = QString::number(i + 1);

    // Create text
    auto textSource = vtkSmartPointer<vtkVectorText>::New();
    textSource->SetText(label.toStdString().c_str());
    textSource->Update();

    auto textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    textMapper->SetInputConnection(textSource->GetOutputPort());

    auto textActor = vtkSmartPointer<vtkFollower>::New();
    initializeActor(textActor);

    textActor->setBondLabelId(i);
    textActor->SetPickable(false);
    textActor->SetMapper(textMapper);
    textActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    textActor->SetScale(0.2, 0.2, 0.2);

    v = (mol->getAtomPosbyIndex(mol->getBondbyIndex(i)->atomIndex0()) +
         mol->getAtomPosbyIndex(mol->getBondbyIndex(i)->atomIndex1())) *
        0.5;

    textActor->SetPosition(v.x() + pc[0], v.y() + pc[1], v.z() + pc[2]);
    textActor->SetCamera(camera);
    m_renderer->AddActor(textActor);

    // bondLabelActors->AddItem(textActor);
  }
}

void View3D::renderAtoms() {
  if (!isMol_Visible)
    return;

  if (mol->NumAtoms() < 1)
    return;

  // cout << "\n\nnumber of atoms : "<< mol->NumAtoms()<<endl;
  // cout << "render atoms! "<<endl;
  // displaySelectedAtomList();

  double x, y, z;
  vector3 color;

  unsigned int beg = numActors();

  bool updateActor = false;

  if (updateSelectedFromRendering) {
    if (selectedAtomList.size() > 0)
      updateActor = true;

    if (updateActor)
      selectedAtomActors->RemoveAllItems();
  }

  // cout << "selected atoms inside render Atom"<<endl;
  auto sphere = vtkSphereSource::New();
  sphere->SetPhiResolution(20 + 20 * scale_atom);
  sphere->SetThetaResolution(20 + 20 * scale_atom);

  for (unsigned int i = 0; i < mol->NumAtoms(); i++) {
    if (!isHydrogen_Visible)
      if (mol->getAtomSymbol(i) == "H")
        continue;

    x = mol->getAtomXbyIndex(i);
    y = mol->getAtomYbyIndex(i);
    z = mol->getAtomZbyIndex(i);

    auto sphere = vtkSphereSource::New();
    sphere->SetPhiResolution(20 + 20 * scale_atom);
    sphere->SetThetaResolution(20 + 20 * scale_atom);

    double radius = scale_atom * mol->getAtomRadiusbyIndex(i);
    sphere->SetRadius(radius);
    sphere->Update();
    color = MakeGlColor(mol->atomList[i]->Color());

    auto actor = vtkActor::New();
    initializeActor(actor);
    actor->SetPosition(x, y, z);
    auto mapper = vtkPolyDataMapper::New();
    actor->storeColor(color.x(), color.y(), color.z());

    /*
        actor->GetProperty()->SetDiffuse(0.6);
        actor->GetProperty()->SetAmbient(0.3);
        actor->GetProperty()->SetSpecular(0.2);
        actor->GetProperty()->SetSpecularPower(3.0);
   */

    if (mol->getAtombyIndex(i)->isSelected()) {
      actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                     selectedColor.z());
      if (updateActor)
        selectedAtomActors->AddItem(actor);
    } else
      actor->GetProperty()->SetColor(color.x(), color.y(), color.z());

    actor->SetMapper(mapper);
    actor->setAtomId(i);
    actor->setId(i + beg);

    // cout << i+1 << " "<< actor->AtomId()<<endl;

    mapper->SetInputConnection(sphere->GetOutputPort());
    m_renderer->AddActor(actor);

    mapper->Delete();
    sphere->Delete();

    if (i < mol->NumAtoms() - 1)
      continue;
    lastAtomActor = actor;
  }

  renderAtomLabel();
}

void View3D::renderQuadrupleBond(unsigned int idx) {
  vector3 color;
  HBond *bond = mol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  bool _selected = mol->getBondbyIndex(idx)->isSelected();

  vector3 v0 = mol->getAtomPosbyIndex(idx1);
  vector3 v1 = mol->getAtomPosbyIndex(idx2);
  vector3 v = v1 - v0;

  vector3 shift, shiftv;

  if (bond->getPlane().length() > 0.5) {
    shift = 0.12 * bond->getPlane();
    shiftv = 0.12 * cross1(bond->getPlane(), v);
  } else {
    v.createOrthoVector(shift);
    shiftv = 0.12 * cross1(v, shift);
    shift = 0.12 * shift;
  }

  unsigned int beg = numActors();

  // the same atom
  double s = 0.5;

  // different atom, the border of two half-bonds lies the middle of two
  // sphere-surface (sphere1)--*--(sphere2)
  if (mol->getAtombyIndex(idx1)->atomicNum() !=
      mol->getAtombyIndex(idx2)->atomicNum()) {
    double length = (v1 - v0).length();
    double r1 = scale_atom * mol->getAtombyIndex(idx1)->radius();
    double r2 = scale_atom * mol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;
  }

  vector3 mid0 = v0 + s * (v1 - v0);

  v0 = v0 + (shift + shiftv) / 2.0;
  v1 = v1 + (shift + shiftv) / 2.0;
  mid0 = mid0 + (shift + shiftv) / 2.0;

  for (int k = 0; k < 2; k++) {
    for (int l = 0; l < 2; l++) {
      vector3 p0 = v0 + shift * (k - 1) + shiftv * (l - 1),
              p1 = v1 + shift * (k - 1) + shiftv * (l - 1),
              mid = mid0 + shift * (k - 1) + shiftv * (l - 1);

      auto line = vtkSmartPointer<vtkLineSource>::New();
      line->SetPoint1(p0[0], p0[1], p0[2]);
      line->SetPoint2(mid[0], mid[1], mid[2]);

      auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
      tubeFilter->SetInputConnection(line->GetOutputPort());
      tubeFilter->SetRadius(bondRadius * 0.8);
      tubeFilter->SetNumberOfSides(20);

      color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
      auto actor = vtkActor::New();
      initializeActor(actor);
      auto mapper = vtkPolyDataMapper::New();

      if (_selected) {
        actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                       selectedColor.z());
        if (updateSelectedFromRendering)
          selectedBondActors->AddItem(actor);
      } else
        actor->GetProperty()->SetColor(color.x(), color.y(), color.z());

      actor->storeColor(color.x(), color.y(), color.z());
      actor->SetMapper(mapper);

      // actor->SetPickable(false);
      actor->setId(beg);
      actor->setAtomId(idx1);
      actor->setAtomId1(idx2);
      actor->setBondId(idx);
      actor->setBondOrder(4);

      mapper->SetInputConnection(tubeFilter->GetOutputPort());
      m_renderer->AddActor(actor);

      auto line1 = vtkSmartPointer<vtkLineSource>::New();
      line1->SetPoint1(mid[0], mid[1], mid[2]);
      line1->SetPoint2(p1[0], p1[1], p1[2]);

      auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
      tubeFilter1->SetInputConnection(line1->GetOutputPort());
      tubeFilter1->SetRadius(bondRadius * 0.8);
      tubeFilter1->SetNumberOfSides(20);

      auto actor1 = vtkActor::New();
      initializeActor(actor1);
      // actor1->SetPickable(false);
      auto mapper1 = vtkPolyDataMapper::New();
      color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());

      if (_selected) {
        actor1->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                        selectedColor.z());
        if (updateSelectedFromRendering)
          selectedBondActors->AddItem(actor1);
      } else
        actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());

      actor1->storeColor(color.x(), color.y(), color.z());
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

void View3D::renderHydrogenBond(unsigned int idx) {
  vector3 color;
  HBond *bond = mol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  bool _selected = mol->getBondbyIndex(idx)->isSelected();

  vector3 v1 = mol->getAtomPosbyIndex(idx1);
  vector3 v2 = mol->getAtomPosbyIndex(idx2);
  vector3 v = v2 - v1;

  unsigned int beg = numActors();

  unsigned int NSteps = 10;

  // the same atom
  double s = 0.5;

  // different atom, the border of two half-bonds lies the middle of two
  // sphere-surface
  if (mol->getAtombyIndex(idx1)->atomicNum() !=
      mol->getAtombyIndex(idx2)->atomicNum()) {
    double length = v.length();
    double r1 = scale_bond * mol->getAtombyIndex(idx1)->radius();
    double r2 = scale_bond * mol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;
  }

  vector3 mid0 = v1 + s * v;
  ;

  vector3 p0 = v1, p1 = v2, mid = mid0;

  // dash line ----------------------------------------;
  double mlength = (v2 - v1).length() / NSteps;
  vector3 step = mlength * (p1 - p0).normalize();

  p0 = p0 + step / 2.0;
  p1 = p1 + step / 2.0;

  color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
  for (unsigned int k = 0; k < NSteps - 1; k++) {
    vector3 pp0 = p0 + k * step, pp1 = p0 + (k + 1) * step;

    if (k < 1)
      continue;

    if (k % 2 == 0)
      continue;

    if (k > (NSteps - 1) / 2)
      color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());
    /*
    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(pp0[0], pp0[1], pp0[2]);
    line->SetPoint2(pp1[0], pp1[1], pp1[2]);
    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(bondRadius*0.2);
    tubeFilter->SetNumberOfSides(20);
    tubeFilter->SetCapping(true);

    auto a = vtkActor::New();
    initializeActor(a);
    auto m = vtkPolyDataMapper::New();

    if(_selected)
    {
        a->GetProperty()->SetColor(selectedColor.x(),selectedColor.y(),selectedColor.z());
        if (updateSelectedFromRendering) selectedBondActors->AddItem(a);
    }
    else   a->GetProperty()->SetColor(color.x(),color.y(),color.z());

    a->storeColor(color.x(),color.y(),color.z());
    a->SetMapper(m);

    a->setBondId(idx);
    a->setId(beg);
    a->setAtomId(idx1);
    a->setAtomId1(idx2);
    a->setBondOrder(5);

    m->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(a);
    m->Delete();
*/

    unsigned int times = 2;
    while (times--) {
      auto sphere = vtkSphereSource::New();
      sphere->SetPhiResolution(20);
      sphere->SetThetaResolution(20);
      sphere->SetRadius(bondRadius * 0.5);
      sphere->Update();

      auto a1 = vtkActor::New();
      initializeActor(a1);

      a1->storeColor(color.x(), color.y(), color.z());

      if (times == 1)
        a1->SetPosition(pp0[0], pp0[1], pp0[2]);
      else
        a1->SetPosition(pp1[0], pp1[1], pp1[2]);

      if (_selected) {
        a1->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                    selectedColor.z());
        if (updateSelectedFromRendering)
          selectedAtomActors->AddItem(a1);
      } else
        a1->GetProperty()->SetColor(blue);

      a1->setBondId(idx);
      a1->setId(beg);
      a1->setAtomId(idx1);
      a1->setAtomId1(idx2);
      a1->setBondOrder(5);

      auto m1 = vtkPolyDataMapper::New();
      a1->SetMapper(m1);
      m1->SetInputConnection(sphere->GetOutputPort());
      m_renderer->AddActor(a1);

      m1->Delete();
      sphere->Delete();
    }
  }

  /*
  vector3 color;
  HBond * bond= mol->bondList[idx];
  unsigned int idx1=bond->atomIndex0();
  unsigned int idx2=bond->atomIndex1();

  bool _selected=mol->getBondbyIndex(idx)->isSelected();

  vector3 v1=mol->getAtomPosbyIndex(idx1);
  vector3 v2=mol->getAtomPosbyIndex(idx2);

  double p1[3] = {v1.x(),v1.y(),v1.z()};
  double p2[3] = {v2.x(),v2.y(),v2.z()};


  auto actor_line = vtkActor::New();initializeActor(actor_line);
  actor_line->GetProperty()->ShadingOn();
  actor_line->GetProperty()->SetLineStipplePattern(0xf0f0);
  actor_line->GetProperty()->SetLineStippleRepeatFactor(5);
  actor_line->SetPickable(true);
  actor_line->GetProperty()->SetPointSize(1.0);
  actor_line->GetProperty()->SetLineWidth(1.5);

  if(_selected)
  {
      actor_line->GetProperty()->SetColor(selectedColor.x(),selectedColor.y(),selectedColor.z());
      if (updateSelectedFromRendering) selectedBondActors->AddItem(actor_line);
  }
  else   actor_line->GetProperty()->SetColor(blue);


  actor_line->setBondId(idx);
  actor_line->setAtomId(idx1);
  actor_line->setAtomId1(idx2);
  actor_line->setBondOrder(5);

  auto line=vtkSmartPointer<vtkLineSource>::New();
  line->SetPoint1(p1);
  line->SetPoint2(p2);
  auto mapper_line =  vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper_line->SetInputConnection( line->GetOutputPort() );
  mapper_line->ScalarVisibilityOn();
  actor_line->SetMapper(mapper_line);
  m_renderer->AddActor(actor_line);
  */
}

void View3D::renderWeakBond(unsigned int idx) {
  vector3 color;
  HBond *bond = mol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  bool _selected = mol->getBondbyIndex(idx)->isSelected();

  vector3 v1 = mol->getAtomPosbyIndex(idx1);
  vector3 v2 = mol->getAtomPosbyIndex(idx2);
  vector3 v = v2 - v1;

  unsigned int beg = numActors();

  unsigned int NSteps = 10;

  // the same atom
  double s = 0.5;

  // different atom, the border of two half-bonds lies the middle of two
  // sphere-surface
  if (mol->getAtombyIndex(idx1)->atomicNum() !=
      mol->getAtombyIndex(idx2)->atomicNum()) {
    double length = v.length();
    double r1 = scale_bond * mol->getAtombyIndex(idx1)->radius();
    double r2 = scale_bond * mol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;
  }

  vector3 mid0 = v1 + s * v;
  ;

  vector3 p0 = v1, p1 = v2, mid = mid0;

  // dash line ----------------------------------------;
  double mlength = (v2 - v1).length() / NSteps;
  vector3 step = mlength * (p1 - p0).normalize();

  p0 = p0 + step / 2.0;
  p1 = p1 + step / 2.0;

  color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
  for (unsigned int k = 0; k < NSteps - 1; k++) {
    vector3 pp0 = p0 + k * step, pp1 = p0 + (k + 1) * step;

    if (k < 1)
      continue;

    if (k % 2 == 0)
      continue;

    if (k > (NSteps - 1) / 2)
      color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());

    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(pp0[0], pp0[1], pp0[2]);
    line->SetPoint2(pp1[0], pp1[1], pp1[2]);
    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(bondRadius * 0.6);
    tubeFilter->SetNumberOfSides(20);
    tubeFilter->SetCapping(true);

    auto a = vtkActor::New();
    initializeActor(a);
    auto m = vtkPolyDataMapper::New();

    if (_selected) {
      a->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                 selectedColor.z());
      if (updateSelectedFromRendering)
        selectedBondActors->AddItem(a);
    } else
      a->GetProperty()->SetColor(color.x(), color.y(), color.z());

    a->storeColor(color.x(), color.y(), color.z());
    a->SetMapper(m);

    a->setBondId(idx);
    a->setId(beg);
    a->setAtomId(idx1);
    a->setAtomId1(idx2);
    a->setBondOrder(7);

    m->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(a);
    m->Delete();

    unsigned int times = 2;
    while (times--) {
      auto sphere = vtkSphereSource::New();
      sphere->SetPhiResolution(20);
      sphere->SetThetaResolution(20);
      sphere->SetRadius(bondRadius * 0.6);
      sphere->Update();

      auto a1 = vtkActor::New();
      initializeActor(a1);

      a1->storeColor(color.x(), color.y(), color.z());

      // cout << " times " << times << endl;

      if (times == 1)
        a1->SetPosition(pp0[0], pp0[1], pp0[2]);
      else
        a1->SetPosition(pp1[0], pp1[1], pp1[2]);

      if (_selected) {
        a1->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                    selectedColor.z());
        if (updateSelectedFromRendering)
          selectedAtomActors->AddItem(a1);
      } else
        a1->GetProperty()->SetColor(color.x(), color.y(), color.z());

      a1->setBondId(idx);
      a1->setId(beg);
      a1->setAtomId(idx1);
      a1->setAtomId1(idx2);
      a1->setBondOrder(7);

      auto m1 = vtkPolyDataMapper::New();
      a1->SetMapper(m1);
      m1->SetInputConnection(sphere->GetOutputPort());
      m_renderer->AddActor(a1);

      m1->Delete();
      sphere->Delete();
    }
  }
}

void View3D::renderAromaticBond(unsigned int idx) {
  HBond *bond = mol->bondList[idx];
  if (bond->isInsideRing()) {
    renderAromaticBond_insideRings(idx);
    return;
  }

  vector3 color;
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  bool _selected = mol->getBondbyIndex(idx)->isSelected();

  vector3 v1 = mol->getAtomPosbyIndex(idx1);
  vector3 v2 = mol->getAtomPosbyIndex(idx2);
  vector3 v = v2 - v1;

  unsigned int beg = numActors();
  unsigned int NSteps = 6;

  // the same atom
  double s = 0.5;

  // different atom, the border of two half-bonds lies the middle of two
  // sphere-surface
  if (mol->getAtombyIndex(idx1)->atomicNum() !=
      mol->getAtombyIndex(idx2)->atomicNum()) {
    double length = v.length();
    double r1 = scale_atom * mol->getAtombyIndex(idx1)->radius();
    double r2 = scale_atom * mol->getAtombyIndex(idx2)->radius();
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

  color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
  auto actor = vtkActor::New();
  initializeActor(actor);
  auto mapper = vtkPolyDataMapper::New();

  if (_selected) {
    actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                   selectedColor.z());
    if (updateSelectedFromRendering)
      selectedBondActors->AddItem(actor);
  } else
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
  color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());

  if (_selected) {
    actor1->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                    selectedColor.z());
    if (updateSelectedFromRendering)
      selectedBondActors->AddItem(actor1);
  } else
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

  color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
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
      color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());

    auto capsule = vtkSmartPointer<vtkCapsuleSource>::New();
    capsule->SetRadius(bondRadius * 0.6);
    capsule->SetThetaResolution(10);
    capsule->SetPhiResolution(10);
    capsule->SetCylinderLength((pp1 - pp0).length());

    auto a = vtkActor::New();
    initializeActor(a);
    auto m = vtkPolyDataMapper::New();

    if (_selected) {
      a->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                 selectedColor.z());
      if (updateSelectedFromRendering)
        selectedBondActors->AddItem(a);
    } else
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

void View3D::renderAromaticBond_insideRings(unsigned int idx) {
  vector3 color;
  HBond *bond = mol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  bool _selected = mol->getBondbyIndex(idx)->isSelected();

  vector3 v1 = mol->getAtomPosbyIndex(idx1);
  vector3 v2 = mol->getAtomPosbyIndex(idx2);
  vector3 v = v2 - v1;
  vector3 midp = 0.5 * (v2 + v1);

  unsigned int NSteps = 6;
  double _length = v.length() / NSteps;

  unsigned int beg = numActors();

  // the same atom
  double s = 0.5;

  // different atom, the border of two half-bonds lies the middle of two
  // sphere-surface
  if (mol->getAtombyIndex(idx1)->atomicNum() !=
      mol->getAtombyIndex(idx2)->atomicNum()) {
    double length = v.length();
    double r1 = scale_atom * mol->getAtombyIndex(idx1)->radius();
    double r2 = scale_atom * mol->getAtombyIndex(idx2)->radius();
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

  color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
  auto actor = vtkActor::New();
  initializeActor(actor);
  auto mapper = vtkPolyDataMapper::New();

  if (_selected) {
    actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                   selectedColor.z());
    if (updateSelectedFromRendering)
      selectedBondActors->AddItem(actor);
  } else
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
  color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());

  if (_selected) {
    actor1->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                    selectedColor.z());
    if (updateSelectedFromRendering)
      selectedBondActors->AddItem(actor1);
  } else
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
    ring = mol->getRingbyId(bond->getRingId(i));
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

    color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
    for (unsigned int k = 0; k < NSteps - 1; k++) {
      if (k < 1)
        continue;
      if (k % 2 == 0)
        continue;

      pp0 = p0 + k * step;
      pp1 = p0 + (k + 1) * step;
      cc = (pp0 + pp1) * 0.5;

      if (k > (NSteps - 1) / 2)
        color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());

      auto capsule = vtkSmartPointer<vtkCapsuleSource>::New();
      capsule->SetRadius(bondRadius * 0.6);
      capsule->SetThetaResolution(10);
      capsule->SetPhiResolution(10);
      capsule->SetCylinderLength((pp1 - pp0).length());

      auto a = vtkActor::New();
      initializeActor(a);
      auto m = vtkPolyDataMapper::New();

      if (_selected) {
        a->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                   selectedColor.z());
        if (updateSelectedFromRendering)
          selectedBondActors->AddItem(a);
      } else
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

// render all rings of the molecule
void View3D::renderAromaticRings() {
  if (_isKekuleStyle)
    return;

  unsigned int _nRings = mol->numRings();
  if (_nRings < 1)
    return;

  // cout << "Number of ring " << mol->numRings()<<endl;

  matrix3x3 m;
  for (unsigned int i = 0; i < _nRings; i++) {
    HRing *ring = mol->getRingbyId(i);

    auto parametricObject = vtkSmartPointer<vtkParametricTorus>::New();
    auto parametricFunctionSource =
        vtkSmartPointer<vtkParametricFunctionSource>::New();
    parametricFunctionSource->SetParametricFunction(parametricObject);

    parametricObject->SetRingRadius(ring->radius - 0.5);
    if (ring->radius < 1.0)
      parametricObject->SetRingRadius(ring->radius - 0.6);
    parametricObject->SetCrossSectionRadius(bondRadius * scale_ringRadius *
                                            ring->scale);

    parametricFunctionSource->SetUResolution(100);
    parametricFunctionSource->SetVResolution(100);
    parametricFunctionSource->SetWResolution(100);

    parametricFunctionSource->Update();

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(parametricFunctionSource->GetOutputPort());
    mapper->ScalarVisibilityOn();

    m.SetupRotateMatrix(VZ, ring->norm);
    auto transform = vtkSmartPointer<vtkTransform>::New();
    transform->PostMultiply();
    if (!isEqual(VZ, ring->norm) && !isNegativeEqual(VZ, ring->norm)) {
      auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
      for (int ii = 0; ii < 3; ii++)
        for (int jj = 0; jj < 3; jj++)
          matrix->SetElement(ii, jj, m(ii, jj));

      for (int ii = 0; ii < 4; ii++) {
        matrix->SetElement(3, ii, 0.0);
        matrix->SetElement(ii, 3, 0.0);
      }
      matrix->SetElement(3, 3, 1.0);

      transform->SetMatrix(matrix);
    }
    transform->Translate(ring->center.x(), ring->center.y(), ring->center.z());

    auto actor = vtkSmartPointer<vtkActor>::New();
    initializeActor(actor);
    actor->SetMapper(mapper);
    actor->SetPickable(true);
    if (ring->isSelected)
      actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                     selectedColor.z());
    else
      actor->GetProperty()->SetColor(ring->color.x(), ring->color.y(),
                                     ring->color.z());
    actor->storeColor(ring->color.x(), ring->color.y(), ring->color.z());
    actor->setRingId(i);
    actor->setBondOrder(6);
    actor->SetUserTransform(transform);

    m_renderer->AddActor(actor);
  }
}

void View3D::renderTripleBond(unsigned int idx) {
  vector3 color;
  HBond *bond = mol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  vector3 v0 = mol->getAtomPosbyIndex(idx1);
  vector3 v1 = mol->getAtomPosbyIndex(idx2);

  vector3 v = v1 - v0;
  vector3 shift;
  v.createOrthoVector(shift);
  shift = 0.1 * shift;
  if (bond->getPlane().length() > 0.5)
    shift = 0.1 * cross1(bond->getPlane(), v1 - v0);
  v1 = v1 - shift;
  v0 = v0 - shift;

  bool _selected = mol->getBondbyIndex(idx)->isSelected();

  unsigned int beg = numActors();

  // the same atom
  double s = 0.5;

  // different atom, the border of two half-bonds lies the middle of two
  // sphere-surface
  if (mol->getAtombyIndex(idx1)->atomicNum() !=
      mol->getAtombyIndex(idx2)->atomicNum()) {
    double length = (v1 - v0).length();
    double r1 = scale_atom * mol->getAtombyIndex(idx1)->radius();
    double r2 = scale_atom * mol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;
  }

  for (unsigned int k = 0; k < 3; k++) {
    vector3 p0 = v0 + shift * k, p1 = v1 + shift * k;
    vector3 p = p0 + s * (p1 - p0);

    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(p0[0], p0[1], p0[2]);
    line->SetPoint2(p[0], p[1], p[2]);

    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputConnection(line->GetOutputPort());
    tubeFilter->SetRadius(bondRadius * 0.7);
    tubeFilter->SetNumberOfSides(20);

    color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
    auto actor = vtkActor::New();
    initializeActor(actor);
    auto mapper = vtkPolyDataMapper::New();

    if (_selected) {
      actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                     selectedColor.z());
      if (updateSelectedFromRendering)
        selectedBondActors->AddItem(actor);
    } else
      actor->GetProperty()->SetColor(color.x(), color.y(), color.z());

    actor->storeColor(color.x(), color.y(), color.z());
    actor->SetMapper(mapper);

    // actor->SetPickable(false);
    actor->setId(beg);
    actor->setAtomId(idx1);
    actor->setAtomId1(idx2);
    actor->setBondId(idx);
    actor->setBondOrder(3);

    mapper->SetInputConnection(tubeFilter->GetOutputPort());
    m_renderer->AddActor(actor);

    auto line1 = vtkSmartPointer<vtkLineSource>::New();
    line1->SetPoint1(p[0], p[1], p[2]);
    line1->SetPoint2(p1[0], p1[1], p1[2]);

    auto tubeFilter1 = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter1->SetInputConnection(line1->GetOutputPort());
    tubeFilter1->SetRadius(bondRadius * 0.7);
    tubeFilter1->SetNumberOfSides(20);

    auto actor1 = vtkActor::New();
    initializeActor(actor1);
    auto mapper1 = vtkPolyDataMapper::New();
    color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());

    if (_selected) {
      actor1->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                      selectedColor.z());
      if (updateSelectedFromRendering)
        selectedBondActors->AddItem(actor1);
    } else
      actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());

    actor1->storeColor(color.x(), color.y(), color.z());
    actor1->SetMapper(mapper1);

    // actor1->SetPickable(false);
    actor1->setAtomId(idx2);
    actor1->setAtomId1(idx1);
    actor1->setId(beg);
    actor1->setBondId(idx);
    actor1->setBondOrder(3);

    mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
    m_renderer->AddActor(actor1);

    mapper->Delete();
    mapper1->Delete();
  }
}

void View3D::renderDoubleBond(unsigned int idx) {
  HBond *bond = mol->bondList[idx];

  if (bond->isInsideRing()) {
    if (!_isKekuleStyle) {
      renderSingleBond(idx);
      return;
    }
  }

  vector3 color;

  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  // cout << idx1+1 << " "<< idx2+1<< " inside renderDoubleBond" <<endl;

  vector3 v0 = mol->getAtomPosbyIndex(idx1);
  vector3 v1 = mol->getAtomPosbyIndex(idx2);

  vector3 v = v1 - v0;
  vector3 shift;

  if (bond->getPlane().length() > 0.5) {
    shift = cross(bond->getPlane(), v).normalize();
  } else {
    v.createOrthoVector(shift);
  }
  shift *= 0.10;

  unsigned int beg = numActors();

  // the same atom
  double s = 0.5;

  // different atom: the border lies the middle of two sphere-surface
  if (mol->getAtombyIndex(idx1)->atomicNum() !=
      mol->getAtombyIndex(idx2)->atomicNum()) {
    double length = v.length();
    double r1 = scale_atom * mol->getAtombyIndex(idx1)->radius();
    double r2 = scale_atom * mol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;
  }

  vector3 mid0 = v0 + s * v;

  bool _selected = mol->getBondbyIndex(idx)->isSelected();

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
    tubeFilter->SetNumberOfSides(20);

    color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
    auto actor = vtkActor::New();
    initializeActor(actor);
    auto mapper = vtkPolyDataMapper::New();

    actor->storeColor(color.x(), color.y(), color.z());

    if (_selected) {
      actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                     selectedColor.z());
      if (updateSelectedFromRendering)
        selectedBondActors->AddItem(actor);
    } else
      actor->GetProperty()->SetColor(color.x(), color.y(), color.z());

    actor->SetMapper(mapper);

    // actor->SetPickable(false);
    actor->setId(beg);
    actor->setAtomId(idx1);
    actor->setAtomId1(idx2);
    actor->setBondOrder(2);
    actor->setBondId(idx);

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
    // actor1->SetPickable(false);
    auto mapper1 = vtkPolyDataMapper::New();
    color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());

    if (_selected) {
      actor1->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                      selectedColor.z());
      if (updateSelectedFromRendering)
        selectedBondActors->AddItem(actor1);
    } else
      actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());

    actor1->storeColor(color.x(), color.y(), color.z());
    actor1->SetMapper(mapper1);

    actor1->setAtomId(idx2);
    actor1->setAtomId1(idx1);
    actor1->setId(beg);
    actor1->setBondOrder(2);
    actor1->setBondId(idx);

    mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
    m_renderer->AddActor(actor1);

    mapper->Delete();
    mapper1->Delete();
  }
}

void View3D::renderSimpleSingleBond(unsigned int idx) {
  vector3 color = {0.6, 0.6, 0.6};
  HBond *bond = mol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  bool _selected = mol->getBondbyIndex(idx)->isSelected();

  vector3 p0 = mol->getAtomPosbyIndex(idx1);
  vector3 p1 = mol->getAtomPosbyIndex(idx2);

  unsigned int beg = numActors();

  auto line = vtkSmartPointer<vtkLineSource>::New();
  line->SetPoint1(p0[0], p0[1], p0[2]);
  line->SetPoint2(p1[0], p1[1], p1[2]);

  auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
  tubeFilter->SetInputConnection(line->GetOutputPort());
  tubeFilter->SetRadius(bondRadius);
  tubeFilter->SetNumberOfSides(20);

  // color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
  auto actor = vtkActor::New();
  initializeActor(actor);
  auto mapper = vtkPolyDataMapper::New();

  vector3 greyColor(0.6, 0.6, 0.6);
  actor->GetProperty()->SetColor(greyColor.x(), greyColor.y(), greyColor.z());
  actor->storeColor(color.x(), color.y(), color.z());
  actor->setBondOrder(1);

  actor->GetProperty()->SetDiffuse(0.9);
  actor->GetProperty()->SetAmbient(0.3);
  actor->GetProperty()->SetSpecular(0.2);
  actor->GetProperty()->SetSpecularPower(6.0);

  if (mol->getBondbyAtomIndex(idx1, idx2)->isSelected()) {
    actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                   selectedColor.z());
    if (updateSelectedFromRendering)
      selectedBondActors->AddItem(actor);
  } else {
    actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
  }

  actor->SetMapper(mapper);

  actor->setId(beg);
  actor->setAtomId(idx1);
  actor->setAtomId1(idx2);
  actor->setBondId(idx);
  actor->setBondOrder(1);

  mapper->SetInputConnection(tubeFilter->GetOutputPort());
  m_renderer->AddActor(actor);
  mapper->Delete();
}

void View3D::renderSingleBond(unsigned int idx) {
  vector3 color;
  HBond *bond = mol->bondList[idx];
  unsigned int idx1 = bond->atomIndex0();
  unsigned int idx2 = bond->atomIndex1();

  bool _selected = mol->getBondbyIndex(idx)->isSelected();

  vector3 p0 = mol->getAtomPosbyIndex(idx1);
  vector3 p1 = mol->getAtomPosbyIndex(idx2);

  //  if(_selected)
  //      cout << idx1+1 << " "<< idx2+1<<" is selected"<<endl;
  //  else
  //     cout << idx1+1 << " "<< idx2+1<<" is not selected"<<endl;

  unsigned int beg = numActors();

  double s = 0.5;

  if (mol->getAtombyIndex(idx1)->atomicNum() !=
      mol->getAtombyIndex(idx2)->atomicNum()) {
    double length = (p1 - p0).length();
    double r1 = scale_atom * mol->getAtombyIndex(idx1)->radius();
    double r2 = scale_atom * mol->getAtombyIndex(idx2)->radius();
    double d = 0.5 * (length - r2 - r1);
    s = (d + r1) / length;
  }

  vector3 p = p0 + s * (p1 - p0);

  auto line = vtkSmartPointer<vtkLineSource>::New();
  line->SetPoint1(p0[0], p0[1], p0[2]);
  line->SetPoint2(p[0], p[1], p[2]);

  auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
  tubeFilter->SetInputConnection(line->GetOutputPort());
  tubeFilter->SetRadius(bondRadius);
  tubeFilter->SetNumberOfSides(20);

  color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex0())->Color());
  auto actor = vtkActor::New();
  initializeActor(actor);
  auto mapper = vtkPolyDataMapper::New();

  actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
  actor->storeColor(color.x(), color.y(), color.z());
  actor->setBondOrder(1);

  /*
  actor->GetProperty()->SetDiffuse(0.9);
  actor->GetProperty()->SetAmbient(0.3);
  actor->GetProperty()->SetSpecular(0.2);
  actor->GetProperty()->SetSpecularPower(6.0);
*/

  if (mol->getBondbyAtomIndex(idx1, idx2)->isSelected()) {
    // cout << idx+1 << " "<< idx1+1 << " "<< idx2+1 << " is selected"<<endl;
    actor->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                   selectedColor.z());
    if (updateSelectedFromRendering)
      selectedBondActors->AddItem(actor);
  } else {
    actor->GetProperty()->SetColor(color.x(), color.y(), color.z());
    // cout << idx+1 << " "<< idx1+1 << " "<< idx2+1 << " is not
    // selected"<<endl;
  }

  actor->setId(beg);
  actor->setAtomId(idx1);
  actor->setAtomId1(idx2);
  actor->setBondId(idx);
  actor->setBondOrder(1);

  actor->SetMapper(mapper);
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
  initializeActor(actor1);
  auto mapper1 = vtkPolyDataMapper::New();
  color = MakeGlColor(mol->getAtombyIndex(bond->atomIndex1())->Color());
  actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());
  actor1->storeColor(color.x(), color.y(), color.z());

  /*
  actor1->GetProperty()->SetDiffuse(0.9);
  actor1->GetProperty()->SetAmbient(0.3);
  actor1->GetProperty()->SetSpecular(0.2);
  actor1->GetProperty()->SetSpecularPower(6.0);
*/

  actor1->SetMapper(mapper1);
  actor1->setAtomId(idx2);
  actor1->setAtomId1(idx1);
  actor1->setId(beg);
  actor1->setBondOrder(1);
  actor1->setBondId(idx);

  if (_selected) {
    actor1->GetProperty()->SetColor(selectedColor.x(), selectedColor.y(),
                                    selectedColor.z());
    if (updateSelectedFromRendering)
      selectedBondActors->AddItem(actor1);
  } else {
    actor1->GetProperty()->SetColor(color.x(), color.y(), color.z());
  }

  mapper1->SetInputConnection(tubeFilter1->GetOutputPort());
  m_renderer->AddActor(actor1);

  mapper->Delete();
  mapper1->Delete();
}

void View3D::renderBonds() {
  if (!isMol_Visible)
    return;

  if (updateSelectedFromRendering)
    selectedBondActors->RemoveAllItems();

  for (unsigned int j = 0; j < mol->NumBonds(); j++) {
    unsigned int bo = mol->bondList[j]->getBondOrder();

    if (!isHydrogen_Visible) {
      if (mol->getAtomSymbol(mol->bondList[j]->atomIndex0()) == "H")
        continue;
      if (mol->getAtomSymbol(mol->bondList[j]->atomIndex1()) == "H")
        continue;
    }

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

    case 5:
      renderHydrogenBond(j);
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

  renderBondLabel();

  // cout << "render bonds done! "<<endl;
}

void View3D::writePNG(QString name) {
  // vector3 color=bkcolor;
  // setBKColor(whiteColor);

  auto windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput(renderWindow());
  windowToImageFilter->SetScale(4);
  windowToImageFilter->SetInputBufferTypeToRGBA(); // also record the alpha
                                                   // (transparency) channel
  windowToImageFilter->ReadFrontBufferOff();       // read from the back buffer
  windowToImageFilter->Update();

  auto writer = vtkSmartPointer<vtkPNGWriter>::New();
  writer->SetFileName(name.toLatin1());
  writer->SetInputConnection(windowToImageFilter->GetOutputPort());
  writer->Write();

  // setBKColor(color);
}

void View3D::updateMolLength() {
  if (mol != nullptr && mol->NumAtoms() > 0)
    mol->perceiveMolLength(verticalLength, HorizontalLengh);

  Cn_Length = verticalLength + 0.1;

  C2_Length = HorizontalLengh;
  C3_Length = HorizontalLengh;

  if (PointGroup.startsWith("O")) // Oh and O
  {
    Cn_Length = HorizontalLengh;
    C2_Length = HorizontalLengh;
    C3_Length = HorizontalLengh;
    ;
  }
  if (PointGroup.startsWith("T")) // Td, Th and T
  {
    Cn_Length = C2_Length;
    C3_Length = Cn_Length;
  }

  if (PointGroup.startsWith("I")) // Ih and I
  {
    C2_Length = Cn_Length;
    C3_Length = Cn_Length;
  }

  // Cn_Radius=0.2;
  mainWindow->updatePanelParameters();
}

void View3D::renderSymmetryElement() {
  // parse symmetry
  if (!isPGVisible)
    return;

  isI2n = false;

  QString temp = PointGroup;
  QString PG = PointGroup.toUpper();

  //  cout << temp.toStdString().c_str() <<endl;

  if (PG == "C1") {
    renderC1();
    goto label;
  }
  //--------------------------------------------------------
  if (PG == "CS") {
    renderCs();
    goto label;
  } else if (PG == "CI") {
    renderCenter(0.1 * discCn_scale, yellow);
    renderC1();
    goto label;
  } else if (PG == "C**V" || PG == "COOV" || PG == "C00V") {
    principalOrder = 100;
    renderCoov();
    goto label;
  } else if (PG == "D**H" || PG == "DOOH" || PG == "D00H") {
    principalOrder = 100;
    renderDooh();
    goto label;
  }

  else if (PG == "T") {
    renderT();
    goto label;
  } else if (PG == "TD") {
    renderTd();
    goto label;
  } else if (PG == "TH") {
    renderTh();
    goto label;
  }

  else if (PG == "O") {
    renderO();
    goto label;
  } else if (PG == "OH") {
    renderOh();
    goto label;
  }

  else if (PG == "I") {
    renderI();
    goto label;
  } else if (PG == "IH") {
    renderIh();
    goto label;
  }

  //-------------------------------------------------
  // Cnv  Cnh  Cn point group
  bool ok;
  if (PG.at(0) == 'C' && PG.at(1).isDigit()) {
    temp.remove(0, 1);
    if (PG.contains("V")) // Cnv
    {
      temp.remove(temp.length() - 1, 1);
      principalOrder = temp.toInt(&ok, 10);
      renderCnv(principalOrder);
      goto label;
    }
    //------------------------------------
    else if (PG.contains("H")) //! Cnh
    {
      temp.remove(temp.length() - 1, 1);
      principalOrder = temp.toInt(&ok, 10);
      renderCnh(principalOrder);
      goto label;
    }
    //-----------------------------------
    else // Cn
    {
      principalOrder = temp.toInt(&ok, 10);
      renderCn();

      goto label;
    }
  }

  // Dn  Dnh
  if (PG.at(0) == 'D') {
    temp.remove(0, 1);

    if (PG.right(1) == "H") // Dnh
    {
      temp.remove(temp.length() - 1, 1);
      principalOrder = temp.toInt(&ok, 10);

      renderDnh(principalOrder);

      goto label;
    } else if (PG.right(1) == "D") // Dnd
    {
      temp.remove(temp.length() - 1, 1);
      principalOrder = temp.toInt(&ok, 10);
      renderDnd(principalOrder);

      goto label;
    } else // Dn
    {
      principalOrder = temp.toInt(&ok, 10);
      renderDn(principalOrder);

      goto label;
    }
  }

  // Sn
  if (PointGroup.at(0) == 'S') {
    temp.remove(0, 1);
    principalOrder = temp.toInt(&ok, 10);
    renderSn(principalOrder);

    goto label;
  }

label:
  // renderPrincipleCylinder(principalOrder);
  return;
}

void View3D::buildC2_Sybmol(double thickness) {
  auto disk0 = vtkSmartPointer<vtkCylinderSource>::New();
  auto disk1 = vtkSmartPointer<vtkCylinderSource>::New();

  disk0->SetCenter(0.2, 0.0, 0.0);
  disk1->SetCenter(-0.2, 0.0, 0.0);

  disk0->SetResolution(100);
  disk0->SetHeight(thickness);
  disk0->SetRadius(0.3);
  disk0->SetCapping(true);
  disk0->Update();

  disk1->SetResolution(100);
  disk1->SetHeight(thickness);
  disk1->SetRadius(0.3);
  disk1->SetCapping(true);
  disk1->Update();

  bfPolyData = vtkPolyDataBooleanFilter::New();
  bfPolyData->SetInputData(0, disk0->GetOutput());
  bfPolyData->SetInputData(1, disk1->GetOutput());
  bfPolyData->SetOperModeToIntersection();
  bfPolyData->Update();
}

void View3D::renderPrincipleCylinder() {
  if (!_isCnVisible)
    return;

  unsigned int order = principalOrder;
  renderPrincipleCylinder(order, color_Cn);
}

void View3D::renderC1() {
  render_Circle();

  if (!_isCnVisible)
    return;

  auto cone = vtkSmartPointer<vtkConeSource>::New();
  cone->SetResolution(100);
  cone->SetHeight(discThickness * 5);
  cone->SetCapping(true);
  cone->SetRadius(discCn_scale * discRadius);
  cone->SetCenter(0.0, 0.0, 0.0);
  cone->Update();

  auto trans = vtkSmartPointer<vtkTransform>::New();
  trans->PostMultiply();
  trans->RotateY(-90.0);
  trans->Translate(0.0, 0.0, Cn_Length * 0.5);

  auto actor = vtkActor::New();
  actor->GetProperty()->SetColor(color_Cn);
  actor->GetProperty()->ShadingOn();
  actor->SetUserTransform(trans);
  actor->GetProperty()->SetInterpolationToGouraud();
  actor->SetPickable(false);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(cone->GetOutputPort());
  mapper->ScalarVisibilityOff();
  actor->SetMapper(mapper);
  m_renderer->AddActor(actor);

  auto cylinderAxis = vtkSmartPointer<vtkCylinderSource>::New();
  cylinderAxis->SetCenter(0.0, 0.0, 0.0);
  cylinderAxis->SetResolution(20);
  cylinderAxis->SetHeight(Cn_Length);
  cylinderAxis->SetRadius(Cn_Radius);
  cylinderAxis->SetCapping(true);
  cylinderAxis->Update();

  auto rotation1 = vtkSmartPointer<vtkTransform>::New();
  rotation1->PostMultiply();
  rotation1->RotateX(90.0);

  auto actorAxis = vtkActor::New();
  actorAxis->GetProperty()->SetColor(color_Cn);
  actorAxis->GetProperty()->ShadingOn();
  actorAxis->GetProperty()->SetInterpolationToGouraud();
  actorAxis->SetUserTransform(rotation1);
  actorAxis->SetPickable(false);

  auto mapperAxis = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperAxis->SetInputData(cylinderAxis->GetOutput());
  actorAxis->SetMapper(mapperAxis);
  m_renderer->AddActor(actorAxis);
}

void View3D::renderPrincipleCylinder(unsigned int order, double color[3]) {
  if (order < 2)
    return;

  if (order == 2) {
    auto trans = vtkSmartPointer<vtkTransform>::New();
    trans->PostMultiply();
    trans->Scale(discCn_scale, 1.0, discCn_scale);
    trans->RotateX(90.0);
    trans->Translate(0.0, 0.0, Cn_Length / 2.0);

    auto actor = vtkActor::New();
    actor->GetProperty()->SetColor(color);
    actor->GetProperty()->ShadingOn();
    actor->SetUserTransform(trans);
    actor->GetProperty()->SetInterpolationToGouraud();
    actor->SetPickable(false);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(bfPolyData->GetOutputPort());
    mapper->ScalarVisibilityOff();
    actor->SetMapper(mapper);

    m_renderer->AddActor(actor);
  }

  else {
    auto cylinder = vtkSmartPointer<vtkCylinderSource>::New();
    cylinder->SetCenter(0.0, 0.0, 0.0);
    cylinder->SetResolution(order);
    cylinder->SetHeight(discThickness);
    cylinder->SetCapping(true);
    cylinder->SetRadius(discCn_scale * discRadius);
    cylinder->Update();

    auto rotation = vtkSmartPointer<vtkTransform>::New();
    rotation->PostMultiply();
    rotation->RotateX(90.0);

    rotation->RotateZ(90.0);
    if (order % 2 == 1)
      rotation->RotateZ(90.0 + 90.0 / order);

    rotation->Translate(0.0, 0.0, Cn_Length / 2.0);

    auto actor = vtkActor::New();
    actor->GetProperty()->SetColor(color);
    actor->GetProperty()->ShadingOn();
    actor->SetUserTransform(rotation);
    actor->SetPickable(false);

    actor->GetProperty()->SetInterpolationToGouraud();

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(cylinder->GetOutput());
    actor->SetMapper(mapper);
    m_renderer->AddActor(actor);
    // mapper->Delete();
  }

  if (isI2n && _isI2nVisible) {
    auto cylinder = vtkSmartPointer<vtkCylinderSource>::New();
    cylinder->SetCenter(0.0, 0.0, 0.0);
    cylinder->SetResolution(order * 2);
    cylinder->SetHeight(discThickness);
    cylinder->SetCapping(true);

    cylinder->SetRadius(discCn_scale * discRadius * 1.1);
    if (order == 2)
      cylinder->SetRadius(discCn_scale * discRadius * 1.2);
    cylinder->Update();

    auto rotation = vtkSmartPointer<vtkTransform>::New();
    rotation->PostMultiply();
    rotation->RotateX(90.0);

    rotation->RotateZ(90.0);
    if (order % 2 == 1)
      rotation->RotateZ(90.0 + 90.0 / order);

    rotation->Translate(0.0, 0.0, Cn_Length / 2.0 - 0.01);

    auto actor = vtkActor::New();
    actor->GetProperty()->SetColor(brown);
    actor->GetProperty()->ShadingOn();
    actor->SetUserTransform(rotation);
    actor->SetPickable(false);

    actor->GetProperty()->SetInterpolationToGouraud();

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(cylinder->GetOutput());
    actor->SetMapper(mapper);
    m_renderer->AddActor(actor);
  }

  auto cylinderAxis = vtkSmartPointer<vtkCylinderSource>::New();
  cylinderAxis->SetCenter(0.0, 0.0, 0.0);
  cylinderAxis->SetResolution(20);
  cylinderAxis->SetHeight(Cn_Length);
  cylinderAxis->SetRadius(Cn_Radius);
  cylinderAxis->SetCapping(true);
  cylinderAxis->Update();

  auto rotation1 = vtkSmartPointer<vtkTransform>::New();
  rotation1->PostMultiply();
  rotation1->RotateX(90.0);

  auto actorAxis = vtkActor::New();
  actorAxis->GetProperty()->SetColor(color);
  actorAxis->GetProperty()->ShadingOn();
  actorAxis->GetProperty()->SetInterpolationToGouraud();
  actorAxis->SetUserTransform(rotation1);
  actorAxis->SetPickable(false);

  auto mapperAxis = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperAxis->SetInputData(cylinderAxis->GetOutput());
  actorAxis->SetMapper(mapperAxis);
  m_renderer->AddActor(actorAxis);
}

void View3D::render_Circle() {
  if (!isPGVisible)
    return;

  if (PointGroup.startsWith("T") || PointGroup.startsWith("O") ||
      PointGroup.startsWith("I"))
    return;

  if (outLineType < 1)
    return;

  // Create a circle
  auto polygonSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
  polygonSource->SetNumberOfSides(1000);
  polygonSource->SetRadius(HorizontalLengh / 2.0);

  polygonSource->GeneratePolygonOff();

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(polygonSource->GetOutputPort());
  ;
  auto actor = vtkSmartPointer<vtkActor>::New();
  initializeActor(actor);

  actor->GetProperty()->SetColor(white);
  actor->SetPickable(false);
  actor->GetProperty()->ShadingOff();
  actor->SetMapper(mapper);
  m_renderer->AddActor(actor);
}

void View3D::renderCoov() {
  renderPrincipleCylinder();
  render_Circle();

  if (list_sigmaV_visible[0] > 0)
    renderSigmaV(90);
}

void View3D::renderDooh() {
  // render one sphere disc and sigmaH

  renderSigmaH();
  renderPrincipleCylinder();

  if (_isCnVisible)
    renderCi();

  renderCenter(0.1 * discCn_scale, yellow);

  if (list_sigmaV_visible[0] > 0)
    renderSigmaV(90);
}

void View3D::renderSigmaV(double angle) {
  // if(!_isSigmaVVisible) return;

  if (verticalLength < 0.3)
    verticalLength = 0.3;

  // square
  if (typeSigmaV == 1) {
    double shift = 0.002;
    auto cube = vtkSmartPointer<vtkCubeSource>::New();
    cube->SetCenter(0.0, 0.0, 0.0);
    cube->SetXLength(HorizontalLengh);
    cube->SetYLength(thicknessV);
    cube->SetZLength(verticalLength);
    cube->Update();

    auto actor = vtkActor::New();
    auto mapper = vtkPolyDataMapper::New();
    actor->GetProperty()->SetColor(color_sigmaV);

    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(OpacityV);
    actor->GetProperty()->ShadingOn();
    actor->SetPickable(false);

    auto rotation = vtkSmartPointer<vtkTransform>::New();
    rotation->PostMultiply();
    rotation->RotateZ(angle + 90.0);
    actor->SetUserTransform(rotation);

    mapper->ScalarVisibilityOn();
    mapper->SetInputConnection(cube->GetOutputPort());
    actor->GetProperty()->SetInterpolationToGouraud();
    m_renderer->AddActor(actor);
    mapper->Delete();

    double v = verticalLength * 0.5 - shift;
    double h = HorizontalLengh * 0.5 - shift;

    double p1[3] = {-h, 0, -v};
    double p2[3] = {h, 0, -v};
    double p3[3] = {h, 0, v};
    double p4[3] = {-h, 0, v};

    auto points = vtkSmartPointer<vtkPoints>::New();

    points->InsertNextPoint(p1);
    points->InsertNextPoint(p2);
    points->InsertNextPoint(p3);
    points->InsertNextPoint(p4);
    points->InsertNextPoint(p1);

    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoints(points);
    line->Update();

    auto actor_skeleton = vtkActor::New();
    actor_skeleton->GetProperty()->SetColor(color_sigmaV);
    actor_skeleton->GetProperty()->ShadingOff();

    actor_skeleton->GetProperty()->SetLineStipplePattern(0xf0f0);
    actor_skeleton->GetProperty()->SetLineStippleRepeatFactor(1);

    actor_skeleton->SetPickable(false);
    actor_skeleton->GetProperty()->SetPointSize(1.0);
    actor_skeleton->GetProperty()->SetLineWidth(1.5);
    actor_skeleton->GetProperty()->ShadingOff();
    // actor_skeleton->GetProperty()->SetOpacity(0.5);

    actor_skeleton->SetUserTransform(rotation);

    auto mapper_skeleton = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper_skeleton->SetInputConnection(line->GetOutputPort());
    mapper_skeleton->ScalarVisibilityOn();

    actor_skeleton->SetMapper(mapper_skeleton);
    m_renderer->AddActor(actor_skeleton);

    return;
  } else // Create a circle
  {
    auto polygonSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource->SetNumberOfSides(1000);
    polygonSource->SetRadius(HorizontalLengh / 2.0);

    auto circle = vtkSmartPointer<vtkRegularPolygonSource>::New();
    circle->SetNumberOfSides(1000);
    circle->SetRadius(HorizontalLengh / 2.0);
    circle->GeneratePolygonOff();

    auto actor = vtkActor::New();
    auto mapper = vtkPolyDataMapper::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(OpacityV);
    actor->GetProperty()->SetColor(color_sigmaV);
    actor->GetProperty()->ShadingOn();

    actor->SetPickable(false);

    auto rotation = vtkSmartPointer<vtkTransform>::New();
    rotation->PostMultiply();
    rotation->Scale(1.0, verticalLength / HorizontalLengh, 1.0);
    rotation->RotateX(90.0);
    rotation->RotateZ(angle + 90.0);
    actor->SetUserTransform(rotation);

    mapper->ScalarVisibilityOn();
    mapper->SetInputConnection(polygonSource->GetOutputPort());

    actor->GetProperty()->SetInterpolationToGouraud();
    m_renderer->AddActor(actor);

    auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper1->SetInputConnection(circle->GetOutputPort());
    ;
    auto actor1 = vtkSmartPointer<vtkActor>::New();

    actor1->GetProperty()->SetColor(color_sigmaV);
    actor1->GetProperty()->ShadingOff();
    actor1->SetUserTransform(rotation);
    actor1->SetMapper(mapper1);
    m_renderer->AddActor(actor1);
  }
}

void View3D::renderSigmaD(double angle) {
  // if(!_isSigmaDVisible) return;
  if (verticalLength < 0.3)
    verticalLength = 0.3;

  // square
  if (typeSigmaD == 1) {
    double shift = 0.002;

    auto cube = vtkSmartPointer<vtkCubeSource>::New();
    cube->SetCenter(0.0, 0.0, 0.0);

    cube->SetXLength(HorizontalLengh);
    cube->SetYLength(thicknessD);
    cube->SetZLength(verticalLength);
    cube->Update();

    auto actor = vtkActor::New();
    auto mapper = vtkPolyDataMapper::New();
    actor->GetProperty()->SetColor(color_sigmaD);

    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(OpacityD);
    actor->GetProperty()->ShadingOn();
    actor->SetPickable(false);

    auto rotation = vtkSmartPointer<vtkTransform>::New();
    rotation->PostMultiply();
    rotation->RotateZ(angle + 90.0);
    actor->SetUserTransform(rotation);

    mapper->ScalarVisibilityOn();
    mapper->SetInputConnection(cube->GetOutputPort());
    actor->GetProperty()->SetInterpolationToGouraud();
    m_renderer->AddActor(actor);
    mapper->Delete();

    // Line-frame
    double v = verticalLength * 0.5 - shift; // hide a little
    double h = HorizontalLengh * 0.5 - shift;

    double p1[3] = {-h, 0, -v};
    double p2[3] = {h, 0, -v};
    double p3[3] = {h, 0, v};
    double p4[3] = {-h, 0, v};

    auto points = vtkSmartPointer<vtkPoints>::New();

    points->InsertNextPoint(p1);
    points->InsertNextPoint(p2);
    points->InsertNextPoint(p3);
    points->InsertNextPoint(p4);
    points->InsertNextPoint(p1);

    auto line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoints(points);
    line->Update();

    auto actor_skeleton = vtkActor::New();
    actor_skeleton->GetProperty()->SetColor(color_sigmaD);
    actor_skeleton->GetProperty()->ShadingOff();

    actor_skeleton->GetProperty()->SetLineStipplePattern(0xf0f0);
    actor_skeleton->GetProperty()->SetLineStippleRepeatFactor(1);

    actor_skeleton->SetPickable(false);
    actor_skeleton->GetProperty()->SetPointSize(1.0);
    actor_skeleton->GetProperty()->SetLineWidth(1.0);
    // actor->GetProperty()->SetOpacity(0.5);
    actor_skeleton->GetProperty()->ShadingOff();

    actor_skeleton->SetUserTransform(rotation);

    auto mapper_skeleton = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper_skeleton->SetInputConnection(line->GetOutputPort());
    mapper_skeleton->ScalarVisibilityOn();

    actor_skeleton->SetMapper(mapper_skeleton);
    m_renderer->AddActor(actor_skeleton);

    return;
  } else {
    // Create a circle
    auto polygonSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource->SetNumberOfSides(1000);
    polygonSource->SetRadius(HorizontalLengh / 2.0);

    auto circle = vtkSmartPointer<vtkRegularPolygonSource>::New();
    circle->SetNumberOfSides(1000);
    circle->SetRadius(HorizontalLengh / 2.0);
    circle->GeneratePolygonOff();

    auto actor = vtkActor::New();
    auto mapper = vtkPolyDataMapper::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(OpacityD);
    actor->GetProperty()->SetColor(color_sigmaD);
    actor->GetProperty()->ShadingOn();
    actor->SetPickable(false);

    auto rotation = vtkSmartPointer<vtkTransform>::New();
    rotation->PostMultiply();
    rotation->Scale(1.0, verticalLength / HorizontalLengh, 1.0);
    rotation->RotateX(90.0);
    rotation->RotateZ(angle + 90.0);
    actor->SetUserTransform(rotation);

    mapper->ScalarVisibilityOn();
    mapper->SetInputConnection(polygonSource->GetOutputPort());

    actor->GetProperty()->SetInterpolationToGouraud();
    m_renderer->AddActor(actor);

    auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper1->SetInputConnection(circle->GetOutputPort());
    ;
    auto actor1 = vtkSmartPointer<vtkActor>::New();

    actor1->GetProperty()->SetColor(color_sigmaD);
    actor1->GetProperty()->ShadingOff();
    actor1->SetUserTransform(rotation);
    actor1->SetMapper(mapper1);
    m_renderer->AddActor(actor1);
  }
}

void View3D::renderSigmaH() {
  if (!_isSigmaHVisible)
    return;

  if (typeSigmaH > 1) {
    // Create a circle
    auto polygonSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource->SetNumberOfSides(1000);
    polygonSource->SetRadius(HorizontalLengh / 2.0);

    // Visualize
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(polygonSource->GetOutputPort());
    ;
    auto actor = vtkSmartPointer<vtkActor>::New();

    actor->GetProperty()->SetOpacity(OpacityH);
    actor->GetProperty()->SetColor(color_sigmaH);
    actor->GetProperty()->ShadingOff();
    actor->SetMapper(mapper);
    m_renderer->AddActor(actor);

    // Visualize
    auto polygonSource1 = vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource1->SetNumberOfSides(1000);
    polygonSource1->SetRadius(HorizontalLengh / 2.0);
    polygonSource1->GeneratePolygonOff();

    auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper1->SetInputConnection(polygonSource1->GetOutputPort());
    ;
    auto actor1 = vtkSmartPointer<vtkActor>::New();

    // actor1->GetProperty()->SetOpacity(OpacityH);
    actor1->GetProperty()->SetColor(color_sigmaH);
    actor1->GetProperty()->ShadingOff();
    actor1->SetMapper(mapper1);
    m_renderer->AddActor(actor1);

    return;
  }

  auto cylinder = vtkSmartPointer<vtkCylinderSource>::New();
  cylinder->SetCenter(0.0, 0.0, 0.0);
  cylinder->SetRadius(HorizontalLengh * 0.5);
  cylinder->SetHeight(thicknessH);

  cylinder->SetResolution(100);
  cylinder->Update();

  auto mapper = vtkPolyDataMapper::New();
  mapper->ScalarVisibilityOff();
  mapper->SetInputData(cylinder->GetOutput());

  auto actor = vtkActor::New();

  actor->GetProperty()->SetColor(color_sigmaH);
  // actor->storeColor(whiteColor[0],whiteColor[1],whiteColor[2]);

  actor->GetProperty()->SetOpacity(OpacityH);
  actor->GetProperty()->ShadingOff();
  actor->SetMapper(mapper);
  actor->SetPickable(false);

  auto rotation = vtkSmartPointer<vtkTransform>::New();
  rotation->PostMultiply();
  rotation->RotateX(90.0);
  actor->SetUserTransform(rotation);

  m_renderer->AddActor(actor);

  mapper->Delete();
}

void View3D::renderHorizontalC2(double angle, double length, double color[3]) {
  // double thickness=discThickness;
  double r = length / 2.0;

  // disk 1
  auto trans0 = vtkSmartPointer<vtkTransform>::New();
  trans0->PostMultiply();
  trans0->Scale(discC2_scale, 1.0, discC2_scale);

  if (horizontalC2_type == 3) {
    trans0->RotateX(90.0);
    trans0->RotateZ(90.0);
  }
  trans0->Translate(0.0, r, 0.0);

  if (horizontalC2_type == 2)
    trans0->RotateY(90.0);

  trans0->RotateZ(angle - 90.0); // to (100)

  auto actor = vtkActor::New();
  actor->GetProperty()->SetColor(color);

  actor->GetProperty()->ShadingOn();
  actor->SetUserTransform(trans0);
  actor->GetProperty()->SetInterpolationToGouraud();

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(bfPolyData->GetOutputPort());
  mapper->ScalarVisibilityOff();
  actor->SetMapper(mapper);
  actor->SetPickable(false);

  m_renderer->AddActor(actor);

  // disk2
  auto trans1 = vtkSmartPointer<vtkTransform>::New();
  trans1->PostMultiply();
  trans1->Scale(discC2_scale, 1.0, discC2_scale);

  if (horizontalC2_type == 3) {
    trans1->RotateX(90.0);
    trans1->RotateZ(90.0);
  }

  trans1->Translate(0.0, -r, 0.0);

  if (horizontalC2_type == 2)
    trans1->RotateY(90.0);

  trans1->RotateZ(angle - 90.0); // to (100)

  auto actor1 = vtkActor::New();
  actor1->GetProperty()->SetColor(color);

  actor1->GetProperty()->ShadingOn();
  actor1->SetUserTransform(trans1);
  actor1->GetProperty()->SetInterpolationToGouraud();
  actor1->SetPickable(false);

  auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper1->SetInputConnection(bfPolyData->GetOutputPort());
  mapper1->ScalarVisibilityOff();
  actor1->SetMapper(mapper1);
  m_renderer->AddActor(actor1);

  // cyinder
  auto cylinder = vtkSmartPointer<vtkCylinderSource>::New();
  cylinder->SetCenter(0.0, 0.0, 0.0);
  cylinder->SetResolution(30);
  cylinder->SetHeight(length);
  cylinder->SetCapping(true);
  cylinder->SetRadius(C2_Radius);
  cylinder->Update();

  auto rotation2 = vtkSmartPointer<vtkTransform>::New();
  rotation2->PostMultiply();
  rotation2->RotateZ(angle - 90.0);

  auto actor2 = vtkActor::New();
  actor2->GetProperty()->SetColor(color);

  actor2->GetProperty()->ShadingOn();
  actor2->SetUserTransform(rotation2);
  actor2->SetPickable(false);
  actor2->GetProperty()->SetInterpolationToGouraud();

  auto mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper2->SetInputData(cylinder->GetOutput());
  actor2->SetMapper(mapper2);
  m_renderer->AddActor(actor2);
}

void View3D::renderHorizontalC2(unsigned int type, double angle,
                                double length) {
  if (!_isC2_1_Visible && type == 1)
    return;
  if (!_isC2_2_Visible && type == 2)
    return;

  if (type < 2)
    renderHorizontalC2(angle, length, color1_C2);
  else
    renderHorizontalC2(angle, length, color2_C2);
}

void View3D::renderVerticalC2(double angle, double length, double color[3]) {
  // vetical C2
  auto disk0 = vtkSmartPointer<vtkCylinderSource>::New();
  disk0->SetCenter(0.2, 0.0, 0.0);
  disk0->SetResolution(80);
  disk0->SetHeight(discThickness);
  disk0->SetRadius(0.3);
  disk0->SetCapping(true);
  disk0->Update();

  auto disk1 = vtkSmartPointer<vtkCylinderSource>::New();
  disk1->SetCenter(-0.2, 0.0, 0.0);
  disk1->SetResolution(80);
  disk1->SetHeight(discThickness);
  disk1->SetRadius(0.3);
  disk1->SetCapping(true);
  disk1->Update();

  vtkPolyDataBooleanFilter *bf = vtkPolyDataBooleanFilter::New();
  bf->SetInputData(0, disk0->GetOutput());
  bf->SetInputData(1, disk1->GetOutput());
  bf->SetOperModeToIntersection();
  bf->Update();

  auto trans = vtkSmartPointer<vtkTransform>::New();
  trans->PostMultiply();
  trans->Scale(discC2_scale, discC2_scale, discC2_scale);
  trans->RotateX(90.0);
  trans->RotateZ(angle);
  trans->Translate(0.0, 0.0, length * 0.5);

  auto actor = vtkActor::New();
  actor->GetProperty()->SetColor(color_Cn);
  actor->GetProperty()->ShadingOn();
  actor->SetUserTransform(trans);
  actor->GetProperty()->SetInterpolationToGouraud();
  actor->SetPickable(false);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(bf->GetOutputPort());
  mapper->ScalarVisibilityOff();
  actor->SetMapper(mapper);

  m_renderer->AddActor(actor);

  auto actor1 = vtkActor::New();
  actor1->GetProperty()->SetColor(color_Cn);
  actor1->GetProperty()->ShadingOn();
  actor1->SetUserTransform(trans);
  actor1->GetProperty()->SetInterpolationToGouraud();
  actor1->SetPickable(false);

  auto trans1 = vtkSmartPointer<vtkTransform>::New();
  trans1->PostMultiply();
  trans1->Scale(discC2_scale, discC2_scale, discC2_scale);

  trans1->RotateX(90.0);
  trans1->RotateZ(angle);
  trans1->Translate(0.0, 0.0, -length * 0.5);

  auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper1->SetInputConnection(bf->GetOutputPort());
  mapper1->ScalarVisibilityOff();
  actor1->SetMapper(mapper1);
  actor1->SetUserTransform(trans1);
  m_renderer->AddActor(actor1);

  auto cylinderAxis = vtkSmartPointer<vtkCylinderSource>::New();
  cylinderAxis->SetCenter(0.0, 0.0, 0.0);
  cylinderAxis->SetResolution(20);
  cylinderAxis->SetHeight(C2_Length);
  cylinderAxis->SetRadius(Cn_Radius);
  cylinderAxis->SetCapping(true);
  cylinderAxis->Update();

  auto rotation1 = vtkSmartPointer<vtkTransform>::New();
  rotation1->PostMultiply();
  rotation1->RotateX(90.0);

  auto actorAxis = vtkActor::New();
  actorAxis->GetProperty()->SetColor(color_Cn);
  actorAxis->GetProperty()->ShadingOn();
  actorAxis->GetProperty()->SetInterpolationToGouraud();
  actorAxis->SetUserTransform(rotation1);
  actorAxis->SetPickable(false);

  auto mapperAxis = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperAxis->SetInputData(cylinderAxis->GetOutput());
  actorAxis->SetMapper(mapperAxis);
  m_renderer->AddActor(actorAxis);
  ;
}

void View3D::renderCubeSkeleton(double length) {
  if (outLineType < 2)
    return;
  /*
   auto box =  vtkSmartPointer<vtkBox>::New();

   double l=length*0.5;
   box->SetXMin(-l,-l,-l);
   box->SetXMax(l,l,l);

   auto actor = vtkActor::New();
   actor->GetProperty()->SetColor(white);
   actor->GetProperty()->ShadingOn();
   actor->GetProperty()->SetLineStipplePattern(0xf0f0);
   actor->SetPickable(false);
   actor->GetProperty()->SetPointSize(1.0);
   actor->GetProperty()->SetLineWidth(1.5);

   auto mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
   mapper->SetInputData(box->GetOutput());

   mapper->ScalarVisibilityOn();
   actor->SetMapper(mapper);
   m_renderer->AddActor(actor);
  */

  double l = length * 0.5;

  double p1[3] = {-l, -l, l};
  double p2[3] = {l, -l, l};
  double p3[3] = {l, l, l};
  double p4[3] = {-l, l, l};

  double p5[3] = {-l, -l, -l};
  double p6[3] = {l, -l, -l};
  double p7[3] = {l, l, -l};
  double p8[3] = {-l, l, -l};

  auto points = vtkSmartPointer<vtkPoints>::New();

  points->InsertNextPoint(p1);
  points->InsertNextPoint(p2);
  points->InsertNextPoint(p3);
  points->InsertNextPoint(p4);
  points->InsertNextPoint(p1);
  points->InsertNextPoint(p5);
  points->InsertNextPoint(p6);
  points->InsertNextPoint(p7);
  points->InsertNextPoint(p8);
  points->InsertNextPoint(p5);

  double color[3];
  for (unsigned int i = 0; i < 3; i++)
    color[i] = white[i];

  auto line = vtkSmartPointer<vtkLineSource>::New();
  line->SetPoints(points);
  line->Update();

  auto actor_skeleton = vtkActor::New();
  actor_skeleton->GetProperty()->SetColor(color);
  actor_skeleton->GetProperty()->ShadingOff();

  actor_skeleton->GetProperty()->SetLineStipplePattern(0xf0f0);
  actor_skeleton->GetProperty()->SetLineStippleRepeatFactor(1);

  actor_skeleton->SetPickable(false);
  actor_skeleton->GetProperty()->SetPointSize(1.0);
  actor_skeleton->GetProperty()->SetLineWidth(1.5);

  auto mapper_skeleton = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper_skeleton->SetInputConnection(line->GetOutputPort());
  mapper_skeleton->ScalarVisibilityOn();
  actor_skeleton->SetMapper(mapper_skeleton);
  m_renderer->AddActor(actor_skeleton);

  //--------------------------------------------
  auto actor_line1 = vtkActor::New();
  actor_line1->GetProperty()->SetColor(color);
  actor_line1->GetProperty()->ShadingOn();
  actor_line1->GetProperty()->SetLineStipplePattern(0xf0f0);
  actor_line1->SetPickable(false);
  actor_line1->GetProperty()->SetPointSize(1.0);
  actor_line1->GetProperty()->SetLineWidth(1.5);

  auto line1 = vtkSmartPointer<vtkLineSource>::New();
  line1->SetPoint1(p2);
  line1->SetPoint2(p6);
  auto mapper_line1 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper_line1->SetInputConnection(line1->GetOutputPort());
  mapper_line1->ScalarVisibilityOn();
  actor_line1->SetMapper(mapper_line1);
  m_renderer->AddActor(actor_line1);
  //----------------------------------------------
  auto actor_line2 = vtkActor::New();
  actor_line2->GetProperty()->SetColor(color);
  actor_line2->GetProperty()->ShadingOn();
  actor_line2->GetProperty()->SetLineStipplePattern(0xf0f0);
  actor_line2->SetPickable(false);
  actor_line2->GetProperty()->SetPointSize(1.0);
  actor_line2->GetProperty()->SetLineWidth(1.5);

  auto line2 = vtkSmartPointer<vtkLineSource>::New();
  line2->SetPoint1(p3);
  line2->SetPoint2(p7);
  auto mapper_line2 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper_line2->SetInputConnection(line2->GetOutputPort());
  mapper_line2->ScalarVisibilityOn();
  actor_line2->SetMapper(mapper_line2);
  m_renderer->AddActor(actor_line2);
  //----------------------------------------------
  auto actor_line3 = vtkActor::New();
  actor_line3->GetProperty()->SetColor(color);
  actor_line3->GetProperty()->ShadingOn();
  actor_line3->GetProperty()->SetLineStipplePattern(0xf0f0);
  actor_line3->SetPickable(false);
  actor_line3->GetProperty()->SetPointSize(1.0);
  actor_line3->GetProperty()->SetLineWidth(1.5);

  auto line3 = vtkSmartPointer<vtkLineSource>::New();
  line3->SetPoint1(p4);
  line3->SetPoint2(p8);
  auto mapper_line3 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper_line3->SetInputConnection(line3->GetOutputPort());
  mapper_line3->ScalarVisibilityOn();
  actor_line3->SetMapper(mapper_line3);
  m_renderer->AddActor(actor_line3);
}

void View3D::renderLine(double p0[3], double p1[3], float width,
                        double color[3], vtkTransform *transform) {
  auto lineSource = vtkSmartPointer<vtkLineSource>::New();
  lineSource->SetPoint1(p0);
  lineSource->SetPoint2(p1);
  lineSource->Update();

  auto lineActor = vtkActor::New();
  auto lineMapper = vtkPolyDataMapper::New();
  lineMapper->SetInputConnection(lineSource->GetOutputPort());

  lineActor->SetMapper(lineMapper);
  lineActor->GetProperty()->SetColor(color[0], color[1], color[2]);
  lineActor->storeColor(color[0], color[1], color[2]);
  lineActor->GetProperty()->SetLineWidth(width);
  lineActor->SetUserTransform(transform);

  m_renderer->AddActor(lineActor);
}

void View3D::renderLineCenter(vector3 beg, vector3 end, float width,
                              double color[3]) {
  vector3 beg0 = beg - center;
  vector3 end0 = end - center;
  renderLine(beg0, end0, width, color);
}

void View3D::renderLine(vector3 beg, vector3 end, float width,
                        double color[3]) {
  double p0[3], p1[3];

  for (unsigned int i = 0; i < 3; i++) {
    p0[i] = beg[i];
    p1[i] = end[i];
  }

  auto lineSource = vtkSmartPointer<vtkLineSource>::New();
  lineSource->SetPoint1(p0);
  lineSource->SetPoint2(p1);
  lineSource->Update();

  auto lineActor = vtkActor::New();
  auto lineMapper = vtkPolyDataMapper::New();
  lineMapper->SetInputConnection(lineSource->GetOutputPort());

  lineActor->SetMapper(lineMapper);
  lineActor->GetProperty()->SetColor(color[0], color[1], color[2]);
  lineActor->storeColor(color[0], color[1], color[2]);
  lineActor->GetProperty()->SetLineWidth(width);

  /*
  lineActor->GetProperty()->SetLineStipplePattern(0xf0f0);
  lineActor->GetProperty()->SetLineStippleRepeatFactor(2);
  lineActor->GetProperty()->SetPointSize(2);
*/

  m_renderer->AddActor(lineActor);
}

void View3D::renderTubeCenter(vector3 beg, vector3 end, double radius,
                              double color[3]) {
  renderTube(beg, end, center, radius, color);
}

void View3D::renderTube(vector3 beg, vector3 end, vector3 center0,
                        double radius, double color[3]) {
  beg = beg - center0;
  end = end - center0;
  renderTube(beg, end, radius, color);
}

void View3D::renderTube(vector3 beg, vector3 end, double radius,
                        double color[3]) {
  auto line = vtkSmartPointer<vtkLineSource>::New();
  line->SetPoint1(beg[0], beg[1], beg[2]);
  line->SetPoint2(end[0], end[1], end[2]);

  auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
  tubeFilter->SetInputConnection(line->GetOutputPort());
  tubeFilter->SetRadius(radius);
  tubeFilter->SetNumberOfSides(20);

  auto actor = vtkActor::New();
  auto mapper = vtkPolyDataMapper::New();
  actor->GetProperty()->SetColor(color[0], color[1], color[2]);
  actor->storeColor(color[0], color[1], color[2]);
  actor->SetMapper(mapper);
  actor->setUnitCellBoxId(0);
  mapper->SetInputConnection(tubeFilter->GetOutputPort());
  m_renderer->AddActor(actor);
}

void View3D::renderArcTube(vector3 beg, vector3 end, vector3 center,
                           double color[3], unsigned int id) {
  auto line = vtkSmartPointer<vtkLineSource>::New();
  double radius = (center - end).length() - 0.2;

  vector3 v1 = beg - center, v2 = end - center;

  double angle = vectorAngle(v1, v2);
  vector3 norm = cross(v1, v2);

  vector3 v = 0.8 * v1.length() * v1 + center;

  unsigned int NPoints = 10;
  matrix3x3 m;
  m.SetupRotateMatrix(norm, angle / 10.);
  for (unsigned int i = 0; i < NPoints; i++) {
    if (i == 0) {
      line->SetPoint1(v[0], v[1], v[2]);
      continue;
    }
    v *= m;
    line->SetPoint1(v[0], v[1], v[2]);
  }

  auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
  tubeFilter->SetInputConnection(line->GetOutputPort());
  tubeFilter->SetRadius(radius);
  tubeFilter->SetNumberOfSides(20);

  auto actor = vtkActor::New();
  auto mapper = vtkPolyDataMapper::New();
  actor->GetProperty()->SetColor(color[0], color[1], color[2]);
  actor->storeColor(color[0], color[1], color[2]);
  actor->SetMapper(mapper);
  actor->setUnitCellBoxId(0);
  mapper->SetInputConnection(tubeFilter->GetOutputPort());
  m_renderer->AddActor(actor);

  actor->setBondId(id);
}

void View3D::addDataSet(vtkSmartPointer<vtkDataSet> dataSet) {
  // Actor
  auto actor = vtkSmartPointer<vtkActor>::New();

  // Mapper
  auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
  mapper->SetInputData(dataSet);
  actor->SetMapper(mapper);

  m_renderer->AddActor(actor);
  // m_renderer->ResetCamera(dataSet->GetBounds());
  //  renderVTK();
}

void View3D::removeAllActors() {
  vtkActorCollection *actorCollection = m_renderer->GetActors();
  unsigned int numActors = actorCollection->GetNumberOfItems();

  actorCollection->InitTraversal();
  for (unsigned int i = 0; i < numActors; ++i) {
    vtkActor *actor = actorCollection->GetNextActor();
    if (actor == nullptr)
      continue;
    m_renderer->RemoveActor(actor);
  }

  actorCollection->RemoveAllItems();
}

void View3D::removeDynamicLineActor() {
  vtkActorCollection *actorCollection = m_renderer->GetActors();
  actorCollection->InitTraversal();
  unsigned int _numActors = actorCollection->GetNumberOfItems();

  for (unsigned int i = 0; i < _numActors; ++i) {
    vtkActor *actor = actorCollection->GetNextActor();
    if (actor == nullptr)
      continue;
    if (actor->MOId() < -9) {
      m_renderer->RemoveActor(actor);
      break;
    }
  }
}

void View3D::removeAtomLabelActor(unsigned int idx) {
  vtkActorCollection *actorCollection = m_renderer->GetActors();
  actorCollection->InitTraversal();
  unsigned int _numActors = actorCollection->GetNumberOfItems();

  cout << "begin to remove actor inside removeAtomLabelActor" << idx + 1
       << endl;

  for (unsigned int i = 0; i < _numActors; ++i) {
    vtkActor *actor = actorCollection->GetNextActor();
    if (actor == nullptr || actor->getAtomLabelId() < 0 ||
        actor->getAtomLabelId() > 100000)
      continue;

    if (actor->getAtomLabelId() != idx)
      continue;
    m_renderer->RemoveActor(actor);
    break;
  }
}

unsigned int View3D::numActors() {
  vtkActorCollection *actorCollection = m_renderer->GetActors();
  actorCollection->InitTraversal();
  return actorCollection->GetNumberOfItems();
}

void View3D::removeLastActor() {
  vtkActor *actor = m_renderer->GetActors()->GetLastActor();

  if (actor != nullptr)
    m_renderer->RemoveActor(actor);
}

void View3D::zoomToExtent() {
  // Zoom to extent of last added actor
  vtkSmartPointer<vtkActor> actor = m_renderer->GetActors()->GetLastActor();
  if (actor != nullptr)
    m_renderer->ResetCamera(actor->GetBounds());
}

void View3D::setMolTypeStick() {
  isStick = true;
  isBallStick = isLine = false;
}

void View3D::setMolTypeBallStick() {
  isBallStick = true;
  isStick = isLine = false;
}

void View3D::setMolTypeLine() {
  isLine = true;
  isStick = isBallStick = false;
}

void View3D::setColorSigmaV(vector3 v) {
  for (unsigned int i = 0; i < 3; i++)
    color_sigmaV[i] = v[i];
}

vector3 View3D::getSigmaVColor() {
  vector3 v;
  v.Set(color_sigmaV[0], color_sigmaV[1], color_sigmaV[2]);
  // cout << " getSigmaVColor()"<<endl;
  // cout <<  v<<endl;
  return v;
}

void View3D::setColorSigmaD(vector3 v) {
  color_sigmaD[0] = v[0];
  color_sigmaD[1] = v[1];
  color_sigmaD[2] = v[2];
}

void View3D::setColorSigmaH(vector3 v) {
  color_sigmaH[0] = v[0];
  color_sigmaH[1] = v[1];
  color_sigmaH[2] = v[2];
}

vector3 View3D::getSigmaDColor() {
  vector3 v(color_sigmaD[0], color_sigmaD[1], color_sigmaD[2]);
  return v;
}
vector3 View3D::getSigmaHColor() {
  vector3 v(color_sigmaH[0], color_sigmaH[1], color_sigmaH[2]);
  return v;
}

vector3 View3D::getColorCn() {
  vector3 v(color_Cn[0], color_Cn[1], color_Cn[2]);
  return v;
}

vector3 View3D::getColorIn() {
  vector3 v(color_In[0], color_In[1], color_In[2]);
  return v;
}
vector3 View3D::getColorC2_1() {
  vector3 v(color1_C2[0], color1_C2[1], color1_C2[2]);
  return v;
}
vector3 View3D::getColorC2_2() {
  vector3 v(color2_C2[0], color2_C2[1], color2_C2[2]);
  return v;
}
vector3 View3D::getColorC3() {
  vector3 v(color_C3[0], color_C3[1], color_C3[2]);
  return v;
}

void View3D::setColorSigmaV(double c[3]) {
  for (unsigned int i = 0; i < 3; i++)
    color_sigmaV[i] = c[i];
}

void View3D::setColorSigmaD(double c[3]) {
  for (unsigned int i = 0; i < 3; i++)
    color_sigmaD[i] = c[i];
}

void View3D::setColorSigmaH(double c[3]) {
  for (unsigned int i = 0; i < 3; i++)
    color_sigmaH[i] = c[i];
}
void View3D::setColorCn(double c[3]) {
  for (unsigned int i = 0; i < 3; i++)
    color_Cn[i] = c[i];
}

void View3D::setColorI2n(double c[3]) {
  for (unsigned int i = 0; i < 3; i++)
    color_In[i] = c[i];
}

void View3D::setColorC2_1(double c[3]) {
  for (unsigned int i = 0; i < 3; i++)
    color1_C2[i] = c[i];
}
void View3D::setColorC2_2(double c[3]) {
  for (unsigned int i = 0; i < 3; i++)
    color2_C2[i] = c[i];
}

void View3D::setColorCn(vector3 c) {
  for (unsigned int i = 0; i < 3; i++)
    color_Cn[i] = c[i];
}

void View3D::setColorI2n(vector3 c) {
  for (unsigned int i = 0; i < 3; i++)
    color_In[i] = c[i];
}

void View3D::setColorC2_1(vector3 c) {
  for (unsigned int i = 0; i < 3; i++)
    color1_C2[i] = c[i];
}

void View3D::setColorC2_2(vector3 c) {
  for (unsigned int i = 0; i < 3; i++)
    color2_C2[i] = c[i];
}

// selection
void View3D::buildAtomLabel(vtkActor *actor) {
  return;

  if (actor == nullptr)
    return;

  unsigned int idx = actor->AtomId();
  QString label = QString::number(idx + 1);

  // cout << idx+1 << "inside showAtomLabel(vtkActor * actor)"<<endl;

  auto textSource = vtkSmartPointer<vtkTextSource>::New();
  textSource->SetText(label.toStdString().c_str());
  textSource->SetForegroundColor(0.0, 0.0, 1.0);
  textSource->BackingOff();
  textSource->Update();

  auto textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  textMapper->SetInputConnection(textSource->GetOutputPort());

  // auto textActor =  vtkSmartPointer<vtkFollower>::New();
  auto labelActor = vtkFollower::New();
  labelActor->SetPickable(false);
  labelActor->SetMapper(textMapper);
  labelActor->SetScale(0.02, 0.02, 0.02);

  double radius = 0.1;
  if (mol)
    radius = 0.1 * mol->getAtomRadiusbyIndex(idx);

  double *x = actor->GetPosition();
  labelActor->AddPosition(x[0] - radius / 2, x[1] - radius / 2,
                          x[2] + radius * 2.0);
  labelActor->SetCamera(camera);
  labelActor->setAtomLabelId(idx);
  labelActor->setAtomId(-1);
  m_renderer->AddActor(labelActor);

  // updateSelectedAtomListFromAtomActors();
  m_renderer->GetRenderWindow()->Render();
}

void View3D::releaseAtomLabelActors() { atomLabelActors->RemoveAllItems(); }

void View3D::releaseSingleAtomActor(vtkActor *a) {
  a->recoverInitialColor();
  unsigned int _numActors = selectedAtomActors->GetNumberOfItems();
  if (_numActors < 0 || _numActors > 10000)
    return;

  if (_numActors == 1)
    selectedAtomActors = vtkSmartPointer<vtkActorCollection>::New();
  else {
    selectedAtomActors->InitTraversal();
    for (int i = 0; i < _numActors; ++i) {
      vtkActor *actor = selectedAtomActors->GetNextActor();
      if (actor == nullptr)
        continue;
      if (actor->AtomId() != a->AtomId())
        continue;
      selectedAtomActors->RemoveItem(i);
      break;
    }
  }

  removeAtomLabelActor(a->AtomId());
  m_renderer->GetRenderWindow()->Render();
  updateSelectedAtomListFromAtomActors();
}

void View3D::releaseSingleBondActor(vtkActor *a) {
  a->recoverInitialColor();
  unsigned int _numActors = selectedBondActors->GetNumberOfItems();
  if (_numActors < 0 || _numActors > 10000)
    return;

  if (_numActors == 1)
    selectedBondActors = vtkSmartPointer<vtkActorCollection>::New();
  else {
    selectedBondActors->InitTraversal();
    for (int i = 0; i < _numActors; ++i) {
      vtkActor *actor = selectedBondActors->GetNextActor();
      if (actor == nullptr)
        continue;

      if (actor->AtomId() != a->AtomId())
        continue;
      if (actor->AtomId1() != a->AtomId1())
        continue;
      selectedBondActors->RemoveItem(i);
      break;
    }
  }

  removeAtomLabelActor(a->AtomId());
  m_renderer->GetRenderWindow()->Render();
  // updateSelectedBonds();
}

void View3D::removefromBondActorsList(vtkActor *a) {
  unsigned int _numActors = selectedBondActors->GetNumberOfItems();
  if (_numActors < 0 || _numActors > 100000)
    return;

  if (_numActors == 1)
    selectedBondActors->RemoveAllItems();
  else {
    selectedBondActors->InitTraversal();
    for (unsigned int i = 0; i < _numActors; i++) {
      vtkActor *actor = selectedBondActors->GetNextActor();
      if (actor != nullptr) {
        if (actor->Id() != a->Id())
          continue;
        actor->recoverInitialColor();
        selectedBondActors->RemoveItem(i);
        i--;
        _numActors--;
      }
    }
  }

  m_renderer->GetRenderWindow()->Render();

  // recover all
  for (unsigned int i = 0; i < mol->NumBonds(); i++)
    mol->getBondbyIndex(i)->setSelected(false);
  selectedBondList.clear();
}

void View3D::removefromAtomActorsList(vtkActor *a) {
  a->recoverInitialColor();

  unsigned int _numActors = selectedAtomActors->GetNumberOfItems();
  if (_numActors < 0 || _numActors > 100000)
    return;

  if (_numActors == 1)
    selectedAtomActors->RemoveAllItems();
  else {
    selectedAtomActors->InitTraversal();
    for (unsigned int i = 0; i < _numActors; ++i) {
      vtkActor *actor = selectedAtomActors->GetNextActor();
      if (actor == nullptr)
        continue;
      if (actor->AtomId() != a->AtomId())
        continue;
      selectedAtomActors->RemoveItem(i);
      break;
    }
  }

  removeAtomLabelActor(a->AtomId());
  m_renderer->GetRenderWindow()->Render();

  // recover all
  for (unsigned int i = 0; i < mol->NumAtoms(); i++)
    mol->getAtombyIndex(i)->setSelected(false);
  selectedAtomList.clear();
}

void View3D::removefromRingActorsList(vtkActor *a) {
  // cout << " inside removefromRingActorsList"<<endl;

  a->recoverInitialColor();

  unsigned int _numActors = selectedRingActors->GetNumberOfItems();
  if (_numActors < 0 || _numActors > 100000)
    return;

  if (_numActors == 1)
    selectedRingActors->RemoveAllItems();
  else {
    selectedRingActors->InitTraversal();
    for (unsigned int i = 0; i < _numActors; ++i) {
      vtkActor *actor = selectedRingActors->GetNextActor();
      if (actor == nullptr)
        continue;
      if (actor->getRingId() != a->getRingId())
        continue;
      selectedRingActors->RemoveItem(i);
      break;
    }
  }

  m_renderer->GetRenderWindow()->Render();
}

void View3D::updateSelectedRingListFromRingActors() {
  // reset all
  for (unsigned int i = 0; i < mol->numRings(); i++)
    mol->getRingbyId(i)->isSelected = false;
  selectedRingList.clear();

  unsigned int numActors = selectedRingActors->GetNumberOfItems();
  if (numActors < 0 || numActors > 100000)
    return;

  selectedRingActors->InitTraversal();
  for (unsigned int i = 0; i < numActors; i++) {
    vtkActor *actor = selectedRingActors->GetNextActor();
    if (actor == nullptr)
      continue;
    selectedRingList.push_back(actor->getRingId());
    mol->getRingbyId(actor->getRingId())->isSelected = true;
  }

  if (numActors <= 1)
    return;
  sort(selectedRingList.begin(), selectedRingList.end());
  vector<uint>::iterator iter =
      unique(selectedRingList.begin(), selectedRingList.end());
  selectedRingList.erase(iter, selectedRingList.end());
}

vector3 View3D::getAtomPos(unsigned int id) {
  return mol->getAtomPosbyIndex(id);
}

// update from selected Actors
void View3D::updateSelectedAtomListFromAtomActors() {
  // recover all
  unsigned int i = 0;
  for (i = 0; i < mol->NumAtoms(); i++)
    mol->getAtombyIndex(i)->setSelected(false);

  // reset
  selectedAtomList.clear();

  unsigned int numActors = selectedAtomActors->GetNumberOfItems();

  // origin_SelectedAtoms=VZero;
  vector<vector3> v;
  origin_SelectedAtoms = VZero;
  length_SelectedAtoms = 0.0;

  if (numActors < 0 || numActors > 10000000)
    return;

  selectedAtomActors->InitTraversal();
  for (i = 0; i < numActors; i++) {
    vtkActor *actor = selectedAtomActors->GetNextActor();
    if (actor == nullptr)
      continue;
    selectedAtomList.push_back(actor->AtomId());
    mol->getAtombyIndex(actor->AtomId())->setSelected(true);

    origin_SelectedAtoms += mol->getAtomPosbyIndex(actor->AtomId());
    v.push_back(mol->getAtomPosbyIndex(actor->AtomId()));
  }

  origin_SelectedAtoms /= numActors;

  for (i = 0; i < numActors; i++)
    v[i] = v[i] - origin_SelectedAtoms;

  for (i = 0; i < numActors; i++) {
    double l = v[i].length();
    if (length_SelectedAtoms < l)
      length_SelectedAtoms = l;
  }

  // displaySelectedAtomList();
}

// update from selected Actors
void View3D::updateSelectedBondListFromBondActors() {
  // reset all
  for (unsigned int i = 0; i < mol->NumBonds(); i++)
    mol->getBondbyIndex(i)->setSelected(false);

  // reset
  selectedBondList.clear();
  unsigned int _numActors = selectedBondActors->GetNumberOfItems();
  if (_numActors < 1)
    return;

  // cout << _numActors << " inside updateSelectedBondList"<< endl;

  selectedBondActors->InitTraversal();
  for (unsigned int i = 0; i < _numActors; i++) {
    vtkActor *actor = selectedBondActors->GetNextActor();
    if (actor == nullptr)
      continue;

    storeSelectBondList(actor->AtomId(), actor->AtomId1());

    mol->getBondbyIndex(actor->BondId())->setSelected(true);
  }
  displaySelectedBondList();
}

unsigned int View3D::getBondOrderbyId(unsigned int id) {
  return mol->getBondOrderbyIndex(id);
}

// help function
void View3D::displaySelectedBondList() {
  if (selectedBondList.size() < 1)
    return;

  cout << endl;
  cout << "Selected Bonds: " << selectedBondList.size() << endl;
  for (unsigned int i = 0; i < selectedBondList.size(); i++) {
    cout << selectedBondList[i][0] << " :";
    cout << selectedBondList[i][1] + 1 << " ";
    cout << selectedBondList[i][2] + 1 << endl;
  }
  cout << endl;
}

// help function
void View3D::storeSelectBondList(unsigned int id0, unsigned int id1) {
  unsigned int id_bond;
  HBond *b = mol->getBondIndexbyAtomIndex(id0, id1, id_bond); // obtain  _idbond
  if (b == nullptr)
    return;

  bool isExisted = false;
  if (selectedBondList.size() > 0) {
    for (unsigned int j = 0; j < selectedBondList.size(); j++) {
      if (id_bond == selectedBondList[j][0]) {
        isExisted = true;
        break;
      }
    }
  }
  if (isExisted)
    return;

  vector<unsigned int> v;
  v.push_back(id_bond);
  if (id0 < id1) {
    v.push_back(id0);
    v.push_back(id1);
  } else {
    v.push_back(id1);
    v.push_back(id0);
  }
  selectedBondList.push_back(v);
}

void View3D::uniqueSelectAtomList() {
  sort(selectedAtomList.begin(), selectedAtomList.end());
  vector<uint>::iterator iter =
      unique(selectedAtomList.begin(), selectedAtomList.end());
  selectedAtomList.erase(iter, selectedAtomList.end());
}

bool View3D::hasPicked(vtkActor *actor) {
  unsigned int size = selectedAtomList.size();
  if (size < 1)
    return false;

  for (unsigned int i = 0; i < size; i++) {
    if (selectedAtomList[i] == actor->AtomId())
      return true;
  }
  return false;
}

unsigned int View3D::getBondOrderSelectedById(unsigned int id) {
  unsigned int beg, end;
  getSelectedBondIds(id, beg, end);
  return mol->getBondOrderbyAtomIndex(beg, end);
}

void View3D::getSelectedBondIds(unsigned int id, unsigned int &id1,
                                unsigned int &id2) {
  id1 = selectedBondList[id][1];
  id2 = selectedBondList[id][2];
}

unsigned int View3D::getSelectedAtomId(unsigned int id) {
  if (selectedAtomList.size() < 1)
    return 10000001;

  if (id >= selectedAtomList.size())
    return 10000001;

  return selectedAtomList[id];
}

unsigned int View3D::getSelectedRingId(unsigned int id) {
  if (selectedRingList.size() < 1)
    return 10000001;
  if (id >= selectedRingList.size())
    return 10000001;

  return selectedRingList[id];
}

unsigned int View3D::numSelectedAtoms() {
  unsigned int n = selectedAtomList.size();
  return n;
}

unsigned int View3D::numSelectedBonds() { return selectedBondList.size(); }

unsigned int View3D::numSelectedRings() { return selectedRingList.size(); }

bool View3D::isAtomPicked(unsigned int id) {
  unsigned int n = selectedAtomList.size();
  if (n < 1)
    return false;

  for (unsigned int i = 0; i < n; i++)
    if (selectedAtomList[i] == id)
      return true;

  return false;
}

// remove label-actor
void View3D::removeAllAtomLabelActors() {
  unsigned int n = selectedAtomList.size();
  if (n < 1)
    return;

  vtkActorCollection *actorCollection = m_renderer->GetActors();
  actorCollection->InitTraversal();
  unsigned int numActors = actorCollection->GetNumberOfItems();

  unsigned int times = 0;
  for (int i = 0; i < numActors; i++) {
    vtkActor *actor = actorCollection->GetNextActor();
    if (actor == nullptr || actor->getAtomLabelId() < 0 ||
        actor->getAtomLabelId() > 100000)
      continue;

    for (unsigned int j = 0; j < n; j++) {
      if (actor->getAtomLabelId() != getSelectedAtomId(j))
        continue;
      m_renderer->RemoveActor(actor);
      times++;
      break;
    }

    if (times == n)
      break;
  }
}

void View3D::releaseAllActors2() {
  releaseAllActors();
  releaseInteractorPickedActor();
  removeDynamicLineActor();

  lastAtomActor = nullptr;
  refresh();
}

void View3D::releaseAllActors() {
  releaseSelectedAtomActors();
  releaseSelectedBondActors();
  releaseSelectedRingActors();

  lastAtomActor = nullptr;
  refresh();
}

void View3D::releaseInteractorPickedActor() {
  actorInteractorA->releasePickedActor();
}

void View3D::clearSelectedAtomList() {
  removeAllAtomLabelActors();
  selectedAtomList.clear();
  updateSelectedAtomListFromAtomActors();
}

void View3D::clearSelectedBondList() {
  selectedBondList.clear();
  updateSelectedBondListFromBondActors();
}
void View3D::clearSelectedRingList() {
  selectedRingList.clear();
  updateSelectedRingListFromRingActors();
}

void View3D::releaseSelectedBondActors() {
  unsigned int numActors = selectedBondActors->GetNumberOfItems();
  if (numActors < 1 || numActors > 10000001) {
    return;
  }

  cout << "\n\nbegin release all selected atom actors:" << endl;
  cout << "there are " << numActors << " actors!" << endl;

  if (numActors == 1) {
    selectedBondActors->GetLastItem()->recoverInitialColor();
  } else {
    selectedBondActors->InitTraversal();
    for (unsigned int i = 0; i < numActors; ++i) {
      selectedBondActors->GetNextActor()->recoverInitialColor();
    }
  }

  selectedBondActors->RemoveAllItems();

  m_renderer->GetRenderWindow()->Render();

  clearSelectedBondList();
  selectedBondList.clear();
}

void View3D::releaseSelectedAtomActors() {
  unsigned int numActors = selectedAtomActors->GetNumberOfItems();
  if (numActors < 1 || numActors > 1000001) {
    return;
  }

  // cout << "\n\nbegin to release all selected atom actors:"<<endl;
  //  cout << "there are " <<numActors<< " actors!"<<endl;

  removeAllAtomLabelActors();

  selectedAtomList.clear();
  if (numActors == 1) {
    selectedAtomActors->GetLastItem()->recoverInitialColor();
  }

  if (numActors > 1 && numActors < 10000) {
    selectedAtomActors->InitTraversal();
    for (unsigned int i = 0; i < numActors; ++i) {
      selectedAtomActors->GetNextActor()->recoverInitialColor();
    }
  }

  selectedAtomActors->RemoveAllItems();

  m_renderer->GetRenderWindow()->Render();

  clearSelectedAtomList();
}

void View3D::releaseSelectedRingActors() {
  unsigned int numActors = selectedRingActors->GetNumberOfItems();
  if (numActors < 1 || numActors > 1000001) {
    return;
  }

  cout << "\n\nbegin to release all selected ring actors:" << endl;
  cout << "there are " << numActors << " actors!" << endl;

  selectedRingList.clear();
  if (numActors == 1) {
    selectedRingActors->GetLastItem()->recoverInitialColor();
  }

  if (numActors > 1 && numActors < 10000) {
    selectedRingActors->InitTraversal();
    for (unsigned int i = 0; i < numActors; ++i) {
      selectedRingActors->GetNextActor()->recoverInitialColor();
    }
  }

  selectedRingActors->RemoveAllItems();

  m_renderer->GetRenderWindow()->Render();

  clearSelectedRingList();
}

double View3D::getSelectedAtomXbyIndex(unsigned int id) {
  if (id < selectedAtomList.size())
    return mol->getAtomXbyIndex(selectedAtomList[id]);
  return 0.0;
}
double View3D::getSelectedAtomYbyIndex(unsigned int id) {
  if (id < selectedAtomList.size())
    return mol->getAtomYbyIndex(selectedAtomList[id]);
  return 0.0;
}
double View3D::getSelectedAtomZbyIndex(unsigned int id) {
  if (id < selectedAtomList.size())
    return mol->getAtomZbyIndex(selectedAtomList[id]);
  return 0.0;
}

vector3 View3D::getSelectedAtomPosbyIndex(unsigned int id) {
  if (id < selectedAtomList.size())
    return mol->getAtomPosbyIndex(selectedAtomList[id]);
  return VZero;
}

void View3D::setSelectedAtomPosbyIndex(unsigned int id, double x, double y,
                                       double z) {
  if (id >= selectedAtomList.size())
    return;

  mol->setAtomPos(selectedAtomList[id], x, y, z);
}

void View3D::displaySelectedAtomList() {
  unsigned int size = selectedAtomList.size();
  if (size < 1)
    return;

  cout << "There are " << size << " selected atoms: ";
  for (unsigned int i = 0; i < size; i++)
    cout << selectedAtomList[i] + 1 << " ";
  cout << endl << endl;
}
void View3D::displaySelectedRingList() {
  unsigned int size = selectedRingList.size();
  if (size < 1)
    return;

  cout << "There are " << size << " selected atoms: ";
  for (unsigned int i = 0; i < size; i++)
    cout << selectedRingList[i] + 1 << " ";
  cout << endl << endl;
}

void View3D::displaySelectedAtomActorsList() {
  unsigned int numActors = selectedAtomActors->GetNumberOfItems();
  if (numActors < 1)
    return;

  cout << numActors << "  selected actors : ";

  selectedAtomActors->InitTraversal();
  for (int i = 0; i < numActors; ++i) {
    vtkActor *actor = selectedAtomActors->GetNextActor();

    if (actor == nullptr)
      continue;
    cout << actor->Id() << " ";
  }

  cout << endl;
}

void View3D::loadTemplateMol(QString path) {
  // cout << path.toStdString().c_str()<<endl;

  view3dT->setMolT(tmol);
  view3dT->loadTemplateMol(path);
}

void View3D::loadMetalTemplateMol(QString path) {
  view3dT->setMolT(tmol);
  view3dT->loadMetalTemplateMol(path);
}

// idx is the hit atom inside this view
void View3D::linkTemplate2Mol(unsigned int idx) {
  if (!isHydrogenAtom(idx)) {
    QMessageBox::information(
        0, "Warning",
        "Only Hydrogen atom can be linked!\nPlease click mouseRightButton or Q "
        "or Escape to switch linker-mode");
    return;
  }

  setSymmetry("C1");

  unsigned int numAtomsT = tmol->numAtoms();
  if (numAtomsT < 1)
    return;

  // cout << "---------------------"<<endl;
  //  cout << numAtomsT<<endl;
  //  cout << idx<<endl;
  // cout << view3dT->getSelectedAtomId()<<endl;

  mol->linkTemplate2Mol(tmol, idx, view3dT->getSelectedAtomId());

  renderMol();
  push2Stack("linkTemplate");

  mainWindow->setFirstTime(false);
  // setDefaultOperationMode();
  firstTime = false;
}

void View3D::addNewTemplate(double x, double y, double z) {
  vector3 v(x, y, z);
  addNewTemplate(v);

  // mainWindow->updatePanelParameters();
  mainWindow->setFirstTime(false);
  firstTime = false;
  // setDefaultOperationMode();
}

void View3D::clearTemplate() {
  tmol->clearAll();
  view3dT->clearAll();
}

void View3D::addNewTemplate(vector3 p) {
  if (tmol->numAtoms() < 1)
    return;
  if (mol->NumAtoms() > 0)
    return;
  mol->addAtomfromTemplate(tmol, p);

  // updateMolLength();
  renderMol(mol);

  push2Stack("newTemplate");
}

void View3D::removeSelectedRings() {
  if (numSelectedRings() < 1)
    return;

  bool turnAromatic2Single = false;
  if (numSelectedBonds() + numSelectedAtoms() < 1)
    turnAromatic2Single = true;

  if (turnAromatic2Single) {
    for (unsigned int i = 0; i < selectedRingList.size(); i++) {
      HRing *ring = mol->getRingbyId(selectedRingList[i]);
      unsigned int size = ring->atomIdList.size() - 1;
      for (unsigned int j = 0; j < size; j++)
        mol->getBondbyAtomIndex(ring->atomIdList[j], ring->atomIdList[j + 1])
            ->setBondOrder(1);
      mol->getBondbyAtomIndex(ring->atomIdList[0], ring->atomIdList[size])
          ->setBondOrder(1);
    }
  }

  if (numSelectedRings() > 1) {
    sort(selectedRingList.begin(), selectedRingList.end());
    vector<uint>::iterator iter =
        unique(selectedRingList.begin(), selectedRingList.end());
    selectedRingList.erase(iter, selectedRingList.end());
    sort(selectedRingList.rbegin(), selectedRingList.rend());

    for (unsigned int i = 0; i < selectedRingList.size(); i++)
      mol->removeRingbyId(selectedRingList[i]);
  } else {
    mol->removeRingbyId(selectedRingList[0]);
  }

  selectedRingList.clear();
  selectedRingActors->RemoveAllItems();

  renderMol(mol);
  // if(turnAromatic2Single)
  push2Stack("remove Ring");
}

void View3D::removeSelectedAtomsBonds() {
  removeSelectedRings();
  if (numSelectedBonds() + numSelectedAtoms() < 1)
    return;

  // remove bonds
  if (numSelectedBonds() > 0 && numSelectedAtoms() < 1) {
    removeSelectedBonds();

    selectedBondList.clear();
    selectedBondActors->RemoveAllItems();
    renderMol(mol);
    return;
  }

  selectedBondList.clear();
  removeSelectedAtoms();
  selectedAtomList.clear();
  selectedAtomActors->RemoveAllItems();
  renderMol(mol);
  return;
}

void View3D::removeSelectedBonds() {
  if (numSelectedBonds() < 1)
    return;

  for (unsigned int i = 0; i < selectedBondList.size(); i++)
    mol->removeBondbyAtomIds(selectedBondList[i][1], selectedBondList[i][2]);

  selectedBondList.clear();
  selectedBondActors->RemoveAllItems();

  renderMol(mol);

  push2Stack("removeSelectedBonds");
}

void View3D::removeSelectedAtoms() {
  if (numSelectedAtoms() < 1)
    return;

  // cout << numSelectedAtoms() <<" Atoms need to be removed"<<endl;

  if (numSelectedAtoms() == 1) {
    mol->removeAtombyId(selectedAtomList[0]);
  } else {
    mol->removeAtombyId(selectedAtomList);
  }
  selectedAtomList.clear();
  selectedAtomActors->RemoveAllItems();

  renderMol(mol);
  push2Stack("removeSelectedAtoms");
}

void View3D::selectAll() {
  cout << "inside View3D::selectAll" << endl;
  selectedAtomList.clear();
  for (unsigned int i = 0; i < mol->NumAtoms(); i++) {
    selectedAtomList.push_back(i);
    mol->getAtombyIndex(i)->setSelected(true);
  }

  selectedBondList.clear();
  for (unsigned int i = 0; i < mol->NumBonds(); i++) {
    mol->getBondbyIndex(i)->setSelected(true);

    vector<unsigned int> v;
    v.push_back(i);
    if (mol->getBondbyIndex(i)->atomIndex0() <
        mol->getBondbyIndex(i)->atomIndex0()) {
      v.push_back(mol->getBondbyIndex(i)->atomIndex0());
      v.push_back(mol->getBondbyIndex(i)->atomIndex1());
    } else {
      v.push_back(mol->getBondbyIndex(i)->atomIndex1());
      v.push_back(mol->getBondbyIndex(i)->atomIndex0());
    }

    selectedBondList.push_back(v);
  }

  updateSelectedFromRendering = true;
  renderMol(mol);
  updateSelectedFromRendering = false;
}

MouseOperationType View3D::currentMouseOperationMode() {
  return mouseOperationMode;
}

bool View3D::isLinkTemplateMode() {
  if (mouseOperationMode == VTKLinkTemplate)
    return true;
  return false;
}

bool View3D::isPencilDrawMode() {
  if (mouseOperationMode == VTKPencilDrawing)
    return true;
  return false;
}

bool View3D::isSelectionMode() {
  if (mouseOperationMode == VTKSelection)
    return true;
  return false;
}

bool View3D::isRotateMode() {
  if (mouseOperationMode == VTKRotation)
    return true;
  return false;
}

bool View3D::isTranslateMode() {
  if (mouseOperationMode == VTKTranslation)
    return true;
  return false;
}

void View3D::setDefaultOperationMode() {
  mouseOperationMode = VTKRotation;
  setCursor(Qt::ArrowCursor);
  actorInteractorA->setRotateMode();

  mainWindow->setLoopPlayingStop();
}

void View3D::setPencilDrawMode() {
  mouseOperationMode = VTKPencilDrawing;
  actorInteractorA->setPencilDrawMode();
  auto pencilCursor = new QCursor(QPixmap(":/images/draw.png"), 0, -2);
  setCursor(*pencilCursor);

  releaseAllActors2();
  setSymmetryElementVisible(false);

  updateMol();
}

void View3D::setLinkTemplateMode() {
  mouseOperationMode = VTKLinkTemplate;
  actorInteractorA->setLinkTemplateMode();
  setCursor(Qt::PointingHandCursor);
  releaseAllActors();
}

void View3D::setRotateMode() {
  mouseOperationMode = VTKRotation;
  actorInteractorA->setRotateMode();
}

void View3D::setTranslateMode() {
  // mouseOperationMode=VTKTranslation;
  // actorInteractorA->setTranslateMode();
  auto translateCursor = new QCursor(QPixmap(":/images/translate.png"), -1, -1);
  setCursor(*translateCursor);
}

void View3D::setSelectionMode() {
  mouseOperationMode = VTKSelection;
  actorInteractorA->setSelectionMode();
}

void View3D::setOperateSelectedMode() {
  mouseOperationMode = VTKOperateSelected;
  actorInteractorA->setOperateSelectedMode();
}

void View3D::undo() {
  if (mainWindow == nullptr)
    return;
  mainWindow->undo();
}

void View3D::redo() {
  if (mainWindow == nullptr)
    return;
  mainWindow->redo();
}

void View3D::translateSelectedAtoms(vector3 v) {
  mol->translate(v);
  updateMol();
}

void View3D::rotateSelectedAtoms(vector3 v, matrix3x3 m) {
  mol->tranform(v, m);
  updateMol();
}

void View3D::push2Stack(QString c) {
  if (mainWindow == nullptr)
    return;
  mainWindow->push2Stack(c);
}

void View3D::setSelectedBondOrder(unsigned int o) {
  unsigned int numActors = selectedBondActors->GetNumberOfItems();
  if (numActors >= 1) {
    selectedBondActors->InitTraversal();
    for (unsigned int i = 0; i < numActors; ++i) {
      unsigned int j = selectedBondActors->GetNextActor()->BondId();
      unsigned int bo = mol->bondList[j]->getBondOrder();

      if (bo == 6 && o == 6)
        mol->getBondbyIndex(j)->swapIndex();
      mol->getBondbyIndex(j)->setBondOrder(o);
    }

    releaseAllActors();
    mol->filterRings();
    renderMol(mol);
    push2Stack("Reset Bond Order");
    return;
  }

  numActors = selectedAtomActors->GetNumberOfItems();
  if (numActors == 2) {
    selectedAtomActors->InitTraversal();
    vector<uint> id;
    for (unsigned int i = 0; i < numActors; ++i)
      id.push_back(selectedAtomActors->GetNextActor()->AtomId());

    if (mol->getBondbyAtomIndex(id[0], id[1]))
      mol->getBondbyAtomIndex(id[0], id[1])->setBondOrder(o);
    else
      mol->addBond(id[0], id[1], o);

    releaseAllActors();
    mol->filterRings();
    renderMol(mol);
    push2Stack("Reset Bond Order");
    return;
  }

  numActors = selectedRingActors->GetNumberOfItems();
  if (numActors > 0 && o == 6) {
    selectedRingActors->InitTraversal();
    for (unsigned int i = 0; i < numActors; ++i) {
      unsigned int size =
          mol->getRingbyId(selectedRingList[i])->atomIdList.size();
      unsigned int beg = mol->getRingbyId(selectedRingList[i])->atomIdList[0];
      unsigned int end =
          mol->getRingbyId(selectedRingList[i])->atomIdList[size - 1];
      mol->getBondbyAtomIndex(beg, end)->setBondOrder(6);
      for (unsigned int j = 0; j < size - 1; j++) {
        beg = mol->getRingbyId(selectedRingList[i])->atomIdList[j];
        end = mol->getRingbyId(selectedRingList[i])->atomIdList[j + 1];
        mol->getBondbyAtomIndex(beg, end)->setBondOrder(6);
      }
    }
    releaseAllActors();
    mol->filterRings();
    renderMol(mol);
    push2Stack("Reset Bond Order");
    return;
  }

  releaseAllActors();

  mol->filterRings();
  renderMol(mol);
}

void View3D::replaceSelelctedAtomWith(unsigned int idx) {
  if (numSelectedAtoms() < 1)
    return;

  mol->replaceSelelctedAtomWith(idx, selectedAtomList);

  releaseAllActors();
  renderMol(mol);
  push2Stack("replace Element");
}

void View3D::Link2LastAtom(vector3 v) { Link2LastAtom(v.x(), v.y(), v.z()); }

void View3D::Link2LastAtom(double x, double y, double z) {
  addCarbon(x, y, z);
  if (mol->getBondOrderbyAtomIndex(mol->NumAtoms() - 1, mol->NumAtoms() - 2) <
      1) {
    mol->addBond(mol->NumAtoms() - 1, mol->NumAtoms() - 2);
    renderMol();
    push2Stack("draw Carbon");
  }
}

void View3D::linkTwoSelectedAtoms(unsigned int id1, unsigned int id2) {
  if (mol->getBondOrderbyAtomIndex(id1, id2) > 0)
    return;
  mol->addBond(id1, id2);
  renderMol();
  push2Stack("link atom");
}

void View3D::addCarbon_and_link2SelectedAtom(vector3 pos, unsigned int id) {
  // cout << " \naddCarbon_and_link2SelectedAtom "<<endl;

  QString Carbon = "C";
  mol->addAtom(Carbon, pos);
  mol->addBond(mol->NumAtoms() - 1, id);

  renderMol();
  push2Stack("draw Carbon");

  // mol->displayAtoms();
  // mol->displayBonds();
}

void View3D::addCarbon(double x, double y, double z) {
  // cout << " \nadd the first Carbon"<<endl;
  QString Carbon = "C";
  mol->addAtom(Carbon, x, y, z);
  renderMol();

  push2Stack("draw Carbon");
  // mol->displayAtoms();    mol->displayBonds();
}

void View3D::addCarbon(vector3 pos) { addCarbon(pos.x(), pos.y(), pos.z()); }

void View3D::symmetryPercept() { mainWindow->symmetryPercept(); }

void View3D::focusOutEvent(QFocusEvent *) {
  // actorInteractorA->loseFocus();
}

unsigned int View3D::numSelectedObjects() {
  return selectedAtomList.size() + selectedBondList.size() +
         selectedRingList.size();
}

void View3D::setSelectedAtomSymbol(QString s) {
  for (unsigned int i = 0; i < selectedAtomList.size(); i++)
    mol->setAtomSymbol(selectedAtomList[i], s);
  releaseAllActors2();
  updateMol();

  push2Stack("modify Atoms");
}

void View3D::reverseAromaticBond() {}

void View3D::modifyAtomColor() {
  if (numSelectedAtoms() < 1)
    return;

  QColor c = QColorDialog::getColor(Qt::white, nullptr);
  vector3 colour(c.red(), c.green(), c.blue());

  if (numSelectedAtoms() < 2)
    mol->setAtomColor(getSelectedAtomId(0), buildColor1(colour));
  for (unsigned int i = 0; i < numSelectedAtoms(); i++)
    mol->setAtomColor(getSelectedAtomId(i), buildColor1(colour));
  this->updateMol();

  push2Stack("modify atom color");
}

void View3D::modifyAtomRadius() {
  mainWindow->on_pushButton_Atom_clicked();
  push2Stack("modify atom radius");
}

void View3D::buildRingForSelectedAtoms() {
  unsigned int numActors = selectedAtomActors->GetNumberOfItems();

  if (numActors <= 2)
    return;
  selectedAtomActors->InitTraversal();

  vector<uint> id;
  vector3 cc = VZero;
  for (unsigned int i = 0; i < numActors; ++i) {
    unsigned int ii = selectedAtomActors->GetNextActor()->AtomId();
    id.push_back(ii);
    cc += mol->getAtomPosbyIndex(ii);
  }
  cc /= numActors;

  // existed?
  bool isExisted = false;
  for (unsigned int i = 0; i < mol->numRings(); i++) {
    if (mol->getRingbyId(i)->size != numActors)
      continue;
    if ((mol->getRingbyId(i)->center - cc).length() > 0.5)
      continue;

    isExisted = true;
    break;
  }

  if (isExisted) {
    releaseAllActors();
    setDefaultOperationMode();
    return;
  }

  unsigned int size = id.size();
  vector<uint> id1;
  id1.clear();

  // reorder
  vector<uint> label;
  for (unsigned int i = 0; i < size; i++)
    label.push_back(0);
  id1.push_back(id[0]);
  label[0] = 1;

  while (id1.size() < size)
    for (unsigned int i = 0; i < size; i++) {
      if (label[i] > 0)
        continue;
      if (mol->getBondOrderbyAtomIndex(id1[id1.size() - 1], id[i]) < 1)
        continue;
      id1.push_back(i);
      label[i] = 1;
    }

  cout << " add New Ring " << endl;
  for (unsigned int i = 0; i < size; i++)
    cout << id1[i] + 1 << endl;

  releaseAllActors();
  setDefaultOperationMode();
  mol->addRing(id1);
  renderMol(mol);

  mol->displayRingList();

  push2Stack("Add New Ring");
}

bool View3D::isRingExistedforSelectedAtoms() {
  unsigned int numActors = selectedAtomActors->GetNumberOfItems();

  if (numActors <= 2)
    return false;
  selectedAtomActors->InitTraversal();

  vector<uint> id;
  vector3 cc = VZero;
  for (unsigned int i = 0; i < numActors; ++i) {
    unsigned int ii = selectedAtomActors->GetNextActor()->AtomId();
    id.push_back(ii);
    cc += mol->getAtomPosbyIndex(ii);
  }
  cc /= numActors;

  // existed?
  bool existed = false;
  for (unsigned int i = 0; i < mol->numRings(); i++) {
    if (mol->getRingbyId(i)->size != numActors)
      continue;

    if ((mol->getRingbyId(i)->center - cc).length() > 0.5)
      continue;

    existed = true;
    break;
  }

  return existed;
}

void View3D::appendXAtomAtCenterofSelectedAtoms() {
  unsigned int numActors = selectedAtomActors->GetNumberOfItems();
  if (numActors < 2)
    return;

  vector3 cc = VZero;
  selectedAtomActors->InitTraversal();
  for (unsigned int i = 0; i < numActors; i++)
    cc = cc +
         mol->getAtomPosbyIndex(selectedAtomActors->GetNextActor()->AtomId());

  cc = cc / numActors;
  QString pseudoAtom = "X";

  mol->addAtom(pseudoAtom, cc);

  updateMol();

  push2Stack("adding Center");
}

// distance, angle, torsion and center
void View3D::calculateDATC() {
  // updateSelectedAtomListFromAtomActors();
  displaySelectedAtomList();

  unsigned int numActors = selectedRingList.size();
  if (numActors == 1 && numSelectedObjects() == 1) {
    QString s = "Ring ";
    vector3 cc = mol->getRingbyId(selectedRingList[0])->center;
    s = s + "with center: " + QString::number(cc.x()) + " " +
        QString::number(cc.y()) + " " + QString::number(cc.z());
    return;
  }

  numActors = selectedBondList.size();
  if (numActors == 1) {
    double distance =
        mol->Distance(selectedBondList[0][1], selectedBondList[0][2]);
    QString s = "Bond length: ";
    s += QString::number(selectedBondList[0][1] + 1) + "<-->" +
         QString::number(selectedBondList[0][2] + 1) + " ";
    s += QString::number(distance);
    mainWindow->displayMessage(s);
    return;
  }

  numActors = selectedAtomList.size();
  if (numActors > 1) {
    QString s = "Atoms ";

    if (numActors == 2) // distance
    {
      s += QString::number(selectedAtomList[0] + 1) + "  " +
           QString::number(selectedAtomList[1] + 1) + " ";
      double distance = mol->Distance(selectedAtomList[0], selectedAtomList[1]);
      s = s + " with distance :" + QString::number(distance);
      mainWindow->displayMessage(s);
    }

    if (numActors == 3) // angle
    {
      s += QString::number(selectedAtomList[0] + 1) + "  " +
           QString::number(selectedAtomList[1] + 1) + "  " +
           QString::number(selectedAtomList[2] + 1) + "  ";

      double angle = mol->Angle(selectedAtomList[0], selectedAtomList[1],
                                selectedAtomList[2]);
      s = s + " with angle :" + QString::number(angle);
      mainWindow->displayMessage(s);
    }

    if (numActors == 4) // angle
    {
      s += QString::number(selectedAtomList[0] + 1) + "  " +
           QString::number(selectedAtomList[1] + 1) + "  " +
           QString::number(selectedAtomList[2] + 1) + "  " +
           QString::number(selectedAtomList[3] + 1) + "  ";

      double angle = mol->Dihedral(selectedAtomList[0], selectedAtomList[1],
                                   selectedAtomList[2], selectedAtomList[3]);
      s = s + " with dihedral :" + QString::number(angle);
      mainWindow->displayMessage(s);
    }

    // center of selected atoms
    vector3 cc = VZero;
    for (unsigned int i = 0; i < numActors; i++)
      cc += mol->getAtomPosbyIndex(selectedAtomList[i]);
    cc /= numActors;
    s = s + " with center: " + QString::number(cc.x()) + " " +
        QString::number(cc.y()) + " " + QString::number(cc.z());
    mainWindow->displayMessage(s);
  }
}

bool View3D::isHydrogenAtom(unsigned int id) {
  if (mol->getAtomSymbol(id) == "H")
    return true;
  return false;
}

void View3D::modifySelectedRing() { mainWindow->on_pushButton_Ring_clicked(); }

void View3D::centralize() {
  releaseAllActors();
  mainWindow->on_pushButton_center_PG_clicked();
}

void View3D::periodicTable() {
  if (numSelectedAtoms() < 1)
    return;
  mainWindow->on_actionPeriodicTable_triggered();
}

void View3D::rotateLeft() {
  setDefaultOperationMode();
  onView_rotateLeft(5.0);
}
void View3D::rotateRight() {
  setDefaultOperationMode();
  onView_rotateRight(5.0);
}

void View3D::rotateUp() {
  setDefaultOperationMode();
  onView_rotateUp(5.0);
}

void View3D::rotateDown() {
  setDefaultOperationMode();
  onView_rotateDown(5.0);
}

void View3D::addHydrogenAtomForSelected() {
  if (numSelectedAtoms() < 1)
    return;

  for (unsigned int i = 0; i < selectedAtomList.size(); i++)
    mol->addHydrogen(selectedAtomList[i]);

  mainWindow->push2Stack("adding-hydrogen");
}

void View3D::copySeleted() {
  copyMolData = "MolSymmetry-copy-data\n";
  copyMolData += QString::number(numSelectedAtoms()) + "\n";
  ;
  for (unsigned int i = 0; i < numSelectedAtoms(); i++) {
    // atom information
    unsigned int id = getSelectedAtomId(i);
    QString a = mol->atomSymbol(id) + "    " +
                QString::number(mol->atomPos(id).x(), 'f', 6) + "    " +
                QString::number(mol->atomPos(id).y(), 'f', 6) + "    " +
                QString::number(mol->atomPos(id).z(), 'f', 6) + "    "

                + QString::number(mol->getAtombyIndex(id)->radius(), 'f', 2) +
                "    " + QString::number(mol->atomList[id]->Color()) + "\n";

    copyMolData += a;
  }

  // bond information
  QString b = "";
  unsigned int nbonds = 0;
  for (unsigned int i = 0; i < numSelectedAtoms(); i++) {
    unsigned int id = getSelectedAtomId(i);
    for (unsigned int k = i + 1; k < numSelectedAtoms(); k++) {
      unsigned int bo = mol->getBondOrderbyAtomIndex(getSelectedAtomId(k), id);
      if (bo < 1)
        continue;
      b += QString::number(i) + " " + QString::number(k) + " " +
           QString::number(bo) + "\n";
      nbonds++;
    }
  }
  copyMolData += QString::number(nbonds) + "\n";
  ;
  copyMolData += b;
  // cout << copyMolData.toStdString().c_str()<<endl;
  QApplication::clipboard()->setText(copyMolData);
}

bool View3D::canbePaste() { return mol->canbePaste(); }

bool View3D::canbeUndo() {
  if (mainWindow->undoList.size() > 0)
    return true;
  return false;
}

bool View3D::canbeRedo() {
  if (mainWindow->redoList.size() > 0)
    return true;
  return false;
}

void View3D::cutSelected() {
  copySeleted();
  setDefaultOperationMode();

  vector<unsigned int> idList;
  for (unsigned int i = 0; i < numSelectedAtoms(); i++)
    idList.push_back(getSelectedAtomId(i));

  sort(idList.begin(), idList.end());
  vector<uint>::iterator iter = unique(idList.begin(), idList.end());
  idList.erase(iter, idList.end());
  sort(idList.rbegin(), idList.rend());

  // remove from the tail one-by-one
  for (unsigned int i = 0; i < idList.size(); i++)
    mol->removeSingleAtombyId(idList[i]);

  updateMol();
  push2Stack("Cut ");
}

void View3D::setSymmetry(QString s) { PointGroup = s; }

void View3D::paste() {
  mol->clearAll();
  setSymmetry("C1");

  setDefaultOperationMode();
  unsigned int initNumAtoms = mol->NumAtoms();
  unsigned int initNumBonds = mol->NumBonds();

  // cout << " initNumAtoms :"<<initNumAtoms<<endl;

  if (!mol->canbePaste())
    return;

  initNumAtoms = mol->NumAtoms();
  initNumBonds = mol->NumBonds();

  mol->pastefromClipboard();
  push2Stack("paste from Clipboard");

  unsigned int curNumAtoms = mol->NumAtoms();
  unsigned int curNumBonds = mol->NumBonds();

  unsigned int i;

  // cout << " initNumAtoms :"<<initNumAtoms<<endl;
  // cout << " curNumAtoms :"<<curNumAtoms<<endl;

  if (initNumAtoms > 0) {
    for (i = 0; i < initNumAtoms; i++)
      mol->getAtombyIndex(i)->setSelected(false);
    for (i = initNumAtoms; i < curNumAtoms; i++)
      mol->getAtombyIndex(i)->setSelected(true);

    for (i = 0; i < initNumBonds; i++)
      mol->getBondbyIndex(i)->setSelected(false);

    for (i = initNumBonds; i < curNumBonds; i++)
      mol->getBondbyIndex(i)->setSelected(true);
  }

  updateSelectedFromRendering = true;
  renderMol(mol);
  updateSelectedFromRendering = false;
}

void View3D::setC3_Visible(bool b) {
  _isC3_Visible = b;
  if (principalOrder == 3)
    mainWindow->setCn_Visible();
}

void View3D::setC4_Visible(bool b) {
  _isC4_Visible = b;

  if (principalOrder == 4)
    mainWindow->setCn_Visible();
}

void View3D::setC5_Visible(bool b) {
  _isC5_Visible = b;
  if (principalOrder == 5)
    mainWindow->setCn_Visible();
}

void View3D::setC6_Visible(bool b) {
  _isC6_Visible = b;
  if (principalOrder == 6)
    mainWindow->setCn_Visible();
}

void View3D::UFF() { mainWindow->on_actionUFF_triggered(); }

void View3D::MM2() { mainWindow->on_actionMM2_triggered(); }

void View3D::Gaff() { mainWindow->on_actionGaff_triggered(); }

void View3D::MMFF94() { mainWindow->on_actionMMFF94_triggered(); }

void View3D::Ghemical() { mainWindow->on_actionGhemical_triggered(); }

void View3D::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  } else
    event->ignore();
}

void View3D::dropEvent(QDropEvent *event) {
  const QMimeData *mimeData = event->mimeData();
  if (!mimeData->hasUrls())
    return;

  QString filename = mimeData->urls().at(0).toLocalFile();
  if (filename.isEmpty())
    return;

  mainWindow->loadFile(filename);
}
