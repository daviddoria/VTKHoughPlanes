#ifndef __vtkHoughPlanes_h
#define __vtkHoughPlanes_h

#include "vtkPolyDataAlgorithm.h"

#include "hough.h"

class vtkHoughPlanes : public vtkPolyDataAlgorithm
{
public:
  enum HoughAlgorithmEnum {
  Randomized, Standard, Probabilistic, Progressive, Adaptive};

  vtkTypeMacro(vtkHoughPlanes,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkHoughPlanes *New();
  
  void SetMaxDist(double ); // maximum distance between points
  void SetMinDist(double ); // minimum distance between points
  void SetAccumulatorMax(unsigned int ); // plane detected if Hough cell reaches AccumulatorMax
  void SetMinSizeAllPoints(unsigned int ); // plane detection stops if only this percentage of points are left
  void SetRhoNum(unsigned int ); // number of cells in the Hough array in the rho dimension
  void SetThetaNum(unsigned int ); // number of cells in the Hough array in the theta dimension
  void SetPhiNum(unsigned int ); // number of cells in the Hough array in the phi dimension
  void SetRhoMax(unsigned int ); // maximum distance between planes and origin
  void SetMaxPointPlaneDist(double ); // maximal noise
  void SetMaxPlanes(unsigned int ); // maximum number of planes to be detected
  void SetMinPlaneSize(unsigned int ); // minimum number of points for a plane to be still considered
  void SetMinPlanarity(double ); // minimum value of the smallest eigenvalue after plane fitting
  void SetPlaneRatio(double ); // consider planes with a counter higher than PlaneRatio*MaxCounter
  void SetPointDist(double ); // threshold for region growing
  void SetPeakWindow(bool ); // true if sliding window for peak detection is desired
  void SetWindowSize(unsigned int ); // side length of sliding window
  void SetTrashMax(unsigned int ); // number of not considerd planes before abort
  void SetAccumulatorType(unsigned int ); // Array 0, Ball 1, Cube 2, BallI 3

  void SetHoughAlgorithm(HoughAlgorithmEnum); // Specify which version of the Hough transform algorithm to use
  
protected:
  vtkHoughPlanes();
  ~vtkHoughPlanes(){}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  HoughAlgorithmEnum HoughAlgorithm;

  Hough m_Hough;
  
private:
  vtkHoughPlanes(const vtkHoughPlanes&);  // Not implemented.
  void operator=(const vtkHoughPlanes&);  // Not implemented.

};

#endif
