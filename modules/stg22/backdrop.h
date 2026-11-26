#ifndef STG22_BACKDROP_H
#define STG22_BACKDROP_H

#include <ninja.h>

/* the background's sky, light strip, sign panels and two pulses. a pulse is
 * four lamps, its object and that object's three siblings, which light in
 * turn. no code reads the roots, which place the pulses in the stage */
extern NJS_CNK_OBJECT gadget_sky_object;
extern NJS_CNK_OBJECT gadget_strip_object[1];
extern NJS_CNK_OBJECT gadget_sign_panel_00;
extern NJS_CNK_OBJECT gadget_sign_panel_01;
extern NJS_CNK_OBJECT gadget_sign_panel_02;
extern NJS_CNK_OBJECT gadget_sign_panel_03;
extern NJS_CNK_OBJECT gadget_sign_panel_04;
extern NJS_CNK_OBJECT gadget_sign_panel_05;
extern NJS_CNK_OBJECT gadget_sign_panel_06;
extern NJS_CNK_OBJECT gadget_sign_panel_07;
extern NJS_CNK_OBJECT gadget_sign_panel_08;
extern NJS_CNK_OBJECT gadget_sign_panel_09;
extern NJS_CNK_OBJECT gadget_sign_panel_10;
extern NJS_CNK_OBJECT gadget_sign_panel_11;
extern NJS_CNK_OBJECT gadget_sign_panel_12;
extern NJS_CNK_OBJECT gadget_sign_panel_13;
extern NJS_CNK_OBJECT gadget_sign_panel_14;
extern NJS_CNK_OBJECT gadget_sign_panel_15;
extern NJS_CNK_OBJECT gadget_sign_panel_16;
extern NJS_CNK_OBJECT gadget_sign_panel_17;
extern NJS_CNK_OBJECT gadget_sign_panel_18;
extern NJS_CNK_OBJECT gadget_sign_panel_19;
extern NJS_CNK_OBJECT gadget_sign_panel_20;
extern NJS_CNK_OBJECT gadget_sign_panel_21;
extern NJS_CNK_OBJECT gadget_pulse_a;
extern NJS_CNK_OBJECT gadget_pulse_a_root;
extern NJS_CNK_OBJECT gadget_pulse_b;
extern NJS_CNK_OBJECT gadget_pulse_b_root;

#endif
