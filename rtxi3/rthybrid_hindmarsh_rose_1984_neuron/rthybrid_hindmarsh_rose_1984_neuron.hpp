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
#include <rtxi/rt.hpp>
#include <rtxi/widgets.hpp>

namespace RTHybridHindmarshRose1984Neuron {

constexpr std::string_view MODULE_NAME =
    "RTHybrid Hindmarsh-Rose (1984) Neuron";

enum VARIABLE {
  NM_HINDMARSH_ROSE_1984_V = 0,
  NM_HINDMARSH_ROSE_1984_Y,
  NM_HINDMARSH_ROSE_1984_Z
};

enum VIEW_PARAMETER : Widgets::Variable::Id {
  V_NM_HINDMARSH_ROSE_1984_BD = 0,
  V_NM_HINDMARSH_ROSE_1984_I,
  V_NM_HINDMARSH_ROSE_1984_A,
  V_NM_HINDMARSH_ROSE_1984_B,
  V_NM_HINDMARSH_ROSE_1984_C,
  V_NM_HINDMARSH_ROSE_1984_D,
  V_NM_HINDMARSH_ROSE_1984_R,
  V_NM_HINDMARSH_ROSE_1984_S,
  V_NM_HINDMARSH_ROSE_1984_XR,
  V_NM_HINDMARSH_ROSE_1984_V0,
  V_NM_HINDMARSH_ROSE_1984_Y0,
  V_NM_HINDMARSH_ROSE_1984_Z0,
  V_NM_HINDMARSH_ROSE_1984_V,
  V_NM_HINDMARSH_ROSE_1984_SP,
  V_NM_HINDMARSH_ROSE_1984_DT,
  V_NM_HINDMARSH_ROSE_1984_SYN,
};

enum PARAMETER {
  NM_HINDMARSH_ROSE_1984_I = 0,
  NM_HINDMARSH_ROSE_1984_A,
  NM_HINDMARSH_ROSE_1984_B,
  NM_HINDMARSH_ROSE_1984_C,
  NM_HINDMARSH_ROSE_1984_D,
  NM_HINDMARSH_ROSE_1984_DT,
  NM_HINDMARSH_ROSE_1984_SYN,
  NM_HINDMARSH_ROSE_1984_XR,
  NM_HINDMARSH_ROSE_1984_R,
  NM_HINDMARSH_ROSE_1984_S,
};

inline std::vector<Widgets::Variable::Info> get_default_vars() {
  return {
      {V_NM_HINDMARSH_ROSE_1984_BD, "Burst duration (s)", "Burst duration",
       Widgets::Variable::DOUBLE_PARAMETER, 1.0},
      {V_NM_HINDMARSH_ROSE_1984_I, "I", "Applied external current",
       Widgets::Variable::DOUBLE_PARAMETER, 3.0},
      {V_NM_HINDMARSH_ROSE_1984_A, "a", "Parameter a",
       Widgets::Variable::DOUBLE_PARAMETER, 1.0},
      {V_NM_HINDMARSH_ROSE_1984_B, "b", "Parameter b",
       Widgets::Variable::DOUBLE_PARAMETER, 3.0},
      {V_NM_HINDMARSH_ROSE_1984_C, "c", "Parameter c",
       Widgets::Variable::DOUBLE_PARAMETER, 1.0},
      {V_NM_HINDMARSH_ROSE_1984_D, "d", "Parameter d",
       Widgets::Variable::DOUBLE_PARAMETER, 5.0},
      {V_NM_HINDMARSH_ROSE_1984_R, "r", "Parameter r",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0021},
      {V_NM_HINDMARSH_ROSE_1984_S, "s", "Parameter s",
       Widgets::Variable::DOUBLE_PARAMETER, 4.0},
      {V_NM_HINDMARSH_ROSE_1984_XR, "xr", "Parameter xr",
       Widgets::Variable::DOUBLE_PARAMETER, -1.6},
      {V_NM_HINDMARSH_ROSE_1984_V0, "x0", "Membrane potential",
       Widgets::Variable::DOUBLE_PARAMETER, -0.712841},
      {V_NM_HINDMARSH_ROSE_1984_Y0, "y0", "Parameter y",
       Widgets::Variable::DOUBLE_PARAMETER, -1.936880},
      {V_NM_HINDMARSH_ROSE_1984_Z0, "z0", "Parameter z",
       Widgets::Variable::DOUBLE_PARAMETER, 3.165680},
      {V_NM_HINDMARSH_ROSE_1984_V, "v", "Membrane potential",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {V_NM_HINDMARSH_ROSE_1984_SP, "s_points", "Number of integration steps ",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {V_NM_HINDMARSH_ROSE_1984_DT, "dt", "Integration time step",
       Widgets::Variable::DOUBLE_PARAMETER, 0.00},
      {V_NM_HINDMARSH_ROSE_1984_SYN, "syn", "Synaptic current",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
  };
}
inline std::vector<IO::channel_t> get_default_channels() {
  return {{"Vm (v)", "Membrane potential (in V)", IO::OUTPUT},
          {"Vm (mV)", "Membrane potential (in mV)", IO::OUTPUT},
          {"Isyn (nA)", "Synaptic input current (in nA)", IO::INPUT},
          {"Burst duration (s)", "Burst duration", IO::INPUT}};
}

class Panel : public Widgets::Panel {
  Q_OBJECT
public:
  Panel(QMainWindow *main_window, Event::Manager *ev_manager);
  // Any functions and data related to the GUI are to be placed
  // here
  void refresh();
  QLineEdit *v_edit = nullptr;
  QLineEdit *sp_edit = nullptr;
  QLineEdit *dt_edit = nullptr;
  QLineEdit *syn_edit = nullptr;
};

class Component : public Widgets::Component {
public:
  explicit Component(Widgets::Plugin *hplugin);
  void execute() override;
  // Additional functionality needed for RealTime computation is to be placed
  // here
  static Component *instance;

  double params_model[10];
  double vars_model[3];
  double s_points;

private:
  double period, freq;
  double burst_duration, burst_duration_value;

  void initParameters();
  double set_pts_burst(double sec_per_burst);
  void select_dt_neuron_model(double *dts, double *pts, unsigned int length,
                              double pts_live, double *dt, double *pts_burst);
  static double nm_hindmarsh_rose_1984_v(double *vars, double *params);
  static double nm_hindmarsh_rose_1984_y(double *vars, double *params);
  static double nm_hindmarsh_rose_1984_z(double *vars, double *params);
  static void nm_hindmarsh_rose_1984_f(double *vars, double *ret,
                                       double *params, double syn);
  void runge_kutta_65(void (*f)(double *, double *, double *, double), int dim,
                      double dt, double *vars, double *params, double aux);
};

class Plugin : public Widgets::Plugin {
public:
  explicit Plugin(Event::Manager *ev_manager);
};

} // namespace RTHybridHindmarshRose1984Neuron
