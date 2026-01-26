#include <Arduino.h>

#include "esp_log.h"
#include "UI/ui.h"
#include "uiDefines.h"
#include "uiVars.h"

#include "uiManager.h"

#include <helpers/ContactInfo.h>
#include <helpers/AdvertDataHelpers.h>

#if defined(LANG_GR)
const char *UIManager::days[7] = {"Κυρ", "Δευ", "Τρι", "Τετ", "Πεμ", "Παρ", "Σαβ"};
const char *UIManager::months[12] = {"Ιαν", "Φεβ", "Μαρ", "Απρ", "Μαι", "Ιουν",
                                     "Ιουλ", "Αυγ", "Σεπ", "Οκτ", "Νοε", "Δεκ"};
#elif defined(LANG_EN)
const char *UIManager::days[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *UIManager::months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
#endif

#define TAG "UIManager"

UIManager::UIManager() {

  //myDebug->println(DEBUG_LEVEL_DEBUG, "[UIManager]");
  tmp_buf = (char*)malloc(128);

}

void UIManager::format_datetime(char *buf, size_t size, const struct tm *timeinfo) {
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%a, %d %b %Y", timeinfo);

    int wday = timeinfo->tm_wday; // 0=Κυρ ... 6=Σαβ
    int mon  = timeinfo->tm_mon;  // 0=Ιαν ... 11=Δεκ

    // replace %a and %b with selected language
    snprintf(buf, size, "%s, %02d %s %d", days[wday], timeinfo->tm_mday, months[mon], 1900 + timeinfo->tm_year);
}

void UIManager::updateDateTime(const struct tm timeinfo) {
  // TODO: Add to settings "Date format"
  char date_str[50];
  format_datetime(date_str, sizeof(date_str), &timeinfo);
  lv_label_set_text(ui_ValueDate, date_str);

  // TODO: Add to settings "Hour format"
  strftime(tmp_buf, 50, "%H:%M", &timeinfo);      // 24h format
  //strftime(tmp_buf, 50, "%I:%M %p", &timeinfo); // 12h format
  lv_label_set_text(ui_ValueTime, tmp_buf);

  // TODO: Add to settings "dim at night"
  // TODO: Add to settings "dim hours"
  // TODO: Add to settings "dim percentage"
  if (timeinfo.tm_hour > 21 || timeinfo.tm_hour < 7) {
    setNightMode(true);
  } else {
    setNightMode(false);
  }
}

void UIManager::clearDateTime() {
  #if defined(LANG_EN)
    uiManager->updateInfo("Clock sync...", COLOR_WHITE);
  #elif defined(LANG_GR)
    uiManager->updateInfo("Συγχρονισμός ώρας...", COLOR_WHITE);
  #endif
  lv_label_set_text(ui_ValueDate, "--- --/--/----");
  lv_label_set_text(ui_ValueTime, "--:--");
}

void UIManager::timestampToTime(time_t timestamp, char *buffer, size_t buffer_size) {
    struct tm *time_info;
    time_info = localtime(&timestamp);
    strftime(buffer, buffer_size, "%H:%M", time_info);
}

const char* UIManager::convertDegreesToDirection(int degrees) {
    // Normalize degrees to [0, 360)
    degrees = degrees % 360;
    if (degrees < 0) degrees += 360;

#if defined(LANG_EN)
    static constexpr const char* dirs[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
#elif defined(LANG_GR)
    static constexpr const char* dirs[] = {"Β", "ΒΑ", "Α", "ΝΑ", "Ν", "ΝΔ", "Δ", "ΒΔ"};
#else
    #error "No Language defined!"
#endif

    // Each direction covers 45°, starting at N = 0°
    int index = static_cast<int>((degrees + 22.5) / 45.0) % 8;
    return dirs[index];
}


int UIManager::windSpeedToBeaufort(float speed) {
    static const float limits[] = {
        0.5, 1.5, 3.3, 5.5, 7.9, 10.7,
        13.8, 17.1, 20.7, 24.4, 28.4, 32.6
    };

    for (int i = 0; i < 12; ++i)
        if (speed < limits[i])
            return i;
    return 12;
}

void UIManager::updateValues() {
    lv_label_set_text(ui_ValueTime, "--:--");
}

void UIManager::updateInfo(const char *str, uint32_t color) {
  // lv_label_set_text(ui_ValueLastUpdate, str);  
  // lv_obj_set_style_text_color(ui_ValueLastUpdate, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
}

void UIManager::addChatBubble(lv_obj_t *list,
                     const char *time_str,
                     const char *sender,
                     const char *msg,
                     bool is_self)
{
    // Remove oldest
    if (chat_count >= MAX_CHAT_MESSAGES) {
        lv_obj_del(chat_items[0]);
        memmove(&chat_items[0], &chat_items[1], sizeof(lv_obj_t*) * (MAX_CHAT_MESSAGES - 1));
        chat_count--;
    }

    // Row container (align bubble left/right)
    lv_obj_t *row = lv_obj_create(list);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_outline_width(row, 0, 0);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row,
        is_self ? LV_FLEX_ALIGN_END : LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START);

    // Bubble container (COLUMN)
    lv_obj_t *bubble = lv_obj_create(row);
    lv_obj_set_width(bubble, lv_pct(85));
    lv_obj_set_height(bubble, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(bubble, 12, 0);
    lv_obj_set_style_pad_all(bubble, 10, 0);    
    lv_obj_set_style_bg_color(bubble,
        is_self ? lv_color_hex(0x1E88E5) : lv_color_hex(0x2C2C2C), 0);

    // IMPORTANT: vertical layout inside bubble
    lv_obj_set_flex_flow(bubble, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(bubble,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START);

    // Header row (sender + time)
    lv_obj_t *hdr = lv_obj_create(bubble);
    lv_obj_set_width(hdr, lv_pct(100));
    lv_obj_set_height(hdr, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(hdr, 0, 0);
    lv_obj_set_style_border_width(hdr, 0, 0);
    lv_obj_set_style_pad_all(hdr, 0, 0);
    lv_obj_set_style_outline_width(hdr, 0, 0);

    lv_obj_set_flex_flow(hdr, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(hdr,
        LV_FLEX_ALIGN_SPACE_BETWEEN,
        LV_FLEX_ALIGN_START, //LV_FLEX_ALIGN_CENTER, // Ευθυγράμμιση ονόματος/ώρας στον κάθετο άξονα
        LV_FLEX_ALIGN_START);

    lv_obj_t *lbl_sender = lv_label_create(hdr);
    lv_label_set_text(lbl_sender, sender);
    lv_obj_set_style_text_color(lbl_sender,
        is_self ? lv_color_hex(0xE3F2FD) : lv_color_hex(0x90CAF9), 0);
    lv_obj_set_style_text_font(lbl_sender, &lv_font_arial_22, 0);

    lv_obj_t *lbl_time = lv_label_create(hdr);
    lv_label_set_text(lbl_time, time_str);
    lv_obj_set_style_text_color(lbl_time, lv_color_hex(0xB0B0B0), 0);
    lv_obj_set_style_text_font(lbl_time, &lv_font_arial_20, 0);

    // Message body (below header)
    lv_obj_t *lbl_msg = lv_label_create(bubble);
    lv_label_set_text(lbl_msg, msg);
    lv_label_set_long_mode(lbl_msg, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(lbl_msg, lv_pct(100));

    lv_obj_set_style_text_color(lbl_msg, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl_msg, &lv_font_arial_26, 0);

    // Spacing between header and text
    //lv_obj_set_style_margin_top(lbl_msg, 6, 0);
    lv_obj_set_style_pad_row(bubble, 6, 0);

    chat_items[chat_count++] = row;

    lv_obj_scroll_to_view(row, LV_ANIM_ON);
  }

void UIManager::addPrivateChatBubble(lv_obj_t *list, const char *time_str, const char *msg, bool is_self) {
    
  lv_obj_set_style_pad_bottom(list, 20, 0);

  // 1. Row container
  lv_obj_t *row = lv_obj_create(list);
  lv_obj_set_width(row, lv_pct(100));
  lv_obj_set_height(row, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(row, 0, 0);
  lv_obj_set_style_border_width(row, 0, 0);
  lv_obj_set_style_pad_all(row, 4, 0);
  lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row, 
      is_self ? LV_FLEX_ALIGN_END : LV_FLEX_ALIGN_START, 
      LV_FLEX_ALIGN_START, 
      LV_FLEX_ALIGN_START);

  // 2. Aligner (Column) - Κρατάει το Label και την Ώρα
  lv_obj_t *aligner = lv_obj_create(row);
  lv_obj_set_width(aligner, LV_SIZE_CONTENT);
  lv_obj_set_height(aligner, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(aligner, 0, 0);
  lv_obj_set_style_border_width(aligner, 0, 0);
  lv_obj_set_style_pad_all(aligner, 0, 0);
  lv_obj_set_flex_flow(aligner, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(aligner, 
      is_self ? LV_FLEX_ALIGN_END : LV_FLEX_ALIGN_START, 
      LV_FLEX_ALIGN_START, 
      LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_row(aligner, 4, 0);

  // 3. Το Label-Bubble (Ενοποιημένο)
  lv_obj_t *lbl_msg = lv_label_create(aligner);

  // Long mode για wrap
  lv_label_set_long_mode(lbl_msg, LV_LABEL_LONG_WRAP);
  lv_label_set_text(lbl_msg, msg);
  lv_obj_set_style_text_font(lbl_msg, &lv_font_arial_22, 0);

  // Fixed max width για wrap
  lv_obj_set_width(lbl_msg, 400);           // max πλάτος
  lv_obj_set_height(lbl_msg, LV_SIZE_CONTENT);  // αυτόματο ύψος

  // Bubble style
  lv_obj_set_style_bg_opa(lbl_msg, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(lbl_msg, 12, 0);
  lv_obj_set_style_pad_all(lbl_msg, 12, 0);

  if(is_self) {
      lv_obj_set_style_bg_color(lbl_msg, lv_color_hex(0x1E88E5), 0);
      lv_obj_set_style_text_color(lbl_msg, lv_color_hex(0xFFFFFF), 0);
  } else {
      lv_obj_set_style_bg_color(lbl_msg, lv_color_hex(0xFFFFFF), 0);
      lv_obj_set_style_text_color(lbl_msg, lv_color_hex(0x000000), 0);
  }

  // 4. Η Ώρα
  lv_obj_t *lbl_time = lv_label_create(aligner);
  lv_label_set_text(lbl_time, time_str);
  lv_obj_set_style_text_color(lbl_time, lv_color_hex(0x808080), 0);
  lv_obj_set_style_text_font(lbl_time, &lv_font_arial_14, 0);

  lv_obj_scroll_to_view(row, LV_ANIM_ON);
}

void UIManager::getInitials(const char *name, char *out) {
    out[0] = 0;
    if (!name || !name[0]) return;

    const char *p = name;
    while (*p && !isalnum((unsigned char)*p)) {
        p++;
    }
    
    char first = (*p) ? *p : name[0];
    char second = 0;
    const char *space = strchr(name, ' ');
    
    if (space) {
        const char *s = space + 1;
        while (*s && !isalnum((unsigned char)*s)) {
            s++;
        }
        if (*s) {
            second = *s;
        }
    }

    out[0] = toupper((unsigned char)first);
    if (second) {
        out[1] = toupper((unsigned char)second);
        out[2] = 0;
    } else {
        out[1] = 0;
    }
}

void UIManager::formatLastSeen(uint32_t ts, char *out, size_t len) {
    if (ts == 0) {
        snprintf(out, len, "Never");
        return;
    }

    time_t t = (time_t)ts;
    struct tm *tm = localtime(&t);

    if (tm == nullptr) {
        snprintf(out, len, "Unknown");
        return;
    }

    snprintf(out, len, "%02d:%02d %02d/%02d/%02d",
        tm->tm_hour, 
        tm->tm_min,
        tm->tm_mday, 
        tm->tm_mon + 1, 
        (tm->tm_year + 1900) % 100); // Προσθέτουμε το 1900 και παίρνουμε τα τελευταία 2 ψηφία
}

static void onContactClick(lv_event_t *e)
{
    UIManager *self = (UIManager*) lv_event_get_user_data(e);
    if(self) self->handleContactClick(e);
}

void UIManager::handleContactClick(lv_event_t *e)
{
    lv_obj_t *row = lv_event_get_target(e);
    ContactInfo *c = (ContactInfo*) lv_obj_get_user_data(row);

    Serial.printf("Clicked: %s\n", c->name);
}

void UIManager::addContactToUI(ContactInfo c)
{
    const int ROW_W = 200;
    const int ROW_H = 64;
    const int AVATAR = 44;
    const int PAD = 4;

    // Create list button row
    lv_obj_t *btn = lv_list_add_btn(ui_Contacts, NULL, NULL);
    lv_obj_set_size(btn, ROW_W, ROW_H);
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_layout(btn, 0);

    lv_obj_set_style_bg_color(btn, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);

    lv_obj_set_style_outline_width(btn, 0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x222222), 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x111111), LV_STATE_PRESSED);

    // Store pointer
    ContactInfo *store = new ContactInfo(c);
    lv_obj_set_user_data(btn, store);

    // Click handler
    lv_obj_add_event_cb(btn, onContactClick, LV_EVENT_CLICKED, this);

    // Disable scrolling
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    // ============================
    // Avatar
    // ============================
    lv_obj_t *content = lv_obj_create(btn);
    lv_obj_set_size(content, ROW_H, ROW_H);
    lv_obj_set_pos(content, 0, 0);
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_set_style_pad_all(content, 0, 0);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *avatar = lv_obj_create(content);
    lv_obj_set_size(avatar, AVATAR, AVATAR);
    lv_obj_set_pos(avatar, 0, 10);
    lv_obj_set_style_radius(avatar, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(avatar, lv_color_hex(0x4A90E2), 0);
    lv_obj_set_style_border_width(avatar, 0, 0);

    char initials[4];
    if (c.type == ADV_TYPE_REPEATER) {
      strcpy(initials, "(R)");
    } else {
      getInitials(c.name, initials);
    }

    lv_obj_t *init_label = lv_label_create(avatar);
    lv_label_set_text(init_label, initials);
    lv_obj_center(init_label);
    lv_obj_set_style_text_font(init_label, &lv_font_arial_22, 0);
    lv_obj_set_style_text_color(init_label, lv_color_white(), 0);

    // ============================
    // TEXT COLUMN (FIXED PIXELS)
    // ============================
    int text_x = PAD + AVATAR+ PAD;
    int text_w = ROW_W - text_x - PAD;

    lv_obj_t *text_col = lv_obj_create(btn);
    lv_obj_set_pos(text_col, text_x, 0);
    lv_obj_set_size(text_col, text_w, ROW_H - PAD);

    lv_obj_set_style_bg_opa(text_col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(text_col, 0, 0);
    lv_obj_set_style_pad_all(text_col, 0, 0);
    lv_obj_clear_flag(text_col, LV_OBJ_FLAG_SCROLLABLE);

    // ============================
    // NAME LABEL (FIXED)
    // ============================
    lv_obj_t *name_label = lv_label_create(text_col);
    lv_obj_set_pos(name_label, 0, PAD + 4);
    lv_obj_set_width(name_label, text_w);

    lv_label_set_text(name_label, c.name);
    lv_label_set_long_mode(name_label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(name_label, &lv_font_arial_20, 0);
    lv_obj_set_style_text_color(name_label, lv_color_white(), 0);

    // ============================
    // LAST SEEN LABEL (FIXED)
    // ============================
    char lastSeen[32];
    formatLastSeen(c.lastmod, lastSeen, sizeof(lastSeen));

    lv_obj_t *seen_label = lv_label_create(text_col);
    lv_obj_set_pos(seen_label, 0, PAD + 32);
    lv_obj_set_width(seen_label, text_w);

    lv_label_set_text_fmt(seen_label, "%s", lastSeen);
    lv_label_set_long_mode(seen_label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(seen_label, &lv_font_arial_16, 0);
    lv_obj_set_style_text_color(seen_label, lv_color_hex(0x888888), 0);

    lv_obj_clear_flag(avatar, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(text_col, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(name_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(seen_label, LV_OBJ_FLAG_CLICKABLE);

}
