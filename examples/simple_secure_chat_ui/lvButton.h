#include "lvBase.h"

#ifndef LV_BUTTON_h
#define LV_BUTTON_h
class LvButton : public LvBase {
public:
    explicit LvButton(lv_obj_t* parent) {
        obj = lv_btn_create(parent);
    }

    LvButton& onClick(lv_event_cb_t cb, void* user = nullptr) {
        lv_obj_add_event_cb(obj, cb, LV_EVENT_CLICKED, user);
        return *this;
    }
};
#endif