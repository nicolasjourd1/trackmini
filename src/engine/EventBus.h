#ifndef EVENTBUS_H_
#define EVENTBUS_H_

#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace trackmini::engine {

namespace Events {

struct WindowResized
{
    int width;
    int height;
};

struct KeyPressed
{
    int scancode; // SDL_Scancode
    bool repeat;
};

struct KeyReleased
{
    int scancode;
};

struct QuitRequested
{};

// NOTE: here will go physics/gameplay event later

} // namespace Events

class EventBus
{
  public:
    using HandlerFn = std::function<void(void const*)>;
    using SubscriptionId = std::size_t;

    template<typename EventT>
    [[nodiscard]]
    SubscriptionId subscribe(std::function<void(EventT const&)> handler)
    {
        auto const id = m_next_id++;
        auto const key = std::type_index{ typeid(EventT) };

        m_handlers[key].push_back(
          { id, [h = std::move(handler)](void const* ptr) {
               h(*static_cast<EventT const*>(ptr));
           } });
        return id;
    }

    template<typename EventT, typename Fn>
    [[nodiscard]]
    SubscriptionId subscribe(Fn&& fn)
    {
        return subscribe<EventT>(
          std::function<void(EventT const&)>{ std::forward<Fn>(fn) });
    }

    template<typename EventT>
    void unsubscribe(SubscriptionId id)
    {
        auto const key = std::type_index{ typeid(EventT) };
        auto it = m_handlers.find(key);
        if (it == m_handlers.end())
            return;

        auto& vec = it->second;
        std::erase_if(vec, [id](Entry const& e) { return e.id == id; });
    }

    template<typename EventT>
    void emit(EventT const& event)
    {
        auto const key = std::type_index{ typeid(EventT) };
        auto it = m_handlers.find(key);
        if (it == m_handlers.end())
            return;

        auto handlers_copy = it->second;
        for (auto const& entry : handlers_copy) {
            entry.fn(static_cast<void const*>(&event));
        }
    }

    template<typename EventT>
    [[nodiscard]]
    std::size_t subscriber_count() const
    {
        auto const key = std::type_index{ typeid(EventT) };
        auto it = m_handlers.find(key);
        return it != m_handlers.end() ? it->second.size() : 0;
    }

  private:
    struct Entry
    {
        SubscriptionId id;
        HandlerFn fn;
    };

    std::unordered_map<std::type_index, std::vector<Entry>> m_handlers;
    SubscriptionId m_next_id{ 0 };
};

} // namespace trackmini::engine

#endif /* EVENTBUS_H_ */
