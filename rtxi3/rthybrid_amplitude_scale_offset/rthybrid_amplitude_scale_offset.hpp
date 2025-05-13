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

#include <rtxi/widgets.hpp>

namespace RTHybridAmplitudeScaleOffset {

constexpr std::string_view MODULE_NAME = "RTHybrid Amplitude Scale Offset";

enum PARAMETER : Widgets::Variable::Id {
  // set parameter ids here
  AMPLITUDE_SCALE_S12 = 0,
  AMPLITUDE_SCALE_O12,
  AMPLITUDE_SCALE_S21,
  AMPLITUDE_SCALE_O21
};

inline std::vector<Widgets::Variable::Info> get_default_vars() {
  return {
      {AMPLITUDE_SCALE_S12, "Scale 1-2", "Scale from neuron 1 to neuron 2",
       Widgets::Variable::DOUBLE_PARAMETER, 1.0},
      {AMPLITUDE_SCALE_O12, "Offset 1-2", "Offset from neuron 1 to neuron 2",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {AMPLITUDE_SCALE_S21, "Scale 2-1", "Scale from neuron 2 to neuron 1",
       Widgets::Variable::DOUBLE_PARAMETER, 1.0},
      {AMPLITUDE_SCALE_O21, "Offset 2-1", "Offset from neuron 2 to neuron 1",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0}};
}

inline std::vector<IO::channel_t> get_default_channels() {
  return {{
              "Min 1 (V)",
              "Neuron 1 minimum membrane potential (in V)",
              IO::INPUT,
          },
          {
              "Max 1 (V)",
              "Neuron 1 maximum membrane potential (in V)",
              IO::INPUT,
          },
          {
              "Min 2 (V)",
              "Neuron 2 minimum membrane potential (in V)",
              IO::INPUT,
          },
          {
              "Max 2 (V)",
              "Neuron 2 maximum membrane potential (in V)",
              IO::INPUT,
          },
          {
              "Scale 1-2",
              "Scale from neuron 1 to neuron 2",
              IO::OUTPUT,
          },
          {
              "Offset 1-2",
              "Offset from neuron 1 to neuron 2",
              IO::OUTPUT,
          },
          {
              "Scale 2-1",
              "Scale from neuron 2 to neuron 1",
              IO::OUTPUT,
          },
          {
              "Offset 2-1",
              "Offset from neuron 2 to neuron 1",
              IO::OUTPUT,
          }};
}

struct scaler_state_t {
  double s12 = 0.0;
  double o12 = 0.0;
  double s21 = 0.0;
  double o21 = 0.0;
};

class Panel : public Widgets::Panel {
  Q_OBJECT
public:
  Panel(QMainWindow *main_window, Event::Manager *ev_manager);
  // Any functions and data related to the GUI are to be placed
  // here
  void refresh() override;

private:
  QLineEdit *s12_edit = nullptr;
  QLineEdit *s21_edit = nullptr;
  QLineEdit *o12_edit = nullptr;
  QLineEdit *o21_edit = nullptr;
};

class Plugin : public Widgets::Plugin {

public:
  explicit Plugin(Event::Manager *ev_manager);
  scaler_state_t get_scaler_state();

private:
  RT::OS::Fifo *component_fifo;
};

class Component : public Widgets::Component {
public:
  explicit Component(Widgets::Plugin *hplugin);

  void execute() override;
  // Additional functionality needed for RealTime computation is to be placed
  // here

  RT::OS::Fifo *get_fifo_ptr() { return this->fifo.get(); }

private:
  double period;
  double s12, s21, o12, o21;

  scaler_state_t scaler_state;
  std::unique_ptr<RT::OS::Fifo> fifo;

  void init_parameters();
};

} // namespace RTHybridAmplitudeScaleOffset
