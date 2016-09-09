#ifndef __vtkIr192SeedSource_h
#define __vtkIr192SeedSource_h

#include "vtkSRPlanPathPlanModuleLogicExport.h"


#include "vtkObject.h"
#include "vtkSmartPointer.h"
#include "vtkMRMLAbstractLogic.h"
#include "vtkSlicerBaseLogic.h"

class VTK_SRPlan_PATHPLAN_MODULE_LOGIC_EXPORT vtkIr192SeedSource : public vtkObject
{
public:
  static vtkIr192SeedSource *New();
  vtkTypeMacro(vtkIr192SeedSource,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  typedef vtkMRMLAbstractLogic::TaskFunctionPointer TaskFunctionPointer;

  ///
  /// Set the function and object to call for the task.
  void SetTaskFunction(vtkMRMLAbstractLogic*, TaskFunctionPointer, void *clientdata);

  ///
  /// Execute the task.
  virtual void Execute();

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
  void SetTypeToProcessing() {this->SetType(vtkIr192SeedSource::Processing);};
  void SetTypeToNetworking() {this->SetType(vtkIr192SeedSource::Networking);};

  const char* GetTypeAsString( ) {
    switch (this->Type)
      {
      case vtkIr192SeedSource::Undefined: return "Undefined";
      case vtkIr192SeedSource::Processing: return "Processing";
      case vtkIr192SeedSource::Networking: return "Networking";
      }
    return "Unknown";
  }

protected:
  vtkIr192SeedSource();
  virtual ~vtkIr192SeedSource();
  vtkIr192SeedSource(const vtkIr192SeedSource&);
  void operator=(const vtkIr192SeedSource&);

private:
  vtkSmartPointer<vtkMRMLAbstractLogic> TaskObject;
  vtkMRMLAbstractLogic::TaskFunctionPointer TaskFunction;
  void *TaskClientData;

  int Type;

};
#endif


