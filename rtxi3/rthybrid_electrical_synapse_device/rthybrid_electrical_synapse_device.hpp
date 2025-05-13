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
#include <rtxi/io.hpp>
#include <rtxi/widgets.hpp>

namespace RTHybridElectricalSynapseDevice {

constexpr std::string_view MODULE_NAME = "RTHybrid Electrical Synapse Device";

enum PARAMETER : Widgets::Variable::Id {
  // set parameter ids here
  ELECTRICAL_SYNAPSE_G = 0,
  ELECTRICAL_SYNAPSE_CURRENT,
  ELECTRICAL_SYNAPSE_OFFSET,
  ELECTRICAL_SYNAPSE_SCALE,
};

enum CONDUCTANCE_VARIABLE {

  SM_ELECTRICAL_G = 0,
  SM_ELECTRICAL_N_G,
};

inline std::vector<Widgets::Variable::Info> get_default_vars() {
  return {

      {ELECTRICAL_SYNAPSE_G, "g (uS)", "Conductance (uS)",
       Widgets::Variable::DOUBLE_PARAMETER, -0.2},
      {ELECTRICAL_SYNAPSE_CURRENT, "Current (nA)", "",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {ELECTRICAL_SYNAPSE_OFFSET, "Offset", "",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {ELECTRICAL_SYNAPSE_SCALE, "Scale", "",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
  };
}

inline std::vector<IO::channel_t> get_default_channels() {
  return {
      {"Current (nA)", "Synaptic current (in nA)", IO::OUTPUT},
      {"Post-synaptic Voltage (V)", "", IO::INPUT},
      {"Pre-synaptic Voltage (V)", "", IO::INPUT},
      {"Scale (Pre to Post)",
       "Scale from the pre-synaptic neuron to the post-synaptic one, as given "
       "by the RTHybridAmplitudeScale module.",
       IO::INPUT},
      {"Offset (Pre to Post)",
       "Offset from the pre-synaptic neuron to the post-synaptic one, as given "
       "by the RTHybridAmplitudeScale module.",
       IO::INPUT},
  };
}
struct synapse_state_t {
  double current = 0.0;
  double offset = 0.0;
  double scale = 0.0;
};

class Panel : public Widgets::Panel {
  Q_OBJECT
public:
  Panel(QMainWindow *main_window, Event::Manager *ev_manager);
  // Any functions and data related to the GUI are to be placed
  // here
  void refresh() override;

private:
  QLineEdit *current_edit = nullptr;
  QLineEdit *offset_edit = nullptr;
  QLineEdit *scale_edit = nullptr;
};

class Plugin : public Widgets::Plugin {

public:
  void receiveEvent(Event::Object *event) override;

  explicit Plugin(Event::Manager *ev_manager);
  synapse_state_t get_synapse_state();

private:
  RT::OS::Fifo *component_fifo;
};

class Component : public Widgets::Component {
public:
  explicit Component(Widgets::Plugin *hplugin);

  void execute() override;
  // Additional functionality needed for RealTime computation is to be placed
  // here

  synapse_state_t get_synapse_states();
  RT::OS::Fifo *get_fifo_ptr() { return this->fifo.get(); }

private:
  double period;
  double i, offset, scale;
  double g[1];

  synapse_state_t synapse_state;
  std::unique_ptr<RT::OS::Fifo> fifo;

  void init_parameters();
  ;
  void sm_electrical(double v_post, double v_pre, double *ret);
};

} // namespace RTHybridElectricalSynapseDevice
