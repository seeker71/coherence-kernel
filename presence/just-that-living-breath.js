(() => {
  const TAU = Math.PI * 2;
  const GOLD = [255, 236, 176];
  const CX = 0.5;
  const CY = 0.455;
  const BREATH0 = 11.5;
  const R_REST = 0.245;
  const R_MAX = 0.39;
  const TUBE = 0.072;
  const TILT = 0.58;
  const YAW = 0.42;

  const SYMBOLS = [
    { name: "lotus", x: 0.155, y: 0.172, rgb: [255, 248, 230] },
    { name: "crescent", x: 0.355, y: 0.078, rgb: [170, 255, 196] },
    { name: "star-david", x: 0.5, y: 0.052, rgb: [255, 220, 120] },
    { name: "tree-of-life", x: 0.735, y: 0.078, rgb: [255, 245, 210] },
    { name: "yin-yang", x: 0.825, y: 0.175, rgb: [232, 232, 240] },
    { name: "heart", x: 0.125, y: 0.305, rgb: [220, 48, 62] },
    { name: "ankh", x: 0.875, y: 0.305, rgb: [120, 214, 188] },
    { name: "om", x: 0.105, y: 0.425, rgb: [196, 86, 255] },
    { name: "torii", x: 0.885, y: 0.525, rgb: [214, 52, 42] },
    { name: "jain", x: 0.885, y: 0.645, rgb: [255, 198, 88] },
    { name: "feather", x: 0.155, y: 0.705, rgb: [214, 218, 224] },
    { name: "atheist", x: 0.345, y: 0.755, rgb: [206, 176, 255] },
    { name: "bahai", x: 0.625, y: 0.755, rgb: [255, 220, 100] },
    { name: "dharma", x: 0.785, y: 0.705, rgb: [255, 158, 72] },
  ];

  const canvas = document.getElementById("field");
  const ctx = canvas.getContext("2d", { alpha: false });
  const hint = document.getElementById("hint");
  const origin = new Image();
  origin.src = "just-that-origin.jpg";

  let W = 0;
  let H = 0;
  let dpr = 1;
  let ox = 0;
  let oy = 0;
  let dw = 0;
  let dh = 0;
  let paused = false;
  let t0 = performance.now();
  let elapsed = 0;
  let period = BREATH0;
  let clickPulse = 0;

  function hash(i) {
    const x = Math.sin(i * 127.1 + 311.7) * 43758.5453;
    return x - Math.floor(x);
  }

  function lerp(a, b, t) {
    return a + (b - a) * t;
  }

  function clamp(x, a, b) {
    return x < a ? a : x > b ? b : x;
  }

  function smooth(a, b, x) {
    const t = clamp((x - a) / (b - a), 0, 1);
    return t * t * (3 - 2 * t);
  }

  function mix3(a, b, t) {
    return [lerp(a[0], b[0], t), lerp(a[1], b[1], t), lerp(a[2], b[2], t)];
  }

  function decodeHomes(b64) {
    const bin = atob(b64);
    const u8 = new Uint8Array(bin.length);
    for (let i = 0; i < bin.length; i++) u8[i] = bin.charCodeAt(i);
    const view = new DataView(u8.buffer);
    const n = view.getUint32(0, true);
    const hx = new Float32Array(n);
    const hy = new Float32Array(n);
    const hr = new Uint8Array(n);
    const hg = new Uint8Array(n);
    const hb = new Uint8Array(n);
    const region = new Uint8Array(n);
    let o = 4;
    for (let i = 0; i < n; i++) {
      hx[i] = view.getUint16(o, true) / 65535;
      hy[i] = view.getUint16(o + 2, true) / 65535;
      hr[i] = u8[o + 4];
      hg[i] = u8[o + 5];
      hb[i] = u8[o + 6];
      region[i] = u8[o + 7];
      o += 8;
    }
    return { n, hx, hy, hr, hg, hb, region };
  }

  const homes = window.JUST_THAT_PARTICLES_B64
    ? decodeHomes(window.JUST_THAT_PARTICLES_B64)
    : { n: 0, hx: new Float32Array(0), hy: new Float32Array(0), hr: new Uint8Array(0), hg: new Uint8Array(0), hb: new Uint8Array(0), region: new Uint8Array(0) };

  const N = homes.n;
  const x = new Float32Array(N);
  const y = new Float32Array(N);
  const seed = new Float32Array(N);
  for (let i = 0; i < N; i++) {
    x[i] = homes.hx[i];
    y[i] = homes.hy[i];
    seed[i] = hash(i + 17);
  }

  function torusPoint(u, v, R, r, spin) {
    const uu = u + spin;
    const c = Math.cos(v);
    const Rrc = R + r * c;
    let x3 = Rrc * Math.cos(uu);
    let y3 = Rrc * Math.sin(uu);
    let z3 = r * Math.sin(v);
    const ct = Math.cos(TILT);
    const st = Math.sin(TILT);
    const y2 = y3 * ct - z3 * st;
    const z2 = y3 * st + z3 * ct;
    const cy = Math.cos(YAW);
    const sy = Math.sin(YAW);
    const x2 = x3 * cy - y2 * sy;
    const y4 = x3 * sy + y2 * cy;
    return { x: x2, y: y4, z: z2 };
  }

  function torusToNorm(p) {
    return {
      x: CX + p.x,
      y: CY + p.y * (dw / Math.max(dh, 1)),
      z: p.z,
    };
  }

  function symbolAngle(si) {
    const s = SYMBOLS[si];
    return Math.atan2(s.y - CY, s.x - CX);
  }

  const TORUS_N = 5600;
  const torusU = new Float32Array(TORUS_N);
  const torusV = new Float32Array(TORUS_N);
  const torusFlow = new Float32Array(TORUS_N);
  for (let i = 0; i < TORUS_N; i++) {
    torusU[i] = hash(i * 3) * TAU;
    torusV[i] = hash(i * 7 + 2) * TAU;
    torusFlow[i] = 0.35 + hash(i * 11) * 0.8;
  }

  const STREAMS = 130 * SYMBOLS.length;
  const streams = [];
  for (let i = 0; i < STREAMS; i++) {
    streams.push({
      si: i % SYMBOLS.length,
      phase: (hash(i * 13 + 4) - 0.5) * 0.06,
      lane: (hash(i * 17) - 0.5) * 0.04,
      v: hash(i * 19) * TAU,
      wob: 0.4 + hash(i * 23) * 1.1,
    });
  }

  function layout() {
    dpr = Math.min(window.devicePixelRatio || 1, 2);
    W = canvas.width = Math.floor(window.innerWidth * dpr);
    H = canvas.height = Math.floor(window.innerHeight * dpr);
    const aspect = 2 / 3;
    if (W / H > aspect) {
      dh = H;
      dw = dh * aspect;
    } else {
      dw = W;
      dh = dw / aspect;
    }
    ox = (W - dw) * 0.5;
    oy = (H - dh) * 0.5;
  }

  function toPx(nx, ny) {
    return [ox + nx * dw, oy + ny * dh];
  }

  function envelopes(u) {
    const inhale = smooth(0.0, 0.34, u) * (1 - smooth(0.34, 0.46, u));
    const still = smooth(0.32, 0.42, u) * (1 - smooth(0.48, 0.58, u));
    const exhale = smooth(0.46, 0.68, u) * (1 - smooth(0.78, 0.94, u));
    const rest = smooth(0.88, 0.99, u);
    return { inhale, still, exhale, rest, u };
  }

  function bezier(ax, ay, bx, by, cx, cy, t) {
    const u = 1 - t;
    return {
      x: u * u * ax + 2 * u * t * bx + t * t * cx,
      y: u * u * ay + 2 * u * t * by + t * t * cy,
    };
  }

  function drawDot(px, py, sz, rgb, a) {
    ctx.fillStyle = `rgba(${rgb[0]|0},${rgb[1]|0},${rgb[2]|0},${a})`;
    ctx.fillRect(px, py, sz, sz);
  }

  function tick(now) {
    if (!paused) elapsed = (now - t0) / 1000;
    clickPulse *= 0.96;

    const u = ((elapsed / period) % 1 + 1) % 1;
    const env = envelopes(u);
    const amp = 1 + clickPulse * 0.7;
    const inhale = env.inhale * amp;
    const still = env.still;
    const exhale = env.exhale * amp;
    const rest = env.rest;
    const expand = (exhale * 0.95 + still * 0.08 + inhale * 0.05) * amp;
    const R = R_REST + (R_MAX - R_REST) * expand;
    const r = TUBE * (1 + 0.08 * Math.sin(elapsed * 0.65) + still * 0.06);
    const spin = elapsed * 0.22 + inhale * 0.4;
    const aspect = dw / Math.max(dh, 1);

    ctx.setTransform(1, 0, 0, 1, 0, 0);
    ctx.globalCompositeOperation = "source-over";
    ctx.fillStyle = "rgba(3, 6, 16, 0.22)";
    ctx.fillRect(0, 0, W, H);
    ctx.fillStyle = "#030610";
    ctx.fillRect(0, 0, ox, H);
    ctx.fillRect(ox + dw, 0, W - ox - dw, H);
    ctx.fillRect(ox, 0, dw, oy);
    ctx.fillRect(ox, oy + dh, dw, H - oy - dh);

    const [cxp, cyp] = toPx(CX, CY);
    const minDim = Math.min(dw, dh);
    if (origin.complete && origin.naturalWidth) {
      ctx.globalAlpha = 0.55 + rest * 0.18 - inhale * 0.08 - exhale * 0.05;
      ctx.drawImage(origin, ox, oy, dw, dh);
      ctx.globalAlpha = 1;
    }

    const g = ctx.createRadialGradient(cxp, cyp, 0, cxp, cyp, minDim * (0.11 + still * 0.04));
    g.addColorStop(0, `rgba(255,236,176,${0.08 + still * 0.1})`);
    g.addColorStop(0.45, `rgba(255,236,176,${0.03 + still * 0.04})`);
    g.addColorStop(1, "rgba(255,236,176,0)");
    ctx.fillStyle = g;
    ctx.beginPath();
    ctx.arc(cxp, cyp, minDim * 0.16, 0, TAU);
    ctx.fill();

    const grain = Math.max(1, dpr * 0.55);

    ctx.globalCompositeOperation = "source-over";
    for (let i = 0; i < N; i += 2) {
      const s = seed[i];
      const hx = homes.hx[i];
      const hy = homes.hy[i];
      const region = homes.region[i];
      if (region === 1) continue;
      const shimmer = 0.0016 * Math.sin(elapsed * 0.7 + s * TAU);
      x[i] = hx + shimmer * (s - 0.5);
      y[i] = hy + shimmer * (hash(i) - 0.5);
      const [px, py] = toPx(x[i], y[i]);
      const a = region === 4 ? 0.08 : region === 3 ? 0.06 : 0.045;
      drawDot(px, py, grain, [homes.hr[i], homes.hg[i], homes.hb[i]], a);
    }

    ctx.globalCompositeOperation = "lighter";

    for (let i = 0; i < TORUS_N; i++) {
      torusU[i] += 0.006 * torusFlow[i] * (0.55 + inhale * 0.6 + exhale * 0.9);
      torusV[i] += 0.011 * torusFlow[i];
      const p = torusToNorm(torusPoint(torusU[i], torusV[i], R, r, spin));
      const depth = (p.z + r) / (2 * r + 1e-5);
      const [px, py] = toPx(p.x, p.y);
      const a = 0.025 + depth * 0.09 + still * 0.02;
      const sz = Math.max(1, dpr * (0.4 + depth * 0.5));
      drawDot(px, py, sz, GOLD, a);
    }

    for (let i = 0; i < streams.length; i++) {
      const st = streams[i];
      const s = SYMBOLS[st.si];
      const local = (u + st.phase + 1) % 1;
      const ang = symbolAngle(st.si) + st.lane * 2.2;
      const entry = torusToNorm(torusPoint(ang - YAW, st.v, R, r * 0.35, spin));
      const exit = torusToNorm(torusPoint(ang - YAW + 0.7 + exhale * 0.4, st.v + 1.2, R, r * 0.2, spin));
      let pxn;
      let pyn;
      let goldK;
      let a;

      if (local < 0.36) {
        const t = smooth(0, 0.36, local);
        const mid = bezier(s.x, s.y, entry.x + st.lane, entry.y, CX, CY, t);
        pxn = mid.x;
        pyn = mid.y;
        goldK = t * 0.35;
        a = 0.16 + t * 0.14 + inhale * 0.1;
      } else if (local < 0.48) {
        const t = (local - 0.36) / 0.12;
        const orbit = t * TAU * 0.35 + st.v;
        pxn = CX + Math.cos(orbit) * 0.018 * (1 - t * 0.3);
        pyn = CY + Math.sin(orbit) * 0.012 * aspect;
        goldK = 1;
        a = 0.2 + still * 0.12;
      } else if (local < 0.66) {
        const t = (local - 0.48) / 0.18;
        const ride = torusToNorm(torusPoint(ang - YAW + t * (1.2 + st.lane * 4), st.v + t * 2, R, r * 0.45, spin));
        pxn = ride.x;
        pyn = ride.y;
        goldK = 1;
        a = 0.15 + exhale * 0.08;
      } else {
        const t = smooth(0.66, 1, local);
        const fed = bezier(exit.x, exit.y, (exit.x + s.x) * 0.5, (exit.y + s.y) * 0.5, s.x, s.y, t);
        pxn = fed.x;
        pyn = fed.y;
        goldK = 1 - t * 0.65;
        a = 0.16 + (1 - t) * 0.12 + exhale * 0.1;
      }

      const wobx = st.lane * Math.sin(elapsed * st.wob + i);
      const woby = st.lane * 0.7 * Math.cos(elapsed * st.wob * 0.8 + i);
      const [px, py] = toPx(pxn + wobx, pyn + woby);
      const col = mix3(s.rgb, GOLD, goldK);
      const sz = Math.max(1, dpr * (0.5 + goldK * 0.4));
      drawDot(px, py, sz, col, a);
    }

    for (let si = 0; si < SYMBOLS.length; si++) {
      const s = SYMBOLS[si];
      const ang = symbolAngle(si);
      const entry = torusToNorm(torusPoint(ang - YAW, si * 0.4, R, r * 0.3, spin));
      const exit = torusToNorm(torusPoint(ang - YAW + 0.85, si * 0.4 + 1.1, R, r * 0.2, spin));
      if (inhale > 0.12) {
        for (let k = 0; k <= 28; k++) {
          const t = k / 28;
          const p = bezier(s.x, s.y, entry.x, entry.y, CX, CY, t);
          const [px, py] = toPx(p.x, p.y);
          const col = mix3(s.rgb, GOLD, t * 0.45);
          drawDot(px, py, Math.max(1, dpr * 0.7), col, 0.08 * inhale * (0.35 + t));
        }
      }
      if (exhale > 0.12) {
        for (let k = 0; k <= 28; k++) {
          const t = k / 28;
          const p = bezier(exit.x, exit.y, (exit.x + s.x) * 0.5, (exit.y + s.y) * 0.5, s.x, s.y, t);
          const [px, py] = toPx(p.x, p.y);
          const col = mix3(GOLD, s.rgb, t * 0.55);
          drawDot(px, py, Math.max(1, dpr * 0.7), col, 0.09 * exhale * (1 - t * 0.4));
        }
        const [sx, sy] = toPx(s.x, s.y);
        const pulse = exhale * 0.12;
        const rg = ctx.createRadialGradient(sx, sy, 0, sx, sy, minDim * 0.05);
        rg.addColorStop(0, `rgba(${GOLD[0]},${GOLD[1]},${GOLD[2]},${pulse})`);
        rg.addColorStop(1, "rgba(255,236,176,0)");
        ctx.fillStyle = rg;
        ctx.beginPath();
        ctx.arc(sx, sy, minDim * 0.05, 0, TAU);
        ctx.fill();
      }
    }

    requestAnimationFrame(tick);
  }

  window.addEventListener("resize", layout);
  window.addEventListener("click", () => {
    clickPulse = 1;
  });
  window.addEventListener("keydown", (e) => {
    if (e.code === "Space") {
      e.preventDefault();
      if (paused) {
        t0 = performance.now() - elapsed * 1000;
        paused = false;
      } else {
        paused = true;
      }
    }
  });

  layout();
  ctx.fillStyle = "#030610";
  ctx.fillRect(0, 0, canvas.width, canvas.height);
  const bootU = parseFloat(new URLSearchParams(location.search).get("u"));
  if (!Number.isNaN(bootU)) t0 = performance.now() - bootU * period * 1000;
  setTimeout(() => hint.classList.add("gone"), 7000);
  requestAnimationFrame(tick);
})();
