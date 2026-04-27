

#ifndef AreaPicker_h
#define AreaPicker_h

#include <vtkAbstractPropPicker.h>
#include <vtkRenderingCoreModule.h>
#include <vtkProp3DCollection.h>

class vtkRenderer;
class vtkPoints;
class vtkPlanes;
class vtkProp3DCollection;
class vtkAbstractMapper3D;
class vtkDataSet;
class vtkExtractSelectedFrustum;
class vtkProp;

class  AreaPicker : public vtkAbstractPropPicker
{
public:
  static AreaPicker* New();
  vtkTypeMacro(AreaPicker, vtkAbstractPropPicker);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Set the default screen rectangle to pick in.
   */
  void SetPickCoords(double x0, double y0, double x1, double y1);

  /**
   * Set the default renderer to pick on.
   */
  void SetRenderer(vtkRenderer*);

  /**
   * Perform an AreaPick within the default screen rectangle and renderer.
   */
  virtual int Pick();

  /**
   * Perform pick operation in volume behind the given screen coordinates.
   * Props intersecting the selection frustum will be accessible via GetProp3D.
   * GetPlanes returns a vtkImplicitFunction suitable for vtkExtractGeometry.
   */
  virtual int AreaPick(double x0, double y0, double x1, double y1, vtkRenderer* renderer = nullptr);

  /**
   * Perform pick operation in volume behind the given screen coordinate.
   * This makes a thin frustum around the selected pixel.
   * Note: this ignores Z in order to pick everying in a volume from z=0 to z=1.
   */
  int Pick(double x0, double y0, double vtkNotUsed(z0), vtkRenderer* renderer = nullptr) override
  {
    return this->AreaPick(x0, y0, x0 + 1.0, y0 + 1.0, renderer);
  }

  //@{
  /**
   * Return mapper that was picked (if any).
   */
  vtkGetObjectMacro(Mapper, vtkAbstractMapper3D);
  //@}

  //@{
  /**
   * Get a pointer to the dataset that was picked (if any). If nothing
   * was picked then NULL is returned.
   */
  vtkGetObjectMacro(DataSet, vtkDataSet);
  //@}

  /**
   * Return a collection of all the prop 3D's that were intersected
   * by the pick ray. This collection is not sorted.
   */
  vtkProp3DCollection* GetProp3Ds() { return this->Prop3Ds; }

  //@{
  /**
   * Return the six planes that define the selection frustum. The implicit
   * function defined by the planes evaluates to negative inside and positive
   * outside.
   */
  vtkGetObjectMacro(Frustum, vtkPlanes);
  //@}

  //@{
  /**
   * Return eight points that define the selection frustum.
   */
  vtkGetObjectMacro(ClipPoints, vtkPoints);
  //@}


  AreaPicker();
  ~AreaPicker() override;

  void Initialize() override;
  void DefineFrustum(double x0, double y0, double x1, double y1, vtkRenderer*);
  virtual int PickProps(vtkRenderer* renderer);
  int TypeDecipher(vtkProp*, vtkAbstractMapper3D**);

  int ABoxFrustumIsect(double bounds[], double& mindist);

  vtkPoints* ClipPoints;
  vtkPlanes* Frustum;

  vtkProp3DCollection* Prop3Ds; // candidate actors (based on bounding box)
  vtkAbstractMapper3D* Mapper;  // selected mapper (if the prop has a mapper)
  vtkDataSet * DataSet;          // selected dataset (if there is one)

  // used internally to do prop intersection tests
  vtkExtractSelectedFrustum* FrustumExtractor;

  double X0;
  double Y0;
  double X1;
  double Y1;

  unsigned int NumSelectedProps() ;
private:
  AreaPicker(const AreaPicker&) = delete;
  void operator=(const AreaPicker&) = delete;
};

#endif
