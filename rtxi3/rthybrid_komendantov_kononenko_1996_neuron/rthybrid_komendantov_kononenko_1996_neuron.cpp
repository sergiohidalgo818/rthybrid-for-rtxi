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
#include <QTimer>
#include <cmath>
#include <cstdio>
#include <qobject.h>
#include <rtxi/rt.hpp>
#include <rtxi/rtos.hpp>

RTHybridKomendantovKononenko1996Neuron::Component
    *RTHybridKomendantovKononenko1996Neuron::Component::instance = nullptr;

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

  v_edit = edits[V_NM_KOMENDANTOV_KONONENKO_1996_V];
  sp_edit = edits[V_NM_KOMENDANTOV_KONONENKO_1996_SP];
  dt_edit = edits[V_NM_KOMENDANTOV_KONONENKO_1996_DT];
  syn_edit = edits[V_NM_KOMENDANTOV_KONONENKO_1996_SYN];

  if (v_edit) {
    v_edit->setReadOnly(true);
    v_edit->setStyleSheet(readonlyStyle);
  }
  if (sp_edit) {
    sp_edit->setReadOnly(true);
    sp_edit->setStyleSheet(readonlyStyle);
  }
  if (dt_edit) {
    dt_edit->setReadOnly(true);
    dt_edit->setStyleSheet(readonlyStyle);
  }
  if (syn_edit) {
    syn_edit->setReadOnly(true);
    syn_edit->setStyleSheet(readonlyStyle);
  }

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this,
          &RTHybridKomendantovKononenko1996Neuron::Panel::refresh);
  timer->start(500); // refresh every 500 ms
  this->parentWidget()->adjustSize();
}

RTHybridKomendantovKononenko1996Neuron::Component::Component(
    Widgets::Plugin *hplugin)
    : Widgets::Component(
          hplugin,
          std::string(RTHybridKomendantovKononenko1996Neuron::MODULE_NAME),
          RTHybridKomendantovKononenko1996Neuron::get_default_channels(),
          RTHybridKomendantovKononenko1996Neuron::get_default_vars()) {
  Component::instance = this;
}

void RTHybridKomendantovKononenko1996Neuron::Panel::refresh() {
  auto *comp = RTHybridKomendantovKononenko1996Neuron::Component::instance;
  if (comp && v_edit) {
    v_edit->setText(
        QString::number(comp->vars_model[NM_KOMENDANTOV_KONONENKO_1996_V]));
  }
  if (comp && sp_edit) {
    sp_edit->setText(QString::number(comp->s_points));
  }
  if (comp && dt_edit) {
    dt_edit->setText(
        QString::number(comp->params_model[NM_KOMENDANTOV_KONONENKO_1996_DT]));
  }
  if (comp && syn_edit) {
    syn_edit->setText(
        QString::number(comp->params_model[NM_KOMENDANTOV_KONONENKO_1996_SYN]));
  }
}

void RTHybridKomendantovKononenko1996Neuron::Component::initParameters() {
  burst_duration_value = getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_BD);
  burst_duration = burst_duration_value;

  freq = 1.0 / (period * 1e-3);
  s_points = (int)(set_pts_burst(burst_duration) / (burst_duration * freq));
  if (s_points == 0)
    s_points = 1;
  params_model[NM_KOMENDANTOV_KONONENKO_1996_I] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_I);

  params_model[NM_KOMENDANTOV_KONONENKO_1996_SYN] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_SYN);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_CM] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_CM);

  params_model[NM_KOMENDANTOV_KONONENKO_1996_G_NA_V] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_G_NA_V);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_V_NA] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_V_NA);

  params_model[NM_KOMENDANTOV_KONONENKO_1996_G_NA] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_G_NA);

  params_model[NM_KOMENDANTOV_KONONENKO_1996_G_NA_TTX] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_G_NA_TTX);

  params_model[NM_KOMENDANTOV_KONONENKO_1996_G_K] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_G_K);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_G_K_TEA] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_G_K_TEA);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_V_K] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_V_K);

  params_model[NM_KOMENDANTOV_KONONENKO_1996_G_B] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_G_B);

  params_model[NM_KOMENDANTOV_KONONENKO_1996_V_B] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_V_B);

  params_model[NM_KOMENDANTOV_KONONENKO_1996_G_CA] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_G_CA);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_V_CA] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_V_CA);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_G_CA_CA] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_G_CA_CA);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_K_BETA] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_K_BETA);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_BETA] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_BETA);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_RHO] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_RHO);
  params_model[NM_KOMENDANTOV_KONONENKO_1996_K_S] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_K_S);

  vars_model[NM_KOMENDANTOV_KONONENKO_1996_V] =
      getValue<double>(V_NM_KOMENDANTOV_KONONENKO_1996_V);
  vars_model[NM_KOMENDANTOV_KONONENKO_1996_CA] = 0;
  vars_model[NM_KOMENDANTOV_KONONENKO_1996_M_B] =
      (1.000000 /
       (1.000000 + exp(0.400000 * (vars_model[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                   34.000000))));
  vars_model[NM_KOMENDANTOV_KONONENKO_1996_H_B] =
      (1.000000 / (1.000000 + exp(-0.550000 *
                                  (vars_model[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                   43.000000))));
  vars_model[NM_KOMENDANTOV_KONONENKO_1996_M] =
      (1.000000 / (1.000000 + exp(-0.400000 *
                                  (vars_model[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                   31.000000))));
  vars_model[NM_KOMENDANTOV_KONONENKO_1996_H] =
      (1.000000 /
       (1.000000 + exp(0.250000 * (vars_model[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                   45.000000))));
  vars_model[NM_KOMENDANTOV_KONONENKO_1996_N] =
      (1.000000 / (1.000000 + exp(-0.180000 *
                                  (vars_model[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                   25.000000))));
  vars_model[NM_KOMENDANTOV_KONONENKO_1996_M_CA] =
      (1.000000 /
       (1.000000 +
        exp(-0.200000 * vars_model[NM_KOMENDANTOV_KONONENKO_1996_V])));
}
void RTHybridKomendantovKononenko1996Neuron::Component::execute() {
  // This is the real-time function that will be called
  switch (this->getState()) {
  case RT::State::EXEC:
    int i;

    if (burst_duration_value <= -1) {
      burst_duration = readinput(1);
      s_points = (int)(set_pts_burst(burst_duration) / (burst_duration * freq));
      if (s_points < 1)
        s_points = 1;
    }

    for (i = 0; i < s_points; i++) {
      runge_kutta_65(nm_komendantov_kononenko_1996_f, VARIABLE_SIZE,
                     params_model[NM_KOMENDANTOV_KONONENKO_1996_DT], vars_model,
                     params_model, readinput(0));
    }

    writeoutput(0, vars_model[NM_KOMENDANTOV_KONONENKO_1996_V] / 1000.0);
    writeoutput(1, vars_model[NM_KOMENDANTOV_KONONENKO_1996_V]);
    break;
  case RT::State::INIT:
    period = RT::OS::getPeriod() * 1e-6; // ms
    freq = 1.0 / (period * 1e-3);

    this->initParameters();
    setState(RT::State::EXEC);
    break;
  case RT::State::MODIFY:
    period = RT::OS::getPeriod() * 1e-6; // ms
    freq = 1.0 / (period * 1e-3);

    this->initParameters();
    setState(RT::State::PAUSE);
    break;
  case RT::State::PERIOD:
    freq = 1.0 / (period * 1e-3);
    s_points = (int)(set_pts_burst(burst_duration) / (burst_duration * freq));
    if (s_points == 0)
      s_points = 1;
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

void RTHybridKomendantovKononenko1996Neuron::Component::runge_kutta_65(
    void (*f)(double *, double *, double *, double), int dim, double dt,
    double *vars, double *params, double aux) {
  double apoyo[dim], retorno[dim];
  double k[6][dim];
  int j;

  (*f)(vars, retorno, params, aux);
  for (j = 0; j < dim; ++j) {
    k[0][j] = dt * retorno[j];
    apoyo[j] = vars[j] + k[0][j] * 0.2;
  }

  (*f)(apoyo, retorno, params, aux);
  for (j = 0; j < dim; ++j) {
    k[1][j] = dt * retorno[j];
    apoyo[j] = vars[j] + k[0][j] * 0.075 + k[1][j] * 0.225;
  }

  (*f)(apoyo, retorno, params, aux);
  for (j = 0; j < dim; ++j) {
    k[2][j] = dt * retorno[j];
    apoyo[j] = vars[j] + k[0][j] * 0.3 - k[1][j] * 0.9 + k[2][j] * 1.2;
  }

  (*f)(apoyo, retorno, params, aux);
  for (j = 0; j < dim; ++j) {
    k[3][j] = dt * retorno[j];
    apoyo[j] = vars[j] + k[0][j] * 0.075 + k[1][j] * 0.675 - k[2][j] * 0.6 +
               k[3][j] * 0.75;
  }

  (*f)(apoyo, retorno, params, aux);
  for (j = 0; j < dim; ++j) {
    k[4][j] = dt * retorno[j];
    apoyo[j] = vars[j] + k[0][j] * 0.660493827160493 + k[1][j] * 2.5 -
               k[2][j] * 5.185185185185185 + k[3][j] * 3.888888888888889 -
               k[4][j] * 0.864197530864197;
  }

  (*f)(apoyo, retorno, params, aux);
  for (j = 0; j < dim; ++j) {
    k[5][j] = dt * retorno[j];
    apoyo[j] = vars[j] + k[0][j] * 0.1049382716049382 +
               k[2][j] * 0.3703703703703703 + k[3][j] * 0.2777777777777777 +
               k[4][j] * 0.2469135802469135;
  }

  for (j = 0; j < dim; ++j) {
    vars[j] += k[0][j] * 0.098765432098765 + k[2][j] * 0.396825396825396 +
               k[3][j] * 0.231481481481481 + k[4][j] * 0.308641975308641 -
               k[5][j] * 0.035714285714285;
  }

  return;
}

void RTHybridKomendantovKononenko1996Neuron::Component::euler(
    void (*f)(double *, double *, double *, double), int dim, double dt,
    double *vars, double *params, double aux) {
  double apoyo[dim], retorno[dim];
  double k[4][dim];
  int j;

  (*f)(vars, retorno, params, aux);

  for (j = 0; j < dim; ++j) {
    vars[j] += dt * retorno[j];
  }

  return;
}

/**
 * @brief Selects the best integration step for a neuron model.
 * @param[in] dts Array with the integration steps
 * @param[in] pts Array with the numbers of points needed to complete one burst
 * by the neuron model using the integration steps in dts
 * @param[in] length Length of the dts and pts arrays
 * @param[in] pts_live Number of points of burst from the living neuron
 * @param[out] dt Pointer to store the selected integration step-. If no
 * integration step is found the value will be -1
 * @param[out] dt Pointer to store the number of points per burst from the
 * neuron model with selected integration step. If no integration step is found
 * the value will be -1
 */

void RTHybridKomendantovKononenko1996Neuron::Component::select_dt_neuron_model(
    double *dts, double *pts, unsigned int length, double pts_live, double *dt,
    double *pts_burst) {
  double aux = pts_live;
  double factor = 1;
  double intpart, fractpart;
  int flag = 0;
  int i;

  *dt = -1;
  *pts_burst = -1;

  while (aux < pts[0]) {
    aux = pts_live * factor;
    factor += 1;

    for (i = length - 1; i >= 0; i--) {
      if (pts[i] > aux) {
        *dt = dts[i];
        *pts_burst = pts[i];

        fractpart = modf(*pts_burst / pts_live, &intpart);

        if (fractpart <= 0.1 * intpart)
          flag = 1;

        break;
      }
    }

    if (flag == 1)
      break;
  }

  if (flag == 0) {
    for (i = length - 1; i >= 0; i--) {
      if (pts[i] > aux) {
        *dt = dts[i];
        *pts_burst = pts[i];

        break;
      }
    }
  }

  return;
}

/**
 * @brief Sets Komendantov_Kononenko_1996 model number of points per burst and
 * integration step.
 *
 * If not previously specified by the user, the number of points per burst of
 * the model and its integration step is set according to the living neuron
 * number of points per burst.
 * @param[in] pts_match Number of points in a burst to match
 * @param[in] nm Pointer to the neuron model structure
 * @return Integration step, default -1
 */

double RTHybridKomendantovKononenko1996Neuron::Component::set_pts_burst(
    double sec_per_burst) {
  int length = 0;
  int method = 3;
  double pts_match = sec_per_burst * freq;
  double pts_burst, dt;

  length = 19.000000;
  double dts[] = {0.000010, 0.000020, 0.000030, 0.000040, 0.000050,
                  0.000060, 0.000070, 0.000080, 0.000090, 0.000100,
                  0.000200, 0.000300, 0.000400, 0.000500, 0.000600,
                  0.000700, 0.000800, 0.000900, 0.001000};
  double pts[] = {489646.000000, 250715.000000, 172155.000000, 131029.714286,
                  105694.000000, 88541.272727,  76167.307692,  66740.857143,
                  59461.687500,  53611.000000,  26996.527778,  18041.654545,
                  13546.534247,  10844.543478,  9040.509091,   7750.382812,
                  6783.319728,   6030.793939,   5428.152174};

  select_dt_neuron_model(dts, pts, length, pts_match,
                         &(params_model[NM_KOMENDANTOV_KONONENKO_1996_DT]),
                         &(pts_burst));

  return pts_burst;
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_V(double *vars, double *params) {
  double i_na_ttx = nm_komendantov_kononenko_1996_i_na_ttx(vars, params);
  double i_k_tea = nm_komendantov_kononenko_1996_i_k_tea(vars, params);
  double i_k = nm_komendantov_kononenko_1996_i_k(vars, params);
  double i_na = nm_komendantov_kononenko_1996_i_na(vars, params);
  double i_na_v = nm_komendantov_kononenko_1996_i_na_v(vars, params);
  double i_b = nm_komendantov_kononenko_1996_i_b(vars, params);
  double i_ca = nm_komendantov_kononenko_1996_i_ca(vars, params);
  double i_ca_ca = nm_komendantov_kononenko_1996_i_ca_ca(vars, params);

  return (-(i_na_ttx + i_k_tea + i_k + i_na + i_na_v + i_b + i_ca + i_ca_ca) +
          params[NM_KOMENDANTOV_KONONENKO_1996_I] -
          params[NM_KOMENDANTOV_KONONENKO_1996_SYN]) /
         params[NM_KOMENDANTOV_KONONENKO_1996_CM];
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_i_na_v(double *vars, double *params) {
  return params[NM_KOMENDANTOV_KONONENKO_1996_G_NA_V] *
         (1.000000 /
          (1.000000 + exp(-0.200000 * (vars[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                       45.000000)))) *
         (vars[NM_KOMENDANTOV_KONONENKO_1996_V] -
          params[NM_KOMENDANTOV_KONONENKO_1996_V_NA]);
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_i_k(double *vars, double *params) {
  return params[NM_KOMENDANTOV_KONONENKO_1996_G_K] *
         (vars[NM_KOMENDANTOV_KONONENKO_1996_V] -
          params[NM_KOMENDANTOV_KONONENKO_1996_V_K]);
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_i_na(double *vars, double *params) {
  return params[NM_KOMENDANTOV_KONONENKO_1996_G_NA] *
         (vars[NM_KOMENDANTOV_KONONENKO_1996_V] -
          params[NM_KOMENDANTOV_KONONENKO_1996_V_NA]);
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_i_b(double *vars, double *params) {
  return params[NM_KOMENDANTOV_KONONENKO_1996_G_B] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_M_B] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_H_B] *
         (vars[NM_KOMENDANTOV_KONONENKO_1996_V] -
          params[NM_KOMENDANTOV_KONONENKO_1996_V_B]);
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_m_b(double *vars, double *params) {
  return (1.000000 / (1.000000 +
                      exp(0.400000 * (vars[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                      34.000000))) -
          vars[NM_KOMENDANTOV_KONONENKO_1996_M_B]) /
         0.050000;
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_h_b(double *vars, double *params) {
  return (1.000000 / (1.000000 +
                      exp(-0.550000 * (vars[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                       43.000000))) -
          vars[NM_KOMENDANTOV_KONONENKO_1996_H_B]) /
         1.500000;
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_i_na_ttx(double *vars, double *params) {
  return params[NM_KOMENDANTOV_KONONENKO_1996_G_NA_TTX] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_M] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_M] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_M] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_H] *
         (vars[NM_KOMENDANTOV_KONONENKO_1996_V] -
          params[NM_KOMENDANTOV_KONONENKO_1996_V_NA]);
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_i_k_tea(double *vars, double *params) {
  return params[NM_KOMENDANTOV_KONONENKO_1996_G_K_TEA] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_N] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_N] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_N] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_N] *
         (vars[NM_KOMENDANTOV_KONONENKO_1996_V] -
          params[NM_KOMENDANTOV_KONONENKO_1996_V_K]);
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_m(double *vars, double *params) {
  // printf("(1 / 1 + exp(-0.4 * (V[%f] + 31))) - m[%f]) / 0.0005\n",
  // vars[NM_KOMENDANTOV_KONONENKO_1996_V],
  // vars[NM_KOMENDANTOV_KONONENKO_1996_M]);
  return (1.0 / (1.0 +
                 exp(-0.4 * (vars[NM_KOMENDANTOV_KONONENKO_1996_V] + 31.0))) -
          vars[NM_KOMENDANTOV_KONONENKO_1996_M]) /
         0.0005;
  // return (1.000000 / (1.000000 + exp(-0.400000 *
  // (vars[NM_KOMENDANTOV_KONONENKO_1996_V] + 31.000000))) -
  // vars[NM_KOMENDANTOV_KONONENKO_1996_M]) / 0.000500;
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_h(double *vars, double *params) {
  return (1.000000 / (1.000000 +
                      exp(0.250000 * (vars[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                      45.000000))) -
          vars[NM_KOMENDANTOV_KONONENKO_1996_H]) /
         0.010000;
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_n(double *vars, double *params) {
  return (1.000000 / (1.000000 +
                      exp(-0.180000 * (vars[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                       25.000000))) -
          vars[NM_KOMENDANTOV_KONONENKO_1996_N]) /
         0.015000;
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_i_ca(double *vars, double *params) {
  return params[NM_KOMENDANTOV_KONONENKO_1996_G_CA] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_M_CA] *
         vars[NM_KOMENDANTOV_KONONENKO_1996_M_CA] *
         (vars[NM_KOMENDANTOV_KONONENKO_1996_V] -
          params[NM_KOMENDANTOV_KONONENKO_1996_V_CA]);
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_m_ca(double *vars, double *params) {
  return (1.000000 / (1.000000 +
                      exp(-0.200000 * vars[NM_KOMENDANTOV_KONONENKO_1996_V])) -
          vars[NM_KOMENDANTOV_KONONENKO_1996_M_CA]) /
         0.010000;
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_i_ca_ca(double *vars, double *params) {
  return params[NM_KOMENDANTOV_KONONENKO_1996_G_CA_CA] *
         (1.000000 /
          (1.000000 + exp(-0.060000 * (vars[NM_KOMENDANTOV_KONONENKO_1996_V] +
                                       45.000000)))) *
         (1.000000 /
          (1.000000 + exp(params[NM_KOMENDANTOV_KONONENKO_1996_K_BETA] *
                          (vars[NM_KOMENDANTOV_KONONENKO_1996_CA] -
                           params[NM_KOMENDANTOV_KONONENKO_1996_BETA])))) *
         (vars[NM_KOMENDANTOV_KONONENKO_1996_V] -
          params[NM_KOMENDANTOV_KONONENKO_1996_V_CA]);
}

double RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_Ca(double *vars, double *params) {
  double i_ca = nm_komendantov_kononenko_1996_i_ca(vars, params);
  return params[NM_KOMENDANTOV_KONONENKO_1996_RHO] *
         ((-i_ca / 808.310846) - (params[NM_KOMENDANTOV_KONONENKO_1996_K_S] *
                                  vars[NM_KOMENDANTOV_KONONENKO_1996_CA]));
}

void RTHybridKomendantovKononenko1996Neuron::Component::
    nm_komendantov_kononenko_1996_f(double *vars, double *ret, double *params,
                                    double syn) {
  params[NM_KOMENDANTOV_KONONENKO_1996_SYN] = syn;

  ret[NM_KOMENDANTOV_KONONENKO_1996_V] =
      nm_komendantov_kononenko_1996_V(vars, params);
  ret[NM_KOMENDANTOV_KONONENKO_1996_M_B] =
      nm_komendantov_kononenko_1996_m_b(vars, params);
  ret[NM_KOMENDANTOV_KONONENKO_1996_H_B] =
      nm_komendantov_kononenko_1996_h_b(vars, params);
  ret[NM_KOMENDANTOV_KONONENKO_1996_M] =
      nm_komendantov_kononenko_1996_m(vars, params);
  ret[NM_KOMENDANTOV_KONONENKO_1996_H] =
      nm_komendantov_kononenko_1996_h(vars, params);
  ret[NM_KOMENDANTOV_KONONENKO_1996_N] =
      nm_komendantov_kononenko_1996_n(vars, params);
  ret[NM_KOMENDANTOV_KONONENKO_1996_M_CA] =
      nm_komendantov_kononenko_1996_m_ca(vars, params);
  ret[NM_KOMENDANTOV_KONONENKO_1996_CA] =
      nm_komendantov_kononenko_1996_Ca(vars, params);
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
