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
#include "rthybrid_electrical_synapse_device.hpp"

#include <QTimer>
#include <cmath>
#include <rtxi/fifo.hpp>
#include <rtxi/rt.hpp>
#include <rtxi/rtos.hpp>
void RTHybridElectricalSynapseDevice::Plugin::receiveEvent(
    Event::Object *event) {
  auto *module_panel =
      dynamic_cast<RTHybridElectricalSynapseDevice::Panel *>(this->getPanel());
  switch (event->getType()) {
  case Event::Type::RT_THREAD_INSERT_EVENT:
  case Event::Type::RT_DEVICE_INSERT_EVENT:
    // module_panel->updateBlockInfo();
    break;
  case Event::Type::RT_THREAD_REMOVE_EVENT:
    // module_panel->updateBlockChannels(
    //     std::any_cast<RT::Thread *>(event->getParam("thread")));
    // module_panel->updateBlockInfo();
    break;
  case Event::Type::RT_DEVICE_REMOVE_EVENT:
    // module_panel->updateBlockChannels(
    //     std::any_cast<RT::Device *>(event->getParam("device")));
    // module_panel->updateBlockInfo();
    break;
  default:
    break;
  }
}
RTHybridElectricalSynapseDevice::Component::Component(Widgets::Plugin *hplugin)
    : Widgets::Component(
          hplugin, std::string(RTHybridElectricalSynapseDevice::MODULE_NAME),
          RTHybridElectricalSynapseDevice::get_default_channels(),
          RTHybridElectricalSynapseDevice::get_default_vars()) {
  if (RT::OS::getFifo(
          this->fifo,
          10 * sizeof(RTHybridElectricalSynapseDevice::synapse_state_t)) < 0) {
    ERROR_MSG("PerformanceMeasurement::Component::Component : Unable to craate "
              "component fifo");
    this->setState(RT::State::PAUSE);
  }
}

RTHybridElectricalSynapseDevice::Plugin::Plugin(Event::Manager *ev_manager)
    : Widgets::Plugin(
          ev_manager,
          std::string(RTHybridElectricalSynapseDevice::MODULE_NAME)) {
  auto component =
      std::make_unique<RTHybridElectricalSynapseDevice::Component>(this);
  this->component_fifo = component->get_fifo_ptr();
  this->attachComponent(std::move(component));
}

RTHybridElectricalSynapseDevice::Panel::Panel(QMainWindow *main_window,
                                              Event::Manager *ev_manager)
    : Widgets::Panel(std::string(RTHybridElectricalSynapseDevice::MODULE_NAME),
                     main_window, ev_manager)
// ,
// s12_edit(new QLineEdit(this)), o12_edit(new QLineEdit(this)),
// s21_edit(new QLineEdit(this)), o21_edit(new QLineEdit(this))
{
  setWhatsThis(
      "<p><b>RTHybrid electrical synapse model</b><br>RTHybrid module for RTXI "
      "that implements a gap junction electrical synapse model.</p>");
  createGUI(RTHybridElectricalSynapseDevice::get_default_vars(),
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

  current_edit = edits[ELECTRICAL_SYNAPSE_CURRENT];
  offset_edit = edits[ELECTRICAL_SYNAPSE_OFFSET];
  scale_edit = edits[ELECTRICAL_SYNAPSE_SCALE];

  if (current_edit) {
    current_edit->setReadOnly(true);
    current_edit->setStyleSheet(readonlyStyle);
  }

  if (offset_edit) {
    offset_edit->setReadOnly(true);
    offset_edit->setStyleSheet(readonlyStyle);
  }

  if (scale_edit) {
    scale_edit->setReadOnly(true);
    scale_edit->setStyleSheet(readonlyStyle);
  }

  this->parentWidget()->adjustSize();
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this,
          &RTHybridElectricalSynapseDevice::Panel::refresh);
  timer->start(500); // refresh every 500 ms
}
RTHybridElectricalSynapseDevice::synapse_state_t
RTHybridElectricalSynapseDevice::Plugin::get_synapse_state() {

  RTHybridElectricalSynapseDevice::synapse_state_t stat;
  while (this->component_fifo->read(
             &stat, sizeof(RTHybridElectricalSynapseDevice::synapse_state_t)) >
         0) {
  };
  return stat;
}

void RTHybridElectricalSynapseDevice::Panel::refresh() {
  auto *hostplugin = dynamic_cast<RTHybridElectricalSynapseDevice::Plugin *>(
      this->getHostPlugin());
  const RTHybridElectricalSynapseDevice::synapse_state_t s_state =
      hostplugin->get_synapse_state();

  current_edit->setText(QString::number(s_state.current));
  offset_edit->setText(QString::number(s_state.offset));
  scale_edit->setText(QString::number(s_state.scale));
}

void RTHybridElectricalSynapseDevice::Component::init_parameters(void) {
  g[SM_ELECTRICAL_G] = getValue<double>(ELECTRICAL_SYNAPSE_G);
  scale = getValue<double>(ELECTRICAL_SYNAPSE_SCALE);
  offset = getValue<double>(ELECTRICAL_SYNAPSE_OFFSET);
  i = getValue<double>(ELECTRICAL_SYNAPSE_CURRENT);
}

void RTHybridElectricalSynapseDevice::Component::execute() {
  // This is the real-time function that will be called
  switch (this->getState()) {
  case RT::State::EXEC:
    scale = readinput(2);
    offset = readinput(3) * 1000;

    if (scale == 0) {
      scale = 1;
      offset = 0;
    }

    sm_electrical(readinput(0) * 1000, readinput(1) * 1000, &i);

    writeoutput(0, i);

    synapse_state.current = i;
    synapse_state.offset = offset;
    synapse_state.scale = scale;

    this->fifo->writeRT(
        &this->synapse_state,
        sizeof(RTHybridElectricalSynapseDevice::synapse_state_t));

    break;
  case RT::State::INIT:
    period = RT::OS::getPeriod() * 1e-6; // ms

    init_parameters();
    setState(RT::State::PAUSE);
    break;
  case RT::State::MODIFY:
    setState(RT::State::INIT);
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

void RTHybridElectricalSynapseDevice::Component::sm_electrical(double v_post,
                                                               double v_pre,
                                                               double *ret) {
  v_pre = v_pre * scale + offset;

  *ret = (g[SM_ELECTRICAL_G] * (v_post - v_pre));

  return;
}

///////// DO NOT MODIFY BELOW //////////
// The exception is if your plugin is not going to need real-time functionality.
// For this case just replace the craeteRTXIComponent return type to nullptr.
// RTXI will automatically handle that case and won't attach a component to the
// real time thread for your plugin.

std::unique_ptr<Widgets::Plugin> createRTXIPlugin(Event::Manager *ev_manager) {
  return std::make_unique<RTHybridElectricalSynapseDevice::Plugin>(ev_manager);
}

Widgets::Panel *createRTXIPanel(QMainWindow *main_window,
                                Event::Manager *ev_manager) {
  return new RTHybridElectricalSynapseDevice::Panel(main_window, ev_manager);
}

std::unique_ptr<Widgets::Component>
createRTXIComponent(Widgets::Plugin *host_plugin) {
  return std::make_unique<RTHybridElectricalSynapseDevice::Component>(
      host_plugin);
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
