//================================================================
// QUADRUPED CRAWLING ROBOT — ESP32 + PCA9685 — VERSION XIX
//
// Bugs fixed in XIX:
//   - saveCalib() catch handler: the outer .catch() ignored its error
//     argument entirely, always showing "Connection error — is the bot on?"
//     regardless of whether the failure was a network drop or a firmware
//     400 rejection (e.g. servoMin >= servoMax guard). Fix: inspect the
//     error message — if it contains "HTTP" it is a firmware rejection and
//     a specific message including the param name is shown instead.
//   - loadConfig() fill bar: pct was computed as (val - mn) / (mx - mn)
//     without clamping val to [mn, mx] first. A value outside the slider's
//     visual range (e.g. servoMax set to 1200 via direct HTTP when the
//     slider min is 1500) produced a negative percentage, rendering the
//     fill bar broken. Fix: clamp val to [mn, mx] before the division.
//   - Changelog: XVIII and XIX entries were missing — version title read
//     XIX but the history only went to XVII.
//
// Bugs fixed in XVIII:
//   - makeSlider(): range inputs had no step attribute, so browsers
//     defaulted to step=1. For sliders with a non-integer min (spd-scale
//     min=0.1, spd-leg/body/turn/sit min=0.1) the snap-points were offset
//     from the min in integer steps (0.1, 1.1, 2.1 ...), making the
//     firmware default of 1.0x unreachable by dragging. Fix: step="any"
//     added to all generated range inputs.
//   - Angle slider range changed from -90..90 (default 0) to 0..180
//     (default 90), matching the firmware's native 0-180 degree range.
//     The +90 offset that was applied in sendCommand('servo','move',...)
//     to convert from the old range was removed — it would have sent
//     values 90..270, far outside the firmware's accepted 0..180 range.
//   - saveCalib() fetch calls: fetch() only rejects on network failure,
//     not on HTTP 4xx responses. The firmware's servoMin >= servoMax guard
//     returns HTTP 400 but the JS promise still resolved as success,
//     showing the green "saved" banner while the firmware had silently
//     reset the values to defaults. Fix: .then(r => { if (!r.ok) throw })
//     added to every fetch in saveCalib() so 400s propagate as rejections.
//   - loadConfig() offsets block: DOM inputs were populated from
//     d.offsets[] but the mirror offsets[][] JS array was not updated,
//     leaving it at all-zeros after a config load. Fix: offsets[l][j]
//     assigned alongside el.value in the same loop iteration.
//
// Bugs fixed in XVII:
//   - Changelog block reordered to newest-first (XVI, XV, XIV ... XI).
//     Previously XII was pinned at the top and newer entries were appended
//     below it, so the history read oldest-first — the opposite of convention.
//   - Version title corrected from XVI to XVII.
//
// Bugs fixed in XVI:
//   - Version title corrected from XIV to XV (XV file had wrong title again).
//     NOTE: the title line must be updated manually on every save — it is the
//     only line not covered by the changelog auto-entries.
//
// Bugs fixed in XV:
//   - Version title corrected from XIII to XIV (XIV file had wrong title).
//     To prevent recurrence, the title is now updated as part of every save.
//   - #speed-num CSS rule removed: no element with id="speed-num" exists
//     anywhere in the HTML. The rule was a leftover from an earlier design
//     where the speed multiplier was an editable input; it was replaced with
//     a read-only <span id="speed-mult"> but the dead CSS was never cleaned up.
//
// Bugs fixed in XIV:
//   - handleSetConfig(): XIII inserted the servoMin/servoMax guard as a
//     standalone if() in the middle of the else-if dispatch chain, making
//     every subsequent else-if (z_default, z_up ... off_*) the else-branch
//     of the guard instead of a continuation of the param chain. The code
//     happened to work because the guard always returns when true, but the
//     structure was semantically broken and fragile. Fix: move the guard to
//     after the complete else-if chain so all params dispatch correctly
//     regardless of whether the guard fires.
//   - Version title corrected from XII to XIII (XIII file had wrong title).
//
// Bugs fixed in XIII:
//   - handleSetConfig(): servoMin/servoMax cross-validation added. If the
//     user sets servoMin >= servoMax the map() call in angleToPulse() inverts,
//     sending corrupt pulse widths to every servo. Fix: reject the write and
//     reset both to safe defaults (500/2400) if the invariant is violated.
//   - loadConfig() JS: fill-bar origin for spd-leg/body/turn/sit corrected
//     from 0 to 0.1, matching the slider min raised in XII.
//   - rest_counter: declaration and increment removed. Inherited from Program2
//     for auto-sit-on-idle logic that was never ported. No code read it.
//   - t_a, t_b, t_alpha: moved from globals to locals inside
//     recomputeTurnConstants() where they are exclusively used.
//
// Bugs fixed in XII:
//   - Speed sliders (leg/body/turn/sit): min raised from 0 to 0.1 to match
//     the speed_multiple fix in XI — a zero value zeroes temp_speed[] and
//     deadlocks wait_reach() indefinitely, triggering a WDT reboot.
//   - handleSetConfig(): server-side floor clamp max(0.1f, val) added to all
//     five speed params so a direct HTTP request cannot bypass the slider min.
//   - gaitTask CMD_STOP / CMD_SIT: added currentCommand guard so a new
//     movement command arriving during the blocking stand()/sit() call is not
//     silently overwritten when the call returns.
//
// Converted from Sunfounder/RegisHsu crawl gait (Arduino Nano + Servo.h)
//
// Bugs fixed in XI:
//   - sendCommand() offset branch: the 'else' path still used the OLD
//     /leg(\d)-(alpha|beta|gamma)/ regex to parse target, which never
//     matches the 'off_L_J' format that setOffset() actually passes.
//     m was always null, the function returned without building a URL,
//     and every live keystroke in the offset inputs was silently dropped.
//     Fix: replace the dead regex with a direct passthrough — target is
//     already in firmware-ready 'off_L_J' format, send it straight to
//     /setconfig.  (saveCalib's direct fetch() calls were unaffected and
//     continued to work; only the live per-keystroke path was broken.)
//   - hand_wave() / hand_shake(): site_now[leg][0/1/2] was read in three
//     separate bare loads with no mutex, same race as the body_left/right
//     fix in X.  servo_service() on Core 0 could update one component
//     between reads, giving a torn restore-position and a brief jolt when
//     the wave/shake ends.  Fix: snapshot all three components under
//     speedMutex in one critical section, same pattern as body_left/right.
//     Applies to both branches of both hand_wave() and hand_shake() (4
//     sites total).
//   - Control pad speed slider range mismatch: syncSpeed(), sendCommand()
//     speed branch, and the showScreen() re-sync all hard-coded the OLD
//     0.3-1.8x range.  The firmware's actual range is 0.1-5.0x (matches
//     the calibration Speed Scale slider).  Any value saved above 1.8x
//     was silently overwritten as soon as the user touched the control
//     pad slider.  The re-sync on screen open also mis-displayed saved
//     values >1.8x (clamped to max).  Fix: all three JS locations now use
//     the 0.1-5.0 mapping; label markers and initial slider position
//     (47% -> 18%, which is 1.0x on the new scale) updated to match.
//   - loadConfig() spd-scale fill bar used min=0 instead of min=0.1,
//     causing the fill percentage to be computed against the wrong origin.
//     Fix: changed to [d.speed_multiple, 0.1, 5].
//   - #stop-btn CSS: 'border: none' was declared and then immediately
//     overridden two lines later by 'border: 1px solid ...', making the
//     first declaration dead code.  Removed the redundant declaration.
//   - servoMin / servoMax declared volatile: both are written by
//     handleSetConfig() on Core 1 and read by angleToPulse() ->
//     polar_to_servo() -> servo_service() on Core 0. Without volatile
//     the compiler is allowed to cache them in a Core 0 register,
//     meaning pulse-width changes from the calibration page could be
//     silently ignored until the next context switch (or forever).
//   - servoOffset[4][3] declared volatile: same cross-core issue —
//     written by handleSetConfig() on Core 1, read by polar_to_servo()
//     on Core 0. Without volatile, per-servo trim edits from the
//     calibration page may never be seen by the IK loop.
//   - body_left() / body_right(): site_now[leg][0] for all four legs
//     is now snapshotted in a single mutex-protected block before any
//     set_site() call. Previously, Core 0 could update one leg's
//     site_now between the four bare reads on Core 1, producing a torn
//     delta vector and a wrong temp_speed direction for that leg —
//     visible as a brief jerk at the start of every body shift.
//   - Speed scale slider minimum raised from 0 to 0.1: a value of 0 zeroed
//     out temp_speed[], causing site_now[] to never converge to site_expect[],
//     which deadlocked wait_reach() indefinitely and triggered the WDT reboot.
//   - saveCalib() speed display re-sync formula corrected: old formula assumed
//     speed_multiple in [0.3, 1.8], giving wrong % for any value above 1.8.
//     New formula matches the actual slider range [0.1, 5.0].
//   - setOffset() oninput now sends param as "off_L_J" (e.g. "off_0_2")
//     instead of "leg0-alpha" — the old format was silently ignored by
//     handleSetConfig(), making live per-servo offset updates do nothing.
//
// Changes in VII:
//   - Complete UI redesign: dark sci-fi aesthetic with animated SVG robot
//     on landing page, multi-screen navigation (Landing / Control / Calibration),
//     joystick with direction compass, glassmorphic calibration dashboard,
//     floating particles, all wired to the same ESP32 HTTP endpoints.
//   - Calibration sliders now load live values from /getconfig on screen open.
//   - Speed% slider on control pad maps to speed_multiple via /setconfig.
//
// Changes from original:
//   - Servo.h + FlexiTimer2  ->  Adafruit_PWMServoDriver (PCA9685)
//   - Timer interrupt         ->  FreeRTOS task on Core 0 (same 20ms period)
//   - Serial-only control     ->  WiFi hotspot + multi-page webpage
//   - All tunable values      ->  runtime variables, editable via Calibration page
//   - servoOffset[4][3]       ->  per-servo trim, applied in polar_to_servo()
//   - Manual motor test       ->  single servo override from calibration page
//
// Identical to original:
//   - All physical measurements (length_a/b/c, length_side)
//   - cartesian_to_polar() IK math
//   - polar_to_servo() angle mapping (plus offset on top)
//   - set_site() / wait_reach() / wait_all_reach() logic
//   - stand(), sit(), step_forward(), step_back()
//   - turn_left(), turn_right()
//   - hand_wave(), hand_shake(), body_left(), body_right()
//
// Bugs fixed vs VI (v6):
//   - polar_to_servo(): when a manual motor override is active, IK was still
//     writing the overridden channel on the same tick, causing the servo to
//     alternate between the IK angle and the manual angle every 20ms tick —
//     visible as a continuous twitch. Fix: skip setPWM for any channel that
//     equals manualChannel; the manual override write at the end of
//     servo_service remains the sole writer for that channel.
//
// Changes from VI (v6):
//   - Complete UI redesign: dark sci-fi aesthetic with animated landing page,
//     robot SVG silhouette, multi-screen navigation, joystick with compass,
//     and glassmorphic calibration dashboard.
//
// Bugs fixed vs V (v5):
//   - wait_reach(): exact float == replaced with fabsf epsilon comparison.
//     Exact equality is unreliable due to float precision and because site_now[]
//     is written on Core 0 without a lock; a partial read on Core 1 could cause
//     the condition to never fire, permanently freezing the gait task.
//   - step_forward() / step_back() branch condition: float == y_start -> fabsf
//   - turn_left() / turn_right() branch condition:   float == y_start -> fabsf
//   - hand_wave() / hand_shake() branch condition:   float == y_start -> fabsf
//
// Bugs fixed vs original (Arduino Nano version):
//   - Division by zero in set_site() when target == current position
//   - Race condition on temp_speed[][] between Core 0 and Core 1 (mutex)
//   - acos() domain error from float drift (constrain before every acos call)
//   - sqrt(v^2+z^2) denominator guard in cartesian_to_polar
//   - abs() on float replaced with fabsf() in servo_service
//================================================================

#include <math.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "webpage.h"     // ROBOT_HTML PROGMEM array — must live in same sketch folder

//================================================================
// SECTION 1: HOTSPOT SETTINGS
//================================================================
const char* AP_SSID     = "Monster bot";
const char* AP_PASSWORD = "mobot1234";

//================================================================
// SECTION 2: SERVO PULSE RANGE
// Tunable at runtime via the Calibration page.
// SERVO_FREQ is fixed at init and cannot change at runtime.
//================================================================
#define SERVO_FREQ 50
volatile int servoMin = 500;
volatile int servoMax = 2400;

//================================================================
// SECTION 3: PCA9685 CHANNEL MAPPING
//   joint 0 = alpha (femur / upper leg)
//   joint 1 = beta  (tibia / lower leg)
//   joint 2 = gamma (coxa  / hip)
//================================================================
const int servo_pin[4][3] = {
  { 0,  1,  2},   // Leg 0 - Front-Left
  { 3,  4,  5},   // Leg 1 - Front-Right
  { 6,  7,  8},   // Leg 2 - Back-Left
  { 9, 10, 11},   // Leg 3 - Back-Right
};

//================================================================
// SECTION 4: PER-SERVO CALIBRATION OFFSETS
// servoOffset[leg][joint] added after IK mapping.
// Leg 0=FL, 1=FR, 2=BL, 3=BR | Joint 0=alpha, 1=beta, 2=gamma
//================================================================
volatile int servoOffset[4][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
};

//================================================================
// SECTION 5: ROBOT DIMENSIONS — identical to original (mm)
//================================================================
const float length_a    = 55.0f;
const float length_b    = 77.5f;
const float length_c    = 27.5f;
const float length_side = 71.0f;

//================================================================
// SECTION 6: TUNABLE MOVEMENT PARAMETERS
//================================================================
float z_default        = -50.0f;
float z_up             = -30.0f;
float z_boot           = -28.0f;
float x_default        =  62.0f;
float y_step           =  40.0f;
float spot_turn_speed  =   4.0f;
float leg_move_speed   =   8.0f;
float body_move_speed  =   3.0f;
float stand_seat_speed =   1.0f;
float speed_multiple   =   1.0f;

//================================================================
// SECTION 7: DERIVED TURN GEOMETRY
// Call recomputeTurnConstants() after changing x_default or y_step.
//================================================================
const float y_start  = 0.0f;
const float x_offset = 0.0f;
const float pi       = 3.14159265f;
const float KEEP     = 255.0f;

float turn_x0, turn_y0, turn_x1, turn_y1;

void recomputeTurnConstants() {
  float t_a = sqrtf(powf(2.0f*x_default + length_side, 2.0f) + powf(y_step, 2.0f));
  float t_b = 2.0f*(y_start + y_step) + length_side;
  float t_c = sqrtf(powf(2.0f*x_default + length_side, 2.0f) +
                    powf(2.0f*y_start + y_step + length_side, 2.0f));
  float arg = constrain((t_a*t_a + t_b*t_b - t_c*t_c) / (2.0f*t_a*t_b), -1.0f, 1.0f);
  float t_alpha = acosf(arg);
  turn_x1 = (t_a - length_side) / 2.0f;
  turn_y1 = y_start + y_step / 2.0f;
  turn_x0 = turn_x1 - t_b * cosf(t_alpha);
  turn_y0 = t_b * sinf(t_alpha) - turn_y1 - length_side;
}

//================================================================
// MOVEMENT STATE
//================================================================
volatile float site_now[4][3];
volatile float site_expect[4][3];
float temp_speed[4][3];
float move_speed;

//================================================================
// MANUAL MOTOR OVERRIDE
// Set by /movemotor — servo_service applies this after IK each tick.
// manualChannel = -1 means no override active.
//================================================================
volatile int manualChannel = -1;
volatile int manualAngle   = 90;

//================================================================
// HARDWARE
//================================================================
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);
WebServer server(80);
DNSServer dnsServer;
Preferences prefs;   // NVS namespace "quadbot" — persists calibration across reboots

//================================================================
// STATE MACHINE
//================================================================
enum CommandState {
  CMD_IDLE, CMD_STOP,
  CMD_FORWARD, CMD_BACKWARD,
  CMD_LEFT, CMD_RIGHT,
  CMD_SIT
};
volatile CommandState currentCommand = CMD_IDLE;
SemaphoreHandle_t speedMutex;

// Set by handleSaveCalib() on Core 1; consumed by gaitTask idle branch
// so the blocking NVS write never interrupts an active movement sequence.
volatile bool pendingSave = false;

//================================================================
// NVS CALIBRATION PERSISTENCE
// loadCalib() — call once in setup() before first use.
// saveToNVS() — called by handleSaveCalib() when the user clicks
//               "Save All & Preview on Robot" in the web UI.
//               NOT called on every /setconfig so that live slider
//               dragging stays fast and NVS wear is minimised.
//================================================================
void loadCalib() {
  if (!prefs.begin("quadbot", /*readOnly=*/true)) {
    // NVS namespace missing or partition corrupt — firmware defaults remain in place.
    Serial.println("NVS open failed — using firmware defaults.");
    return;
  }
  servoMin         = prefs.getInt(  "servoMin",  500);
  servoMax         = prefs.getInt(  "servoMax",  2400);
  z_default        = prefs.getFloat("z_def",    -50.0f);
  z_up             = prefs.getFloat("z_up",     -30.0f);
  z_boot           = prefs.getFloat("z_boot",   -28.0f);
  x_default        = prefs.getFloat("x_def",     62.0f);
  y_step           = prefs.getFloat("y_step",    40.0f);
  speed_multiple   = prefs.getFloat("spd_mul",    1.0f);
  leg_move_speed   = prefs.getFloat("spd_leg",    8.0f);
  body_move_speed  = prefs.getFloat("spd_body",   3.0f);
  spot_turn_speed  = prefs.getFloat("spd_turn",   4.0f);
  stand_seat_speed = prefs.getFloat("spd_sit",    1.0f);
  // Read 12 servo offsets as a single blob.
  int tmp[4][3];
  if (prefs.getBytes("offsets", tmp, sizeof(tmp)) == sizeof(tmp)) {
    for (int l = 0; l < 4; l++)
      for (int j = 0; j < 3; j++)
        servoOffset[l][j] = tmp[l][j];
  }
  prefs.end();
  Serial.println("Calibration loaded from NVS.");
}

void saveToNVS() {
  if (!prefs.begin("quadbot", /*readOnly=*/false)) {
    // NVS partition full or corrupt — values NOT persisted.
    Serial.println("NVS open failed — calibration NOT saved.");
    return;
  }
  prefs.putInt(  "servoMin",  servoMin);
  prefs.putInt(  "servoMax",  servoMax);
  prefs.putFloat("z_def",     z_default);
  prefs.putFloat("z_up",      z_up);
  prefs.putFloat("z_boot",    z_boot);
  prefs.putFloat("x_def",     x_default);
  prefs.putFloat("y_step",    y_step);
  prefs.putFloat("spd_mul",   speed_multiple);
  prefs.putFloat("spd_leg",   leg_move_speed);
  prefs.putFloat("spd_body",  body_move_speed);
  prefs.putFloat("spd_turn",  spot_turn_speed);
  prefs.putFloat("spd_sit",   stand_seat_speed);
  int tmp[4][3];
  for (int l = 0; l < 4; l++)
    for (int j = 0; j < 3; j++)
      tmp[l][j] = servoOffset[l][j];
  prefs.putBytes("offsets",   tmp, sizeof(tmp));
  prefs.end();
  Serial.println("Calibration saved to NVS.");
}

//================================================================
// HELPER: degrees -> PCA9685 pulse ticks
// MG90S servo: 500 µs (0°) ... 2400 µs (180°) at 50 Hz
// Ticks = pulse_µs × freq × 4096 / 1,000,000
// Max value: 2400 × 50 × 4096 / 1,000,000 = 491 ticks — no overflow
//================================================================
int angleToPulse(int angle) {
  int  pulse_us = map(angle, 0, 180, servoMin, servoMax);
  long ticks    = (long)pulse_us * (long)SERVO_FREQ * 4096L / 1000000L;
  return (int)ticks;
}

//================================================================
// CARTESIAN -> POLAR
//================================================================
void cartesian_to_polar(volatile float &alpha, volatile float &beta,
                        volatile float &gamma,
                        volatile float x, volatile float y, volatile float z) {
  float v, w;
  w = (x >= 0.0f ? 1.0f : -1.0f) * sqrtf(x*x + y*y);
  v = w - length_c;
  float denom1 = sqrtf(v*v + z*z);
  float arg1   = (denom1 < 1e-6f) ? 0.0f :
                 constrain((length_a*length_a - length_b*length_b + v*v + z*z)
                           / (2.0f * length_a * denom1), -1.0f, 1.0f);
  alpha = atan2f(z, v) + acosf(arg1);
  float arg2 = constrain((length_a*length_a + length_b*length_b - v*v - z*z)
                         / (2.0f * length_a * length_b), -1.0f, 1.0f);
  beta  = acosf(arg2);
  gamma = (w >= 0.0f) ? atan2f(y, x) : atan2f(-y, -x);
  alpha = alpha / pi * 180.0f;
  beta  = beta  / pi * 180.0f;
  gamma = gamma / pi * 180.0f;
}

//================================================================
// POLAR -> SERVO
//================================================================
void polar_to_servo(int leg, float alpha, float beta, float gamma) {
  if (leg == 0) {
    alpha  = 90.0f - alpha;
    gamma += 90.0f;
  } else if (leg == 1) {
    alpha += 90.0f;
    beta   = 180.0f - beta;
    gamma  = 90.0f - gamma;
  } else if (leg == 2) {
    alpha += 90.0f;
    beta   = 180.0f - beta;
    gamma  = 90.0f - gamma;
  } else {
    alpha  = 90.0f - alpha;
    gamma += 90.0f;
  }
  alpha += (float)servoOffset[leg][0];
  beta  += (float)servoOffset[leg][1];
  gamma += (float)servoOffset[leg][2];
  alpha  = constrain(alpha, 0.0f, 180.0f);
  beta   = constrain(beta,  0.0f, 180.0f);
  gamma  = constrain(gamma, 0.0f, 180.0f);
  // Skip any channel that is under manual override — writing it here would
  // cause it to alternate between the IK angle and the manual angle every
  // tick, producing a visible twitch. The manual override write at the end
  // of servo_service is the sole writer for that channel while active.
  // roundf() before the int cast: plain (int) truncates toward zero, which
  // introduces a systematic up-to-1 degree low bias on every servo (e.g.
  // 89.9 degrees becomes 89).  Rounding gives the nearest integer instead.
  if (servo_pin[leg][0] != manualChannel)
    pca9685.setPWM(servo_pin[leg][0], 0, angleToPulse((int)roundf(alpha)));
  if (servo_pin[leg][1] != manualChannel)
    pca9685.setPWM(servo_pin[leg][1], 0, angleToPulse((int)roundf(beta)));
  if (servo_pin[leg][2] != manualChannel)
    pca9685.setPWM(servo_pin[leg][2], 0, angleToPulse((int)roundf(gamma)));
}

//================================================================
// SERVO SERVICE — Core 0, every 20ms
//================================================================
void servo_service(void) {
  static float alpha, beta, gamma;
  float speed_snap[4][3];
  // BUG FIX (Bug 2): also snapshot site_expect under the same mutex that
  // protects temp_speed.  Previously site_expect was read directly (without
  // any lock) while set_site() on Core 1 could be writing it — a classic
  // torn-read race on ESP32 dual-core.  Snapshotting both arrays in one
  // critical section guarantees that every (speed, target) pair used in this
  // tick is internally consistent and was written atomically by set_site().
  float expect_snap[4][3];
  // Timeout pdMS_TO_TICKS(1) instead of 0: a timeout of 0 means "don't wait
  // at all" — if set_site() or handleClearMotor() on Core 1 happens to hold
  // the mutex at this exact moment, the entire 20ms servo tick is silently
  // dropped (no IK, no PCA9685 writes).  set_site() only holds the mutex for
  // a few float operations (sub-microsecond at 240 MHz), so 1ms is virtually
  // guaranteed to succeed while still being safe inside a FreeRTOS task.
  if (xSemaphoreTake(speedMutex, pdMS_TO_TICKS(1)) == pdTRUE) {
    memcpy(speed_snap,  temp_speed,         sizeof(temp_speed));
    memcpy(expect_snap, (void*)site_expect, sizeof(site_expect));
    xSemaphoreGive(speedMutex);
  } else {
    return;
  }
  for (int i = 0; i < 4; i++) {
    // If this leg contains the manually-overridden channel, freeze site_now
    // so IK produces the same angle every tick and doesn't fight the override.
    bool legFrozen = (manualChannel >= 0 &&
                      (servo_pin[i][0] == manualChannel ||
                       servo_pin[i][1] == manualChannel ||
                       servo_pin[i][2] == manualChannel));
    if (!legFrozen) {
      for (int j = 0; j < 3; j++) {
        if (fabsf(site_now[i][j] - expect_snap[i][j]) >= fabsf(speed_snap[i][j]))
          site_now[i][j] += speed_snap[i][j];
        else
          site_now[i][j] = expect_snap[i][j];
      }
    }
    cartesian_to_polar(alpha, beta, gamma,
                       site_now[i][0], site_now[i][1], site_now[i][2]);
    polar_to_servo(i, alpha, beta, gamma);
  }
  // Manual single-servo override: applied after IK so it always wins.
  // The leg is frozen above so IK is stable — no oscillation.
  if (manualChannel >= 0) {
    pca9685.setPWM(manualChannel, 0, angleToPulse(manualAngle));
  }
  // rest_counter removed — was inherited from Program2 for auto-sit-on-idle
  // logic that was never ported to this version. No code reads it.
}

void servoTask(void* pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    servo_service();
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20));
  }
}

//================================================================
// SET SITE
//================================================================
void set_site(int leg, float x, float y, float z) {
  // Acquire the mutex FIRST — before reading site_now.
  // site_now is written by servo_service() on Core 0 continuously.  If we
  // read the three site_now components before taking the mutex, Core 0 could
  // update one of them between reads, giving us a torn delta vector and a
  // wrong temp_speed direction for that tick.
  //
  // Note: servo_service() writes site_now *outside* this same mutex (it only
  // holds the mutex briefly to snapshot temp_speed/site_expect, then releases
  // before touching site_now).  So this mutex does not give us a hard
  // guarantee against a torn site_now read in every possible schedule.
  // However it eliminates the most common interleaving — another call to
  // set_site() updating the same leg's site_expect between our read and our
  // write — and is consistent with every other shared-data access in this
  // file.  The one-tick positional error that could still theoretically occur
  // due to servo_service() writing site_now concurrently is self-correcting
  // by the next servo_service() tick.
  xSemaphoreTake(speedMutex, portMAX_DELAY);
  float lx = (x != KEEP) ? x - site_now[leg][0] : 0.0f;
  float ly = (y != KEEP) ? y - site_now[leg][1] : 0.0f;
  float lz = (z != KEEP) ? z - site_now[leg][2] : 0.0f;
  float length = sqrtf(lx*lx + ly*ly + lz*lz);
  if (length < 1e-6f) {
    temp_speed[leg][0] = 0.0f;
    temp_speed[leg][1] = 0.0f;
    temp_speed[leg][2] = 0.0f;
  } else {
    temp_speed[leg][0] = lx / length * move_speed * speed_multiple;
    temp_speed[leg][1] = ly / length * move_speed * speed_multiple;
    temp_speed[leg][2] = lz / length * move_speed * speed_multiple;
  }
  if (x != KEEP) site_expect[leg][0] = x;
  if (y != KEEP) site_expect[leg][1] = y;
  if (z != KEEP) site_expect[leg][2] = z;
  xSemaphoreGive(speedMutex);
}

//================================================================
// WAIT FUNCTIONS
//================================================================
void wait_reach(int leg) {
  // Snapshot site_expect under the mutex before entering the poll loop.
  // site_expect is written by set_site() on Core 1 under speedMutex; reading
  // it here without the mutex would allow a torn read on ESP32 dual-core
  // (set_site() writes three floats sequentially — Core 0 could observe a
  // mix of old and new values between reads).  We snapshot once and compare
  // against the snapshot so we always check against a complete, consistent
  // target.  If set_site() is called again mid-wait with a new target the
  // snapshot becomes stale, but wait_reach() will simply exit ~one tick early
  // (fabsf < 0.01) and the caller's next wait_all_reach() will re-block on
  // the new target — a safe, self-correcting outcome.
  float expect_snap[3];
  xSemaphoreTake(speedMutex, portMAX_DELAY);
  expect_snap[0] = site_expect[leg][0];
  expect_snap[1] = site_expect[leg][1];
  expect_snap[2] = site_expect[leg][2];
  xSemaphoreGive(speedMutex);

  while (1) {
    server.handleClient();
    dnsServer.processNextRequest();
    if (fabsf(site_now[leg][0] - expect_snap[0]) < 0.01f &&
        fabsf(site_now[leg][1] - expect_snap[1]) < 0.01f &&
        fabsf(site_now[leg][2] - expect_snap[2]) < 0.01f)
      break;
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
void wait_all_reach(void) {
  for (int i = 0; i < 4; i++) wait_reach(i);
}

//================================================================
// STAND / SIT
//================================================================
void sit(void) {
  move_speed = stand_seat_speed;
  for (int leg = 0; leg < 4; leg++) set_site(leg, KEEP, KEEP, z_boot);
  wait_all_reach();
}
void stand(void) {
  move_speed = stand_seat_speed;
  for (int leg = 0; leg < 4; leg++) set_site(leg, KEEP, KEEP, z_default);
  wait_all_reach();
}

//================================================================
// BODY SHIFT
//================================================================
void body_left(int i) {
  // Snapshot site_now[leg][0] for all 4 legs in one critical section so
  // every x-component is read from the same servo_service() tick.
  // Without this, Core 0 could update one leg's site_now between reads,
  // giving a torn delta and a wrong speed vector for that leg.
  float x0, x1, x2, x3;
  xSemaphoreTake(speedMutex, portMAX_DELAY);
  x0 = site_now[0][0];
  x1 = site_now[1][0];
  x2 = site_now[2][0];
  x3 = site_now[3][0];
  xSemaphoreGive(speedMutex);
  set_site(0, x0+i, KEEP, KEEP);
  set_site(1, x1+i, KEEP, KEEP);
  set_site(2, x2-i, KEEP, KEEP);
  set_site(3, x3-i, KEEP, KEEP);
  wait_all_reach();
}
void body_right(int i) {
  float x0, x1, x2, x3;
  xSemaphoreTake(speedMutex, portMAX_DELAY);
  x0 = site_now[0][0];
  x1 = site_now[1][0];
  x2 = site_now[2][0];
  x3 = site_now[3][0];
  xSemaphoreGive(speedMutex);
  set_site(0, x0-i, KEEP, KEEP);
  set_site(1, x1-i, KEEP, KEEP);
  set_site(2, x2+i, KEEP, KEEP);
  set_site(3, x3+i, KEEP, KEEP);
  wait_all_reach();
}

//================================================================
// STEP FORWARD
//================================================================
void step_forward(unsigned int step) {
  move_speed = leg_move_speed;
  while (step-- > 0) {
    if (fabsf(site_now[2][1] - y_start) < 0.01f) {
      set_site(2, x_default+x_offset, y_start,          z_up);      wait_all_reach();
      set_site(2, x_default+x_offset, y_start+2*y_step, z_up);      wait_all_reach();
      set_site(2, x_default+x_offset, y_start+2*y_step, z_default); wait_all_reach();
      move_speed = body_move_speed;
      set_site(0, x_default+x_offset, y_start,          z_default);
      set_site(1, x_default+x_offset, y_start+2*y_step, z_default);
      set_site(2, x_default-x_offset, y_start+y_step,   z_default);
      set_site(3, x_default-x_offset, y_start+y_step,   z_default);
      wait_all_reach();
      move_speed = leg_move_speed;
      set_site(1, x_default+x_offset, y_start+2*y_step, z_up);      wait_all_reach();
      set_site(1, x_default+x_offset, y_start,          z_up);      wait_all_reach();
      set_site(1, x_default+x_offset, y_start,          z_default); wait_all_reach();
    } else {
      set_site(0, x_default+x_offset, y_start,          z_up);      wait_all_reach();
      set_site(0, x_default+x_offset, y_start+2*y_step, z_up);      wait_all_reach();
      set_site(0, x_default+x_offset, y_start+2*y_step, z_default); wait_all_reach();
      move_speed = body_move_speed;
      set_site(0, x_default-x_offset, y_start+y_step,   z_default);
      set_site(1, x_default-x_offset, y_start+y_step,   z_default);
      set_site(2, x_default+x_offset, y_start,          z_default);
      set_site(3, x_default+x_offset, y_start+2*y_step, z_default);
      wait_all_reach();
      move_speed = leg_move_speed;
      set_site(3, x_default+x_offset, y_start+2*y_step, z_up);      wait_all_reach();
      set_site(3, x_default+x_offset, y_start,          z_up);      wait_all_reach();
      set_site(3, x_default+x_offset, y_start,          z_default); wait_all_reach();
    }
  }
}

//================================================================
// STEP BACK
//================================================================
void step_back(unsigned int step) {
  move_speed = leg_move_speed;
  while (step-- > 0) {
    if (fabsf(site_now[3][1] - y_start) < 0.01f) {
      set_site(3, x_default+x_offset, y_start,          z_up);      wait_all_reach();
      set_site(3, x_default+x_offset, y_start+2*y_step, z_up);      wait_all_reach();
      set_site(3, x_default+x_offset, y_start+2*y_step, z_default); wait_all_reach();
      move_speed = body_move_speed;
      set_site(0, x_default+x_offset, y_start+2*y_step, z_default);
      set_site(1, x_default+x_offset, y_start,          z_default);
      set_site(2, x_default-x_offset, y_start+y_step,   z_default);
      set_site(3, x_default-x_offset, y_start+y_step,   z_default);
      wait_all_reach();
      move_speed = leg_move_speed;
      set_site(0, x_default+x_offset, y_start+2*y_step, z_up);      wait_all_reach();
      set_site(0, x_default+x_offset, y_start,          z_up);      wait_all_reach();
      set_site(0, x_default+x_offset, y_start,          z_default); wait_all_reach();
    } else {
      set_site(1, x_default+x_offset, y_start,          z_up);      wait_all_reach();
      set_site(1, x_default+x_offset, y_start+2*y_step, z_up);      wait_all_reach();
      set_site(1, x_default+x_offset, y_start+2*y_step, z_default); wait_all_reach();
      move_speed = body_move_speed;
      set_site(0, x_default-x_offset, y_start+y_step,   z_default);
      set_site(1, x_default-x_offset, y_start+y_step,   z_default);
      set_site(2, x_default+x_offset, y_start+2*y_step, z_default);
      set_site(3, x_default+x_offset, y_start,          z_default);
      wait_all_reach();
      move_speed = leg_move_speed;
      set_site(2, x_default+x_offset, y_start+2*y_step, z_up);      wait_all_reach();
      set_site(2, x_default+x_offset, y_start,          z_up);      wait_all_reach();
      set_site(2, x_default+x_offset, y_start,          z_default); wait_all_reach();
    }
  }
}

//================================================================
// TURN LEFT
//================================================================
void turn_left(unsigned int step) {
  move_speed = spot_turn_speed;
  while (step-- > 0) {
    if (fabsf(site_now[3][1] - y_start) < 0.01f) {
      set_site(3, x_default+x_offset, y_start, z_up); wait_all_reach();
      set_site(0, turn_x1-x_offset, turn_y1, z_default);
      set_site(1, turn_x0-x_offset, turn_y0, z_default);
      set_site(2, turn_x1+x_offset, turn_y1, z_default);
      set_site(3, turn_x0+x_offset, turn_y0, z_up);
      wait_all_reach();
      set_site(3, turn_x0+x_offset, turn_y0, z_default); wait_all_reach();
      set_site(0, turn_x1+x_offset, turn_y1, z_default);
      set_site(1, turn_x0+x_offset, turn_y0, z_default);
      set_site(2, turn_x1-x_offset, turn_y1, z_default);
      set_site(3, turn_x0-x_offset, turn_y0, z_default);
      wait_all_reach();
      set_site(1, turn_x0+x_offset, turn_y0, z_up); wait_all_reach();
      set_site(0, x_default+x_offset, y_start,        z_default);
      set_site(1, x_default+x_offset, y_start,        z_up);
      set_site(2, x_default-x_offset, y_start+y_step, z_default);
      set_site(3, x_default-x_offset, y_start+y_step, z_default);
      wait_all_reach();
      set_site(1, x_default+x_offset, y_start, z_default); wait_all_reach();
    } else {
      set_site(0, x_default+x_offset, y_start, z_up); wait_all_reach();
      set_site(0, turn_x0+x_offset, turn_y0, z_up);
      set_site(1, turn_x1+x_offset, turn_y1, z_default);
      set_site(2, turn_x0-x_offset, turn_y0, z_default);
      set_site(3, turn_x1-x_offset, turn_y1, z_default);
      wait_all_reach();
      set_site(0, turn_x0+x_offset, turn_y0, z_default); wait_all_reach();
      set_site(0, turn_x0-x_offset, turn_y0, z_default);
      set_site(1, turn_x1-x_offset, turn_y1, z_default);
      set_site(2, turn_x0+x_offset, turn_y0, z_default);
      set_site(3, turn_x1+x_offset, turn_y1, z_default);
      wait_all_reach();
      set_site(2, turn_x0+x_offset, turn_y0, z_up); wait_all_reach();
      set_site(0, x_default-x_offset, y_start+y_step, z_default);
      set_site(1, x_default-x_offset, y_start+y_step, z_default);
      set_site(2, x_default+x_offset, y_start,        z_up);
      set_site(3, x_default+x_offset, y_start,        z_default);
      wait_all_reach();
      set_site(2, x_default+x_offset, y_start, z_default); wait_all_reach();
    }
  }
}

//================================================================
// TURN RIGHT
//================================================================
void turn_right(unsigned int step) {
  move_speed = spot_turn_speed;
  while (step-- > 0) {
    if (fabsf(site_now[2][1] - y_start) < 0.01f) {
      set_site(2, x_default+x_offset, y_start, z_up); wait_all_reach();
      set_site(0, turn_x0-x_offset, turn_y0, z_default);
      set_site(1, turn_x1-x_offset, turn_y1, z_default);
      set_site(2, turn_x0+x_offset, turn_y0, z_up);
      set_site(3, turn_x1+x_offset, turn_y1, z_default);
      wait_all_reach();
      set_site(2, turn_x0+x_offset, turn_y0, z_default); wait_all_reach();
      set_site(0, turn_x0+x_offset, turn_y0, z_default);
      set_site(1, turn_x1+x_offset, turn_y1, z_default);
      set_site(2, turn_x0-x_offset, turn_y0, z_default);
      set_site(3, turn_x1-x_offset, turn_y1, z_default);
      wait_all_reach();
      set_site(0, turn_x0+x_offset, turn_y0, z_up); wait_all_reach();
      set_site(0, x_default+x_offset, y_start,        z_up);
      set_site(1, x_default+x_offset, y_start,        z_default);
      set_site(2, x_default-x_offset, y_start+y_step, z_default);
      set_site(3, x_default-x_offset, y_start+y_step, z_default);
      wait_all_reach();
      set_site(0, x_default+x_offset, y_start, z_default); wait_all_reach();
    } else {
      set_site(1, x_default+x_offset, y_start, z_up); wait_all_reach();
      set_site(0, turn_x1+x_offset, turn_y1, z_default);
      set_site(1, turn_x0+x_offset, turn_y0, z_up);
      set_site(2, turn_x1-x_offset, turn_y1, z_default);
      set_site(3, turn_x0-x_offset, turn_y0, z_default);
      wait_all_reach();
      set_site(1, turn_x0+x_offset, turn_y0, z_default); wait_all_reach();
      set_site(0, turn_x1-x_offset, turn_y1, z_default);
      set_site(1, turn_x0-x_offset, turn_y0, z_default);
      set_site(2, turn_x1+x_offset, turn_y1, z_default);
      set_site(3, turn_x0+x_offset, turn_y0, z_default);
      wait_all_reach();
      set_site(3, turn_x0+x_offset, turn_y0, z_up); wait_all_reach();
      set_site(0, x_default-x_offset, y_start+y_step, z_default);
      set_site(1, x_default-x_offset, y_start+y_step, z_default);
      set_site(2, x_default+x_offset, y_start,        z_default);
      set_site(3, x_default+x_offset, y_start,        z_up);
      wait_all_reach();
      set_site(3, x_default+x_offset, y_start, z_default); wait_all_reach();
    }
  }
}

//================================================================
// HAND WAVE / HAND SHAKE
//================================================================
void hand_wave(int i) {
  float x_tmp, y_tmp, z_tmp;
  move_speed = 1;
  if (fabsf(site_now[3][1] - y_start) < 0.01f) {
    body_right(15);
    // Snapshot all three components in one critical section — same pattern as
    // body_left/right — to avoid a torn read if servo_service() updates one
    // component between bare sequential reads on Core 1.
    xSemaphoreTake(speedMutex, portMAX_DELAY);
    x_tmp = site_now[2][0]; y_tmp = site_now[2][1]; z_tmp = site_now[2][2];
    xSemaphoreGive(speedMutex);
    move_speed = body_move_speed;
    for (int j = 0; j < i; j++) {
      set_site(2, turn_x1, turn_y1, 50); wait_all_reach();
      set_site(2, turn_x0, turn_y0, 50); wait_all_reach();
    }
    set_site(2, x_tmp, y_tmp, z_tmp); wait_all_reach();
    move_speed = 1; body_left(15);
  } else {
    body_left(15);
    xSemaphoreTake(speedMutex, portMAX_DELAY);
    x_tmp = site_now[0][0]; y_tmp = site_now[0][1]; z_tmp = site_now[0][2];
    xSemaphoreGive(speedMutex);
    move_speed = body_move_speed;
    for (int j = 0; j < i; j++) {
      set_site(0, turn_x1, turn_y1, 50); wait_all_reach();
      set_site(0, turn_x0, turn_y0, 50); wait_all_reach();
    }
    set_site(0, x_tmp, y_tmp, z_tmp); wait_all_reach();
    move_speed = 1; body_right(15);
  }
}

void hand_shake(int i) {
  float x_tmp, y_tmp, z_tmp;
  move_speed = 1;
  if (fabsf(site_now[3][1] - y_start) < 0.01f) {
    body_right(15);
    xSemaphoreTake(speedMutex, portMAX_DELAY);
    x_tmp = site_now[2][0]; y_tmp = site_now[2][1]; z_tmp = site_now[2][2];
    xSemaphoreGive(speedMutex);
    move_speed = body_move_speed;
    for (int j = 0; j < i; j++) {
      set_site(2, x_default-30, y_start+2*y_step, 55); wait_all_reach();
      set_site(2, x_default-30, y_start+2*y_step, 10); wait_all_reach();
    }
    set_site(2, x_tmp, y_tmp, z_tmp); wait_all_reach();
    move_speed = 1; body_left(15);
  } else {
    body_left(15);
    xSemaphoreTake(speedMutex, portMAX_DELAY);
    x_tmp = site_now[0][0]; y_tmp = site_now[0][1]; z_tmp = site_now[0][2];
    xSemaphoreGive(speedMutex);
    move_speed = body_move_speed;
    for (int j = 0; j < i; j++) {
      set_site(0, x_default-30, y_start+2*y_step, 55); wait_all_reach();
      set_site(0, x_default-30, y_start+2*y_step, 10); wait_all_reach();
    }
    set_site(0, x_tmp, y_tmp, z_tmp); wait_all_reach();
    move_speed = 1; body_right(15);
  }
}

//================================================================
// CONFIG API
//   GET /getconfig               -> JSON of all tunable values
//   GET /setconfig?param=X&val=Y -> update one value, return OK
//   GET /movemotor?ch=X&angle=Y  -> single servo override for testing
//   GET /clearmotor              -> release manual override
//   GET /applystand              -> move robot to stand pose
//   GET /applysit                -> move robot to sit pose
//================================================================
void handleGetConfig() {
  String json = "{";
  json += "\"servoMin\":"          + String(servoMin)         + ",";
  json += "\"servoMax\":"          + String(servoMax)         + ",";
  json += "\"z_default\":"         + String(z_default)        + ",";
  json += "\"z_up\":"              + String(z_up)             + ",";
  json += "\"z_boot\":"            + String(z_boot)           + ",";
  json += "\"x_default\":"         + String(x_default)        + ",";
  json += "\"y_step\":"            + String(y_step)           + ",";
  json += "\"speed_multiple\":"    + String(speed_multiple)   + ",";
  json += "\"leg_move_speed\":"    + String(leg_move_speed)   + ",";
  json += "\"body_move_speed\":"   + String(body_move_speed)  + ",";
  json += "\"spot_turn_speed\":"   + String(spot_turn_speed)  + ",";
  json += "\"stand_seat_speed\":"  + String(stand_seat_speed) + ",";
  json += "\"offsets\":[";
  for (int l = 0; l < 4; l++)
    for (int j = 0; j < 3; j++)
      json += String(servoOffset[l][j]) + (l == 3 && j == 2 ? "" : ",");
  json += "]}";
  server.send(200, "application/json", json);
}

void handleSetConfig() {
  if (!server.hasArg("param") || !server.hasArg("val")) {
    server.send(400, "text/plain", "Missing param or val");
    return;
  }
  String param = server.arg("param");
  float  val   = server.arg("val").toFloat();

  // Guard: toFloat() returns 0 for non-numeric strings, but NaN/Infinity
  // from exotic inputs would cause (int)val undefined behaviour.
  if (isnan(val) || isinf(val)) {
    server.send(400, "text/plain", "Invalid numeric value");
    return;
  }

  bool   recompute = false;
  if      (param == "servoMin")         servoMin         = (int)val;
  else if (param == "servoMax")         servoMax         = (int)val;
  else if (param == "z_default")        z_default        = val;
  else if (param == "z_up")             z_up             = val;
  else if (param == "z_boot")           z_boot           = val;
  else if (param == "x_default")      { x_default        = val; recompute = true; }
  else if (param == "y_step")         { y_step           = val; recompute = true; }
  else if (param == "speed_multiple")   speed_multiple   = max(0.1f, val);
  else if (param == "leg_move_speed")   leg_move_speed   = max(0.1f, val);
  else if (param == "body_move_speed")  body_move_speed  = max(0.1f, val);
  else if (param == "spot_turn_speed")  spot_turn_speed  = max(0.1f, val);
  else if (param == "stand_seat_speed") stand_seat_speed = max(0.1f, val);
  else if (param.startsWith("off_")) {
    int leg   = param.charAt(4) - '0';
    int joint = param.charAt(6) - '0';
    if (leg >= 0 && leg < 4 && joint >= 0 && joint < 3)
      servoOffset[leg][joint] = (int)val;
  }
  // Cross-validate after dispatch: if either servoMin or servoMax was just written
  // and the range is now inverted, angleToPulse()'s map() call would reverse the
  // pulse direction and send corrupt widths to every servo. Reset to safe defaults.
  if (servoMin >= servoMax) {
    servoMin = 500;
    servoMax = 2400;
    server.send(400, "text/plain", "servoMin must be less than servoMax — reset to defaults");
    return;
  }
  if (recompute) recomputeTurnConstants();
  server.send(200, "text/plain", "OK");
}

// Move a single servo to a specific angle, bypassing IK.
// Useful for checking servo range and calibration offsets.
void handleMoveMotor() {
  if (server.hasArg("ch") && server.hasArg("angle")) {
    int ch  = server.arg("ch").toInt();
    int ang = server.arg("angle").toInt();
    if (ch >= 0 && ch < 12 && ang >= 0 && ang <= 180) {
      manualChannel = ch;
      manualAngle   = ang;
    }
  }
  server.send(200, "text/plain", "OK");
}

// Release manual override — IK resumes control of all channels.
// Sync site_expect to site_now for the frozen leg first so the leg
// stays exactly where the motor test left it rather than jolting
// back to the stale pre-freeze target.
void handleClearMotor() {
  if (manualChannel >= 0) {
    for (int leg = 0; leg < 4; leg++) {
      if (servo_pin[leg][0] == manualChannel ||
          servo_pin[leg][1] == manualChannel ||
          servo_pin[leg][2] == manualChannel) {
        // While manualChannel >= 0, servo_service() sets legFrozen = true
        // for this leg and skips its site_now update entirely.  That means
        // Core 0 is NOT writing site_now for this leg right now, so we can
        // safely take a local snapshot of the three components before we
        // touch any mutex-protected data.
        float snap[3];
        snap[0] = site_now[leg][0];
        snap[1] = site_now[leg][1];
        snap[2] = site_now[leg][2];
        // Now acquire the mutex and write the snapshot into site_expect so
        // that when IK resumes (after manualChannel is cleared below) the
        // leg holds its current physical position rather than jumping back
        // to whatever target was set before the motor test began.
        xSemaphoreTake(speedMutex, portMAX_DELAY);
        site_expect[leg][0] = snap[0];
        site_expect[leg][1] = snap[1];
        site_expect[leg][2] = snap[2];
        temp_speed[leg][0]  = 0.0f;
        temp_speed[leg][1]  = 0.0f;
        temp_speed[leg][2]  = 0.0f;
        xSemaphoreGive(speedMutex);
        break;
      }
    }
  }
  manualChannel = -1;
  server.send(200, "text/plain", "OK");
}

// Tells gait task to move robot to stand pose.
void handleApplyStand() {
  manualChannel = -1;          // release any motor override first
  currentCommand = CMD_STOP;   // CMD_STOP calls stand() then goes to IDLE
  server.send(200, "text/plain", "OK");
}

// Tells gait task to move robot to sit pose.
void handleApplySit() {
  manualChannel = -1;          // release any motor override first
  currentCommand = CMD_SIT;
  server.send(200, "text/plain", "OK");
}

//================================================================
// WEB SERVER ROUTES
//================================================================
// BUG FIX: previously getHTML() returned String(FPSTR(ROBOT_HTML)), which
// heap-allocated a copy of the entire ~20 KB HTML on every page request.
// On repeated loads or low heap, this caused OOM crashes.
// Fix: server.send() with a const char* sends directly from flash—no heap copy.
void handleRoot() { server.send(200, "text/html", ROBOT_HTML); }
void handleNotFound() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}
void handleCommand() {
  if (server.hasArg("action")) {
    String a = server.arg("action");
    // Always release motor override before any movement command —
    // a frozen leg causes wait_reach() to deadlock and the watchdog to reboot.
    manualChannel = -1;
    if      (a == "forward")  currentCommand = CMD_FORWARD;
    else if (a == "backward") currentCommand = CMD_BACKWARD;
    else if (a == "left")     currentCommand = CMD_LEFT;
    else if (a == "right")    currentCommand = CMD_RIGHT;
    else if (a == "stop")     currentCommand = CMD_STOP;
    else                      currentCommand = CMD_IDLE;
  }
  server.send(200, "text/plain", "OK");
}


//================================================================
// SAVE CALIBRATION TO NVS
// Sets a flag and returns immediately so the HTTP handler never blocks
// the gait task mid-movement. The actual NVS write happens in gaitTask's
// idle branch, where no servo movement is in progress.
//================================================================
void handleSaveCalib() {
  pendingSave = true;
  server.send(200, "text/plain", "OK");
}


//================================================================
// GAIT TASK — Core 1
//================================================================
void gaitTask(void* pvParameters) {
  vTaskDelay(pdMS_TO_TICKS(500));
  stand();
  for (;;) {
    server.handleClient();
    dnsServer.processNextRequest();
    switch (currentCommand) {
      case CMD_FORWARD:  step_forward(1); break;
      case CMD_BACKWARD: step_back(1);    break;
      case CMD_LEFT:     turn_left(1);    break;
      case CMD_RIGHT:    turn_right(1);   break;
      case CMD_STOP:
        stand();
        if (currentCommand == CMD_STOP)   // don't overwrite a new command that arrived during stand()
          currentCommand = CMD_IDLE;
        break;
      case CMD_SIT:
        sit();
        if (currentCommand == CMD_SIT)    // don't overwrite a new command that arrived during sit()
          currentCommand = CMD_IDLE;
        break;
      default:
        // Idle branch: safe moment to run NVS write since no movement is active.
        if (pendingSave) {
          saveToNVS();
          pendingSave = false;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
        break;
    }
  }
}

//================================================================
// SETUP
//================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("Monster Bot starting...");

  Wire.begin(13, 14);
  Wire.setClock(400000);
  pca9685.begin();
  pca9685.setOscillatorFrequency(25000000);
  pca9685.setPWMFreq(SERVO_FREQ);
  delay(10);

  recomputeTurnConstants();  // first pass with firmware defaults

  speedMutex = xSemaphoreCreateMutex();

  loadCalib();               // overwrite defaults with NVS-saved values (if any)
  recomputeTurnConstants();  // recompute turn geometry with loaded x_default / y_step

  set_site(0, x_default - x_offset, y_start + y_step, z_boot);
  set_site(1, x_default - x_offset, y_start + y_step, z_boot);
  set_site(2, x_default + x_offset, y_start,          z_boot);
  set_site(3, x_default + x_offset, y_start,          z_boot);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 3; j++)
      site_now[i][j] = site_expect[i][j];

  xTaskCreatePinnedToCore(servoTask, "ServoTask", 4096, NULL, 2, NULL, 0);
  Serial.println("Servo task started on Core 0.");

  delay(1500);

  WiFi.setSleep(false);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print("Hotspot: "); Serial.println(AP_SSID);
  Serial.println("Connect then open http://monsterbot.com");

  server.on("/",           handleRoot);
  server.on("/cmd",        handleCommand);
  server.on("/getconfig",  handleGetConfig);
  server.on("/setconfig",  handleSetConfig);
  server.on("/movemotor",  handleMoveMotor);
  server.on("/clearmotor", handleClearMotor);
  server.on("/applystand", handleApplyStand);
  server.on("/applysit",   handleApplySit);
  server.on("/savecalib",  handleSaveCalib);   // explicit NVS save triggered by "Save All" button
  server.onNotFound(handleNotFound);
  server.begin();
  dnsServer.start(53, "*", WiFi.softAPIP());

  xTaskCreatePinnedToCore(gaitTask, "GaitTask", 8192, NULL, 1, NULL, 1);
  Serial.println("Gait task started on Core 1. Ready.");
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
