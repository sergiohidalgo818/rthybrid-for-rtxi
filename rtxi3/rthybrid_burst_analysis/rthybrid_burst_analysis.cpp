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
#include "rthybrid_burst_analysis.hpp"
#include <QTimer>
#include <cstdio>
#include <qchar.h>
#include <qlineedit.h>
#include <rtxi/fifo.hpp>
#include <rtxi/rt.hpp>
#include <rtxi/rtos.hpp>

RTHybridBurstAnalysis::Component::Component(Widgets::Plugin *hplugin)
    : Widgets::Component(hplugin,
                         std::string(RTHybridBurstAnalysis::MODULE_NAME),
                         RTHybridBurstAnalysis::get_default_channels(),
                         RTHybridBurstAnalysis::get_default_vars()) {
  if (RT::OS::getFifo(this->fifo,
                      10 * sizeof(RTHybridBurstAnalysis::analyzer_state_t)) <
      0) {
    ERROR_MSG("PerformanceMeasurement::Component::Component : Unable to craate "
              "component fifo");
    this->setState(RT::State::PAUSE);
  }
}

RTHybridBurstAnalysis::Plugin::Plugin(Event::Manager *ev_manager)
    : Widgets::Plugin(ev_manager,
                      std::string(RTHybridBurstAnalysis::MODULE_NAME)) {
  auto component = std::make_unique<RTHybridBurstAnalysis::Component>(this);
  this->component_fifo = component->get_fifo_ptr();
  this->attachComponent(std::move(component));
}

RTHybridBurstAnalysis::Panel::Panel(QMainWindow *main_window,
                                    Event::Manager *ev_manager)
    : Widgets::Panel(std::string(RTHybridBurstAnalysis::MODULE_NAME),
                     main_window, ev_manager)
// ,
// s12_edit(new QLineEdit(this)), o12_edit(new QLineEdit(this)),
// s21_edit(new QLineEdit(this)), o21_edit(new QLineEdit(this))
{
  setWhatsThis("<p><b>RTHybridBurstAnalysis:</b><br>RTHybrid module for RTXI "
               "to get the minimum and maximum membrane potential values of a "
               "neuron and its bursts duration.</p>");
  createGUI(RTHybridBurstAnalysis::get_default_vars(),
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

  min_edit = edits[BURST_ANALYSIS_MIN];
  max_edit = edits[BURST_ANALYSIS_MAX];
  dur_edit = edits[BURST_ANALYSIS_DUR];
  upp_thresh_edit = edits[BURST_ANALYSIS_UPP_THRESH];
  down_thresh_edit = edits[BURST_ANALYSIS_DOWN_THRESH];
  ampl_edit = edits[BURST_ANALYSIS_AMPL];
  pc_edit = edits[BURST_ANALYSIS_PC];
  bc_edit = edits[BURST_ANALYSIS_BC];
  isb_edit = edits[BURST_ANALYSIS_ISB];

  if (min_edit) {
    min_edit->setReadOnly(true);
    min_edit->setStyleSheet(readonlyStyle);
  }
  if (max_edit) {
    max_edit->setReadOnly(true);
    max_edit->setStyleSheet(readonlyStyle);
  }
  if (dur_edit) {
    dur_edit->setReadOnly(true);
    dur_edit->setStyleSheet(readonlyStyle);
  }
  if (upp_thresh_edit) {
    upp_thresh_edit->setReadOnly(true);
    upp_thresh_edit->setStyleSheet(readonlyStyle);
  }
  if (down_thresh_edit) {
    down_thresh_edit->setReadOnly(true);
    down_thresh_edit->setStyleSheet(readonlyStyle);
  }
  if (ampl_edit) {
    ampl_edit->setReadOnly(true);
    ampl_edit->setStyleSheet(readonlyStyle);
  }
  if (isb_edit) {
    isb_edit->setReadOnly(true);
    isb_edit->setStyleSheet(readonlyStyle);
  }
  if (pc_edit) {
    pc_edit->setReadOnly(true);
    pc_edit->setStyleSheet(readonlyStyle);
  }
  if (bc_edit) {
    bc_edit->setReadOnly(true);
    bc_edit->setStyleSheet(readonlyStyle);
  }

  this->parentWidget()->adjustSize();
  auto *timer = new QTimer(this);
  timer->setInterval(300);
  QObject::connect(timer, &QTimer::timeout, this,
                   &RTHybridBurstAnalysis::Panel::refresh);
  timer->start();
}

RTHybridBurstAnalysis::analyzer_state_t
RTHybridBurstAnalysis::Plugin::get_analyzer_state() {

  RTHybridBurstAnalysis::analyzer_state_t stat;
  while (this->component_fifo->read(
             &stat, sizeof(RTHybridBurstAnalysis::analyzer_state_t)) > 0) {
  };
  return stat;
}

void RTHybridBurstAnalysis::Panel::refresh() {
  auto *hostplugin =
      dynamic_cast<RTHybridBurstAnalysis::Plugin *>(this->getHostPlugin());
  const RTHybridBurstAnalysis::analyzer_state_t s_state =
      hostplugin->get_analyzer_state();
  min_edit->setText(QString::number(s_state.min));
  max_edit->setText(QString::number(s_state.max));
  dur_edit->setText(QString::number(s_state.sec_per_burst));
  upp_thresh_edit->setText(QString::number(s_state.thresh_up));
  down_thresh_edit->setText(QString::number(s_state.thresh_down));
  ampl_edit->setText(QString::number(s_state.range));
  isb_edit->setText(QString::number(s_state.is_burst));
  pc_edit->setText(QString::number(s_state.pts_counter));
  bc_edit->setText(QString::number(s_state.burst_counter));
}

void RTHybridBurstAnalysis::Component::init_parameters(void) {

  min = getValue<double>(BURST_ANALYSIS_MIN);
  max = getValue<double>(BURST_ANALYSIS_MAX);
  temp_min = 999999.0;
  temp_max = -999999.0;
  count = 0.0;

  sec_per_burst = getValue<double>(BURST_ANALYSIS_DUR);
  pts_counter = getValue<double>(BURST_ANALYSIS_PC);
  burst_counter = getValue<double>(BURST_ANALYSIS_BC);
  is_burst = getValue<double>(BURST_ANALYSIS_ISB);
  burst_dur_sum = 0.0;
  old_burst_time = 0.0;
}

void RTHybridBurstAnalysis::Component::execute() {
  // This is the real-time function that will be called
  switch (this->getState()) {
  case RT::State::EXEC:
    double vm;   // V
    double time; // ms
    time = count * period;
    vm = readinput(0);

    if (time > (observation_time * 1000)) {
      min = temp_min;
      max = temp_max;
      writeoutput(0, min);
      writeoutput(1, max);
      temp_min = 999999.0;
      temp_max = -999999.0;
      count = 0;

      sec_per_burst = (burst_dur_sum / burst_counter) / freq;
      writeoutput(2, sec_per_burst);
      pts_counter = 0;
      burst_counter = 0;
      burst_dur_sum = 0;
      old_burst_time = 0;
    }

    if (vm < temp_min)
      temp_min = vm;
    if (vm > temp_max)
      temp_max = vm;

    range = max - min;
    thresh_down = min + (range * 0.1);
    thresh_up = min + (range * 0.9);

    if (is_burst == 0 && vm > thresh_up) {
      is_burst = 1;
      burst_counter++;
      burst_dur_sum += pts_counter - old_burst_time;
      old_burst_time = pts_counter;
    } else if (is_burst == 1 && vm < thresh_down) {
      is_burst = 0;
    }

    pts_counter++;
    count++;

    analyzer_state = {
        min,   max,         sec_per_burst, pts_counter, burst_counter,
        range, thresh_down, range,         is_burst,
    };

    this->fifo->writeRT(&this->analyzer_state,
                        sizeof(RTHybridBurstAnalysis::analyzer_state_t));

    break;

  case RT::State::INIT:
    period = RT::OS::getPeriod() * 1e-6;
    freq = 1.0 / (period * 1e-3);
    observation_time = getValue<double>(BURST_ANALYSIS_OBST);

    init_parameters();
    setState(RT::State::PAUSE);
    break;
  case RT::State::MODIFY:
  case RT::State::PERIOD:
    period = RT::OS::getPeriod() * 1e-6;
    freq = 1.0 / (period * 1e-3);
    observation_time = getValue<double>(BURST_ANALYSIS_OBST);

    init_parameters();
    setState(RT::State::EXEC);
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

///////// DO NOT MODIFY BELOW //////////
// The exception is if your plugin is not going to need real-time
// functionality. For this case just replace the craeteRTXIComponent return
// type to nullptr. RTXI will automatically handle that case and won't attach
// a component to the real time thread for your plugin.

std::unique_ptr<Widgets::Plugin> createRTXIPlugin(Event::Manager *ev_manager) {
  return std::make_unique<RTHybridBurstAnalysis::Plugin>(ev_manager);
}

Widgets::Panel *createRTXIPanel(QMainWindow *main_window,
                                Event::Manager *ev_manager) {
  return new RTHybridBurstAnalysis::Panel(main_window, ev_manager);
}

std::unique_ptr<Widgets::Component>
createRTXIComponent(Widgets::Plugin *host_plugin) {
  return std::make_unique<RTHybridBurstAnalysis::Component>(host_plugin);
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
