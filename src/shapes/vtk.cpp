/**
 * @file 
 * @author Dorit Borrmann. Institute of Computer Science, University of Osnabrueck, Germany.
*/


#include <vtkXMLPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <iostream>

#include "shapes/hough.h"
#include "shapes/shape.h"
#include "shapes/ransac.h"

enum plane_alg { 
  RHT, SHT, PHT, PPHT, APHT, RANSAC
};

int main(int argc, char *argv[])
{
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
  Hough hough(&scan);
  
  std::cout << "Created scan" << std::endl;
  
  // Choose Hough method here
  plane_alg alg = SHT;
    
  switch(alg) 
  {
    case RHT: hough.RHT(); // Randomized Hough Transform
              break;
    case SHT: hough.SHT(); // Standard Hough Transform
              break;
    case PHT: hough.PHT(); // Probabilistic Hough Transform
              break;
    case PPHT:  hough.PPHT(); // Progressive Probabilistic Hough Transform
                break;
    case APHT:  hough.APHT(); // Adaptive Probabilistic Hough Transform
                break;
  }

  std::cout << "Writing planes..." << std::endl;
  hough.writePlanes("output.txt");
  
  return 0;
}
