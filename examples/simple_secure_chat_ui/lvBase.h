#include "lvgl.h"

#ifndef LV_BASE_h
#define LV_BASE_h
class LvBase {
public:
    operator lv_obj_t*() const { return obj; }
    lv_obj_t* raw() const { return obj; }

    LvBase& width(int v) {
        lv_obj_set_width(obj, v == 100 ? lv_pct(100) : v);
        return *this;
    }

    LvBase& height(int v) {
        lv_obj_set_height(obj, v);
        return *this;
    }

    LvBase& size(int w, int h) {
        lv_obj_set_size(obj, w, h);
        return *this;
    }

    LvBase& align(lv_align_t a, int x = 0, int y = 0) {
        lv_obj_align(obj, a, x, y);
        return *this;
    }

    LvBase& pos(int x, int y) {
        lv_obj_set_pos(obj, x, y);
        return *this;
    }

    LvBase& padAll(int v) {
        lv_obj_set_style_pad_all(obj, v, 0);
        return *this;
    }

    LvBase& bgOpa(int v) {
        lv_obj_set_style_bg_opa(obj, v, 0);
        return *this;
    }

    LvBase& bgColor(uint32_t hex) {
        lv_obj_set_style_bg_color(obj, lv_color_hex(hex), 0);
        return *this;
    }

    LvBase& border(int w) {
        lv_obj_set_style_border_width(obj, w, 0);
        return *this;
    }

    LvBase& radius(int r) {
        lv_obj_set_style_radius(obj, r, 0);
        return *this;
    }

    LvBase& noScroll() {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        return *this;
    }

    LvBase& clickable(bool v = true) {
        v ? lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE)
          : lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        return *this;
    }

protected:
    lv_obj_t* obj = nullptr;
};
#endif