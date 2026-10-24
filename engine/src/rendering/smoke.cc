#include <spear/rendering/smoke.hh>

#include <algorithm>

namespace spear::rendering
{

SmokeManager::Id SmokeManager::addEmitter(const SmokeEmitter& emitter)
{
    const Id id = m_nextId++;
    m_emitters.emplace_back(id, emitter);
    return id;
}

bool SmokeManager::removeEmitter(Id id)
{
    auto it = std::find_if(m_emitters.begin(), m_emitters.end(), [id](const auto& entry)
                           { return entry.first == id; });
    if (it == m_emitters.end())
        return false;
    m_emitters.erase(it);
    return true;
}

void SmokeManager::clearEmitters()
{
    m_emitters.clear();
}

SmokeEmitter* SmokeManager::getEmitter(Id id)
{
    for (auto& [emitterId, emitter] : m_emitters)
    {
        if (emitterId == id)
            return &emitter;
    }
    return nullptr;
}

} // namespace spear::rendering
