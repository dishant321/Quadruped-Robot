// Auto-split from RobotCode.ino — Arduino preprocessor cannot handle
// C++11 raw string literals inside .ino files when they contain
// JavaScript function declarations (it mistakes them for C++ functions).
// Storing the HTML in a .h file bypasses that preprocessor entirely.
#pragma once

static const char ROBOT_HTML[] PROGMEM = R"MBHTML(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>Monster Bot</title>
<style>
/* Reset */
*, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
html, body { height: 100%; overflow: hidden; background: #050810; }
body {
  font-family: 'Inter', 'Segoe UI', sans-serif;
  color: rgba(220,230,255,0.9);
  -webkit-tap-highlight-color: transparent;
  user-select: none;
}
.screen { position: fixed; inset: 0; display: none; flex-direction: column; overflow: hidden; }
.screen.active { display: flex; }
#screen-calib { overflow-y: auto; display: none; }
#screen-calib.active { display: block; }
.particle { position: fixed; border-radius: 50%; pointer-events: none; z-index: 0; }
@keyframes float-up {
  0%,100% { transform: translateY(0);    opacity: var(--op); }
  50%      { transform: translateY(-14px); opacity: calc(var(--op) * 1.6); }
}
@keyframes robot-breathe {
  0%,100% { opacity: .13; transform: scale(1); }
  50%      { opacity: .22; transform: scale(1.012); }
}
@keyframes eye-pulse {
  0%,100% { fill: rgba(100,160,255,.5); }
  50%      { fill: rgba(100,200,255,.95); }
}
@keyframes scan-line {
  0%   { transform: translateY(146px); opacity: 0; }
  10%  { opacity: 1; }
  90%  { opacity: 1; }
  100% { transform: translateY(406px); opacity: 0; }
}
@keyframes ring-pulse {
  0%,100% { opacity: .18; }
  50%      { opacity: .35; }
}
@keyframes thumb-idle-glow {
  0%,100% { box-shadow: 0 0 10px rgba(100,160,255,.45), 0 0 24px rgba(60,100,255,.2); }
  50%      { box-shadow: 0 0 16px rgba(100,160,255,.75), 0 0 36px rgba(60,100,255,.35); }
}
.robot-wrap  { animation: robot-breathe 5s ease-in-out infinite; }
.eye-1       { animation: eye-pulse 2.8s ease-in-out infinite; }
.eye-2       { animation: eye-pulse 2.8s .4s ease-in-out infinite; }
.scan-rect   { animation: scan-line 3.5s ease-in-out infinite; }

/* LANDING */
.nav-btn {
  position: relative; display: flex; align-items: center; gap: 16px;
  padding: 20px 24px; width: 100%;
  background: linear-gradient(145deg,rgba(255,255,255,.05),rgba(255,255,255,.02));
  border: 1px solid rgba(255,255,255,.09); border-radius: 18px;
  cursor: pointer; text-align: left; font-family: inherit; color: inherit;
  transition: all .2s cubic-bezier(.4,0,.2,1);
  box-shadow: 0 4px 20px rgba(0,0,0,.4); backdrop-filter: blur(16px);
}
.nav-btn.active-blue {
  background: linear-gradient(145deg,rgba(60,100,255,.22),rgba(30,60,180,.14));
  border-color: rgba(100,150,255,.5);
  box-shadow: 0 0 40px rgba(60,100,255,.2),0 8px 24px rgba(0,0,0,.5);
}
.nav-btn.active-teal {
  background: linear-gradient(145deg,rgba(0,200,160,.18),rgba(0,160,120,.1));
  border-color: rgba(0,200,160,.45);
  box-shadow: 0 0 40px rgba(0,200,160,.15),0 8px 24px rgba(0,0,0,.5);
}
.nav-btn-shimmer {
  position: absolute; top: 0; left: 15%; right: 15%; height: 1px; border-radius: 1px;
  background: linear-gradient(90deg,transparent,rgba(120,160,255,.28),transparent);
  transition: background .2s;
}
.nav-btn.active-blue .nav-btn-shimmer { background: linear-gradient(90deg,transparent,rgba(120,160,255,.85),transparent); }
.nav-btn.active-teal .nav-btn-shimmer { background: linear-gradient(90deg,transparent,rgba(0,220,180,.85),transparent); }
.nav-btn-icon {
  width: 44px; height: 44px; border-radius: 12px;
  display: flex; align-items: center; justify-content: center; flex-shrink: 0;
}
.nav-btn-icon.blue { background: rgba(80,120,255,.15); border: 1px solid rgba(80,120,255,.32); }
.nav-btn-icon.teal { background: rgba(0,200,160,.12);  border: 1px solid rgba(0,200,160,.3); }
.nav-btn-label { font-size: 16px; font-weight: 700; color: rgba(220,230,255,.9); margin-bottom: 4px; }
.nav-btn-sub   { font-size: 12px; color: rgba(140,160,210,.55); line-height: 1.4; }
.nav-btn-arrow { font-size: 18px; color: rgba(120,160,255,.3); flex-shrink: 0; transition: color .2s; }
.nav-btn.active-blue .nav-btn-arrow { color: rgba(120,160,255,.9); }
.nav-btn.active-teal .nav-btn-arrow { color: rgba(0,210,170,.9); }

/* SHARED HEADER */
.page-header {
  position: relative; z-index: 2; width: 100%; padding: 16px 20px;
  display: flex; align-items: center; gap: 10px; flex-shrink: 0;
  border-bottom: 1px solid rgba(255,255,255,.06);
}
.back-btn {
  font-size: 10px; letter-spacing: .3em; text-transform: uppercase;
  color: rgba(140,170,255,.4); font-weight: 500; cursor: pointer;
  background: none; border: none; font-family: inherit;
}
.hdr-div   { width: 1px; height: 12px; background: rgba(120,150,220,.2); }
.hdr-title { font-size: 12px; font-weight: 700; color: rgba(200,220,255,.7); letter-spacing: .08em; }

/* SLIDERS */
input[type=range] { -webkit-appearance: none; background: transparent; width: 100%; }
input[type=range]::-webkit-slider-thumb {
  -webkit-appearance: none; width: 16px; height: 16px; border-radius: 50%; margin-top: -5.5px;
  background: linear-gradient(135deg,#5080ff,#00c8a0); cursor: pointer;
  box-shadow: 0 0 6px rgba(80,128,255,.5);
}
input[type=range]::-webkit-slider-runnable-track { height: 5px; border-radius: 3px; background: transparent; }
input[type=number]::-webkit-inner-spin-button { opacity: .3; }
.slider-row { display: flex; flex-direction: column; gap: 8px; }
.slider-head { display: flex; justify-content: space-between; align-items: center; }
.slider-label { font-size: 12px; color: rgba(160,185,230,.65); }
.slider-num {
  width: 58px; padding: 4px 8px; text-align: right; outline: none;
  background: rgba(255,255,255,.05); border: 1px solid rgba(255,255,255,.1);
  border-radius: 8px; color: rgba(200,220,255,.85); font-size: 12px; font-family: inherit;
}
.track-wrap { position: relative; height: 5px; border-radius: 3px; background: rgba(255,255,255,.08); }
.track-fill { position: absolute; left: 0; height: 100%; border-radius: 3px; background: linear-gradient(90deg,#3b6fff,#00c8a0); pointer-events: none; }
.track-input { position: absolute; inset: -8px 0; opacity: 0; cursor: pointer; width: 100%; }

/* CARDS */
.card {
  background: linear-gradient(145deg,rgba(255,255,255,.04),rgba(255,255,255,.02));
  border: 1px solid rgba(255,255,255,.08); border-radius: 16px; padding: 18px;
  box-shadow: 0 6px 24px rgba(0,0,0,.4);
}
.card-sm {
  background: linear-gradient(145deg,rgba(255,255,255,.03),rgba(255,255,255,.01));
  border: 1px solid rgba(255,255,255,.07); border-radius: 12px; padding: 14px 18px;
}
.sec-title { display: flex; align-items: center; gap: 8px; margin-bottom: 18px; }
.sec-bar   { width: 3px; height: 14px; border-radius: 2px; }
.sec-text  { font-size: 10px; font-weight: 700; letter-spacing: .22em; text-transform: uppercase; color: rgba(200,220,255,.55); }

/* ACTION BUTTONS */
.action-row { display: flex; gap: 10px; padding-top: 2px; }
.action-btn {
  flex: 1; padding: 10px 16px; border-radius: 10px; font-size: 12px; font-weight: 600;
  cursor: pointer; font-family: inherit; letter-spacing: .04em; transition: filter .15s;
}
.action-btn:active { filter: brightness(1.2); }
.btn-blue  { background:linear-gradient(145deg,rgba(60,100,255,.25),rgba(40,70,200,.15)); border:1px solid rgba(100,150,255,.4); color:rgba(160,200,255,.95); }
.btn-teal  { background:linear-gradient(145deg,rgba(0,200,160,.2),rgba(0,160,120,.12));   border:1px solid rgba(0,200,160,.4);   color:rgba(0,220,180,.95); }
.btn-ghost { background:rgba(255,255,255,.04); border:1px solid rgba(255,255,255,.1); color:rgba(180,200,240,.7); }

/* JOYSTICK */
#joystick-wrap {
  position: relative; width: 280px; height: 280px; border-radius: 50%;
  cursor: grab; touch-action: none; flex-shrink: 0; align-self: center;
}
#joy-ring {
  position: absolute; inset: 0; border-radius: 50%;
  background: radial-gradient(circle at 50%,rgba(60,100,255,.13) 0%,rgba(5,8,20,.82) 80%);
  border: 1.5px solid rgba(100,150,255,.22);
  box-shadow: 0 0 36px rgba(60,100,255,.12),inset 0 0 30px rgba(60,100,255,.07);
  animation: ring-pulse 3s ease-in-out infinite; pointer-events: none;
}
#joy-thumb {
  position: absolute; width: 110px; height: 110px; border-radius: 50%;
  left: 50%; top: 50%; transform: translate(-50%,-50%);
  background: radial-gradient(circle at 42% 40%,rgba(100,150,255,.38),rgba(40,75,200,.65));
  border: 1.5px solid rgba(120,165,255,.32);
  box-shadow: 0 0 18px rgba(80,140,255,.38),0 0 38px rgba(60,100,255,.16);
  animation: thumb-idle-glow 2.5s ease-in-out infinite; pointer-events: none;
  transition: transform .35s cubic-bezier(.34,1.56,.64,1);
}
#joy-thumb.dragging {
  background: radial-gradient(circle at 42% 40%,rgba(120,170,255,.55),rgba(50,90,230,.82));
  border-color: rgba(160,200,255,.55);
  box-shadow: 0 0 30px rgba(80,140,255,.7),0 0 60px rgba(60,100,255,.35);
  animation: none; transition: none;
}

/* DIRECTION COMPASS */
.compass-grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 8px; }
.compass-cell {
  height: 52px; border-radius: 10px; background: rgba(255,255,255,.04);
  border: 1px solid rgba(255,255,255,.08);
  display: flex; align-items: center; justify-content: center;
  font-size: 20px; color: rgba(120,150,200,.28); transition: all .1s;
}
.compass-cell.active {
  background: rgba(80,130,255,.55); border-color: rgba(100,160,255,.7);
  color: #fff; box-shadow: 0 0 14px rgba(80,130,255,.35);
}
.compass-gap { height: 52px; }

/* INFO ROW */
.info-row { display: flex; gap: 10px; width: 100%; max-width: 340px; }
.info-cell {
  flex: 1; background: linear-gradient(145deg,rgba(255,255,255,.03),rgba(255,255,255,.01));
  border: 1px solid rgba(255,255,255,.07); border-radius: 12px; padding: 12px 14px;
}
.info-cell-label { font-size: 10px; color: rgba(140,160,210,.5); margin-bottom: 6px; }
#stop-btn {
  flex: 1; padding: 12px 14px; border-radius: 12px; cursor: pointer;
  font-size: 11px; font-weight: 700; letter-spacing: .08em; text-transform: uppercase;
  text-align: center; display: flex; align-items: center; justify-content: center;
  transition: all .2s; font-family: inherit;
  background: linear-gradient(145deg,rgba(220,50,50,.2),rgba(180,30,30,.13));
  border: 1px solid rgba(220,80,80,.4); color: rgba(255,150,150,.95);
}
#stop-btn.inactive {
  background: rgba(255,255,255,.03); border-color: rgba(255,255,255,.07);
  color: rgba(150,170,220,.4); cursor: default;
}

/* CALIBRATION EXTRAS */
.calib-select {
  width: 100%; padding: 10px 12px; background: rgba(255,255,255,.05);
  border: 1px solid rgba(255,255,255,.12); border-radius: 10px;
  color: rgba(200,220,255,.85); font-size: 12px; outline: none;
  cursor: pointer; font-family: inherit;
}
select option { background: #0d1425; }
.connected-badge {
  display: flex; align-items: center; gap: 6px;
  background: rgba(0,255,160,.07); border: 1px solid rgba(0,255,160,.18);
  border-radius: 100px; padding: 4px 11px;
}
.connected-dot  { width: 6px; height: 6px; border-radius: 50%; background: #00ffa0; box-shadow: 0 0 5px #00ffa0; }
.connected-text { color: #00ffa0; font-size: 11px; font-weight: 500; }
.offset-grid { display: grid; grid-template-columns: 64px 1fr 1fr 1fr; gap: 6px; align-items: center; }
.offset-input {
  width: 100%; padding: 8px 6px; text-align: center; outline: none;
  background: rgba(255,255,255,.05); border: 1px solid rgba(255,255,255,.1);
  border-radius: 8px; color: rgba(200,220,255,.85); font-size: 13px; font-family: inherit;
}
.joint-hdr { text-align: center; font-size: 10px; font-weight: 600; letter-spacing: .1em; text-transform: uppercase; color: rgba(160,185,230,.45); }
.leg-lbl   { font-size: 11px; color: rgba(160,185,230,.6); font-weight: 500; }
</style>
</head>
<body>

<!-- SCREEN 1: LANDING -->
<div id="screen-landing" class="screen active"
  style="align-items:center;justify-content:center;background:#050810;overflow:hidden;padding:0 20px;">
  <div style="position:absolute;top:-10%;left:50%;transform:translateX(-50%);width:500px;height:400px;
    background:radial-gradient(ellipse at center top,rgba(80,120,255,.2)0%,rgba(40,80,200,.08)40%,transparent 70%);
    pointer-events:none;"></div>
  <div style="position:absolute;top:50%;left:50%;transform:translate(-50%,-48%);pointer-events:none;">
    <div class="robot-wrap">
      <svg width="220" height="360" viewBox="0 0 420 520" fill="none" xmlns="http://www.w3.org/2000/svg">
        <line x1="210" y1="8" x2="210" y2="48" stroke="rgba(120,160,255,.9)" stroke-width="4" stroke-linecap="round"/>
        <circle cx="210" cy="8" r="7" fill="rgba(120,160,255,.9)"/>
        <rect x="140" y="48" width="140" height="110" rx="22" fill="rgba(80,110,200,.55)" stroke="rgba(120,160,255,.5)" stroke-width="1.5"/>
        <rect x="162" y="78" width="36" height="22" rx="6" fill="rgba(100,160,255,.3)" stroke="rgba(120,180,255,.5)" stroke-width="1"/>
        <rect x="222" y="78" width="36" height="22" rx="6" fill="rgba(100,160,255,.3)" stroke="rgba(120,180,255,.5)" stroke-width="1"/>
        <circle class="eye-1" cx="180" cy="89" r="7" fill="rgba(100,160,255,.5)"/>
        <circle class="eye-2" cx="240" cy="89" r="7" fill="rgba(100,160,255,.5)"/>
        <rect x="172" y="122" width="76" height="14" rx="7" fill="rgba(80,160,255,.3)" stroke="rgba(100,160,255,.4)" stroke-width="1"/>
        <rect x="180" y="126" width="10" height="6" rx="2" fill="rgba(100,180,255,.5)"/>
        <rect x="196" y="126" width="10" height="6" rx="2" fill="rgba(100,180,255,.5)"/>
        <rect x="212" y="126" width="10" height="6" rx="2" fill="rgba(100,180,255,.5)"/>
        <rect x="228" y="126" width="10" height="6" rx="2" fill="rgba(100,180,255,.5)"/>
        <rect x="192" y="158" width="36" height="22" rx="6" fill="rgba(70,100,180,.5)" stroke="rgba(100,140,220,.4)" stroke-width="1"/>
        <rect x="100" y="180" width="220" height="170" rx="26" fill="rgba(60,90,180,.45)" stroke="rgba(100,140,220,.4)" stroke-width="1.5"/>
        <rect x="130" y="206" width="160" height="90" rx="12" fill="rgba(40,70,160,.5)" stroke="rgba(80,130,220,.35)" stroke-width="1"/>
        <rect x="146" y="220" width="50" height="28" rx="6" fill="rgba(0,200,160,.18)" stroke="rgba(0,210,170,.4)" stroke-width="1"/>
        <rect x="208" y="220" width="50" height="28" rx="6" fill="rgba(80,120,255,.18)" stroke="rgba(100,150,255,.4)" stroke-width="1"/>
        <rect x="146" y="258" width="112" height="8" rx="4" fill="rgba(80,120,255,.18)" stroke="rgba(100,150,255,.3)" stroke-width="1"/>
        <circle cx="171" cy="234" r="9" fill="rgba(0,200,160,.25)" stroke="rgba(0,210,170,.5)" stroke-width="1"/>
        <circle cx="233" cy="234" r="9" fill="rgba(80,120,255,.25)" stroke="rgba(100,150,255,.5)" stroke-width="1"/>
        <rect x="36"  y="185" width="58" height="140" rx="22" fill="rgba(60,90,175,.45)"  stroke="rgba(90,130,210,.4)"  stroke-width="1.5"/>
        <rect x="38"  y="325" width="58" height="34"  rx="14" fill="rgba(55,85,170,.5)"   stroke="rgba(85,125,210,.4)"  stroke-width="1.5"/>
        <rect x="326" y="185" width="58" height="140" rx="22" fill="rgba(60,90,175,.45)"  stroke="rgba(90,130,210,.4)"  stroke-width="1.5"/>
        <rect x="324" y="325" width="58" height="34"  rx="14" fill="rgba(55,85,170,.5)"   stroke="rgba(85,125,210,.4)"  stroke-width="1.5"/>
        <rect x="126" y="350" width="168" height="30" rx="10" fill="rgba(50,80,165,.5)"   stroke="rgba(85,125,210,.35)" stroke-width="1"/>
        <rect x="122" y="378" width="74"  height="140" rx="22" fill="rgba(55,85,170,.45)" stroke="rgba(85,125,210,.4)"  stroke-width="1.5"/>
        <rect x="108" y="500" width="94"  height="22"  rx="11" fill="rgba(50,80,160,.55)" stroke="rgba(80,120,200,.4)"  stroke-width="1.5"/>
        <rect x="224" y="378" width="74"  height="140" rx="22" fill="rgba(55,85,170,.45)" stroke="rgba(85,125,210,.4)"  stroke-width="1.5"/>
        <rect x="218" y="500" width="94"  height="22"  rx="11" fill="rgba(50,80,160,.55)" stroke="rgba(80,120,200,.4)"  stroke-width="1.5"/>
        <rect class="scan-rect" x="100" y="0" width="220" height="2" rx="1" fill="rgba(100,200,255,.5)"/>
      </svg>
    </div>
  </div>
  <div style="position:absolute;bottom:-10%;left:50%;transform:translateX(-50%);width:400px;height:200px;
    background:radial-gradient(ellipse at center,rgba(0,200,160,.09)0%,transparent 70%);pointer-events:none;"></div>
  <div style="position:relative;z-index:1;display:flex;flex-direction:column;align-items:center;gap:36px;width:100%;max-width:360px;">
    <div style="text-align:center;">
      <div style="font-size:10px;letter-spacing:.35em;text-transform:uppercase;color:rgba(140,170,255,.5);margin-bottom:12px;font-weight:500;">Control Interface</div>
      <h1 style="margin:0;font-size:42px;font-weight:900;letter-spacing:-.01em;line-height:1;
        background:linear-gradient(160deg,#fff 30%,rgba(160,190,255,.7)100%);
        -webkit-background-clip:text;-webkit-text-fill-color:transparent;">MONSTER BOT</h1>
    </div>
    <div style="display:flex;flex-direction:column;gap:14px;width:100%;">
      <button class="nav-btn" id="btn-control" onclick="showScreen('control')">
        <div class="nav-btn-shimmer"></div>
        <div class="nav-btn-icon blue">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none">
            <circle cx="12" cy="12" r="2.5" fill="rgba(120,160,255,.9)"/>
            <path d="M12 4v4M12 16v4M4 12h4M16 12h4" stroke="rgba(120,160,255,.9)" stroke-width="2" stroke-linecap="round"/>
            <path d="M6.7 6.7l2.6 2.6M14.7 14.7l2.6 2.6M6.7 17.3l2.6-2.6M14.7 9.3l2.6-2.6" stroke="rgba(120,160,255,.4)" stroke-width="1.5" stroke-linecap="round"/>
          </svg>
        </div>
        <div style="flex:1;">
          <div class="nav-btn-label">Control Pad</div>
          <div class="nav-btn-sub">Directional movement &amp; real-time commands</div>
        </div>
        <div class="nav-btn-arrow">&#x2192;</div>
      </button>
      <button class="nav-btn" id="btn-calib" onclick="showScreen('calib')">
        <div class="nav-btn-shimmer"></div>
        <div class="nav-btn-icon teal">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none">
            <circle cx="12" cy="12" r="3" stroke="rgba(0,210,170,.9)" stroke-width="1.8"/>
            <path d="M12 2v4M12 18v4M2 12h4M18 12h4" stroke="rgba(0,210,170,.9)" stroke-width="1.8" stroke-linecap="round"/>
            <circle cx="12" cy="12" r="7" stroke="rgba(0,210,170,.22)" stroke-width="1" stroke-dasharray="2 3"/>
          </svg>
        </div>
        <div style="flex:1;">
          <div class="nav-btn-label">Calibration Dashboard</div>
          <div class="nav-btn-sub">Sensor tuning &amp; precision alignment</div>
        </div>
        <div class="nav-btn-arrow">&#x2192;</div>
      </button>
    </div>
  </div>
</div>

<!-- SCREEN 2: CONTROL PAD -->
<div id="screen-control" class="screen" style="background:#050810;">
  <div class="page-header">
    <button class="back-btn" onclick="showScreen('landing')">&#x2190; MONSTER BOT</button>
    <div class="hdr-div"></div>
    <span class="hdr-title">Control Pad</span>
  </div>
  <div style="position:relative;z-index:1;display:flex;flex-direction:column;align-items:center;
    gap:24px;padding:28px 20px 32px;width:100%;overflow-y:auto;box-sizing:border-box;">
    <div style="font-size:10px;letter-spacing:.3em;text-transform:uppercase;color:rgba(140,170,255,.45);font-weight:500;">
      Directional Control
    </div>
    <div id="joystick-wrap">
      <svg width="280" height="280" viewBox="0 0 280 280" style="position:absolute;inset:0;pointer-events:none;">
        <line x1="18" y1="18" x2="262" y2="262" stroke="rgba(100,150,255,.13)" stroke-width="1"/>
        <line x1="262" y1="18" x2="18" y2="262" stroke="rgba(100,150,255,.13)" stroke-width="1"/>
      </svg>
      <div id="joy-ring"></div>
      <div id="joy-thumb"></div>
    </div>
    <div style="width:100%;max-width:340px;" class="card-sm">
      <div style="font-size:10px;color:rgba(140,160,210,.5);letter-spacing:.08em;text-transform:uppercase;margin-bottom:12px;">Direction</div>
      <div class="compass-grid">
        <div class="compass-gap"></div>
        <div class="compass-cell" id="dir-forward">&#x2191;</div>
        <div class="compass-gap"></div>
        <div class="compass-cell" id="dir-left">&#x2190;</div>
        <div class="compass-gap"></div>
        <div class="compass-cell" id="dir-right">&#x2192;</div>
        <div class="compass-gap"></div>
        <div class="compass-cell" id="dir-back">&#x2193;</div>
        <div class="compass-gap"></div>
      </div>
    </div>
    <div style="width:100%;max-width:340px;" class="card-sm">
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:4px;">
        <div>
          <span style="font-size:12px;color:rgba(160,185,230,.75);font-weight:600;">Movement Speed</span>
          <div style="font-size:10px;color:rgba(0,210,160,.55);margin-top:2px;font-weight:500;">&#x26a1; Real-time &mdash; no save needed</div>
        </div>
        <div style="display:flex;align-items:baseline;gap:3px;">
          <span id="speed-mult" style="font-size:16px;font-weight:700;color:rgba(120,160,255,.95);">1.00</span>
          <span style="font-size:11px;color:rgba(140,160,210,.5);">&#xd7;</span>
        </div>
      </div>
      <div style="display:flex;justify-content:space-between;margin-bottom:10px;margin-top:8px;">
        <span style="font-size:10px;color:rgba(140,160,210,.35);">Slow</span>
        <span style="font-size:10px;color:rgba(140,160,210,.35);">Default</span>
        <span style="font-size:10px;color:rgba(140,160,210,.35);">Fast</span>
      </div>
      <div class="track-wrap">
        <div class="track-fill" id="speed-fill" style="width:18%;"></div>
        <input class="track-input" id="speed-range" type="range" min="0" max="100" value="18"
          oninput="syncSpeed(this.value,'range')">
      </div>
      <div style="display:flex;justify-content:space-between;margin-top:6px;">
        <span style="font-size:9px;color:rgba(100,130,200,.35);">0.1&#xd7;</span>
        <span style="font-size:9px;color:rgba(100,180,255,.4);">&#x25cb; 1.00&#xd7;</span>
        <span style="font-size:9px;color:rgba(100,130,200,.35);">5.0&#xd7;</span>
      </div>
    </div>
    <div class="info-row">
      <div class="info-cell">
        <div class="info-cell-label">Status</div>
        <div style="display:flex;align-items:center;gap:6px;">
          <div id="status-dot" style="width:6px;height:6px;border-radius:50%;background:#00ffa0;box-shadow:0 0 5px #00ffa0;flex-shrink:0;"></div>
          <span id="status-text" style="font-size:11px;font-weight:700;color:#00ffa0;line-height:1.2;">Stop</span>
        </div>
      </div>
      <div class="info-cell">
        <div class="info-cell-label">Connection</div>
        <div style="font-size:13px;font-weight:600;color:rgba(200,220,255,.8);">Active</div>
      </div>
      <button id="stop-btn" class="inactive" onclick="stopRobot()">STOP</button>
    </div>
  </div>
</div>

<!-- SCREEN 3: CALIBRATION -->
<div id="screen-calib" class="screen" style="background:#050810;">
  <div style="position:sticky;top:0;z-index:10;background:rgba(5,8,16,.9);backdrop-filter:blur(16px);
    border-bottom:1px solid rgba(255,255,255,.07);padding:13px 18px;
    display:flex;align-items:center;justify-content:space-between;flex-shrink:0;">
    <div style="display:flex;align-items:center;gap:10px;">
      <button class="back-btn" onclick="showScreen('landing')">&#x2190; MONSTER BOT</button>
      <div class="hdr-div"></div>
      <span style="font-size:13px;font-weight:700;color:rgba(200,220,255,.85);letter-spacing:.04em;">Calibration</span>
    </div>
    <div class="connected-badge">
      <div class="connected-dot"></div>
      <span class="connected-text">Connected</span>
    </div>
  </div>
  <div style="padding:20px 16px 48px;display:flex;flex-direction:column;gap:16px;position:relative;z-index:1;">
    <!-- Servo Testing -->
    <div class="card">
      <div class="sec-title">
        <div class="sec-bar" style="background:rgba(80,120,255,.8);"></div>
        <span class="sec-text">Servo Testing</span>
      </div>
      <div style="display:flex;flex-direction:column;gap:14px;">
        <div id="servo-sliders"></div>
        <div style="display:flex;flex-direction:column;gap:8px;">
          <span class="slider-label">Motor / Channel</span>
          <select class="calib-select" id="channel-select"></select>
        </div>
        <div id="angle-slider"></div>
        <div class="action-row">
          <button class="action-btn btn-blue"
            onclick="sendCommand('servo','move',+document.getElementById('angle-range').value)">Move</button>
          <button class="action-btn btn-ghost"
            onclick="sendCommand('servo','release',0)">Release</button>
        </div>
      </div>
    </div>
    <!-- Stance & Height -->
    <div class="card">
      <div class="sec-title">
        <div class="sec-bar" style="background:rgba(0,200,160,.8);"></div>
        <span class="sec-text">Stance &amp; Height</span>
      </div>
      <div style="display:flex;flex-direction:column;gap:14px;">
        <div id="stance-sliders"></div>
        <div class="action-row">
          <button class="action-btn btn-teal"  onclick="sendCommand('stance','stand',1)">Stand</button>
          <button class="action-btn btn-ghost"  onclick="sendCommand('stance','sit',1)">Sit</button>
        </div>
      </div>
    </div>
    <!-- Speed -->
    <div class="card">
      <div class="sec-title">
        <div class="sec-bar" style="background:rgba(250,180,0,.8);"></div>
        <span class="sec-text">Speed</span>
      </div>
      <div style="display:flex;flex-direction:column;gap:14px;" id="speed-sliders"></div>
    </div>
    <!-- Servo Offsets -->
    <div class="card">
      <div class="sec-title">
        <div class="sec-bar" style="background:rgba(200,100,255,.8);"></div>
        <span class="sec-text">Servo Offsets (Degrees)</span>
      </div>
      <div style="display:flex;flex-direction:column;gap:10px;">
        <div class="offset-grid" style="margin-bottom:2px;">
          <div></div>
          <div class="joint-hdr">Alpha</div>
          <div class="joint-hdr">Beta</div>
          <div class="joint-hdr">Gamma</div>
        </div>
        <div id="offset-rows"></div>
        <div class="action-row" style="margin-top:4px;">
          <button class="action-btn btn-blue" onclick="applyOffsets()">Stand to preview offsets</button>
        </div>
      </div>
    </div>

    <!-- SAVE BUTTON -->
    <div style="padding:4px 0 8px;">
      <button id="save-btn" onclick="saveCalib()"
        style="width:100%;padding:16px;border-radius:14px;font-size:14px;font-weight:700;
          letter-spacing:.06em;cursor:pointer;font-family:inherit;
          background:linear-gradient(145deg,rgba(60,100,255,.35),rgba(0,180,140,.25));
          border:1px solid rgba(100,180,255,.5);color:rgba(220,235,255,.95);
          box-shadow:0 0 32px rgba(60,100,255,.18),0 4px 16px rgba(0,0,0,.4);
          transition:filter .15s;">
        &#x2713;&nbsp; Save All &amp; Preview on Robot
      </button>
      <div id="save-feedback" style="text-align:center;margin-top:10px;font-size:11px;
        color:rgba(0,220,170,0);transition:color .4s;">Settings applied &#x2713;</div>
    </div>

  </div>
</div>

<script>
/* ================================================================
   sendCommand — routes UI actions to ESP32 HTTP endpoints
   ================================================================ */
var _selectedChannel = 0;

function sendCommand(type, target, value) {
  var url = null;
  if (type === 'joystick') {
    var zone = value;
    var action = (zone === 'back') ? 'backward' : (zone || 'stop');
    url = '/cmd?action=' + action;
  } else if (type === 'speed') {
    /* Map 0-100% -> 0.1-5.0 speed_multiple, matching the firmware's actual range */
    var mult = (0.1 + (Number(value) / 100) * 4.9).toFixed(2);
    url = '/setconfig?param=speed_multiple&val=' + mult;
  } else if (type === 'calib') {
    var paramMap = {
      'min-pulse':'servoMin','max-pulse':'servoMax',
      'z-default':'z_default','z-up':'z_up','z-boot':'z_boot',
      'x-default':'x_default','y-step':'y_step',
      'spd-scale':'speed_multiple','spd-leg':'leg_move_speed',
      'spd-body':'body_move_speed','spd-turn':'spot_turn_speed','spd-sit':'stand_seat_speed'
    };
    var p = paramMap[target];
    if (!p) return;
    url = '/setconfig?param=' + encodeURIComponent(p) + '&val=' + encodeURIComponent(value);
  } else if (type === 'servo') {
    if (target === 'channel') {
      _selectedChannel = Number(value);
      return;
    } else if (target === 'move') {
      /* angle is 0..180 directly from UI slider; pass straight to firmware */
      var ang = Math.round(Number(value));
      url = '/movemotor?ch=' + _selectedChannel + '&angle=' + ang;
    } else if (target === 'release') {
      url = '/clearmotor';
    }
  } else if (type === 'stance') {
    url = (target === 'stand') ? '/applystand' : '/applysit';
  } else if (type === 'offset') {
    if (target === 'apply') {
      url = '/applystand';
    } else {
      /* target is already in firmware format 'off_L_J' (e.g. 'off_0_2'),
         produced by setOffset() as 'off_'+li+'_'+ji.
         Pass it straight through to /setconfig — no regex needed. */
      url = '/setconfig?param=' + encodeURIComponent(target) + '&val=' + encodeURIComponent(value);
    }
  }
  if (url) fetch(url).catch(function(e) { console.warn('sendCommand failed:', url, e); });
}

/* ================================================================
   SCREEN NAVIGATION
   ================================================================ */
function showScreen(name) {
  document.querySelectorAll('.screen').forEach(function(s) { s.classList.remove('active'); });
  document.getElementById('screen-' + name).classList.add('active');
  if (name === 'calib') loadConfig();
  /* When returning to control, re-sync the speed multiplier display from ESP32 */
  if (name === 'control') {
    fetch('/getconfig').then(function(r) { return r.json(); }).then(function(d) {
      if (d.speed_multiple !== undefined) {
        /* Reverse-map speed_multiple (0.1-5.0) back to slider 0-100 */
        var pct = Math.round(Math.max(0, Math.min(100, (d.speed_multiple - 0.1) / 4.9 * 100)));
        var rng = document.getElementById('speed-range');
        var fill = document.getElementById('speed-fill');
        var mult = document.getElementById('speed-mult');
        if (rng)  rng.value = pct;
        if (fill) fill.style.width = pct + '%';
        if (mult) mult.textContent = d.speed_multiple.toFixed(2);
      }
    }).catch(function() {});
  }
}

/* ================================================================
   LOAD CONFIG from ESP32 and populate calibration sliders
   ================================================================ */
function loadConfig() {
  fetch('/getconfig').then(function(r) { return r.json(); }).then(function(d) {
    var map = {
      'min-pulse':[d.servoMin,500,1500],
      'max-pulse':[d.servoMax,1500,2500],
      'z-default':[d.z_default,-200,0],
      'z-up':[d.z_up,-200,0],
      'z-boot':[d.z_boot,-200,0],
      'x-default':[d.x_default, 0, 120],
      'y-step':[d.y_step,0,100],
      'spd-scale':[d.speed_multiple,0.1,5],
      'spd-leg':[d.leg_move_speed,   0.1, 200],
      'spd-body':[d.body_move_speed, 0.1, 200],
      'spd-turn':[d.spot_turn_speed, 0.1, 200],
      'spd-sit':[d.stand_seat_speed, 0.1, 200]
    };
    Object.keys(map).forEach(function(id) {
      var arr = map[id], val = arr[0], mn = arr[1], mx = arr[2];
      if (val === undefined) return;
      var fill = document.getElementById(id+'-fill');
      var num  = document.getElementById(id+'-num');
      var rng  = document.getElementById(id+'-range');
      var pct  = ((Math.max(mn, Math.min(mx, val)) - mn) / (mx - mn) * 100).toFixed(1);
      if (fill) fill.style.width = pct + '%';
      if (num)  num.value = val;
      if (rng)  rng.value = val;
    });
    if (d.offsets) {
      var i = 0;
      for (var l = 0; l < 4; l++) {
        for (var j = 0; j < 3; j++) {
          var el = document.getElementById('off-'+l+'-'+j);
          if (el) {
            el.value = d.offsets[i];
            offsets[l][j] = d.offsets[i]; // keep JS mirror array in sync with DOM
          }
          i++;
        }
      }
    }
  }).catch(function(e) { console.warn('loadConfig failed:', e); });
}

/* ================================================================
   BACKGROUND PARTICLES
   ================================================================ */
(function() {
  var frag = document.createDocumentFragment();
  for (var i = 0; i < 36; i++) {
    var x   = Math.round((Math.sin(i * 137.508) * 0.5 + 0.5) * 100);
    var y   = Math.round((Math.cos(i * 97.3)   * 0.5 + 0.5) * 100);
    var sz  = 1 + (i % 2);
    var dur = 6 + (i % 8) * 1.4;
    var del = -(i * 0.42);
    var op  = 0.15 + (i % 5) * 0.07;
    var col = i % 7 === 0 ? 'rgba(0,210,170,0.9)' : 'rgba(120,160,255,0.9)';
    var d   = document.createElement('div');
    d.className = 'particle';
    d.style.cssText = 'left:'+x+'%;top:'+y+'%;width:'+sz+'px;height:'+sz+'px;background:'+col+
      ';--op:'+op+';opacity:'+op+';animation:float-up '+dur+'s '+del+'s ease-in-out infinite;';
    frag.appendChild(d);
  }
  document.body.appendChild(frag);
})();

/* ================================================================
   LANDING PAGE hover/touch states
   ================================================================ */
(function() {
  var bc = document.getElementById('btn-control');
  var bg = document.getElementById('btn-calib');
  bc.addEventListener('mouseenter', function() { bc.classList.add('active-blue'); });
  bc.addEventListener('mouseleave', function() { bc.classList.remove('active-blue'); });
  bc.addEventListener('touchstart', function() { bc.classList.add('active-blue'); }, { passive: true });
  bc.addEventListener('touchend',   function() { bc.classList.remove('active-blue'); });
  bg.addEventListener('mouseenter', function() { bg.classList.add('active-teal'); });
  bg.addEventListener('mouseleave', function() { bg.classList.remove('active-teal'); });
  bg.addEventListener('touchstart', function() { bg.classList.add('active-teal'); }, { passive: true });
  bg.addEventListener('touchend',   function() { bg.classList.remove('active-teal'); });
})();

/* ================================================================
   JOYSTICK
   ================================================================ */
(function() {
  var OUTER_R   = 120;
  var DEAD_ZONE = 16;
  var wrap  = document.getElementById('joystick-wrap');
  var thumb = document.getElementById('joy-thumb');
  var dragging = false;
  var lastZone = null;
  var STATUS = { forward:'Moving Forward', back:'Moving Back', left:'Turning Left', right:'Turning Right' };

  function getZone(x, y) {
    var dist = Math.sqrt(x*x + y*y);
    if (dist < DEAD_ZONE) return null;
    var a = Math.atan2(y, x) * (180 / Math.PI);
    if (a >= -135 && a < -45)  return 'forward';
    if (a >= -45  && a < 45)   return 'right';
    if (a >= 45   && a < 135)  return 'back';
    return 'left';
  }

  function applyZone(zone) {
    if (zone === lastZone) return;
    lastZone = zone;
    ['forward','back','left','right'].forEach(function(dir) {
      document.getElementById('dir-'+dir).classList.toggle('active', dir === zone);
    });
    var dot  = document.getElementById('status-dot');
    var txt  = document.getElementById('status-text');
    var stop = document.getElementById('stop-btn');
    if (zone) {
      dot.style.cssText = 'width:6px;height:6px;border-radius:50%;background:#f59e0b;box-shadow:0 0 5px #f59e0b;flex-shrink:0;';
      txt.style.color   = '#fbbf24';
      txt.textContent   = STATUS[zone];
      stop.classList.remove('inactive');
    } else {
      dot.style.cssText = 'width:6px;height:6px;border-radius:50%;background:#00ffa0;box-shadow:0 0 5px #00ffa0;flex-shrink:0;';
      txt.style.color   = '#00ffa0';
      txt.textContent   = 'Stop';
      stop.classList.add('inactive');
    }
    sendCommand('joystick', 'zone', zone || 'stop');
  }

  function moveThumb(clientX, clientY) {
    var rect = wrap.getBoundingClientRect();
    var cx = rect.left + rect.width  / 2;
    var cy = rect.top  + rect.height / 2;
    var dx = clientX - cx;
    var dy = clientY - cy;
    var dist = Math.sqrt(dx*dx + dy*dy);
    if (dist > OUTER_R) { dx = dx/dist*OUTER_R; dy = dy/dist*OUTER_R; }
    thumb.style.transform = 'translate(calc(-50% + '+dx+'px), calc(-50% + '+dy+'px))';
    applyZone(getZone(dx, dy));
  }

  function startDrag(cx, cy) { dragging = true; thumb.classList.add('dragging'); moveThumb(cx, cy); }
  function endDrag() {
    if (!dragging) return;
    dragging = false; thumb.classList.remove('dragging');
    thumb.style.transform = 'translate(-50%,-50%)';
    applyZone(null);
  }

  wrap.addEventListener('pointerdown', function(e) {
    e.preventDefault(); wrap.setPointerCapture(e.pointerId); startDrag(e.clientX, e.clientY);
  });
  wrap.addEventListener('pointermove', function(e) { if (dragging) moveThumb(e.clientX, e.clientY); });
  wrap.addEventListener('pointerup',     endDrag);
  wrap.addEventListener('pointercancel', endDrag);
  window.addEventListener('pointerup',   endDrag);

  // Expose internals so stopRobot() (outside this IIFE) can reset the UI
  window._joyApplyZone  = applyZone;
  window._joyResetThumb = function() { thumb.style.transform = 'translate(-50%,-50%)'; };
})();

function stopRobot() {
  sendCommand('joystick', 'zone', 'stop');
  if (window._joyApplyZone)  window._joyApplyZone(null);
  if (window._joyResetThumb) window._joyResetThumb();
}

/* ================================================================
   SPEED SLIDER — real-time, maps 0-100% to 0.1x-5.0x speed_multiple
   Default 18% ≈ 1.0x (firmware default)
   ================================================================ */
function syncSpeed(val, source) {
  val = Math.max(0, Math.min(100, Number(val)));
  /* Map 0-100% -> 0.1-5.0 speed_multiple, matching the firmware's actual range */
  var mult = (0.1 + (val / 100) * 4.9).toFixed(2);
  document.getElementById('speed-fill').style.width = val + '%';
  document.getElementById('speed-mult').textContent = mult;
  if (source !== 'range') document.getElementById('speed-range').value = val;
  sendCommand('speed', 'value', val);
}

/* ================================================================
   CALIBRATION SLIDER FACTORY
   ================================================================ */
function makeSlider(label, min, max, def, unit, id) {
  var pct = ((def - min) / (max - min)) * 100;
  return '<div class="slider-row">'
    + '<div class="slider-head">'
    +   '<span class="slider-label">'+label+(unit?' ('+unit+')':'')+'</span>'
    +   '<input class="slider-num" type="number" id="'+id+'-num" value="'+def+'"'
    +   ' oninput="sliderSync(\''+id+'\',this.value,\'num\','+min+','+max+')">'
    + '</div>'
    + '<div class="track-wrap">'
    +   '<div class="track-fill" id="'+id+'-fill" style="width:'+pct+'%;"></div>'
    +   '<input class="track-input" type="range" id="'+id+'-range" min="'+min+'" max="'+max+'" step="any" value="'+def+'"'
    +   ' oninput="sliderSync(\''+id+'\',this.value,\'range\','+min+','+max+')">'
    + '</div>'
    + '</div>';
}

function sliderSync(id, val, source, min, max) {
  val = Math.max(min, Math.min(max, Number(val)));
  var pct = ((val - min) / (max - min)) * 100;
  document.getElementById(id+'-fill').style.width  = pct + '%';
  if (source !== 'num')   document.getElementById(id+'-num').value   = val;
  if (source !== 'range') document.getElementById(id+'-range').value = val;
  sendCommand('calib', id, val);
}

/* Build calibration sections */
document.getElementById('servo-sliders').innerHTML =
  makeSlider('Min pulse', 500, 1500, 500, 'us', 'min-pulse') +
  makeSlider('Max pulse', 1500, 2500, 2400, 'us', 'max-pulse');

var CHANNELS = [
  'Ch 0 - FL Alpha','Ch 1 - FL Beta','Ch 2 - FL Gamma',
  'Ch 3 - FR Alpha','Ch 4 - FR Beta','Ch 5 - FR Gamma',
  'Ch 6 - BL Alpha','Ch 7 - BL Beta','Ch 8 - BL Gamma',
  'Ch 9 - BR Alpha','Ch 10 - BR Beta','Ch 11 - BR Gamma'
];
var sel = document.getElementById('channel-select');
CHANNELS.forEach(function(c, i) {
  var o = document.createElement('option');
  o.value = i; o.textContent = c; sel.appendChild(o);
});
sel.addEventListener('change', function() { sendCommand('servo','channel', this.value); });

document.getElementById('angle-slider').innerHTML = makeSlider('Angle', 0, 180, 90, 'deg', 'angle');
// NOTE: the angle slider's oninput routes through sliderSync -> sendCommand('calib', 'angle', ...)
// which intentionally does nothing ('angle' is not in paramMap).  The slider is purely a
// visual position picker for the motor test — the actual move only fires when the user
// clicks the "Move" button, which calls sendCommand('servo', 'move', ...) -> /movemotor.
// This is by design: live-dragging a single servo while IK is running could damage the
// mechanics, so the explicit button press is the correct gate for motor testing.

document.getElementById('stance-sliders').innerHTML = [
  makeSlider('z_default', -200, 0, -50,  'mm', 'z-default'),
  makeSlider('z_up',      -200, 0, -30,  'mm', 'z-up'),
  makeSlider('z_boot',    -200, 0, -28,  'mm', 'z-boot'),
  makeSlider('x_default',    0, 120, 62, 'mm', 'x-default'),
  makeSlider('y_step',       0, 100, 40, 'mm', 'y-step'),
].join('');

document.getElementById('speed-sliders').innerHTML = [
  makeSlider('Speed scale',  0.1, 5,   1, '',  'spd-scale'),
  makeSlider('Leg speed',  0.1, 200,   8, '',  'spd-leg'),
  makeSlider('Body speed', 0.1, 200,   3, '',  'spd-body'),
  makeSlider('Turn speed', 0.1, 200,   4, '',  'spd-turn'),
  makeSlider('Sit speed',  0.1, 200,   1, '',  'spd-sit'),
].join('');

var LEGS   = ['FL (0)','FR (1)','BL (2)','BR (3)'];
var JOINTS = ['alpha','beta','gamma'];
var offsets = [[0,0,0],[0,0,0],[0,0,0],[0,0,0]];
document.getElementById('offset-rows').innerHTML = LEGS.map(function(leg, li) {
  return '<div class="offset-grid">'
    + '<div class="leg-lbl">'+leg+'</div>'
    + JOINTS.map(function(j, ji) {
        return '<input class="offset-input" type="number" value="0"'
          + ' oninput="setOffset('+li+','+ji+',this.value)" id="off-'+li+'-'+ji+'">';
      }).join('')
    + '</div>';
}).join('');

function setOffset(li, ji, val) {
  offsets[li][ji] = Number(val);
  sendCommand('offset', 'off_'+li+'_'+ji, Number(val));
}
function applyOffsets() { sendCommand('offset', 'apply', JSON.stringify(offsets)); }

/* ================================================================
   SAVE & APPLY
   1. Sends every slider value to /setconfig explicitly (belt-and-braces
      in case a value was typed rather than dragged)
   2. Calls /applystand so the robot physically moves to the new stance
      — this is what makes all the calibration changes visible
   3. Re-syncs the control pad speed multiplier display
   ================================================================ */
function saveCalib() {
  var btn = document.getElementById('save-btn');
  var fb  = document.getElementById('save-feedback');
  btn.style.opacity = '0.6';

  /* Build list of all param→value pairs from current slider states */
  var paramMap = {
    'min-pulse':'servoMin','max-pulse':'servoMax',
    'z-default':'z_default','z-up':'z_up','z-boot':'z_boot',
    'x-default':'x_default','y-step':'y_step',
    'spd-scale':'speed_multiple','spd-leg':'leg_move_speed',
    'spd-body':'body_move_speed','spd-turn':'spot_turn_speed','spd-sit':'stand_seat_speed'
  };
  var sends = Object.keys(paramMap).map(function(id) {
    // Read from the range slider, not the number input. The range slider is
    // always clamped to [min, max] by the browser, so an out-of-range value
    // typed directly into the number box cannot reach the firmware on Save.
    // The live-send path (sliderSync) already uses the clamped value, so this
    // makes Save consistent with what was previously sent in real time.
    var rng = document.getElementById(id+'-range');
    if (!rng) return Promise.resolve();
    var v = encodeURIComponent(rng.value);
    var p = encodeURIComponent(paramMap[id]);
    return fetch('/setconfig?param='+p+'&val='+v)
      .then(function(r) { if (!r.ok) throw new Error('HTTP '+r.status+' for '+p); })
      .catch(function(e) { console.warn('saveCalib setconfig failed:', e); throw e; });
  });
  /* Also flush servo offsets */
  for (var l = 0; l < 4; l++) {
    for (var j = 0; j < 3; j++) {
      (function(ll,jj) {
        var el = document.getElementById('off-'+ll+'-'+jj);
        if (!el) return;
        var v = encodeURIComponent(el.value);
        sends.push(fetch('/setconfig?param=off_'+ll+'_'+jj+'&val='+v)
          .then(function(r) { if (!r.ok) throw new Error('HTTP '+r.status+' for off_'+ll+'_'+jj); })
          .catch(function(e) { console.warn('saveCalib offset failed:', e); throw e; }));
      })(l,j);
    }
  }

  Promise.all(sends).then(function() {
    // Persist all values to NVS so they survive a reboot.
    // Only fires on explicit "Save All" click, not on every slider move.
    return fetch('/savecalib')
      .then(function(r) { if (!r.ok) throw new Error('HTTP '+r.status+' on /savecalib'); });
  }).then(function() {
    return fetch('/applystand')
      .then(function(r) { if (!r.ok) throw new Error('HTTP '+r.status+' on /applystand'); });
  }).then(function() {
    btn.style.opacity = '1';
    fb.style.color = 'rgba(0,220,170,1)';
    fb.textContent = 'All settings saved \u2713 Robot is standing to preview';
    setTimeout(function() { fb.style.color = 'rgba(0,220,170,0)'; fb.textContent = 'Settings applied \u2713'; }, 3000);
    /* Sync control pad speed display */
    fetch('/getconfig').then(function(r) { return r.json(); }).then(function(d) {
      if (d.speed_multiple !== undefined) {
        var pct  = Math.round(Math.max(0, Math.min(100, (d.speed_multiple - 0.1) / 4.9 * 100)));
        var rng  = document.getElementById('speed-range');
        var fill = document.getElementById('speed-fill');
        var mult = document.getElementById('speed-mult');
        if (rng)  rng.value = pct;
        if (fill) fill.style.width = pct + '%';
        if (mult) mult.textContent = d.speed_multiple.toFixed(2);
      }
    }).catch(function(){});
  }).catch(function(e) {
    btn.style.opacity = '1';
    fb.style.color = 'rgba(255,100,100,1)';
    var msg = (e && e.message && e.message.indexOf('HTTP') !== -1)
      ? 'Firmware rejected a value \u2014 ' + e.message
      : 'Connection error \u2014 is the bot on?';
    fb.textContent = msg;
    setTimeout(function() { fb.style.color = 'rgba(255,100,100,0)'; fb.textContent = 'Settings applied \u2713'; }, 3000);
  });
}
</script>
</body>
</html>
)MBHTML";
