/*
 * App.cpp
 *
 *  Created on: 18.05.2023
 *      Author: didi
 */
#include <Arduino.h>
#include <SerialCommand.h>
#include <SpinTimer.h>
#include <Indicator.h>
#include <IndicatorFactory.h>
#include "MyBuiltinLedIndicatorAdapter.h"
#include <Button.h>
#include <ButtonEdgeDetector.h>
#include <MyButtonAdapter.h>
#include <ArduinoDigitalInPinSupervisor.h>
#include <ProductDebug.h>
#include <DbgTracePort.h>
#include <DbgTraceLevel.h>
#include "App.h"

const char App::s_termChar = '\n';

const int  App::s_directionPin = 12;
const int  App::s_pwmPin = 3;
const int  App::s_brakePin = 9;

const int  App::s_maxVal = 160;
const int  App::s_minVal = (-1) * s_maxVal;


App::App()
: m_sCmd(new SerialCommand(s_termChar))
, m_led(0)
, m_trPort(0)
, m_directionState(true)
, m_setPoint(0)
{ }

App::~App()
{
  delete m_trPort;
  m_trPort = 0;

  if (0 != m_led)
  {
    delete m_led->adapter();
    m_led->assignAdapter(0);
  }

  delete m_led;
  m_led = 0;

  delete m_sCmd;
  m_sCmd = 0;
}

void App::setup()
{
  // setup basic debug environment (heap usage printer, trace ports & dbg cli)
  ProductDebug::setupProdDebugEnv(m_sCmd);

  // indicator LED
  m_led = IndicatorFactory::createIndicator("led", "Built in LED.");
  m_led->assignAdapter(new MyBuiltinLedIndicatorAdapter());

  #ifdef USER_BTN
    new Button(new ArduinoDigitalInPinSupervisor(USER_BTN), new ButtonEdgeDetector(), new MyButtonAdapter(m_led));
  #endif

  //define pins
  pinMode(s_directionPin, OUTPUT);
  pinMode(s_pwmPin, OUTPUT);
  pinMode(s_brakePin, OUTPUT);

  //release breaks
  digitalWrite(s_brakePin, LOW);

  // trace port
  m_trPort = new DbgTrace_Port("mot", DbgTrace_Level::notice);
}

void App::loop()
{
  if (0 != m_sCmd)
  {
    m_sCmd->readSerial();     // process serial commands
  }
  scheduleTimers();

  // read set point value from Poti
  m_setPoint = map(analogRead(A5), 0, 1023, s_minVal, s_maxVal);

  if (0 != m_trPort)
  {
    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "setPoint: %d", m_setPoint);
  }

  // select direction
  m_directionState = (m_setPoint >= 0);
  digitalWrite(s_directionPin, m_directionState);

  // set work duty for the motor
  analogWrite(s_pwmPin, abs(m_setPoint));
}
