// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file refactor_action_registry.hpp
/// Contains definition of the refactor_action_registry class.

#pragma once

#include "refactor_action.hpp"
#include <cassert>
#include <map>
#include <memory>
#include <sstream>


/// Registry of refactor actions
class refactor_action_registry {
public:
    /// Registers action in registry
    void reg_action(std::unique_ptr<refactor_action> && act) {
        auto [it, inserted] = actions_.emplace(act->name(), std::move(act));
        assert(inserted && "refactor action with same name already registered");
    }

    /// Searches for action with specified name. Returns reference to action
    const refactor_action & find_action(const std::string & nm) const {
        auto it = actions_.find(nm);
        if (it == actions_.end()) {
            std::ostringstream msg;
            msg << "can't find refactor action with name: " << nm;
            throw std::runtime_error(msg.str());
        }

        return *it->second;
    }

    /// Returns range of const pointers to all registered actions
    auto actions() const {
        auto fn = [](auto && pair) {
            return const_cast<const refactor_action*>(pair.second.get());
        };

        return actions_ | std::ranges::views::transform(fn);
    }

private:
    std::map<std::string, std::unique_ptr<refactor_action>> actions_;
};
