// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_TOPOLOGY_CONTEXT_H
#define STORM_TOPOLOGY_CONTEXT_H

#include <map>
#include <string>
#include <utility>
#include "storm/values.h"

namespace storm {

// Context of a component(Spout or Bolt) in the current topology.
class TopologyContext {
public:
    TopologyContext(
            int task_id,
            std::map<int, std::string> &&task_2_component,
            std::string &&pid_dir,
            Values &&config
    ): _task_id(std::move(task_id)),
       _task_2_component(std::move(task_2_component)),
       _pid_dir(std::move(pid_dir)),
       _config(std::move(config)) {}

    int task_id() const { return _task_id; }
    const std::string &pid_dir() const { return _pid_dir; }
    const std::map<int, std::string> task_2_component() { return _task_2_component; }
    const std::string &component() const { return _task_2_component.at(_task_id); }
    const Values &config() const { return _config; }

private:
    const int _task_id;
    const std::map<int, std::string> _task_2_component;
    const std::string _pid_dir;
    Values _config;
};

}  // namespace storm

#endif // STORM_TOPOLOGY_CONTEXT_H
