#ifndef _CFADEANIMATION_H
#define _CFADEANIMATION_H

#include "IAnimation.h"

class CFadeAnimation : public IAnimation
{
public:
  CFadeAnimation();

  virtual ~CFadeAnimation();

  virtual bool transform(CRGB* current, CRGB* target, int num_leds, bool changed);

private:
  unsigned long previousMillis;
  unsigned long interval;
};

#endif
