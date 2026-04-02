/*
 *
void IMU_Update_MF(Mahony_t *filter, float gx, float gy, float gz, float ax, float ay, float az, float dt) {
    float recipNorm;
    float v[3], e[3];

    // 1. G-Validator (Linear Move Rejection)
    float norm = sqrtf(ax*ax + ay*ay + az*az);

    if (norm > 0.85f && norm < 1.15f) {
        recipNorm = 1.0f / norm;
        ax *= recipNorm; ay *= recipNorm; az *= recipNorm;

        // Odhad gravitace
        v[0] = 2.0f * (filter->q[1] * filter->q[3] - filter->q[0] * filter->q[2]);
        v[1] = 2.0f * (filter->q[0] * filter->q[1] + filter->q[2] * filter->q[3]);
        v[2] = filter->q[0] * filter->q[0] - 0.5f + filter->q[3] * filter->q[3];

        // Chyba (Cross product)
        e[0] = (ay * v[2] - az * v[1]);
        e[1] = (az * v[0] - ax * v[2]);
        e[2] = (ax * v[1] - ay * v[0]);

        // Bias korekce
        filter->integralFB[0] += filter->Ki * e[0] * dt;
        filter->integralFB[1] += filter->Ki * e[1] * dt;
        filter->integralFB[2] += filter->Ki * e[2] * dt;

        gx += filter->Kp * e[0] + filter->integralFB[0];
        gy += filter->Kp * e[1] + filter->integralFB[1];
        gz += filter->Kp * e[2] + filter->integralFB[2];
    }

    // 2. Integrace Quaternionu
    // Tady probíhá fyzické mapování os.
    // Pokud chceš Roll kolem Y, musíš zajistit, aby gy ovlivňovalo roll složku quaternionu.
    float q0 = filter->q[0], q1 = filter->q[1], q2 = filter->q[2], q3 = filter->q[3];

    // Standardní Mahony (q1=X, q2=Y, q3=Z).
    // Pokud tvůj hardware vyžaduje Roll na Y, ujisti se, že vstupy gx/gy/gz
    // odpovídají orientaci čipu na tvém dronu.
    filter->q[0] += (-q1 * gx - q2 * gy - q3 * gz) * (0.5f * dt);
    filter->q[1] += (q0 * gx + q2 * gz - q3 * gy) * (0.5f * dt);
    filter->q[2] += (q0 * gy - q1 * gz + q3 * gx) * (0.5f * dt);
    filter->q[3] += (q0 * gz + q1 * gy - q2 * gx) * (0.5f * dt);

    // Normalizace
    recipNorm = 1.0f / sqrtf(filter->q[0]*filter->q[0] + filter->q[1]*filter->q[1] +
                             filter->q[2]*filter->q[2] + filter->q[3]*filter->q[3]);
    filter->q[0] *= recipNorm; filter->q[1] *= recipNorm;
    filter->q[2] *= recipNorm; filter->q[3] *= recipNorm;

    // 3. Výpočet úhlů -180 až 180
    // Prohození Roll a Pitch ve výstupu:

    // Teď počítáme ROLL pomocí komponenty q2 (osa Y)
    filter->roll = atan2f(2.0f * (filter->q[0] * filter->q[2] + filter->q[3] * filter->q[1]),
                          1.0f - 2.0f * (filter->q[2] * filter->q[2] + filter->q[1] * filter->q[1])) * RAD2DEG;

    // PITCH přepočítaný pro osu X
    float sinp = 2.0f * (filter->q[0] * filter->q[1] - filter->q[2] * filter->q[3]);
    if (fabsf(sinp) >= 1)
        filter->pitch = copysignf(1.570796f, sinp) * RAD2DEG;
    else
        filter->pitch = asinf(sinp) * RAD2DEG;

    filter->yaw = atan2f(2.0f * (filter->q[0] * filter->q[3] + filter->q[1] * filter->q[2]),
                         1.0f - 2.0f * (filter->q[2] * filter->q[2] + filter->q[3] * filter->q[3])) * RAD2DEG;
}

 */

