#include "IEvent.h"

namespace Event {
size_t IEvent::s_instanceId = 0;

IEvent::IEvent()
    : m_instanceId(s_instanceId++)
{
}

size_t IEvent::GetInstanceId() const
{
    return m_instanceId;
}
} // namespace Event