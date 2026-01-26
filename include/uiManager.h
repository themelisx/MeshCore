#ifndef UI_MANAGER_h
#define UI_MANAGER_h

#include <Arduino.h>
#include "uiDefines.h"
#include <helpers/ContactInfo.h>

class UIManager {
  private:
    // functions 
    void format_datetime(char *buf, size_t size, const struct tm *timeinfo);
    void timestampToTime(time_t timestamp, char *buffer, size_t buffer_size);
    const char* convertDegreesToDirection(int degrees);
    int windSpeedToBeaufort(float speed);
    void getInitials(const char *name, char *out);
    void formatLastSeen(uint32_t ts, char *out, size_t len);
    
    // Calendar days and months  
    static const char *days[7];
    static const char *months[12];

    // vars
    char time_str[9];
    String lastUpdate = "";
    char* tmp_buf;

    lv_obj_t *chat_items[MAX_CHAT_MESSAGES];
    int chat_count = 0;

  public:
    UIManager();

    void updateDateTime(const struct tm timeinfo);
    void updateInfo(const char *str, uint32_t color);
    void clearDateTime();
    void updateValues();    
    void addPrivateChatBubble(lv_obj_t *list, const char *time_str, const char *msg, bool is_self);
    void addChatBubble(lv_obj_t *list, const char *time_str, const char *sender, const char *msg, bool is_self);
    void addContactToUI(ContactInfo c);
    void handleContactClick(lv_event_t *e);
};

#endif