/*
 * App.h
 *
 *  Created on: 18.05.2023
 *      Author: didi
 */

#ifndef APP_H_
#define APP_H_

class SerialCommand;
class Indicator;
class DbgTrace_Port;

class App
{
public:
  App();
  virtual ~App();

  void setup();

  void loop();

private:
  SerialCommand* m_sCmd;
  Indicator* m_led;
  DbgTrace_Port* m_trPort;

private:
  static const char s_termChar;

  static const int  s_directionPin;
  static const int  s_pwmPin;
  static const int  s_brakePin;

  static const int  s_maxVal;
  static const int  s_minVal;

  bool m_directionState;
  int m_setPoint;
};

#endif /* APP_H_ */
