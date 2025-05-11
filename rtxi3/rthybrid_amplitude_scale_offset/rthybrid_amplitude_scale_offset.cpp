/*
 * Copyright (C) 2011 Georgia Institute of Technology, University of Utah,
 * Weill Cornell Medical College
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This is a template implementation file for a user module,
 */
#include "rthybrid_amplitude_scale_offset.hpp"
#include <QTimer>
#include <cstdio>
#include <rtxi/rt.hpp>
#include <rtxi/rtos.hpp>

RTHybridAmplitudeScaleOffset::Component
    *RTHybridAmplitudeScaleOffset::Component::instance = nullptr;

RTHybridAmplitudeScaleOffset::Plugin::Plugin(Event::Manager *ev_manager)
    : Widgets::Plugin(ev_manager,
                      std::string(RTHybridAmplitudeScaleOffset::MODULE_NAME)) {}

RTHybridAmplitudeScaleOffset::Panel::Panel(QMainWindow *main_window,
                                           Event::Manager *ev_manager)
    : Widgets::Panel(std::string(RTHybridAmplitudeScaleOffset::MODULE_NAME),
                     main_window, ev_manager) {
  setWhatsThis(
      "<p><b>RTHybrid Amplitude Scale Offset:</b><br>Given two neurons "
      "membrane potential minimum and maximum values, this module calculates "
      "the amplitude scale factor and offset between them, in both directions. "
      "i.e. if Neuron 1 membrane potential is multiplied by Scale 1-2 and "
      "added Offset 1-2, the result will be in the same amplitude range than "
      "Neuron 2 membrane potential, and viceversa.</p>");
  createGUI(RTHybridAmplitudeScaleOffset::get_default_vars(),
            {}); // this is required to create the GUI

  auto edits = findChildren<QLineEdit *>();
  QString readonlyStyle = R"(
    QLineEdit {
    background-color: #e0e0e0;
    color: #666666;
    border: 1px solid #cccccc;
    }
    QLineEdit:focus {
    border: 1px solid #cccccc;
    outline: none;
    }
  )";

  s12_edit = edits[AMPLITUDE_SCALE_S12];
  s21_edit = edits[AMPLITUDE_SCALE_S21];
  o12_edit = edits[AMPLITUDE_SCALE_O12];
  o21_edit = edits[AMPLITUDE_SCALE_O21];
  if (s12_edit) {
    s12_edit->setReadOnly(true);
    s12_edit->setStyleSheet(readonlyStyle);
  }
  if (s21_edit) {
    s21_edit->setReadOnly(true);
    s21_edit->setStyleSheet(readonlyStyle);
  }
  if (o12_edit) {
    o12_edit->setReadOnly(true);
    o12_edit->setStyleSheet(readonlyStyle);
  }
  if (o21_edit) {
    o21_edit->setReadOnly(true);
    o21_edit->setStyleSheet(readonlyStyle);
  }

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this,
          &RTHybridAmplitudeScaleOffset::Panel::refresh);
  timer->start(500); // refresh every 500 ms
  this->parentWidget()->adjustSize();
}

RTHybridAmplitudeScaleOffset::Component::Component(Widgets::Plugin *hplugin)
    : Widgets::Component(hplugin,
                         std::string(RTHybridAmplitudeScaleOffset::MODULE_NAME),
                         RTHybridAmplitudeScaleOffset::get_default_channels(),
                         RTHybridAmplitudeScaleOffset::get_default_vars()) {
  Component::instance = this;
}

void RTHybridAmplitudeScaleOffset::Panel::refresh() {
  auto *comp = RTHybridAmplitudeScaleOffset::Component::instance;
  if (comp && s12_edit) {
    s12_edit->setText(QString::number(comp->s12));
  }
  if (comp && s21_edit) {
    s21_edit->setText(QString::number(comp->s21));
  }
  if (comp && o12_edit) {
    o12_edit->setText(QString::number(comp->o12));
  }
  if (comp && o21_edit) {
    o21_edit->setText(QString::number(comp->o21));
  }
}

void RTHybridAmplitudeScaleOffset::Component::execute() {
  // This is the real-time function that will be called
  switch (this->getState()) {
  case RT::State::EXEC:
    double min1;
    double max1;
    double min2;
    double max2;
    double range1;
    double range2;

    min1 = readinput(0);

    max1 = readinput(1);
    min2 = readinput(2);
    max2 = readinput(3);
    range1 = max1 - min1;
    range2 = max2 - min2;

    s12 = range2 / range1;
    s21 = range1 / range2;

    o12 = min2 - (min1 * s12);
    o21 = min1 - (min2 * s21);

    writeoutput(0, s12);
    writeoutput(1, o12);
    writeoutput(2, s21);
    writeoutput(3, o21);

    break;
  case RT::State::INIT:
    period = RT::OS::getPeriod() * 1e-6; // ms

    initParameters();
    setState(RT::State::PAUSE);

    break;
  case RT::State::MODIFY:
    setState(RT::State::PAUSE);
    break;
  case RT::State::PERIOD:
    period = RT::OS::getPeriod() * 1e-6; // ms

    setState(RT::State::PAUSE);
    break;
  case RT::State::PAUSE:
    break;
  case RT::State::UNPAUSE:
    setState(RT::State::EXEC);
    break;

  default:
    break;
  }
}

void RTHybridAmplitudeScaleOffset::Component::initParameters() {
  s12 = getValue<double>(AMPLITUDE_SCALE_S12);
  s21 = getValue<double>(AMPLITUDE_SCALE_S21);
  o12 = getValue<double>(AMPLITUDE_SCALE_O12);
  o21 = getValue<double>(AMPLITUDE_SCALE_O21);
}

///////// DO NOT MODIFY BELOW //////////
// The exception is if your plugin is not going to need real-time functionality.
// For this case just replace the craeteRTXIComponent return type to nullptr.
// RTXI will automatically handle that case and won't attach a component to the
// real time thread for your plugin.

std::unique_ptr<Widgets::Plugin> createRTXIPlugin(Event::Manager *ev_manager) {
  return std::make_unique<RTHybridAmplitudeScaleOffset::Plugin>(ev_manager);
}

Widgets::Panel *createRTXIPanel(QMainWindow *main_window,
                                Event::Manager *ev_manager) {
  return new RTHybridAmplitudeScaleOffset::Panel(main_window, ev_manager);
}

std::unique_ptr<Widgets::Component>
createRTXIComponent(Widgets::Plugin *host_plugin) {
  return std::make_unique<RTHybridAmplitudeScaleOffset::Component>(host_plugin);
}

Widgets::FactoryMethods fact;

extern "C" {
Widgets::FactoryMethods *getFactories() {
  fact.createPanel = &createRTXIPanel;
  fact.createComponent = &createRTXIComponent;
  fact.createPlugin = &createRTXIPlugin;
  return &fact;
}
};

//////////// END //////////////////////
