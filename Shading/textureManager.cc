#include <cstdio>
#include <cstdlib>
#include "tools.h"
#include "tools.h"
#include "textureManager.h"
#include "texturert.h"

using std::map;
using std::make_pair;
using std::pair;
using std::string;

TextureManager * TextureManager::instance() {
	static TextureManager mgr;
	return &mgr;
}

TextureManager::TextureManager() {
	// Create default "white" texture
	m_white = new Texture();
	m_white->setWhiteTexture();
	m_hash.insert(make_pair("MG_WHITE_TEX", m_white));

}

TextureManager::~TextureManager() {
	for(auto & [k,v] : m_hash)
		delete v;
}

Texture *TextureManager::create(const std::string & texName, const std::string & fileName) {
	std::string fName = fileName.size() ? fileName : texName;
	auto it = m_hash.find(texName);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate texture %s\n", texName.c_str());
		return it->second;
	}
	Texture * newtex = new Texture(texName);
	newtex->setImage(fName);
	it = m_hash.insert(make_pair(texName, newtex)).first;
	return it->second;
}

Texture *TextureManager::createBumpMap(const std::string & texName, const std::string & fileName) {
	std::string fName = fileName.size() ? fileName : texName;
	auto it = m_hash.find(texName);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate bump texture %s\n", texName.c_str());
		return it->second;
	}
	Texture * newtex = new Texture(texName);
	newtex->setBumpMap(fName);
	it = m_hash.insert(make_pair(texName, newtex)).first;
	return it->second;
}

Texture *TextureManager::createProj(const std::string & texName, const std::string & fileName) {
	std::string fName = fileName.size() ? fileName : texName;
	auto it = m_hash.find(texName);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate projective texture %s\n", texName.c_str());
		return it->second;
	}
	Texture * newtex = new Texture(texName);
	newtex->setProj(fName);
	it = m_hash.insert(make_pair(texName, newtex)).first;
	return it->second;
}

Texture *TextureManager::createCubeMap(const std::string &texName,
									   const std::string &xpos,
									   const std::string &xneg,
									   const std::string &ypos,
									   const std::string &yneg,
									   const std::string &zpos,
									   const std::string &zneg) {
	auto it = m_hash.find(texName);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate cubemap texture %s\n", texName.c_str());
		return it->second;
	}
	Texture *newtex = new Texture(texName);
	newtex->setCubeMap(xpos, xneg,
					   ypos, yneg,
					   zpos, zneg);
	it = m_hash.insert(make_pair(texName, newtex)).first;
	return it->second;
}

Texture *TextureManager::createRTDepth(const std::string & texName, int h, int w) {
	auto it = m_hash.find(texName);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate rtdepth texture %s\n", texName.c_str());
		return 0;
	}
	TextureRT *newtex = new TextureRT(Texture::rt_depth, h, w);
	it = m_hash.insert(make_pair(texName, newtex)).first;
	return newtex;
}

Texture *TextureManager::createRTColor(const std::string & texName, int h, int w) {
	auto it = m_hash.find(texName);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate rt texture %s\n", texName.c_str());
		return 0;
	}
	TextureRT *newtex = new TextureRT(Texture::rt_color, h, w);
	it = m_hash.insert(make_pair(texName, newtex)).first;
	return newtex;
}

Texture *TextureManager::find(const std::string & fName) const {
	auto it = m_hash.find(fName);
	if (it == m_hash.end()) return 0;
	return it->second;
}

Texture *TextureManager::whiteTexture() const { return m_white; }

TextureManager::iterator TextureManager::begin() { return TextureManager::iterator(m_hash.begin()); }
TextureManager::iterator TextureManager::end() { return TextureManager::iterator(m_hash.end()); }

// Debug

void TextureManager::print() const {
	for(auto & [k,v] : m_hash)
		v->print();
}
