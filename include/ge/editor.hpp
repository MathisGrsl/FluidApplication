#pragma once

namespace ge {

    inline ge::object::instanceData *objectSelected = nullptr;
    inline ge::object::instanceData objectSelectedOriginSetting = {};
    inline bool isEditing = false;
    inline ge::object::instanceData *arrowSelected = nullptr;

    inline bool isCreating = false;
    inline int newObjectSelected = -1;

    inline int objTypeIsEditing = -1;

}
