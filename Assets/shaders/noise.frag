float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

float noise3d(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);

    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
    mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
    mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
    mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}


float fade(float t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float rand2d(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float noise2d(vec2 p) {
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0-2.0*u);

    float res = mix(
    mix(rand2d(ip), rand2d(ip + vec2(1.0, 0.0)), u.x),
    mix(rand2d(ip + vec2(0.0, 1.0)), rand2d(ip + vec2(1.0, 1.0)), u.x),
    u.y
    );
    return res * res;
}

float fbm2d(vec2 p) {
    float f = 0.0;
    f += 0.5000 * noise2d(p); p = p * 2.02;
    f += 0.2500 * noise2d(p); p = p * 2.03;
    f += 0.1250 * noise2d(p); p = p * 2.01;
    f += 0.0625 * noise2d(p);
    return f / 0.9375;
}

float fbm3d(vec3 x) {
    float v = 0.0;
    float a = 0.5;
    float frequency = 1.0;
    float amplitude = 1.0;
    for (int i = 0; i < 5; ++i) {
        v += a * noise3d(x * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
        x *= 2.0;
    }
    return v;
}

