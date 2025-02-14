#include <cdk.h>
#include <cdk/cdkscreen.h>
#include <cdk/entry.h>
#include <cdk/label.h>
#include <ncurses.h>
#include <string.h>

/*static int callback(EObjectType cdktype, void *obj, void *data, chtype key) {
  injectCDKButton((CDKBUTTON *)obj, SPACE);
  activateCDKEntry(data, NULL);
  return FALSE;
}*/

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
  /*CDKBUTTON *button =
      newCDKButton(screen, CENTER, 18, "Log in", NULL, FALSE, FALSE);
  bindCDKObject(vBUTTON, button, 9, callback, login_entry);
  setCDKButtonHighlight(button, FALSE);*/
  setCDKEntryHiddenChar(password_entry, '*');
  while (1) {
    drawCDKEntry(login_entry, TRUE);
    drawCDKEntry(password_entry, FALSE);
    // drawCDKButton(button, FALSE);
    char *login = activateCDKEntry(login_entry, NULL);
    char *pass = activateCDKEntry(password_entry, NULL);
    // int butt = activateCDKButton(button, NULL);
    refreshCDKScreen(screen);
    if (authorize(login, pass))
      break;
    drawCDKLabel(label, FALSE);
    refresh();
  }
  destroyCDKEntry(login_entry);
  destroyCDKEntry(password_entry);
  destroyCDKLabel(label);
  // destroyCDKButton(button);
}

void main_window(CDKSCREEN *screen) {
  char *s[3] = {"Hello", "My", "Friend"};
  CDKLABEL *label = newCDKLabel(screen, LEFT, TOP, s, 3, FALSE, FALSE);
  drawCDKLabel(label, TRUE);
  getch();
  destroyCDKLabel(label);
}

int main() {
  CDKSCREEN *screen;
  WINDOW *win = initscr();
  screen = initCDKScreen(win);
  login_menu(screen);
  main_window(screen);
  destroyCDKScreen(screen);
  endCDK();
  return 0;
}
