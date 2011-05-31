#ifndef __vtkHoughPlanes_h
#define __vtkHoughPlanes_h

#include "vtkPolyDataAlgorithm.h"

class vtkHoughPlanes : public vtkPolyDataAlgorithm
{
public:
  enum HoughAlgorithmEnum {
  RHT, SHT, PHT, PPHT, APHT, RANSAC};

  vtkTypeMacro(vtkHoughPlanes,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkHoughPlanes *New();

protected:
  vtkHoughPlanes();
  ~vtkHoughPlanes(){}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  HoughAlgorithmEnum HoughAlgorithm;

private:
  vtkHoughPlanes(const vtkHoughPlanes&);  // Not implemented.
  void operator=(const vtkHoughPlanes&);  // Not implemented.

};

#endif