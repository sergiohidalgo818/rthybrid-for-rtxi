#include "rthybrid_burst_analysis.hpp"
#include <QTimer>
#include <rtxi/rt.hpp>
#include <rtxi/rtos.hpp>

rthybrid_burst_analysis::Component
    *rthybrid_burst_analysis::Component::instance = nullptr;

rthybrid_burst_analysis::Plugin::Plugin(Event::Manager *ev_manager)
    : Widgets::Plugin(ev_manager,
                      std::string(rthybrid_burst_analysis::MODULE_NAME)) {}

rthybrid_burst_analysis::Panel::Panel(QMainWindow *main_window,
                                      Event::Manager *ev_manager)
    : Widgets::Panel(std::string(rthybrid_burst_analysis::MODULE_NAME),
                     main_window, ev_manager) {
  setWhatsThis("<p><b>RTHybridBurstAnalysis:</b><br>RTHybrid module for RTXI "
               "to get the minimum and maximum membrane potential values of a "
               "neuron and its bursts duration.</p>");
  createGUI(rthybrid_burst_analysis::get_default_vars(),
            {}); // this is required to create the GUI
  auto edits = findChildren<QLineEdit *>();
  min_edit = edits[BURST_ANALYSIS_MIN];
  max_edit = edits[BURST_ANALYSIS_MAX];
  dur_edit = edits[BURST_ANALYSIS_DUR];
  upp_thresh_edit = edits[BURST_ANALYSIS_UPP_THRESH];
  down_thresh_edit = edits[BURST_ANALYSIS_DOWN_THRESH];
  ampl_edit = edits[BURST_ANALYSIS_AMPL];
  pc_edit = edits[BURST_ANALYSIS_PC];
  bc_edit = edits[BURST_ANALYSIS_BC];
  isb_edit = edits[BURST_ANALYSIS_ISB];

  if (min_edit)
    min_edit->setReadOnly(true);
  if (max_edit)
    max_edit->setReadOnly(true);
  if (dur_edit)
    dur_edit->setReadOnly(true);
  if (upp_thresh_edit)
    upp_thresh_edit->setReadOnly(true);
  if (down_thresh_edit)
    down_thresh_edit->setReadOnly(true);
  if (ampl_edit)
    ampl_edit->setReadOnly(true);
  if (isb_edit)
    isb_edit->setReadOnly(true);
  if (pc_edit)
    pc_edit->setReadOnly(true);
  if (bc_edit)
    bc_edit->setReadOnly(true);

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this,
          &rthybrid_burst_analysis::Panel::refresh);
  timer->start(500); // refresh every 500 ms

  this->parentWidget()->adjustSize();
}

rthybrid_burst_analysis::Component::Component(Widgets::Plugin *hplugin)
    : Widgets::Component(hplugin,
                         std::string(rthybrid_burst_analysis::MODULE_NAME),
                         rthybrid_burst_analysis::get_default_channels(),
                         rthybrid_burst_analysis::get_default_vars()) {
  Component::instance = this;
}
void rthybrid_burst_analysis::Panel::refresh() {
  auto *comp = rthybrid_burst_analysis::Component::instance;
  if (comp && min_edit) {
    min_edit->setText(QString::number(comp->min));
  }
  if (comp && max_edit) {
    max_edit->setText(QString::number(comp->max));
  }
  if (comp && dur_edit) {
    dur_edit->setText(QString::number(comp->sec_per_burst));
  }
  if (comp && upp_thresh_edit) {
    upp_thresh_edit->setText(QString::number(comp->thresh_up));
  }
  if (comp && down_thresh_edit) {
    down_thresh_edit->setText(QString::number(comp->thresh_down));
  }
  if (comp && ampl_edit) {
    ampl_edit->setText(QString::number(comp->range));
  }
  if (comp && isb_edit) {
    isb_edit->setText(QString::number(comp->is_burst));
  }
  if (comp && pc_edit) {
    pc_edit->setText(QString::number(comp->pts_counter));
  }
  if (comp && bc_edit) {
    bc_edit->setText(QString::number(comp->burst_counter));
  }
}

void rthybrid_burst_analysis::Component::initParameters(void) {

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

void rthybrid_burst_analysis::Component::execute() {
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
    break;
  case RT::State::INIT:
    period = RT::OS::getPeriod() * 1e-6;
    freq = 1.0 / (period * 1e-3);
    observation_time = getValue<double>(BURST_ANALYSIS_OBST);

    initParameters();
    setState(RT::State::PAUSE);
    break;
  case RT::State::MODIFY:
    observation_time = getValue<double>(BURST_ANALYSIS_OBST);

    setState(RT::State::PAUSE);
    break;
  case RT::State::PERIOD:
    period = RT::OS::getPeriod() * 1e-6;
    freq = 1.0 / (period * 1e-3);

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

///////// DO NOT MODIFY BELOW //////////
// The exception is if your plugin is not going to need real-time
// functionality. For this case just replace the craeteRTXIComponent return
// type to nullptr. RTXI will automatically handle that case and won't attach
// a component to the real time thread for your plugin.

std::unique_ptr<Widgets::Plugin> createRTXIPlugin(Event::Manager *ev_manager) {
  return std::make_unique<rthybrid_burst_analysis::Plugin>(ev_manager);
}

Widgets::Panel *createRTXIPanel(QMainWindow *main_window,
                                Event::Manager *ev_manager) {
  return new rthybrid_burst_analysis::Panel(main_window, ev_manager);
}

std::unique_ptr<Widgets::Component>
createRTXIComponent(Widgets::Plugin *host_plugin) {
  return std::make_unique<rthybrid_burst_analysis::Component>(host_plugin);
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
