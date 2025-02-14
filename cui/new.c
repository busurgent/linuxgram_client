#include <cdk.h>
#include <cdk/cdkscreen.h>
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
      newCDKEntry(screen, CENTER, 14, "</B>Username<!B>", "", A_NORMAL, ' ',
                  vMIXED, 21, 6, 20, FALSE, FALSE);
  CDKENTRY *password_entry =
      newCDKEntry(screen, CENTER, 16, "</B>Password<!B>", "", A_NORMAL, ' ',
                  vHMIXED, 21, 6, 20, FALSE, FALSE);
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
    mvprintw(20, 55, "Wrong username or password");
    refresh();
  }
  destroyCDKEntry(login_entry);
  destroyCDKEntry(password_entry);
  // destroyCDKButton(button);
  mvprintw(20, 55, "                          ");
  move(0, 0);
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
  // WINDOW *win;
  screen = initCDKScreen(NULL);
  login_menu(screen);
  main_window(screen);
  destroyCDKScreen(screen);
  endCDK();
  return 0;
}
