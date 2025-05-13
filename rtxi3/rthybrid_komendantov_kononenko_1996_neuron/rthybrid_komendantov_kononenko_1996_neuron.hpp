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

namespace RTHybridKomendantovKononenko1996Neuron {

constexpr std::string_view MODULE_NAME =
    "RTHybrid Komendantov-Kononenko (1996) Neuron";

enum VARIABLE {
  NM_KOMENDANTOV_KONONENKO_1996_V = 0,
  NM_KOMENDANTOV_KONONENKO_1996_M_B,
  NM_KOMENDANTOV_KONONENKO_1996_H_B,
  NM_KOMENDANTOV_KONONENKO_1996_M,
  NM_KOMENDANTOV_KONONENKO_1996_H,
  NM_KOMENDANTOV_KONONENKO_1996_N,
  NM_KOMENDANTOV_KONONENKO_1996_M_CA,
  NM_KOMENDANTOV_KONONENKO_1996_CA,
  VARIABLE_SIZE // If more variables are added, this must be the last one
};

enum PARAMETER {
  NM_KOMENDANTOV_KONONENKO_1996_DT = 0,
  NM_KOMENDANTOV_KONONENKO_1996_I,
  NM_KOMENDANTOV_KONONENKO_1996_SYN,
  NM_KOMENDANTOV_KONONENKO_1996_CM,
  NM_KOMENDANTOV_KONONENKO_1996_G_NA_V,
  NM_KOMENDANTOV_KONONENKO_1996_V_NA,
  NM_KOMENDANTOV_KONONENKO_1996_G_K,
  NM_KOMENDANTOV_KONONENKO_1996_V_K,
  NM_KOMENDANTOV_KONONENKO_1996_G_NA,
  NM_KOMENDANTOV_KONONENKO_1996_G_B,
  NM_KOMENDANTOV_KONONENKO_1996_V_B,
  NM_KOMENDANTOV_KONONENKO_1996_G_NA_TTX,
  NM_KOMENDANTOV_KONONENKO_1996_G_K_TEA,
  NM_KOMENDANTOV_KONONENKO_1996_G_CA,
  NM_KOMENDANTOV_KONONENKO_1996_V_CA,
  NM_KOMENDANTOV_KONONENKO_1996_G_CA_CA,
  NM_KOMENDANTOV_KONONENKO_1996_K_BETA,
  NM_KOMENDANTOV_KONONENKO_1996_BETA,
  NM_KOMENDANTOV_KONONENKO_1996_RHO,
  NM_KOMENDANTOV_KONONENKO_1996_K_S
};

enum VIEW_PARAMETER : Widgets::Variable::Id {
  V_NM_KOMENDANTOV_KONONENKO_1996_BD = 0,
  V_NM_KOMENDANTOV_KONONENKO_1996_I,
  V_NM_KOMENDANTOV_KONONENKO_1996_CM,
  V_NM_KOMENDANTOV_KONONENKO_1996_G_NA_V,
  V_NM_KOMENDANTOV_KONONENKO_1996_V_NA,
  V_NM_KOMENDANTOV_KONONENKO_1996_G_NA,
  V_NM_KOMENDANTOV_KONONENKO_1996_G_NA_TTX,
  V_NM_KOMENDANTOV_KONONENKO_1996_G_K,
  V_NM_KOMENDANTOV_KONONENKO_1996_G_K_TEA,
  V_NM_KOMENDANTOV_KONONENKO_1996_V_K,
  V_NM_KOMENDANTOV_KONONENKO_1996_G_B,
  V_NM_KOMENDANTOV_KONONENKO_1996_V_B,
  V_NM_KOMENDANTOV_KONONENKO_1996_G_CA,
  V_NM_KOMENDANTOV_KONONENKO_1996_V_CA,
  V_NM_KOMENDANTOV_KONONENKO_1996_G_CA_CA,
  V_NM_KOMENDANTOV_KONONENKO_1996_K_BETA,
  V_NM_KOMENDANTOV_KONONENKO_1996_BETA,
  V_NM_KOMENDANTOV_KONONENKO_1996_RHO,
  V_NM_KOMENDANTOV_KONONENKO_1996_K_S,
  V_NM_KOMENDANTOV_KONONENKO_1996_V0,
  V_NM_KOMENDANTOV_KONONENKO_1996_V,
  V_NM_KOMENDANTOV_KONONENKO_1996_SP,
  V_NM_KOMENDANTOV_KONONENKO_1996_DT,
  V_NM_KOMENDANTOV_KONONENKO_1996_SYN
};

inline std::vector<Widgets::Variable::Info> get_default_vars() {
  return {
      {V_NM_KOMENDANTOV_KONONENKO_1996_BD, "Burst duration (s)",
       "Burst duration (s)", Widgets::Variable::DOUBLE_PARAMETER, 1.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_I, "i", "i",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_CM, "cm", "cm",
       Widgets::Variable::DOUBLE_PARAMETER, 0.02},
      {V_NM_KOMENDANTOV_KONONENKO_1996_G_NA_V, "g_na_v", "g_na_v",
       Widgets::Variable::DOUBLE_PARAMETER, 0.11},
      {V_NM_KOMENDANTOV_KONONENKO_1996_V_NA, "v_na", "v_na",
       Widgets::Variable::DOUBLE_PARAMETER, 40.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_G_NA, "g_na", "g_na",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0231},
      {V_NM_KOMENDANTOV_KONONENKO_1996_G_NA_TTX, "g_na_ttx", "g_na_ttx",
       Widgets::Variable::DOUBLE_PARAMETER, 400.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_G_K, "g_k", "g_k",
       Widgets::Variable::DOUBLE_PARAMETER, 0.25},
      {V_NM_KOMENDANTOV_KONONENKO_1996_G_K_TEA, "g_k_tea", "g_k_tea",
       Widgets::Variable::DOUBLE_PARAMETER, 10.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_V_K, "v_k", "v_k",
       Widgets::Variable::DOUBLE_PARAMETER, -70.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_G_B, "g_b", "g_b",
       Widgets::Variable::DOUBLE_PARAMETER, 0.165},
      {V_NM_KOMENDANTOV_KONONENKO_1996_V_B, "v_b", "v_b",
       Widgets::Variable::DOUBLE_PARAMETER, -58.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_G_CA, "g_ca", "g_ca",
       Widgets::Variable::DOUBLE_PARAMETER, 1.5},
      {V_NM_KOMENDANTOV_KONONENKO_1996_V_CA, "v_ca", "v_ca",
       Widgets::Variable::DOUBLE_PARAMETER, 150.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_G_CA_CA, "g_ca_ca", "g_ca_ca",
       Widgets::Variable::DOUBLE_PARAMETER, 0.02},
      {V_NM_KOMENDANTOV_KONONENKO_1996_K_BETA, "k_beta", "k_beta",
       Widgets::Variable::DOUBLE_PARAMETER, 15000.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_BETA, "beta", "beta",
       Widgets::Variable::DOUBLE_PARAMETER, 0.00004},
      {V_NM_KOMENDANTOV_KONONENKO_1996_RHO, "rho", "rho",
       Widgets::Variable::DOUBLE_PARAMETER, 0.002},
      {V_NM_KOMENDANTOV_KONONENKO_1996_K_S, "k_s", "k_s",
       Widgets::Variable::DOUBLE_PARAMETER, 50.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_V0, "v0", "v0",
       Widgets::Variable::DOUBLE_PARAMETER, -55.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_V, "Vm", "Vm",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_SP, "s_points", "s_points",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_DT, "dt", "dt",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},
      {V_NM_KOMENDANTOV_KONONENKO_1996_SYN, "syn", "syn",
       Widgets::Variable::DOUBLE_PARAMETER, 0.0},

  };
}
inline std::vector<IO::channel_t> get_default_channels() {
  return {
      {"Vm (v)", "Vm (v)", IO::OUTPUT},
      {"Vm (mV)", "Vm (mV)", IO::OUTPUT},
      {"Isyn (nA)", "Isyn (nA)", IO::INPUT},
      {"Burst duration (s)", "", IO::INPUT},
  };
}

struct neuron_state_t {
  double v = 0.0;
  double s_points = 0.0;
  double dt_points = 0.0;
  double syn_points = 0.0;
};

class Panel : public Widgets::Panel {
  Q_OBJECT
public:
  Panel(QMainWindow *main_window, Event::Manager *ev_manager);
  // Any functions and data related to the GUI are to be placed
  // here
  void refresh() override;

private:
  QLineEdit *v_edit = nullptr;
  QLineEdit *sp_edit = nullptr;
  QLineEdit *dt_edit = nullptr;
  QLineEdit *syn_edit = nullptr;
};

class Plugin : public Widgets::Plugin {

public:
  explicit Plugin(Event::Manager *ev_manager);
  neuron_state_t get_neuron_state();

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
  double period, freq;
  double burst_duration;
  double vars_model[8];
  double params_model[20];
  double s_points;
  double burst_duration_value;

  neuron_state_t neuron_state;
  std::unique_ptr<RT::OS::Fifo> fifo;

  void init_parameters();

  void runge_kutta_65(void (*f)(double *, double *, double *, double), int dim,
                      double dt, double *vars, double *params, double aux);
  void euler(void (*f)(double *, double *, double *, double), int dim,
             double dt, double *vars, double *params, double aux);
  void select_dt_neuron_model(double *dts, double *pts, unsigned int length,
                              double pts_live, double *dt, double *pts_burst);
  double set_pts_burst(double sec_per_burst);
  static double nm_komendantov_kononenko_1996_V(double *vars, double *params);
  static double nm_komendantov_kononenko_1996_i_na_v(double *vars,
                                                     double *params);
  static double nm_komendantov_kononenko_1996_i_k(double *vars, double *params);
  static double nm_komendantov_kononenko_1996_i_na(double *vars,
                                                   double *params);
  static double nm_komendantov_kononenko_1996_i_b(double *vars, double *params);
  static double nm_komendantov_kononenko_1996_m_b(double *vars, double *params);
  static double nm_komendantov_kononenko_1996_h_b(double *vars, double *params);
  static double nm_komendantov_kononenko_1996_i_na_ttx(double *vars,
                                                       double *params);
  static double nm_komendantov_kononenko_1996_i_k_tea(double *vars,
                                                      double *params);
  static double nm_komendantov_kononenko_1996_m(double *vars, double *params);
  static double nm_komendantov_kononenko_1996_h(double *vars, double *params);
  static double nm_komendantov_kononenko_1996_n(double *vars, double *params);
  static double nm_komendantov_kononenko_1996_i_ca(double *vars,
                                                   double *params);
  static double nm_komendantov_kononenko_1996_m_ca(double *vars,
                                                   double *params);
  static double nm_komendantov_kononenko_1996_i_ca_ca(double *vars,
                                                      double *params);
  static double nm_komendantov_kononenko_1996_Ca(double *vars, double *params);
  static void nm_komendantov_kononenko_1996_f(double *vars, double *ret,
                                              double *params, double syn);
};

} // namespace RTHybridKomendantovKononenko1996Neuron
