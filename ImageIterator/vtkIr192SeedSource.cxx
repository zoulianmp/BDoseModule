#include "vtkIr192SeedSource.h"

// VTK includes
#include <vtkObjectFactory.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkIr192SeedSource);

//----------------------------------------------------------------------------
vtkIr192SeedSource::vtkIr192SeedSource()
{
  this->TaskObject = 0;
  this->TaskFunction = 0;
  this->Type = vtkIr192SeedSource::Undefined;
}
//----------------------------------------------------------------------------
vtkIr192SeedSource::~vtkIr192SeedSource()
{
}

//----------------------------------------------------------------------------
void vtkIr192SeedSource::SetTaskFunction(vtkMRMLAbstractLogic *object,
                                    vtkMRMLAbstractLogic::TaskFunctionPointer function,
                                    void *clientdata)
{
  this->TaskObject = object;
  this->TaskFunction = function;
  this->TaskClientData = clientdata;
}

//----------------------------------------------------------------------------
void vtkIr192SeedSource::Execute()
{
  if (this->TaskObject)
    {
    ((*this->TaskObject).*(this->TaskFunction))(this->TaskClientData);
    }
}

//----------------------------------------------------------------------------
void vtkIr192SeedSource::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}
