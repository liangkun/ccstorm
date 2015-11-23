// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef CCSTORM_TOPOLOGY_CONTEXT_H
#define CCSTORM_TOPOLOGY_CONTEXT_H

#include <map>
#include <string>
#include <utility>
#include "rapidjson/document.h"

namespace storm {

// Context of a component(Spout or Bolt) in current topology.
class TopologyContext {
public:
    TopologyContext(
            int task_id,
            std::map<int, std::string> *task_2_component,
            std::string *pid_dir,
            rapidjson::Value *config
    ): _task_id(task_id),
       _task_2_component(std::move(*task_2_component)),
       _pid_dir(std::move(*pid_dir)) {

        _config = *config;
    };

    int task_id() const { return _task_id; }
    const std::string &pid_dir() const { return _pid_dir; }
    const std::map<int, std::string> task_2_component() { return _task_2_component; }
    const std::string &component() const { return _task_2_component.at(_task_id); }
    const rapidjson::Value &config() { return _config; }

private:
    const int _task_id;
    const std::map<int, std::string> _task_2_component;
    const std::string _pid_dir;
    rapidjson::Value _config;
};

}  // namespace storm

#endif // CCSTORM_TOPOLOGY_CONTEXT_H
