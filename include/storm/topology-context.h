// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef CCSTORM_TOPOLOGY_CONTEXT_H
#define CCSTORM_TOPOLOGY_CONTEXT_H

#include <string>
#include <map>

namespace storm {

class TopologyContext {
public:
    TopologyContext(
            int task_id,
            const std::map<int, std::string> &task_2_component,
            const std::string &pid_dir
    ): _task_id(task_id),
       _task_2_component(task_2_component),
       _pid_dir(pid_dir) {};

    int task_id() const { return _task_id; }
    const std::string &pid_dir() const { return _pid_dir; }
    const std::string &component() const { return _task_2_component.at(_task_id); }

private:
    const int _task_id;
    const std::map<int, std::string> _task_2_component;
    const std::string _pid_dir;
};

}  // namespace storm

#endif // CCSTORM_TOPOLOGY_CONTEXT_H
