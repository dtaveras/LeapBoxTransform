#ifndef _LEAPCONTROLLER_H
#define _LEAPCONTROLLER_H

#include <QObject>
#include "vtkLeapListener.h"

class vtkLeapListener;
class pqLeapView;

using namespace Leap;

class vtkLeapController : public QObject
{
  Q_OBJECT

public:
  vtkLeapController();
  ~vtkLeapController();
  bool handInRange(Vector palmPosition);

private:
  vtkLeapListener* leapListener;
  Leap::Controller* leapController;
  pqLeapView* view;

  //In range bounds
  double y_bot;
  double y_top;
  double x_left;
  double x_right;
  double z_back;
  double z_front;
  
public slots:
  void newFrame(Frame frame);
signals:
  void gotFrame(Frame frame);
};

#endif
