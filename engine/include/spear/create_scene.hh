#ifndef SPEAR_CREATE_SCENE_HH
#define SPEAR_CREATE_SCENE_HH

#include <functional>
#include <spear/scene_manager.hh>

namespace spear
{

uint64_t createScene(spear::Scene::Container objects, const std::function<void(spear::Scene::Container&)> place_elements, spear::SceneManager& scene_manager);

}

#endif
