#include "EcsFactory.h"

#include <iostream>
#include <vector>
#include <array>
#include <string>


/*


*/

namespace Architecture {


    






    // Create new entity and add to the entity list.

void ECS::CreateEntity(/*attributes*/) {
    Entity tempEntity = {/*attributes*/ };
    entities.emplace_back(tempEntity);
    ++entityID;
}

void ECS::DeleteEntity(size_t entityID) {
    entities.erase(entities.begin() + entityID);
}

void ECS::DeleteAllEntities() {
    for (Entity val : entities) {
        /* Delete Each Attribute */
    }
}

}