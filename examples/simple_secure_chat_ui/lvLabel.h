#include "lvBase.h"


#ifndef LV_LABEL_h
#define LV_LABEL_h
class LvLabel : public LvBase {
public:
    explicit LvLabel(lv_obj_t* parent, const char* text = "") {
        obj = lv_label_create(parent);
        lv_label_set_text(obj, text);
    }

    LvLabel& text(const char* t) {
        lv_label_set_text(obj, t);
        return *this;
    }

    LvLabel& font(const lv_font_t* f) {
        lv_obj_set_style_text_font(obj, f, 0);
        return *this;
    }

    LvLabel& textColor(uint32_t hex) {
        lv_obj_set_style_text_color(obj, lv_color_hex(hex), 0);
        return *this;
    }

    LvLabel& opa(int v) {
        lv_obj_set_style_text_opa(obj, v, 0);
        return *this;
    }

    LvLabel& wrap(bool v = true) {
        lv_label_set_long_mode(
            obj,
            v ? LV_LABEL_LONG_WRAP : LV_LABEL_LONG_CLIP
        );
        return *this;
    }
};
#endif