#include "vtkIr192SeedSource.h"

// VTK includes
#include <vtkObjectFactory.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkIr192SeedSource);

//----------------------------------------------------------------------------
vtkIr192SeedSource::vtkIr192SeedSource()
{

}

//----------------------------------------------------------------------------
vtkIr192SeedSource::~vtkIr192SeedSource()
{
}



//----------------------------------------------------------------------------
void vtkIr192SeedSource::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}


void vtkIr192SeedSource::SetupIr192Seed()
{
	vtkIr192SeedSource::Ir192Spec = {
		"Ir-192 (mCi)",			/* name */
		4.62,				/* gamma */
		MC,				/* units - MC or MG */
		0.96,				/* R to rad */
		74.2 * 24.0,			/* half-life (hours) */
		13,				/* number of entries in tissue
						*  attenuation table  */
						/* attenuation factors */
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		/* corresponding distances (cm) */
		0.5, 1.0, 1.5, 2.0, 2.5,
		3.0, 4.0, 5.0, 6.0, 7.0,
		8.0, 9.0, 10.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0,				/* mu */
		FALSE				/* boolean - last entry in file? */
	};

}