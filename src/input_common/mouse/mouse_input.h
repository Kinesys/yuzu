// Copyright 2020 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <algorithm>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include "common/common_types.h"
#include "common/threadsafe_queue.h"
#include "core/frontend/input.h"
#include "input_common/main.h"
#include "input_common/motion_input.h"

namespace MouseInput {

enum class MouseButton {
    Left,
    Wheel,
    Right,
    Foward,
    Backward,
    Undefined,
};

struct MouseStatus {
    MouseButton button{MouseButton::Undefined};
};

struct MouseData {
    bool pressed{};
    std::array<int, 2> axis{};
    Input::MotionStatus motion{};
    Input::TouchStatus touch{};
};

class Mouse {
public:
    Mouse();
    ~Mouse();

    /// Used for polling
    void BeginConfiguration();
    void EndConfiguration();

    /**
     * Signals that a button is pressed.
     * @param x the x-coordinate of the cursor
     * @param y the y-coordinate of the cursor
     * @param button the button pressed
     */
    void PressButton(int x, int y, int button_);

    /**
     * Signals that mouse has moved.
     * @param x the x-coordinate of the cursor
     * @param y the y-coordinate of the cursor
     */
    void MouseMove(int x, int y);

    /**
     * Signals that a motion sensor tilt has ended.
     */
    void ReleaseButton(int button_);

    [[nodiscard]] Common::SPSCQueue<MouseStatus>& GetMouseQueue();
    [[nodiscard]] const Common::SPSCQueue<MouseStatus>& GetMouseQueue() const;

    [[nodiscard]] MouseData& GetMouseState(std::size_t button);
    [[nodiscard]] const MouseData& GetMouseState(std::size_t button) const;

private:
    void UpdateThread();
    void UpdateYuzuSettings();

    struct MouseInfo {
        InputCommon::MotionInput motion{0.0f, 0.0f, 0.0f};
        Common::Vec2<int> mouse_origin;
        Common::Vec2<int> last_mouse_position;
        bool is_tilting = false;
        float sensitivity{0.120f};

        float tilt_speed = 0;
        Common::Vec2<float> tilt_direction;
        MouseData data;
    };

    u16 buttons{};
    std::thread update_thread;
    MouseButton last_button{MouseButton::Undefined};
    std::array<MouseInfo, 5> mouse_info;
    Common::SPSCQueue<MouseStatus> mouse_queue;
    bool configuring{false};
    bool update_thread_running{true};
};
} // namespace MouseInput
