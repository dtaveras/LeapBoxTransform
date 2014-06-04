/*=========================================================================

  Program:   ParaView
  Module:    vtkSMLeapBoxRepresentationProxy.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMLeapBoxRepresentationProxy.h"

#include "vtkLeapBoxRepresentation.h"
#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkSMSession.h"
#include "vtkTransform.h"
#include <QDebug>

vtkStandardNewMacro(vtkSMLeapBoxRepresentationProxy);
//----------------------------------------------------------------------------
vtkSMLeapBoxRepresentationProxy::vtkSMLeapBoxRepresentationProxy()
{
}

//----------------------------------------------------------------------------
vtkSMLeapBoxRepresentationProxy::~vtkSMLeapBoxRepresentationProxy()
{
}

//----------------------------------------------------------------------------
void vtkSMLeapBoxRepresentationProxy::CreateVTKObjects()
{
  if (this->ObjectsCreated)
    {
    return;
    }

  this->Superclass::CreateVTKObjects();

  vtkClientServerStream stream;
  stream << vtkClientServerStream::Invoke
         << VTKOBJECT(this)
         << "SetTransform"
         << VTKOBJECT(this->GetSubProxy("Transform"))
         << vtkClientServerStream::End;
  this->ExecuteStream(stream);
}

//----------------------------------------------------------------------------
void vtkSMLeapBoxRepresentationProxy::UpdateVTKObjects()
{
  if (this->InUpdateVTKObjects)
    {
    return;
    }

  int something_changed = this->ArePropertiesModified();

  this->Superclass::UpdateVTKObjects();

  if (something_changed)
    {
    vtkClientServerStream stream;
    stream << vtkClientServerStream::Invoke
      << VTKOBJECT(this)
      << "SetTransform"
      << VTKOBJECT(this->GetSubProxy("Transform"))
      << vtkClientServerStream::End;
    this->ExecuteStream(stream);
    }
}

//----------------------------------------------------------------------------
void vtkSMLeapBoxRepresentationProxy::UpdatePropertyInformation()
{
  vtkLeapBoxRepresentation* repr = vtkLeapBoxRepresentation::SafeDownCast(
    this->GetClientSideObject());
  vtkTransform* transform = vtkTransform::SafeDownCast(
      this->GetSubProxy("Transform")->GetClientSideObject());
      repr->GetTransform(transform);

  this->Superclass::UpdatePropertyInformation();
}

//----------------------------------------------------------------------------
void vtkSMLeapBoxRepresentationProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


