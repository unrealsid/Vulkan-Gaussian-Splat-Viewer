//
// Created by Sid on 1/3/2026.
//

#include "platform/input/UIActionManager.h"

namespace ui
{
    void UIActionManager::register_action(UIAction action, const UIActionCallback& callback)
    {
        action_callbacks[action].push_back(callback);
    }

    void UIActionManager::register_bool_action(UIAction action, const UIBoolCallback& callback)
    {
        bool_callbacks[action].push_back(callback);
    }

    void UIActionManager::register_int_action(UIAction action, const UIIntCallback& callback)
    {
        int_callbacks[action].push_back(callback);
    }

    void UIActionManager::register_float_action(UIAction action, const UIFloatCallback& callback)
    {
        float_callbacks[action].push_back(callback);
    }

    void UIActionManager::register_string_action(UIAction action, const UIStringCallback& callback)
    {
        string_callbacks[action].push_back(callback);
    }

    // ===== Unregister actions =====

    void UIActionManager::unregister_action(UIAction action)
    {
        action_callbacks.erase(action);
    }

    void UIActionManager::unregister_bool_action(UIAction action)
    {
        bool_callbacks.erase(action);
    }

    void UIActionManager::unregister_int_action(UIAction action)
    {
        int_callbacks.erase(action);
    }

    void UIActionManager::unregister_float_action(UIAction action)
    {
        float_callbacks.erase(action);
    }

    void UIActionManager::unregister_string_action(UIAction action)
    {
        string_callbacks.erase(action);
    }

    // ===== Trigger actions =====

    void UIActionManager::trigger_action(UIAction action)
    {
        auto it = action_callbacks.find(action);
        if (it != action_callbacks.end())
        {
            for (auto& callback : it->second)
            {
                callback();
            }
        }
    }

    void UIActionManager::trigger_bool_action(UIAction action, bool value)
    {
        auto it = bool_callbacks.find(action);
        if (it != bool_callbacks.end())
        {
            for (auto& callback : it->second)
            {
                callback(value);
            }
        }
    }

    void UIActionManager::trigger_int_action(UIAction action, int value)
    {
        auto it = int_callbacks.find(action);
        if (it != int_callbacks.end())
        {
            for (auto& callback : it->second)
            {
                callback(value);
            }
        }
    }

    void UIActionManager::trigger_float_action(UIAction action, float value)
    {
        auto it = float_callbacks.find(action);
        if (it != float_callbacks.end())
        {
            for (auto& callback : it->second)
            {
                callback(value);
            }
        }
    }

    void UIActionManager::trigger_string_action(UIAction action, const std::string& value)
    {
        auto it = string_callbacks.find(action);
        if (it != string_callbacks.end())
        {
            for (auto& callback : it->second)
            {
                callback(value);
            }
        }
    }

    bool UIActionManager::has_action(UIAction action) const
    {
        return action_callbacks.find(action) != action_callbacks.end();
    }

    bool UIActionManager::has_bool_action(UIAction action) const
    {
        return bool_callbacks.contains(action);
    }

    bool UIActionManager::has_int_action(UIAction action) const
    {
        return int_callbacks.contains(action);
    }

    bool UIActionManager::has_float_action(UIAction action) const
    {
        return float_callbacks.contains(action);
    }

    bool UIActionManager::has_string_action(UIAction action) const
    {
        return string_callbacks.contains(action);
    }

    // ===== Clear all actions =====

    void UIActionManager::clear_all_actions()
    {
        action_callbacks.clear();
        bool_callbacks.clear();
        int_callbacks.clear();
        float_callbacks.clear();
        string_callbacks.clear();
    }
} // ui