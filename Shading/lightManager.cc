#include <cstdio>
#include "tools.h"
#include "lightManager.h"

using std::vector;
using std::string;

LightManager * LightManager::instance() {
	static LightManager mgr;
	return &mgr;
}

LightManager::LightManager() {}

LightManager::~LightManager() {
	for(auto & light : m_lights)
		delete light;
}

Light *LightManager::create(const std::string &name,
							Light::type_t t) {
	Light *l = this->find(name);
	if (l) {
		fprintf(stderr, "[W] duplicate light %s\n", name.c_str());
		return l;
	}
	Light *newlght = new Light(name, t);
	m_lights.push_back(newlght);
	return newlght;
}


Light *LightManager::find(const std::string & name) const {
	for (auto & light :m_lights)
		if (light->getNameString() == name) return light;
	return 0;
}

void LightManager::switchOn() {

	for(auto & light : m_lights)
		light->switchLight(true);
}

void LightManager::switchOff() {

	for(auto & light : m_lights)
		light->switchLight(false);
}

void LightManager::print() const {
	for(auto & light : m_lights)
		light->print();
}

LightManager::iterator LightManager::begin() { return LightManager::iterator(m_lights.begin()); }
LightManager::iterator LightManager::end() { return LightManager::iterator(m_lights.end()); }
