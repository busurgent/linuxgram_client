#include <json-c/json.h>
#include <json-c/json_object.h>
#include <json-c/json_util.h>
#include <time.h>
#include <cdk.h>


#include <cdk/cdkscreen.h>
#include <cdk/dialog.h>
#include <cdk/entry.h>
#include <cdk/label.h>
#include <cdk/mentry.h>
#include <cdk/scroll.h>
#include <cdk/selection.h>
#include <cdk/swindow.h>
#include <cdk/viewer.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

int authorize(char *username, char *pass) { return !strcmp(username, pass); }


void login_menu(CDKSCREEN *screen) {
  CDKENTRY *login_entry =
      newCDKEntry(screen, CENTER, CENTER, "</B>Username<!B>", "", A_NORMAL, ' ',
                  vMIXED, 21, 6, 20, FALSE, FALSE);
  CDKENTRY *password_entry =
      newCDKEntry(screen, CENTER, CENTER, "</B>Password<!B>", "", A_NORMAL, ' ',
                  vHMIXED, 21, 6, 20, FALSE, FALSE);
  moveCDKEntry(login_entry, -3, 0, TRUE, TRUE);
  moveCDKEntry(password_entry, 3, 0, TRUE, TRUE);
  char *s[2] = {"<C></31>Wrong username or password<!31>", NULL};
  CDKLABEL *label = newCDKLabel(screen, CENTER, BOTTOM, s, 1, false, false);
  setCDKEntryHiddenChar(password_entry, '*');
  while (1) {
    drawCDKEntry(login_entry, TRUE);
    char *login = activateCDKEntry(login_entry, NULL);
    drawCDKEntry(password_entry, FALSE);
    char *pass = activateCDKEntry(password_entry, NULL);
    refreshCDKScreen(screen);
    if (authorize(login, pass))
      break;
    drawCDKLabel(label, FALSE);
    refreshCDKScreen(screen);
  }
  eraseCDKEntry(login_entry);
  eraseCDKEntry(password_entry);
  eraseCDKLabel(label);
  refreshCDKScreen(screen);
  destroyCDKEntry(login_entry);
  destroyCDKEntry(password_entry);
  destroyCDKLabel(label);
}

typedef struct {
  uint year : 16;
  uint month : 4;
  uint hours : 5;
  uint minutes : 7;
} date;

typedef struct {
  char username[21];
  date last_date;
  char last_msg[21];
  uint unread : 1;
} chat;

chat *get_chats_list(int *len) {
  chat *ret = calloc(1, sizeof(chat));
  strncpy(ret->username, "pintosum", 20);
  ret->last_date = (date){2025, 2, 23, 19};
  strncpy(ret->last_msg, "Hello, buddy!", 20);
  ret->unread = 1;
  *len = 1;
  return ret;
}

char **get_chat_ui(chat *chats, int len, int width) {
  char **ret = calloc(len, sizeof(char *));
  int name_width = width / 2.5;
  int msg_width = width / 2.5;
  int time_width = width / 5;
  for (int i = 0; i < len; i++) {
    ret[i] = calloc(width * 2, sizeof(char));
    int shift = sprintf(ret[i], "</B>%-*s<!B>", name_width, chats[i].username);
    shift += sprintf(ret[i] + shift, "</D>%-*s", msg_width, chats[i].last_msg);
    shift += sprintf(ret[i] + shift, "%*c", time_width - 7, ' ');
    if (chats[i].unread) {
      sprintf(ret[i] + shift, "</U></K>%u:%u<!U><!K><!D>",
              chats[i].last_date.hours, chats[i].last_date.minutes);
    } else {
      sprintf(ret[i] + shift, "</U>%u:%u<!U><!D>", chats[i].last_date.hours,
              chats[i].last_date.minutes);
    }
  }
  return ret;
}

char **read_file(int *len){
  FILE *f = fopen(".chat.txt", "r");
  char **ret = calloc(1024, sizeof(char*));
  for(int i = 0; i < 1024; i++){
    ret[i] = calloc(31, sizeof(char));
    fread(ret[i], sizeof(char), 30, f);
    if(feof(f))
      break;
  }
  fclose(f);
  return ret;
}

char *string(size_t size){
  return calloc(size, 1);
}

json_object *msg_to_json(const char *msg, int id){
  json_object *msg_json = json_object_new_object();
  json_object *msg_id = json_object_new_int(id);
  json_object *msg_msg = json_object_new_string(msg);
  json_object *msg_owner = json_object_new_int(1);
  json_object_object_add(msg_json, "id", msg_id);
  json_object_object_add(msg_json, "owner", msg_owner);
  json_object_object_add(msg_json, "message", msg_msg);
  return msg_json;
}

int append_to_file(const char *filename, const char *buf){
  FILE *file = fopen(filename, "a");
  if(!file)
    return 1;
  fprintf(file, "%s", buf);
  fclose(file);
  return 0;
}

const char* save_msg(const char *msg, int id, json_object *json){
  json_object *msg_json = msg_to_json(msg, id);
  const char *buf = json_object_to_json_string_ext(msg_json, JSON_C_TO_STRING_PRETTY);
  json = msg_json;
  append_to_file(".chat", buf);
  return buf;
}

void send_msg(const char *msg, int id){
  time_t t = time(NULL);
  struct tm d = *localtime(&t);
  json_object *msg_json = NULL;
  const char *buf = save_msg(msg, id, msg_json);

  json_object_put(msg_json);
}

char **update_buffer(int *len){
  return read_file(len);
}

void main_window(CDKSCREEN *screen) {
  int len = 0;
  chat *chats = get_chats_list(&len);
  int width = 60;
  char **chats_list_ui = get_chat_ui(chats, len, width);
  char *s[5] = {"</B>pintosum<!B>  </D>Асамасмламла... </U>23:12<!U><!D>",
                "</B>stuartfi<!B>  </D>Hello darling! </U>12:43<!U><!D>",
                "Friend", "How", "Do you do"};
  CDKSCROLL *scroll =
      newCDKScroll(screen, LEFT, CENTER, NONE, 0, 60, "<C></B>Chats<!B>",
                   chats_list_ui, len, FALSE, A_NORMAL, TRUE, FALSE);
  // CDKSWINDOW *dialog = newCDKSwindow(screen, RIGHT, CENTER, 0, 40, "Dialog",
  // int, boolean, boolean)
  // CDKSELECTION *selection = newCDKSelection(screen, RIGHT, CENTER, 0, 50,
  // RIGHT, "Chat", char **, int, char **, int, chtype, boolean, boolean)

  CDKVIEWER *dialog = newCDKViewer(screen, RIGHT, TOP, 26, 75, NULL, 0, 0, TRUE, FALSE);
  char *d[2] = {".chat.txt", NULL};
  char **c = read_file(&len);
  setCDKViewer(dialog, "<C></B>Dialog<!B>", c, len, 0, TRUE, FALSE, TRUE);
  drawCDKViewer(dialog, TRUE);

  CDKMENTRY *entry =
      newCDKMentry(screen, RIGHT, BOTTOM, "</D>Message<!D>", "", A_NORMAL, ' ',
                   vMIXED, 73, 5, 1000, 1, TRUE, FALSE);

  setCDKButtonboxBoxAttribute(scroll, A_DIM);
  drawCDKButtonbox(scroll, TRUE);
  drawCDKMentry(entry, TRUE);
  while(1){
  activateCDKScroll(scroll, NULL);
  char *msg = activateCDKMentry(entry, NULL);
  send_msg(msg, 1);
  setCDKViewerInfo(dialog, update_buffer(&len), len, TRUE);
  activateCDKViewer(dialog, NULL);
  refreshCDKScreen(screen);
  }
  getch();
  eraseCDKButtonbox(scroll);
  destroyCDKButtonbox(scroll);
  refreshCDKScreen(screen);
}

int main() {
  CDKSCREEN *screen;
  WINDOW *win = initscr();
  screen = initCDKScreen(win);
  // login_menu(screen);
 // main_window(screen);
  eraseCDKScreen(screen);
  destroyCDKScreen(screen);
  endCDK();
  return 0;
}
