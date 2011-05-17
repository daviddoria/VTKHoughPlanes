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

#include "shapes/hough.h"
#include "shapes/shape.h"
#include "shapes/ransac.h"

enum plane_alg { 
  RHT, SHT, PHT, PPHT, APHT, RANSAC
};

void writeVTP(Hough& hough) {
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
  writer->SetFileName("outputtest.vtp");
  writer->Write();
}

int main(int argc, char *argv[])
{
  if(argc < 2) {
    std::cerr << "Usage: " << argv[0] << " filename.vtp" << endl;
    exit(1);
  }
  std::string filename = argv[1];
  
  Scan scan;
  
  // Read the input file
  vtkSmartPointer<vtkXMLPolyDataReader> reader =
    vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName(filename.c_str());
  reader->Update();
  
  vtkPolyData* polydata = reader->GetOutput();
 
  std::cout << "Read file." << std::endl;
  
  vector<Point> points;
  // Convert the points to a Scan object
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
  //hough.writeAllPoints(0, *(hough.allPoints)); 
  std::cout << "Created scan" << std::endl;
  //hough.SHT(); // Standard Hough Transform
  hough.RHT(); // Randomized Hough Transform
  
  std::cout << "Writing planes..." << std::endl;
  hough.writePlanes("output");

  // writes all points vtp file points colored by plane
  writeVTP(hough);

  // writes all points as xyzrgb ascii file points colored by plane
  hough.writePlanePoints("output");
  
  return 0;
}
