#include"Object.h"

class Effect : public Object {
protected:
	float life, lifeTime;

public:
	Effect(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos, float lt) :Object(me, ma, hb, pos), life(lt), lifeTime(lt) {  }

	virtual void Die(std::vector<Effect*> &effect_list);
	virtual bool Cull();
};