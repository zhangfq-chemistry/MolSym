#ifndef vtkCapsuleSource_h
#define vtkCapsuleSource_h

#include <vtkFiltersSourcesModule.h> // For export macro
#include <vtkPolyDataAlgorithm.h>

#include <vtkSpherePuzzle.h> // For VTK_MAX_SPHERE_RESOLUTION

class vtkCapsuleSource : public vtkPolyDataAlgorithm {
public:
  vtkTypeMacro(vtkCapsuleSource, vtkPolyDataAlgorithm);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  static vtkCapsuleSource *New();

  vtkSetClampMacro(Radius, double, 0.0, VTK_DOUBLE_MAX);
  vtkGetMacro(Radius, double);

  vtkSetVector3Macro(Center, double);
  vtkGetVectorMacro(Center, double, 3);

  vtkSetClampMacro(CylinderLength, double, 0.0, VTK_DOUBLE_MAX);
  vtkGetMacro(CylinderLength, double);

  vtkSetClampMacro(ThetaResolution, int, 8, VTK_MAX_SPHERE_RESOLUTION);
  vtkGetMacro(ThetaResolution, int);

  vtkSetClampMacro(PhiResolution, int, 8, VTK_MAX_SPHERE_RESOLUTION);
  vtkGetMacro(PhiResolution, int);

  vtkSetMacro(LatLongTessellation, int);
  vtkGetMacro(LatLongTessellation, int);
  vtkBooleanMacro(LatLongTessellation, int);

  vtkSetMacro(OutputPointsPrecision, int);
  vtkGetMacro(OutputPointsPrecision, int);

protected:
  vtkCapsuleSource(int res = 8);
  ~vtkCapsuleSource() override {}

  int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *) override;
  int RequestInformation(vtkInformation *, vtkInformationVector **,
                         vtkInformationVector *) override;

  double Radius;
  double Center[3];
  int ThetaResolution;
  int PhiResolution;
  int LatLongTessellation;
  int FillPoles;
  double CylinderLength;
  int OutputPointsPrecision;

private:
  vtkCapsuleSource(const vtkCapsuleSource &) = delete;
  void operator=(const vtkCapsuleSource &) = delete;
};

#endif
