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

#include "rthybrid_komendantov_kononenko_1996_neuron.hpp"

RTHybridKomendantovKononenko1996Neuron::Plugin::Plugin(
    Event::Manager *ev_manager)
    : Widgets::Plugin(
          ev_manager,
          std::string(RTHybridKomendantovKononenko1996Neuron::MODULE_NAME)) {}

RTHybridKomendantovKononenko1996Neuron::Panel::Panel(QMainWindow *main_window,
                                                     Event::Manager *ev_manager)
    : Widgets::Panel(
          std::string(RTHybridKomendantovKononenko1996Neuron::MODULE_NAME),
          main_window, ev_manager) {
  setWhatsThis("Template Plugin");
  createGUI(RTHybridKomendantovKononenko1996Neuron::get_default_vars(),
            {}); // this is required to create the GUI
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
  this->parentWidget()->adjustSize();
}

RTHybridKomendantovKononenko1996Neuron::Component::Component(
    Widgets::Plugin *hplugin)
    : Widgets::Component(
          hplugin,
          std::string(RTHybridKomendantovKononenko1996Neuron::MODULE_NAME),
          RTHybridKomendantovKononenko1996Neuron::get_default_channels(),
          RTHybridKomendantovKononenko1996Neuron::get_default_vars()) {}

void RTHybridKomendantovKononenko1996Neuron::Component::execute() {
  // This is the real-time function that will be called
  switch (this->getState()) {
  case RT::State::EXEC:
    break;
  case RT::State::INIT:
    break;
  case RT::State::MODIFY:
    break;
  case RT::State::PERIOD:
    break;
  case RT::State::PAUSE:
    break;
  case RT::State::UNPAUSE:
    break;
  default:
    break;
  }
}

///////// DO NOT MODIFY BELOW //////////
// The exception is if your plugin is not going to need real-time functionality.
// For this case just replace the craeteRTXIComponent return type to nullptr.
// RTXI will automatically handle that case and won't attach a component to the
// real time thread for your plugin.

std::unique_ptr<Widgets::Plugin> createRTXIPlugin(Event::Manager *ev_manager) {
  return std::make_unique<RTHybridKomendantovKononenko1996Neuron::Plugin>(
      ev_manager);
}

Widgets::Panel *createRTXIPanel(QMainWindow *main_window,
                                Event::Manager *ev_manager) {
  return new RTHybridKomendantovKononenko1996Neuron::Panel(main_window,
                                                           ev_manager);
}

std::unique_ptr<Widgets::Component>
createRTXIComponent(Widgets::Plugin *host_plugin) {
  return std::make_unique<RTHybridKomendantovKononenko1996Neuron::Component>(
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
