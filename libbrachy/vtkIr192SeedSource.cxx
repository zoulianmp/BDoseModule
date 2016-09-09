

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include "libbrachy.h"

#include <io.h>
#include <fcntl.h>

#include <vtkImageIterator.h>


#include "vtkIr192SeedSource.h"

// VTK includes
#include <vtkObjectFactory.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkIr192SeedSource);

//----------------------------------------------------------------------------
vtkIr192SeedSource::vtkIr192SeedSource()
{
	//the c-arm image size is 256*256
	//voxel spacing is 0.43 mm the all range is 110 mm
	
	m_grid_spacing = 2.0; // unit is mm voxel length
    m_cutoff = 50; // unit mm

	this->SetupIr192Seed();


	// Update the Dose Kernal
	this->UpdateDoseKernalVolume();



	m_kernal_invalid = false; 
}

//----------------------------------------------------------------------------
vtkIr192SeedSource::~vtkIr192SeedSource()
{
	m_DoseKernal->Delete();
}



//----------------------------------------------------------------------------
void vtkIr192SeedSource::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

float vtkIr192SeedSource::GetDoseKernalCutoff()
{
	return m_cutoff;
}

void vtkIr192SeedSource::SetDoseKernalCutoff(float cutoff)
{
	if (cutoff != m_cutoff)
	{
		m_kernal_invalid = true;
        m_cutoff = cutoff;
	}

}

float vtkIr192SeedSource::GetGridSpacing()
{
	return m_grid_spacing;

}


void vtkIr192SeedSource::SetGridSpacing(float spacing)
{
	if (spacing != m_grid_spacing)
	{
		m_kernal_invalid = true;
		m_grid_spacing = spacing;
	}
}




void vtkIr192SeedSource::SetupIr192Seed()
{
	SEED_SPEC sspec = {
		"Ir-192 (mCi)",			/* name */
		4.62f,				/* gamma */
		MC,				/* units - MC or MG */
		0.96f,				/* R to rad */
		74.2f * 24.0f,			/* half-life (hours) */
		13,				/* number of entries in tissue
						*  attenuation table  */
						/* attenuation factors */
		float(0.0), float(0.0),float(0.0),float(0.0),float(0.0),
		float(0.0), float(0.0),float(0.0),float(0.0),float(0.0),
		float(0.0), float(0.0),float(0.0),float(0.0),float(0.0),
		float(0.0), float(0.0),float(0.0),float(0.0),float(0.0),

		/* corresponding distances (cm) */
		float(0.5), float(1.0), float(1.5), float(2.0), float(2.5),
		float(3.0), float(4.0), float(5.0), float(6.0), float(7.0),
		float(8.0), float(9.0), float(10.0), float(0.0), float(0.0),
		float(0.0), float(0.0), float(0.0), 0.0f, 0.0f,
		0.0f,				/* mu */
		FALSE				/* boolean - last entry in file? */
	};

	

	int n,
		loop,
		index;

	float sumxy,
		sumx2,
		x,
		y,
		sumx,
		sumy,
		dist,
		dist_square,
		atten,
		fx;

	/* the iridiums */
	for (loop = 0; loop < sspec.TA_count; loop++)
	{
		sspec.tissue_attenuation[loop] =
			POLY(sspec.TA_distance[loop],
				1.0128, 0.00519, -0.001178, -0.00002008);
	}
	

	sumxy = 0.0;
	sumx2 = 0.0;
	sumx = 0.0;
	sumy = 0.0;
	n = 0;
	for (loop = sspec.TA_count - 1;	loop > sspec.TA_count - 7 && loop > 0; loop--)
	{
		x = sspec.TA_distance[loop];
		y = log((double)sspec.tissue_attenuation[loop]);
		sumxy += x * y;
		sumx2 += x * x;
		sumx += x;
		sumy += y;
		n++;
	}
	sspec.mu = (sumxy - sumx * sumy / n) /
		(sumx2 - sumx * sumx / n);

	/*
	The following code (which is liberally borrowed from seed_pdose)
	calculates the dose_table on a millimeter vector.
	*/
	for (loop = 0; loop < SEED_RADII; loop++)
	{
		dist = ((float)loop) / 100.0;
		dist_square = dist * dist;
		if (dist_square < .00001)
			dist_square = .00001;

		if (sspec.TA_count < 2 ||dist > sspec.TA_distance[sspec.TA_count - 1])
			atten = (float)exp((double)(sspec.mu * dist));
		else
			atten = v_interp(0, sspec.TA_count,
				sspec.TA_distance,
				dist,
				sspec.tissue_attenuation,
				&index, &fx);

		sspec.seed_dose_table[loop] = sspec.gamma *	sspec.R_to_r * atten / dist_square;
	
	}

	m_Ir192Spec = sspec;
}


void vtkIr192SeedSource::UpdateDoseKernalVolume()
{
	m_DoseKernal = vtkImageData::New();

	//Calculate the Kernal Extent

 	int halfsize = int(m_cutoff / m_grid_spacing);

	halfsize = 1;

	m_DoseKernal->SetExtent(-halfsize, halfsize, -halfsize, halfsize, -halfsize, halfsize);
	int* extent = m_DoseKernal->GetExtent();

	// Fill every entry of the image data with x,y,z
	int* dims = m_DoseKernal->GetDimensions();
	int x_s = dims[0];
	int y_s = dims[1];
	int z_s = dims[2];

	m_DoseKernal->AllocateScalars(VTK_FLOAT,1);

	float dosevalue, Px, Py, Pz;

	for (int z = extent[4]; z<=extent[5]; z++)
	{
		for (int y = extent[2]; y<= extent[3]; y++)
		{
			for (int x = extent[0]; x<= extent[1]; x++)
			{		
				Px = x*m_grid_spacing*0.1;
				Py = y*m_grid_spacing*0.1;
				Pz = z*m_grid_spacing*0.1;
				if (x == 0 && y == 0 && z == 0)
				{
					Pz= m_grid_spacing*0.05; //Used for Center Voxel dose calculation
				}


			    dosevalue = seed_pdose(&m_Ir192Spec, 0, Px, Py, Pz);

				m_DoseKernal->SetScalarComponentFromFloat(x,y,z,0, dosevalue);

			}
		}
	}

	//Just for debug
	this->PrintROIDose(m_DoseKernal, extent);
}

vtkImageData * vtkIr192SeedSource::GetDoseKernalVolume()
{
	if (m_kernal_invalid)
	{
		this->UpdateDoseKernalVolume();
		m_kernal_invalid = false;

		return this->m_DoseKernal;
	}
	else
	{
		return this->m_DoseKernal;
	}

}

// Retrieve the entries from the image data and print them to the screen

void vtkIr192SeedSource::PrintROIDose(vtkImageData * data, int * extent) 
{
	
	int numComp = data->GetNumberOfScalarComponents();
		
	vtkImageIterator<float> it(data, extent);		

		while (!it.IsAtEnd())
		{
			float* valIt = it.BeginSpan();
			float *valEnd = it.EndSpan();
			while (valIt != valEnd)
			{
				
				float Dose = *valIt++;
			
				std::cout << "(Dose:" << Dose << ") ";
			}
			std::cout << std::endl;
			it.NextSpan();
		}
		

}
