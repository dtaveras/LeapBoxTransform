/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkNewTransformFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkNewTransformFilter - transform points and associated normals and vectors
// .SECTION Description
// vtkNewTransformFilter is a filter to transform point coordinates, and
// associated point normals and vectors. Other point data is passed
// through the filter.
//
// An alternative method of transformation is to use vtkActor's methods
// to scale, rotate, and translate objects. The difference between the
// two methods is that vtkActor's transformation simply effects where
// objects are rendered (via the graphics pipeline), whereas
// vtkNewTransformFilter actually modifies point coordinates in the
// visualization pipeline. This is necessary for some objects
// (e.g., vtkProbeFilter) that require point coordinates as input.

// .SECTION See Also
// vtkAbstractTransform vtkTransformPolyDataFilter vtkActor

#ifndef __vtkNewTransformFilter_h
#define __vtkNewTransformFilter_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPointSetAlgorithm.h"

class vtkAbstractTransform;

class VTKFILTERSGENERAL_EXPORT vtkNewTransformFilter : public vtkPointSetAlgorithm
{
public:
  static vtkNewTransformFilter *New();
  vtkTypeMacro(vtkNewTransformFilter,vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Return the MTime also considering the transform.
  unsigned long GetMTime();

  // Description:
  // Specify the transform object used to transform points.
  virtual void SetTransform(vtkAbstractTransform*);
  vtkGetObjectMacro(Transform,vtkAbstractTransform);

  virtual int FillInputPortInformation(int port, vtkInformation *info);

protected:
  vtkNewTransformFilter();
  ~vtkNewTransformFilter();

  int RequestDataObject(vtkInformation *request,
                        vtkInformationVector **inputVector,
                        vtkInformationVector *outputVector);
  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *);
  vtkAbstractTransform *Transform;
private:
  vtkNewTransformFilter(const vtkNewTransformFilter&);  // Not implemented.
  void operator=(const vtkNewTransformFilter&);  // Not implemented.
};

#endif
