/*=========================================================================

   Program: ParaView
   Module:    pqParaWidget.h

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 
   
   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

========================================================================*/
#ifndef __pqParaWidget_h 
#define __pqParaWidget_h

#include "pq3DWidget.h"
#include "Leap/Include/Leap.h"
#include "Leap/VTK/vtkLeapController.h"

using namespace Leap;

class pqServer;
class pqView;
class pqProxyWidget;
/// Provides UI for Box Widget.
class PQCOMPONENTS_EXPORT pqParaWidget : public pq3DWidget
{
  Q_OBJECT
  typedef pq3DWidget Superclass;

//Functions and Objects needed for Leap Motion Interaction
public:
  void nextFilter();
private:
  pqProxyWidget* pwidget;
  vtkLeapController* leapController;
  Frame prevFrame;
  int tracking;
  int ctrl;
public slots:
  void newFrame(Frame frame);
  void changeControl();
public:
  pqParaWidget(vtkSMProxy* refProxy, vtkSMProxy* proxy, QWidget* p = 0);
  virtual ~pqParaWidget();
  /// Resets the bounds of the 3D widget to the reference proxy bounds.
  /// This typically calls PlaceWidget on the underlying 3D Widget 
  /// with reference proxy bounds.
  /// This should be explicitly called after the panel is created
  /// and the widget is initialized i.e. the reference proxy, controlled proxy
  /// and hints have been set.
  virtual void resetBounds(double bounds[6]);
  virtual void resetBounds()
    { this->Superclass::resetBounds(); }

  /// accept the changes. Overridden to hide handles.
  virtual void accept();

  /// reset the changes. Overridden to hide handles.
  virtual void reset();

  /// Overridden to update widget placement based on data bounds.
  virtual void select();

protected:
  /// Internal method to create the widget.
  void createWidget(pqServer*);

private slots:
  /// Called when the user changes widget visibility
  void onWidgetVisibilityChanged(bool visible);

  void showHandles();
  void hideHandles();
private:
  pqParaWidget(const pqParaWidget&); // Not implemented.
  void operator=(const pqParaWidget&); // Not implemented.

  class pqImplementation;
  pqImplementation* Implementation;
};

#endif


