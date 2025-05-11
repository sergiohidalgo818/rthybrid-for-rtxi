
#include <rtxi/widgets.hpp>

namespace rthybrid_amplitude_scale_offset {

constexpr std::string_view MODULE_NAME = "rthybrid_amplitude_scale_offset";

enum PARAMETER : Widgets::Variable::Id {
  // set parameter ids here

  V_S12 = 0,
  V_S21 = 1,
  V_O12 = 2,
  V_O21 = 3
};

inline std::vector<Widgets::Variable::Info> get_default_vars() {
  return {{V_S12, "Scale 1-2", "Scale from neuron 1 to neuron 2",
           Widgets::Variable::DOUBLE_PARAMETER, 1.0},
          {V_S21, "Scale 2-1", "Scale from neuron 2 to neuron 1",
           Widgets::Variable::DOUBLE_PARAMETER, 1.0},
          {V_O12, "Offset 1-2", "Offset from neuron 1 to neuron 2",
           Widgets::Variable::DOUBLE_PARAMETER, 0.0},
          {V_O21, "Offset 2-1", "Offset from neuron 2 to neuron 1",
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

class Panel : public Widgets::Panel {
  Q_OBJECT
public:
  Panel(QMainWindow *main_window, Event::Manager *ev_manager);

  // Any functions and data related to the GUI are to be placed here
  void refresh();
  QLineEdit *s12_edit = nullptr;
  QLineEdit *s21_edit = nullptr;
  QLineEdit *o12_edit = nullptr;
  QLineEdit *o21_edit = nullptr;
};

class Component : public Widgets::Component {
public:
  explicit Component(Widgets::Plugin *hplugin);
  void execute() override;

  // Additional functionality needed for RealTime computation is to be placed
  // here
  static Component *instance;
  double s12, s21, o12, o21;

private:
  double period;

  void initParameters();
};

class Plugin : public Widgets::Plugin {
public:
  explicit Plugin(Event::Manager *ev_manager);
};

} // namespace rthybrid_amplitude_scale_offset
