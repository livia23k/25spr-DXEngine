#pragma once

#include "utils/forward.h"
#include <functional>
#include <string>

class WindowBase
{
public:
    WindowBase(uint32_t width, uint32_t height, const std::string &title)
        : m_width(width), m_height(height), m_title(title) {}
    virtual ~WindowBase() = default;

    virtual void onCreate() {}
    virtual void onDestroy() {}
    virtual void onFocus() {}
    virtual void onKillFocus() {}
    virtual void onResize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
    }

    virtual void onProcessEvents() const = 0;

    virtual uint32_t getWidth() const { return m_width; }
    virtual uint32_t getHeight() const { return m_height; }
    virtual void *getNativeHandle() const = 0;

    using DestroyCallback = std::function<void()>;
    using ResizeCallback = std::function<void(uint32_t width, uint32_t height)>;
    using ZoomCallback = std::function<void(float delta)>;

    void setDestroyCallback(DestroyCallback callback) { m_destroyCallback = std::move(callback); }
    void setResizeCallback(ResizeCallback callback) { m_resizeCallback = std::move(callback); }
    void setZoomCallback(ZoomCallback callback) { m_zoomCallback = std::move(callback); }

protected:
    uint32_t m_width;
    uint32_t m_height;
    std::string m_title;

    DestroyCallback m_destroyCallback;
    ResizeCallback m_resizeCallback;
    ZoomCallback m_zoomCallback;
};