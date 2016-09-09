#ifndef __vtkIr192SeedSource_h
#define __vtkIr192SeedSource_h

#include "vtkSRPlanPathPlanModuleLogicExport.h"

#include "libbrachy.h"

#include "vtkObject.h"
#include "vtkSmartPointer.h"


class VTK_SRPlan_PATHPLAN_MODULE_LOGIC_EXPORT vtkIr192SeedSource : public vtkObject
{
public:
  static vtkIr192SeedSource *New();
  vtkTypeMacro(vtkIr192SeedSource,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  static void SetupIr192Seed();

  ///
  /// The type of task - this can be used, for example, to decide
  /// how many concurrent threads should be allowed
  enum
    {
    Undefined = 0,
    Processing,
    Networking
    };

  vtkSetClampMacro (Type, int, vtkIr192SeedSource::Undefined, vtkIr192SeedSource::Networking);
  vtkGetMacro (Type, int);
  

protected:
  vtkIr192SeedSource();
  virtual ~vtkIr192SeedSource();
  vtkIr192SeedSource(const vtkIr192SeedSource&);
  void operator=(const vtkIr192SeedSource&);

private:
  
  int Type;

  static SEED_SPEC Ir192Spec;

};
#endif


