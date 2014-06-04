#include "vtkLeapController.h"
#include <QDebug>

vtkLeapController::vtkLeapController():
  y_bot(90.0), y_top(250.0), x_left(-130.0), x_right(130.0),
  z_back(-100.0), z_front(100.0)
{
  qDebug() << "vtkLeapController Constructor";
  this->leapController = new Leap::Controller();
  this->leapListener = new vtkLeapListener(this);
  this->leapController->addListener(*this->leapListener);
}

vtkLeapController::~vtkLeapController()
{
  this->leapController->removeListener(*this->leapListener);
  delete this->leapListener;
  delete this->leapController;
}

void vtkLeapController::newFrame(Frame frame){
  //qDebug() << "Received Signal from Listener";
  emit gotFrame(frame);
}

//Takes a frame and based on the variables above allows Leap Interaction
bool vtkLeapController::handInRange(Vector palmPosition){
  if(y_bot > palmPosition.y || palmPosition.y > y_top)
    return false;
  else if(z_back > palmPosition.z || palmPosition.z > z_front)
    return false;
  else if(x_left > palmPosition.x || palmPosition.x > x_right)
    return false;

  return true;
}
