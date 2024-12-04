#include "codin.h"

#define Time XTime

#include <X11/X.h>
#include <X11/Xlib.h>

#include "include/glad/glad.h"
#include "include/KHR/khrplatform.h"

#undef Time

Display *dpy;
Window root;
// GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
// XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
Window win;
// GLXContext glc;
XWindowAttributes gwa;
XEvent xev;

void DrawAQuad() {
  glClearColor(0, 1.0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glCreateVertexArrays(1, nil);
}

int main(Arg_Slice args) {
  dpy = XOpenDisplay(NULL);

  if (dpy == NULL) {
    fmt_printf(LIT("\n\tcannot connect to X server\n\n"));
    return 1;
  }

  root = DefaultRootWindow(dpy);

  // vi = glXChooseVisual(dpy, 0, att);

  // if (vi == NULL) {
  //   fmt_printf(LIT("\n\tno appropriate visual found\n\n"));
  //   return 2;
  // } else {
  //   fmt_printf(LIT("\n\tvisual %x selected\n"), (void *)vi->visualid);
  // }

  // cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask;

  // win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput,
  //                     vi->visual, CWColormap | CWEventMask, &swa);

  XMapWindow(dpy, win);
  XStoreName(dpy, win, "VERY SIMPLE APPLICATION");

  // glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
  // glXMakeCurrent(dpy, win, glc);

  gladLoadGL();
  glEnable(GL_DEPTH_TEST);

  while (1) {
    XNextEvent(dpy, &xev);

    if (xev.type == Expose) {
      XGetWindowAttributes(dpy, win, &gwa);
      glViewport(0, 0, gwa.width, gwa.height);
      DrawAQuad();
      // glXSwapBuffers(dpy, win);
    } else if (xev.type == KeyPress) {
      // glXMakeCurrent(dpy, None, NULL);
      // glXDestroyContext(dpy, glc);
      XDestroyWindow(dpy, win);
      XCloseDisplay(dpy);
      return 0;
    }
  }
}
