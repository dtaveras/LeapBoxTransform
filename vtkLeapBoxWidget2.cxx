/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLeapBoxWidget2.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkLeapBoxWidget2.h"
#include "vtkLeapBoxRepresentation.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkEvent.h"
#include "vtkSMProxy.h"
#include "vtkPVRenderView.h"
#include "pqView.h"
#include "vtkWidgetEvent.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "pqActiveView.h"
#include <QDateTime>
#include <QDebug>
#include <cmath>
//#define DBG

#define SCALE 0.1

#define Y_BOT 90.0
#define Y_TOP 250.0

#define X_LEFT -130.0
#define X_RIGHT 130.0

#define Z_BACK -100.0
#define Z_FRONT 100.0
#define MAX_TRANSLATION 0.5

vtkStandardNewMacro(vtkLeapBoxWidget2);

//----------------------------------------------------------------------------
vtkLeapBoxWidget2::vtkLeapBoxWidget2()
{
  qDebug() << "vtkLeapBoxWidget2 Created";
  this->WidgetState = vtkLeapBoxWidget2::Start;
  this->ManagesCursor = 1;

  this->TranslationEnabled = 1;
  this->ScalingEnabled = 1;
  this->RotationEnabled = 1;
  this->MoveFacesEnabled = 1;

  // Define widget events
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
                                          vtkEvent::NoModifier,
                                          0, 0, NULL,
                                          vtkWidgetEvent::Select,
                                          this, vtkLeapBoxWidget2::SelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
                                          vtkEvent::NoModifier,
                                          0, 0, NULL,
                                          vtkWidgetEvent::EndSelect,
                                          this, vtkLeapBoxWidget2::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonPressEvent,
                                          vtkWidgetEvent::Translate,
                                          this, vtkLeapBoxWidget2::TranslateAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonReleaseEvent,
                                          vtkWidgetEvent::EndTranslate,
                                          this, vtkLeapBoxWidget2::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
                                          vtkEvent::ControlModifier,
                                          0, 0, NULL,
                                          vtkWidgetEvent::Translate,
                                          this, vtkLeapBoxWidget2::TranslateAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
                                            vtkEvent::ControlModifier,
                                            0, 0, NULL,
                                          vtkWidgetEvent::EndTranslate,
                                          this, vtkLeapBoxWidget2::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
                                          vtkEvent::ShiftModifier,
                                          0, 0, NULL,
                                          vtkWidgetEvent::Translate,
                                          this, vtkLeapBoxWidget2::TranslateAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
                                            vtkEvent::ShiftModifier,
                                            0, 0, NULL,
                                          vtkWidgetEvent::EndTranslate,
                                          this, vtkLeapBoxWidget2::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
                                          vtkWidgetEvent::Scale,
                                          this, vtkLeapBoxWidget2::ScaleAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
                                          vtkWidgetEvent::EndScale,
                                          this, vtkLeapBoxWidget2::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
                                          vtkWidgetEvent::Move,
                                          this, vtkLeapBoxWidget2::MoveAction);
}

//----------------------------------------------------------------------------
vtkLeapBoxWidget2::~vtkLeapBoxWidget2()
{

}

//----------------------------------------------------------------------------
vtkWidgetRepresentation* vtkLeapBoxWidget2::GetWidgetRep(){
  return this->WidgetRep;
}

//----------------------------------------------------------------------
void vtkLeapBoxWidget2::SelectAction(vtkAbstractWidget *w)
{
  // We are in a static method, cast to ourself
  vtkLeapBoxWidget2 *self = reinterpret_cast<vtkLeapBoxWidget2*>(w);

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  if ( !self->CurrentRenderer ||
       !self->CurrentRenderer->IsInViewport(X,Y) )
    {
    self->WidgetState = vtkLeapBoxWidget2::Start;
    return;
    }

  // Begin the widget interaction which has the side effect of setting the
  // interaction state.
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  self->WidgetRep->StartWidgetInteraction(e);
  int interactionState = self->WidgetRep->GetInteractionState();
  if ( interactionState == vtkLeapBoxRepresentation::Outside )
    {
    return;
    }

  // Test for states that involve face or handle picking here so
  // selection highlighting doesn't happen if that interaction is disabled.
  // Non-handle-grabbing transformations are tested in the "Action" methods.

  // Rotation
  if (interactionState == vtkLeapBoxRepresentation::Rotating
       && self->RotationEnabled == 0)
  {
    return;
  }
  // Face Movement
  if ((interactionState == vtkLeapBoxRepresentation::MoveF0 ||
       interactionState == vtkLeapBoxRepresentation::MoveF1 ||
       interactionState == vtkLeapBoxRepresentation::MoveF2 ||
       interactionState == vtkLeapBoxRepresentation::MoveF3 ||
       interactionState == vtkLeapBoxRepresentation::MoveF4 ||
       interactionState == vtkLeapBoxRepresentation::MoveF5)
        && self->MoveFacesEnabled == 0)
  {
    return;
  }
  // Translation
  if (interactionState == vtkLeapBoxRepresentation::Translating
       && self->TranslationEnabled == 0)
  {
    return;
  }

  // We are definitely selected
  self->WidgetState = vtkLeapBoxWidget2::Active;
  self->GrabFocus(self->EventCallbackCommand);

  // The SetInteractionState has the side effect of highlighting the widget
  reinterpret_cast<vtkLeapBoxRepresentation*>(self->WidgetRep)->
    SetInteractionState(interactionState);

  // start the interaction
  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  self->Render();
}

//----------------------------------------------------------------------
void vtkLeapBoxWidget2::TranslateAction(vtkAbstractWidget *w)
{
  // We are in a static method, cast to ourself
  vtkLeapBoxWidget2 *self = reinterpret_cast<vtkLeapBoxWidget2*>(w);

  if (self->TranslationEnabled == 0)
  {
    return;
  }

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  if ( !self->CurrentRenderer ||
       !self->CurrentRenderer->IsInViewport(X,Y) )
    {
    self->WidgetState = vtkLeapBoxWidget2::Start;
    return;
    }

  // Begin the widget interaction which has the side effect of setting the
  // interaction state.
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  self->WidgetRep->StartWidgetInteraction(e);
  int interactionState = self->WidgetRep->GetInteractionState();
  if ( interactionState == vtkLeapBoxRepresentation::Outside )
    {
    return;
    }

  // We are definitely selected
  self->WidgetState = vtkLeapBoxWidget2::Active;
  self->GrabFocus(self->EventCallbackCommand);
  reinterpret_cast<vtkLeapBoxRepresentation*>(self->WidgetRep)->
    SetInteractionState(vtkLeapBoxRepresentation::Translating);

  // start the interaction
  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  self->Render();
}

//----------------------------------------------------------------------
void vtkLeapBoxWidget2::ScaleAction(vtkAbstractWidget *w)
{
  // We are in a static method, cast to ourself
  vtkLeapBoxWidget2 *self = reinterpret_cast<vtkLeapBoxWidget2*>(w);

  if (self->ScalingEnabled == 0)
  {
    return;
  }

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  if ( !self->CurrentRenderer ||
       !self->CurrentRenderer->IsInViewport(X,Y) )
    {
    self->WidgetState = vtkLeapBoxWidget2::Start;
    return;
    }

  // Begin the widget interaction which has the side effect of setting the
  // interaction state.
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  self->WidgetRep->StartWidgetInteraction(e);
  int interactionState = self->WidgetRep->GetInteractionState();
  if ( interactionState == vtkLeapBoxRepresentation::Outside )
    {
    return;
    }

  // We are definitely selected
  self->WidgetState = vtkLeapBoxWidget2::Active;
  self->GrabFocus(self->EventCallbackCommand);
  reinterpret_cast<vtkLeapBoxRepresentation*>(self->WidgetRep)->
    SetInteractionState(vtkLeapBoxRepresentation::Scaling);

  // start the interaction
  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  self->Render();
}

//----------------------------------------------------------------------
void vtkLeapBoxWidget2::MoveAction(vtkAbstractWidget *w)
{
  vtkLeapBoxWidget2 *self = reinterpret_cast<vtkLeapBoxWidget2*>(w);

  // See whether we're active
  if ( self->WidgetState == vtkLeapBoxWidget2::Start )
    {
    return;
    }

  // compute some info we need for all cases
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // Okay, adjust the representation
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  self->WidgetRep->WidgetInteraction(e);

  // moving something
  self->EventCallbackCommand->SetAbortFlag(1);
  self->InvokeEvent(vtkCommand::InteractionEvent,NULL);
  self->Render();
}

//----------------------------------------------------------------------
void vtkLeapBoxWidget2::EndSelectAction(vtkAbstractWidget *w)
{
  vtkLeapBoxWidget2 *self = reinterpret_cast<vtkLeapBoxWidget2*>(w);
  if ( self->WidgetState == vtkLeapBoxWidget2::Start )
    {
    return;
    }

  // Return state to not active
  self->WidgetState = vtkLeapBoxWidget2::Start;
  reinterpret_cast<vtkLeapBoxRepresentation*>(self->WidgetRep)->
    SetInteractionState(vtkLeapBoxRepresentation::Outside);
  self->ReleaseFocus();

  self->EventCallbackCommand->SetAbortFlag(1);
  self->EndInteraction();
  self->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  self->Render();
}

//----------------------------------------------------------------------
void vtkLeapBoxWidget2::CreateDefaultRepresentation()
{
  if ( ! this->WidgetRep )
    {
    this->WidgetRep = vtkLeapBoxRepresentation::New();
    }
}

//----------------------------------------------------------------------------
void vtkLeapBoxWidget2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Translation Enabled: " << (this->TranslationEnabled ? "On\n" : "Off\n");
  os << indent << "Scaling Enabled: " << (this->ScalingEnabled ? "On\n" : "Off\n");
  os << indent << "Rotation Enabled: " << (this->RotationEnabled ? "On\n" : "Off\n");
  os << indent << "Move Faces Enabled: " << (this->MoveFacesEnabled ? "On\n" : "Off\n");
}


