#include "Map.hpp"

#include <fstream>
#include <sstream>
#include "sys/System.hpp"
#include "Block.hpp"
#include "Renderer.hpp"
#include "sys/IPositionable.hpp"

std::map< std::string, Map* > Map::m_loaded;

Map::Map(const Map& r)
{
    copy(r);
}

Map::Map(float width, float height)
: m_width(width), m_height(height), m_fileName("")
{}

Map::Map(const std::string& fileName)
{
    m_fileName = fileName;
    if (m_loaded.count(fileName)) {
        copy(*m_loaded.at(fileName));
    } else {
        loadFromFile(MAPS_PATH + fileName + ".mp");
        m_loaded[fileName] = new Map(*this);
    }
}

Map::~Map() {
    for (unsigned int i = 0; i < m_blocks.size(); ++i)
        delete m_blocks[i];
}

void Map::clear() {
    m_blocks.clear();
    m_spawns.clear();
}

void Map::draw(Renderer* renderer) {
    for (unsigned int i = 0; i < m_blocks.size(); ++i)
        renderer->draw(m_blocks[i]);
    
    renderer->draw(Polygon({{0, 0}, {m_width, 0}, {m_width, m_height}, {0, m_height}}), IPositionable());
}

void Map::freeLoaded() {
    Map* tmp;
    for (auto i = m_loaded.begin(); i != m_loaded.end(); ++i) {
        tmp = i->second;
        m_loaded.erase(i->first);
        delete tmp;
    }
}

void Map::loadFromFile(const std::string& filePath) {
    m_blocks.clear();
    std::ifstream s;
    s.open(filePath);
    if (!s)
        throw std::runtime_error("Couldn't open map file " + filePath);
    std::string line, type;
    while (std::getline(s, line)) {
        std::stringstream buf(line);
        buf >> type;
        if (!type.size()) continue;
        
        if (type == "d") {
            float w, h;
            buf >> w >> h;
            m_width = w;
            m_height = h;
        } else if (type == "b") {
            std::string shape;
            bool bulletproof, passable;
            float posx, posy, rotdeg, scalex, scaley;
            buf >> bulletproof >> passable >> shape >> posx >> posy >> rotdeg >> scalex >> scaley;
            
            Block* b = new Block(bulletproof, passable, new Polygon(shape));
            b->setPos({posx, posy});
            b->setRotDeg(rotdeg);
            b->setScale({scalex, scaley});
            m_blocks.push_back(b);
        } else if (type == "s") {
            float x, y;
            buf >> x >> y;
            m_spawns.push_back({x, y}); //!!!!!
        }
    }
}

void Map::copy(const Map& r) {
    m_width = r.getWidth();
    m_height = r.getHeight();
    m_blocks = r.getBlocks();
    m_spawns = r.getSpawns();
    m_fileName = r.getFileName();
}