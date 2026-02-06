#include "lvBase.h"

#ifndef LV_OBJ_h
#define LV_OBJ_h
class LvObj : public LvBase {
public:
    explicit LvObj(lv_obj_t* parent) {
        obj = lv_obj_create(parent);
    }

    explicit LvObj(lv_obj_t* existing, bool wrapOnly) {
        obj = existing;
    }

    // Fluent Flex
    LvObj& flexFlow(lv_flex_flow_t flow) {
        lv_obj_set_flex_flow(obj, flow);
        return *this;
    }

    LvObj& flexAlign(
        lv_flex_align_t main, 
        lv_flex_align_t cross, 
        lv_flex_align_t track = LV_FLEX_ALIGN_START
    ) {
        lv_obj_set_flex_align(obj, main, cross, track);
        return *this;
    }
};
#endif