/*
 * panel.js -- PDP-11/34A front panel layout and audio configuration
 *
 * All coordinates are in pixels relative to panel.jpg (2006 x 1074).
 * The JavaScript UI converts them to CSS percentages at runtime so
 * the panel scales cleanly to any window width.
 *
 * To adjust button/LED positions: edit the x,y values below, reload.
 * Enable PANEL_DEBUG = true to show coordinate crosshairs on click.
 */

const PANEL_IMG_W = 2006;
const PANEL_IMG_H = 1074;
const PANEL_DEBUG = true; /* set true to click-calibrate positions */

/* ----------------------------------------------------------------
 * KEY_* enum — must stay in sync with console/console.h
 * ---------------------------------------------------------------- */
const KEY = {
  K0: 0,
  K1: 1,
  K2: 2,
  K3: 3,
  K4: 4,
  K5: 5,
  K6: 6,
  K7: 7,
  HALT_SS: 8,
  CONT: 9,
  BOOT: 10,
  START: 11,
  DEP: 12,
  EXAM: 13,
  INIT: 14,
  CLR: 15,
  DIS_AD: 16,
  LAD: 17,
  LSR: 18,
  CNTRL: 19,
};

/* ----------------------------------------------------------------
 * Power button overlay (button.png = ON state)
 * Positioned at pixel (1574, 778) in panel.jpg
 * ---------------------------------------------------------------- */
const POWER_BTN = { x: 1573, y: 779, w: 198, h: 153 };

/* ----------------------------------------------------------------
 * 7-segment display region — top-left (840,709) bottom-right (1072,736)
 * ---------------------------------------------------------------- */
const DISPLAY = { x: 836, y: 706, w: 240, h: 34 };

/* ----------------------------------------------------------------
 * LED indicators  { x, y = centre, r = radius in px }
 * Calibrated from GIMP on panel.jpg (2006x1074)
 * ---------------------------------------------------------------- */
const LEDS = {
  run: { x: 1149, y: 741, r: 8 },
  sr_disp: { x: 1151, y: 798, r: 8 },
  bus_err: { x: 1154, y: 857, r: 8 },
  maint: { x: 1156, y: 915, r: 8 },
  dc_on: { x: 1636, y: 740, r: 8 },
};

/* ----------------------------------------------------------------
 * Clickable buttons  { x, y = centre, w, h = hit area, key, label }
 * key = -1 for power-zone buttons handled separately
 * Calibrated from GIMP on panel.jpg (2006x1074)
 * ---------------------------------------------------------------- */
const BUTTONS = [
  /* -- Left function column -- */
  {
    id: "dis_ad",
    key: KEY.DIS_AD,
    x: 1269,
    y: 740,
    w: 50,
    h: 30,
    label: "DIS AD",
  },
  { id: "lad", key: KEY.LAD, x: 1274, y: 796, w: 50, h: 30, label: "LAD" },
  { id: "lsr", key: KEY.LSR, x: 1278, y: 856, w: 50, h: 30, label: "LSR" },
  { id: "clr", key: KEY.CLR, x: 1284, y: 916, w: 50, h: 30, label: "CLR" },

  /* -- Numeric keypad -- */
  { id: "k7", key: KEY.K7, x: 1331, y: 740, w: 50, h: 30, label: "7" },
  { id: "k4", key: KEY.K4, x: 1337, y: 796, w: 50, h: 30, label: "4" },
  { id: "k5", key: KEY.K5, x: 1398, y: 796, w: 50, h: 30, label: "5" },
  { id: "k6", key: KEY.K6, x: 1461, y: 796, w: 50, h: 30, label: "6" },
  { id: "k1", key: KEY.K1, x: 1342, y: 856, w: 50, h: 30, label: "1" },
  { id: "k2", key: KEY.K2, x: 1403, y: 856, w: 50, h: 30, label: "2" },
  { id: "k3", key: KEY.K3, x: 1466, y: 856, w: 50, h: 30, label: "3" },
  { id: "k0", key: KEY.K0, x: 1346, y: 916, w: 50, h: 30, label: "0" },

  /* -- Right function column -- */
  { id: "exam", key: KEY.EXAM, x: 1391, y: 740, w: 50, h: 30, label: "EXAM" },
  { id: "dep", key: KEY.DEP, x: 1451, y: 740, w: 50, h: 30, label: "DEP" },
  {
    id: "hlt_ss",
    key: KEY.HALT_SS,
    x: 1512,
    y: 740,
    w: 50,
    h: 30,
    label: "HLT/SS",
  },
  { id: "cont", key: KEY.CONT, x: 1519, y: 796, w: 50, h: 30, label: "CONT" },
  { id: "boot", key: KEY.BOOT, x: 1530, y: 856, w: 50, h: 30, label: "BOOT" },
  { id: "init", key: KEY.INIT, x: 1410, y: 916, w: 50, h: 30, label: "INIT" },
  {
    id: "cntrl",
    key: KEY.CNTRL,
    x: 1473,
    y: 916,
    w: 50,
    h: 30,
    label: "CNTRL",
  },
  {
    id: "start",
    key: KEY.START,
    x: 1537,
    y: 916,
    w: 50,
    h: 30,
    label: "START",
  },
];

/* ----------------------------------------------------------------
 * Audio configuration
 *
 * clicks.mp3  : multiple button click sounds end-to-end.
 *   segments[] : {start, end} in seconds for each individual click.
 *   One is chosen at random on each button press.
 *
 * run.mp3     : recording of power-on / running / power-off.
 *   intro_end  : seconds where intro ends and run-loop begins.
 *   loop_end   : seconds where run-loop ends and outro begins.
 *
 * Set the correct timestamps after opening the file in Audacity.
 * ---------------------------------------------------------------- */
const AUDIO = {
  keys: {
    file: "assets/keys.mp3",
    segments: [
      { start: 0.0, end: 0.05 } /* key click 1 */,
      { start: 0.506, end: 0.556 } /* key click 2 */,
      { start: 0.964, end: 1.014 } /* key click 3 */,
    ],
  },
  power: {
    file: "assets/run.mp3",
    skip: 0.55 /* skip initial silence before the "clac" */,
    intro_end: 6.1 /* 5 s — fin de l'intro, début du run en boucle */,
    loop_end: 9.3 /* 9.43 s — début de l'extinction */,
  },
};
