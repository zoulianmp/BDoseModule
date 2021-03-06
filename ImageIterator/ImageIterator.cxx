#include <vtkVersion.h>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageIterator.h>

#include <vtkIr192SeedSource.h>


int main(int, char *[])
{


  // Create an image data
  vtkSmartPointer<vtkImageData> imageData =
      vtkSmartPointer<vtkImageData>::New();

  // Specify the size of the image data
  imageData->SetDimensions(6,6,6);

  imageData->AllocateScalars(VTK_DOUBLE,1);


  // Fill every entry of the image data with x,y,z
  int* dims = imageData->GetDimensions();
  double *ptr = static_cast<double *>(imageData->GetScalarPointer(0, 0, 0));

  double * a;





  double zbase =0.0, ybase = 0.0;

  for (int z=0; z<dims[2]; z++)
    {
	

    for (int y=0; y<dims[1]; y++)
	{

      for (int x=0; x<dims[0]; x++)
        {
       
			*ptr = 0;
			ptr++;

        }
      }
    }

  imageData->SetScalarComponentFromDouble(2, 2, 2, 0, 222);
  imageData->SetScalarComponentFromDouble(3, 2, 2, 0, 223);
  imageData->SetScalarComponentFromDouble(2, 3, 2, 0, 232);
  imageData->SetScalarComponentFromDouble(3, 3, 2, 0, 233);


  imageData->SetScalarComponentFromDouble(2, 2, 3, 0, 322);
  imageData->SetScalarComponentFromDouble(3, 2, 3, 0, 323);
  imageData->SetScalarComponentFromDouble(2, 3, 3, 0, 332);
  imageData->SetScalarComponentFromDouble(3, 3, 3, 0, 333);

  // Define the extent to be extracted
  int extent[6];
  extent[0] = 2; extent[1] = 3;
  extent[2] = 2; extent[3] = 3;
  extent[4] = 2; extent[5] = 3;

  // Retrieve the entries from the image data and print them to the screen
  vtkImageIterator<double> it(imageData, extent);

  double Dose;


  while(!it.IsAtEnd())
    {
    double* valIt = it.BeginSpan();
    double *valEnd = it.EndSpan();


    while (valIt != valEnd)
      {
      // Increment for each component
    
	    Dose = *valIt++;

	 // std::cout <<  Dose ;
      }      

	//std::cout << std::endl;

    it.NextSpan();
    }
  

  return EXIT_SUCCESS;
}
