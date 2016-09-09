#ifndef __vtkIr192SeedSource_h
#define __vtkIr192SeedSource_h

//#include "vtkSRPlanPathPlanModuleLogicExport.h"

#include "libbrachy.h"

#include "vtkObject.h"
#include "vtkSmartPointer.h"
#include <vtkImageData.h>


//class VTK_SRPlan_PATHPLAN_MODULE_LOGIC_EXPORT vtkIr192SeedSource : public vtkObject
class  vtkIr192SeedSource : public vtkObject
{
public:
  static vtkIr192SeedSource *New();
  vtkTypeMacro(vtkIr192SeedSource,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetupIr192Seed();


  void SetDoseKernalCutoff(float cutoff);
  float GetDoseKernalCutoff();


  void SetGridSpacing(float spacing);
  float GetGridSpacing();


  void UpdateDoseKernalVolume();

  vtkImageData * GetDoseKernalVolume();

  void PrintROIDose(vtkImageData * data, int * extent);



protected:
  vtkIr192SeedSource();
  virtual ~vtkIr192SeedSource();
  vtkIr192SeedSource(const vtkIr192SeedSource&);
  void operator=(const vtkIr192SeedSource&);

private:
  
  bool m_kernal_invalid; // Flag to indicate the volume kernal state

  float m_grid_spacing; // dose voxel length unit : mm 

  float m_cutoff; //Using the cutoff value in mm unit, to define the dose kernal 

  /*
  In the SEED_SPEC, LookupTabel deta is 1% cm, 30cm length got equal divided seg,total is 3000 point.
  */
  SEED_SPEC m_Ir192Spec;
  vtkImageData*  m_DoseKernal; //3D Dose kernal for Brachy
};
#endif


