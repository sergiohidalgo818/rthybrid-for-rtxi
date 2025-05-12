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
#include "rthybrid_hindmarsh_rose_1984_neuron.hpp"
#include <QTimer>
#include <cmath>
#include <cstdio>
#include <qchar.h>
#include <qlineedit.h>
#include <rtxi/fifo.hpp>
#include <rtxi/rt.hpp>
#include <rtxi/rtos.hpp>

RTHybridHindmarshRose1984Neuron::Component::Component(Widgets::Plugin *hplugin)
    : Widgets::Component(
          hplugin, std::string(RTHybridHindmarshRose1984Neuron::MODULE_NAME),
          RTHybridHindmarshRose1984Neuron::get_default_channels(),
          RTHybridHindmarshRose1984Neuron::get_default_vars()) {
  if (RT::OS::getFifo(
          this->fifo,
          10 * sizeof(RTHybridHindmarshRose1984Neuron::neuron_state_t)) < 0) {
    ERROR_MSG("PerformanceMeasurement::Component::Component : Unable to craate "
              "component fifo");
    this->setState(RT::State::PAUSE);
  }
}

RTHybridHindmarshRose1984Neuron::Plugin::Plugin(Event::Manager *ev_manager)
    : Widgets::Plugin(
          ev_manager,
          std::string(RTHybridHindmarshRose1984Neuron::MODULE_NAME)) {
  auto component =
      std::make_unique<RTHybridHindmarshRose1984Neuron::Component>(this);
  this->component_fifo = component->get_fifo_ptr();
  this->attachComponent(std::move(component));
}

RTHybridHindmarshRose1984Neuron::Panel::Panel(QMainWindow *main_window,
                                              Event::Manager *ev_manager)
    : Widgets::Panel(std::string(RTHybridHindmarshRose1984Neuron::MODULE_NAME),
                     main_window, ev_manager)
// ,
// v_edit(new QLineEdit(this)), sp_edit(new QLineEdit(this)),
// dt_edit(new QLineEdit(this)), syn_edit(new QLineEdit(this))
{
  setWhatsThis("<p><b>RTHybrid Hindmarsh-Rose (1984) neuron model V2</b></p>");
  createGUI(RTHybridHindmarshRose1984Neuron::get_default_vars(),
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

  v_edit = edits[V_NM_HINDMARSH_ROSE_1984_V];
  sp_edit = edits[V_NM_HINDMARSH_ROSE_1984_SP];
  dt_edit = edits[V_NM_HINDMARSH_ROSE_1984_DT];
  syn_edit = edits[V_NM_HINDMARSH_ROSE_1984_SYN];

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

  this->parentWidget()->adjustSize();
  auto *timer = new QTimer(this);
  timer->setInterval(1000);
  QObject::connect(timer, &QTimer::timeout, this,
                   &RTHybridHindmarshRose1984Neuron::Panel::refresh);
  timer->start();
}

RTHybridHindmarshRose1984Neuron::neuron_state_t
RTHybridHindmarshRose1984Neuron::Plugin::get_neuron_state() {

  RTHybridHindmarshRose1984Neuron::neuron_state_t stat;
  while (this->component_fifo->read(
             &stat, sizeof(RTHybridHindmarshRose1984Neuron::neuron_state_t)) >
         0) {
  };
  return stat;
}

void RTHybridHindmarshRose1984Neuron::Panel::refresh() {
  auto *hostplugin = dynamic_cast<RTHybridHindmarshRose1984Neuron::Plugin *>(
      this->getHostPlugin());
  const RTHybridHindmarshRose1984Neuron::neuron_state_t n_state =
      hostplugin->get_neuron_state();
  v_edit->setText(QString::number(n_state.v));
  sp_edit->setText(QString::number(n_state.s_points));
  dt_edit->setText(QString::number(n_state.dt_points));
  syn_edit->setText(QString::number(n_state.syn_points));
}

void RTHybridHindmarshRose1984Neuron::Component::init_parameters(void) {
  burst_duration_value = getValue<double>(V_NM_HINDMARSH_ROSE_1984_BD);
  burst_duration = burst_duration_value;
  freq = 1.0 / (period * 1e-3);
  s_points = (int)(set_pts_burst(burst_duration) / (burst_duration * freq));
  if (s_points == 0)
    s_points = 1;

  vars_model[NM_HINDMARSH_ROSE_1984_V] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_V0);
  vars_model[NM_HINDMARSH_ROSE_1984_Y] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_Y0);
  vars_model[NM_HINDMARSH_ROSE_1984_Z] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_Z0);

  params_model[NM_HINDMARSH_ROSE_1984_I] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_I);

  params_model[NM_HINDMARSH_ROSE_1984_R] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_R);
  params_model[NM_HINDMARSH_ROSE_1984_S] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_S);
  params_model[NM_HINDMARSH_ROSE_1984_XR] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_XR);

  params_model[NM_HINDMARSH_ROSE_1984_A] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_A);
  params_model[NM_HINDMARSH_ROSE_1984_B] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_B);
  params_model[NM_HINDMARSH_ROSE_1984_C] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_C);
  params_model[NM_HINDMARSH_ROSE_1984_D] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_D);

  params_model[NM_HINDMARSH_ROSE_1984_DT] =
      getValue<double>(V_NM_HINDMARSH_ROSE_1984_DT);
  params_model[NM_HINDMARSH_ROSE_1984_SYN] = 0.0;
}

void RTHybridHindmarshRose1984Neuron::Component::execute() {
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
      runge_kutta_65(nm_hindmarsh_rose_1984_f, 3,
                     params_model[NM_HINDMARSH_ROSE_1984_DT], vars_model,
                     params_model, readinput(0));
    }

    writeoutput(0, vars_model[NM_HINDMARSH_ROSE_1984_V] / 1000.0);
    writeoutput(1, vars_model[NM_HINDMARSH_ROSE_1984_V]);

    neuron_state.v = vars_model[NM_HINDMARSH_ROSE_1984_V];
    neuron_state.dt_points = params_model[NM_HINDMARSH_ROSE_1984_DT];
    neuron_state.syn_points = params_model[NM_HINDMARSH_ROSE_1984_SYN];
    neuron_state.s_points = s_points;

    this->fifo->writeRT(
        &this->neuron_state,
        sizeof(RTHybridHindmarshRose1984Neuron::neuron_state_t));

    break;
  case RT::State::INIT:
    period = RT::OS::getPeriod() * 1e-6; // ms
    freq = 1.0 / (period * 1e-3);

    this->neuron_state = {0.0, 0.0, 0.0, 0.0};

    this->init_parameters();
    setState(RT::State::EXEC);

    break;
  case RT::State::MODIFY:
    period = RT::OS::getPeriod() * 1e-6; // ms
    freq = 1.0 / (period * 1e-3);

    this->init_parameters();
    setState(RT::State::PAUSE);
    break;
  case RT::State::PERIOD:
    period = RT::OS::getPeriod() * 1e-6; // ms

    freq = 1.0 / (period * 1e-3);
    s_points = (int)(set_pts_burst(burst_duration) / (burst_duration * freq));
    if (s_points == 0)
      s_points = 1;

    setState(RT::State::PAUSE);

    break;
  case RT::State::PAUSE:

    writeoutput(0, 0);
    writeoutput(1, 0);

    break;
  case RT::State::UNPAUSE:
    freq = 1.0 / (period * 1e-3);
    s_points = (int)(set_pts_burst(burst_duration) / (burst_duration * freq));
    if (s_points == 0)
      s_points = 1;
    setState(RT::State::EXEC);

    break;
  default:
    break;
  }
}

/**
 * @brief Hindmarsh_Rose_1986 neuron model differential equations.
 * @param[in] vars Neuron model variables
 * @param[out] ret Return values array
 * @param[in] params Neuron models parameters
 * @param[in] syn Synapse input current value
 */

void RTHybridHindmarshRose1984Neuron::Component::nm_hindmarsh_rose_1984_f(
    double *vars, double *ret, double *params, double syn) {
  params[NM_HINDMARSH_ROSE_1984_SYN] = syn;

  ret[NM_HINDMARSH_ROSE_1984_V] = nm_hindmarsh_rose_1984_v(vars, params);
  ret[NM_HINDMARSH_ROSE_1984_Y] = nm_hindmarsh_rose_1984_y(vars, params);
  ret[NM_HINDMARSH_ROSE_1984_Z] = nm_hindmarsh_rose_1984_z(vars, params);
}

double RTHybridHindmarshRose1984Neuron::Component::nm_hindmarsh_rose_1984_v(
    double *vars, double *params) {
  return vars[NM_HINDMARSH_ROSE_1984_Y] +
         params[NM_HINDMARSH_ROSE_1984_B] *
             (vars[NM_HINDMARSH_ROSE_1984_V] * vars[NM_HINDMARSH_ROSE_1984_V]) -
         params[NM_HINDMARSH_ROSE_1984_A] *
             (vars[NM_HINDMARSH_ROSE_1984_V] * vars[NM_HINDMARSH_ROSE_1984_V] *
              vars[NM_HINDMARSH_ROSE_1984_V]) -
         vars[NM_HINDMARSH_ROSE_1984_Z] + params[NM_HINDMARSH_ROSE_1984_I] -
         params[NM_HINDMARSH_ROSE_1984_SYN];
}

double RTHybridHindmarshRose1984Neuron::Component::nm_hindmarsh_rose_1984_y(
    double *vars, double *params) {
  return params[NM_HINDMARSH_ROSE_1984_C] -
         params[NM_HINDMARSH_ROSE_1984_D] * vars[NM_HINDMARSH_ROSE_1984_V] *
             vars[NM_HINDMARSH_ROSE_1984_V] -
         vars[NM_HINDMARSH_ROSE_1984_Y];
}

double RTHybridHindmarshRose1984Neuron::Component::nm_hindmarsh_rose_1984_z(
    double *vars, double *params) {
  return params[NM_HINDMARSH_ROSE_1984_R] *
         (params[NM_HINDMARSH_ROSE_1984_S] *
              (vars[NM_HINDMARSH_ROSE_1984_V] -
               params[NM_HINDMARSH_ROSE_1984_XR]) -
          vars[NM_HINDMARSH_ROSE_1984_Z]);
}

double RTHybridHindmarshRose1984Neuron::Component::set_pts_burst(
    double sec_per_burst) {
  int length = 0;
  int method = 3;
  double pts_match = sec_per_burst * freq;
  double pts_burst, dt;

  length = 144;
  double dts[] = {
      0.000500, 0.000600, 0.000700, 0.000800, 0.000900, 0.001000, 0.001100,
      0.001200, 0.001300, 0.001400, 0.001500, 0.001600, 0.001800, 0.002000,
      0.002200, 0.002500, 0.002800, 0.002900, 0.003000, 0.003100, 0.003200,
      0.003300, 0.003400, 0.003500, 0.003600, 0.003700, 0.003800, 0.003900,
      0.004000, 0.004100, 0.004200, 0.004300, 0.004400, 0.004500, 0.004600,
      0.004700, 0.004800, 0.004900, 0.005000, 0.005100, 0.005200, 0.005400,
      0.005600, 0.005800, 0.006000, 0.006200, 0.006400, 0.006600, 0.006800,
      0.007000, 0.007200, 0.007400, 0.007700, 0.008000, 0.008300, 0.008600,
      0.008900, 0.009200, 0.009600, 0.010000, 0.010400, 0.010900, 0.011400,
      0.011900, 0.012500, 0.013100, 0.013800, 0.014600, 0.015400, 0.016300,
      0.017300, 0.018500, 0.019900, 0.021500, 0.023300, 0.025500, 0.028100,
      0.028400, 0.028700, 0.029000, 0.029400, 0.029800, 0.030200, 0.030600,
      0.031000, 0.031400, 0.031800, 0.032200, 0.032600, 0.033000, 0.033400,
      0.033900, 0.034400, 0.034900, 0.035400, 0.035900, 0.036400, 0.036900,
      0.037400, 0.038000, 0.038600, 0.039200, 0.039800, 0.040400, 0.041000,
      0.041700, 0.042400, 0.043100, 0.043800, 0.044500, 0.045300, 0.046100,
      0.046900, 0.047700, 0.048600, 0.049500, 0.050400, 0.051400, 0.052400,
      0.053400, 0.054500, 0.055600, 0.056800, 0.058000, 0.059300, 0.060600,
      0.062000, 0.063400, 0.064900, 0.066500, 0.068200, 0.069900, 0.071700,
      0.073600, 0.075600, 0.077700, 0.079900, 0.082300, 0.084800, 0.087500,
      0.090300, 0.093300, 0.096500, 0.100000};
  double pts[] = {
      577638.000000, 481366.000000, 412599.000000, 357615.500000, 317880.000000,
      286092.500000, 259143.333333, 237548.000000, 218869.500000, 203236.000000,
      189687.000000, 177634.000000, 157897.000000, 142001.833333, 129024.142857,
      113496.125000, 101304.555556, 97811.222222,  94527.400000,  91478.200000,
      88619.400000,  85916.636364,  83389.636364,  81007.090909,  78743.583333,
      76615.416667,  74599.250000,  72676.000000,  70859.076923,  69130.846154,
      67476.642857,  65907.357143,  64402.666667,  62971.466667,  61602.533333,
      60286.187500,  59030.250000,  57825.562500,  56664.411765,  55553.294118,
      54485.000000,  52463.222222,  50586.263158,  48841.842105,  47211.050000,
      45685.666667,  44255.818182,  42914.772727,  41650.739130,  40459.083333,
      39335.208333,  38270.680000,  36778.346154,  35398.000000,  34117.571429,
      32926.517241,  31815.833333,  30777.612903,  29493.939394,  28313.588235,
      27223.638889,  25974.405405,  24834.410256,  23790.268293,  22647.767442,
      21609.977778,  20513.166667,  19388.627451,  18381.132075,  17365.719298,
      16361.600000,  15299.937500,  14223.202899,  13164.400000,  12147.123457,
      11098.876404,  10071.693878,  9965.282828,   9861.100000,   9759.059406,
      9626.242718,   9497.009615,   9371.179245,   9248.672897,   9129.293578,
      9012.981818,   8899.594595,   8789.000000,   8681.149123,   8575.896552,
      8473.170940,   8348.176471,   8226.809917,   8108.934426,   7994.379032,
      7883.015873,   7774.710938,   7669.348837,   7566.801527,   7447.308271,
      7331.525926,   7219.291971,   7110.435714,   7004.816901,   6902.298611,
      6786.417808,   6674.355705,   6565.940397,   6460.987013,   6359.339744,
      6247.012579,   6138.592593,   6033.866667,   5932.660714,   5822.777778,
      5716.896552,   5614.796610,   5505.541436,   5400.467391,   5299.324468,
      5192.348958,   5089.615385,   4982.075000,   4878.985294,   4772.014354,
      4669.633803,   4564.178899,   4463.381166,   4360.214912,   4255.294872,
      4149.216667,   4048.296748,   3946.654762,   3844.764479,   3743.041353,
      3641.872263,   3541.583630,   3438.300000,   3336.926421,   3233.951299,
      3133.666667,   3032.899696,   2932.320588,   2829.684659};
  select_dt_neuron_model(dts, pts, length, pts_match,
                         &(params_model[NM_HINDMARSH_ROSE_1984_DT]),
                         &(pts_burst));

  return pts_burst;
}

void RTHybridHindmarshRose1984Neuron::Component::select_dt_neuron_model(
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

void RTHybridHindmarshRose1984Neuron::Component::runge_kutta_65(
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

///////// DO NOT MODIFY BELOW //////////
// The exception is if your plugin is not going to need real-time
// functionality. For this case just replace the craeteRTXIComponent return
// type to nullptr. RTXI will automatically handle that case and won't attach
// a component to the real time thread for your plugin.

std::unique_ptr<Widgets::Plugin> createRTXIPlugin(Event::Manager *ev_manager) {
  return std::make_unique<RTHybridHindmarshRose1984Neuron::Plugin>(ev_manager);
}

Widgets::Panel *createRTXIPanel(QMainWindow *main_window,
                                Event::Manager *ev_manager) {
  return new RTHybridHindmarshRose1984Neuron::Panel(main_window, ev_manager);
}

std::unique_ptr<Widgets::Component>
createRTXIComponent(Widgets::Plugin *host_plugin) {
  return std::make_unique<RTHybridHindmarshRose1984Neuron::Component>(
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
