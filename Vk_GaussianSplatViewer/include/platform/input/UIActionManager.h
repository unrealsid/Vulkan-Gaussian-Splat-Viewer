#pragma once

#include "enums/inputs/UIAction.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

namespace ui
{
    // Callback types for different UI interactions
    using UIActionCallback = std::function<void()>;
    using UIBoolCallback = std::function<void(bool)>;
    using UIIntCallback = std::function<void(int)>;
    using UIFloatCallback = std::function<void(float)>;
    using UIStringCallback = std::function<void(const std::string&)>;

    class UIActionManager
    {
         public:
        UIActionManager() = default;
        ~UIActionManager() = default;

        // ===== Action callback registration =====

        // Register a simple button/action callback
        void register_action(UIAction action, const UIActionCallback& callback);

        // Register a checkbox/toggle callback (receives bool state)
        void register_bool_action(UIAction action, const UIBoolCallback& callback);

        // Register a slider/spinner callback (receives int value)
        void register_int_action(UIAction action, const UIIntCallback& callback);

        // Register a slider callback (receives float value)
        void register_float_action(UIAction action, const UIFloatCallback& callback);

        // Register a text input callback (receives string value)
        void register_string_action(UIAction action, const UIStringCallback& callback);

        // ===== Unregister actions =====

        void unregister_action(UIAction action);
        void unregister_bool_action(UIAction action);
        void unregister_int_action(UIAction action);
        void unregister_float_action(UIAction action);
        void unregister_string_action(UIAction action);

        // Trigger a simple action (for buttons, menu items, etc.)
        void trigger_action(UIAction action);

        // Trigger a bool action (for checkboxes, toggles)
        void trigger_bool_action(UIAction action, bool value);

        // Trigger an int action (for sliders, spinners)
        void trigger_int_action(UIAction action, int value);

        // Trigger a float action (for sliders)
        void trigger_float_action(UIAction action, float value);

        // Trigger a string action (for text inputs)
        void trigger_string_action(UIAction action, const std::string& value);

        [[nodiscard]] bool has_action(UIAction action) const;
        [[nodiscard]] bool has_bool_action(UIAction action) const;
        [[nodiscard]] bool has_int_action(UIAction action) const;
        [[nodiscard]] bool has_float_action(UIAction action) const;
        [[nodiscard]] bool has_string_action(UIAction action) const;

        // ===== Clear all actions =====

        void clear_all_actions();

    private:
        // Callback storage for different types
        std::unordered_map<UIAction, std::vector<UIActionCallback>> action_callbacks;
        std::unordered_map<UIAction, std::vector<UIBoolCallback>> bool_callbacks;
        std::unordered_map<UIAction, std::vector<UIIntCallback>> int_callbacks;
        std::unordered_map<UIAction, std::vector<UIFloatCallback>> float_callbacks;
        std::unordered_map<UIAction, std::vector<UIStringCallback>> string_callbacks;
    };
} // ui
