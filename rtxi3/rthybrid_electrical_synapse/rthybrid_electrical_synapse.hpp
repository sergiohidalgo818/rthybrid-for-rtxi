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

namespace RTHybridElectricalSynapse {

constexpr std::string_view MODULE_NAME = "RTHybrid Electrical Synapse";

enum PARAMETER : Widgets::Variable::Id {
  // set parameter ids here
  FIRST_PARAMETER = 0,
  SECOND_PARAMETER,
  THIRD_PARAMETER
};

inline std::vector<Widgets::Variable::Info> get_default_vars() {
  return {{PARAMETER::FIRST_PARAMETER, "First Parameter Name",
           "First Parameter Description", Widgets::Variable::INT_PARAMETER,
           int64_t{0}},
          {PARAMETER::SECOND_PARAMETER, "Second Parameter Name",
           "Second Parameter Description", Widgets::Variable::DOUBLE_PARAMETER,
           1.0},
          {PARAMETER::THIRD_PARAMETER, "Third Parameter Name",
           "Third Parameter Description", Widgets::Variable::STATE,
           uint64_t{1}}};
}

inline std::vector<IO::channel_t> get_default_channels() {
  return {{"First Channel Output Name", "First Channel Output Description",
           IO::OUTPUT},
          {"First Channel Input Name", "First Channel Input Description",
           IO::INPUT}};
}

class Panel : public Widgets::Panel {
  Q_OBJECT
public:
  Panel(QMainWindow *main_window, Event::Manager *ev_manager);

  // Any functions and data related to the GUI are to be placed here
};

class Component : public Widgets::Component {
public:
  explicit Component(Widgets::Plugin *hplugin);
  void execute() override;

  // Additional functionality needed for RealTime computation is to be placed
  // here
};

class Plugin : public Widgets::Plugin {
public:
  explicit Plugin(Event::Manager *ev_manager);
};

} // namespace RTHybridElectricalSynapse
