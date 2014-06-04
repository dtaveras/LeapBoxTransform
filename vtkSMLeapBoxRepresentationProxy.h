/*=========================================================================

  Program:   ParaView
  Module:    vtkSMLeapBoxRepresentationProxy.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMLeapBoxRepresentationProxy - proxy for vtkBoxRepresentation
// .SECTION Description
// vtkSMLeapBoxRepresentationProxy is a proxy for vtkBoxRepresentation. A
// specialization is needed to set the tranform on the vtkBoxRepresentation.

#ifndef __vtkSMLeapBoxRepresentationProxy_h
#define __vtkSMLeapBoxRepresentationProxy_h

#include "vtkPVServerManagerRenderingModule.h" //needed for exports
#include "vtkSMWidgetRepresentationProxy.h"

class VTKPVSERVERMANAGERRENDERING_EXPORT vtkSMLeapBoxRepresentationProxy : public vtkSMWidgetRepresentationProxy
{
public:
  static vtkSMLeapBoxRepresentationProxy* New();
  vtkTypeMacro(vtkSMLeapBoxRepresentationProxy, vtkSMWidgetRepresentationProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void UpdateVTKObjects();
  virtual void UpdatePropertyInformation();
  virtual void UpdatePropertyInformation(vtkSMProperty* prop)
    { this->Superclass::UpdatePropertyInformation(prop); }

//BTX
protected:
  vtkSMLeapBoxRepresentationProxy();
  ~vtkSMLeapBoxRepresentationProxy();

  // This method is overridden to set the transform on the vtkWidgetRepresentation.
  virtual void CreateVTKObjects();

private:
  vtkSMLeapBoxRepresentationProxy(const vtkSMLeapBoxRepresentationProxy&); // Not implemented
  void operator=(const vtkSMLeapBoxRepresentationProxy&); // Not implemented
//ETX
};

#endif

