// Copyright (c) 2018 Samsung Research America
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NAV2_BEHAVIOR_TREE__WAIT_ACTION_HPP_
#define NAV2_BEHAVIOR_TREE__WAIT_ACTION_HPP_

#include <string>
#include <memory>
#include <cmath>

#include "nav2_behavior_tree/bt_action_node.hpp"
#include "nav2_msgs/action/wait.hpp"

namespace nav2_behavior_tree
{

class WaitAction : public BtActionNode<nav2_msgs::action::Wait>
{
public:
  WaitAction(
    const std::string & action_name,
    const BT::NodeConfiguration & conf)
  : BtActionNode<nav2_msgs::action::Wait>(action_name, conf)
  {
    int duration;
    getInput("wait_duration", duration);
    if (duration <= 0) {
      RCLCPP_WARN(node_->get_logger(), "Wait duration is negative or zero "
        "(%i). Setting to positive.", duration);
      duration *= -1;
    }

    goal_.time.sec = duration;
  }

  // Any BT node that accepts parameters must provide a requiredNodeParameters method
  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<int>("wait_duration", 1, "Wait time")
    };
  }
};

}  // namespace nav2_behavior_tree

#endif  // NAV2_BEHAVIOR_TREE__WAIT_ACTION_HPP_
