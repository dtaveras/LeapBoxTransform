/*=========================================================================

   Program: ParaView
   Module:    pqParaWidget.cxx

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
//Local Includes
#include "pqParaWidget.h"
#include "ui_pqParaWidget.h"
#include "vtkLeapBoxWidget2.h"
#include "vtkLeapBoxRepresentation.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"

// Server Manager Includes.
#include "vtkSMNewWidgetRepresentationProxy.h"
#include "vtkSMPropertyHelper.h"

// Qt Includes.
#include <QDoubleValidator>
#include <QDebug>

// ParaView Includes.
#include "pq3DWidgetFactory.h"
#include "pqApplicationCore.h"
#include "pqPipelineSource.h"
#include "pqServerManagerModel.h"
#include "pqProxyWidget.h"
#include "pqDisplayPolicy.h"
#include "pqDataRepresentation.h"
#include "pqPipelineFilter.h"
#include "pqSettings.h"
#include "pqProxyModifiedStateUndoElement.h"
#include "pqView.h"
#include "pqOutputPort.h"
#include "pqPropertyLinks.h"
#include "pqServer.h"
#include "pqActiveObjects.h"
#include "pqServerManagerModel.h"
#include "pqSMAdaptor.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMProperty.h"
#include "vtkSMProxy.h"
#include "vtkPVRenderView.h"
//C++ Includes
#include <cmath>

#include <vtkPolyDataMapper.h>
#include <vtkObjectFactory.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkInteractorStyleTrackballCamera.h>

#define SCALE 0.1

// Define interaction style
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
private:
  pqParaWidget* widget;
public:
  //static KeyPressInteractorStyle* New();
  vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);
  KeyPressInteractorStyle(pqParaWidget* widget);
  virtual void OnKeyPress() 
  {
    // Get the keypress
    vtkRenderWindowInteractor *rwi = this->Interactor;
    std::string key = rwi->GetKeySym();
 
    // Output the key that was pressed
    qDebug() << "Pressed " << key.c_str();
 
    // Handle an arrow key
    if(key == "Up")
      {
	qDebug() << "The up arrow was pressed.";
      }
 
    // Handle a "normal" key
    if(key == "Return")
      {
	qDebug() << "The a key was pressed.";
	widget->nextFilter();
      }
 
    // Forward events
    vtkInteractorStyleTrackballCamera::OnKeyPress();
  }
 
};
//vtkStandardNewMacro(KeyPressInteractorStyle);
KeyPressInteractorStyle::KeyPressInteractorStyle(pqParaWidget* widget){
  this->widget = widget;
}

class pqParaWidget::pqImplementation : public Ui::pqParaWidget
{
public:
  pqPropertyLinks Links;
};


#define PVBOXWIDGET_TRIGGER_RENDER(ui)  \
  QObject::connect(this->Implementation->ui,\
    SIGNAL(editingFinished()),\
    this, SLOT(render()), Qt::QueuedConnection);
//-----------------------------------------------------------------------------
pqParaWidget::pqParaWidget(vtkSMProxy* refProxy, vtkSMProxy* pxy, QWidget* _parent) :
  Superclass(refProxy, pxy, _parent)
{
  qDebug() << "The Widget";
  this->pwidget = NULL;
  this->ctrl = 0;
  this->tracking = 0;

  this->Implementation = new pqImplementation();
  this->Implementation->setupUi(this);
  this->Implementation->show3DWidget->setChecked(this->widgetVisible());  

  // Setup validators for all line edits.
  QDoubleValidator* validator = new QDoubleValidator(this);
  this->Implementation->positionX->setValidator(validator);
  this->Implementation->positionY->setValidator(validator);
  this->Implementation->positionZ->setValidator(validator);
  this->Implementation->scaleX->setValidator(validator);
  this->Implementation->scaleY->setValidator(validator);
  this->Implementation->scaleZ->setValidator(validator);
  this->Implementation->rotationX->setValidator(validator);
  this->Implementation->rotationY->setValidator(validator);
  this->Implementation->rotationZ->setValidator(validator);

  PVBOXWIDGET_TRIGGER_RENDER(positionX);
  PVBOXWIDGET_TRIGGER_RENDER(positionY);
  PVBOXWIDGET_TRIGGER_RENDER(positionZ);
  PVBOXWIDGET_TRIGGER_RENDER(scaleX);
  PVBOXWIDGET_TRIGGER_RENDER(scaleY);
  PVBOXWIDGET_TRIGGER_RENDER(scaleZ);
  PVBOXWIDGET_TRIGGER_RENDER(rotationX);
  PVBOXWIDGET_TRIGGER_RENDER(rotationY);
  PVBOXWIDGET_TRIGGER_RENDER(rotationZ);

  QObject::connect(this->Implementation->show3DWidget,
    SIGNAL(toggled(bool)), this, SLOT(setWidgetVisible(bool)));

  QObject::connect(this, SIGNAL(widgetVisibilityChanged(bool)),
    this, SLOT(onWidgetVisibilityChanged(bool)));

  QObject::connect(this->Implementation->resetBounds,
    SIGNAL(clicked()), this, SLOT(resetBounds()));

  QObject::connect(this->Implementation->changeControl,
		   SIGNAL(clicked()), this, SLOT(changeControl()));

  //QObject::connect(this, SIGNAL(widgetStartInteraction()),
  //  this, SLOT(showHandles()));

  QObject::connect(&this->Implementation->Links, SIGNAL(qtWidgetChanged()),
    this, SLOT(setModified()));

  pqServerManagerModel* smmodel =
    pqApplicationCore::instance()->getServerManagerModel();
  this->createWidget(smmodel->findServer(refProxy->GetSession()));
}

//-----------------------------------------------------------------------------
pqParaWidget::~pqParaWidget()
{
  delete this->Implementation;
  delete this->leapController;
}

#define PVBOXWIDGET_LINK(ui, smproperty, index)\
{\
  this->Implementation->Links.addPropertyLink(\
    this->Implementation->ui, "text2",\
    SIGNAL(textChanged(const QString&)),\
    widget, widget->GetProperty(smproperty), index);\
}

//-----------------------------------------------------------------------------
void pqParaWidget::createWidget(pqServer* server)
{
  qDebug() << "creating Widget";
  vtkSMNewWidgetRepresentationProxy* widget =
    pqApplicationCore::instance()->get3DWidgetFactory()->
    get3DWidget("theWidgetRepresentation", server, this->getReferenceProxy());
  this->setWidgetProxy(widget);

  widget->UpdateVTKObjects();
  widget->UpdatePropertyInformation();

  PVBOXWIDGET_LINK(positionX, "Position", 0);
  PVBOXWIDGET_LINK(positionY, "Position", 1);
  PVBOXWIDGET_LINK(positionZ, "Position", 2);

  PVBOXWIDGET_LINK(rotationX, "Rotation", 0);
  PVBOXWIDGET_LINK(rotationY, "Rotation", 1);
  PVBOXWIDGET_LINK(rotationZ, "Rotation", 2);

  PVBOXWIDGET_LINK(scaleX, "Scale", 0);
  PVBOXWIDGET_LINK(scaleY, "Scale", 1);
  PVBOXWIDGET_LINK(scaleZ, "Scale", 2);

  //Initialize and Connect Leap Motion after all other things are Setup
  leapController = new vtkLeapController;
  QObject::connect(leapController, SIGNAL(gotFrame(Frame)), this, SLOT(newFrame(Frame)));

  pqActiveObjects & objects = pqActiveObjects::instance();
  
  pqView* view = objects.activeView();
  vtkSMProxy* tmpProxy = view->getProxy();
  vtkPVRenderView* clientView = vtkPVRenderView::SafeDownCast(
				 tmpProxy->GetClientSideObject());
  //vtkCamera* clientCamera = clientView->GetActiveCamera();
  vtkRenderWindowInteractor* renderWindowInteractor = (vtkRenderWindowInteractor*)clientView->GetInteractor();
  KeyPressInteractorStyle* style = new KeyPressInteractorStyle(this);
  renderWindowInteractor->SetInteractorStyle(style);
  style->SetCurrentRenderer(clientView->GetRenderer());
}

void pqParaWidget::nextFilter(){
  pqApplicationCore *core = pqApplicationCore::instance();
  pqServerManagerModel *smModel = core->getServerManagerModel();
  int numFilters = smModel->getNumberOfItems<pqPipelineFilter*>();
  QList<pqPipelineFilter*> filters = smModel->findItems<pqPipelineFilter*>();
  pqActiveObjects & objects = pqActiveObjects::instance();
  pqPipelineSource* curActiveSource = objects.activeSource();
  
  int index = -1;
  for(int i=0; i < numFilters; i++){
    if((unsigned long long int)filters[i] == (unsigned long long int)curActiveSource){
      qDebug() << "Found";
      index = i;
    }
  }

  if(index == -1)
    return;
  
  if(index == numFilters-1)
    index = 0;
  else
    index += 1;
  
  objects.setActiveSource(filters[index]);
  return;
}

//-----------------------------------------------------------------------------
// Switches from rotation to translation
void pqParaWidget::changeControl(){
  qDebug() << "Switch Control";
  ctrl = ctrl > 1 ? 0 : ctrl+1;
  qDebug() << "ctrl: " << ctrl;

  /*pqApplicationCore *core = pqApplicationCore::instance();
  pqServerManagerModel *smModel = core->getServerManagerModel();
  QList<pqPipelineSource*> sources = smModel->findItems<pqPipelineSource*>();
  QList<pqPipelineFilter*> filters = smModel->findItems<pqPipelineFilter*>();

  int numSources = smModel->getNumberOfItems<pqPipelineSource*>();
  int numFilters = smModel->getNumberOfItems<pqPipelineFilter*>();
  qDebug() << "numFilters: " << numFilters;
  qDebug() << "numSources: " << numSources;
  pqPipelineSource* tmpSource = sources[0];
  pqActiveObjects & objects = pqActiveObjects::instance();
  
  //objects.setActiveSource(tmpSource);
  qDebug() << tmpSource->getOutputPort(0)->getDataClassName();

  pqView* view = objects.activeView();
  vtkSMProxy* tmpProxy = view->getProxy();
  vtkPVRenderView* clientView = vtkPVRenderView::SafeDownCast(
				 tmpProxy->GetClientSideObject());
  vtkCamera* clientCamera = clientView->GetActiveCamera();
  qDebug() << "Actor Count: " << clientView->GetNonCompositedRenderer()->VisibleActorCount();

  double* clipRange = clientCamera->GetClippingRange();
  vtkRenderWindowInteractor* renderWindowInteractor = (vtkRenderWindowInteractor*)clientView->GetInteractor();
  KeyPressInteractorStyle* style = new KeyPressInteractorStyle(this);
  renderWindowInteractor->SetInteractorStyle(style);
  style->SetCurrentRenderer(clientView->GetRenderer());

  view->render();*/
}

void printVector(Leap::Vector v){
  qDebug() << "x: " << v.x << "y: " << v.y << "z: " << v.z;
}

//-----------------------------------------------------------------------------
// Receive and React to the new incoming Frame
void pqParaWidget::newFrame(Frame frame)
{
  //qDebug() << "new Frame Received";
  //vtkLeapListener::printLeapInfo(frame);
  vtkSMNewWidgetRepresentationProxy* const widget = this->getWidgetProxy();
  vtkLeapBoxWidget2* absWidget = static_cast<vtkLeapBoxWidget2*>(widget->GetWidget());
  vtkLeapBoxRepresentation* WidgetRep = reinterpret_cast<vtkLeapBoxRepresentation*>(absWidget->GetWidgetRep());
  if(!WidgetRep) return;
  Leap::Vector curPalmPos = frame.hands().rightmost().palmPosition();
  int numFingers = frame.hands().rightmost().fingers().count();
  bool handRange = leapController->handInRange(curPalmPos);
  if((numFingers > 0 && handRange) || (ctrl == 2 && handRange) ){
    if(!tracking){
      this->Implementation->leapLabel->setPixmap(QPixmap(QString::fromUtf8("/Users/delvistaveras/Desktop/TransformWidget/handon.png")));
      this->tracking = 1;
    }

    Leap::Vector curHandPos = curPalmPos;
    Leap::Vector prevHandPos = this->prevFrame.hands().rightmost().palmPosition();

    if(ctrl == 0){
      double p2[3], p1[3];
      p2[0] = prevHandPos[0]*SCALE; p2[1] = prevHandPos[1]*SCALE; p2[2] = prevHandPos[2]*SCALE;
      p1[0] = curHandPos[0]*SCALE;  p1[1] = curHandPos[1]*SCALE;  p1[2] = curHandPos[2]*SCALE;
      double xdiff = fabs(p1[0] - p2[0]);
      double ydiff = fabs(p1[1] - p2[1]);
      double zdiff = fabs(p1[2] - p2[2]);

      //qDebug() << "x: " << p2[0] << "y: " << p2[1] << "z: " << p2[2];
      //qDebug() << "x: " << p1[0] << "y: " << p1[1] << "z: " << p1[2];
      //qDebug() << diff;
      
      if(xdiff < 2.0 && ydiff < 2.0 && zdiff < 2.0){
	WidgetRep->Translate(p2, p1);
      }
    }
    else if(ctrl == 1){
      double y_cur = curPalmPos.y;
      double y_prv = this->prevFrame.hands()[0].palmPosition().y;
      double chngPos = y_cur - y_prv;
      //qDebug() << "chngPos: " << chngPos;
      if(fabs(chngPos) < 10.0)
	WidgetRep->Rotate(chngPos*2.0, 1.0,0.0,0.0);
    }
    else if(ctrl == 2) {
      Leap::Hand trackingHand = frame.hands().rightmost();
      Leap::Vector palmNormal = trackingHand.palmNormal();
      //printVector(palmNormal);
      float yaw = trackingHand.palmNormal().yaw();
      float pitch = trackingHand.palmNormal().pitch();
      float roll = trackingHand.palmNormal().roll();
      /*qDebug() << "yaw: " << yaw;
	qDebug() << "pitch: " << pitch;
	qDebug() << "roll: " << roll;*/
      if(fabs(palmNormal.x) > .85){//Used to check if palm is facing +-x axis
	qDebug() << "Hand Perpendicular";
	Leap::Hand prvTrackingHand = this->prevFrame.hands().rightmost();
	double prv[3];
	double pnt[3];
	pnt[0] = trackingHand.palmPosition().x*SCALE; pnt[1] = trackingHand.palmPosition().y*SCALE;
	pnt[2] = trackingHand.palmPosition().z*SCALE;

	prv[0] = prvTrackingHand.palmPosition().x*SCALE; prv[1] = prvTrackingHand.palmPosition().y*SCALE;
	prv[2] = prvTrackingHand.palmPosition().z*SCALE;
	float xdiff = fabs(pnt[0]-prv[0]);
	float ydiff = fabs(pnt[1]-prv[1]);
	float zdiff = fabs(pnt[2]-prv[2]);

	qDebug() << "Xdiff:" << fabs(pnt[0]-prv[0]);
	qDebug() << "Ydiff:" << fabs(pnt[1]-prv[1]);
	qDebug() << "Zdiff:" << fabs(pnt[2]-prv[2]);
    
	if(xdiff < 2.0 && ydiff < 2.0 && zdiff < 2.0){
	  WidgetRep->translateSelected(prv, pnt);
	}
      }
    }

    this->prevFrame = frame;
    this->render();
    this->setModified();
    widget->UpdatePropertyInformation();
    //widget->UpdateVTKObjects();
  }
  else{
    if(tracking){
      this->Implementation->leapLabel->setPixmap(QPixmap(QString::fromUtf8("/Users/delvistaveras/Desktop/TransformWidget/handoff.png")));
      widget->UpdatePropertyInformation();
      //widget->UpdateVTKObjects();
      tracking = 0;
    }
  }
}

//-----------------------------------------------------------------------------
// update widget bounds.
void pqParaWidget::select()
{
  vtkSMNewWidgetRepresentationProxy* widget = this->getWidgetProxy();
  double input_bounds[6];
  if (widget  && this->getReferenceInputBounds(input_bounds))
    {
    vtkSMPropertyHelper(widget, "PlaceWidget").Set(input_bounds, 6);
    widget->UpdateVTKObjects();
    }

  this->Superclass::select();
}

//-----------------------------------------------------------------------------
void pqParaWidget::resetBounds(double input_bounds[6])
{
  vtkSMNewWidgetRepresentationProxy* widget = this->getWidgetProxy();
  vtkSMPropertyHelper(widget, "PlaceWidget").Set(input_bounds, 6);
  widget->UpdateVTKObjects();
}

//-----------------------------------------------------------------------------
void pqParaWidget::onWidgetVisibilityChanged(bool visible)
{
  this->Implementation->show3DWidget->blockSignals(true);
  this->Implementation->show3DWidget->setChecked(visible);
  this->Implementation->show3DWidget->blockSignals(false);
}

//-----------------------------------------------------------------------------
void pqParaWidget::accept()
{
  this->Superclass::accept();
  this->hideHandles();
}

//-----------------------------------------------------------------------------
void pqParaWidget::reset()
{
  this->Superclass::reset();
  this->hideHandles();
}

//-----------------------------------------------------------------------------
void pqParaWidget::showHandles()
{
  /*
  vtkSMProxy* proxy = this->getWidgetProxy();
  if (proxy)
    {
    pqSMAdaptor::setElementProperty(proxy->GetProperty("HandleVisibility"), 1);
    proxy->UpdateVTKObjects();
    }
    */
}

//-----------------------------------------------------------------------------
void pqParaWidget::hideHandles()
{
  /*
  vtkSMProxy* proxy = this->getWidgetProxy();
  if (proxy)
    {
    pqSMAdaptor::setElementProperty(proxy->GetProperty("HandleVisibility"), 1);
    proxy->UpdateVTKObjects();
    }
  */
}
