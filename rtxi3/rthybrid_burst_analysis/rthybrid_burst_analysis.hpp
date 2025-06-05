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

#include <qlineedit.h>
#include <rtxi/widgets.hpp>

namespace RTHybridBurstAnalysis {

constexpr std::string_view MODULE_NAME = "RTHybrid Burst Analysis";

enum PARAMETER : Widgets::Variable::Id {
  // set parameter ids here
  BURST_ANALYSIS_OBST = 0,
  BURST_ANALYSIS_MIN,
  BURST_ANALYSIS_MAX,
  BURST_ANALYSIS_DUR,
  BURST_ANALYSIS_UPP_THRESH,
  BURST_ANALYSIS_DOWN_THRESH,
  BURST_ANALYSIS_AMPL,
  BURST_ANALYSIS_PC,
  BURST_ANALYSIS_BC,
  BURST_ANALYSIS_ISB,
};

inline std::vector<Widgets::Variable::Info> get_default_vars() {
  return {
      {PARAMETER::BURST_ANALYSIS_OBST, "Observation time (s)",
       "Observation time", Widgets::Variable::DOUBLE_PARAMETER, 5.0},
      {PARAMETER::BURST_ANALYSIS_MIN, "Min (V)", "Minimum voltage",
       Widgets::Variable::DOUBLE_PARAMETER, 999999.0},
      {PARAMETER::BURST_ANALYSIS_MAX, "Max (V)", "Maximum voltage",
       Widgets::Variable::DOUBLE_PARAMETER, -999999.0},
      {PARAMETER::BURST_ANALYSIS_DUR, "Burst duration (s)", "Burst duration",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {PARAMETER::BURST_ANALYSIS_UPP_THRESH, "Upper threshold (V)",
       "Upper threshold", Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {PARAMETER::BURST_ANALYSIS_DOWN_THRESH, "Lower threshold (V)",
       "Lower threshold", Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {PARAMETER::BURST_ANALYSIS_AMPL, "Amplitude (V)", "Amplitude",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {PARAMETER::BURST_ANALYSIS_PC, "Points counter", "Points counter",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {PARAMETER::BURST_ANALYSIS_BC, "Burst counter", "Burst counter",
       Widgets::Variable::DOUBLE_PARAMETER, -1.0},
      {PARAMETER::BURST_ANALYSIS_ISB, "Is burst", "",
       Widgets::Variable::DOUBLE_PARAMETER, 1.0},
  };
}
inline std::vector<IO::channel_t> get_default_channels() {
  return {
      {"Vm (V)", "Membrane potential (in V)", IO::INPUT},

      {"Min (V)", "Minimum membrane potential (in V)", IO::OUTPUT},
      {"Max (V)", "Maximum membrane potential (in V)", IO::OUTPUT},
      {"Burst duration (s)", "Average burst duration (in s)", IO::OUTPUT},
  };
}

struct analyzer_state_t {
  double min = 999999.0;
  double max = -999999.0;
  double sec_per_burst = 0.0;
  double pts_counter = 0.0;
  double burst_counter = 0.0;
  double thresh_up = 0.0;
  double thresh_down = 0.0;
  double range = 0.0;
  double is_burst = 0.0;
};

class Panel : public Widgets::Panel {
  Q_OBJECT
public:
  Panel(QMainWindow *main_window, Event::Manager *ev_manager);
  // Any functions and data related to the GUI are to be placed
  // here
  void refresh() override;

private:
  QLineEdit *min_edit = nullptr;
  QLineEdit *max_edit = nullptr;
  QLineEdit *dur_edit = nullptr;
  QLineEdit *upp_thresh_edit = nullptr;
  QLineEdit *down_thresh_edit = nullptr;
  QLineEdit *ampl_edit = nullptr;
  QLineEdit *pc_edit = nullptr;
  QLineEdit *bc_edit = nullptr;
  QLineEdit *isb_edit = nullptr;
};

class Plugin : public Widgets::Plugin {

public:
  explicit Plugin(Event::Manager *ev_manager);
  analyzer_state_t get_analyzer_state();

private:
  RT::OS::Fifo *component_fifo;
};

class Component : public Widgets::Component {
public:
  explicit Component(Widgets::Plugin *hplugin);

  void execute() override;
  // Additional functionality needed for RealTime computation is to be placed
  // here

  analyzer_state_t get_analyzer_states();
  RT::OS::Fifo *get_fifo_ptr() { return this->fifo.get(); }

private:
  double period, freq;
  double observation_time, temp_min, temp_max, count, burst_dur_sum,
      old_burst_time;
  double min, max, sec_per_burst, thresh_up, thresh_down, range, is_burst,
      pts_counter, burst_counter;

  analyzer_state_t analyzer_state;
  std::unique_ptr<RT::OS::Fifo> fifo;

  void init_parameters();
};

} // namespace RTHybridBurstAnalysis
