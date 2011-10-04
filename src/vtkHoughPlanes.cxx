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
#include <vtkVertexGlyphFilter.h>

#include <iostream>

#include "shape.h"
#include "ransac.h"

vtkStandardNewMacro(vtkHoughPlanes);

vtkHoughPlanes::vtkHoughPlanes()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);

  this->HoughAlgorithm = Randomized;
}

void vtkHoughPlanes::SetHoughAlgorithm(HoughAlgorithmEnum algorithm)
{
  this->HoughAlgorithm = algorithm;
}

void vtkHoughPlanes::SetMaxDist(double maxDist)
{
  this->m_Hough.myConfigFileHough.MaxDist = maxDist;
}

void vtkHoughPlanes::SetMinDist(double minDist)
{
  this->m_Hough.myConfigFileHough.MinDist = minDist;
}

void vtkHoughPlanes::SetAccumulatorMax(unsigned int accumulatorMax)
{
  this->m_Hough.myConfigFileHough.AccumulatorMax = accumulatorMax;
}

void vtkHoughPlanes::SetMinSizeAllPoints(unsigned int minSizeAllPoints)
{
  this->m_Hough.myConfigFileHough.MinSizeAllPoints = minSizeAllPoints;
}

void vtkHoughPlanes::SetRhoNum(unsigned int rhoNum)
{
  this->m_Hough.myConfigFileHough.RhoNum = rhoNum;
}

void vtkHoughPlanes::SetThetaNum(unsigned int thetaNum)
{
  this->m_Hough.myConfigFileHough.ThetaNum = thetaNum;
}

void vtkHoughPlanes::SetPhiNum(unsigned int phiNum)
{
  this->m_Hough.myConfigFileHough.PhiNum = phiNum;
}

void vtkHoughPlanes::SetRhoMax(unsigned int rhoMax)
{
  this->m_Hough.myConfigFileHough.RhoMax = rhoMax;
}

void vtkHoughPlanes::SetMaxPointPlaneDist(double maxPointPlaneDist)
{
  this->m_Hough.myConfigFileHough.MaxPointPlaneDist = maxPointPlaneDist;
}

void vtkHoughPlanes::SetMaxPlanes(unsigned int maxPlanes)
{
  this->m_Hough.myConfigFileHough.MaxPlanes = maxPlanes;
}

void vtkHoughPlanes::SetMinPlaneSize(unsigned int minPlaneSize)
{
  this->m_Hough.myConfigFileHough.MinPlaneSize = minPlaneSize;
}

void vtkHoughPlanes::SetMinPlanarity(double minPlanarity)
{
  this->m_Hough.myConfigFileHough.MinPlanarity = minPlanarity;
}

void vtkHoughPlanes::SetPlaneRatio(double planeRatio)
{
  this->m_Hough.myConfigFileHough.PlaneRatio = planeRatio;
}

void vtkHoughPlanes::SetPointDist(double pointDist)
{
  this->m_Hough.myConfigFileHough.PointDist = pointDist;
}

void vtkHoughPlanes::SetPeakWindow(bool peakWindow)
{
  this->m_Hough.myConfigFileHough.PeakWindow = peakWindow;
}

void vtkHoughPlanes::SetWindowSize(unsigned int windowSize)
{
  this->m_Hough.myConfigFileHough.WindowSize = windowSize;
}

void vtkHoughPlanes::SetTrashMax(unsigned int trashMax)
{
  this->m_Hough.myConfigFileHough.TrashMax = trashMax;
}

void vtkHoughPlanes::SetAccumulatorType(unsigned int accumulatorType)
{
  this->m_Hough.myConfigFileHough.AccumulatorType = accumulatorType;
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
  this->m_Hough.SetScan(&scan);

  switch(this->HoughAlgorithm) {
    case Randomized:
      this->m_Hough.RHT(); // Randomized Hough Transform
      break;
    case Standard:
      this->m_Hough.SHT(); // Standard Hough Transform
      break;
    case Probabilistic:
      this->m_Hough.PHT(); // Probabilistic Hough Transform
      break;
    case Progressive:
      this->m_Hough.PPHT(); // Progressive Probabilistic Hough Transform
      break;
    case Adaptive:
      this->m_Hough.APHT(); // Adaptive Probabilistic Hough Transform
      break;
  }

  //hough.writePlanes("output");
  //this->m_Hough.writePlanePoints("data/scans/scan000.3d");

  // Color the points to a vtp file with a random color for each plane
  vtkSmartPointer<vtkPoints> outputPoints = vtkSmartPointer<vtkPoints>::New();

  vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetNumberOfComponents(3);
  colors->SetName("Colors");

  // Color all points that belong to a plane a random color
  Point p;
  vector<Point>::iterator itr = this->m_Hough.coloredPoints.begin();
  while(itr != this->m_Hough.coloredPoints.end())
    {
    p = *(itr);
    colors->InsertNextTupleValue(p.rgb);
    outputPoints->InsertNextPoint(p.x, p.y, p.z);
    itr++;
    }

  // Color all points that do not belong to a plane gray
  itr = this->m_Hough.allPoints->begin();
  unsigned char gray[3] = {133,133,133};
  while(itr != this->m_Hough.allPoints->end())
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
