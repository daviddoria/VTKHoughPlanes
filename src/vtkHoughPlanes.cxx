#include "vtkHoughPlanes.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkVertexGlyphFilter.h>

#include <iostream>

#include "hough.h"
#include "shape.h"
#include "ransac.h"

vtkStandardNewMacro(vtkHoughPlanes);

vtkHoughPlanes::vtkHoughPlanes()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
}

int vtkHoughPlanes::RequestData(vtkInformation *vtkNotUsed(request),
                                             vtkInformationVector **inputVector,
                                             vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *output = vtkPolyData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Create the objects necessary for 3DTK
  Scan scan;
  vector<Point> points;

  // Convert the points to a 'Scan' object needed by the HoughPlanes code
  for(vtkIdType pointIndex = 0; pointIndex < input->GetNumberOfPoints(); ++pointIndex)
    {
    double data[3];
    input->GetPoint(pointIndex,data);
    Point point(data);
    points.push_back(point);
    }

  scan.setPoints(&points);
  scan.toGlobal(false,-1);
  Hough hough(&scan, true);

  //hough.SHT(); // Standard Hough Transform
  hough.RHT(); // Randomized Hough Transform

  std::cout << "Writing planes..." << std::endl;
  //hough.writePlanes("output");
  //hough.writePlanePoints("data/scans/scan000.3d");

  // Color the points to a vtp file with a random color for each plane
  vtkSmartPointer<vtkPoints> outputPoints = vtkSmartPointer<vtkPoints>::New();

  vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetNumberOfComponents(3);
  colors->SetName("Colors");

  // Color all points that belong to a plane a random color
  Point p;
  vector<Point>::iterator itr = hough.coloredPoints.begin();
  while(itr != hough.coloredPoints.end())
    {
    p = *(itr);
    colors->InsertNextTupleValue(p.rgb);
    outputPoints->InsertNextPoint(p.x, p.y, p.z);
    itr++;
    }

  // Color all points that do not belong to a plane gray
  itr = hough.allPoints->begin();
  unsigned char gray[3] = {133,133,133};
  while(itr != hough.allPoints->end())
    {
    p = *(itr);
    colors->InsertNextTupleValue(gray);
    outputPoints->InsertNextPoint(p.x, p.y, p.z);
    itr++;
    }

  vtkSmartPointer<vtkPolyData> outputPointsPolyData = vtkSmartPointer<vtkPolyData>::New();
  outputPointsPolyData->SetPoints(outputPoints);
  outputPointsPolyData->GetPointData()->SetScalars(colors);
  
  vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexFilter->SetInputConnection(outputPointsPolyData->GetProducerPort());
  vertexFilter->Update();

  output->ShallowCopy(vertexFilter->GetOutput());

  return 1;
}


//----------------------------------------------------------------------------
void vtkHoughPlanes::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
