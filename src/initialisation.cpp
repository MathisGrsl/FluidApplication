#include "ge.hpp"

void initialisation()
{
    ge::initMolecules(0.03);
    ge::generateMolecules();
    ge::initGridHitbox(ge::objects);
}
