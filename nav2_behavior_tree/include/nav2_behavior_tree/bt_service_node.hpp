// Copyright (c) 2019 Samsung Research America
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

#ifndef NAV2_BEHAVIOR_TREE__BT_SERVICE_NODE_HPP_
#define NAV2_BEHAVIOR_TREE__BT_SERVICE_NODE_HPP_

#include <string>
#include <memory>

#include "behaviortree_cpp/action_node.h"
#include "nav2_util/node_utils.hpp"
#include "rclcpp/rclcpp.hpp"

namespace nav2_behavior_tree
{

template<class ServiceT>
class BtServiceNode : public BT::CoroActionNode
{
public:
  BtServiceNode(
    const std::string & service_node_name,
    const BT::NodeConfiguration & conf)
  : BT::CoroActionNode(service_node_name, conf), service_node_name_(service_node_name)
  {
    node_ = config().blackboard->get<rclcpp::Node::SharedPtr>("node");

    // Get the required items from the blackboard
    node_loop_timeout_ =
      config().blackboard->get<std::chrono::milliseconds>("node_loop_timeout");

    // Now that we have node_ to use, create the service client for this BT service
    getInput("service_name", service_name_);
    service_client_ = node_->create_client<ServiceT>(service_name_);

    // Make sure the server is actually there before continuing
    RCLCPP_INFO(node_->get_logger(), "Waiting for \"%s\" service",
      service_name_.c_str());
    service_client_->wait_for_service();

    RCLCPP_INFO(node_->get_logger(), "\"%s\" BtServiceNode initialized",
      service_node_name_.c_str());
  }

  BtServiceNode() = delete;

  virtual ~BtServiceNode()
  {
  }

  // Any BT node that accepts parameters must provide a requiredNodeParameters method
  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("service_name", "please_set_service_name_in_BT_Node")
    };
  }

  // The main override required by a BT service
  BT::NodeStatus tick() override
  {
    on_tick();
    auto future_result = service_client_->async_send_request(request_);

    rclcpp::executor::FutureReturnCode rc;
    rc = rclcpp::spin_until_future_complete(node_,
        future_result, node_loop_timeout_);
    if (rc != rclcpp::executor::FutureReturnCode::SUCCESS) {
      return BT::NodeStatus::FAILURE;
    } else {
      return BT::NodeStatus::SUCCESS;
    }
  }

  // Fill in service request with information if necessary
  virtual void on_tick()
  {
    request_ = std::make_shared<typename ServiceT::Request>();
  }

protected:
  std::string service_name_, service_node_name_;
  typename std::shared_ptr<rclcpp::Client<ServiceT>> service_client_;
  std::shared_ptr<typename ServiceT::Request> request_;

  // The node that will be used for any ROS operations
  rclcpp::Node::SharedPtr node_;

  // The timeout value while to use in the tick loop while waiting for
  // a result from the server
  std::chrono::milliseconds node_loop_timeout_;
};

}  // namespace nav2_behavior_tree

#endif  // NAV2_BEHAVIOR_TREE__BT_SERVICE_NODE_HPP_
