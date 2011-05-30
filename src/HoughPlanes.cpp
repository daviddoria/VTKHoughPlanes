/**
 * @file 
 * @author Dorit Borrmann. Institute of Computer Science, University of Osnabrueck, Germany.
*/


#include <vtkXMLPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkVertexGlyphFilter.h>

#include <iostream>

#include "hough.h"
#include "shape.h"
#include "ransac.h"

enum plane_alg { 
  RHT, SHT, PHT, PPHT, APHT, RANSAC
};

void writeVTP(Hough& hough, const std::string &outputFilename);

int main(int argc, char *argv[])
{
  // Verify command line arguments
  if(argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " input.vtp output.vtp" << endl;
    exit(1);
  }

  // Parse command line arguments
  std::string inputFilename = argv[1];
  std::string outputFilename = argv[2];
  
  Scan scan;
  
  // Read the input file
  vtkSmartPointer<vtkXMLPolyDataReader> reader =
    vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName(inputFilename.c_str());
  reader->Update();
  
  vtkPolyData* polydata = reader->GetOutput();
 
  std::cout << "Read file." << std::endl;
  
  vector<Point> points;
  
  // Convert the points to a 'Scan' object needed by the HoughPlanes code
  for(vtkIdType pointIndex = 0; pointIndex < polydata->GetNumberOfPoints(); ++pointIndex)
  {
    double data[3];
    polydata->GetPoint(pointIndex,data);
    Point point(data);
    points.push_back(point);
  }
  
  scan.setPoints(&points);
  scan.toGlobal(false,-1);
  Hough hough(&scan, true);

  //hough.SHT(); // Standard Hough Transform
  hough.RHT(); // Randomized Hough Transform
  
  std::cout << "Writing planes..." << std::endl;
  hough.writePlanes("output");
  hough.writePlanePoints("data/scans/scan000.3d"); 

  // Write all points to a vtp file with a random color for each plane
  writeVTP(hough, outputFilename);

  return 0;
}


void writeVTP(Hough& hough, const std::string &outputFilename) {
  vtkSmartPointer<vtkPoints> outputdata = vtkSmartPointer<vtkPoints>::New();

  vtkSmartPointer<vtkUnsignedCharArray> colordata = vtkSmartPointer<vtkUnsignedCharArray>::New();
  colordata->SetNumberOfComponents(3);
  colordata->SetName("Colors");

  Point p;
  vector<Point>::iterator itr = hough.coloredPoints.begin();
  while(itr != hough.coloredPoints.end()) {
    p = *(itr);
    colordata->InsertNextTupleValue(p.rgb);
    outputdata->InsertNextPoint(p.x, p.y, p.z);
    itr++;
  }
  itr = hough.allPoints->begin();
  unsigned char gray[3] = {133,133,133};
  while(itr != hough.allPoints->end()) {
    p = *(itr);
    colordata->InsertNextTupleValue(gray);
    outputdata->InsertNextPoint(p.x, p.y, p.z);
    itr++;
  }

  vtkSmartPointer<vtkPolyData> firstpolydata = vtkSmartPointer<vtkPolyData>::New();
  firstpolydata->SetPoints(outputdata);

  vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexFilter->SetInputConnection(firstpolydata->GetProducerPort());
  vertexFilter->Update();

  vtkSmartPointer<vtkPolyData> polyoutputdata = vtkSmartPointer<vtkPolyData>::New();
  polyoutputdata->ShallowCopy(vertexFilter->GetOutput());

  polyoutputdata->GetPointData()->SetScalars(colordata);

  vtkSmartPointer<vtkPolyDataMapper> colorMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  colorMapper->SetInput(polyoutputdata);

  vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetInput(polyoutputdata);
  writer->SetFileName(outputFilename.c_str());
  writer->Write();
}
