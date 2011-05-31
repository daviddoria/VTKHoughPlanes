/**
 * This program accepts a vtp file containing a point cloud and attempts to fit many planes
 * to the points. The output is a vtp file containing the input points colored by which plane,
 * if any, they belong to. The colors of the planes are randomly assigned.
 */ 

#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

#include "vtkHoughPlanes.h"

int main(int argc, char *argv[])
{
  // Verify command line arguments
  if(argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " input.vtp output.vtp" << endl;
    exit(1);
  }

  // Parse command line arguments
  std::string inputFileName = argv[1];
  std::string outputFileName = argv[2];

  // Read the input file
  vtkSmartPointer<vtkXMLPolyDataReader> reader =
    vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName(inputFileName.c_str());
  reader->Update();
  
  vtkSmartPointer<vtkHoughPlanes> houghPlanes =
    vtkSmartPointer<vtkHoughPlanes>::New();
  houghPlanes->SetInputConnection(reader->GetOutputPort());
  houghPlanes->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetInputConnection(houghPlanes->GetOutputPort());
  writer->SetFileName(outputFileName.c_str());
  writer->Write();
  
  return EXIT_SUCCESS;
}
