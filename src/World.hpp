#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include "sys/IDrawable.hpp"

class Map;
class Tank;
class Bullet;
class Renderer;

class World : public IDrawable {
public:
    World(Map* map);
    virtual ~World();
    
    const std::vector<Tank*>& getTanks() const;
    const std::vector<Bullet*>& getBullets() const;
    Map* getMap() const;
    void addTank(Tank* tank);
    void addBullet(Bullet* bullet);
    
    // void setUpdateTime(float time) { m_updateTime = time; }
    // float getCurTime() const { return m_curTime; }
    
    void update(float time);
    virtual void draw(Renderer* renderer) override;
protected:
private:
    Map* m_map;
    std::vector<Tank*> m_tanks;
    std::vector<Bullet*> m_bullets;
    // std::vector<Bonus*> m_bonuses;
    
    // float m_updateTime;
    // float m_curTime;
};

#endif